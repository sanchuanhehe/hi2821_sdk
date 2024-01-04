/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: zdiag pkt
 * This file should be changed only infrequently and with great care.
 */
#ifndef DIAG_PKT_H
#define DIAG_PKT_H

#include <stdint.h>
#include <stdbool.h>
#include "diag_common.h"
#include "common_def.h"

enum {
    DIAG_PKT_MEM_TYPE_STACK_DATA_STACK,
    DIAG_PKT_MEM_TYPE_DATA_DFX_MALLOC,
};

enum {
    DIAG_PKT_DATA_ID_0,
    DIAG_PKT_DATA_ID_1,
    DIAG_PKT_DATA_ID_2,
    DIAG_PKT_DATA_ID_MAX,
};

#define DIAG_PKT_DATA_ATTRIBUTE_SINGLE_TASK 0x1
#define DIAG_PKT_DATA_ATTRIBUTE_DYN_MEM 0x2

typedef enum {
    DIAG_PKT_STACK_DATA = DIAG_PKT_DATA_ATTRIBUTE_SINGLE_TASK,
    DIAG_PKT_DFX_MALLOC_DATA = DIAG_PKT_DATA_ATTRIBUTE_DYN_MEM,
} diag_pkt_data_t;

typedef struct {
    uint8_t *data[DIAG_PKT_DATA_ID_MAX];
    uint16_t data_len[DIAG_PKT_DATA_ID_MAX];
    uint8_t data_cnt : 2;
    uint8_t need_free : 1;      /* true:pkt有数据需要释放 false:pkt无数据需要释放 */
    uint8_t single_task : 1;    /* true:pkt有数据无法跨任务传递 false:pkt无数据无法跨任务传递 */
    uint8_t critical : 1;       /* 非调度报文，如：死机 */
    uint8_t output_type : 2;    /* 0：从对应通道输出或处理，1: 保存至本地 */
    uint8_t resvered : 1;       /* 保留字段 */
} diag_pkt_handle_t;

errcode_t diag_check_mux_pkt(const diag_router_frame_t *frame, uint16_t size);
errcode_t diag_check_hcc_pkt(uint8_t *data, uint16_t size);
void diag_pkt_handle_init(diag_pkt_handle_t *pkt, uint8_t data_cnt);
void diag_pkt_set_critical(diag_pkt_handle_t *pkt);
void diag_pkt_set_output_type(diag_pkt_handle_t *pkt, uint8_t type);

void diag_pkt_handle_set_data(diag_pkt_handle_t *pkt, uint8_t idx, uint8_t *data,
                              uint16_t data_len, diag_pkt_data_t attribute);

void diag_mk_log_pkt(diag_cmd_log_layer_stru_t *log_pkt, uint32_t module_id, uint32_t msg_id);
void diag_mk_normal_log_pkt(diag_cmd_normal_log_layer_t *log_pkt, uint8_t module_id, uint32_t msg_id);
void diag_mk_extend_log_pkt(diag_cmd_extend_log_layer_t *log_pkt, uint8_t module_id, uint32_t msg_id);
void diag_mk_min_log_pkt(diag_cmd_minimal_log_layer_t *log_pkt, uint32_t msg_id);

void diag_mk_log_pkt_sn(diag_cmd_log_layer_stru_t *log_pkt, uint32_t module_id, uint32_t msg_id, uint32_t sn);
void diag_mk_normal_log_pkt_sn(diag_cmd_normal_log_layer_t *log_pkt, uint8_t module_id, uint32_t msg_id, uint16_t sn);
void diag_mk_extend_log_pkt_sn(diag_cmd_extend_log_layer_t *log_pkt, uint8_t module_id, uint32_t msg_id, uint8_t sn);
void diag_mk_min_log_pkt_sn(diag_cmd_minimal_log_layer_t *log_pkt, uint32_t msg_id, uint16_t sn);

void diag_mk_ind_header(diag_head_ind_stru_t *ind, uint16_t cmd_id);
void diag_mk_req_header(diag_head_req_stru_t *req, uint16_t cmd_id);
void diag_mk_frame_header_1(diag_router_frame_t *frame, uint16_t pkt_size);

static inline diag_router_frame_t *diag_pkt_handle_get_frame(diag_pkt_handle_t *pkt)
{
    return (diag_router_frame_t *)pkt->data[0];
}

static inline diag_head_ind_stru_t *diag_receive_pkt_handle_get_ind(diag_pkt_handle_t *pkt)
{
    return (diag_head_ind_stru_t *)(pkt->data[0] + DIAG_ROUTER_HEADER_LEN);
}

static inline uint8_t *diag_receive_pkt_handle_get_ind_data(diag_pkt_handle_t *pkt)
{
    return pkt->data[0] + DIAG_ROUTER_HEADER_LEN + DIAG_IND_HEADER_SIZE;
}

static inline diag_head_req_stru_t *diag_receive_pkt_handle_get_req(diag_pkt_handle_t *pkt)
{
    return (diag_head_req_stru_t *)(pkt->data[0] + DIAG_ROUTER_HEADER_LEN);
}

static inline uint8_t *diag_receive_pkt_handle_get_req_data(diag_pkt_handle_t *pkt)
{
    return pkt->data[0] + DIAG_ROUTER_HEADER_LEN + DIAG_REQ_HEADER_SIZE;
}

static inline uint32_t diag_pkt_handle_get_total_size(diag_pkt_handle_t *pkt)
{
    uint32_t total_size = 0;
    for (int i = 0; i < pkt->data_cnt; i++) {
        total_size += (uint32_t)pkt->data_len[i];
    }
    return total_size;
}

static inline void diag_pkt_handle_clean(diag_pkt_handle_t *pkt)
{
    pkt->need_free = false;
    for (int i = 0; i < pkt->data_cnt; i++) {
        pkt->data[i] = NULL;
        pkt->data_len[i] = 0;
    }
    pkt->data_cnt = 0;
}

static inline void diag_pkt_cnt_increase(diag_pkt_handle_t *pkt, uint8_t cnt)
{
    pkt->data_cnt += cnt;
}

#endif
