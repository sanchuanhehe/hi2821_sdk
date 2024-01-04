/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Provides PDM driver source \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-02-08, Create file. \n
 */
#include "common_def.h"
#include "pdm_porting.h"
#include "pdm.h"

static hal_pdm_funcs_t *g_hal_funcs = NULL;

errcode_t uapi_pdm_init(void)
{
#if defined(CONFIG_PDM_SUPPORT_LPC)
    pdm_port_clock_enable(true);
#endif
    pdm_port_register_hal_funcs();

    g_hal_funcs = hal_pdm_get_funcs();
    return g_hal_funcs->init();
}

void uapi_pdm_deinit(void)
{
    g_hal_funcs->deinit();
    g_hal_funcs = NULL;

    pdm_port_unregister_hal_funcs();
#if defined(CONFIG_PDM_SUPPORT_LPC)
    pdm_port_clock_enable(false);
#endif
}

errcode_t uapi_pdm_set_attr(uint8_t mic, const pdm_config_t *attr)
{
    if (unlikely(mic >= (uint8_t)HAL_PDM_MIC_MAX_NUM || attr == NULL)) {
        return ERRCODE_INVALID_PARAM;
    }

    hal_pdm_config_t hal_attr = {
#if defined(CONFIG_PDM_USING_V151)
        .fs_ctrl_freq           = (hal_pdm_fre_t)attr->fs_ctrl_freq,
        .linear_select          = (hal_pdm_mic_pag_linear_sel_t)attr->linear_select,
        .zero_num               = attr->zero_num,
        .threshold_id           = attr->threshold_id,
        .noise_enable           = attr->noise_enable,
        .pga_bypass_enable      = attr->pga_bypass_enable,
        .fade_out_time          = attr->fade_out_time,
        .fade_in_time           = attr->fade_in_time,
        .little_signal          = attr->little_signal,
        .anti_clip              = attr->anti_clip,
        .fade_in_out            = attr->fade_in_out,
        .pga_gain               = attr->pga_gain,
#endif  /* CONFIG_PDM_USING_V151 */
        .srcdn_src_mode         = (hal_pdm_srcdn_src_mode_t)attr->srcdn_src_mode
    };
    return g_hal_funcs->set_attr((uint8_t)mic, &hal_attr);
}

void uapi_pdm_get_attr(uint8_t mic, pdm_config_t *attr)
{
    if (unlikely(mic >= (uint8_t)HAL_PDM_MIC_MAX_NUM || attr == NULL)) {
        return;
    }

    hal_pdm_config_t hal_attr = { 0 };
    g_hal_funcs->get_attr((uint8_t)mic, &hal_attr);

#if defined(CONFIG_PDM_USING_V151)
    attr->fs_ctrl_freq           = (hal_pdm_fre_t)hal_attr.fs_ctrl_freq;
    attr->linear_select          = (hal_pdm_mic_pag_linear_sel_t)hal_attr.linear_select;
    attr->zero_num               = hal_attr.zero_num;
    attr->threshold_id           = hal_attr.threshold_id;
    attr->noise_enable           = hal_attr.noise_enable;
    attr->pga_bypass_enable      = hal_attr.pga_bypass_enable;
    attr->fade_out_time          = hal_attr.fade_out_time;
    attr->fade_in_time           = hal_attr.fade_in_time;
    attr->little_signal          = hal_attr.little_signal;
    attr->anti_clip              = hal_attr.anti_clip;
    attr->fade_in_out            = hal_attr.fade_in_out;
    attr->pga_gain               = hal_attr.pga_gain;
#endif  /* CONFIG_PDM_USING_V151 */
    attr->srcdn_src_mode         = (hal_pdm_srcdn_src_mode_t)hal_attr.srcdn_src_mode;
}

errcode_t uapi_pdm_start(void)
{
    return g_hal_funcs->start();
}

errcode_t uapi_pdm_stop(void)
{
    return g_hal_funcs->stop();
}

uintptr_t uapi_pdm_get_fifo_addr(void)
{
    return g_hal_funcs->get_fifo_addr();
}

uint32_t uapi_pdm_get_fifo_deepth(void)
{
    return g_hal_funcs->get_fifo_deepth();
}