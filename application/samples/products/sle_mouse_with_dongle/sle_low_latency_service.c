/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Sle Low Latency Mouse Source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-08-01, Create file. \n
 */
#include <los_swtmr.h>
#include "soc_osal.h"
#include "osal_debug.h"
#include "securec.h"
#include "sle_low_latency.h"
#include "qdec.h"
#include "pinctrl_porting.h"
#include "pinctrl.h"
#include "gpio.h"
#include "tcxo.h"
#include "arch_barrier.h"
#include "watchdog_porting.h"
#include "watchdog.h"
#include "gpio.h"
#include "adc.h"
#include "adc_porting.h"
#include "timer.h"
#include "chip_core_irq.h"
#include "gadget/f_hid.h"
#include "mouse_usb/usb_init_app.h"
#include "mouse_sensor/mouse_sensor.h"
#include "mouse_button/mouse_button.h"
#include "sle_ssap_server.h"
#include "sle_connection_manager.h"
#include "sle_errcode.h"
#include "sle_mouse_server/sle_mouse_server.h"
#include "errcode.h"
#include "usb_porting.h"
#include "sle_low_latency_service.h"

#define GAFE_SAMPLE_VALUE_SIGN_BIT      17
#define VBAT_SAMPLE_INTERVAL_US         30000000
#define ADC_REFERENCE_VOLTAGE_MV        1500
#define ADC_REF_VOL_DIFFERENCE_MULT     2
#define ADC_TICK2VOL_REF_VOLTAGE_MV     (ADC_REFERENCE_VOLTAGE_MV * ADC_REF_VOL_DIFFERENCE_MULT)
#define SPI_RECV_DATA_LEN 1
#define SPI_SEND_DATA_LEN 2
#define MOUSE_TO_BT_DATA_LEN 5
#define DELAY_MS3 3
#define MOUSE_DELAY_J_NUM 40
#define MOUSE_DELAY_1US 1
#define MOUSE_DELAY_2US 2
#define MOUSE_DELAY_6US 6
#define TURNOVER_SIGN (-1)
#define DELAY_US200 200
#define SEND_MOUSE_MSG_TEST 100
#define TEST_USB_TIMER_MS 2
#define WHEEL_DATA 4
#define USB_MOUSE_REPORTER_LEN 5
#define MS_PER_S 1000
#define USB_HID_MOUSE_SIM_SEND_DELAY_MS (500UL)
#define USB_HID_MOUSE_SIM_SEND_DELAY_US (500UL)
#define USB_HID_MOUSE_INIT_DELAY_MS (500UL)
#define BLE_HID_MOUSE_HIGH_XY_BIT 12
#define USB_MOUSE_DRAW_QUADRATE_ANGLE 4
#define USB_MOUSE_DRAW_QUADRATE_TIMES 100
#define MOUSE_INPUT_MOVE_LEFT 200
#define MOUSE_INPUT_MOVE_RIGHT 100
#define MOUSE_INPUT_MOVE_UP 200
#define MOUSE_INPUT_MOVE_DOWN 100
#define MOUSE_INPUT_ROLL_FORWARD 200
#define MOUSE_INPUT_ROLL_BACK 100
#define MOUSE_MOVE_STEP 100
#define MOUSE_ROLL_STEP 100
#define MOUSE_INPUT_NUM 6
#define MOUSE_SIM_TIME 1
#define MOUSE_INPUT_KEY 0
#define MOUSE_INPUT_X 1
#define MOUSE_INPUT_Y 2
#define MOUSE_INPUT_WHEEL 3
#define SIMULATE_TIMES (380)
#define DELAY_MS (1000)
#define DELAY_INPUT (20)
#define USB_MOUSE_POLLING_RATE_MAX_BIT 4
#define USB_MOUSE_POLLING_RATE_MAX_VALUE 8
#define MOUSE_KEY_UP_DELAY 10
#define MOUSE_KEY_CLICK_DELAY 200
#define DATA_BIT2 2
#define DATA_BIT3 3
#define DATA_BIT4 4
#define DATA_BIT8 8
#define MOUSE_KIND 0x2
#define SLE_MOUSE_TASK_DELAY_300_MS 300
#define SLE_MOUSE_TASK_DELAY_1700_MS 1700
#ifdef CONFIG_SAMPLE_SLE_DONGLE_1K
#define REPORT_TIME  7
#elif defined(CONFIG_SAMPLE_SLE_DONGLE_2K)
#define REPORT_TIME  3
#elif defined(CONFIG_SAMPLE_SLE_DONGLE_4K)
#define REPORT_TIME  1
#elif defined(CONFIG_SAMPLE_SLE_DONGLE_8K)
#define REPORT_TIME  0
#else
#define REPORT_TIME  7
#endif
uint8_t g_report_time = REPORT_TIME;

typedef struct usb_hid_mouse_report {
    uint8_t kind;
    mouse_key_t key;
    int8_t x;
    int8_t y;
    int8_t wheel;
} usb_hid_mouse_report_t;

ssap_mouse_key_t g_mouse_notify_data = {0};
#pragma pack (1)
typedef struct {
    int32_t button_mask : 8;
    int32_t x : 12; /* mouse x */
    int32_t y : 12; /* mouse y */
    int8_t wheel;
} low_latency_mouse_t;
#pragma pack ()

static mouse_sensor_oprator_t g_usb_hid_hs_mouse_operator = { 0 };
static usb_hid_mouse_report_t g_send_mouse_msg = { 0 };
static qdec_config_t g_usb_qdec_config = QDEC_DEFAULT_CONFIG;
static int g_usb_mouse_hid_index;
extern errcode_t sle_low_latency_dongle_get_em_data(uint8_t* em_data);

static bool sle_send_msg(void)
{
    uint8_t conn_state = SLE_ACB_STATE_NONE;
    uint32_t pair_status = ERRCODE_SLE_FAIL;
    bool ssap_able = false;
    get_g_sle_mouse_server_conn_state(&conn_state);
    get_g_sle_mouse_pair_state(&pair_status);
    if (conn_state != SLE_ACB_STATE_CONNECTED || pair_status != ERRCODE_SLE_SUCCESS) {
        return false;
    }
    get_g_read_ssap_support(&ssap_able);
    if (ssap_able == true) {
        sle_hid_mouse_server_send_input_report(&g_mouse_notify_data);
        osal_msleep(SLE_MOUSE_TASK_DELAY_20_MS);
    }
    return true;
}

static void mouse_left_button_func(pin_t pin)
{
    uapi_tcxo_delay_us(DELAY_US200);
    g_send_mouse_msg.key.b.left_key = !uapi_gpio_get_val(pin);
    g_mouse_notify_data.button_mask = g_send_mouse_msg.key.d8;
    sle_send_msg();
    uapi_gpio_clear_interrupt(pin);
}

static void mouse_right_button_func(pin_t pin)
{
    uapi_tcxo_delay_us(DELAY_US200);
    g_send_mouse_msg.key.b.right_key = !uapi_gpio_get_val(pin);
    g_mouse_notify_data.button_mask = g_send_mouse_msg.key.d8;
    sle_send_msg();
    uapi_gpio_clear_interrupt(pin);
}

static void mouse_mid_button_func(pin_t pin)
{
    uapi_tcxo_delay_us(DELAY_US200);
    g_send_mouse_msg.key.b.mid_key = !uapi_gpio_get_val(pin);
    g_mouse_notify_data.button_mask = g_send_mouse_msg.key.d8;
    sle_send_msg();
    uapi_gpio_clear_interrupt(pin);
}

static int qdec_report_callback(int argc, char *argv[])
{
    UNUSED(argv);
    g_send_mouse_msg.wheel = -argc;
    g_mouse_notify_data.wheel += g_send_mouse_msg.wheel;
    if (sle_send_msg()) {
        g_mouse_notify_data.wheel = 0;
    }
    osal_irq_clear(QDEC_IRQN);
    return 0;
}

