/*
 * Copyright (c) @CompanyNameMagicTag 2021-2023. All rights reserved.
 * Description: pkt router
 * This file should be changed only infrequently and with great care.
 */

#ifndef DIAG_PKT_ROUTER_H
#define DIAG_PKT_ROUTER_H

#include "errcode.h"
#include "diag_pkt.h"
#include "diag_channel.h"

typedef struct {
    uint8_t cur_proc; /* diag_pkt_mode_t */
    uint8_t dst_addr;
    uint8_t src_addr;
} diag_pkt_process_param_t;

typedef struct {
    diag_pkt_process_param_t param;
    diag_pkt_handle_t pkt;
} diag_pkt_msg_t;

typedef errcode_t (*diag_router_notify_f)(diag_router_frame_t *data, uint16_t size);

void diag_router_register_notify(diag_router_notify_f func);

errcode_t diag_pkt_router(diag_pkt_handle_t *pkt, diag_pkt_process_param_t *process_param);

errcode_t diag_pkt_router_send(diag_router_data_t *router_data);

void diag_pkt_msg_proc(uint32_t msg_id, uint8_t *msg, uint32_t msg_len);

#endif
