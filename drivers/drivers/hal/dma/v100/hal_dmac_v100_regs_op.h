/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides V100 dma register operation api \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-10-16， Create file. \n
 */

#ifndef HAL_DMAC_V100_REGS_OP_H
#define HAL_DMAC_V100_REGS_OP_H

#include <stdint.h>
#include <stdbool.h>
#include "common_def.h"
#include "errcode.h"
#include "hal_dmac_v100_regs_def.h"
#include "dma_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_dma_v100_regs_op DMA V100 Regs Operation
 * @ingroup  drivers_hal_dma
 * @{
 */

extern uintptr_t g_dma_regs;
extern uintptr_t g_sdma_regs;

/**
 * @brief  Set the value of @ref dma_cfg_reg_data.dma_enable.
 * @param  [in]  val The value of @ref dma_cfg_reg_data.dma_enable.
 * @param  [out] dma_regs The DMA Registers address.
 */
static inline void hal_dma_cfg_reg_set_en(uint32_t val, dma_regs_t *dma_regs)
{
    dma_cfg_reg_data_t dma_cfg_reg;
    dma_cfg_reg.d32 = dma_regs->dma_cfg_reg;
    dma_cfg_reg.b.dma_enable = val;
    dma_regs->dma_cfg_reg = dma_cfg_reg.d32;
}

/**
 * @brief  Get the value of @ref dma_ctrl_h_data.block_ts.
 * @param  [in]  channel The DMA channel.
 * @param  [in]  dma_regs The DMA Registers address.
 * @return The value of @ref dma_ctrl_h_data.block_ts.
 */
static inline uint32_t hal_dma_ctrl_h_get_block_ts(dma_channel_t channel, const dma_regs_t *dma_regs)
{
    dma_ctrl_h_data_t dma_ctrl_h;
    dma_ctrl_h.d32 = dma_regs->dma_ch_config[channel].ctl_h;
    return dma_ctrl_h.b.block_ts;
}

/**
 * @brief  Set the value of @ref dma_ctrl_h_data.block_ts.
 * @param  [in]  channel The DMA channel.
 * @param  [in]  val The value of @ref dma_ctrl_h_data.block_ts.
 * @param  [out] dma_regs The DMA Registers address.
 */
static inline void hal_dma_ctrl_h_set_block_ts(dma_channel_t channel, uint32_t val, dma_regs_t *dma_regs)
{
    dma_ctrl_h_data_t dma_ctrl_h;
    dma_ctrl_h.d32 = dma_regs->dma_ch_config[channel].ctl_h;
    dma_ctrl_h.b.block_ts = val;
    dma_regs->dma_ch_config[channel].ctl_h = dma_ctrl_h.d32;
}

/**
 * @brief  Set the value of @ref dma_ctrl_l_data.dst_tr_width.
 * @param  [in]  channel The DMA channel.
 * @param  [in]  val The value of @ref dma_ctrl_l_data.dst_tr_width.
 * @param  [out] dma_regs The DMA Registers address.
 */
static inline void hal_dma_ctrl_l_set_dst_tr_width(dma_channel_t channel, uint32_t val, dma_regs_t *dma_regs)
{
    dma_ctrl_l_data_t dma_ctrl_l;
    dma_ctrl_l.d32 = dma_regs->dma_ch_config[channel].ctl_l;
    dma_ctrl_l.b.dst_tr_width = val;
    dma_regs->dma_ch_config[channel].ctl_l = dma_ctrl_l.d32;
}

/**
 * @brief  Set the value of @ref dma_ctrl_l_data.src_tr_width.
 * @param  [in]  channel The DMA channel.
 * @param  [in]  val The value of @ref dma_ctrl_l_data.src_tr_width.
 * @param  [out] dma_regs The DMA Registers address.
 */
static inline void hal_dma_ctrl_l_set_src_tr_width(dma_channel_t channel, uint32_t val, dma_regs_t *dma_regs)
{
    dma_ctrl_l_data_t dma_ctrl_l;
    dma_ctrl_l.d32 = dma_regs->dma_ch_config[channel].ctl_l;
    dma_ctrl_l.b.src_tr_width = val;
    dma_regs->dma_ch_config[channel].ctl_l = dma_ctrl_l.d32;
}

/**
 * @brief  Set the value of @ref dma_ctrl_l_data.dinc.
 * @param  [in]  channel The DMA channel.
 * @param  [in]  val The value of @ref dma_ctrl_l_data.dinc.
 * @param  [out] dma_regs The DMA Registers address.
 */
static inline void hal_dma_ctrl_l_set_dinc(dma_channel_t channel, uint32_t val, dma_regs_t *dma_regs)
{
    dma_ctrl_l_data_t dma_ctrl_l;
    dma_ctrl_l.d32 = dma_regs->dma_ch_config[channel].ctl_l;
    dma_ctrl_l.b.dinc = val;
    dma_regs->dma_ch_config[channel].ctl_l = dma_ctrl_l.d32;
}

/**
 * @brief  Set the value of.
 * @param  [in]  channel The DMA channel.
 * @param  [in]  val The value of @ref dma_ctrl_l_data.sinc.
 * @param  [out] dma_regs The DMA Registers address.
 */
static inline void hal_dma_ctrl_l_set_sinc(dma_channel_t channel, uint32_t val, dma_regs_t *dma_regs)
{
    dma_ctrl_l_data_t dma_ctrl_l;
    dma_ctrl_l.d32 = dma_regs->dma_ch_config[channel].ctl_l;
    dma_ctrl_l.b.sinc = val;
    dma_regs->dma_ch_config[channel].ctl_l = dma_ctrl_l.d32;
}

/**
 * @brief  Set the value of @ref dma_ctrl_l_data.src_msize.
 * @param  [in]  channel The DMA channel.
 * @param  [in]  val The value of @ref dma_ctrl_l_data.src_msize.
 * @param  [out] dma_regs The DMA Registers address.
 */
static inline void hal_dma_ctrl_l_set_src_msize(dma_channel_t channel, uint32_t val, dma_regs_t *dma_regs)
{
    dma_ctrl_l_data_t dma_ctrl_l;
    dma_ctrl_l.d32 = dma_regs->dma_ch_config[channel].ctl_l;
    dma_ctrl_l.b.src_msize = val;
    dma_regs->dma_ch_config[channel].ctl_l = dma_ctrl_l.d32;
}

/**
 * @brief  Set the value of @ref dma_ctrl_l_data.dest_msize.
 * @param  [in]  channel The DMA channel.
 * @param  [in]  val The value of @ref dma_ctrl_l_data.dest_msize.
 * @param  [out] dma_regs The DMA Registers address.
 */
static inline void hal_dma_ctrl_l_set_dest_msize(dma_channel_t channel, uint32_t val, dma_regs_t *dma_regs)
{
    dma_ctrl_l_data_t dma_ctrl_l;
    dma_ctrl_l.d32 = dma_regs->dma_ch_config[channel].ctl_l;
    dma_ctrl_l.b.dest_msize = val;
    dma_regs->dma_ch_config[channel].ctl_l = dma_ctrl_l.d32;
}

/**
 * @brief  Set the value of @ref dma_cfg_l_data.ch_prior.
 * @param  [in]  channel The DMA channel.
 * @param  [in]  val The value of @ref dma_cfg_l_data.ch_prior.
 * @param  [out] dma_regs The DMA Registers address.
 */
static inline void hal_dma_cfg_l_set_ch_prior(dma_channel_t channel, uint32_t val, dma_regs_t *dma_regs)
{
    dma_cfg_l_data_t dma_cfg_l;
    dma_cfg_l.d32 = dma_regs->dma_ch_config[channel].cfg_l;
    dma_cfg_l.b.ch_prior = val;
    dma_regs->dma_ch_config[channel].cfg_l = dma_cfg_l.d32;
}

/**
 * @brief  Set the value of @ref dma_ctrl_l_data.dms.
 * @param  [in]  channel The DMA channel.
 * @param  [in]  val The value of @ref dma_ctrl_l_data.dms.
 * @param  [out] dma_regs The DMA Registers address.
 */
static inline void hal_dma_ctrl_l_set_dms(dma_channel_t channel, uint32_t val, dma_regs_t *dma_regs)
{
    dma_ctrl_l_data_t dma_ctrl_l;
    dma_ctrl_l.d32 = dma_regs->dma_ch_config[channel].ctl_l;
    dma_ctrl_l.b.dms = val;
    dma_regs->dma_ch_config[channel].ctl_l = dma_ctrl_l.d32;
}

/**
 * @brief  Set the value of @ref dma_ctrl_l_data.sms.
 * @param  [in]  channel The DMA channel.
 * @param  [in]  val The value of @ref dma_ctrl_l_data.sms.
 * @param  [out] dma_regs The DMA Registers address.
 */
static inline void hal_dma_ctrl_l_set_sms(dma_channel_t channel, uint32_t val, dma_regs_t *dma_regs)
{
    dma_ctrl_l_data_t dma_ctrl_l;
    dma_ctrl_l.d32 = dma_regs->dma_ch_config[channel].ctl_l;
    dma_ctrl_l.b.sms = val;
    dma_regs->dma_ch_config[channel].ctl_l = dma_ctrl_l.d32;
}

/**
 * @brief  Set the value of @ref dma_ctrl_l_data.int_en.
 * @param  [in]  channel The DMA channel.
 * @param  [in]  val The value of @ref dma_ctrl_l_data.int_en.
 * @param  [out] dma_regs The DMA Registers address.
 */
static inline void hal_dma_ctrl_l_set_int_en(dma_channel_t channel, uint32_t val, dma_regs_t *dma_regs)
{
    dma_ctrl_l_data_t dma_ctrl_l;
    dma_ctrl_l.d32 = dma_regs->dma_ch_config[channel].ctl_l;
    dma_ctrl_l.b.int_en = val;
    dma_regs->dma_ch_config[channel].ctl_l = dma_ctrl_l.d32;
}

/**
 * @brief  Set the value of @ref dma_ctrl_l_data.tt_fc.
 * @param  [in]  channel The DMA channel.
 * @param  [in]  val The value of @ref dma_ctrl_l_data.tt_fc.
 * @param  [out] dma_regs The DMA Registers address.
 */
static inline void hal_dma_ctrl_l_set_tt_fc(dma_channel_t channel, uint32_t val, dma_regs_t *dma_regs)
{
    dma_ctrl_l_data_t dma_ctrl_l;
    dma_ctrl_l.d32 = dma_regs->dma_ch_config[channel].ctl_l;
    dma_ctrl_l.b.tt_fc = val;
    dma_regs->dma_ch_config[channel].ctl_l = dma_ctrl_l.d32;
}

/**
 * @brief  Set the value of @ref dma_cfg_l_data.hs_sel_src.
 * @param  [in]  channel The DMA channel.
 * @param  [in]  val The value of @ref dma_cfg_l_data.hs_sel_src.
 * @param  [out] dma_regs The DMA Registers address.
 */
static inline void hal_dma_cfg_l_set_hs_sel_src(dma_channel_t channel, uint32_t val, dma_regs_t *dma_regs)
{
    dma_cfg_l_data_t dma_cfg_l;
    dma_cfg_l.d32 = dma_regs->dma_ch_config[channel].cfg_l;
    dma_cfg_l.b.hs_sel_src = val;
    dma_regs->dma_ch_config[channel].cfg_l = dma_cfg_l.d32;
}

/**
 * @brief  Set the value of @ref dma_cfg_l_data.hs_sel_dst.
 * @param  [in]  channel The DMA channel.
 * @param  [in]  val The value of @ref dma_cfg_l_data.hs_sel_dst.
 * @param  [out] dma_regs The DMA Registers address.
 */
static inline void hal_dma_cfg_l_set_hs_sel_dst(dma_channel_t channel, uint32_t val, dma_regs_t *dma_regs)
{
    dma_cfg_l_data_t dma_cfg_l;
    dma_cfg_l.d32 = dma_regs->dma_ch_config[channel].cfg_l;
    dma_cfg_l.b.hs_sel_dst = val;
    dma_regs->dma_ch_config[channel].cfg_l = dma_cfg_l.d32;
}

/**
 * @brief  Set the value of @ref dma_cfg_h_data.dest_per.
 * @param  [in]  channel The DMA channel.
 * @param  [in]  val The value of @ref dma_cfg_h_data.dest_per.
 * @param  [out] dma_regs The DMA Registers address.
 */
static inline void hal_dma_cfg_h_set_dest_per(dma_channel_t channel, uint32_t val, dma_regs_t *dma_regs)
{
    dma_cfg_h_data_t dma_cfg_h;
    dma_cfg_h.d32 = dma_regs->dma_ch_config[channel].cfg_h;
    dma_cfg_h.b.dest_per = val;
    dma_regs->dma_ch_config[channel].cfg_h = dma_cfg_h.d32;
}

/**
 * @brief  Get the value of @ref dma_cfg_h_data.dest_per.
 * @param  [in]  channel The DMA channel.
 * @param  [in]  dma_regs The DMA Registers address.
 * @return The value of @ref dma_cfg_h_data.dest_per.
 */
static inline uint32_t hal_dma_cfg_h_get_dest_per(dma_channel_t channel, const dma_regs_t *dma_regs)
{
    dma_cfg_h_data_t dma_cfg_h;
    dma_cfg_h.d32 = dma_regs->dma_ch_config[channel].cfg_h;
    return dma_cfg_h.b.dest_per;
}

/**
 * @brief  Set the value of @ref dma_cfg_h_data.src_per.
 * @param  [in]  channel The DMA channel.
 * @param  [in]  val The value of @ref dma_cfg_h_data.src_per.
 * @param  [out] dma_regs The DMA Registers address.
 */
static inline void hal_dma_cfg_h_set_src_per(dma_channel_t channel, uint32_t val, dma_regs_t *dma_regs)
{
    dma_cfg_h_data_t dma_cfg_h;
    dma_cfg_h.d32 = dma_regs->dma_ch_config[channel].cfg_h;
    dma_cfg_h.b.src_per = val;
    dma_regs->dma_ch_config[channel].cfg_h = dma_cfg_h.d32;
}

/**
 * @brief  Get the value of @ref dma_cfg_h_data.src_per.
 * @param  [in]  channel The DMA channel.
 * @param  [in]  dma_regs The DMA Registers address.
 * @return The value of @ref dma_cfg_h_data.src_per.
 */
static inline uint32_t hal_dma_cfg_h_get_src_per(dma_channel_t channel, const dma_regs_t *dma_regs)
{
    dma_cfg_h_data_t dma_cfg_h;
    dma_cfg_h.d32 = dma_regs->dma_ch_config[channel].cfg_h;
    return dma_cfg_h.b.src_per;
}

/**
 * @brief  Set the value of @ref dma_cfg_h_data.protctl.
 * @param  [in]  channel The DMA channel.
 * @param  [in]  val The value of @ref dma_cfg_h_data.protctl.
 * @param  [out] dma_regs The DMA Registers address.
 */
static inline void hal_dma_cfg_h_set_protctl(dma_channel_t channel, uint32_t val, dma_regs_t *dma_regs)
{
    dma_cfg_h_data_t dma_cfg_h;
    dma_cfg_h.d32 = dma_regs->dma_ch_config[channel].cfg_h;
    dma_cfg_h.b.protctl = val;
    dma_regs->dma_ch_config[channel].cfg_h = dma_cfg_h.d32;
}

/**
 * @brief  Get the value of @ref dma_int_status_data_t.tfr.
 * @param  [in]  dma_regs The DMA Registers address.
 * @return The value of @ref dma_int_status_data_t.tfr.
 */
static inline bool hal_dma_status_data_get_tfr(const dma_regs_t *dma_regs)
{
    dma_int_status_data_t status_data;
    status_data.d32 = dma_regs->dma_sts_int;
    return (bool)status_data.b.tfr;
}

/**
 * @brief  Get the value of @ref dma_int_status_data_t.block.
 * @param  [in]  dma_regs The DMA Registers address.
 * @return The value of @ref dma_int_status_data_t.block.
 */
static inline bool hal_dma_status_data_get_block(const dma_regs_t *dma_regs)
{
    dma_int_status_data_t status_data;
    status_data.d32 = dma_regs->dma_sts_int;
    return (bool)status_data.b.block;
}

/**
 * @brief  Get the value of @ref dma_int_status_data_t.srct.
 * @param  [in]  dma_regs The DMA Registers address.
 * @return The value of @ref dma_int_status_data_t.srct.
 */
static inline bool hal_dma_status_data_get_srct(const dma_regs_t *dma_regs)
{
    dma_int_status_data_t status_data;
    status_data.d32 = dma_regs->dma_sts_int;
    return (bool)status_data.b.srct;
}

/**
 * @brief  Get the value of @ref dma_int_status_data_t.dstt.
 * @param  [in]  dma_regs The DMA Registers address.
 * @return (bool)The value of @ref dma_int_status_data_t.dstt.
 */
static inline bool hal_dma_status_data_get_dstt(const dma_regs_t *dma_regs)
{
    dma_int_status_data_t status_data;
    status_data.d32 = dma_regs->dma_sts_int;
    return (bool)status_data.b.dstt;
}

/**
 * @brief  Get the value of @ref dma_int_status_data_t.err.
 * @param  [in]  dma_regs The DMA Registers address.
 * @return The value of @ref dma_int_status_data_t.err.
 */
static inline bool hal_dma_status_data_get_err(const dma_regs_t *dma_regs)
{
    dma_int_status_data_t status_data;
    status_data.d32 = dma_regs->dma_sts_int;
    return (bool)status_data.b.err;
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