/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Provides V153 adc register operation api \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-08-31， Create file. \n
 */

#ifndef HAL_ADC_V153_REGS_OP_H
#define HAL_ADC_V153_REGS_OP_H

#include <stdint.h>
#include <stdbool.h>
#include "errcode.h"
#include "hal_adc_v153_regs_def.h"
#include "adc_porting.h"
#include "tcxo.h"
#include "chip_io.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_adc_v153_regs_op ADC V153 Regs Operation
 * @ingroup  drivers_hal_adc
 * @{
 */

#define GADC_WCAL_DATA_BYTES     9
#define GADC_WCAL_DATA_BYTES8    8
#define ADC_OS_CALI_CODE_7BITS   0x7F
#define GAFE_SAMPLE_VALUE_SIGN_BIT   17
#define GAFE_SAMPLE_MINUS_VALUE      262144
#define NFCLDO_CFG_OFFSET      0x230

typedef struct {
    uint32_t ana_div_cyc;
    cfg_clk_div1_t cfg_clk_div1;
    cfg_gadc_data0_t cfg_gadc_data0;
    cfg_gadc_data1_t cfg_gadc_data1;
    cfg_gadc_data4_t cfg_gadc_data4;
    uint32_t adc_dcoc_os_val;
    cfg_freg_10_t cfg_freg_10;
    cfg_freg_11_t cfg_freg_11;
} hal_common_sample_info_t;

typedef struct {
    cfg_amux_1_t cfg_amux_1;
    adc_v153_diag_node_t diag_node;
    bool os_cali;
    bool cdac_cali;
} hal_gafe_sample_info_t;

typedef struct {
    uint32_t pga_gain;
    adc_v153_diag_node_t diag_node;
    bool os_cali;
    bool cdac_cali;
    bool dcoc_cali;
} hal_amic_sample_info_t;

extern adc_regs_t *g_adc_regs;
extern adc_ana_regs_t *g_adc_ana_regs;
extern adc_pmu_regs_t *g_adc_pmu_regs;
extern adc_aon_regs_t *g_adc_aon_regs;
extern adc_diag_regs0_t *g_adc_diag_regs0;
extern adc_diag_regs1_t *g_adc_diag_regs1;

/**
 * @brief  Set the value of @ref cfg_rstn_data.
 * @param  [in]  cfg_rstn The value of @ref cfg_rstn_data.
 */
static inline void hal_afe_dig_crg_set(cfg_rstn_data_t cfg_rstn)
{
    g_adc_regs->cfg_rstn = cfg_rstn.d32;
}

/**
 * @brief  Get the value of @ref cfg_rstn_data.
 * @return The value of @ref cfg_rstn_data.
 */
static inline uint32_t hal_afe_dig_crg_get(void)
{
    return g_adc_regs->cfg_rstn;
}

static inline void hal_afe_dig_clr(void)
{
    g_adc_regs->cfg_clken = 0;
    g_adc_regs->cfg_rstn = 0;
}

static inline void hal_afe_dig_reset(void)
{
    cfg_rstn_data_t cfg_rstn = { 0 };
    cfg_rstn.b.cfg_rstn_tst = 1;
    cfg_rstn.b.cfg_gadc_rstn_bc = 1;
    cfg_rstn.b.cfg_gadc_rstn_fc = 1;
    cfg_rstn.b.cfg_gadc_rstn_data = 1;
    cfg_rstn.b.cfg_gadc_rstn_ana = 1;
    g_adc_regs->cfg_rstn = cfg_rstn.d32;
}

static inline void hal_afe_dig_start(void)
{
    cfg_clken_data_t cfg_clken_data = { 0 };
    cfg_clken_data.b.cfg_clken_tst = 1;
    cfg_clken_data.b.cfg_gadc_clken_bc = 1;
    cfg_clken_data.b.cfg_gadc_clken_fc = 1;
    cfg_clken_data.b.cfg_gadc_clken_byp = 1;
    cfg_clken_data.b.cfg_gadc_clken_prechg = 1;
    cfg_clken_data.b.cfg_gadc_clken_ctrl = 1;
    g_adc_regs->cfg_clken = cfg_clken_data.d32;
}

static inline void hal_adc_clk_div_set(uint32_t ana_div_cyc, cfg_clk_div1_t cfg_clk_div1)
{
    g_adc_regs->cfg_clk_div_0 = ana_div_cyc;
    g_adc_regs->cfg_clk_div_1 = cfg_clk_div1.d32;
}

static inline void hal_adc_sample_set(cfg_gadc_data0_t cfg_gadc_data0, cfg_gadc_data1_t cfg_gadc_data1,
                                      cfg_gadc_data4_t cfg_gadc_data4, uint32_t adc_dcoc_os_val)
{
    g_adc_regs->cfg_gadc_data_0 = cfg_gadc_data0.d32;
    g_adc_regs->cfg_gadc_data_1 = cfg_gadc_data1.d32;
    g_adc_regs->cfg_gadc_data_4 = cfg_gadc_data4.d32;
    g_adc_regs->cfg_gadc_data_5 = adc_dcoc_os_val;
}

static inline void hal_gadc_gap_start_set(cfg_cdac_fc0_data1_t cdac_fc0_data1)
{
    g_adc_regs->cfg_cdac_fc0_1 = cdac_fc0_data1.d32;
}

static inline void hal_adc_performance_set(cfg_freg_10_t cfg_freg_10, cfg_freg_11_t cfg_freg_11)
{
    g_adc_ana_regs->cfg_freg_10 = cfg_freg_10.d32;
    g_adc_ana_regs->cfg_freg_11 = cfg_freg_11.d32;
}

/**
 * @brief  Set the value of CFG_ISO.
 * @param  [in]  cfg_iso_en The value CFG_ISO.
 */
static inline void hal_afe_cfg_iso_set(uint32_t cfg_iso_en)
{
    g_adc_regs->cfg_iso = cfg_iso_en;
}

/**
 * @brief  Set the value of @ref adldo_en_cfg_data.
 * @param  [in]  adldo_cfg The value of @ref adldo_en_cfg_data.
 */
static inline void hal_afe_adldo1_set(adldo_en_cfg_data_t adldo_cfg)
{
    g_adc_aon_regs->adldo1_en = adldo_cfg.d32;
}

/**
 * @brief  Set the value of @ref adldo_en_cfg_data.
 * @param  [in]  adldo_cfg The value of @ref adldo_en_cfg_data.
 */
static inline void hal_afe_adldo2_set(adldo_en_cfg_data_t adldo_cfg)
{
    g_adc_aon_regs->adldo2_en = adldo_cfg.d32;
}

/**
 * @brief  Set the value of @ref cfg_adc_cali_ctrl.
 * @param  [in]  gadc_cali The value of @ref cfg_adc_cali_ctrl.
 */
static inline void hal_gafe_cali_ctrl(cfg_adc_cali_ctrl_t gadc_cali)
{
    g_adc_regs->cfg_gadc_ctrl_4 = gadc_cali.d32;
}

static inline void hal_gafe_os_cali_set_code(uint8_t os_cali_code)
{
    cfg_cmp_os_code_t os_cal = { 0 };
    os_cal.b.adc_manual_cmp_os_code = os_cali_code;
    g_adc_regs->cfg_cmp_os_12 = os_cal.d32;
    os_cal.b.adc_manual_cmp_os_update_en = 1;
    g_adc_regs->cfg_cmp_os_12 = os_cal.d32;
    os_cal.b.adc_manual_cmp_os_update_en = 0;
    g_adc_regs->cfg_cmp_os_12 = os_cal.d32;
}

static inline uint8_t hal_gafe_os_cali_get_code(void)
{
    uint8_t os_cal_code = (uint8_t)(g_adc_regs->rpt_cmp_os_2);
    os_cal_code = os_cal_code & ADC_OS_CALI_CODE_7BITS;
    return os_cal_code;
}

static inline void hal_gafe_cdac_cali_set_code32(uint32_t cdac_cali_code)
{
    g_adc_regs->cfg_gadc_data_8 = cdac_cali_code;
}

static inline void hal_gafe_cdac_cali_set_code16(uint8_t index, uint16_t cdac_cali_code)
{
    *(&(g_adc_regs->cfg_gadc_data_9) + index) = cdac_cali_code;
}

static inline void hal_gafe_cdac_cali_sync(void)
{
    g_adc_regs->cfg_gadc_data_7 = 1;
    g_adc_regs->cfg_gadc_data_7 = 0;
}

static inline uint32_t hal_gafe_cdac_cali_get_code32(void)
{
    return g_adc_regs->rpt_gadc_data_5;
}

static inline uint16_t hal_gafe_cdac_cali_get_code16(uint8_t index)
{
    uint32_t *temp = (uint32_t *)&(g_adc_regs->rpt_gadc_data_6);
    return (uint16_t)*(temp + index);
}

static inline bool hal_afe_spd_cali_get_sts(void)
{
    uint32_t spd_cali_status = 0;
    spd_cali_status = g_adc_regs->rpt_sar_spd_1;
    spd_cali_status = spd_cali_status & BIT_0;
    return (spd_cali_status == BIT_0);
}

static inline bool hal_afe_os_cali_get_sts(void)
{
    uint32_t os_cali_status = 0;
    os_cali_status = g_adc_regs->rpt_cmp_os_0;
    os_cali_status = os_cali_status & BIT_0;
    return (os_cali_status == BIT_0);
}

static inline bool hal_afe_cdac_cali_get_sts(void)
{
    uint32_t cdac_cali_status = 0;
    cdac_cali_status = g_adc_regs->rpt_cdac_fc0_0;
    cdac_cali_status = cdac_cali_status & BIT_0;
    return (cdac_cali_status == BIT_0);
}

static inline bool hal_afe_dcoc_cali_get_sts(void)
{
    uint32_t dcoc_cali_status = 0;
    dcoc_cali_status = g_adc_regs->rpt_dcoc_cal_0;
    dcoc_cali_status = dcoc_cali_status & BIT_0;
    return (dcoc_cali_status == BIT_0);
}

static inline uint16_t hal_gafe_single_sample_get_value(void)
{
    return (uint16_t)(g_adc_regs->rpt_gadc_data_2);
}

static inline void hal_afe_adcldo_rpoly_trim_set(void)
{
    cfg_ana_5_t cfg_ana_5 = { 0 };
    cfg_ana_5.d32 = g_adc_ana_regs->cfg_ana_5;
    cfg_ana_5.b.cfg_afe_adcldo_rpoly_trim = 0x8;
    g_adc_ana_regs->cfg_ana_5 = cfg_ana_5.d32;
}

static inline void hal_afe_afeldo_rpoly_trim_set(void)
{
    cfg_ana_7_t cfg_ana_7 = { 0 };
    cfg_ana_7.d32 = g_adc_ana_regs->cfg_ana_7;
    cfg_ana_7.b.cfg_afe_afeldo_rpoly_trim = 0x8;
    g_adc_ana_regs->cfg_ana_7 = cfg_ana_7.d32;
}

static inline void hal_afe_vrefldo_rpoly_trim_set(void)
{
    cfg_freg_0_t cfg_freg_0 = { 0 };
    cfg_freg_0.d32 = g_adc_ana_regs->cfg_freg_0;
    cfg_freg_0.b.cfg_vrefldo_rpoly_trim = 0x8;
    g_adc_ana_regs->cfg_freg_0 = cfg_freg_0.d32;
}

static inline void hal_afe_adcldo_open(void)
{
    g_adc_ana_regs->cfg_ana_4 = 0x1;
    uapi_tcxo_delay_us(HAL_ADC_V153_CFG_DELAY_150);
    cfg_ana_5_t cfg_ana_5 = { 0 };
    cfg_ana_5.d32 = g_adc_ana_regs->cfg_ana_5;
    cfg_ana_5.b.cfg_afe_adcldo_en_dly = 0x1;
    g_adc_ana_regs->cfg_ana_5 = cfg_ana_5.d32;
}

static inline void hal_afe_afeldo_open(void)
{
    g_adc_ana_regs->cfg_ana_6 = 0x1;
    uapi_tcxo_delay_us(HAL_ADC_V153_CFG_DELAY_150);
    cfg_ana_7_t cfg_ana_7 = { 0 };
    cfg_ana_7.d32 = g_adc_ana_regs->cfg_ana_7;
    cfg_ana_7.b.cfg_afe_afeldo_en_dly = 0x1;
    g_adc_ana_regs->cfg_ana_7 = cfg_ana_7.d32;
}

static inline void hal_afe_vrefldo_open(void)
{
    cfg_ana_0_t cfg_ana_0 = { 0 };
    cfg_ana_0.b.cfg_vrefldo_en = 1;
    g_adc_ana_regs->cfg_ana_0 = cfg_ana_0.d32;
    uapi_tcxo_delay_us(HAL_ADC_V153_CFG_DELAY_150);
    cfg_ana_0.b.cfg_vrefldo_en_dly = 1;
    g_adc_ana_regs->cfg_ana_0 = cfg_ana_0.d32;
}

static inline void hal_afe_vrefldo_close_rst(void)
{
    cfg_ana_0_t temp = { 0 };
    temp.d32 = g_adc_ana_regs->cfg_ana_0;
    temp.b.cfg_topbias_en = 1;
    g_adc_ana_regs->cfg_ana_0 = temp.d32;
    uapi_tcxo_delay_us(HAL_ADC_V153_CFG_DELAY_150);
    temp.b.cfg_clk_rstn = 1;
    g_adc_ana_regs->cfg_ana_0 = temp.d32;
}

static inline void hal_gadc_buf_en(void)
{
    cfg_ana_1_t cfg_ana_1 = { 0 };
    cfg_ana_1.d32 = g_adc_ana_regs->cfg_ana_1;
    cfg_ana_1.b.cfg_bufn_en = 1;
    cfg_ana_1.b.cfg_bufp_en = 1;
    g_adc_ana_regs->cfg_ana_1 = cfg_ana_1.d32;
}

static inline void hal_afe_remap_set(void)
{
    cfg_freg_3_t cfg_freg_3 = { 0 };
    cfg_freg_3.d32 = g_adc_ana_regs->cfg_freg_3;
    cfg_freg_3.b.cfg_preamp_rc_en = 1;
    g_adc_ana_regs->cfg_freg_3 = cfg_freg_3.d32;
}

static inline void hal_afe_pga_set(void)
{
    cfg_freg_5_t cfg_freg_5 = { 0 };
    cfg_freg_5.d32 = g_adc_ana_regs->cfg_freg_5;
    cfg_freg_5.b.cfg_pga_rc_en = 1;
    g_adc_ana_regs->cfg_freg_5 = cfg_freg_5.d32;
    cfg_freg_6_t cfg_freg_6 = { 0 };
    cfg_freg_6.d32 = g_adc_ana_regs->cfg_freg_6;
    cfg_freg_6.b.cfg_pga_ibias_sel = 1;
    g_adc_ana_regs->cfg_freg_6 = cfg_freg_6.d32;
}

static inline void hal_afe_dcoc_static_set(void)
{
    cfg_freg_7_t cfg_freg_7 = { 0 };
    cfg_freg_7.d32 = g_adc_ana_regs->cfg_freg_7;
    cfg_freg_7.b.cfg_chopclk_div_dly_sel = 1;
    g_adc_ana_regs->cfg_freg_7 = cfg_freg_7.d32;
}

static inline void hal_gadc_node_sel(adc_v153_diag_node_t diag_node)
{
    g_adc_regs->cfg_tst_1 = diag_node;
}

static inline void hal_afe_ana_rstn_release(void)
{
    g_adc_pmu_regs->afe_adc_rst_n = 1;
}

static inline void hal_afe_release_xo32m(void)
{
    g_adc_pmu_regs->afe_iso_cfg = 0;
}

/**
 * @brief  Set the value of @ref afe_dig_pwr_data.
 */
static inline void hal_afe_mtcmos_en(void)
{
    afe_dig_pwr_data_t ctrl = { 0 };
    ctrl.b.afe_pwr_en = 1;
    ctrl.b.afe_iso_en = 1;
    g_adc_pmu_regs->afe_dig_pwr_en = ctrl.d16;
}

static inline void hal_afe_iso_release(void)
{
    afe_dig_pwr_data_t ctrl;
    ctrl.d16 = g_adc_pmu_regs->afe_dig_pwr_en;
    ctrl.b.afe_iso_en = 0;
    g_adc_pmu_regs->afe_dig_pwr_en = ctrl.d16;
}

static inline void hal_afe_dig_apb_rstn_release(void)
{
    g_adc_pmu_regs->afe_soft_rst = 1;
}

static inline void hal_afe_dig_clk_release(void)
{
    g_adc_pmu_regs->afe_clk_en = 1;
}

/**
 * @brief  Set the value of @ref afe_ldo_cfg_data.
 * @param  [in]  ldo_cfg The value of @ref afe_ldo_cfg_data.
 */
static inline void hal_afe_ldo_set(afe_ldo_cfg_data_t ldo_cfg)
{
    g_adc_pmu_regs->afe_adc_ldo_cfg = ldo_cfg.d32;
}

/**
 * @brief  Set the value of @ref afe_gadc_cfg_data.
 * @param  [in]  gadc_cfg The value of @ref afe_gadc_cfg_data.
 */
static inline void hal_gadc_cfg_set(afe_gadc_cfg_data_t gadc_cfg)
{
    g_adc_pmu_regs->afe_gadc_cfg = gadc_cfg.d32;
}

/**
 * @brief  Get the value of @ref afe_gadc_cfg_data.
 * @return The value of @ref afe_gadc_cfg_data.
 */
static inline uint32_t hal_gadc_cfg_get(void)
{
    return g_adc_pmu_regs->afe_gadc_cfg;
}

static inline void hal_gafe_channel_close(void)
{
    afe_gadc_cfg_data_t gadc_cfg;
    gadc_cfg.d32 = hal_gadc_cfg_get();
    gadc_cfg.b.s2d_gadc_en = 0;
    hal_gadc_cfg_set(gadc_cfg);
}

/**
 * @brief  Set the value of @ref afe_adlao_cfg_data.
 * @param  [in]  adldo_voltage The value of @ref afe_adlao_cfg_data.
 */
static inline void hal_afe_adldo1_vset(adc_v153_adldo_voltage_t adldo_voltage)
{
    afe_adlao_cfg_data_t ctrl;
    ctrl.b.adldo_vset = adldo_voltage;
    g_adc_aon_regs->adldo1_cfg = ctrl.d32;
}

/**
 * @brief  Set the value of @ref afe_adlao_cfg_data.
 * @param  [in]  adldo_voltage The value of @ref afe_adlao_cfg_data.
 */
static inline void hal_afe_adldo2_vset(adc_v153_adldo_voltage_t adldo_voltage)
{
    afe_adlao_cfg_data_t ctrl;
    ctrl.b.adldo_vset = adldo_voltage;
    g_adc_aon_regs->adldo2_cfg = ctrl.d32;
}

static inline void hal_afe_adldo1_trim_set(uint8_t adldo_trim)
{
    g_adc_aon_regs->adldo1_trim = adldo_trim;
}

static inline void hal_afe_adldo2_trim_set(uint8_t adldo_trim)
{
    g_adc_aon_regs->adldo2_trim = adldo_trim;
}

static inline void hal_afe_simo_trim_set(uint16_t simo_trim)
{
    g_adc_aon_regs->simo_ref_trim = simo_trim;
}

static inline void hal_afe_diag_set(void)
{
    g_adc_diag_regs0->cfg_monitor_sel = 1;
}

static inline void hal_afe_diag_send2aix(void)
{
    g_adc_diag_regs0->cfg_mcu_diag_trace_save_sel = 1;
}

static inline void hal_afe_diag_clk_enable(void)
{
    g_adc_diag_regs0->cfg_mcu_diag_monitor_clock = 0x1fa; // Enable all clocks.
}

static inline void hal_afe_diag_source_sel(void)
{
    g_adc_diag_regs1->cfg_mcu_diag_sample_sel = 0x6; // AFE ADC(GADC/HADC).
}

static inline void hal_afe_diag_sample_len(uint32_t sample_len)
{
    g_adc_diag_regs1->cfg_mcu_diag_sample_length_l = sample_len & 0xffff; // Get length low 16 bit;
    g_adc_diag_regs1->cfg_mcu_diag_sample_length_h = sample_len >> 0x10; // Get length high 16 bit;
}

static inline void hal_afe_diag_sample_start_addr(uint32_t start_addr)
{
    g_adc_diag_regs1->cfg_mcu_diag_sample_start_addr_l = start_addr & 0xffff; // Get start addr low 16 bit;
    g_adc_diag_regs1->cfg_mcu_diag_sample_start_addr_h = start_addr >> 0x10; // Get start addr high 16 bit;
}

static inline void hal_afe_diag_sample_end_addr(uint32_t end_addr)
{
    g_adc_diag_regs1->cfg_mcu_diag_sample_end_addr_l = end_addr & 0xffff; // Get end addr low 16 bit;
    g_adc_diag_regs1->cfg_mcu_diag_sample_end_addr_h = end_addr >> 0x10; // Get end addr high 16 bit;
}

/**
 * @brief  Set the value of @ref diag_sample_mode_data.
 */
static inline void hal_afe_diag_set_full_done_mode(void)
{
    diag_sample_mode_data_t sample_mode = { 0 };
    sample_mode.b.cfg_mcu_diag_sample_mode = 0;
    g_adc_diag_regs1->cfg_mcu_diag_sample_mode = sample_mode.d16;
}

/**
 * @brief  Set the value of @ref diag_sample_mode_data.
 */
static inline void hal_afe_diag_sample_sync(void)
{
    diag_sample_mode_data_t sample_mode;
    sample_mode.d16 = g_adc_diag_regs1->cfg_mcu_diag_sample_mode;
    sample_mode.b.cfg_mcu_diag_sample_sync = 1;
    g_adc_diag_regs1->cfg_mcu_diag_sample_mode = sample_mode.d16;
}

/**
 * @brief  Set the value of @ref diag_sample_mode_data.
 */
static inline void hal_afe_diag_sample_en(void)
{
    diag_sample_mode_data_t sample_mode;
    sample_mode.d16 = g_adc_diag_regs1->cfg_mcu_diag_sample_mode;
    sample_mode.b.cfg_mcu_diag_sample_en = 1;
    g_adc_diag_regs1->cfg_mcu_diag_sample_mode = sample_mode.d16;
}

static inline void hal_afe_diag_sample_off(void)
{
}

static inline bool hal_afe_diag_get_sts(void)
{
    uint32_t diag_done = 0;
    diag_done = g_adc_diag_regs1->mcu_diag_sample_done;
    diag_done = diag_done & BIT_0;
    return ((diag_done == BIT_0) ? true : false);
}

static inline int16_t hal_gafe_sample_symbol_judge(int16_t sample_data)
{
    int16_t data_with_symbol = sample_data;
    if ((sample_data & BIT(GAFE_SAMPLE_VALUE_SIGN_BIT)) != 0) {
        data_with_symbol -= GAFE_SAMPLE_MINUS_VALUE;
    }
    return data_with_symbol;
}

/**
 * @brief  Init the adc which will set the base address of registers.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t.
 */
errcode_t hal_adc_v153_regs_init(void);

/**
 * @brief  Deinit the adc which have been set the base address of registers.
 */
void hal_adc_v153_regs_deinit(void);

/**
 * @brief  ADC clock enable config.
 * @param  [in]  afe_dig_clk scan mode.
 */
void hal_afe_dig_clk_set(cfg_clken_data_t afe_dig_clk);

/**
 * @brief  GADC sample power off.
 */
void hal_gafe_power_off(void);
void hal_amic_power_off(void);

void hal_adc_common_enable(hal_common_sample_info_t common_sample);
/**
 * @brief  GADC sample enable.
 */
void hal_adc_gadc_enable(hal_gafe_sample_info_t gafe_sample);

/**
 * @brief  AMIC sample enable.
 */
void hal_adc_amic_enable(hal_amic_sample_info_t amic_sample);

void hal_gafe_enable(void);

/**
 * @brief  ADC OS calibration config.
 */
void hal_adc_v153_os_cali(void);

/**
 * @brief  ADC CADC calibration config.
 */
void hal_adc_v153_cdac_cali(void);

/**
 * @brief  ADC DCOC calibration config.
 */
void hal_adc_v153_dcoc_cali(void);

/**
 * @brief  ADC SPD calibration config.
 */
void hal_adc_v153_spd_cali(void);

void hal_gafe_channel_sel(cfg_amux_1_t cfg_amux_1);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif