/*
 * Copyright (c) @CompanyNameMagicTag 2021-2023. All rights reserved.
 * Description: diag mem
 * This file should be changed only infrequently and with great care.
 */

#include "diag_mem.h"
#include "zdiag_adapt_layer.h"
#include "diag_dfx.h"
#include "diag_pkt.h"
#include "diag_debug.h"
#include "errcode.h"

errcode_t diag_pkt_need_cross_task(diag_pkt_handle_t *pkt)
{
    uint8_t *buf = NULL;
    diag_pkt_handle_t pkt_tmp;

    if (pkt->single_task == false) {
        return ERRCODE_SUCC;
    }

    uint32_t copyed_size = 0;
    uint32_t total_size = diag_pkt_handle_get_total_size(pkt);
    buf = dfx_malloc(0, total_size);
    if (buf == NULL) {
        return ERRCODE_MALLOC;
    }

    diag_dfx_alloc_pkt(0, total_size);

    for (unsigned i = 0; i < pkt->data_cnt; i++) {
        if (memcpy_s(buf + copyed_size, total_size - copyed_size, pkt->data[i], pkt->data_len[i]) != EOK) {
            dfx_log_debug("diag_pkt_need_cross_task: memcpy fail\r\n");
        }
        copyed_size += pkt->data_len[i];
    }

    pkt_tmp = *pkt;

    diag_pkt_handle_init(pkt, 1);
    diag_pkt_handle_set_data(pkt, DIAG_PKT_DATA_ID_0, buf, (uint16_t)total_size, DIAG_PKT_DFX_MALLOC_DATA);
    pkt->critical = pkt_tmp.critical;
    pkt->output_type = pkt_tmp.output_type;
    return ERRCODE_SUCC;
}

void diag_pkt_free(diag_pkt_handle_t *pkt)
{
    if ((pkt->need_free) != 0) {
        dfx_assert(pkt->data_cnt == 1);
        dfx_free(0, pkt->data[0]);
        diag_dfx_free_pkt(0, pkt->data_len[0]);
        diag_pkt_handle_clean(pkt);
        return;
    }
    return;
}
