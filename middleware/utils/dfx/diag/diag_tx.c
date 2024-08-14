/*
 * Copyright (c) @CompanyNameMagicTag 2021-2023. All rights reserved.
 * Description: diag tx
 * This file should be changed only infrequently and with great care.
 */

#include "diag_tx.h"
#include "diag_channel_item.h"
#include "diag_mem.h"
#include "diag_debug.h"
#include "zdiag_adapt_layer.h"
#include "diag_dfx.h"
#include "log_file.h"
#include "errcode.h"

STATIC uint8_t *diag_alloc_pkt_buffer(diag_pkt_handle_t *pkt)
{
    uint8_t *buf = NULL;
    uint32_t copied_size = 0;
    uint32_t total_size = diag_pkt_handle_get_total_size(pkt);
    buf = dfx_malloc(0, total_size);
    if (buf == NULL) {
        return NULL;
    }
    (void)memset_s(buf, total_size, 0, total_size);

    for (uint32_t i = 0; i < pkt->data_cnt; i++) {
        if (total_size < copied_size) {
            break;
        }
        if (pkt->data_len[i] == 0) {
            continue;
        }
        if (memcpy_s(buf + copied_size, total_size - copied_size, pkt->data[i], pkt->data_len[i]) != EOK) {
            dfx_log_err("diag_alloc_pkt_buffer memcpy fail\r\n");
            dfx_free(0, buf);
            return NULL;
        }
        copied_size += pkt->data_len[i];
    }
    return buf;
}

STATIC errcode_t zdiag_pkt_router_tx(const diag_channel_item_t *chan, diag_pkt_handle_t *pkt)
{
    if (chan == NULL || pkt == NULL || pkt->data_cnt == 0) {
        return ERRCODE_INVALID_PARAM;
    }

    dfx_data_type_t data_type = (pkt->critical != 0) ? DFX_DATA_DIAG_PKT_CRITICAL : DFX_DATA_DIAG_PKT;

    if (chan->tx_hook != NULL && pkt->output_type == 0) {
        return (errcode_t)chan->tx_hook(0, data_type, pkt->data, pkt->data_len, pkt->data_cnt);
    }

#if CONFIG_DFX_SUPPORT_OFFLINE_LOG_FILE == DFX_YES
    if (pkt->output_type == 1) {
        uint8_t *alloc_buf = NULL;
        uint8_t *data = NULL;
        uint32_t data_size = 0;
        if (pkt->data_cnt > 1) {
            /* 如果PKT中数据是分散的，需要聚合到一段连续内存中 */
            alloc_buf = diag_alloc_pkt_buffer(pkt);
            data = alloc_buf;
            data_size = diag_pkt_handle_get_total_size(pkt);
        } else {
            data = pkt->data[0];
            data_size = pkt->data_len[0];
        }

        if (data == NULL || data_size == 0) {
            return ERRCODE_FAIL;
        }

        errcode_t ret = uapi_logfile_write(STORE_DIAG, 0, data, data_size);
        if (ret != ERRCODE_SUCC && ret != ERRCODE_DFX_LOGFILE_SUSPENDED) {
            dfx_log_err("offline log wrtie failed, ret = 0x%x\r\n", ret);
        }
        if (alloc_buf != NULL) {
            dfx_free(0, alloc_buf);
        }
        return ret;
    }
#endif

    return ERRCODE_FAIL;
}

errcode_t diag_pkt_router_output(diag_pkt_handle_t *pkt, const diag_pkt_process_param_t *process_param)
{
    diag_frame_fid_t dst = process_param->dst_addr;
    diag_channel_item_t *chan = zdiag_dst_2_chan(dst);
    return zdiag_pkt_router_tx(chan, pkt);
}
