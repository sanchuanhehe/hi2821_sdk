/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Provides V153 adc register operation api \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-08-31， Create file. \n
 */

#include "common_def.h"
#include "hal_adc_v153_regs_op.h"

#define cdac_cali_c13_invalid(x)   (((x) <= 64) || ((x) >= 256))
#define ADC_REG_LEN                16

adc_regs_t *g_adc_regs = NULL;
adc_ana_regs_t *g_adc_ana_regs = NULL;
adc_pmu_regs_t *g_adc_pmu_regs = NULL;
adc_aon_regs_t *g_adc_aon_regs = NULL;
adc_diag_regs0_t *g_adc_diag_regs0 = NULL;
adc_diag_regs1_t *g_adc_diag_regs1 = NULL;
static bool g_vset_flag = false;
static bool g_adldo_set_flag = false;

errcode_t hal_adc_v153_regs_init(void)
{
    if ((adc_porting_base_addr_get() == 0) || (adc_aon_cfg_addr_get() == 0) || (adc_pmu_base_addr_get() == 0) ||
        (adc_porting_ana_base_addr_get() == 0) || (adc_diag_cfg_addr0_get() == 0) || (adc_diag_cfg_addr1_get() == 0)) {
        return ERRCODE_ADC_REG_ADDR_INVALID;
    }

    g_adc_regs = (adc_regs_t *)adc_porting_base_addr_get();
    g_adc_ana_regs = (adc_ana_regs_t *)adc_porting_ana_base_addr_get();
    g_adc_pmu_regs = (adc_pmu_regs_t *)adc_pmu_base_addr_get();
    g_adc_aon_regs = (adc_aon_regs_t *)adc_aon_cfg_addr_get();
    g_adc_diag_regs0 = (adc_diag_regs0_t *)adc_diag_cfg_addr0_get();
    g_adc_diag_regs1 = (adc_diag_regs1_t *)adc_diag_cfg_addr1_get();
    return ERRCODE_SUCC;
}

void hal_adc_v153_regs_deinit(void)
{
    g_adc_regs = NULL;
    g_adc_pmu_regs = NULL;
    g_adc_aon_regs = NULL;
    g_adc_diag_regs0 = NULL;
    g_adc_diag_regs1 = NULL;
    g_vset_flag = false;
    g_adldo_set_flag = false;
}

void hal_afe_dig_clk_set(cfg_clken_data_t afe_dig_clk)
{
    g_adc_regs->cfg_clken = afe_dig_clk.d32;
}

void hal_gafe_power_off(void)
{
    afe_gadc_cfg_data_t gadc_cfg;
    gadc_cfg.d32 = hal_gadc_cfg_get();
    cfg_rstn_data_t cfg_rstn;
    cfg_rstn.d32 = hal_afe_dig_crg_get();
    gadc_cfg.b.s2d_gadc_en = 0;
    gadc_cfg.b.s2d_gadc_iso_en = 1;
    hal_gadc_cfg_set(gadc_cfg);
    uapi_tcxo_delay_us(HAL_ADC_V153_CFG_DELAY_30);
    gadc_cfg.b.s2d_gadc_mux_en = 0;
    hal_gadc_cfg_set(gadc_cfg);
    cfg_rstn.b.cfg_gadc_rstn_ana = 0;
    hal_afe_dig_crg_set(cfg_rstn);
}

void hal_amic_power_off(void)
{
    afe_gadc_cfg_data_t gadc_cfg;
    gadc_cfg.d32 = hal_gadc_cfg_get();
    cfg_rstn_data_t cfg_rstn;
    cfg_rstn.d32 = hal_afe_dig_crg_get();
    gadc_cfg.b.s2d_gadc_en = 0;
    gadc_cfg.b.s2d_gadc_iso_en = 1;
    hal_gadc_cfg_set(gadc_cfg);
    uapi_tcxo_delay_us(HAL_ADC_V153_CFG_DELAY_30);
    gadc_cfg.b.s2d_gadc_mux_en = 0;
    hal_gadc_cfg_set(gadc_cfg);
    cfg_rstn.b.cfg_gadc_rstn_ana = 0;
    hal_afe_dig_crg_set(cfg_rstn);
}

void hal_adc_common_enable(hal_common_sample_info_t common_sample)
{
    hal_adc_clk_div_set(common_sample.ana_div_cyc, common_sample.cfg_clk_div1);
    hal_adc_sample_set(common_sample.cfg_gadc_data0, common_sample.cfg_gadc_data1,
                       common_sample.cfg_gadc_data4, common_sample.adc_dcoc_os_val);
    hal_adc_performance_set(common_sample.cfg_freg_10, common_sample.cfg_freg_11);
    hal_afe_vrefldo_close_rst();
}

void hal_adc_gadc_enable(hal_gafe_sample_info_t gafe_sample)
{
    hal_gadc_buf_en();
    g_adc_ana_regs->cfg_freg_5 = 0;
    hal_gadc_node_sel(gafe_sample.diag_node);
    hal_afe_dig_reset();
    hal_afe_cfg_iso_set(0);
}

