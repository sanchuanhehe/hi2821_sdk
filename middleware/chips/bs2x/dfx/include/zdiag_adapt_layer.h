/*
 * Copyright (c) @CompanyNameMagicTag 2021-2023. All rights reserved.
 * Description: diag usr adapt
 * This file should be changed only infrequently and with great care.
 */

#ifndef ZDIAG_ADAPT_LAYER_H
#define ZDIAG_ADAPT_LAYER_H

#include "zdiag_config.h"
#include "diag_common.h"
#include "diag_channel.h"
#include "diag_addr_impl.h"

#define DIAG_HSO_MAX_MUX_PKT_SIZE 512
#define DIAG_HSO_VER_DEFAULT 0
#define DIAG_HSO_THIS_VER 0x11

#define USER_CMD_LIST_NUM 10 /* Maximum number of non-system command lists */

static inline uint32_t diag_adapt_get_msg_time(void)
{
    return diag_get_msg_time_impl();
}

static inline uint64_t diag_adapt_get_msg_time_ms(void)
{
    return diag_get_msg_time_ms_impl();
}

static inline errcode_t diag_adapt_sync_tx_sem_wait(void)
{
    return diag_sync_tx_sem_wait_impl();
}

static inline void diag_adapt_sync_tx_sem_signal(void)
{
    return diag_sync_tx_sem_signal_impl();
}

static inline uint32_t diag_adapt_get_default_dst(void)
{
    return diag_get_default_dst_impl();
}

#ifdef SUPPORT_DIAG_V2_PROTOCOL
static inline diag_channel_id_t diag_adapt_dst_2_channel_id(diag_frame_fid_t addr)
{
    return diag_adapt_dst_2_channel_id_impl(addr);
}
static inline diag_frame_fid_t diag_adapt_get_local_addr(void)
{
    return diag_get_local_addr_impl();
}
#else
static inline diag_channel_id_t diag_adapt_dst_2_channel_id(diag_addr addr)
{
    return diag_adapt_dst_2_channel_id_impl(addr);
}
static inline diag_addr diag_adapt_get_local_addr(void)
{
    return diag_get_local_addr_impl();
}

static inline diag_addr_attribute_t diag_adapt_addr_2_attribute(diag_addr addr)
{
    return diag_addr_2_attribute_impl(addr);
}
#endif /* SUPPORT_DIAG_V2_PROTOCOL */
static inline bool diag_adapt_is_in_unblocking_context(void)
{
    return diag_is_in_unblocking_context_impl();
}
#endif
