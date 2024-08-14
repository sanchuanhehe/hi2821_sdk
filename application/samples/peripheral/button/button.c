/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Button Sample Source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-04-03, Create file. \n
 */
#include "boards.h"
#include "pinctrl.h"
#include "gpio.h"
#include "test_suite_log.h"
#include "cmsis_os2.h"
#include "app_init.h"

static void gpio_callback_func(pin_t pin, uintptr_t param)
{
    UNUSED(pin);
    UNUSED(param);
    test_suite_log_stringf("Button pressed.\r\n");
}

static void button_entry(void)
{
    uapi_pin_set_mode(BUTTON_0, HAL_PIO_FUNC_GPIO);

    gpio_select_core(BUTTON_0, CORES_APPS_CORE);

    uapi_gpio_set_dir(BUTTON_0, GPIO_DIRECTION_INPUT);
    uapi_gpio_register_isr_func(BUTTON_0, GPIO_INTERRUPT_DEDGE, gpio_callback_func);
}

/* Run the button_entry. */
app_run(button_entry);