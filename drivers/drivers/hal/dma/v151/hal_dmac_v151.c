/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Provides V151 HAL dma \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-2-25， Create file. \n
 */
#include "securec.h"
#include "hal_dmac_v151_regs_op.h"
#include "hal_dmac_v151.h"
#ifdef CONFIG_SUPPORT_DATA_CACHE
#include "osal_adapt.h"
#endif
#include "hal_dma_mem.h"

#define HAL_DMA_CH_MAX_TRANSFER_NUM 4096

static void hal_dmac_v151_close(void);
static bool hal_dmac_v151_ch_is_enabled(dma_channel_t ch);

/* DMA ch informations */
typedef struct hal_dma_ch_info {
    hal_dma_ch_state_t state;                    /* ch transferring state */
    hal_dma_transfer_cb_t isr_callback;          /* ch ISR callback functions */
    uintptr_t arg;                               /* ch user arg */
} hal_dma_ch_info_t;

/* DMA transfer link list buffer. */
typedef struct hal_dma_lli {
    uint32_t src_addr;
    uint32_t dst_addr;
    struct hal_dma_lli *next;
    dma_ctrl_data_t ctrl;
} hal_dma_lli_t;

static hal_dma_ch_info_t g_hal_dma_channel[DMA_CHANNEL_MAX_NUM];

static hal_dma_lli_t *g_dma_node_cfg[DMA_CHANNEL_MAX_NUM] = { NULL };
static hal_dma_lli_t *g_dma_lli_add[DMA_CHANNEL_MAX_NUM] = { NULL };

/* Specific configuration to enable the DMA controller. */
static void hal_dma_enable_controller(void)
{
    hal_dma_cfg_set_en((uint32_t)1);
}

/* Specific configuration to disable the DMA controller. */
static void hal_dma_disable_controller(void)
{
    hal_dma_cfg_set_en((uint32_t)0);
}

static void hal_dma_reset(void)
{
    for (uint8_t ch = DMA_CHANNEL_0; ch < B_DMA_CHANNEL_MAX_NUM; ch++) {
        hal_dma_ch_cfg_set_ch_en(ch, (uint32_t)0);
        g_hal_dma_channel[ch].state = HAL_DMA_CH_STATE_CLOSED;
    }
    hal_dma_disable_controller();
    hal_dma_enable_controller();
}

static errcode_t hal_dmac_v151_init(void)
{
    if (hal_dma_regs_init() != ERRCODE_SUCC) {
        return ERRCODE_DMA_REG_ADDR_INVALID;
    }

    hal_dma_reset();
    return hal_dma_mem_init(sizeof(hal_dma_lli_t));
}

static void hal_dmac_v151_deinit(void)
{
    hal_dmac_v151_close();
    hal_dma_regs_deinit();
    hal_dma_mem_deinit();
}

static void hal_dmac_v151_open(void)
{
    hal_dma_enable_controller();

    for (uint8_t ch = DMA_CHANNEL_0; ch < DMA_CHANNEL_MAX_NUM; ch++) {
        g_hal_dma_channel[ch].state = HAL_DMA_CH_STATE_CLOSED;
    }
}

static void hal_dmac_v151_close(void)
{
    hal_dma_disable_controller();
}

static void hal_dma_lli_free(dma_channel_t ch)
{
    hal_dma_lli_t *dma_p = g_dma_node_cfg[ch];
    hal_dma_lli_t *dma_q;
    while (dma_p != NULL) {
        dma_q = dma_p->next;
        hal_dma_mem_free(ch, dma_p);
        dma_p = dma_q;
    }
    g_dma_node_cfg[ch] = NULL;
}

static void hal_dmac_v151_ch_enable(dma_channel_t ch, bool en)
{
    if (en) {
#ifdef CONFIG_SUPPORT_DATA_CACHE
        osal_dcache_flush_all();
#endif
        dma_port_add_sleep_veto();
        hal_dma_ch_cfg_set_ch_en(ch, (uint32_t)1);
    } else {
        hal_dma_lli_free(ch);
        hal_dma_ch_cfg_set_ch_en(ch, (uint32_t)0);
    }
}

static uint32_t hal_dmac_v151_get_tranf_size(dma_channel_t ch)
{
    return hal_dma_ctrl_get_tranf_size(ch);
}

static void hal_dmac_v151_interrupt_clear(dma_channel_t ch, hal_dma_interrupt_t int_type)
{
    if (int_type == HAL_DMA_INTERRUPT_TFR) {
        hal_dma_interrupt_clear_tc(ch);
    } else {
        hal_dma_interrupt_clear_err(ch);
    }
}

