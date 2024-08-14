/*
 * Copyright (c) @CompanyNameMagicTag 2021-2023. All rights reserved.
 * Description: pkt router
 * This file should be changed only infrequently and with great care.
 */

#include "diag_pkt_router.h"
#include "securec.h"
#include "diag_msg.h"
#include "diag_mem.h"
#include "diag_cmd_dst.h"
#include "diag_ind_dst.h"
#include "diag_tx.h"
#include "diag_debug.h"
#include "zdiag_adapt_layer.h"
#include "diag_dfx.h"
#include "dfx_adapt_layer.h"
#include "errcode.h"
#include "debug_print.h"
#include "diag_tx.h"
#include "uapi_crc.h"

#define DIAG_ROUTER_EXTRAS_MAX_LEN  (DIAG_ROUTER_HEADER_MAX_LEN + DIAG_ROUTER_CRC_LEN)

/* SN definitions */
enum {
    FRAME_SN_FLAG_NONE,
    FRAME_SN_FLAG_START,
    FRAME_SN_FLAG_ONGOING,
    FRAME_SN_FLAG_END,
};

static diag_router_notify_f g_diag_notify_func = NULL;

static void diag_router_build_frame(diag_router_frame_t *frame, uint16_t send_len, diag_router_data_t *data)
{
    uint16_t offset = 0;

    frame->sof      = DIAG_ROUTER_FRAME_START_FLAG;
    frame->ctrl     = frame_ctrl_build(data->ctrl.en_crc, data->ctrl.en_fid, data->ctrl.en_sn, data->ctrl.ack_type,
                                       data->ctrl.en_eof);
    frame->len_msb  = (uint8_t)((send_len - DIAG_ROUTER_HEADER_LEN - data->ctrl.en_eof) >> DIAG_FRAME_SHIFT_8);
    frame->len_lsb  = (uint8_t)(send_len - DIAG_ROUTER_HEADER_LEN - data->ctrl.en_eof);

    if ((data->ctrl.en_fid) != 0) {
        frame->payload[offset] = data->fid;
        offset++;
    }
    if ((data->ctrl.en_sn) != 0) {
        frame->payload[offset] = data->sn_count;
        offset++;
    }

    if ((data->echo) != 0) {
        frame->payload[offset] = data->echo;
        offset++;
    }

    memcpy_s((void *)(frame->payload + offset), data->data_len, data->data, data->data_len);
    offset += data->data_len;
    if ((data->ctrl.en_crc) != 0) {
        uint16_t crc16 = uapi_crc16(0, (uint8_t *)frame->payload, offset);
        memcpy_s((void *)(frame->payload + offset), sizeof(uint16_t), &crc16,  sizeof(uint16_t));
        offset += sizeof(uint16_t);
    }

    if ((data->ctrl.en_eof) != 0) {
        frame->payload[offset] = DIAG_ROUTER_FRAME_END_FLAG;
    }
}

// 回复ack
static void diag_router_send_ack(diag_router_frame_t *frame, diag_pkt_process_param_t *process_param, uint8_t echo)
{
    diag_router_data_t router_data = {0};

    router_data.ctrl.en_crc   = 1;
    router_data.ctrl.ack_type = FRAME_ACK_TYPE_NONE;
    router_data.ctrl.en_fid   = 0;
    router_data.ctrl.en_sn    = FRAME_SN_FLAG_NONE;
    router_data.ctrl.en_eof   = 0;

    router_data.sn_count      = 0;
    router_data.echo          = echo;

    router_data.data          = NULL;
    router_data.data_len      = 0;

    uint16_t send_len = DIAG_ROUTER_HEADER_LEN + DIAG_ROUTER_ECHO_LEN + DIAG_ROUTER_CRC_LEN;
    uint8_t *ack_buffer = dfx_malloc(0, send_len);
    if (ack_buffer != NULL) {
        diag_router_build_frame((diag_router_frame_t *)ack_buffer, send_len, &router_data);

        diag_pkt_handle_t pkt;
        diag_pkt_handle_init(&pkt, 1);
        diag_pkt_handle_set_data(&pkt, DIAG_PKT_DATA_ID_0, ack_buffer, send_len, DIAG_PKT_STACK_DATA);

        process_param->dst_addr = process_param->src_addr;
        diag_pkt_router_output(&pkt, process_param);
        dfx_free(0, ack_buffer);
    }
}

// 接收处理
STATIC errcode_t diag_pkt_router_notify(diag_pkt_handle_t *pkt, diag_pkt_process_param_t *process_param)
{
    uint8_t frame_echo = 0;
    uint16_t crc_val;
    uint16_t frame_crc;

    if (pkt == NULL) {
        return ERRCODE_INVALID_PARAM;
    }

    diag_router_frame_t *frame = (diag_router_frame_t *)pkt->data[0];
    if (frame->sof != DIAG_ROUTER_FRAME_START_FLAG) {
        return ERRCODE_FAIL;
    }

    uint16_t payload_len = get_frame_len(frame->len_msb, frame->len_lsb);
    diag_router_ctrl_t *ctrl = (diag_router_ctrl_t *)&frame->ctrl;
    // 校验crc
    if ((ctrl->en_crc) != 0) {
        payload_len -= DIAG_ROUTER_CRC_LEN;
        crc_val = uapi_crc16(0, (uint8_t *)&frame->payload, payload_len);
        frame_crc = *(uint16_t *)(&frame->payload[payload_len]);
        if (crc_val != frame_crc) {
            frame_echo = 1;
        }
    }
    // 是否需要ack
    if (ctrl->ack_type == FRAME_ACK_TYPE_ACK) {
        dfx_log_debug("diag pkt send ack\r\n");
        diag_router_send_ack(frame, process_param, frame_echo);
    }
    // 校验失败
    if (frame_echo != 0) {
        dfx_log_err("diag pkt check crc error! frame_crc = 0x%x crc_val = 0x%x\r\n", frame_crc, crc_val);
        return ERRCODE_DIAG_CRC_ERROR;
    }

    /* frame len is no longer used and is used to record addr */
    frame->len_msb = process_param->dst_addr;
    frame->len_lsb = process_param->src_addr;
    if (g_diag_notify_func) {
        return g_diag_notify_func(frame, payload_len);
    }
    return ERRCODE_FAIL;
}

static bool diag_check_transmit_pkt(diag_pkt_handle_t *pkt)
{
    uint8_t extra_len = 0;
    diag_router_frame_t *frame = (diag_router_frame_t *)pkt->data[0];

    /* 数据保存至Flash的类型（如离线日志） */
    if (pkt->output_type == 1) {
        return false;
    }

    if (frame->sof != DIAG_ROUTER_FRAME_START_FLAG) {
        return false;
    }

    // fid en
    if (get_frame_ctrl_fid_en(frame->ctrl) != 0) {
        extra_len++;
    }

    // 是否是分包数据
    if (get_frame_ctrl_sn(frame->ctrl) != 0) {
        extra_len++;
    }

    diag_ser_frame_t *ser_frame = (diag_ser_frame_t *)((uint8_t *)frame + DIAG_ROUTER_HEADER_LEN + extra_len);
    if (ser_frame->module_id == DIAG_SER_FILE_TRANFER || ser_frame->module_id == DIAG_SER_OTA) {
        return true;
    }

    return false;
}

STATIC errcode_t diag_pkt_router_enqueue(diag_pkt_handle_t *pkt, diag_pkt_process_param_t *process_param)
{
    errcode_t ret;
    diag_pkt_msg_t msg;

    ret = diag_pkt_need_cross_task(pkt);
    if (ret != ERRCODE_SUCC) {
        dfx_log_err("[ERROR][diag_pkt_router_enqueue][%d][errcode=%u]\r\n", __LINE__, ret);
        return ret;
    }

    msg.pkt = *pkt;
    msg.param.cur_proc = DIAG_PKT_PROC_SYNC;
    msg.param.src_addr = process_param->src_addr;
    msg.param.dst_addr = process_param->dst_addr;

    if (diag_check_transmit_pkt(pkt)) {
        ret = transmit_msg_write(DFX_MSG_ID_DIAG_PKT, (uint8_t *)&msg, sizeof(diag_pkt_msg_t), false);
    } else {
        ret = dfx_msg_write(DFX_MSG_ID_DIAG_PKT, (uint8_t *)&msg, sizeof(diag_pkt_msg_t), false);
    }

    if (ret == ERRCODE_SUCC) {
        zdiag_dfx_send_local_q_success();
        diag_pkt_handle_clean(pkt);
    } else {
        zdiag_dfx_send_local_q_fail();
    }
    return ret;
}

STATIC errcode_t diag_pkt_router_process(diag_pkt_handle_t *pkt, diag_pkt_process_param_t *process_param)
{
    diag_frame_fid_t dst_addr = process_param->dst_addr;
    if (dst_addr != DIAG_FRAME_FID_LOCAL) {
        return diag_pkt_router_output(pkt, process_param);
    } else {
        return diag_pkt_router_notify(pkt, process_param);
    }
}

