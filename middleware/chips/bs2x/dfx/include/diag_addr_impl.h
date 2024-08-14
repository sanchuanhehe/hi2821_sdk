/*
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved.
 * Description:diag addr impl
 * This file should be changed only infrequently and with great care.
 */
#ifndef DIAG_ADDR_IMPL_H
#define DIAG_ADDR_IMPL_H
#include "zdiag_config.h"
#include "dfx_adapt_layer.h"
#include "dfx_resource_id.h"
#include "diag_common.h"

#define DIAG_LOCAL_ADDR 100
#define DIAG_UART_CONNECT_HSO_ADDR 200

static inline uint32_t diag_get_msg_time_impl(void)
{
    return dfx_get_cur_second();
}

static inline uint64_t diag_get_msg_time_ms_impl(void)
{
    uint64_t time_ms = (uint64_t)dfx_get_cur_second() * 1000; /* 1 second = 1000 milliseconds */
    return time_ms;
}

static inline errcode_t diag_sync_tx_sem_wait_impl(void)
{
    return ERRCODE_FAIL;
}
static inline void diag_sync_tx_sem_signal_impl(void)
{
    return;
}
#ifdef SUPPORT_DIAG_V2_PROTOCOL
static inline diag_frame_fid_t diag_get_default_dst_impl(void)
{
    return DIAG_FRAME_FID_PC;
}

static inline diag_channel_id_t diag_adapt_dst_2_channel_id_impl(diag_frame_fid_t addr)
{
    if (addr == DIAG_FRAME_FID_PC) {
        return DIAG_CHANNEL_ID_0;
    } else if (addr == DIAG_FRAME_FID_BT) {
        return DIAG_CHANNEL_ID_1;
    }

    return DIAG_CHANNEL_ID_INVALID;
}

static inline diag_frame_fid_t diag_adapt_channel_id_2_src_impl(diag_channel_id_t id)
{
    if (id == DIAG_CHANNEL_ID_0) {
        return DIAG_FRAME_FID_PC;
    } else if (id == DIAG_CHANNEL_ID_1) {
        return DIAG_FRAME_FID_BT;
    }

    return DIAG_FRAME_FID_MAX;
}

static inline diag_frame_fid_t diag_get_local_addr_impl(void)
{
    return DIAG_FRAME_FID_LOCAL;
}

#else
static inline diag_addr diag_get_default_dst_impl(void)
{
    return DIAG_UART_CONNECT_HSO_ADDR;
}
static inline diag_channel_id_t diag_adapt_dst_2_channel_id_impl(diag_addr addr)
{
    if (addr == DIAG_UART_CONNECT_HSO_ADDR) {
        return DIAG_CHANNEL_ID_0;
    }
    return DIAG_CHANNEL_ID_INVALID;
}

static inline diag_addr diag_get_local_addr_impl(void)
{
    return DIAG_LOCAL_ADDR;
}

static inline diag_addr_attribute_t diag_addr_2_attribute_impl(diag_addr addr)
{
    if (addr == DIAG_UART_CONNECT_HSO_ADDR) {
        return DIAG_ADDR_ATTRIBUTE_VALID|DIAG_ADDR_ATTRIBUTE_HSO_CONNECT;
    } else if (addr == DIAG_LOCAL_ADDR) {
        return DIAG_ADDR_ATTRIBUTE_VALID;
    } else {
        return 0;
    }
}
#endif /* SUPPORT_DIAG_V2_PROTOCOL */
static inline bool diag_is_in_unblocking_context_impl(void)
{
    return false;
}

#endif