static void mouse_io_init(void)
{
    uapi_pin_set_mode(CONFIG_MOUSE_PIN_LEFT, (pin_mode_t)HAL_PIO_FUNC_GPIO);
    uapi_pin_set_mode(CONFIG_MOUSE_PIN_RIGHT, (pin_mode_t)HAL_PIO_FUNC_GPIO);
    uapi_pin_set_mode(CONFIG_MOUSE_PIN_MID, (pin_mode_t)HAL_PIO_FUNC_GPIO);
    gpio_select_core(CONFIG_MOUSE_PIN_LEFT, CORES_APPS_CORE);
    gpio_select_core(CONFIG_MOUSE_PIN_RIGHT, CORES_APPS_CORE);
    gpio_select_core(CONFIG_MOUSE_PIN_MID, CORES_APPS_CORE);
    uapi_gpio_set_dir(CONFIG_MOUSE_PIN_LEFT, GPIO_DIRECTION_INPUT);
    uapi_gpio_set_dir(CONFIG_MOUSE_PIN_RIGHT, GPIO_DIRECTION_INPUT);
    uapi_gpio_set_dir(CONFIG_MOUSE_PIN_MID, GPIO_DIRECTION_INPUT);
    uapi_gpio_register_isr_func(CONFIG_MOUSE_PIN_LEFT, GPIO_INTERRUPT_DEDGE, (gpio_callback_t)mouse_left_button_func);
    uapi_gpio_register_isr_func(CONFIG_MOUSE_PIN_RIGHT, GPIO_INTERRUPT_DEDGE, (gpio_callback_t)mouse_right_button_func);
    uapi_gpio_register_isr_func(CONFIG_MOUSE_PIN_MID, GPIO_INTERRUPT_DEDGE, (gpio_callback_t)mouse_mid_button_func);

    uapi_pin_set_mode(CONFIG_MOUSE_PIN_QDEC_COMMON, (pin_mode_t)HAL_PIO_FUNC_GPIO);
    gpio_select_core(CONFIG_MOUSE_PIN_QDEC_COMMON, CORES_APPS_CORE);
    uapi_gpio_set_dir(CONFIG_MOUSE_PIN_QDEC_COMMON, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(CONFIG_MOUSE_PIN_QDEC_COMMON, 0);
}

static void vbat_sample_cb(uintptr_t data)
{
    int adc_value = 0;
    adc_value =  uapi_adc_auto_sample(CONFIG_MOUSE_ADC_VBAT_CH);
    osal_printk("VBAT: %dmv\n", (adc_value * ADC_TICK2VOL_REF_VOLTAGE_MV) >> GAFE_SAMPLE_VALUE_SIGN_BIT);
    osal_irq_clear(TIMER_0_IRQN);
    uapi_timer_start((timer_handle_t)data, VBAT_SAMPLE_INTERVAL_US, vbat_sample_cb, data);
}

void vbat_adc_init(void)
{
    uapi_pin_set_mode(CONFIG_MOUSE_ADC_VBAT_PIN, PIN_MODE_0);
    uapi_gpio_set_dir(CONFIG_MOUSE_ADC_VBAT_PIN, GPIO_DIRECTION_INPUT);
    uapi_pin_set_pull(CONFIG_MOUSE_ADC_VBAT_PIN, PIN_PULL_NONE);
#if defined(CONFIG_PINCTRL_SUPPORT_IE)
    uapi_pin_set_ie(CONFIG_MOUSE_ADC_VBAT_PIN, PIN_IE_1);
#endif
    uapi_adc_init(ADC_CLOCK_NONE);
    uapi_adc_power_en(AFE_GADC_MODE, true);
    uapi_adc_open_channel(CONFIG_MOUSE_ADC_VBAT_CH);
    adc_calibration(AFE_GADC_MODE, true, true, true);
    static timer_handle_t timer = 0;
    uapi_timer_create(DEFAULT_TIMER, &timer);
    uapi_timer_start(timer, VBAT_SAMPLE_INTERVAL_US, vbat_sample_cb, (uintptr_t)timer);
}

mouse_freq_t mouse_init(uint32_t sensor_id)
{
    mouse_io_init();
    vbat_adc_init();
    uapi_qdec_init(&g_usb_qdec_config);
    qdec_port_pinmux_init(CONFIG_MOUSE_PIN_QDEC_A, CONFIG_MOUSE_PIN_QDEC_B);
    uapi_qdec_register_callback(qdec_report_callback);
    uapi_qdec_enable();
    osal_printk("sensor:%d\r\n", sensor_id);
    g_usb_hid_hs_mouse_operator = get_mouse_sensor_operator(sensor_id);
    mouse_freq_t freq = g_usb_hid_hs_mouse_operator.init();
    osal_printk("g_usb_hid_hs_mouse_operator set frequency :%d\r\n", freq);
    osal_printk("mouse init done\r\n");
    return freq;
}

static errcode_t sle_mouse_key_set(int8_t *button_mask, int16_t *x, int16_t *y, int8_t *wheel)
{
    g_usb_hid_hs_mouse_operator.get_xy(x, y);
    *button_mask = g_send_mouse_msg.key.d8;
    *wheel = g_send_mouse_msg.wheel;
    g_send_mouse_msg.wheel = 0;
    return ERRCODE_SUCC;
}

void sle_mouse_get_key(void)
{
    int8_t button_mask = 0;
    int16_t x = 0;
    int16_t y = 0;
    int8_t wheel = 0;
    sle_mouse_key_set(&button_mask, &x, &y, &wheel);
    g_mouse_notify_data.button_mask = button_mask;
    g_mouse_notify_data.x = x;
    g_mouse_notify_data.y = y;
    g_mouse_notify_data.wheel = wheel;
    sle_hid_mouse_server_send_input_report(&g_mouse_notify_data);
}

void sle_low_latency_mouse_app_init(void)
{
    sle_low_latency_mouse_callbacks_t mouse_cbk;
    mouse_cbk.set_value_cb = sle_mouse_key_set;
    sle_low_latency_mouse_register_callbacks(&mouse_cbk);
    return ;
}

void dongle_cbk(uint8_t **data, uint16_t *length, uint8_t *device_index)
{
    static uint8_t report_count = 0;
    if (report_count < g_report_time) {
        report_count++;
        return;
    }
    report_count = 0;
    static usb_hid_mouse_report_t mouse_message = { 0 }; // must be static or global variabal
    low_latency_mouse_t key_base = { 0 };
    if (sle_low_latency_dongle_get_em_data((uint8_t *)&key_base) != 0) {
        return;
    }
    mouse_message.key.d8 = key_base.button_mask;
    mouse_message.x = key_base.x;
    mouse_message.y = key_base.y;
    mouse_message.wheel = key_base.wheel;
    mouse_message.kind = MOUSE_KIND;

    *data = (uint8_t *)&mouse_message;
    *length = sizeof(usb_hid_mouse_report_t);
    *device_index = g_usb_mouse_hid_index;
}

void usb_sle_high_mouse_report(uint8_t *data, uint8_t lenth)
{
    if (lenth > sizeof(g_send_mouse_msg)) {
        osal_printk("Invalide data\r\n");
        return;
    }
    g_send_mouse_msg.key.d8 = data[0];
    int16_t temp_x = data[1] | ((data[DATA_BIT2] & 0xf) << DATA_BIT8);
    int16_t temp_y = (data[DATA_BIT3] << DATA_BIT4) | ((data[DATA_BIT2] & 0xf0) >> DATA_BIT4);
    g_send_mouse_msg.x = trans_to_16_bit((uint16_t)temp_x, BLE_HID_MOUSE_HIGH_XY_BIT);
    g_send_mouse_msg.y = trans_to_16_bit((uint16_t)temp_y, BLE_HID_MOUSE_HIGH_XY_BIT);
    g_send_mouse_msg.wheel = (int8_t)data[WHEEL_DATA];
    g_send_mouse_msg.kind = MOUSE_KIND;
    fhid_send_data(g_usb_mouse_hid_index, (char *)&g_send_mouse_msg, USB_MOUSE_REPORTER_LEN);
}

void sle_low_latency_dongle_init(int usb_hid_index)
{
    g_usb_mouse_hid_index = usb_hid_index;
    usb_register_callback(&dongle_cbk);
}
