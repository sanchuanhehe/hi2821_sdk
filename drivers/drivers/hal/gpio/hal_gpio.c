/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides HAL gpio \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-07-26, Create file. \n
 */

#include <stdint.h>
#include "hal_gpio.h"

uintptr_t g_gpios_regs[GPIO_CHANNEL_MAX_NUM] = { 0 };
hal_gpio_funcs_t *g_hal_gpios_funcs = { 0 };

uintptr_t hal_gpio_base_addrs_get(uint32_t i)
{
    return gpio_porting_base_addr_get((gpio_channel_t)i);
}

void hal_gpio_regs_init(void)
{
    for (uint32_t i = 0; i < GPIO_CHANNEL_MAX_NUM; i++) {
        g_gpios_regs[i] = hal_gpio_base_addrs_get(i);
    }
}

void hal_gpio_regs_deinit(void)
{
    for (uint32_t i = 0; i < GPIO_CHANNEL_MAX_NUM; i++) {
        g_gpios_regs[i] = 0;
    }
}

errcode_t hal_gpio_register_funcs(hal_gpio_funcs_t *funcs)
{
    if (funcs == NULL) {
        return ERRCODE_INVALID_PARAM;
    }
    g_hal_gpios_funcs = funcs;
    return ERRCODE_SUCC;
}

errcode_t hal_gpio_unregister_funcs(void)
{
    g_hal_gpios_funcs = NULL;
    return ERRCODE_SUCC;
}

hal_gpio_funcs_t *hal_gpio_get_funcs(void)
{
    return g_hal_gpios_funcs;
}