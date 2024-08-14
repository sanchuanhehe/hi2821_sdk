/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides pinctrl port \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-08-25， Create file. \n
 */
#include "pinctrl_porting.h"
#include "hal_pinctrl.h"
#include "boards.h"
#include "hal_pinctrl_bs21.h"
#include "platform_types.h"

const hal_pio_config_t g_pio_function_config[PIN_MAX_NUMBER] = {
    { HAL_PIO_FUNC_GPIO, HAL_PIO_DRIVE_MAX, HAL_PIO_PULL_DOWN, PIN_IE_0 }, // !< S_MGPIO0
    { HAL_PIO_FUNC_GPIO, HAL_PIO_DRIVE_MAX, HAL_PIO_PULL_DOWN, PIN_IE_0 }, // !< S_MGPIO1
    { HAL_PIO_FUNC_GPIO, HAL_PIO_DRIVE_MAX, HAL_PIO_PULL_DOWN, PIN_IE_0 }, // !< S_MGPIO2
    { HAL_PIO_FUNC_GPIO, HAL_PIO_DRIVE_MAX, HAL_PIO_PULL_DOWN, PIN_IE_0 }, // !< S_MGPIO3
    { HAL_PIO_FUNC_GPIO, HAL_PIO_DRIVE_MAX, HAL_PIO_PULL_DOWN, PIN_IE_0 }, // !< S_MGPIO4
    { HAL_PIO_FUNC_GPIO, HAL_PIO_DRIVE_MAX, HAL_PIO_PULL_DOWN, PIN_IE_0 }, // !< S_MGPIO5
    { HAL_PIO_FUNC_GPIO, HAL_PIO_DRIVE_MAX, HAL_PIO_PULL_DOWN, PIN_IE_0 }, // !< S_MGPIO6
    { HAL_PIO_FUNC_GPIO, HAL_PIO_DRIVE_MAX, HAL_PIO_PULL_DOWN, PIN_IE_0 }, // !< S_MGPIO7
    { HAL_PIO_FUNC_GPIO, HAL_PIO_DRIVE_MAX, HAL_PIO_PULL_DOWN, PIN_IE_0 }, // !< S_MGPIO8
    { HAL_PIO_FUNC_GPIO, HAL_PIO_DRIVE_MAX, HAL_PIO_PULL_DOWN, PIN_IE_0 }, // !< S_MGPIO9
    { HAL_PIO_FUNC_GPIO, HAL_PIO_DRIVE_MAX, HAL_PIO_PULL_DOWN, PIN_IE_0 }, // !< S_MGPIO10
    { HAL_PIO_FUNC_GPIO, HAL_PIO_DRIVE_MAX, HAL_PIO_PULL_DOWN, PIN_IE_0 }, // !< S_MGPIO11
    { HAL_PIO_FUNC_GPIO, HAL_PIO_DRIVE_MAX, HAL_PIO_PULL_DOWN, PIN_IE_0 }, // !< S_MGPIO12
    { HAL_PIO_FUNC_GPIO, HAL_PIO_DRIVE_MAX, HAL_PIO_PULL_DOWN, PIN_IE_0 }, // !< S_MGPIO13
    { HAL_PIO_FUNC_GPIO, HAL_PIO_DRIVE_MAX, HAL_PIO_PULL_DOWN, PIN_IE_0 }, // !< S_MGPIO14
    { HAL_PIO_FUNC_GPIO, HAL_PIO_DRIVE_MAX, HAL_PIO_PULL_DOWN, PIN_IE_0 }, // !< S_MGPIO15
    { HAL_PIO_FUNC_GPIO, HAL_PIO_DRIVE_MAX, HAL_PIO_PULL_DOWN, PIN_IE_0 }, // !< S_MGPIO16
    { HAL_PIO_UART_H0_TXD, HAL_PIO_DRIVE_MAX, HAL_PIO_PULL_NONE, PIN_IE_0 }, // !< S_MGPIO17
    { HAL_PIO_UART_H0_RXD, HAL_PIO_DRIVE_MAX, HAL_PIO_PULL_MAX, PIN_IE_1 }, // !< S_MGPIO18
    { HAL_PIO_UART_L0_TXD, HAL_PIO_DRIVE_MAX, HAL_PIO_PULL_NONE, PIN_IE_0 }, // !< S_MGPIO19
    { HAL_PIO_UART_L0_RXD, HAL_PIO_DRIVE_MAX, HAL_PIO_PULL_MAX, PIN_IE_1 }, // !< S_MGPIO20
    { HAL_PIO_FUNC_GPIO, HAL_PIO_DRIVE_MAX, HAL_PIO_PULL_DOWN, PIN_IE_0 }, // !< S_MGPIO21
    { HAL_PIO_FUNC_GPIO, HAL_PIO_DRIVE_MAX, HAL_PIO_PULL_DOWN, PIN_IE_0 }, // !< S_MGPIO22
    { HAL_PIO_FUNC_GPIO, HAL_PIO_DRIVE_MAX, HAL_PIO_PULL_DOWN, PIN_IE_0 }, // !< S_MGPIO23
    { HAL_PIO_FUNC_GPIO, HAL_PIO_DRIVE_MAX, HAL_PIO_PULL_DOWN, PIN_IE_0 }, // !< S_MGPIO24
    { HAL_PIO_FUNC_GPIO, HAL_PIO_DRIVE_MAX, HAL_PIO_PULL_DOWN, PIN_IE_0 }, // !< S_MGPIO25
    { HAL_PIO_FUNC_GPIO, HAL_PIO_DRIVE_MAX, HAL_PIO_PULL_DOWN, PIN_IE_0 }, // !< S_MGPIO26
    { HAL_PIO_FUNC_GPIO, HAL_PIO_DRIVE_MAX, HAL_PIO_PULL_DOWN, PIN_IE_0 }, // !< S_MGPIO27
    { HAL_PIO_FUNC_GPIO, HAL_PIO_DRIVE_MAX, HAL_PIO_PULL_DOWN, PIN_IE_0 }, // !< S_MGPIO28
    { HAL_PIO_FUNC_GPIO, HAL_PIO_DRIVE_MAX, HAL_PIO_PULL_DOWN, PIN_IE_0 }, // !< S_MGPIO29
    { HAL_PIO_FUNC_GPIO, HAL_PIO_DRIVE_MAX, HAL_PIO_PULL_DOWN, PIN_IE_0 }, // !< S_MGPIO30
    { HAL_PIO_FUNC_GPIO, HAL_PIO_DRIVE_MAX, HAL_PIO_PULL_DOWN, PIN_IE_0 }, // !< S_MGPIO31
};

void get_pio_func_config(size_t *pin_num, hal_pio_config_t **pin_func_array)
{
    if (pin_num == NULL || pin_func_array == NULL) { return; }
    *pin_num = sizeof(g_pio_function_config) / sizeof(g_pio_function_config[0]);
    *pin_func_array = (hal_pio_config_t *)g_pio_function_config;
}

bool pin_check_mode_is_valid(pin_t pin, pin_mode_t mode)
{
    UNUSED(pin);
    if (mode > PIN_MODE_MAX) {
        return false;
    }
    return true;
}

void pin_port_register_hal_funcs(void)
{
    hal_pin_register_funcs(hal_pin_bs21_funcs_get());
}

void pin_port_unregister_hal_funcs(void)
{
    hal_pin_unregister_funcs();
}
