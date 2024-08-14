/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Provides V150 HAL PDM \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-02-09, Create file. \n
 */
#include <stdbool.h>
#include "securec.h"
#include "errcode.h"
#include "common_def.h"
#include "pdm_porting.h"
#include "hal_pdm_v150_regs_op.h"
#include "hal_pdm_v150.h"

static hal_pdm_config_t g_hal_pdm_config[HAL_PDM_MIC_MAX_NUM] = { 0 };

static errcode_t hal_pdm_v150_init(void)
{
    pdm_port_clock_enable(true);
    return hal_pdm_regs_init();
}

static void hal_pdm_v150_deinit(void)
{
    hal_pdm_regs_deinit();
    pdm_port_clock_enable(false);
}

static errcode_t hal_pdm_v150_set_attr(uint8_t mic, hal_pdm_config_t *attr)
{
    hal_pdm_mic_t mic_tmp = (hal_pdm_mic_t)mic;
    if (mic_tmp == HAL_PDM_AMIC || mic_tmp == HAL_PDM_DMIC_0) {
        hal_pdm_v150_cic_ctrl_set_cic_gain_0(CONFIG_PDM_CIC_GAIN);
        hal_pdm_v150_cic_ctrl_set_cic_en_0(1);
        hal_pdm_v150_srcdn_ctrl_set_srcdn_mode_0((uint32_t)attr->srcdn_src_mode);

        hal_pdm_v150_clk_rst_en_set_srcdn_clken_0(1);
        hal_pdm_v150_clk_rst_en_set_hpf_clken_0(1);
        hal_pdm_v150_clk_rst_en_set_compd_clken_0(1);
        hal_pdm_v150_clk_rst_en_set_cic_clken_0(1);
        hal_pdm_v150_clk_rst_en_set_func_up_ch_en_0(1);
    } else if (mic_tmp == HAL_PDM_DMIC_1) {
        hal_pdm_v150_cic_ctrl_set_cic_gain_1(CONFIG_PDM_CIC_GAIN);
        hal_pdm_v150_cic_ctrl_set_cic_en_1(1);
        hal_pdm_v150_srcdn_ctrl_set_srcdn_mode_1((uint32_t)attr->srcdn_src_mode);

        hal_pdm_v150_clk_rst_en_set_srcdn_clken_1(1);
        hal_pdm_v150_clk_rst_en_set_hpf_clken_1(1);
        hal_pdm_v150_clk_rst_en_set_compd_clken_1(1);
        hal_pdm_v150_clk_rst_en_set_cic_clken_1(1);
        hal_pdm_v150_clk_rst_en_set_func_up_ch_en_1(1);
    } else {
        return ERRCODE_INVALID_PARAM;
    }

    hal_pdm_v150_clk_rst_en_set_up_fifo_clken(1);
    hal_pdm_v150_clk_rst_en_set_ckdiv_6144k_clken(1);
    hal_pdm_v150_clk_rst_en_set_saradc_clken(1);
    hal_pdm_v150_clk_rst_en_set_dmic_clken(1);
    hal_pdm_v150_clk_rst_en_set_func_up_en(1);
    hal_pdm_v150_clk_rst_en_set_clk_freq_sel(0);

    hal_pdm_v150_up_fifo_ctrl_set_up_fifo_aempty_th(CONFIG_PDM_AFIFO_AEMPTY_TH);
    hal_pdm_v150_up_fifo_ctrl_set_up_fifo_afull_th(CONFIG_PDM_AFIFO_AFULL_TH);
    hal_pdm_v150_up_fifo_st_ctrl_set_up_fifo_afull_int_en(1);
    hal_pdm_v150_up_fifo_st_ctrl_set_up_fifo_full_int_en(1);

    (void)memcpy_s(&g_hal_pdm_config[mic], sizeof(hal_pdm_config_t), attr, sizeof(hal_pdm_config_t));

    return ERRCODE_SUCC;
}

static void hal_pdm_v150_get_attr(uint8_t mic, hal_pdm_config_t *attr)
{
    (void)memcpy_s(attr, sizeof(hal_pdm_config_t), &g_hal_pdm_config[mic], sizeof(hal_pdm_config_t));
}

static errcode_t hal_pdm_v150_start(void)
{
    hal_pdm_v150_clk_rst_en_set_pdm_dp_rst_n(1);

    return ERRCODE_SUCC;
}

static errcode_t hal_pdm_v150_stop(void)
{
    hal_pdm_v150_clk_rst_en_set_pdm_dp_rst_n(0);
    return ERRCODE_SUCC;
}

static uintptr_t hal_pdm_v150_get_fifo_addr(void)
{
    return 0;
}

static uint32_t hal_pdm_v150_get_fifo_deepth(void)
{
    return 0;
}

static hal_pdm_funcs_t g_hal_pdm_funcs = {
    .init               = hal_pdm_v150_init,
    .deinit             = hal_pdm_v150_deinit,
    .set_attr           = hal_pdm_v150_set_attr,
    .get_attr           = hal_pdm_v150_get_attr,
    .start              = hal_pdm_v150_start,
    .stop               = hal_pdm_v150_stop,
    .get_fifo_addr      = hal_pdm_v150_get_fifo_addr,
    .get_fifo_deepth    = hal_pdm_v150_get_fifo_deepth
};

hal_pdm_funcs_t *hal_pdm_v150_funcs_get(void)
{
    return &g_hal_pdm_funcs;
}