static dma_channel_t hal_dmac_v151_ch_get_idle(hal_dma_handshaking_source_t source,
                                               hal_dma_burst_transaction_length_t burst_length)
{
    for (uint8_t ch = DMA_CHANNEL_0; ch < DMA_CHANNEL_MAX_NUM; ch++) {
        if (!hal_dmac_v151_ch_is_enabled((dma_channel_t)ch) &&
            (g_hal_dma_channel[ch].state == HAL_DMA_CH_STATE_CLOSED)) {
            g_hal_dma_channel[ch].state = HAL_DMA_CH_STATE_ACTIVE;
            return (dma_channel_t)ch;
        }
    }
    unused(burst_length);
    unused(source);

    return DMA_CHANNEL_NONE;
}

static errcode_t hal_dma_config_periph(dma_channel_t ch, hal_dma_transfer_peri_config_t *periph_cfg)
{
    if (dma_port_set_mux_channel(ch, periph_cfg) != ERRCODE_SUCC) {
        return ERRCODE_DMA_RET_HANDSHAKING_USING;
    }
    hal_dma_cfg_set_src_per(ch, (uint32_t)periph_cfg->hs_source);
    hal_dma_cfg_set_dest_per(ch, (uint32_t)periph_cfg->hs_dest);
    hal_dma_ctrl_set_protection(ch, (uint32_t)periph_cfg->protection);
    return ERRCODE_SUCC;
}

/* Convert address increment according to the following rules:
 *  increment : 1
 *  others : 0
 */
static inline uint32_t hal_dma_inc_convert(hal_dma_address_inc_t inc_data)
{
    if (inc_data == HAL_DMA_ADDRESS_INC_INCREMENT) {
        return 1;
    } else {
        return 0;
    }
}

static void hal_dma_set_base_cfg_single_block(dma_channel_t ch, const hal_dma_transfer_base_config_t *base_cfg)
{
    /* Transfer src addr and dest addr */
    ((dma_v151_regs_t *)g_dma_regs)->ch_config[ch].src = base_cfg->src;
    ((dma_v151_regs_t *)g_dma_regs)->ch_config[ch].dest = base_cfg->dest;
    /* Transfer width */
    hal_dma_ctrl_set_src_tr_width(ch, (uint32_t)base_cfg->src_width);
    hal_dma_ctrl_set_dst_tr_width(ch, (uint32_t)base_cfg->dest_width);

    /* Address increment mode */
    hal_dma_ctrl_set_dest_inc(ch, hal_dma_inc_convert(base_cfg->dest_inc));
    hal_dma_ctrl_set_src_inc(ch, hal_dma_inc_convert(base_cfg->src_inc));

    /* Burst Transaction Length */
    hal_dma_ctrl_set_dest_bsize(ch, (uint32_t)base_cfg->dest_burst_trans_length);
    hal_dma_ctrl_set_src_bsize(ch, (uint32_t)base_cfg->src_burst_trans_length);

    hal_dma_cfg_set_lock(ch, (uint32_t)1);
}

static errcode_t hal_dmac_v151_config_single_block(dma_channel_t ch,
                                                   const hal_dma_transfer_base_config_t *base_cfg,
                                                   hal_dma_transfer_peri_config_t *periph_cfg)
{
    if (base_cfg->transfer_num >= HAL_DMA_CH_MAX_TRANSFER_NUM) {
        return ERRCODE_DMA_RET_TOO_MANY_DATA_TO_TRANSFER;
    }

    if (hal_dmac_v151_ch_is_enabled(ch)) {
        return ERRCODE_DMA_CH_BUSY;
    }
    hal_dma_set_base_cfg_single_block(ch, base_cfg);

    /* master: only used in mdma/smdma */
    hal_dma_ctrl_set_dest_ms_sel(ch, (uint32_t)dma_port_get_master_select(ch, base_cfg->dest));
    hal_dma_ctrl_set_src_ms_sel(ch, (uint32_t)dma_port_get_master_select(ch, base_cfg->src));

    if (base_cfg->callback != NULL) {
        hal_dma_ctrl_set_tc_int_en(ch, (uint32_t)1);
    }

    if (periph_cfg != NULL) {
        hal_dma_cfg_set_fc_tt(ch, (uint32_t)periph_cfg->trans_type);
        if (hal_dma_config_periph(ch, periph_cfg) != ERRCODE_SUCC) {
            return ERRCODE_DMA_RET_HANDSHAKING_USING;
        }
    } else {
        hal_dma_cfg_set_fc_tt(ch, (uint32_t)HAL_DMA_TRANS_MEMORY_TO_MEMORY_DMA);
    }

    /* Register callback or clear it */
    g_hal_dma_channel[ch].isr_callback = base_cfg->callback;
    g_hal_dma_channel[ch].arg = base_cfg->priv_arg;

    hal_dma_cfg_set_int_err_mask(ch, (uint32_t)1);
    hal_dma_cfg_set_tc_int_mask(ch, (uint32_t)1);

    /* Transfer length */
    hal_dma_ctrl_set_tranf_size(ch, (uint32_t)base_cfg->transfer_num);

    return ERRCODE_SUCC;
}

