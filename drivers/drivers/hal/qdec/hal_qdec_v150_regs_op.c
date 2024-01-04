/**
 * Copyright (c) @CompanyNameMagicTag 2022-2023. All rights reserved. \n
 *
 * Description: Provides V150 qdec register operation api \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-09-16, Create file. \n
 */
#include "common_def.h"
#include "tcxo.h"
#include "qdec_porting.h"
#include "hal_qdec_v150_regs_op.h"

qdec_regs_t *g_qdec_regs = NULL;
qdec_regs_clk_div_t *g_qdec_regs_clk_div = NULL;

errcode_t hal_qdec_v150_regs_init(void)
{
    if ((qdec_porting_base_addr_get() == 0) || (qdec_clk_div_porting_base_addr_get() == 0)) {
        return ERRCODE_QDEC_REG_ADDR_INVALID;
    }

    g_qdec_regs = (qdec_regs_t *)qdec_porting_base_addr_get();
    g_qdec_regs_clk_div = (qdec_regs_clk_div_t *)qdec_clk_div_porting_base_addr_get();

    return ERRCODE_SUCC;
}

void hal_qdec_v150_regs_deinit(void)
{
    g_qdec_regs = NULL;
    g_qdec_regs_clk_div = NULL;
}

void hal_qdec_v150_regs_soft_rst(void)
{
    hal_qdec_v150_regs_set_clk_div_num();
    hal_qdec_v150_regs_set_clk_div_en();
    hal_qdec_v150_regs_clr_clk_ctl_sel();
    hal_qdec_v150_regs_set_soft_rst();
    uapi_tcxo_delay_ms(1);
    hal_qdec_v150_regs_set_clk_ctl_en();
}