void hal_adc_amic_enable(hal_amic_sample_info_t amic_sample)
{
    cfg_amux_1_t cfg_amux_1 = { 0 };
    cfg_amux_1.b.amuxn_devide_disable = 1;
    cfg_amux_1.b.amuxp_devide_disable = 1;
    g_adc_regs->cfg_amux_1 = cfg_amux_1.d32;
    cfg_amux_2_t cfg_amux_2 = { 0 };
    cfg_amux_2.b.cfg_amuxn_audio_ch_sel = 0x4;
    cfg_amux_2.b.cfg_amuxp_audio_ch_sel = 0x8;
    g_adc_regs->cfg_amux_2 = cfg_amux_2.d32;
    g_adc_ana_regs->cfg_freg_9 = 0;
    cfg_ana_1_t cfg_ana_1 = { 0 };
    cfg_ana_1.b.cfg_pga_vcm_en = 1;
    cfg_ana_1.b.cfg_preamp_vcm_en = 1;
    g_adc_ana_regs->cfg_ana_1 = cfg_ana_1.d32;
    cfg_amux_1.b.amuxn_sensor_ch_sel = BIT(VICMREF);
    cfg_amux_1.b.amuxp_sensor_ch_sel = BIT(VICMREF);
    g_adc_regs->cfg_amux_1 = cfg_amux_1.d32;
    hal_gadc_buf_en();
    cfg_freg_5_t cfg_freg_5 = { 0 };
    cfg_freg_5.b.cfg_pga_bypass_en = 1;
    cfg_freg_5.b.cfg_pga_rc_en = 1;
    cfg_freg_5.b.cfg_pga_gain_sel = amic_sample.pga_gain;
    g_adc_ana_regs->cfg_freg_5 = cfg_freg_5.d32;
    cfg_amux_1.b.amuxn_sensor_ch_sel = 0;
    cfg_amux_1.b.amuxp_sensor_ch_sel = 0;
    g_adc_regs->cfg_amux_1 = cfg_amux_1.d32;
    cfg_ana_1.b.cfg_bufp_en = 0;
    cfg_ana_1.b.cfg_bufn_en = 0;
    g_adc_ana_regs->cfg_ana_1 = cfg_ana_1.d32;
    cfg_freg_5.b.cfg_pga_bypass_en = 0;
    g_adc_ana_regs->cfg_freg_5 = cfg_freg_5.d32;
    cfg_ana_1.b.cfg_preamp_os_dac_en = 1;
    cfg_ana_1.b.cfg_pga_en = 1;
    cfg_ana_1.b.cfg_preamp_en = 1;
    g_adc_ana_regs->cfg_ana_1 = cfg_ana_1.d32;
    hal_afe_remap_set();
    hal_afe_pga_set();
    hal_afe_dcoc_static_set();
    hal_gadc_node_sel(amic_sample.diag_node);
    hal_afe_dig_reset();
    hal_afe_cfg_iso_set(0);
}

void hal_gafe_enable(void)
{
    afe_gadc_cfg_data_t gadc_cfg_data = { 0 };
    gadc_cfg_data.b.s2d_gadc_iso_en = 1;
    gadc_cfg_data.b.s2d_gadc_mux_en = 1;
    g_adc_pmu_regs->afe_gadc_cfg = gadc_cfg_data.d32;
    uapi_tcxo_delay_ms(HAL_ADC_V153_CFG_DELAY_30);
    gadc_cfg_data.b.s2d_gadc_iso_en = 0;
    g_adc_pmu_regs->afe_gadc_cfg = gadc_cfg_data.d32;
    uapi_tcxo_delay_ms(HAL_ADC_V153_CFG_DELAY_30);
    gadc_cfg_data.b.s2d_gadc_en = 1;
    g_adc_pmu_regs->afe_gadc_cfg = gadc_cfg_data.d32;
}

void hal_adc_v153_os_cali(void)
{
    cfg_adc_cali_ctrl_t gadc_cali;
    gadc_cali.b.cfg_adc_cal_en = 1;
    hal_gafe_cali_ctrl(gadc_cali);
    uapi_tcxo_delay_ms(HAL_ADC_V153_CFG_DELAY_50);
    g_adc_regs->cfg_cmp_os_0 = 1;
    uapi_tcxo_delay_ms(HAL_ADC_V153_CFG_DELAY_50);
    while (!hal_afe_os_cali_get_sts()) { }
    g_adc_regs->cfg_cmp_os_0 = 0;
    gadc_cali.b.cfg_adc_cal_en = 0;
    hal_gafe_cali_ctrl(gadc_cali);
    g_adc_regs->cfg_cmp_os_11 = 1;
}

void hal_adc_v153_cdac_cali(void)
{
    cfg_cdac_fc0_data1_t cdac_cali_bit;
    cdac_cali_bit.b.cfg_gadc_cdac_fc_cap_start = 0xB;
    cdac_cali_bit.b.cfg_gadc_cdac_fc_cap_end = 0x11;
    hal_gadc_gap_start_set(cdac_cali_bit);

    cfg_adc_cali_ctrl_t gadc_cali;
    gadc_cali.b.cfg_adc_cal_en = 1;
    hal_gafe_cali_ctrl(gadc_cali);
    uapi_tcxo_delay_ms(HAL_ADC_V153_CFG_DELAY_50);
    g_adc_regs->cfg_cdac_fc0_0 = 1;
    while (!hal_afe_cdac_cali_get_sts()) { }
    g_adc_regs->cfg_cdac_fc0_0 = 0;
    gadc_cali.b.cfg_adc_cal_en = 0;
    hal_gafe_cali_ctrl(gadc_cali);
}

void hal_adc_v153_spd_cali(void)
{
    g_adc_regs->cfg_sar_spd_0 = 1;
    while (!hal_afe_spd_cali_get_sts()) { }
    g_adc_regs->cfg_sar_spd_0 = 0;
}

void hal_adc_v153_dcoc_cali(void)
{
    g_adc_regs->cfg_dcoc_cal_0 = 1;
    while (!hal_afe_dcoc_cali_get_sts()) { }
    g_adc_regs->cfg_dcoc_cal_0 = 0;
}

void hal_gafe_channel_sel(cfg_amux_1_t cfg_amux_1)
{
    g_adc_regs->cfg_amux_1 = cfg_amux_1.d32;
}