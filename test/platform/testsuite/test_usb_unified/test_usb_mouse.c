/**
 * Copyright (c) @CompanyNameMagicTag 2022-2023. All rights reserved. \n
 *
 * Description: Test usb mouse source \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-07-09, Create file. \n
 */
#include <los_swtmr.h>
#include "cmsis_os2.h"
#include "securec.h"
#include "sle_low_latency.h"
#include "high_speed_mouse.h"
#include "qdec.h"
#include "pinctrl_porting.h"
#include "pinctrl.h"
#include "gpio.h"
#include "tcxo.h"
#include "test_suite_errors.h"
#include "test_suite_log.h"
#include "arch_barrier.h"
#include "watchdog_porting.h"
#include "watchdog.h"
#include "gpio.h"
#include "gadget/f_hid.h"
#include "usb_porting.h"
#include "test_usb.h"
#include "test_usb_timer.h"
#include "test_usb_mouse.h"

#define SPI_RECV_DATA_LEN           1
#define SPI_SEND_DATA_LEN           2
#define MOUSE_TO_BT_DATA_LEN        5
#define DELAY_MS3                   3
#define MOUSE_DELAY_J_NUM           40
#define MOUSE_DELAY_1US             1
#define MOUSE_DELAY_2US             2
#define MOUSE_DELAY_6US             6
#define TURNOVER_SIGN               (-1)
#define DELAY_US200                 200
#define SEND_MOUSE_MSG_TEST         100
#define TEST_USB_TIMER_MS           2
#define WHEEL_DATA                  4

#define MS_PER_S                    1000
#define USB_HID_MOUSE_SIM_SEND_DELAY_MS  (500UL)
#define USB_HID_MOUSE_SIM_SEND_DELAY_US  (500UL)
#define USB_HID_MOUSE_INIT_DELAY_MS      (500UL)
#define BLE_HID_MOUSE_HIGH_XY_BIT                     12
#define USB_MOUSE_DRAW_QUADRATE_ANGLE    4
#define USB_MOUSE_DRAW_QUADRATE_TIMES    100
#define MOUSE_INPUT_MOVE_LEFT            200
#define MOUSE_INPUT_MOVE_RIGHT           100
#define MOUSE_INPUT_MOVE_UP              200
#define MOUSE_INPUT_MOVE_DOWN            100
#define MOUSE_INPUT_ROLL_FORWARD         200
#define MOUSE_INPUT_ROLL_BACK            100
#define MOUSE_MOVE_STEP                  100
#define MOUSE_ROLL_STEP                  100
#define MOUSE_INPUT_NUM                  6
#define MOUSE_SIM_TIME                   1
#define MOUSE_INPUT_KEY                  0
#define MOUSE_INPUT_X                    1
#define MOUSE_INPUT_Y                    2
#define MOUSE_INPUT_WHEEL                3
#define trans_to_16_bit(num, bit) ((((num) & (1 << ((bit) - 1))) != 0) ? ((num) | (0xFFFF - (1 << (bit)) + 1)) : (num))

#define SIMULATE_TIMES              (380)
#define DELAY_MS                    (1000)
#define DELAY_INPUT                 (20)

#define USB_MOUSE_POLLING_RATE_MAX_BIT   4
#define USB_MOUSE_POLLING_RATE_MAX_VALUE   8
#define MOUSE_KEY_UP_DELAY               10
#define MOUSE_KEY_CLICK_DELAY            200
#define MOUSE_REPORT_ID                  0x2

typedef enum {
    KEY_LEFT,
    KEY_RIGHT,
    KEY_MID
} key_type_t;

/**
 * @brief  USB HID mouse key.
 */
typedef union mouse_key {
    struct {
        uint8_t left_key   : 1;
        uint8_t right_key  : 1;
        uint8_t mid_key    : 1;
    } b;
    uint8_t d8;
} usb_hid_mouse_key_t;

/**
 * @brief  USB HID mouse report massage.
 */
typedef struct usb_hid_mouse_report {
    usb_hid_mouse_key_t key;
    int8_t wheel;              /*!< A negative value indicates that the wheel roll forward. */
    int16_t x;                 /*!< A negative value indicates that the mouse moves left. */
    int16_t y;                 /*!< A negative value indicates that the mouse moves up. */
} usb_hid_mouse_report_t;

high_mouse_oprator_t g_usb_hid_hs_mouse_operator;

static usb_hid_mouse_report_t g_send_sim_mouse_msg[] = {
    {{{ 0, 0, 0 }}, -1, 0, 0},
    {{{ 0, 0, 0 }}, 0, -1, 0},
    {{{ 0, 0, 0 }}, 1, 0, 0},
    {{{ 0, 0, 0 }}, 0, 1, 0},
};

#pragma pack (1)
typedef struct {
    int32_t button_mask : 8;
    int32_t x : 12; /* mouse x */
    int32_t y : 12; /* mouse y */
    int8_t wheel;
} low_latency_mouse_t;
#pragma pack ()

#ifdef CONFIG_FEATURE_GLE_LOW_LATENCY
errcode_t sle_low_latency_dongle_get_em_data(uint8_t* em_data);
#endif
static uint32_t g_mouse_sim_send_times = 0;
static usb_hid_mouse_report_t g_send_mouse_msg;
static qdec_config_t g_usb_qdec_config = QDEC_DEFAULT_CONFIG;
static int g_mouse_index = 0;
static uint8_t g_usb_mouse_polling_rate = 1;
bool g_is_dongle = false;

static void mouse_left_button_func(pin_t pin);
static void mouse_right_button_func(pin_t pin);
static void mouse_mid_button_func(pin_t pin);

typedef bool(*mouse_key_set_callback)(int8_t *button_mask, int16_t *x, int16_t *y, int8_t *wheel);

void low_latency_report_callback_reg(low_latency_report_callback report_cbk);
void usb_ble_high_mouse_report(uint8_t *data, uint8_t lenth);
void mouse_key_value_set_callback_reg(mouse_key_set_callback mouse_cbk);
static void dongle_cb(uint8_t **data, uint16_t *length, uint8_t *device_index);

static void test_mouse_io_init(void)
{
    uapi_pin_set_mode(PIN_LEFT, (pin_mode_t)HAL_PIO_FUNC_GPIO);
    uapi_pin_set_mode(PIN_RIGHT, (pin_mode_t)HAL_PIO_FUNC_GPIO);
    uapi_pin_set_mode(PIN_MID, (pin_mode_t)HAL_PIO_FUNC_GPIO);
    uapi_pin_set_mode(S_MGPIO12, (pin_mode_t)HAL_PIO_FUNC_GPIO);
    uapi_pin_set_mode(S_MGPIO10, (pin_mode_t)HAL_PIO_FUNC_GPIO);
    gpio_select_core(PIN_LEFT, CORES_APPS_CORE);
    gpio_select_core(PIN_RIGHT, CORES_APPS_CORE);
    gpio_select_core(S_MGPIO12, CORES_APPS_CORE);
    gpio_select_core(S_MGPIO10, CORES_APPS_CORE);
    gpio_select_core(PIN_MID, CORES_APPS_CORE);
    uapi_gpio_set_dir(PIN_LEFT, GPIO_DIRECTION_INPUT);
    uapi_gpio_set_dir(S_MGPIO12, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(S_MGPIO12, 1);
    uapi_gpio_set_dir(S_MGPIO10, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(S_MGPIO10, 1);
    uapi_gpio_set_dir(PIN_RIGHT, GPIO_DIRECTION_INPUT);
    uapi_gpio_set_dir(PIN_MID, GPIO_DIRECTION_INPUT);
    uapi_gpio_register_isr_func(PIN_LEFT, GPIO_INTERRUPT_DEDGE, (gpio_callback_t)mouse_left_button_func);
    uapi_gpio_register_isr_func(PIN_RIGHT, GPIO_INTERRUPT_DEDGE, (gpio_callback_t)mouse_right_button_func);
    uapi_gpio_register_isr_func(PIN_MID, GPIO_INTERRUPT_DEDGE, (gpio_callback_t)mouse_mid_button_func);
}

static void test_high_speed_mouse_init(void)
{
    test_mouse_io_init();
}

static void mouse_left_button_func(pin_t pin)
{
    uapi_tcxo_delay_us(DELAY_US200);
    g_send_mouse_msg.key.b.left_key = !uapi_gpio_get_val(pin);
    UNUSED(pin);
}

static void mouse_right_button_func(pin_t pin)
{
    uapi_tcxo_delay_us(DELAY_US200);
    g_send_mouse_msg.key.b.right_key = !uapi_gpio_get_val(pin);
    UNUSED(pin);
}

static void mouse_mid_button_func(pin_t pin)
{
    uapi_tcxo_delay_us(DELAY_US200);
    g_send_mouse_msg.key.b.mid_key = !uapi_gpio_get_val(pin);
    UNUSED(pin);
}

static int qdec_usb_report_callback(int argc, char *argv[])
{
    UNUSED(argv);
    g_send_mouse_msg.wheel = -argc;
    return 0;
}

int tesetsuit_usb_remote_wakeup(int argc, char *argv[])
{
    UNUSED(argc);
    UNUSED(argv);
    g_send_mouse_msg.x = SEND_MOUSE_MSG_TEST;
    g_send_mouse_msg.y = SEND_MOUSE_MSG_TEST;
    g_send_mouse_msg.wheel = 0;
    fhid_send_data(g_mouse_index, (char *)&g_send_mouse_msg, sizeof(usb_hid_mouse_report_t));
    return 0;
}

static void test_usb_mouse_draw_quadrate(void)
{
    for (int i = 0; i < USB_MOUSE_DRAW_QUADRATE_ANGLE; i++) {
        for (int j = 0; j < USB_MOUSE_DRAW_QUADRATE_TIMES; j++) {
            fhid_send_data(g_mouse_index, (char *)&g_send_sim_mouse_msg[i], sizeof(usb_hid_mouse_report_t));
            uapi_tcxo_delay_ms(DELAY_INPUT);
        }
    }

    g_mouse_sim_send_times++;
}

int tesetsuit_usb_mouse_simulator(int argc, char *argv[])
{
    uint32_t times;

    if (!tesetsuit_usb_get_hid_is_inited()) {
        if (test_usb_init_internal(DEV_HID, 0xa) != TEST_SUITE_OK) {
            return TEST_SUITE_TEST_FAILED;
        }
        g_mouse_index = tesetsuit_usb_get_hid_index();
        osDelay(USB_HID_MOUSE_INIT_DELAY_MS);
    }

    if (argc == 0) {
        times = 1;
    } else {
        times = (uint32_t)strtol(argv[0], NULL, 0);
    }

    for (uint32_t i = 0; i < times; i++) {
        osDelay(USB_HID_MOUSE_INIT_DELAY_MS);
        test_usb_mouse_draw_quadrate();
        uapi_watchdog_kick();
    }
    return 0;
}

void tesetsuit_usb_mouse_key_click(key_type_t key, uint8_t times)
{
    usb_hid_mouse_report_t msg = { 0 };
    for (uint8_t i = 0; i < times; i++) {
        msg.key.d8 = 1 << key;
        fhid_send_data(g_mouse_index, (char *)&msg, sizeof(usb_hid_mouse_report_t));
        uapi_tcxo_delay_ms(MOUSE_KEY_UP_DELAY);  // key up delay
        msg.key.d8 = 0;
        fhid_send_data(g_mouse_index, (char *)&msg, sizeof(usb_hid_mouse_report_t));
        uapi_tcxo_delay_ms(MOUSE_KEY_CLICK_DELAY); // key click delay
    }
}

int tesetsuit_usb_mouse_input(int argc, char *argv[])
{
    if (argc != MOUSE_INPUT_NUM) {
        return TEST_SUITE_ERROR_BAD_PARAMS;
    }
    if (!tesetsuit_usb_get_hid_is_inited()) {
        if (test_usb_init_internal(DEV_HID, 0xa) != TEST_SUITE_OK) {
            return TEST_SUITE_TEST_FAILED;
        }
        g_mouse_index = tesetsuit_usb_get_hid_index();
        osDelay(USB_HID_MOUSE_INIT_DELAY_MS);
    }

    usb_hid_mouse_report_t msg = { 0 };
    uint8_t param_index = 0;
    uint8_t left_key_count = (uint8_t)strtol(argv[param_index++], NULL, 0);
    uint8_t right_key_count = (uint8_t)strtol(argv[param_index++], NULL, 0);
    uint8_t mid_key_count = (uint8_t)strtol(argv[param_index++], NULL, 0);
    if (left_key_count > 1) {
        tesetsuit_usb_mouse_key_click(KEY_LEFT, left_key_count);
        return 0;
    }
    if (right_key_count > 1) {
        tesetsuit_usb_mouse_key_click(KEY_RIGHT, right_key_count);
        return 0;
    }
    if (mid_key_count > 1) {
        tesetsuit_usb_mouse_key_click(KEY_MID, mid_key_count);
        return 0;
    }
    msg.key.b.left_key = left_key_count;
    msg.key.b.right_key = right_key_count;
    msg.key.b.mid_key = mid_key_count;
    msg.wheel = (uint32_t)strtol(argv[param_index++], NULL, 0);
    msg.x = (uint32_t)strtol(argv[param_index++], NULL, 0);
    msg.y = (uint32_t)strtol(argv[param_index++], NULL, 0);

    fhid_send_data(g_mouse_index, (char *)&msg, sizeof(usb_hid_mouse_report_t));
    return 0;
}

int tesetsuit_usb_mouse_get_times(int argc, char *argv[])
{
    UNUSED(argc);
    UNUSED(argv);
    print("usb mouse send data times:%d\r\n", g_mouse_sim_send_times);
    return 0;
}

int tesetsuit_usb_dongle_mouse(int argc, char *argv[])
{
    if (!tesetsuit_usb_get_hid_is_inited()) {
        if (test_usb_init_internal(DEV_HID, 0xa) != TEST_SUITE_OK) {
            return TEST_SUITE_TEST_FAILED;
        }
        g_mouse_index = tesetsuit_usb_get_hid_index();
        osDelay(USB_HID_MOUSE_INIT_DELAY_MS);
    }
    bool is_lowlatency_dongle = false;
    if (argc >= 1) {
        is_lowlatency_dongle = (uint8_t)strtol(argv[0], NULL, 0);
    }

    if (is_lowlatency_dongle) {
        usb_register_callback(dongle_cb);
        return 0;
    }
#ifdef CONFIG_FEATURE_GLE_LOW_LATENCY
    sle_low_latency_dongle_callbacks_t dongle_cbk = {NULL};
    dongle_cbk.report_cb = (low_latency_report_callback)usb_ble_high_mouse_report;
    sle_low_latency_dongle_register_callbacks(&dongle_cbk);
#endif
    return 0;
}

void usb_ble_high_mouse_report(uint8_t *data, uint8_t lenth)
{
    if (lenth > sizeof(g_send_mouse_msg)) {
        return;
    }
    g_send_mouse_msg.key.d8 = data[0];
    int16_t temp_x = data[1] | ((data[2] & 0xf) << 8);
    int16_t temp_y = (data[3] << 4) | ((data[2] & 0xf0) >> 4);
    g_send_mouse_msg.x = trans_to_16_bit((uint16_t)temp_x, BLE_HID_MOUSE_HIGH_XY_BIT);
    g_send_mouse_msg.y = trans_to_16_bit((uint16_t)temp_y, BLE_HID_MOUSE_HIGH_XY_BIT);
    g_send_mouse_msg.wheel = (int8_t)data[WHEEL_DATA];
    fhid_send_data(g_mouse_index, (char *)&g_send_mouse_msg, sizeof(usb_hid_mouse_report_t));
}

static void mouse_sensor_cb(uint8_t **data, uint16_t *length, uint8_t *device_index)
{
    static uint8_t usb_sof_cnt = 0;
    usb_sof_cnt = (usb_sof_cnt + 1) % g_usb_mouse_polling_rate;
    if (usb_sof_cnt != 0) {
        return;
    }
    int16_t x = 0;
    int16_t y = 0;
    static usb_hid_mouse_report_t mouse_message = { 0 }; // must be static or global variabal
    g_usb_hid_hs_mouse_operator.get_xy(&x, &y);
    mouse_message.x = x;
    mouse_message.y = y;
    mouse_message.wheel = g_send_mouse_msg.wheel;
    mouse_message.key.d8 = g_send_mouse_msg.key.d8;
    g_send_mouse_msg.wheel = 0;
    *data = (uint8_t *)&mouse_message;
    *length = sizeof(usb_hid_mouse_report_t);
    *device_index = g_mouse_index;
}

static void dongle_cb(uint8_t **data, uint16_t *length, uint8_t *device_index)
{
    static uint8_t usb_sof_cnt = 0;
    usb_sof_cnt = (usb_sof_cnt + 1) % g_usb_mouse_polling_rate;
    if (usb_sof_cnt != 0) {
        return;
    }

    static usb_hid_mouse_report_t mouse_message = { 0 }; // must be static or global variabal
    low_latency_mouse_t key_base = { 0 };
#ifdef CONFIG_FEATURE_GLE_LOW_LATENCY
    if (sle_low_latency_dongle_get_em_data((uint8_t *)&key_base) != 0) {
        return;
    }
#endif
    mouse_message.key.d8 = key_base.button_mask;
    mouse_message.x = key_base.x;
    mouse_message.y = key_base.y;
    mouse_message.wheel = key_base.wheel;
    *data = (uint8_t *)&mouse_message;
    *length = sizeof(usb_hid_mouse_report_t);
    *device_index = g_mouse_index;
}

errcode_t mouse_key_set(int8_t *button_mask, int16_t *x, int16_t *y, int8_t *wheel)
{
    g_usb_hid_hs_mouse_operator.get_xy(x, y);
    *button_mask = g_send_mouse_msg.key.d8;
    *wheel = g_send_mouse_msg.wheel;
    g_send_mouse_msg.wheel = 0;
    return ERRCODE_SUCC;
}

static mouse_freq_type_t mouse_init(uint32_t sensor_id)
{
    test_high_speed_mouse_init();
    uapi_qdec_init(&g_usb_qdec_config);
    qdec_port_pinmux_init(QDEC_A, QDEC_B);
    uapi_qdec_register_callback(qdec_usb_report_callback);
    uapi_qdec_enable();
    print("sensor:%d\r\n", sensor_id);
    g_usb_hid_hs_mouse_operator = get_high_mouse_operator(sensor_id);
    mouse_freq_type_t freq = g_usb_hid_hs_mouse_operator.init();
    print("mouse inited\r\n");
    return freq;
}

static void mouse_gpio_callback_func(pin_t pin, uintptr_t param)
{
    unused(pin);
    unused(param);
    static uint8_t change_index = 0;
    change_index = (change_index + 1) % USB_MOUSE_POLLING_RATE_MAX_BIT;
    g_usb_mouse_polling_rate = bit(change_index);
    print("speed: %dK\r\n", USB_MOUSE_POLLING_RATE_MAX_VALUE / g_usb_mouse_polling_rate);
}

static bool mouse_gpio_register(int argc, char *argv[])
{
    pin_t pin;
    uint32_t trigger = 0;
    if (argc != 4) { /* 4: Indicates the number of input parameters */
        pin = S_MGPIO12;
        trigger = (uint32_t)GPIO_INTERRUPT_FALLING_EDGE;
    } else {
        uint8_t index = 1;
        index++;
        pin = (pin_t)strtol(argv[index++], NULL, 0);
        trigger = (uint32_t)strtol(argv[index], NULL, 0);
    }

    uapi_gpio_init();
    uapi_pin_set_mode(pin, HAL_PIO_FUNC_GPIO);
    uapi_gpio_set_dir(pin, GPIO_DIRECTION_INPUT);
    if (uapi_gpio_register_isr_func(pin, trigger, mouse_gpio_callback_func) != ERRCODE_SUCC) {
        print("PIN: %d int reg fail. \r\n", pin);
        uapi_gpio_unregister_isr_func(pin);
        return false;
    }

    return true;
}

int mouse_init_usb(int argc, char *argv[])
{
    if (!tesetsuit_usb_get_hid_is_inited()) {
        if (test_usb_init_internal(DEV_HID, 0xa) != TEST_SUITE_OK) {
            return TEST_SUITE_TEST_FAILED;
        }
        g_mouse_index = tesetsuit_usb_get_hid_index();
        osDelay(USB_HID_MOUSE_INIT_DELAY_MS);
    }

    mouse_sensor_t sensor = (mouse_sensor_t)strtol(argv[0], NULL, 0); // sensor id
    if ((sensor >= MOUSE_SENSOR_MAX_NUM) || (argc < 1)) {
        return TEST_SUITE_ERROR_BAD_PARAMS;
    }
    mouse_freq_type_t freq = mouse_init(sensor);
    osDelay(USB_HID_MOUSE_INIT_DELAY_MS);

    if (argc > 1 && ((bool)strtol(argv[1], NULL, 0))) {
        if (!mouse_gpio_register(argc, argv)) {
            return -1;
        }
        print("io registed\r\n");
    }

    print("mouse init done %d\r\n", freq);
    usb_register_callback(mouse_sensor_cb);

    return 0;
}

int mouse_init_bt(int argc, char *argv[])
{
    mouse_sensor_t sensor = (mouse_sensor_t)strtol(argv[0], NULL, 0); // sensor id
    if ((sensor >= MOUSE_SENSOR_MAX_NUM) || (argc != 1)) {
        return TEST_SUITE_ERROR_BAD_PARAMS;
    }

    mouse_init(sensor);
#ifdef CONFIG_FEATURE_GLE_LOW_LATENCY
    sle_low_latency_mouse_callbacks_t mouse_cbk;
    mouse_cbk.set_value_cb = mouse_key_set;
    sle_low_latency_mouse_register_callbacks(&mouse_cbk);
#endif
    return 0;
}
