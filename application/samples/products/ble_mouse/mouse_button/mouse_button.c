/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Mouse Button source \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-08-01, Create file. \n
 */
#include "osal_debug.h"
#include "gpio.h"
#include "pinctrl.h"
#include "tcxo.h"
#include "bts_le_gap.h"
#include "../ble_mouse_server/ble_hid_mouse_server.h"
#include "../ble_mouse_server/ble_mouse_server.h"
#if defined(CONFIG_PM_SYS_SUPPORT)
#include "pm_sys.h"
#endif
#include "mouse_button.h"

#define PIN_LEFT        CONFIG_MOUSE_PIN_LEFT
#define PIN_RIGHT       CONFIG_MOUSE_PIN_RIGHT
#define PIN_MID         CONFIG_MOUSE_PIN_MID
#define DELAY_US200     200

static mouse_key_t g_mouse_key = { 0 };
extern ble_hid_high_mouse_event_st g_send_msg;

void mouse_left_button_func(pin_t pin)
{
#if defined(CONFIG_PM_SYS_SUPPORT)
    uapi_pm_work_state_reset();
#endif
    osal_printk("Left button clicked.\r\n");
    uapi_tcxo_delay_us(DELAY_US200);
    g_mouse_key.b.left_key = !uapi_gpio_get_val(pin);
    g_send_msg.button_mask = g_mouse_key.d8;
    if (get_g_connection_state() == GAP_BLE_STATE_CONNECTED) {
        ble_hid_mouse_server_send_input_report_by_uuid((const ble_hid_high_mouse_event_st *)&g_send_msg);
    }
    uapi_gpio_clear_interrupt(pin);
}

void mouse_right_button_func(pin_t pin)
{
#if defined(CONFIG_PM_SYS_SUPPORT)
    uapi_pm_work_state_reset();
#endif
    osal_printk("Right button clicked.\r\n");
    uapi_tcxo_delay_us(DELAY_US200);
    g_mouse_key.b.right_key = !uapi_gpio_get_val(pin);
    g_send_msg.button_mask = g_mouse_key.d8;
    if (get_g_connection_state() == GAP_BLE_STATE_CONNECTED) {
        ble_hid_mouse_server_send_input_report_by_uuid((const ble_hid_high_mouse_event_st *)&g_send_msg);
    }
    uapi_gpio_clear_interrupt(pin);
}

void mouse_mid_button_func(pin_t pin)
{
#if defined(CONFIG_PM_SYS_SUPPORT)
    uapi_pm_work_state_reset();
#endif
    osal_printk("MID button clicked.\r\n");
    uapi_tcxo_delay_us(DELAY_US200);
    g_mouse_key.b.mid_key = !uapi_gpio_get_val(pin);
    g_send_msg.button_mask = g_mouse_key.d8;
    if (get_g_connection_state() == GAP_BLE_STATE_CONNECTED) {
        ble_hid_mouse_server_send_input_report_by_uuid((const ble_hid_high_mouse_event_st *)&g_send_msg);
    }
    uapi_gpio_clear_interrupt(pin);
}

void mouse_button_init(void)
{
    uapi_pin_set_mode(CONFIG_MOUSE_PIN_LEFT, (pin_mode_t)HAL_PIO_FUNC_GPIO);
    uapi_pin_set_mode(CONFIG_MOUSE_PIN_RIGHT, (pin_mode_t)HAL_PIO_FUNC_GPIO);
    uapi_pin_set_mode(CONFIG_MOUSE_PIN_MID, (pin_mode_t)HAL_PIO_FUNC_GPIO);

    uapi_pin_set_pull(CONFIG_MOUSE_PIN_LEFT, PIN_PULL_UP);
    uapi_pin_set_pull(CONFIG_MOUSE_PIN_RIGHT, PIN_PULL_UP);
    uapi_pin_set_pull(CONFIG_MOUSE_PIN_MID, PIN_PULL_UP);

    gpio_select_core(CONFIG_MOUSE_PIN_LEFT, CORES_APPS_CORE);
    gpio_select_core(CONFIG_MOUSE_PIN_RIGHT, CORES_APPS_CORE);
    gpio_select_core(CONFIG_MOUSE_PIN_MID, CORES_APPS_CORE);

    uapi_gpio_set_dir(CONFIG_MOUSE_PIN_LEFT, GPIO_DIRECTION_INPUT);
    uapi_gpio_set_dir(CONFIG_MOUSE_PIN_RIGHT, GPIO_DIRECTION_INPUT);
    uapi_gpio_set_dir(CONFIG_MOUSE_PIN_MID, GPIO_DIRECTION_INPUT);

    uapi_gpio_register_isr_func(CONFIG_MOUSE_PIN_LEFT, GPIO_INTERRUPT_DEDGE, (gpio_callback_t)mouse_left_button_func);
    uapi_gpio_register_isr_func(CONFIG_MOUSE_PIN_RIGHT, GPIO_INTERRUPT_DEDGE, (gpio_callback_t)mouse_right_button_func);
    uapi_gpio_register_isr_func(CONFIG_MOUSE_PIN_MID, GPIO_INTERRUPT_DEDGE, (gpio_callback_t)mouse_mid_button_func);
}