static void hal_dma_set_base_cfg_lli(dma_channel_t ch, const hal_dma_transfer_base_config_t *base_cfg)
{
    g_dma_lli_add[ch]->src_addr = base_cfg->src;
    g_dma_lli_add[ch]->dst_addr = base_cfg->dest;

    g_dma_lli_add[ch]->ctrl.b.dwsize = base_cfg->dest_width;
    g_dma_lli_add[ch]->ctrl.b.swsize = base_cfg->src_width;

    /* Address increment mode. */
    g_dma_lli_add[ch]->ctrl.b.dest_inc = hal_dma_inc_convert(base_cfg->dest_inc);
    g_dma_lli_add[ch]->ctrl.b.src_inc = hal_dma_inc_convert(base_cfg->src_inc);

    /* Burst Transaction Length. */
    g_dma_lli_add[ch]->ctrl.b.dbsize = base_cfg->dest_burst_trans_length;
    g_dma_lli_add[ch]->ctrl.b.sbsize = base_cfg->src_burst_trans_length;

    /* Master select. */
    g_dma_lli_add[ch]->ctrl.b.dest_ms_sel = (uint32_t)dma_port_get_master_select(ch, base_cfg->dest);
    g_dma_lli_add[ch]->ctrl.b.src_ms_sel = (uint32_t)dma_port_get_master_select(ch, base_cfg->src);

    /* Transfer length. */
    g_dma_lli_add[ch]->ctrl.b.transfersize = base_cfg->transfer_num;
}

static errcode_t hal_dma_lli_get(dma_channel_t ch)
{
    if (g_dma_node_cfg[ch] == NULL) {
        g_dma_node_cfg[ch] = (hal_dma_lli_t *)hal_dma_mem_alloc(ch, sizeof(hal_dma_lli_t));
        if (g_dma_node_cfg[ch] == NULL) {
            return ERRCODE_MALLOC;
        }
        g_dma_lli_add[ch] = g_dma_node_cfg[ch];
        g_dma_lli_add[ch]->next = NULL;
    } else {
        /* llp enable */
        g_dma_lli_add[ch]->next = (hal_dma_lli_t *)hal_dma_mem_alloc(ch, sizeof(hal_dma_lli_t));
        if (g_dma_lli_add[ch]->next == NULL) {
            return ERRCODE_MALLOC;
        }
        g_dma_lli_add[ch] = g_dma_lli_add[ch]->next;
        g_dma_lli_add[ch]->next = NULL;
    }
    return ERRCODE_SUCC;
}

static errcode_t hal_dmac_v151_add_lli_transfer(dma_channel_t ch,
                                                const hal_dma_transfer_base_config_t *base_cfg,
                                                hal_dma_transfer_peri_config_t *periph_cfg)
{
    bool dma_flag = false;
    if (ch >= DMA_CHANNEL_MAX_NUM) { return ERRCODE_DMA_INVALID_PARAMETER; }

    if (g_dma_node_cfg[ch] == NULL) {
        dma_flag = true;
    }

    if (hal_dma_lli_get(ch) != ERRCODE_SUCC) {
        return ERRCODE_MALLOC;
    }
    (void)memset_s((void *)g_dma_lli_add[ch], sizeof(hal_dma_lli_t), 0, sizeof(hal_dma_lli_t));

    hal_dma_set_base_cfg_lli(ch, base_cfg);

    if (dma_flag) {
        if (periph_cfg != NULL) {
            hal_dma_cfg_set_fc_tt(ch, (uint32_t)periph_cfg->trans_type);
            if (hal_dma_config_periph(ch, periph_cfg) != ERRCODE_SUCC) {
                return ERRCODE_DMA_RET_HANDSHAKING_USING;
            }
        } else {
            hal_dma_cfg_set_fc_tt(ch, (uint32_t)HAL_DMA_TRANS_MEMORY_TO_MEMORY_DMA);
        }
        hal_dma_cfg_set_int_err_mask(ch, (uint32_t)1);
        hal_dma_cfg_set_tc_int_mask(ch, (uint32_t)1);
        hal_dma_cfg_set_lock(ch, (uint32_t)1);
    }

    return ERRCODE_SUCC;
}

static bool hal_dmac_v151_ch_is_enabled(dma_channel_t ch)
{
    return hal_dma_regs_data_get_ch_en(ch);
}

static void hal_dmac_v151_enable_lli(dma_channel_t ch, hal_dma_transfer_cb_t callback, uintptr_t arg)
{
    if (g_dma_node_cfg[ch] == NULL) { return; }
     /* Interrupt enable. */
    g_dma_lli_add[ch]->ctrl.b.tc_int_en = 1;
    ((dma_v151_regs_t *)g_dma_regs)->ch_config[ch].src = g_dma_node_cfg[ch]->src_addr;
    ((dma_v151_regs_t *)g_dma_regs)->ch_config[ch].dest = g_dma_node_cfg[ch]->dst_addr;

    ((dma_v151_regs_t *)g_dma_regs)->ch_config[ch].ctrl = g_dma_node_cfg[ch]->ctrl;

    ((dma_v151_regs_t *)g_dma_regs)->ch_config[ch].lli.d32 = (uint32_t)(uintptr_t)(g_dma_node_cfg[ch]->next);

    g_hal_dma_channel[ch].isr_callback = callback;
    g_hal_dma_channel[ch].arg = arg;

    hal_dmac_v151_ch_enable(ch, true);
}

/* Transfer Complete Interrupt. */
static void hal_dma_tc_isr(void)
{
    for (uint8_t ch = DMA_CHANNEL_0; ch < DMA_CHANNEL_MAX_NUM; ch++) {
        if (hal_dma_regs_data_get_int_tc_st(ch)) {
            hal_dma_interrupt_clear_tc(ch);
            dma_port_release_handshaking_source(ch);
            g_hal_dma_channel[ch].state = HAL_DMA_CH_STATE_CLOSED;
            if (g_hal_dma_channel[ch].isr_callback != NULL) {
                g_hal_dma_channel[ch].isr_callback(HAL_DMA_INTERRUPT_TFR, ch, g_hal_dma_channel[ch].arg);
            }
        }
    }
}

/* DMA Error Interrupt. */
static void hal_dma_err_isr(void)
{
    for (uint8_t ch = DMA_CHANNEL_0; ch < DMA_CHANNEL_MAX_NUM; ch++) {
        if (hal_dma_regs_data_get_int_err_st(ch)) {
            hal_dma_interrupt_clear_err(ch);
            g_hal_dma_channel[ch].state = HAL_DMA_CH_STATE_CLOSED;
            if (g_hal_dma_channel[ch].isr_callback != NULL) {
                g_hal_dma_channel[ch].isr_callback(HAL_DMA_INTERRUPT_ERR, ch, g_hal_dma_channel[ch].arg);
            }
        }
    }
}

void hal_dma_v151_irq_handler(void)
{
    /* Transfer Complete. */
    hal_dma_tc_isr();

    /* Error. */
    hal_dma_err_isr();

    /* remove pm veto */
    dma_port_remove_sleep_veto();
}

static hal_dma_funcs_t g_hal_dma_funcs = {
    .init = hal_dmac_v151_init,
    .deinit = hal_dmac_v151_deinit,
    .open = hal_dmac_v151_open,
    .close = hal_dmac_v151_close,
    .ch_enable = hal_dmac_v151_ch_enable,
    .get_block = hal_dmac_v151_get_tranf_size,
    .clear = hal_dmac_v151_interrupt_clear,
    .get_idle = hal_dmac_v151_ch_get_idle,
    .cfg_single = hal_dmac_v151_config_single_block,
    .add_lli = hal_dmac_v151_add_lli_transfer,
    .is_enabled = hal_dmac_v151_ch_is_enabled,
    .enable_lli = hal_dmac_v151_enable_lli
};

hal_dma_funcs_t *hal_dmac_v151_funcs_get(void)
{
    return &g_hal_dma_funcs;
}
