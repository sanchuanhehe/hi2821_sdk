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
#include "mouse_button.h"

#define PIN_LEFT        S_MGPIO2
#define PIN_RIGHT       S_MGPIO3
#define PIN_MID         S_MGPIO4
#define DELAY_US200     200

static mouse_key_t *g_mouse_key = NULL;

static void mouse_left_button_func(pin_t pin)
{
    osal_printk("Left button clicked.\r\n");
    uapi_tcxo_delay_us(DELAY_US200);
    if (g_mouse_key != NULL) {
        g_mouse_key->b.left_key = !uapi_gpio_get_val(pin);
    }
}

static void mouse_right_button_func(pin_t pin)
{
    osal_printk("Right button clicked.\r\n");
    uapi_tcxo_delay_us(DELAY_US200);
    if (g_mouse_key != NULL) {
        g_mouse_key->b.right_key = !uapi_gpio_get_val(pin);
    }
}

static void mouse_mid_button_func(pin_t pin)
{
    osal_printk("MID button clicked.\r\n");
    uapi_tcxo_delay_us(DELAY_US200);
    if (g_mouse_key != NULL) {
        g_mouse_key->b.mid_key = !uapi_gpio_get_val(pin);
    }
}

void mouse_button_init(mouse_key_t *mouse_key)
{
    g_mouse_key = mouse_key;

    uapi_gpio_init();

    uapi_pin_set_mode(PIN_LEFT, (pin_mode_t)HAL_PIO_FUNC_GPIO);
    uapi_pin_set_mode(PIN_RIGHT, (pin_mode_t)HAL_PIO_FUNC_GPIO);
    uapi_pin_set_mode(PIN_MID, (pin_mode_t)HAL_PIO_FUNC_GPIO);

    gpio_select_core(PIN_LEFT, CORES_APPS_CORE);
    gpio_select_core(PIN_RIGHT, CORES_APPS_CORE);
    gpio_select_core(PIN_MID, CORES_APPS_CORE);

    uapi_gpio_set_dir(PIN_LEFT, GPIO_DIRECTION_INPUT);
    uapi_gpio_set_dir(PIN_RIGHT, GPIO_DIRECTION_INPUT);
    uapi_gpio_set_dir(PIN_MID, GPIO_DIRECTION_INPUT);

    uapi_gpio_register_isr_func(PIN_LEFT, GPIO_INTERRUPT_DEDGE, (gpio_callback_t)mouse_left_button_func);
    uapi_gpio_register_isr_func(PIN_RIGHT, GPIO_INTERRUPT_DEDGE, (gpio_callback_t)mouse_right_button_func);
    uapi_gpio_register_isr_func(PIN_MID, GPIO_INTERRUPT_DEDGE, (gpio_callback_t)mouse_mid_button_func);
}