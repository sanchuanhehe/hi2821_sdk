/*
 * Copyright (c) @CompanyNameMagicTag 2021-2023. All rights reserved.
 * Description: zdiag cmd producer
 * This file should be changed only infrequently and with great care.
 */
#include "diag_cmd_src.h"
#include "securec.h"
#include "diag_mem.h"
#include "diag_pkt_router.h"
#include "diag_debug.h"
#include "zdiag_adapt_layer.h"
#include "diag_pkt.h"
#include "debug_print.h"

errcode_t uapi_diag_run_cmd(uint16_t cmd_id, uint8_t *data, uint16_t data_size, diag_option_t *option)
{
    errcode_t ret;
    diag_pkt_handle_t pkt;

    uint8_t buf[DIAG_FRAME_HEADER_SIZE + DIAG_REQ_HEADER_SIZE];
    diag_router_frame_t *frame = (diag_router_frame_t *)buf;
    diag_head_req_stru_t *req = (diag_head_req_stru_t *)(buf + DIAG_FRAME_HEADER_SIZE);

    diag_pkt_handle_set_data(&pkt, DIAG_PKT_DATA_ID_0, (uint8_t *)frame,
                             DIAG_FRAME_HEADER_SIZE + DIAG_REQ_HEADER_SIZE, DIAG_PKT_STACK_DATA);
    diag_pkt_handle_set_data(&pkt, DIAG_PKT_DATA_ID_1, (uint8_t *)data,
                             data_size, DIAG_PKT_STACK_DATA);

    diag_mk_req_header(req, cmd_id);

    diag_mk_frame_header_1(frame, DIAG_REQ_HEADER_SIZE + data_size);

    diag_pkt_process_param_t process_param = {DIAG_PKT_PROC_ASYNC, DIAG_FRAME_FID_PC, DIAG_FRAME_FID_MCU};
    ret = diag_pkt_router(&pkt, &process_param);
    return ret;
}
