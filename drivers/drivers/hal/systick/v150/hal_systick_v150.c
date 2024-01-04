/**
 * Copyright (c) @CompanyNameMagicTag 2022-2023. All rights reserved. \n
 *
 * Description: Provides V150 HAL systick \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-08-01, Create file. \n
 */
#include <stdint.h>
#include "hal_systick_v150_regs_op.h"
#include "hal_systick_v150.h"

#define SYSTICK_COUNT_CLEAR_WAIT_TIMEOUT 0xFFFF

void hal_systick_init(void)
{
    hal_systick_v150_regs_init();
}

void hal_systick_deinit(void)
{
    hal_systick_v150_regs_deinit();
}

errcode_t hal_systick_count_clear(void)
{
    uint32_t timeout = 0;

    hal_systick_reg_cfg_set_stz(1);
    while (timeout < SYSTICK_COUNT_CLEAR_WAIT_TIMEOUT) {
        if (hal_systick_reg_cfg_get_clr() == 1) {
            return ERRCODE_SUCC;
        }
        timeout++;
    }
    return ERRCODE_SYSTICK_NOT_CLEARED;
}

uint64_t hal_systick_get_count(void)
{
    return hal_systick_reg_count_get();
}