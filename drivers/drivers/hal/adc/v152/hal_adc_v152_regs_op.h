/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Provides V152 adc register operation api \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-03-30， Create file. \n
 */

#ifndef HAL_ADC_V152_REGS_OP_H
#define HAL_ADC_V152_REGS_OP_H

#include <stdint.h>
#include <stdbool.h>
#include "errcode.h"
#include "hal_adc_v152_regs_def.h"
#include "adc_porting.h"
#include "tcxo.h"
#include "chip_io.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_adc_v152_regs_op ADC V152 Regs Operation
 * @ingroup  drivers_hal_adc
 * @{
 */

#define GADC_WCAL_DATA_BYTES     9
#define GADC_WCAL_DATA_BYTES8    8
#define HADC_WCAL_DATA_BYTES     14
#define HADC_WCAL_DATA_BYTES32   4
#define ADC_OS_CALI_CODE_7BITS   0x7F
#define HADC_DCOC_CALI_CODE_10BITS   0x3FF
#define HAFE_SAMPLE_VALUE_SIGN_BIT   21
#define HAFE_SAMPLE_MINUS_VALUE      4194304
#define GAFE_SAMPLE_VALUE_SIGN_BIT   16
#define GAFE_SAMPLE_MINUS_VALUE      131072
#define HAFE_CDAC_CALI_ENODE_18      18

typedef struct {
    adc_v152_adldo_voltage_t adldo_voltage;
    uint8_t adldo_trim;
    uint16_t simo_trim;
    cfg_clk_div_data_t sample_rate;
    cfg_gadc_data0_t cfg_adc_data0;
    cfg_gadc_data1_t cfg_adc_data1;
    cfg_adc_data4_t cfg_adc_data4;
    cfg_dreg_data0_t cfg_dreg_data0;
    cfg_dreg_data1_t cfg_dreg_data1;
    cfg_dreg_data2_t cfg_dreg_data2;
    cfg_dreg_data3_t cfg_dreg_data3;
    uint16_t cfg_dreg_data10;
    uint16_t cfg_dreg_data11;
    uint16_t cfg_dreg_data12;
    uint16_t cfg_dreg_data13;
    adc_v152_diag_node_t diag_node;
    bool os_cali;
    bool cdac_cali;
} hal_gafe_sample_info_t;

typedef struct {
    adc_v152_adldo_voltage_t adldo_voltage;
    uint8_t adldo_trim;
    uint16_t simo_trim;
    cfg_clk_div_data_t sample_rate;
    cfg_hadc_data0_t cfg_adc_data0;
    cfg_hadc_data1_t cfg_adc_data1;
    cfg_adc_data4_t cfg_adc_data4;
    cfg_hadc_ctrl2_t cfg_hadc_ctrl2;
    uint16_t cfg_dreg_data4;
    uint16_t cfg_dreg_data5;
    uint16_t cfg_dreg_data6;
    uint16_t cfg_dreg_data7;
    uint16_t cfg_dreg_data8;
    uint16_t cfg_dreg_data9;
    uint16_t cfg_dreg_data10;
    uint16_t cfg_dreg_data11;
    uint16_t cfg_dreg_data12;
    uint16_t cfg_dreg_data13;
    adc_v152_diag_node_t diag_node;
    bool os_cali;
    bool cdac_cali;
    bool dcoc_cali;
} hal_hafe_sample_info_t;

extern adc_regs_t *g_adc_regs;
extern adc_pmu_regs_t *g_adc_pmu_regs;
extern adc_aon_regs_t *g_adc_aon_regs;
extern adc_diag_regs_t *g_adc_diag_regs;
extern afe_config_t *g_afe_nv_cfg;
static uint32_t g_cdac_cali_default_value[HADC_WCAL_DATA_BYTES] = {
    0x78000, 0x40000, 0x20000, 0x10000, 0x8000, 0x8000, 0x4000, 0x2000, 0x1000, 0x800, 0x800, 0x400, 0x200, 0x100
};

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

/**
 * @brief  Set the value of @ref cfg_iso_data.
 * @param  [in]  cfg_iso The value of @ref cfg_iso_data.
 */
static inline void hal_afe_cfg_iso_set(cfg_iso_data_t cfg_iso)
{
    g_adc_regs->cfg_iso = cfg_iso.d32;
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
    g_adc_regs->cfg_gadc_ctrl_3 = gadc_cali.d32;
}

/**
 * @brief  Set the value of @ref cfg_adc_cali_ctrl.
 * @param  [in]  hadc_cali The value of @ref cfg_adc_cali_ctrl.
 */
static inline void hal_hafe_cali_ctrl(cfg_adc_cali_ctrl_t hadc_cali)
{
    g_adc_regs->cfg_hadc_ctrl_5 = hadc_cali.d32;
}

/**
 * @brief  Check whether extremum protection is triggered.
 * @return Whether extremum protect is triggered.
 */
static inline bool hal_hafe_os_cali_extremum_protect_status(void)
{
    bool extremum_protect = (g_adc_regs->rpt_cmp_os_1 & BIT(4)) == BIT(4) ? true : false; // BIT4 is os cali alarm bit.
    return extremum_protect;
}

/**
 * @brief  Determine whether extremum protection is overflow or underflow.
 * @return Overflow will return true, underflow will return false.
 */
static inline bool hal_hafe_os_cali_extremum_symbol_get(void)
{
    bool extremum_symbol = (g_adc_regs->rpt_hadc_data_2 & BIT(21)) == BIT(21) ? true : false; // BIT21 is sign bit.
    return extremum_symbol;
}

/**
 * @brief  Check whether extremum protection is triggered.
 * @return Whether extremum protect is triggered.
 */
static inline bool hal_gafe_os_cali_extremum_protect_status(void)
{
    bool extremum_protect = (g_adc_regs->rpt_cmp_os_0 & BIT(4)) == BIT(4) ? true : false; // BIT4 is os cali alarm bit.
    return extremum_protect;
}

/**
 * @brief  Determine whether extremum protection is overflow or underflow.
 * @return Overflow will return true, underflow will return false.
 */
static inline bool hal_gafe_os_cali_extremum_symbol_get(void)
{
    bool extremum_symbol = (g_adc_regs->rpt_gadc_data_2 & BIT(16)) == BIT(16) ? true : false; // BIT16 is sign bit.
    return extremum_symbol;
}

/**
 * @brief  Set the value of @ref cfg_cmp_os_data.
 * @param  [in]  os_cali The value of @ref cfg_cmp_os_data.
 */
static inline void hal_afe_os_cali_ctrl(cfg_cmp_os_data_t os_cali)
{
    g_adc_regs->cfg_cmp_os_0 = os_cali.d32;
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

static inline void hal_hafe_os_cali_set_code(uint8_t os_cali_code)
{
    cfg_cmp_os_code_t os_cal = { 0 };
    os_cal.b.adc_manual_cmp_os_code = os_cali_code;
    g_adc_regs->cfg_cmp_os_13 = os_cal.d32;
    os_cal.b.adc_manual_cmp_os_update_en = 1;
    g_adc_regs->cfg_cmp_os_13 = os_cal.d32;
    os_cal.b.adc_manual_cmp_os_update_en = 0;
    g_adc_regs->cfg_cmp_os_13 = os_cal.d32;
}

static inline void hal_hafe_os_cali_set_extremum_max(void)
{
    hal_hafe_os_cali_set_code(0x7f);
}

static inline void hal_hafe_os_cali_set_extremum_min(void)
{
    hal_hafe_os_cali_set_code(0);
}

static inline void hal_gafe_os_cali_set_extremum_max(void)
{
    hal_gafe_os_cali_set_code(0x7f);
}

static inline void hal_gafe_os_cali_set_extremum_min(void)
{
    hal_gafe_os_cali_set_code(0);
}

static inline uint8_t hal_hafe_os_cali_get_code(void)
{
    uint8_t os_cal_code = (uint8_t)(g_adc_regs->rpt_cmp_os_3);
    os_cal_code = os_cal_code & ADC_OS_CALI_CODE_7BITS;
    return os_cal_code;
}

/**
 * @brief  Set the value of @ref cfg_cdac_fc0_data.
 * @param  [in]  cdac_cali The value of @ref cfg_cdac_fc0_data.
 */
static inline void hal_afe_cdac_cali_ctrl(cfg_cdac_fc0_data_t cdac_cali)
{
    g_adc_regs->cfg_cdac_fc0_0 = cdac_cali.d32;
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

static inline void hal_hafe_cdac_cali_set_code32(uint8_t index, uint32_t cdac_cali_code)
{
    *(&(g_adc_regs->cfg_hadc_data_8) + index) = cdac_cali_code;
}

static inline void hal_hafe_cdac_cali_set_code16(uint8_t index, uint16_t cdac_cali_code)
{
    *(&(g_adc_regs->cfg_hadc_data_12) + index) = cdac_cali_code;
}

static inline void hal_hafe_cdac_cali_sync(void)
{
    g_adc_regs->cfg_hadc_data_7 = 1;
    g_adc_regs->cfg_hadc_data_7 = 0;
}

static inline uint32_t hal_hafe_cdac_cali_get_code32(uint8_t index)
{
    uint32_t *temp = (uint32_t *)&(g_adc_regs->rpt_hadc_data_5);
    return *(temp + index);
}

static inline uint16_t hal_hafe_cdac_cali_get_code16(uint8_t index)
{
    uint32_t *temp = (uint32_t *)&(g_adc_regs->rpt_hadc_data_9);
    return (uint16_t)*(temp + index);
}

static inline void hal_hafe_dcoc_cali_set_code_cfg(int16_t dcoc_cal_code)
{
    cfg_dcoc_cal_13_data_t dcoc_cal = { 0 };
    dcoc_cal.b.hadc_manual_dcoc_cal_code = (uint32_t)dcoc_cal_code;
    g_adc_regs->cfg_dcoc_cal_13 = dcoc_cal.d32;
    dcoc_cal.b.hadc_manual_dcoc_cal_update_en = 1;
    g_adc_regs->cfg_dcoc_cal_13 = dcoc_cal.d32;
    dcoc_cal.b.hadc_manual_dcoc_cal_update_en = 0;
    g_adc_regs->cfg_dcoc_cal_13 = dcoc_cal.d32;
}

static inline int16_t hal_gafe_dcoc_cali_get_code_cfg(void)
{
    uint32_t dcoc_cal_code = g_adc_regs->cfg_dcoc_cal_13;
    dcoc_cal_code = dcoc_cal_code & HADC_DCOC_CALI_CODE_10BITS;
    return (int16_t)dcoc_cal_code;
}

static inline int16_t hal_gafe_dcoc_cali_get_code_rpt(void)
{
    uint32_t dcoc_cal_code = g_adc_regs->rpt_dcoc_cal_1;
    dcoc_cal_code = dcoc_cal_code & HADC_DCOC_CALI_CODE_10BITS;
    return (int16_t)dcoc_cal_code;
}

static inline void hal_afe_dcoc_cali_ctrl(bool on)
{
    if (on) {
        g_adc_regs->cfg_dcoc_cal[0] = 1;
    } else {
        g_adc_regs->cfg_dcoc_cal[0] = 0;
    }
}

static inline bool hal_afe_dcoc_cali_get_sts(void)
{
    uint32_t dcoc_cali_status = 0;
    dcoc_cali_status = g_adc_regs->rpt_dcoc_cal_0;
    dcoc_cali_status = dcoc_cali_status & BIT_0;
    return ((dcoc_cali_status == BIT_0) ? true : false);
}

/**
 * @brief  Set the value of @ref cfg_dcoc_cal_data2.
 * @param  [in]  dcoc_cali The value of @ref cfg_dcoc_cal_data2.
 */
static inline void hal_afe_dcoc_cali_cfg(cfg_dcoc_cal_data2_t dcoc_cali)
{
    g_adc_regs->cfg_dcoc_cal[2] = dcoc_cali.d32;  // cfg_dcoc_cal[2].
}

static inline void hal_afe_dcoc_compensation_mode_sel(adc_v152_compensation_mode_t compensation_mode)
{
    g_adc_regs->cfg_dcoc_cal[9] = compensation_mode;  // cfg_dcoc_cal[9].
}

static inline bool hal_gafe_single_sample_get_sts(void)
{
    uint32_t single_sample_done = 0;
    single_sample_done = g_adc_regs->rpt_gadc_data_3;
    single_sample_done = single_sample_done & BIT_0;
    return ((single_sample_done == BIT_0) ? true : false);
}

static inline uint32_t hal_gafe_single_sample_get_value(void)
{
    return g_adc_regs->rpt_gadc_data_2;
}

static inline void hal_hafe_reduced_cali_accuracy(void)
{
    cfg_cdac_fc0_data1_t cfg_cdac_fc0_data1 = { 0 };
    cfg_cdac_fc0_data1.d32 = g_adc_regs->cfg_cdac_fc0_1;
    cfg_cdac_fc0_data1.b.cfg_hadc_cdac_fc_cap_start = HAFE_CDAC_CALI_ENODE_18;
    g_adc_regs->cfg_cdac_fc0_1 = cfg_cdac_fc0_data1.d32;
}

static inline void hal_hafe_cdec_cali_set_default_value(void)
{
    uint8_t i = 0;
    for (; i < HADC_WCAL_DATA_BYTES32; i++) {
        hal_hafe_cdac_cali_set_code32(i, g_cdac_cali_default_value[i]);
    }
    for (; i < HADC_WCAL_DATA_BYTES - HADC_WCAL_DATA_BYTES32; i++) {
        hal_hafe_cdac_cali_set_code16(i - HADC_WCAL_DATA_BYTES32, g_cdac_cali_default_value[i]);
    }
}

static inline void hal_afe_ana_rstn_release(void)
{
    g_adc_pmu_regs->afe_adc_rst_n = 1;
}

/**
 * @brief  Set the value of @ref afe_dig_pwr_data.
 */
static inline void hal_afe_mtcmos_en(void)
{
    afe_dig_pwr_data_t ctrl;
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
 * @brief  Set the value of @ref afe_hadc_cfg_data.
 * @param  [in]  hadc_cfg The value of @ref afe_hadc_cfg_data.
 */
static inline void hal_hadc_cfg_set(afe_hadc_cfg_data_t hadc_cfg)
{
    g_adc_pmu_regs->afe_hadc_cfg = hadc_cfg.d32;
}

/**
 * @brief  Get the value of @ref afe_hadc_cfg_data.
 * @return The value of @ref afe_hadc_cfg_data.
 */
static inline uint32_t hal_hadc_cfg_get(void)
{
    return g_adc_pmu_regs->afe_hadc_cfg;
}

/**
 * @brief  Set the value of @ref afe_adlao_cfg_data.
 * @param  [in]  adldo_voltage The value of @ref afe_adlao_cfg_data.
 */
static inline void hal_afe_adldo1_vset(adc_v152_adldo_voltage_t adldo_voltage)
{
    afe_adlao_cfg_data_t ctrl;
    ctrl.b.adldo_vset = adldo_voltage;
    g_adc_aon_regs->adldo1_cfg = ctrl.d32;
}

/**
 * @brief  Set the value of @ref afe_adlao_cfg_data.
 * @param  [in]  adldo_voltage The value of @ref afe_adlao_cfg_data.
 */
static inline void hal_afe_adldo2_vset(adc_v152_adldo_voltage_t adldo_voltage)
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
    g_adc_diag_regs->cfg_monitor_sel = 1;
}

static inline void hal_afe_diag_send2aix(void)
{
    g_adc_diag_regs->cfg_mcu_diag_trace_save_sel = 1;
}

static inline void hal_afe_diag_clk_enable(void)
{
    g_adc_diag_regs->cfg_mcu_diag_monitor_clock = 0x1fa; // Enable all clocks.
}

static inline void hal_afe_diag_source_sel(void)
{
    g_adc_diag_regs->cfg_mcu_diag_sample_sel = 0x6; // AFE ADC(GADC/HADC).
}

static inline void hal_afe_diag_sample_len(uint32_t sample_len)
{
    g_adc_diag_regs->cfg_mcu_diag_sample_length_l = sample_len & 0xffff; // Get length low 16 bit;
    g_adc_diag_regs->cfg_mcu_diag_sample_length_h = sample_len >> 0x10; // Get length high 16 bit;
}

static inline void hal_afe_diag_sample_start_addr(uint32_t start_addr)
{
    g_adc_diag_regs->cfg_mcu_diag_sample_start_addr_l = start_addr & 0xffff; // Get start addr low 16 bit;
    g_adc_diag_regs->cfg_mcu_diag_sample_start_addr_h = start_addr >> 0x10; // Get start addr high 16 bit;
}

static inline void hal_afe_diag_sample_end_addr(uint32_t end_addr)
{
    g_adc_diag_regs->cfg_mcu_diag_sample_end_addr_l = end_addr & 0xffff; // Get end addr low 16 bit;
    g_adc_diag_regs->cfg_mcu_diag_sample_end_addr_h = end_addr >> 0x10; // Get end addr high 16 bit;
}

/**
 * @brief  Set the value of @ref diag_sample_mode_data.
 */
static inline void hal_afe_diag_set_full_done_mode(void)
{
    diag_sample_mode_data_t sample_mode = { 0 };
    sample_mode.b.cfg_mcu_diag_sample_mode = 0;
    g_adc_diag_regs->cfg_mcu_diag_sample_mode = sample_mode.d16;
}

/**
 * @brief  Set the value of @ref diag_sample_mode_data.
 */
static inline void hal_afe_diag_sample_sync(void)
{
    diag_sample_mode_data_t sample_mode = { 0 };
    sample_mode.d16 = g_adc_diag_regs->cfg_mcu_diag_sample_mode;
    sample_mode.b.cfg_mcu_diag_sample_sync = 1;
    g_adc_diag_regs->cfg_mcu_diag_sample_mode = sample_mode.d16;
}

/**
 * @brief  Set the value of @ref diag_sample_mode_data.
 */
static inline void hal_afe_diag_sample_en(void)
{
    diag_sample_mode_data_t sample_mode = { 0 };
    sample_mode.d16 = g_adc_diag_regs->cfg_mcu_diag_sample_mode;
    sample_mode.b.cfg_mcu_diag_sample_en = 1;
    g_adc_diag_regs->cfg_mcu_diag_sample_mode = sample_mode.d16;
}

static inline bool hal_afe_diag_get_sts(void)
{
    uint32_t diag_done = 0;
    diag_done = g_adc_diag_regs->mcu_diag_sample_done;
    diag_done = diag_done & BIT_0;
    return ((diag_done == BIT_0) ? true : false);
}

static inline int32_t hal_gafe_sample_symbol_judge(uint32_t sample_data)
{
    int32_t data_with_symbol = sample_data;
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
errcode_t hal_adc_v152_regs_init(void);

/**
 * @brief  Deinit the adc which have been set the base address of registers.
 */
void hal_adc_v152_regs_deinit(void);

/**
 * @brief  ADC clock enable config.
 * @param  [in]  afe_dig_clk scan mode.
 */
void hal_afe_dig_clk_set(cfg_clken_data_t afe_dig_clk);

/**
 * @brief  GADC sample parameter config.
 * @param  [in]  afe_sample GADC sample parameter.
 */
void hal_gafe_sample_set(hal_gafe_sample_info_t afe_sample);

/**
 * @brief  HADC sample parameter config.
 * @param  [in]  afe_sample HADC sample parameter.
 */
void hal_hafe_sample_set(hal_hafe_sample_info_t afe_sample);

/**
 * @brief  ADC sample power on.
 * @param  [in]  gafe_sample GADC sample parameter.
 * @param  [in]  hafe_sample HADC sample parameter.
 */
void hal_afe_power_on(hal_gafe_sample_info_t gafe_sample, hal_hafe_sample_info_t hafe_sample);

/**
 * @brief  GADC sample power off.
 */
void hal_gafe_power_off(void);

/**
 * @brief  HADC sample power off.
 */
void hal_hafe_power_off(void);

/**
 * @brief  ADLDO associated with ADC power on.
 */
void hal_adc_adldo_pwron(void);

/**
 * @brief  GADC sample enable.
 */
void hal_adc_gadc_enable(void);

/**
 * @brief  HADC sample enable.
 */
void hal_adc_hadc_enable(void);

/**
 * @brief  ADC OS calibration config.
 * @param  [in]  afe_scan_mode AFE mode to use.
 */
void hal_adc_v152_os_cali(afe_scan_mode_t afe_scan_mode);

/**
 * @brief  ADC CADC calibration config.
 * @param  [in]  afe_scan_mode AFE mode to use.
 */
void hal_adc_v152_cdac_cali(afe_scan_mode_t afe_scan_mode);

/**
 * @brief  ADC DCOC calibration config, only HADC support this calibration.
 */
void hal_adc_v152_dcoc_cali(void);

/**
 * @brief  ADC voltage value config.
 */
void hal_adc_v152_adldo_vset(void);

/**
 * @brief  ADC sample start.
 * @param  [in]  channel ADC channel want to sample.
 * @param  [in]  sample_data_addr Sample data save addr.
 * @param  [in]  sample_data_len Sample data length.
 */
void hal_adc_v152_sample_start(adc_channel_t channel, uint32_t sample_data_addr, uint32_t sample_data_len);

/**
 * @brief  GAFE select sample end.
 * @param  [in]  ch GAFE end want to sample.
 */
void hal_gafe_channel_sel(uint8_t ch);

/**
 * @brief  GAFE iso on.
 * @param  [in]  cfg_dreg_data1 GAFE cfg_dreg_data1 cfg.
 */
void hal_gafe_iso_on(cfg_dreg_data1_t cfg_dreg_data1);

/**
 * @brief  GAFE iso off.
 */
void hal_gafe_iso_off(void);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif