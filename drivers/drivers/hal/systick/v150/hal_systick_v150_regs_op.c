/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides V150 systick register operation api \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-08-01, Create file. \n
 */
#include <stdint.h>
#include "common_def.h"
#include "hal_systick_v150_regs_op.h"

systick_regs_t *g_systick_regs = NULL;

void hal_systick_v150_regs_init(void)
{
    g_systick_regs = (systick_regs_t *)systick_porting_base_addr_get();
}

void hal_systick_v150_regs_deinit(void)
{
    g_systick_regs = NULL;
}

uint64_t hal_systick_reg_count_get(void)
{
    uint64_t count;

    cfg_data_t cfg;
    cfg.d32 = g_systick_regs->cfg;
    cfg.b.frz = 1;
    g_systick_regs->cfg = cfg.d32;

    value_data_t value;
    value.d32 = g_systick_regs->value3;
    count = (uint64_t)value.b.data << 48U;
    value.d32 = g_systick_regs->value2;
    count |= (uint64_t)value.b.data << 32U;
    value.d32 = g_systick_regs->value1;
    count |= (uint64_t)value.b.data << 16U;
    value.d32 = g_systick_regs->value0;
    count |= (uint64_t)value.b.data;

    cfg.b.frz = 0;
    g_systick_regs->cfg = cfg.d32;

    return count;
}