errcode_t diag_pkt_router(diag_pkt_handle_t *pkt, diag_pkt_process_param_t *process_param)
{
    errcode_t ret = ERRCODE_FAIL;
    if (process_param->cur_proc == DIAG_PKT_PROC_ASYNC) {
        ret = diag_pkt_router_enqueue(pkt, process_param);
    } else {
        ret = diag_pkt_router_process(pkt, process_param);
    }

    diag_pkt_free(pkt);
    return ret;
}

// 发送流程
STATIC errcode_t diag_pkt_router_send_single(diag_router_data_t *data)
{
    diag_pkt_handle_t pkt;
    diag_pkt_process_param_t process_param;

    uint16_t send_len = data->data_len + DIAG_ROUTER_HEADER_LEN;
    if ((data->ctrl.en_crc) != 0) {
        send_len += DIAG_ROUTER_CRC_LEN;
    }

    if ((data->ctrl.en_fid) != 0) {
        send_len += DIAG_ROUTER_FID_LEN;
    }

    if ((data->ctrl.en_sn) != 0) {
        send_len += DIAG_ROUTER_SN_LEN;
    }

    if ((data->echo) != 0) {
        send_len += DIAG_ROUTER_ECHO_LEN;
    }

    if ((data->ctrl.en_eof) != 0) {
        send_len += DIAG_ROUTER_EOF_LEN;
    }

    uint8_t *buffer = dfx_malloc(0, send_len);
    if (buffer == NULL) {
        return ERRCODE_MALLOC;
    }
    diag_router_build_frame((diag_router_frame_t *)buffer, send_len, data);

    diag_pkt_handle_init(&pkt, 1);
    diag_pkt_handle_set_data(&pkt, DIAG_PKT_DATA_ID_0, buffer, send_len, DIAG_PKT_STACK_DATA);

    process_param.cur_proc = DIAG_PKT_PROC_SYNC;
    process_param.dst_addr = data->fid >> DIAG_ROUTER_FID_DST_BIT;
    process_param.src_addr = data->fid & DIAG_ROUTER_FID_MASK;
    errcode_t ret = diag_pkt_router(&pkt, &process_param);
    dfx_free(0, buffer);
    return ret;
}

// 拆包
STATIC errcode_t diag_pkt_router_send_loop(diag_router_data_t *router_data)
{
    errcode_t ret;
    uint16_t offset    = 0;
    uint16_t valid_mfs = 0;
    uint16_t size      = router_data->data_len;

    if (router_data->mfs <= DIAG_ROUTER_EXTRAS_MAX_LEN) {
        dfx_log_err("diag pkt mfs error! valid_mfs = 0x%x\r\n", valid_mfs);
        return ERRCODE_DIAG_BAD_DATA;
    }

    valid_mfs = router_data->mfs - DIAG_ROUTER_EXTRAS_MAX_LEN;

    diag_router_data_t *send_data = router_data;
    while (offset < size) {
        /* frist frame data */
        if (offset == 0) {
            send_data->data_len = valid_mfs;
            send_data->ctrl.en_sn = FRAME_SN_FLAG_START;
        } else if (offset + valid_mfs > size) {
            send_data->data_len = size - offset;
            send_data->ctrl.en_sn = FRAME_SN_FLAG_END;
        } else {
            send_data->data_len = valid_mfs;
            send_data->ctrl.en_sn = FRAME_SN_FLAG_ONGOING;
        }

        ret = diag_pkt_router_send_single(send_data);
        if (ret != ERRCODE_SUCC) {
            return ret;
        }
        offset += valid_mfs;
        send_data->data += valid_mfs;
        send_data->sn_count++;
    }
    return ERRCODE_SUCC;
}

errcode_t diag_pkt_router_send(diag_router_data_t *router_data)
{
    if (router_data == NULL) {
        return ERRCODE_INVALID_PARAM;
    }

    if ((router_data->ctrl.en_sn) != 0) {
        return diag_pkt_router_send_loop(router_data);
    } else {
        return diag_pkt_router_send_single(router_data);
    }
}

void diag_router_register_notify(diag_router_notify_f func)
{
    g_diag_notify_func = func;
}

void diag_pkt_msg_proc(uint32_t msg_id, uint8_t *msg, uint32_t msg_len)
{
    diag_pkt_msg_t *pkt_msg = (diag_pkt_msg_t *)msg;
    diag_pkt_process_param_t process_param;
    process_param.cur_proc = pkt_msg->param.cur_proc;
    process_param.src_addr = pkt_msg->param.src_addr;
    process_param.dst_addr = pkt_msg->param.dst_addr;
    diag_pkt_router(&pkt_msg->pkt, &process_param);
    unused(msg_id);
    unused(msg_len);
}
