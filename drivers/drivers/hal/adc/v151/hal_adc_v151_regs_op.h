/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides V151 adc register operation api \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-09-16， Create file. \n
 */

#ifndef HAL_ADC_V151_REGS_OP_H
#define HAL_ADC_V151_REGS_OP_H

#include <stdint.h>
#include <stdbool.h>
#include "errcode.h"
#include "hal_adc_v151_regs_def.h"
#include "adc_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_adc_v151_regs_op ADC V151 Regs Operation
 * @ingroup  drivers_hal_adc
 * @{
 */

extern adc_regs_t *g_adc_regs;

/**
 * @brief  Set the value of @ref adc_power_data.en.
 */
static inline void hal_adc_reg_power_set_en(uint32_t value)
{
    adc_power_data_t power;
    power.d32 = g_adc_regs->aux_adc_reg;
    power.b.en = value;
    g_adc_regs->aux_adc_reg = power.d32;
}

/**
 * @brief  Set the value of @ref adc_power_data.comp_vref.
 */
static inline void hal_adc_reg_power_set_comp(uint32_t value)
{
    adc_power_data_t power;
    power.d32 = g_adc_regs->aux_adc_reg;
    power.b.comp_vref = value;
    g_adc_regs->aux_adc_reg = power.d32;
}

/**
 * @brief  Set the value of @ref adc_ctrl_data.rstn.
 */
static inline void hal_adc_reg_ctrl_set_rstn(void)
{
    adc_ctrl_data_t ctrl;
    ctrl.d32 = g_adc_regs->aux_adc_cfg;
    ctrl.b.rstn = 1;
    g_adc_regs->aux_adc_cfg = ctrl.d32;
}

/**
 * @brief  Get the value of @ref adc_stick_data_t.flag.
 */
static inline uint32_t hal_adc_reg_spl_get_flag(void)
{
    adc_stick_data_t stick_data;
    stick_data.d32 = g_adc_regs->aux_adc_stick;
    return stick_data.b.flag;
}

/**
 * @brief  Get the value of @ref adc_stick_data_t.stick.
 */
static inline uint32_t hal_adc_reg_spl_get_stick(void)
{
    adc_stick_data_t stick_data;
    stick_data.d32 = g_adc_regs->aux_adc_stick;
    return stick_data.b.stick;
}

/**
 * @brief  Set the value of @ref adc_freq_data_t.freq.
 */
static inline void hal_adc_reg_freq_set_freq(uint32_t value)
{
    adc_freq_data_t freq_data;
    freq_data.d32 = g_adc_regs->aux_adc_scan_freq;
    freq_data.b.freq = value;
    g_adc_regs->aux_adc_scan_freq = freq_data.d32;
}

/**
 * @brief  Set the value of @ref adc_freq_data_t.dis_sel0.
 */
static inline void hal_adc_reg_freq_set_dis_sel0(uint32_t value)
{
    adc_freq_data_t freq_data;
    freq_data.d32 = g_adc_regs->aux_adc_scan_freq;
    freq_data.b.dis_sel0 = value;
    g_adc_regs->aux_adc_scan_freq = freq_data.d32;
}

/**
 * @brief  Set the value of @ref adc_freq_data_t.dis_sel0.
 */
static inline void hal_adc_reg_freq_set_avg_sel0(uint32_t value)
{
    adc_freq_data_t freq_data;
    freq_data.d32 = g_adc_regs->aux_adc_scan_freq;
    freq_data.b.avg_sel0 = value;
    g_adc_regs->aux_adc_scan_freq = freq_data.d32;
}

/**
 * @brief  Set the value of @ref adc_freq_data_t.dis_sel1.
 */
static inline void hal_adc_reg_freq_set_dis_sel1(uint32_t value)
{
    adc_freq_data_t freq_data;
    freq_data.d32 = g_adc_regs->aux_adc_scan_freq;
    freq_data.b.dis_sel1 = value;
    g_adc_regs->aux_adc_scan_freq = freq_data.d32;
}

/**
 * @brief  Set the value of @ref adc_freq_data_t.dis_sel1.
 */
static inline void hal_adc_reg_freq_set_avg_sel1(uint32_t value)
{
    adc_freq_data_t freq_data;
    freq_data.d32 = g_adc_regs->aux_adc_scan_freq;
    freq_data.b.avg_sel1 = value;
    g_adc_regs->aux_adc_scan_freq = freq_data.d32;
}

/**
 * @brief  Set the value of @ref adc_freq_data_t.dis_sel2.
 */
static inline void hal_adc_reg_freq_set_dis_sel2(uint32_t value)
{
    adc_freq_data_t freq_data;
    freq_data.d32 = g_adc_regs->aux_adc_scan_freq;
    freq_data.b.dis_sel2 = value;
    g_adc_regs->aux_adc_scan_freq = freq_data.d32;
}

static inline void hal_adc_reg_scan_mode_set_avg_sel2(hal_adc_scan_mode_t mode, uint32_t value)
{
    adc_trig_data_t mode_data;
    mode_data.d32 = g_adc_regs->aux_adc_scan_mode[mode];
    mode_data.b.avg_sel2 = value;
    g_adc_regs->aux_adc_scan_mode[mode] = mode_data.d32;
}

/**
 * @brief  Set the value of @ref adc_scan_en_data_t.scan_en.
 */
static inline void hal_adc_reg_scan_en_set_en(uint32_t value)
{
    adc_scan_en_data_t en_data;
    en_data.d32 = g_adc_regs->aux_adc_scan_en;
    en_data.b.scan_en = value;
    g_adc_regs->aux_adc_scan_en = en_data.d32;
}

/**
 * @brief  Get the value of @ref adc_scan_en_data_t.scan_en.
 */
static inline uint32_t hal_adc_reg_scan_en_get_en(void)
{
    adc_scan_en_data_t en_data;
    en_data.d32 = g_adc_regs->aux_adc_scan_en;
    return en_data.b.scan_en;
}

/**
 * @brief  Set the value of @ref adc_auto_cfg_data_t.ch_sel.
 */
static inline void hal_adc_reg_auto_cfg_set_ch(uint32_t value)
{
    adc_auto_cfg_data_t cfg_data;
    cfg_data.d32 = g_adc_regs->aux_adc_auto_cfg;
    cfg_data.b.ch_sel = value;
    g_adc_regs->aux_adc_auto_cfg = cfg_data.d32;
}

/**
 * @brief  Set the value of @ref adc_auto_cfg_data_t.man.
 */
static inline void hal_adc_reg_auto_cfg_set_man(uint32_t value)
{
    adc_auto_cfg_data_t cfg_data;
    cfg_data.d32 = g_adc_regs->aux_adc_auto_cfg;
    cfg_data.b.man = value;
    g_adc_regs->aux_adc_auto_cfg = cfg_data.d32;
}

/**
 * @brief  Set the value of @ref adc_auto_cfg_data_t.man_sel.
 */
static inline void hal_adc_reg_auto_cfg_set_man_sel(uint32_t value)
{
    adc_auto_cfg_data_t cfg_data;
    cfg_data.d32 = g_adc_regs->aux_adc_auto_cfg;
    cfg_data.b.man_sel = value;
    g_adc_regs->aux_adc_auto_cfg = cfg_data.d32;
}

/**
 * @brief  Init the adc which will set the base address of registers.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t.
 */
errcode_t hal_adc_v151_regs_init(void);

/**
 * @brief  Deinit the adc which will clear the base address of registers has been set by @ref hal_adc_v151_regs_init.
 */
void hal_adc_v151_regs_deinit(void);

/**
 * @brief  Set the value of @ref adc_cail_en_data.cali_en.
 * @param  [in]  value scan mode.
 */
void hal_adc_reg_cali_en_set(uint32_t value);

/**
 * @brief  Set the scan mode of adc.
 * @param  [in]  mode Scan mode type. For details, see @ref hal_adc_scan_mode_t.
 * @param  [in]  channel The adc channel. For details, see @ref adc_channel_t.
 * @param  [in]  value scan mode.
 */
void hal_adc_reg_scan_mode_set(hal_adc_scan_mode_t mode, adc_channel_t channel, uint32_t value);

/**
 * @brief  Get the scan mode of adc.
 * @param  [in]  mode Scan mode type. For details, see @ref hal_adc_scan_mode_t.
 * @param  [in]  channel The adc channel. For details, see @ref adc_channel_t.
 */
uint32_t hal_adc_reg_scan_mode_get(hal_adc_scan_mode_t mode, adc_channel_t channel);

/**
 * @brief  Set the scan mode enable of adc.
 * @param  [in]  channel The adc channel. For details, see @ref adc_channel_t.
 * @param  [in]  value scan mode.
 */
void hal_adc_reg_scan_en_set_ch(adc_channel_t channel, uint32_t value);

/**
 * @brief  Set the scan start of adc.
 * @param  [in]  reg Scan start type. For details, see @ref hal_adc_scan_en_start_t.
 * @param  [in]  channel The adc channel. For details, see @ref adc_channel_t.
 * @param  [in]  value scan start value.
 */
void hal_adc_reg_scan_start_set(hal_adc_scan_en_start_t reg, adc_channel_t channel, uint32_t value);

/**
 * @brief  Get the scan start of adc.
 * @param  [in]  reg Scan start type. For details, see @ref hal_adc_scan_en_start_t.
 * @param  [in]  channel The adc channel. For details, see @ref adc_channel_t.
 */
uint32_t hal_adc_reg_scan_start_get(hal_adc_scan_en_start_t reg, adc_channel_t channel);

/**
 * @brief  Get the scan ptr of adc.
 * @param  [in]  ptr Scan ptr number. For details, see @ref adc_ptr_data_t.
 * @param  [in]  on ptr channel select. true : channel 2_3/2/5/7 false : channel 0_1/0/4/6/8.
 */
uint32_t hal_adc_reg_scan_ptr_get(hal_adc_scan_ptr_t ptr, bool on);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif