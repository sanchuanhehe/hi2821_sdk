/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Provides V151 PDM register operation API. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-01-31, Create file. \n
 */
#ifndef HAL_PDM_V151_REGS_OP_H
#define HAL_PDM_V151_REGS_OP_H

#include "hal_pdm_v151_regs_def.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_pdm_v151_regs_op PDM V151 Regs Operation
 * @ingroup  drivers_hal_pdm
 * @{
 */

#define HAL_PDM_V151_FIFO_OFFSET 0x628
#define HAL_PDM_V151_FIFO_DEEPTH 32

extern uintptr_t g_hal_pdm_regs;

/**
 * @brief  Set the value of @ref pdm_v151_codec_clk_en0_data.mic4_up_afifo_clken.
 * @param  [in]  val The value of @ref pdm_v151_codec_clk_en0_data.mic4_up_afifo_clken.
 */
static inline void hal_pdm_v151_codec_clk_en0_set_mic4_up_afifo_clken(uint32_t val)
{
    pdm_v151_codec_clk_en0_data_t codec_clk_en0;
    codec_clk_en0.d32 = ((pdm_v151_regs_t *)g_hal_pdm_regs)->codec_clk_en0;
    codec_clk_en0.b.mic4_up_afifo_clken = val;
    ((pdm_v151_regs_t *)g_hal_pdm_regs)->codec_clk_en0 = codec_clk_en0.d32;
}

/**
 * @brief  Set the value of @ref pdm_v151_codec_clk_en0_data.mic5_up_afifo_clken.
 * @param  [in]  val The value of @ref pdm_v151_codec_clk_en0_data.mic5_up_afifo_clken.
 */
static inline void hal_pdm_v151_codec_clk_en0_set_mic5_up_afifo_clken(uint32_t val)
{
    pdm_v151_codec_clk_en0_data_t codec_clk_en0;
    codec_clk_en0.d32 = ((pdm_v151_regs_t *)g_hal_pdm_regs)->codec_clk_en0;
    codec_clk_en0.b.mic5_up_afifo_clken = val;
    ((pdm_v151_regs_t *)g_hal_pdm_regs)->codec_clk_en0 = codec_clk_en0.d32;
}

/**
 * @brief  Set the value of @ref pdm_v151_codec_clk_en0_data.
 * @param  [in]  val The value of @ref pdm_v151_codec_clk_en0_data.
 */
static inline void hal_pdm_v151_codec_clk_en0_set(uint32_t val)
{
    ((pdm_v151_regs_t *)g_hal_pdm_regs)->codec_clk_en0 = val;
}

/**
 * @brief  Set the value of @ref pdm_v151_codec_clk_en0_data.mic4_pga_clken.
 * @param  [in]  val The value of @ref pdm_v151_codec_clk_en0_data.mic4_pga_clken.
 */
static inline void hal_pdm_v151_codec_clk_en0_set_mic4_pga_clken(uint32_t val)
{
    pdm_v151_codec_clk_en0_data_t codec_clk_en0;
    codec_clk_en0.d32 = ((pdm_v151_regs_t *)g_hal_pdm_regs)->codec_clk_en0;
    codec_clk_en0.b.mic4_pga_clken = val;
    ((pdm_v151_regs_t *)g_hal_pdm_regs)->codec_clk_en0 = codec_clk_en0.d32;
}

/**
 * @brief  Set the value of @ref pdm_v151_codec_clk_en0_data.mic5_pga_clken.
 * @param  [in]  val The value of @ref pdm_v151_codec_clk_en0_data.mic5_pga_clken.
 */
static inline void hal_pdm_v151_codec_clk_en0_set_mic5_pga_clken(uint32_t val)
{
    pdm_v151_codec_clk_en0_data_t codec_clk_en0;
    codec_clk_en0.d32 = ((pdm_v151_regs_t *)g_hal_pdm_regs)->codec_clk_en0;
    codec_clk_en0.b.mic5_pga_clken = val;
    ((pdm_v151_regs_t *)g_hal_pdm_regs)->codec_clk_en0 = codec_clk_en0.d32;
}

/**
 * @brief  Set the value of @ref pdm_v151_codec_clk_en1_data.
 * @param  [in]  val The value of @ref pdm_v151_codec_clk_en1_data.
 */
static inline void hal_pdm_v151_codec_clk_en1_set(uint32_t val)
{
    ((pdm_v151_regs_t *)g_hal_pdm_regs)->codec_clk_en1 = val;
}

/**
 * @brief  Set the value of @ref pdm_v151_codec_clk_en1_data.mic4_srcdn_clken.
 * @param  [in]  val The value of @ref pdm_v151_codec_clk_en1_data.mic4_srcdn_clken.
 */
static inline void hal_pdm_v151_codec_clk_en1_set_mic4_srcdn_clken(uint32_t val)
{
    pdm_v151_codec_clk_en1_data_t codec_clk_en1;
    codec_clk_en1.d32 = ((pdm_v151_regs_t *)g_hal_pdm_regs)->codec_clk_en1;
    codec_clk_en1.b.mic4_srcdn_clken = val;
    ((pdm_v151_regs_t *)g_hal_pdm_regs)->codec_clk_en1 = codec_clk_en1.d32;
}

/**
 * @brief  Set the value of @ref pdm_v151_codec_clk_en1_data.mic5_srcdn_clken.
 * @param  [in]  val The value of @ref pdm_v151_codec_clk_en1_data.mic5_srcdn_clken.
 */
static inline void hal_pdm_v151_codec_clk_en1_set_mic5_srcdn_clken(uint32_t val)
{
    pdm_v151_codec_clk_en1_data_t codec_clk_en1;
    codec_clk_en1.d32 = ((pdm_v151_regs_t *)g_hal_pdm_regs)->codec_clk_en1;
    codec_clk_en1.b.mic5_srcdn_clken = val;
    ((pdm_v151_regs_t *)g_hal_pdm_regs)->codec_clk_en1 = codec_clk_en1.d32;
}

/**
 * @brief  Set the value of @ref pdm_v151_codec_clk_en1_data.mic4_adc_clken.
 * @param  [in]  val The value of @ref pdm_v151_codec_clk_en1_data.mic4_adc_clken.
 */
static inline void hal_pdm_v151_codec_clk_en1_set_mic4_adc_clken(uint32_t val)
{
    pdm_v151_codec_clk_en1_data_t codec_clk_en1;
    codec_clk_en1.d32 = ((pdm_v151_regs_t *)g_hal_pdm_regs)->codec_clk_en1;
    codec_clk_en1.b.mic4_adc_clken = val;
    ((pdm_v151_regs_t *)g_hal_pdm_regs)->codec_clk_en1 = codec_clk_en1.d32;
}

/**
 * @brief  Set the value of @ref pdm_v151_codec_clk_en1_data.mic5_adc_clken.
 * @param  [in]  val The value of @ref pdm_v151_codec_clk_en1_data.mic5_adc_clken.
 */
static inline void hal_pdm_v151_codec_clk_en1_set_mic5_adc_clken(uint32_t val)
{
    pdm_v151_codec_clk_en1_data_t codec_clk_en1;
    codec_clk_en1.d32 = ((pdm_v151_regs_t *)g_hal_pdm_regs)->codec_clk_en1;
    codec_clk_en1.b.mic5_adc_clken = val;
    ((pdm_v151_regs_t *)g_hal_pdm_regs)->codec_clk_en1 = codec_clk_en1.d32;
}

/**
 * @brief  Set the value of @ref pdm_v151_codec_clk_en2_data.
 * @param  [in]  val The value of @ref pdm_v151_codec_clk_en2_data.
 */
static inline void hal_pdm_v151_codec_clk_en2_set(uint32_t val)
{
    ((pdm_v151_regs_t *)g_hal_pdm_regs)->codec_clk_en2 = val;
}

/**
 * @brief  Set the value of @ref pdm_v151_codec_clk_en2_data.dmic0_clken.
 * @param  [in]  val The value of @ref pdm_v151_codec_clk_en2_data.dmic0_clken.
 */
static inline void hal_pdm_v151_codec_clk_en2_set_dmic0_clken(uint32_t val)
{
    pdm_v151_codec_clk_en2_data_t codec_clk_en2;
    codec_clk_en2.d32 = ((pdm_v151_regs_t *)g_hal_pdm_regs)->codec_clk_en2;
    codec_clk_en2.b.dmic0_clken = val;
    ((pdm_v151_regs_t *)g_hal_pdm_regs)->codec_clk_en2 = codec_clk_en2.d32;
}

/**
 * @brief  Set the value of @ref pdm_v151_codec_sw_rst_n_data.codec_sw_rst_n.
 * @param  [in]  val The value of @ref pdm_v151_codec_sw_rst_n_data.codec_sw_rst_n.
 */
static inline void hal_pdm_v151_codec_sw_rst_n_set_codec_sw_rst_n(uint32_t val)
{
    pdm_v151_codec_sw_rst_n_data_t codec_sw_rst_n;
    codec_sw_rst_n.d32 = ((pdm_v151_regs_t *)g_hal_pdm_regs)->codec_sw_rst_n;
    codec_sw_rst_n.b.codec_sw_rst_n = val;
    ((pdm_v151_regs_t *)g_hal_pdm_regs)->codec_sw_rst_n = codec_sw_rst_n.d32;
}

/**
 * @brief  Set the value of @ref pdm_v151_codec_sw_rst_n_data.rst_2dmic_access_irq.
 * @param  [in]  val The value of @ref pdm_v151_codec_sw_rst_n_data.rst_2dmic_access_irq.
 */
static inline void hal_pdm_v151_codec_sw_rst_n_set_rst_2dmic_access_irq(uint32_t val)
{
    pdm_v151_codec_sw_rst_n_data_t codec_sw_rst_n;
    codec_sw_rst_n.d32 = ((pdm_v151_regs_t *)g_hal_pdm_regs)->codec_sw_rst_n;
    codec_sw_rst_n.b.rst_2dmic_access_irq = val;
    ((pdm_v151_regs_t *)g_hal_pdm_regs)->codec_sw_rst_n = codec_sw_rst_n.d32;
}

/**
 * @brief  Set the value of @ref pdm_v151_pga_gainoffset_ctrl2_data.mic4_adc_pga_gainoffset.
 * @param  [in]  val The value of @ref pdm_v151_pga_gainoffset_ctrl2_data.mic4_adc_pga_gainoffset.
 */
static inline void hal_pdm_v151_pga_gainoffset_ctrl2_set_mic4_adc_pga_gainoffset(uint32_t val)
{
    pdm_v151_pga_gainoffset_ctrl2_data_t pga_gainoffset_ctrl2;
    pga_gainoffset_ctrl2.d32 = ((pdm_v151_regs_t *)g_hal_pdm_regs)->pga_gainoffset_ctrl2;
    pga_gainoffset_ctrl2.b.mic4_adc_pga_gainoffset = val;
    ((pdm_v151_regs_t *)g_hal_pdm_regs)->pga_gainoffset_ctrl2 = pga_gainoffset_ctrl2.d32;
}

/**
 * @brief  Set the value of @ref pdm_v151_pga_gainoffset_ctrl2_data.mic5_adc_pga_gainoffset.
 * @param  [in]  val The value of @ref pdm_v151_pga_gainoffset_ctrl2_data.mic5_adc_pga_gainoffset.
 */
static inline void hal_pdm_v151_pga_gainoffset_ctrl2_set_mic5_adc_pga_gainoffset(uint32_t val)
{
    pdm_v151_pga_gainoffset_ctrl2_data_t pga_gainoffset_ctrl2;
    pga_gainoffset_ctrl2.d32 = ((pdm_v151_regs_t *)g_hal_pdm_regs)->pga_gainoffset_ctrl2;
    pga_gainoffset_ctrl2.b.mic5_adc_pga_gainoffset = val;
    ((pdm_v151_regs_t *)g_hal_pdm_regs)->pga_gainoffset_ctrl2 = pga_gainoffset_ctrl2.d32;
}

/**
 * @brief  Set the value of @ref pdm_v151_mic_pga_ctrl_data.pga_linear_sel.
 * @param  [in]  mic The mic need to configure.
 * @param  [in]  val The value of @ref pdm_v151_mic_pga_ctrl_data.pga_linear_sel.
 */
static inline void hal_pdm_v151_mic_pga_ctrl_set_pga_linear_sel(uint8_t mic, uint32_t val)
{
    pdm_v151_mic_pga_ctrl_data_t mic_pga_ctrl;
    mic_pga_ctrl.d32 = (mic == PDM_V151_DMIC4) ? ((pdm_v151_regs_t *)g_hal_pdm_regs)->mic4_pga_ctrl :
                                                 ((pdm_v151_regs_t *)g_hal_pdm_regs)->mic5_pga_ctrl;
    mic_pga_ctrl.b.pga_linear_sel = val;
    if (mic == PDM_V151_DMIC4) {
        ((pdm_v151_regs_t *)g_hal_pdm_regs)->mic4_pga_ctrl = mic_pga_ctrl.d32;
    } else {
        ((pdm_v151_regs_t *)g_hal_pdm_regs)->mic5_pga_ctrl = mic_pga_ctrl.d32;
    }
}

/**
 * @brief  Set the value of @ref pdm_v151_mic_pga_ctrl_data.pga_zero_num.
 * @param  [in]  mic The mic need to configure.
 * @param  [in]  val The value of @ref pdm_v151_mic_pga_ctrl_data.pga_zero_num.
 */
static inline void hal_pdm_v151_mic_pga_ctrl_set_pga_zero_num(uint8_t mic, uint32_t val)
{
    pdm_v151_mic_pga_ctrl_data_t mic_pga_ctrl;
    mic_pga_ctrl.d32 = (mic == PDM_V151_DMIC4) ? ((pdm_v151_regs_t *)g_hal_pdm_regs)->mic4_pga_ctrl :
                                                 ((pdm_v151_regs_t *)g_hal_pdm_regs)->mic5_pga_ctrl;
    mic_pga_ctrl.b.pga_zero_num = val;
    if (mic == PDM_V151_DMIC4) {
        ((pdm_v151_regs_t *)g_hal_pdm_regs)->mic4_pga_ctrl = mic_pga_ctrl.d32;
    } else {
        ((pdm_v151_regs_t *)g_hal_pdm_regs)->mic5_pga_ctrl = mic_pga_ctrl.d32;
    }
}

/**
 * @brief  Set the value of @ref pdm_v151_mic_pga_ctrl_data.pga_thre_id.
 * @param  [in]  mic The mic need to configure.
 * @param  [in]  val The value of @ref pdm_v151_mic_pga_ctrl_data.pga_thre_id.
 */
static inline void hal_pdm_v151_mic_pga_ctrl_set_pga_thre_id(uint8_t mic, uint32_t val)
{
    pdm_v151_mic_pga_ctrl_data_t mic_pga_ctrl;
    mic_pga_ctrl.d32 = (mic == PDM_V151_DMIC4) ? ((pdm_v151_regs_t *)g_hal_pdm_regs)->mic4_pga_ctrl :
                                                 ((pdm_v151_regs_t *)g_hal_pdm_regs)->mic5_pga_ctrl;
    mic_pga_ctrl.b.pga_thre_id = val;
    if (mic == PDM_V151_DMIC4) {
        ((pdm_v151_regs_t *)g_hal_pdm_regs)->mic4_pga_ctrl = mic_pga_ctrl.d32;
    } else {
        ((pdm_v151_regs_t *)g_hal_pdm_regs)->mic5_pga_ctrl = mic_pga_ctrl.d32;
    }
}

/**
 * @brief  Set the value of @ref pdm_v151_mic_pga_ctrl_data.pga_noise_en.
 * @param  [in]  mic The mic need to configure.
 * @param  [in]  val The value of @ref pdm_v151_mic_pga_ctrl_data.pga_noise_en.
 */
static inline void hal_pdm_v151_mic_pga_ctrl_set_pga_noise_en(uint8_t mic, uint32_t val)
{
    pdm_v151_mic_pga_ctrl_data_t mic_pga_ctrl;
    mic_pga_ctrl.d32 = (mic == PDM_V151_DMIC4) ? ((pdm_v151_regs_t *)g_hal_pdm_regs)->mic4_pga_ctrl :
                                                 ((pdm_v151_regs_t *)g_hal_pdm_regs)->mic5_pga_ctrl;
    mic_pga_ctrl.b.pga_noise_en = val;
    if (mic == PDM_V151_DMIC4) {
        ((pdm_v151_regs_t *)g_hal_pdm_regs)->mic4_pga_ctrl = mic_pga_ctrl.d32;
    } else {
        ((pdm_v151_regs_t *)g_hal_pdm_regs)->mic5_pga_ctrl = mic_pga_ctrl.d32;
    }
}

/**
 * @brief  Set the value of @ref pdm_v151_mic_pga_ctrl_data.pga_bypass.
 * @param  [in]  mic The mic need to configure.
 * @param  [in]  val The value of @ref pdm_v151_mic_pga_ctrl_data.pga_bypass.
 */
static inline void hal_pdm_v151_mic_pga_ctrl_set_pga_bypass(uint8_t mic, uint32_t val)
{
    pdm_v151_mic_pga_ctrl_data_t mic_pga_ctrl;
    mic_pga_ctrl.d32 = (mic == PDM_V151_DMIC4) ? ((pdm_v151_regs_t *)g_hal_pdm_regs)->mic4_pga_ctrl :
                                                 ((pdm_v151_regs_t *)g_hal_pdm_regs)->mic5_pga_ctrl;
    mic_pga_ctrl.b.pga_bypass = val;
    if (mic == PDM_V151_DMIC4) {
        ((pdm_v151_regs_t *)g_hal_pdm_regs)->mic4_pga_ctrl = mic_pga_ctrl.d32;
    } else {
        ((pdm_v151_regs_t *)g_hal_pdm_regs)->mic5_pga_ctrl = mic_pga_ctrl.d32;
    }
}

/**
 * @brief  Set the value of @ref pdm_v151_mic_pga_ctrl_data.pga_fade_out.
 * @param  [in]  mic The mic need to configure.
 * @param  [in]  val The value of @ref pdm_v151_mic_pga_ctrl_data.pga_fade_out.
 */
static inline void hal_pdm_v151_mic_pga_ctrl_set_pga_fade_out(uint8_t mic, uint32_t val)
{
    pdm_v151_mic_pga_ctrl_data_t mic_pga_ctrl;
    mic_pga_ctrl.d32 = (mic == PDM_V151_DMIC4) ? ((pdm_v151_regs_t *)g_hal_pdm_regs)->mic4_pga_ctrl :
                                                 ((pdm_v151_regs_t *)g_hal_pdm_regs)->mic5_pga_ctrl;
    mic_pga_ctrl.b.pga_fade_out = val;
    if (mic == PDM_V151_DMIC4) {
        ((pdm_v151_regs_t *)g_hal_pdm_regs)->mic4_pga_ctrl = mic_pga_ctrl.d32;
    } else {
        ((pdm_v151_regs_t *)g_hal_pdm_regs)->mic5_pga_ctrl = mic_pga_ctrl.d32;
    }
}

/**
 * @brief  Set the value of @ref pdm_v151_mic_pga_ctrl_data.pga_fade_in.
 * @param  [in]  mic The mic need to configure.
 * @param  [in]  val The value of @ref pdm_v151_mic_pga_ctrl_data.pga_fade_in.
 */
static inline void hal_pdm_v151_mic_pga_ctrl_set_pga_fade_in(uint8_t mic, uint32_t val)
{
    pdm_v151_mic_pga_ctrl_data_t mic_pga_ctrl;
    mic_pga_ctrl.d32 = (mic == PDM_V151_DMIC4) ? ((pdm_v151_regs_t *)g_hal_pdm_regs)->mic4_pga_ctrl :
                                                 ((pdm_v151_regs_t *)g_hal_pdm_regs)->mic5_pga_ctrl;
    mic_pga_ctrl.b.pga_fade_in = val;
    if (mic == PDM_V151_DMIC4) {
        ((pdm_v151_regs_t *)g_hal_pdm_regs)->mic4_pga_ctrl = mic_pga_ctrl.d32;
    } else {
        ((pdm_v151_regs_t *)g_hal_pdm_regs)->mic5_pga_ctrl = mic_pga_ctrl.d32;
    }
}

/**
 * @brief  Set the value of @ref pdm_v151_mic_pga_ctrl_data.pga_cfg_small_sig_en.
 * @param  [in]  mic The mic need to configure.
 * @param  [in]  val The value of @ref pdm_v151_mic_pga_ctrl_data.pga_cfg_small_sig_en.
 */
static inline void hal_pdm_v151_mic_pga_ctrl_set_pga_cfg_small_sig_en(uint8_t mic, uint32_t val)
{
    pdm_v151_mic_pga_ctrl_data_t mic_pga_ctrl;
    mic_pga_ctrl.d32 = (mic == PDM_V151_DMIC4) ? ((pdm_v151_regs_t *)g_hal_pdm_regs)->mic4_pga_ctrl :
                                                 ((pdm_v151_regs_t *)g_hal_pdm_regs)->mic5_pga_ctrl;
    mic_pga_ctrl.b.pga_cfg_small_sig_en = val;
    if (mic == PDM_V151_DMIC4) {
        ((pdm_v151_regs_t *)g_hal_pdm_regs)->mic4_pga_ctrl = mic_pga_ctrl.d32;
    } else {
        ((pdm_v151_regs_t *)g_hal_pdm_regs)->mic5_pga_ctrl = mic_pga_ctrl.d32;
    }
}

/**
 * @brief  Set the value of @ref pdm_v151_mic_pga_ctrl_data.pga_cfg_anti_clip_en.
 * @param  [in]  mic The mic need to configure.
 * @param  [in]  val The value of @ref pdm_v151_mic_pga_ctrl_data.pga_cfg_anti_clip_en.
 */
static inline void hal_pdm_v151_mic_pga_ctrl_set_pga_cfg_anti_clip_en(uint8_t mic, uint32_t val)
{
    pdm_v151_mic_pga_ctrl_data_t mic_pga_ctrl;
    mic_pga_ctrl.d32 = (mic == PDM_V151_DMIC4) ? ((pdm_v151_regs_t *)g_hal_pdm_regs)->mic4_pga_ctrl :
                                                 ((pdm_v151_regs_t *)g_hal_pdm_regs)->mic5_pga_ctrl;
    mic_pga_ctrl.b.pga_cfg_anti_clip_en = val;
    if (mic == PDM_V151_DMIC4) {
        ((pdm_v151_regs_t *)g_hal_pdm_regs)->mic4_pga_ctrl = mic_pga_ctrl.d32;
    } else {
        ((pdm_v151_regs_t *)g_hal_pdm_regs)->mic5_pga_ctrl = mic_pga_ctrl.d32;
    }
}

/**
 * @brief  Set the value of @ref pdm_v151_mic_pga_ctrl_data.pga_cfg_fade_en.
 * @param  [in]  mic The mic need to configure.
 * @param  [in]  val The value of @ref pdm_v151_mic_pga_ctrl_data.pga_cfg_fade_en.
 */
static inline void hal_pdm_v151_mic_pga_ctrl_set_pga_cfg_fade_en(uint8_t mic, uint32_t val)
{
    pdm_v151_mic_pga_ctrl_data_t mic_pga_ctrl;
    mic_pga_ctrl.d32 = (mic == PDM_V151_DMIC4) ? ((pdm_v151_regs_t *)g_hal_pdm_regs)->mic4_pga_ctrl :
                                                 ((pdm_v151_regs_t *)g_hal_pdm_regs)->mic5_pga_ctrl;
    mic_pga_ctrl.b.pga_cfg_fade_en = val;
    if (mic == PDM_V151_DMIC4) {
        ((pdm_v151_regs_t *)g_hal_pdm_regs)->mic4_pga_ctrl = mic_pga_ctrl.d32;
    } else {
        ((pdm_v151_regs_t *)g_hal_pdm_regs)->mic5_pga_ctrl = mic_pga_ctrl.d32;
    }
}

/**
 * @brief  Set the value of @ref pdm_v151_mic_pga_ctrl_data.pga_gain.
 * @param  [in]  mic The mic need to configure.
 * @param  [in]  val The value of @ref pdm_v151_mic_pga_ctrl_data.pga_gain.
 */
static inline void hal_pdm_v151_mic_pga_ctrl_set_pga_gain(uint8_t mic, uint32_t val)
{
    pdm_v151_mic_pga_ctrl_data_t mic_pga_ctrl;
    mic_pga_ctrl.d32 = (mic == PDM_V151_DMIC4) ? ((pdm_v151_regs_t *)g_hal_pdm_regs)->mic4_pga_ctrl :
                                                 ((pdm_v151_regs_t *)g_hal_pdm_regs)->mic5_pga_ctrl;
    mic_pga_ctrl.b.pga_gain = val;
    if (mic == PDM_V151_DMIC4) {
        ((pdm_v151_regs_t *)g_hal_pdm_regs)->mic4_pga_ctrl = mic_pga_ctrl.d32;
    } else {
        ((pdm_v151_regs_t *)g_hal_pdm_regs)->mic5_pga_ctrl = mic_pga_ctrl.d32;
    }
}

/**
 * @brief  Set the value of @ref pdm_v151_srcdn_ctrl0_data.mic4_srcdn_src_mode.
 * @param  [in]  val The value of @ref pdm_v151_srcdn_ctrl0_data.mic4_srcdn_src_mode.
 */
static inline void hal_pdm_v151_srcdn_ctrl0_set_mic4_srcdn_src_mode(uint32_t val)
{
    pdm_v151_srcdn_ctrl0_data_t srcdn_ctrl0;
    srcdn_ctrl0.d32 = ((pdm_v151_regs_t *)g_hal_pdm_regs)->srcdn_ctrl0;
    srcdn_ctrl0.b.mic4_srcdn_src_mode = val;
    ((pdm_v151_regs_t *)g_hal_pdm_regs)->srcdn_ctrl0 = srcdn_ctrl0.d32;
}

/**
 * @brief  Set the value of @ref pdm_v151_srcdn_ctrl0_data.mic4_srcdn_fifo_clr.
 * @param  [in]  val The value of @ref pdm_v151_srcdn_ctrl0_data.mic4_srcdn_fifo_clr.
 */
static inline void hal_pdm_v151_srcdn_ctrl0_set_mic4_srcdn_fifo_clr(uint32_t val)
{
    pdm_v151_srcdn_ctrl0_data_t srcdn_ctrl0;
    srcdn_ctrl0.d32 = ((pdm_v151_regs_t *)g_hal_pdm_regs)->srcdn_ctrl0;
    srcdn_ctrl0.b.mic4_srcdn_fifo_clr = val;
    ((pdm_v151_regs_t *)g_hal_pdm_regs)->srcdn_ctrl0 = srcdn_ctrl0.d32;
}

/**
 * @brief  Set the value of @ref pdm_v151_srcdn_ctrl0_data.mic5_srcdn_src_mode.
 * @param  [in]  val The value of @ref pdm_v151_srcdn_ctrl0_data.mic5_srcdn_src_mode.
 */
static inline void hal_pdm_v151_srcdn_ctrl0_set_mic5_srcdn_src_mode(uint32_t val)
{
    pdm_v151_srcdn_ctrl0_data_t srcdn_ctrl0;
    srcdn_ctrl0.d32 = ((pdm_v151_regs_t *)g_hal_pdm_regs)->srcdn_ctrl0;
    srcdn_ctrl0.b.mic5_srcdn_src_mode = val;
    ((pdm_v151_regs_t *)g_hal_pdm_regs)->srcdn_ctrl0 = srcdn_ctrl0.d32;
}

/**
 * @brief  Set the value of @ref pdm_v151_srcdn_ctrl0_data.mic5_srcdn_fifo_clr.
 * @param  [in]  val The value of @ref pdm_v151_srcdn_ctrl0_data.mic5_srcdn_fifo_clr.
 */
static inline void hal_pdm_v151_srcdn_ctrl0_set_mic5_srcdn_fifo_clr(uint32_t val)
{
    pdm_v151_srcdn_ctrl0_data_t srcdn_ctrl0;
    srcdn_ctrl0.d32 = ((pdm_v151_regs_t *)g_hal_pdm_regs)->srcdn_ctrl0;
    srcdn_ctrl0.b.mic5_srcdn_fifo_clr = val;
    ((pdm_v151_regs_t *)g_hal_pdm_regs)->srcdn_ctrl0 = srcdn_ctrl0.d32;
}

/**
 * @brief  Set the value of @ref pdm_v151_dmic_ctrl_data.dmic0_sw_dmic_mode.
 * @param  [in]  val The value of @ref pdm_v151_dmic_ctrl_data.dmic0_sw_dmic_mode.
 */
static inline void hal_pdm_v151_dmic_ctrl_set_dmic0_sw_dmic_mode(uint32_t val)
{
    pdm_v151_dmic_ctrl_data_t dmic_ctrl;
    dmic_ctrl.d32 = ((pdm_v151_regs_t *)g_hal_pdm_regs)->dmic_ctrl;
    dmic_ctrl.b.dmic0_sw_dmic_mode = val;
    ((pdm_v151_regs_t *)g_hal_pdm_regs)->dmic_ctrl = dmic_ctrl.d32;
}

/**
 * @brief  Set the value of @ref pdm_v151_dmic_ctrl_data.dmic0_reverse.
 * @param  [in]  val The value of @ref pdm_v151_dmic_ctrl_data.dmic0_reverse.
 */
static inline void hal_pdm_v151_dmic_ctrl_set_dmic0_reverse(uint32_t val)
{
    pdm_v151_dmic_ctrl_data_t dmic_ctrl;
    dmic_ctrl.d32 = ((pdm_v151_regs_t *)g_hal_pdm_regs)->dmic_ctrl;
    dmic_ctrl.b.dmic0_reverse = val;
    ((pdm_v151_regs_t *)g_hal_pdm_regs)->dmic_ctrl = dmic_ctrl.d32;
}

/**
 * @brief  Set the value of @ref pdm_v151_dmic_div_data.fs_dmic0.
 * @param  [in]  val The value of @ref pdm_v151_dmic_div_data.fs_dmic0.
 */
static inline void hal_pdm_v151_dmic_div_set_fs_dmic0(uint32_t val)
{
    pdm_v151_dmic_div_data_t dmic_div;
    dmic_div.d32 = ((pdm_v151_regs_t *)g_hal_pdm_regs)->dmic_div;
    dmic_div.b.fs_dmic0 = val;
    ((pdm_v151_regs_t *)g_hal_pdm_regs)->dmic_div = dmic_div.d32;
}

/**
 * @brief  Set the value of @ref pdm_v151_mic_up_afifo_ctrl_data.mic4_upfifo_aempty_th.
 * @param  [in]  val The value of @ref pdm_v151_mic_up_afifo_ctrl_data.mic4_upfifo_aempty_th.
 */
static inline void hal_pdm_v151_mic_up_afifo_ctrl_set_mic4_upfifo_aempty_th(uint32_t val)
{
    pdm_v151_mic_up_afifo_ctrl_data_t mic_up_afifo_ctrl;
    mic_up_afifo_ctrl.d32 = ((pdm_v151_regs_t *)g_hal_pdm_regs)->mic_up_afifo_ctrl;
    mic_up_afifo_ctrl.b.mic4_upfifo_aempty_th = val;
    ((pdm_v151_regs_t *)g_hal_pdm_regs)->mic_up_afifo_ctrl = mic_up_afifo_ctrl.d32;
}

/**
 * @brief  Set the value of @ref pdm_v151_mic_up_afifo_ctrl_data.mic4_upfifo_afull_th.
 * @param  [in]  val The value of @ref pdm_v151_mic_up_afifo_ctrl_data.mic4_upfifo_afull_th.
 */
static inline void hal_pdm_v151_mic_up_afifo_ctrl_set_mic4_upfifo_afull_th(uint32_t val)
{
    pdm_v151_mic_up_afifo_ctrl_data_t mic_up_afifo_ctrl;
    mic_up_afifo_ctrl.d32 = ((pdm_v151_regs_t *)g_hal_pdm_regs)->mic_up_afifo_ctrl;
    mic_up_afifo_ctrl.b.mic4_upfifo_afull_th = val;
    ((pdm_v151_regs_t *)g_hal_pdm_regs)->mic_up_afifo_ctrl = mic_up_afifo_ctrl.d32;
}

/**
 * @brief  Set the value of @ref pdm_v151_mic_up_afifo_ctrl_data.mic4_up_fifo_clr.
 * @param  [in]  val The value of @ref pdm_v151_mic_up_afifo_ctrl_data.mic4_up_fifo_clr.
 */
static inline void hal_pdm_v151_mic_up_afifo_ctrl_set_mic4_up_fifo_clr(uint32_t val)
{
    pdm_v151_mic_up_afifo_ctrl_data_t mic_up_afifo_ctrl;
    mic_up_afifo_ctrl.d32 = ((pdm_v151_regs_t *)g_hal_pdm_regs)->mic_up_afifo_ctrl;
    mic_up_afifo_ctrl.b.mic4_up_fifo_clr = val;
    ((pdm_v151_regs_t *)g_hal_pdm_regs)->mic_up_afifo_ctrl = mic_up_afifo_ctrl.d32;
}

/**
 * @brief  Set the value of @ref pdm_v151_mic_up_afifo_ctrl_data.mic5_upfifo_aempty_th.
 * @param  [in]  val The value of @ref pdm_v151_mic_up_afifo_ctrl_data.mic5_upfifo_aempty_th.
 */
static inline void hal_pdm_v151_mic_up_afifo_ctrl_set_mic5_upfifo_aempty_th(uint32_t val)
{
    pdm_v151_mic_up_afifo_ctrl_data_t mic_up_afifo_ctrl;
    mic_up_afifo_ctrl.d32 = ((pdm_v151_regs_t *)g_hal_pdm_regs)->mic_up_afifo_ctrl;
    mic_up_afifo_ctrl.b.mic5_upfifo_aempty_th = val;
    ((pdm_v151_regs_t *)g_hal_pdm_regs)->mic_up_afifo_ctrl = mic_up_afifo_ctrl.d32;
}

/**
 * @brief  Set the value of @ref pdm_v151_mic_up_afifo_ctrl_data.mic5_upfifo_afull_th.
 * @param  [in]  val The value of @ref pdm_v151_mic_up_afifo_ctrl_data.mic5_upfifo_afull_th.
 */
static inline void hal_pdm_v151_mic_up_afifo_ctrl_set_mic5_upfifo_afull_th(uint32_t val)
{
    pdm_v151_mic_up_afifo_ctrl_data_t mic_up_afifo_ctrl;
    mic_up_afifo_ctrl.d32 = ((pdm_v151_regs_t *)g_hal_pdm_regs)->mic_up_afifo_ctrl;
    mic_up_afifo_ctrl.b.mic5_upfifo_afull_th = val;
    ((pdm_v151_regs_t *)g_hal_pdm_regs)->mic_up_afifo_ctrl = mic_up_afifo_ctrl.d32;
}

/**
 * @brief  Set the value of @ref pdm_v151_mic_up_afifo_ctrl_data.mic5_up_fifo_clr.
 * @param  [in]  val The value of @ref pdm_v151_mic_up_afifo_ctrl_data.mic5_up_fifo_clr.
 */
static inline void hal_pdm_v151_mic_up_afifo_ctrl_set_mic5_up_fifo_clr(uint32_t val)
{
    pdm_v151_mic_up_afifo_ctrl_data_t mic_up_afifo_ctrl;
    mic_up_afifo_ctrl.d32 = ((pdm_v151_regs_t *)g_hal_pdm_regs)->mic_up_afifo_ctrl;
    mic_up_afifo_ctrl.b.mic5_up_fifo_clr = val;
    ((pdm_v151_regs_t *)g_hal_pdm_regs)->mic_up_afifo_ctrl = mic_up_afifo_ctrl.d32;
}

/**
 * @brief  Set the value of @ref pdm_v151_fs_ctrl0_data.fs_mic4_afifo.
 * @param  [in]  val The value of @ref pdm_v151_fs_ctrl0_data.fs_mic4_afifo.
 */
static inline void hal_pdm_v151_fs_ctrl0_set_fs_mic4_afifo(uint32_t val)
{
    pdm_v151_fs_ctrl0_data_t fs_ctrl0;
    fs_ctrl0.d32 = ((pdm_v151_regs_t *)g_hal_pdm_regs)->fs_ctrl0;
    fs_ctrl0.b.fs_mic4_afifo = val;
    ((pdm_v151_regs_t *)g_hal_pdm_regs)->fs_ctrl0 = fs_ctrl0.d32;
}

/**
 * @brief  Set the value of @ref pdm_v151_fs_ctrl0_data.fs_mic5_afifo.
 * @param  [in]  val The value of @ref pdm_v151_fs_ctrl0_data.fs_mic5_afifo.
 */
static inline void hal_pdm_v151_fs_ctrl0_set_fs_mic5_afifo(uint32_t val)
{
    pdm_v151_fs_ctrl0_data_t fs_ctrl0;
    fs_ctrl0.d32 = ((pdm_v151_regs_t *)g_hal_pdm_regs)->fs_ctrl0;
    fs_ctrl0.b.fs_mic5_afifo = val;
    ((pdm_v151_regs_t *)g_hal_pdm_regs)->fs_ctrl0 = fs_ctrl0.d32;
}

/**
 * @brief  Set the value of @ref pdm_v151_fs_ctrl1_data.fs_mic4_srcdn_dout.
 * @param  [in]  val The value of @ref pdm_v151_fs_ctrl1_data.fs_mic4_srcdn_dout.
 */
static inline void hal_pdm_v151_fs_ctrl1_set_fs_mic4_srcdn_dout(uint32_t val)
{
    pdm_v151_fs_ctrl1_data_t fs_ctrl1;
    fs_ctrl1.d32 = ((pdm_v151_regs_t *)g_hal_pdm_regs)->fs_ctrl1;
    fs_ctrl1.b.fs_mic4_srcdn_dout = val;
    ((pdm_v151_regs_t *)g_hal_pdm_regs)->fs_ctrl1 = fs_ctrl1.d32;
}

/**
 * @brief  Set the value of @ref pdm_v151_fs_ctrl1_data.fs_mic5_srcdn_dout.
 * @param  [in]  val The value of @ref pdm_v151_fs_ctrl1_data.fs_mic5_srcdn_dout.
 */
static inline void hal_pdm_v151_fs_ctrl1_set_fs_mic5_srcdn_dout(uint32_t val)
{
    pdm_v151_fs_ctrl1_data_t fs_ctrl1;
    fs_ctrl1.d32 = ((pdm_v151_regs_t *)g_hal_pdm_regs)->fs_ctrl1;
    fs_ctrl1.b.fs_mic5_srcdn_dout = val;
    ((pdm_v151_regs_t *)g_hal_pdm_regs)->fs_ctrl1 = fs_ctrl1.d32;
}

/**
 * @brief  Set the value of @ref pdm_v151_codec_adc_dc_offset_data.adc_dc_offset.
 * @param  [in]  mic The mic need to configure.
 * @param  [in]  val The value of @ref pdm_v151_codec_adc_dc_offset_data.adc_dc_offset.
 */
static inline void hal_pdm_v151_codec_adc_dc_offset_set_adc_dc_offset(uint8_t mic, uint32_t val)
{
    pdm_v151_codec_adc_dc_offset_data_t codec_adc_dc_offset;
    codec_adc_dc_offset.d32 =  (mic == PDM_V151_DMIC4) ? ((pdm_v151_regs_t *)g_hal_pdm_regs)->codec_adc4_dc_offset :
                                                         ((pdm_v151_regs_t *)g_hal_pdm_regs)->codec_adc5_dc_offset;
    codec_adc_dc_offset.b.adc_dc_offset = val;
    if (mic == PDM_V151_DMIC4) {
        ((pdm_v151_regs_t *)g_hal_pdm_regs)->codec_adc4_dc_offset = codec_adc_dc_offset.d32;
    } else {
        ((pdm_v151_regs_t *)g_hal_pdm_regs)->codec_adc5_dc_offset = codec_adc_dc_offset.d32;
    }
}

/**
 * @brief  Set the value of @ref pdm_v151_codec_adc_cic_gain_data.adc4_cic_gain.
 * @param  [in]  val The value of @ref pdm_v151_codec_adc_cic_gain_data.adc4_cic_gain.
 */
static inline void hal_pdm_v151_codec_adc_cic_gain_set_adc4_cic_gain(uint32_t val)
{
    pdm_v151_codec_adc_cic_gain_data_t codec_adc_cic_gain;
    codec_adc_cic_gain.d32 = ((pdm_v151_regs_t *)g_hal_pdm_regs)->codec_adc_cic_gain;
    codec_adc_cic_gain.b.adc4_cic_gain = val;
    ((pdm_v151_regs_t *)g_hal_pdm_regs)->codec_adc_cic_gain = codec_adc_cic_gain.d32;
}

/**
 * @brief  Set the value of @ref pdm_v151_codec_adc_cic_gain_data.adc5_cic_gain.
 * @param  [in]  val The value of @ref pdm_v151_codec_adc_cic_gain_data.adc5_cic_gain.
 */
static inline void hal_pdm_v151_codec_adc_cic_gain_set_adc5_cic_gain(uint32_t val)
{
    pdm_v151_codec_adc_cic_gain_data_t codec_adc_cic_gain;
    codec_adc_cic_gain.d32 = ((pdm_v151_regs_t *)g_hal_pdm_regs)->codec_adc_cic_gain;
    codec_adc_cic_gain.b.adc5_cic_gain = val;
    ((pdm_v151_regs_t *)g_hal_pdm_regs)->codec_adc_cic_gain = codec_adc_cic_gain.d32;
}

/**
 * @brief  Set the value of @ref pdm_v151_lp_cic_gain1_data.cicdn_adc4_gain.
 * @param  [in]  val The value of @ref pdm_v151_lp_cic_gain1_data.cicdn_adc4_gain.
 */
static inline void hal_pdm_v151_lp_cic_gain1_set_cicdn_adc4_gain(uint32_t val)
{
    pdm_v151_lp_cic_gain1_data_t lp_cic_gain1;
    lp_cic_gain1.d32 = ((pdm_v151_regs_t *)g_hal_pdm_regs)->lp_cic_gain1;
    lp_cic_gain1.b.cicdn_adc4_gain = val;
    ((pdm_v151_regs_t *)g_hal_pdm_regs)->lp_cic_gain1 = lp_cic_gain1.d32;
}

/**
 * @brief  Set the value of @ref pdm_v151_lp_cic_gain1_data.cicdn_adc5_gain.
 * @param  [in]  val The value of @ref pdm_v151_lp_cic_gain1_data.cicdn_adc5_gain.
 */
static inline void hal_pdm_v151_lp_cic_gain1_set_cicdn_adc5_gain(uint32_t val)
{
    pdm_v151_lp_cic_gain1_data_t lp_cic_gain1;
    lp_cic_gain1.d32 = ((pdm_v151_regs_t *)g_hal_pdm_regs)->lp_cic_gain1;
    lp_cic_gain1.b.cicdn_adc5_gain = val;
    ((pdm_v151_regs_t *)g_hal_pdm_regs)->lp_cic_gain1 = lp_cic_gain1.d32;
}

/**
 * @brief  Set the value of @ref pdm_v151_event_data.mic5_fifo_afull.
 * @param  [in]  val The value of @ref pdm_v151_event_data.mic5_fifo_afull.
 */
static inline void hal_pdm_v151_event_set_mic5_fifo_afull(uint32_t val)
{
    pdm_v151_event_clr_data_t event_clr;
    event_clr.d32 = ((pdm_v151_regs_t *)g_hal_pdm_regs)->event_clr;
    event_clr.b.mic5_fifo_afull = val;
    ((pdm_v151_regs_t *)g_hal_pdm_regs)->event_clr = event_clr.d32;
}

/**
 * @brief  Set the value of @ref pdm_v151_event_data.mic4_fifo_afull.
 * @param  [in]  val The value of @ref pdm_v151_event_data.mic4_fifo_afull.
 */
static inline void hal_pdm_v151_event_set_mic4_fifo_afull(uint32_t val)
{
    pdm_v151_event_clr_data_t event_clr;
    event_clr.d32 = ((pdm_v151_regs_t *)g_hal_pdm_regs)->event_clr;
    event_clr.b.mic4_fifo_afull = val;
    ((pdm_v151_regs_t *)g_hal_pdm_regs)->event_clr = event_clr.d32;
}

/**
 * @brief  Set the value of @ref pdm_v151_event_data.mic5_fifo_full.
 * @param  [in]  val The value of @ref pdm_v151_event_data.mic5_fifo_full.
 */
static inline void hal_pdm_v151_event_set_mic5_fifo_full(uint32_t val)
{
    pdm_v151_event_clr_data_t event_clr;
    event_clr.d32 = ((pdm_v151_regs_t *)g_hal_pdm_regs)->event_clr;
    event_clr.b.mic5_fifo_full = val;
    ((pdm_v151_regs_t *)g_hal_pdm_regs)->event_clr = event_clr.d32;
}

/**
 * @brief  Set the value of @ref pdm_v151_event_data.mic4_fifo_full.
 * @param  [in]  val The value of @ref pdm_v151_event_data.mic4_fifo_full.
 */
static inline void hal_pdm_v151_event_set_mic4_fifo_full(uint32_t val)
{
    pdm_v151_event_clr_data_t event_clr;
    event_clr.d32 = ((pdm_v151_regs_t *)g_hal_pdm_regs)->event_clr;
    event_clr.b.mic4_fifo_full = val;
    ((pdm_v151_regs_t *)g_hal_pdm_regs)->event_clr = event_clr.d32;
}

/**
 * @brief  Get the value of @ref pdm_v151_event_data.mic5_fifo_afull.
 * @return The value of @ref pdm_v151_event_data.mic5_fifo_afull.
 */
static inline uint32_t hal_pdm_v151_event_st_get_mic5_fifo_afull(void)
{
    pdm_v151_event_st_data_t event_st;
    event_st.d32 = ((pdm_v151_regs_t *)g_hal_pdm_regs)->event_st;
    return event_st.b.mic5_fifo_afull;
}

/**
 * @brief  Get the value of @ref pdm_v151_event_data.mic4_fifo_afull.
 * @return The value of @ref pdm_v151_event_data.mic4_fifo_afull.
 */
static inline uint32_t hal_pdm_v151_event_st_get_mic4_fifo_afull(void)
{
    pdm_v151_event_st_data_t event_st;
    event_st.d32 = ((pdm_v151_regs_t *)g_hal_pdm_regs)->event_st;
    return event_st.b.mic4_fifo_afull;
}

/**
 * @brief  Get the value of @ref pdm_v151_event_data.mic5_fifo_full.
 * @return The value of @ref pdm_v151_event_data.mic5_fifo_full.
 */
static inline uint32_t hal_pdm_v151_event_st_get_mic5_fifo_full(void)
{
    pdm_v151_event_st_data_t event_st;
    event_st.d32 = ((pdm_v151_regs_t *)g_hal_pdm_regs)->event_st;
    return event_st.b.mic5_fifo_full;
}

/**
 * @brief  Get the value of @ref pdm_v151_event_data.mic4_fifo_full.
 * @return The value of @ref pdm_v151_event_data.mic4_fifo_full.
 */
static inline uint32_t hal_pdm_v151_event_st_get_mic4_fifo_full(void)
{
    pdm_v151_event_st_data_t event_st;
    event_st.d32 = ((pdm_v151_regs_t *)g_hal_pdm_regs)->event_st;
    return event_st.b.mic4_fifo_full;
}

/**
 * @brief  Set the value of @ref pdm_v151_adc_filter_data.adc4_compd_bypass_en.
 * @param  [in]  val The value of @ref pdm_v151_adc_filter_data.adc4_compd_bypass_en.
 */
static inline void hal_pdm_v151_adc_filter_set_adc4_compd_bypass_en(uint32_t val)
{
    pdm_v151_adc_filter_data_t adc_filter;
    adc_filter.d32 = ((pdm_v151_regs_t *)g_hal_pdm_regs)->adc_filter;
    adc_filter.b.adc4_compd_bypass_en = val;
    ((pdm_v151_regs_t *)g_hal_pdm_regs)->adc_filter = adc_filter.d32;
}

/**
 * @brief  Set the value of @ref pdm_v151_adc_filter_data.adc4_fir2d_en.
 * @param  [in]  val The value of @ref pdm_v151_adc_filter_data.adc4_fir2d_en.
 */
static inline void hal_pdm_v151_adc_filter_set_adc4_fir2d_en(uint32_t val)
{
    pdm_v151_adc_filter_data_t adc_filter;
    adc_filter.d32 = ((pdm_v151_regs_t *)g_hal_pdm_regs)->adc_filter;
    adc_filter.b.adc4_fir2d_en = val;
    ((pdm_v151_regs_t *)g_hal_pdm_regs)->adc_filter = adc_filter.d32;
}

/**
 * @brief  Set the value of @ref pdm_v151_adc_filter_data.adc4_com2d_en.
 * @param  [in]  val The value of @ref pdm_v151_adc_filter_data.adc4_com2d_en.
 */
static inline void hal_pdm_v151_adc_filter_set_adc4_com2d_en(uint32_t val)
{
    pdm_v151_adc_filter_data_t adc_filter;
    adc_filter.d32 = ((pdm_v151_regs_t *)g_hal_pdm_regs)->adc_filter;
    adc_filter.b.adc4_com2d_en = val;
    ((pdm_v151_regs_t *)g_hal_pdm_regs)->adc_filter = adc_filter.d32;
}

/**
 * @brief  Set the value of @ref pdm_v151_adc_filter_data.adc4_hpf_en.
 * @param  [in]  val The value of @ref pdm_v151_adc_filter_data.adc4_hpf_en.
 */
static inline void hal_pdm_v151_adc_filter_set_adc4_hpf_en(uint32_t val)
{
    pdm_v151_adc_filter_data_t adc_filter;
    adc_filter.d32 = ((pdm_v151_regs_t *)g_hal_pdm_regs)->adc_filter;
    adc_filter.b.adc4_hpf_en = val;
    ((pdm_v151_regs_t *)g_hal_pdm_regs)->adc_filter = adc_filter.d32;
}

/**
 * @brief  Set the value of @ref pdm_v151_adc_filter_data.adc5_compd_bypass_en.
 * @param  [in]  val The value of @ref pdm_v151_adc_filter_data.adc5_compd_bypass_en.
 */
static inline void hal_pdm_v151_adc_filter_set_adc5_compd_bypass_en(uint32_t val)
{
    pdm_v151_adc_filter_data_t adc_filter;
    adc_filter.d32 = ((pdm_v151_regs_t *)g_hal_pdm_regs)->adc_filter;
    adc_filter.b.adc5_compd_bypass_en = val;
    ((pdm_v151_regs_t *)g_hal_pdm_regs)->adc_filter = adc_filter.d32;
}

/**
 * @brief  Set the value of @ref pdm_v151_adc_filter_data.adc5_fir2d_en.
 * @param  [in]  val The value of @ref pdm_v151_adc_filter_data.adc5_fir2d_en.
 */
static inline void hal_pdm_v151_adc_filter_set_adc5_fir2d_en(uint32_t val)
{
    pdm_v151_adc_filter_data_t adc_filter;
    adc_filter.d32 = ((pdm_v151_regs_t *)g_hal_pdm_regs)->adc_filter;
    adc_filter.b.adc5_fir2d_en = val;
    ((pdm_v151_regs_t *)g_hal_pdm_regs)->adc_filter = adc_filter.d32;
}

/**
 * @brief  Set the value of @ref pdm_v151_adc_filter_data.adc5_com2d_en.
 * @param  [in]  val The value of @ref pdm_v151_adc_filter_data.adc5_com2d_en.
 */
static inline void hal_pdm_v151_adc_filter_set_adc5_com2d_en(uint32_t val)
{
    pdm_v151_adc_filter_data_t adc_filter;
    adc_filter.d32 = ((pdm_v151_regs_t *)g_hal_pdm_regs)->adc_filter;
    adc_filter.b.adc5_com2d_en = val;
    ((pdm_v151_regs_t *)g_hal_pdm_regs)->adc_filter = adc_filter.d32;
}

/**
 * @brief  Set the value of @ref pdm_v151_adc_filter_data.adc5_hpf_en.
 * @param  [in]  val The value of @ref pdm_v151_adc_filter_data.adc5_hpf_en.
 */
static inline void hal_pdm_v151_adc_filter_set_adc5_hpf_en(uint32_t val)
{
    pdm_v151_adc_filter_data_t adc_filter;
    adc_filter.d32 = ((pdm_v151_regs_t *)g_hal_pdm_regs)->adc_filter;
    adc_filter.b.adc5_hpf_en = val;
    ((pdm_v151_regs_t *)g_hal_pdm_regs)->adc_filter = adc_filter.d32;
}

/**
 * @brief  Set the value of @ref pdm_v151_codec_clk_div0_data.clk_6144k_div_ratio.
 * @param  [in]  val The value of @ref pdm_v151_codec_clk_div0_data.clk_6144k_div_ratio.
 */
static inline void hal_pdm_v151_codec_clk_div0_set_clk_6144k_div_ratio(uint32_t val)
{
    pdm_v151_codec_clk_div0_data_t codec_clk_div0 = { 0 };
    codec_clk_div0.b.clk_6144k_div_ratio = val;
    ((pdm_v151_regs_t *)g_hal_pdm_regs)->codec_clk_div0 = codec_clk_div0.d32;
}

/**
 * @brief  Get the FIFO address of PDM.
 */
static inline uintptr_t hal_pdm_v151_fifo_get_addr(void)
{
    return g_hal_pdm_regs + HAL_PDM_V151_FIFO_OFFSET;
}

/**
 * @brief  Get the FIFO deepth of PDM.
 */
static inline uint32_t hal_pdm_v151_fifo_get_deepth(void)
{
    return HAL_PDM_V151_FIFO_DEEPTH;
}

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif