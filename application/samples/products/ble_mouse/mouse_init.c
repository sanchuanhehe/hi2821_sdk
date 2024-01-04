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
#include "osal_interrupt.h"
#include "interrupt.h"
#include "securec.h"
#include "qdec.h"
#include "pinctrl_porting.h"
#include "pinctrl.h"
#include "gpio.h"
#include "tcxo.h"
#include "arch_barrier.h"
#include "watchdog_porting.h"
#include "watchdog.h"
#include "gpio.h"
#include "gadget/f_hid.h"
#include "ble_mouse_server/ble_hid_mouse_server.h"
#include "ble_mouse_server/ble_mouse_server.h"
#include "bts_le_gap.h"
#if defined(CONFIG_PM_SYS_SUPPORT)
#include "ulp_gpio.h"
#include "pm_sys.h"
#include "app_init.h"
#endif
#include "mouse_sensor/mouse_sensor.h"
#include "mouse_button/mouse_button.h"
#include "mouse_init.h"

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

#define DURATION_MS_OF_WORK2STANDBY    60000
#define DURATION_MS_OF_STANDBY2SLEEP   120000

typedef struct usb_hid_mouse_report {
    uint8_t kind;
    mouse_key_t key;
    int8_t x;
    int8_t y;
    int8_t wheel;
} usb_hid_mouse_report_t;

static mouse_sensor_oprator_t g_usb_hid_hs_mouse_operator = { 0 };
static qdec_config_t g_usb_qdec_config = QDEC_DEFAULT_CONFIG;
ble_hid_high_mouse_event_st g_send_msg = { 0 };

static int qdec_usb_report_callback(int argc, char *argv[])
{
    UNUSED(argv);
    g_send_msg.wheel += argc;
    if (get_g_connection_state() == GAP_BLE_STATE_CONNECTED) {
        ble_hid_mouse_server_send_input_report_by_uuid((const ble_hid_high_mouse_event_st *)&g_send_msg);
        g_send_msg.wheel = 0;
    }
    osal_irq_clear(QDEC_IRQN);
    return 0;
}

void mouse_io_init(void)
{
    uapi_pin_set_mode(CONFIG_MOUSE_PIN_QDEC_COMMON, HAL_PIO_FUNC_GPIO);
    uapi_gpio_set_dir(CONFIG_MOUSE_PIN_QDEC_COMMON, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(CONFIG_MOUSE_PIN_QDEC_COMMON, 0);
}

void ble_mouse_init(void)
{
    uapi_pin_set_pull(CONFIG_MOUSE_PIN_NRESET, PIN_PULL_DOWN);
    osal_printk("enter mouse init\r\n");
    mouse_io_init();
    uapi_qdec_init(&g_usb_qdec_config);
    qdec_port_pinmux_init(CONFIG_MOUSE_PIN_QDEC_A, CONFIG_MOUSE_PIN_QDEC_B);
    uapi_qdec_register_callback(qdec_usb_report_callback);
    mouse_button_init();
    uapi_qdec_enable();
    g_usb_hid_hs_mouse_operator = get_mouse_sensor_operator();
    osal_printk("g_usb_hid_hs_mouse_operator:%x\r\n", g_usb_hid_hs_mouse_operator.init);
    osal_printk("g_usb_hid_hs_mouse_operator:%x\r\n", g_usb_hid_hs_mouse_operator.get_xy);
    mouse_freq_t freq = g_usb_hid_hs_mouse_operator.init();
    osal_printk("g_usb_hid_hs_mouse_operator:%d\r\n", freq);
    osal_printk("mouse init done\r\n");
}

#if defined(CONFIG_PM_SYS_SUPPORT)
static void ulp_gpio_wkup_handler(uint8_t ulp_gpio)
{
    uapi_pm_wkup_process(0);
    osal_printk("ulp_gpio%d wakeup\n", ulp_gpio);
}

static ulp_gpio_int_wkup_cfg_t g_wk_cfg[] = {
    { 1, CONFIG_MOUSE_PIN_QDEC_B, true, ULP_GPIO_INTERRUPT_FALLING_EDGE, ulp_gpio_wkup_handler },    // qdec唤醒
    { 2, CONFIG_MOUSE_PIN_LEFT, true, ULP_GPIO_INTERRUPT_FALLING_EDGE, ulp_gpio_wkup_handler },      // 左键唤醒
    { 3, CONFIG_MOUSE_PIN_RIGHT, true, ULP_GPIO_INTERRUPT_FALLING_EDGE, ulp_gpio_wkup_handler },     // 右键唤醒
    { 4, CONFIG_MOUSE_PIN_MID, true, ULP_GPIO_INTERRUPT_FALLING_EDGE, ulp_gpio_wkup_handler },       // 中键唤醒
    { 5, CONFIG_MOUSE_PIN_MONTION, true, ULP_GPIO_INTERRUPT_FALLING_EDGE, ulp_gpio_wkup_handler },   // Sensor唤醒
};

static void mouse_enable_ulpgpio_wkup(void)
{
    uapi_gpio_deinit();
    ulp_gpio_init();
    ulp_gpio_int_wkup_config(g_wk_cfg, sizeof(g_wk_cfg) / sizeof(ulp_gpio_int_wkup_cfg_t));
}

static void mouse_disable_ulpgpio_wkup(void)
{
    ulp_gpio_deinit();
    uapi_gpio_init();
}

static void ble_mouse_deinit(void)
{
    uapi_qdec_disable();
    uapi_qdec_deinit();
    mouse_enable_ulpgpio_wkup();
}

static int32_t mouse_state_work_to_standby(uintptr_t arg)
{
    unused(arg);
    ble_mouse_deinit();
    mouse_enable_ulpgpio_wkup();
    // 连接态：增大连接间隔；广播态：暂不操作。
    ble_mouse_work2standby();
    return 0;
}

static int32_t mouse_state_standby_to_sleep(uintptr_t arg)
{
    unused(arg);
    // 断开连接、关闭广播
    ble_mouse_standby2sleep();
    return 0;
}

static int32_t mouse_state_standby_to_work(uintptr_t arg)
{
    unused(arg);
    mouse_disable_ulpgpio_wkup();
    ble_mouse_init();
    // 连接态：减小连接间隔；广播态：不操作
    ble_mouse_standby2work();
    return 0;
}

static int32_t mouse_state_sleep_to_work(uintptr_t arg)
{
    unused(arg);
    mouse_disable_ulpgpio_wkup();
    ble_mouse_init();
    // 打开广播、蓝牙回连
    ble_mouse_sleep2work();
    return 0;
}

void mouse_low_power_entry(void)
{
    pm_state_trans_handler_t handler = {
        .work_to_standby = mouse_state_work_to_standby,
        .standby_to_sleep = mouse_state_standby_to_sleep,
        .standby_to_work = mouse_state_standby_to_work,
        .sleep_to_work = mouse_state_sleep_to_work,
    };
    uapi_pm_state_trans_handler_register(&handler);

    uapi_pm_work_state_reset();
    mouse_disable_ulpgpio_wkup();
    uapi_pm_set_state_trans_duration(DURATION_MS_OF_WORK2STANDBY, DURATION_MS_OF_STANDBY2SLEEP);
}

app_run(mouse_low_power_entry);
#endif