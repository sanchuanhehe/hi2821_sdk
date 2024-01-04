/*
 * Copyright (c) @CompanyNameMagicTag 2021-2023. All rights reserved.
 * Description: zdiag ind producer
 * This file should be changed only infrequently and with great care.
 */
#ifndef DIAG_IND_SRC_H
#define DIAG_IND_SRC_H

#include "errcode.h"
#include "diag_common.h"
#include "diag.h"

typedef errcode_t (*report_msg_func)(uint32_t module_id, uint32_t msg_id, const uint8_t *buf, uint16_t buf_size,
    uint8_t level);

typedef struct {
    uint8_t *packet;
    uint16_t packet_size;
} diag_report_sys_msg_packet;

typedef struct {
    uint8_t **packet;
    uint16_t *packet_size;
    uint8_t pkt_cnt;
} diag_report_packet;

errcode_t uapi_diag_report_packet_direct(uint16_t cmd_id, diag_option_t *option, const uint8_t *packet,
    uint16_t packet_size);
errcode_t uapi_zdiag_report_sys_msg_instance(uint32_t module_id, uint32_t msg_id, const uint8_t *packet,
    uint16_t packet_size, uint8_t level);
errcode_t uapi_zdiag_report_sys_msg_instance_sn(uint32_t module_id, uint32_t msg_id,
    diag_report_sys_msg_packet *report_sys_msg_packet, uint8_t level, uint32_t sn);

#endif
