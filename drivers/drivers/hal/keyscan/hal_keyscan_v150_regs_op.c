/**
 * Copyright (c) @CompanyNameMagicTag 2022-2023. All rights reserved. \n
 *
 * Description: Provides v150 keyscan register operation api \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-09-16， Create file. \n
 */

#include <stdio.h>
#include "keyscan_porting.h"
#include "hal_keyscan_v150_regs_op.h"

keyscan_regs_t *g_keyscan_regs = NULL;
static keyscan_m_ctl_regs_t *g_keyscan_m_ctl_regs = NULL;

#if defined (CONFIG_KEYSCAN_USE_LP)
static keyscan_key_int_regs_t *g_keyscan_key_int_regs = NULL;
#endif

void hal_keyscan_v150_regs_init(void)
{
    g_keyscan_regs = (keyscan_regs_t *)keyscan_porting_base_addr_get();
}

void hal_keyscan_v150_regs_deinit(void)
{
    g_keyscan_regs = NULL;
}

void hal_keyscan_soft_rst(void)
{
    clk_key_h_div_data_t clk_key_h_div;
    clk_key_l_div_data_t clk_key_l_div;
    keyscan_soft_rst_data_t keyscan_soft_rst;
    g_keyscan_m_ctl_regs = (keyscan_m_ctl_regs_t *)keyscan_m_ctl_porting_base_addr_get();
    clk_key_h_div.d32 = g_keyscan_m_ctl_regs->clk_key_h_div;
    clk_key_h_div.b.clk_key_h_div_num = CLK_HIGH_DIV;
    clk_key_h_div.b.clk_key_hen = 1;
    clk_key_h_div.b.clk_key_h_div_en = 1;
    clk_key_h_div.b.clk_key_h_div_load_en = 1;
    g_keyscan_m_ctl_regs->clk_key_h_div = clk_key_h_div.d32;
    clk_key_l_div.d32 = g_keyscan_m_ctl_regs->clk_key_l_div;
    clk_key_l_div.b.clk_key_l_div_num = CLK_LOW_DIV;
    clk_key_l_div.b.clk_key_l_div_en = 1;
    clk_key_l_div.b.clk_key_l_div_load_en = 1;
    g_keyscan_m_ctl_regs->clk_key_l_div = clk_key_l_div.d32;
    keyscan_soft_rst.d32 = g_keyscan_m_ctl_regs->keyscan_soft_rst;
    keyscan_soft_rst.b.soft_rst_key_n = 1;
    keyscan_soft_rst.b.soft_rst_key_dp_n = 1;
    g_keyscan_m_ctl_regs->keyscan_soft_rst = keyscan_soft_rst.d32;
}

#if defined (CONFIG_KEYSCAN_USE_LP)
void hal_keyscan_regs_set_key_int_lp_en(bool en)
{
    key_int_lp_int_en_data_t key_int_lp_int_en;
    g_keyscan_key_int_regs = (keyscan_key_int_regs_t *)keyscan_key_int_porting_base_addr_get();
    key_int_lp_int_en.d32 = g_keyscan_key_int_regs->key_int_lp_int_en;
    key_int_lp_int_en.b.key_int_lp_int_en = en;
    g_keyscan_key_int_regs->key_int_lp_int_en = key_int_lp_int_en.d32;
}

void hal_keyscan_regs_set_clk_key_h_en(bool en)
{
    clk_key_h_div_data_t clk_key_h_div;
    clk_key_h_div.d32 = g_keyscan_m_ctl_regs->clk_key_h_div;
    clk_key_h_div.b.clk_key_hen = en;
    g_keyscan_m_ctl_regs->clk_key_h_div = clk_key_h_div.d32;
}
#endif

#if defined (CONFIG_KEYSCAN_SUPPORT_SLEEP)
void hal_keyscan_regs_set_slp_req(bool en)
{
    keyscan_lp_ctl_data_t keyscan_lp_ctl;
    keyscan_lp_ctl.d32 = g_keyscan_regs->keyscan_lp_ctl;
    keyscan_lp_ctl.b.slp_req = en;
    g_keyscan_regs->keyscan_lp_ctl = keyscan_lp_ctl.d32;
}

uint32_t hal_keyscan_regs_get_slp_step_sta(uint32_t slp_step)
{
    keyscan_lp_ctl_data_t keyscan_lp_ctl;
    keyscan_lp_ctl.d32 = g_keyscan_regs->keyscan_lp_ctl;
    g_keyscan_regs->keyscan_lp_ctl = keyscan_lp_ctl.d32;
    return (keyscan_lp_ctl.d32 >> slp_step) & 1U;
}
#endif

void hal_keyscan_regs_set_clk_keep(bool en)
{
    aon_clk_cfg_data_t aon_clk_cfg;
    aon_clk_cfg.b.aon_en = 0;
    aon_clk_cfg.b.aon_sel = en;
    aon_clk_cfg.b.aon_en = 1;
    g_keyscan_m_ctl_regs->aon_clk_cfg = aon_clk_cfg.d32;
}