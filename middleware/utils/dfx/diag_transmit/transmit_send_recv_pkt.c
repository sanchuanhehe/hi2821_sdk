/*
 * Copyright (c) @CompanyNameMagicTag 2021-2023. All rights reserved.
 * Description: transmit header file
 * This file should be changed only infrequently and with great care.
 */
#include "transmit_send_recv_pkt.h"
#include "diag.h"
#include "transmit_item.h"
#include "transmit_dst.h"
#include "transmit_src.h"
#include "transmit_debug.h"
#include "errcode.h"
#include "dfx_adapt_layer.h"
#include "diag_service.h"
#include "diag_pkt_router.h"

errcode_t transmit_receiver_start(uint8_t cmd_id, void *cmd_param, uint16_t cmd_param_size,
                                  diag_option_t *option, bool from_upper_machine)
{
    unused(cmd_id);
    unused(cmd_param_size);
    errcode_t ret = ERRCODE_FAIL;
    transmit_start_pkt_t *start_pkt = (transmit_start_pkt_t *)transmit_get_tlv_payload(cmd_param);

    switch (start_pkt->transmit_type) {
        case TRANSMIT_TYPE_SAVE_FILE:
        case TRANSMIT_TYPE_SAVE_OTA_IMG:
        case TRANSMIT_TYPE_WRITE_MEMORY:
        case TRANSMIT_TYPE_WRITE_FLASH:
            ret = transmit_dst_item_process_start_frame(start_pkt, option, from_upper_machine);
            break;
        case TRANSMIT_TYPE_READ_FILE:
        case TRANSMIT_TYPE_DUMP:
        case TRANSMIT_TYPE_READ_MEMORY:
        case TRANSMIT_TYPE_READ_FLASH:
            ret = transmit_src_item_process_start_frame(start_pkt, option, from_upper_machine);
            break;
        default:
            break;
    }
    return ret;
}

errcode_t transmit_receiver_negotiate(uint8_t cmd_id, void *cmd_param, uint16_t cmd_param_size,
                                      diag_option_t *option, bool from_upper_machine)
{
    unused(cmd_id);
    unused(cmd_param_size);
    errcode_t ret = ERRCODE_FAIL;
    transmit_negotiate_pkt_t *negotiate_pkt = (transmit_negotiate_pkt_t *)transmit_get_tlv_payload(cmd_param);

    switch (negotiate_pkt->transmit_type) {
        case TRANSMIT_TYPE_SAVE_FILE:
        case TRANSMIT_TYPE_SAVE_OTA_IMG:
        case TRANSMIT_TYPE_WRITE_MEMORY:
        case TRANSMIT_TYPE_WRITE_FLASH:
            ret = transmit_dst_item_process_negotiate_frame(negotiate_pkt, option, from_upper_machine);
            break;
        case TRANSMIT_TYPE_READ_FILE:
        case TRANSMIT_TYPE_DUMP:
        case TRANSMIT_TYPE_READ_MEMORY:
        case TRANSMIT_TYPE_READ_FLASH:
            ret = transmit_src_item_process_negotiate_frame(negotiate_pkt, option, from_upper_machine);
            break;
        default:
            break;
    }
    return ret;
}

errcode_t transmit_receiver_data_request(uint8_t cmd_id, void *cmd_param, uint16_t cmd_param_size,
    diag_option_t *option, bool from_upper_machine)
{
    unused(cmd_id);
    unused(cmd_param_size);
    transmit_data_request_pkt_t *request_pkt = (transmit_data_request_pkt_t *)transmit_get_tlv_payload(cmd_param);
    return transmit_src_item_process_data_request_frame(request_pkt, option, from_upper_machine);
}

errcode_t transmit_receiver_data_reply(uint8_t cmd_id, void *cmd_param, uint16_t cmd_param_size,
    diag_option_t *option, bool from_upper_machine)
{
    unused(cmd_id);
    unused(cmd_param_size);
    transmit_data_reply_pkt_t *reply_pkt = (transmit_data_reply_pkt_t *)transmit_get_tlv_payload(cmd_param);
    return transmit_dst_item_process_data_reply_frame(reply_pkt, option, from_upper_machine);
}

errcode_t transmit_receiver_notify(uint8_t cmd_id, void *cmd_param, uint16_t cmd_param_size,
                                   diag_option_t *option, bool from_upper_machine)
{
    unused(cmd_id);
    unused(cmd_param_size);
    transmit_state_notify_pkt_t *notify_pkt = (transmit_state_notify_pkt_t *)transmit_get_tlv_payload(cmd_param);
    transmit_item_process_notify_frame(notify_pkt, option, from_upper_machine);
    return ERRCODE_SUCC;
}

errcode_t transmit_receiver_stop(uint8_t cmd_id, void *cmd_param, uint16_t cmd_param_size,
                                 diag_option_t *option, bool from_upper_machine)
{
    unused(cmd_id);
    unused(cmd_param_size);
    transmit_stop_pkt_t *stop_pkt = (transmit_stop_pkt_t *)transmit_get_tlv_payload(cmd_param);
    transmit_dst_item_process_stop_frame(stop_pkt, option, from_upper_machine);
    return ERRCODE_SUCC;
}

errcode_t transmit_send_packet(uint8_t cmd_id, uint8_t *pkt, uint32_t pkt_size, diag_option_t *option,
                               bool down_machine)
{
    dfx_assert(pkt);
    errcode_t ret;
    uint16_t buf_len = (uint16_t)sizeof(diag_ser_header_t) + (uint16_t)sizeof(diag_ser_frame_t) + (uint16_t)pkt_size;
    uint8_t *buf = dfx_malloc(0, buf_len);
    if (buf == NULL) {
        return ERRCODE_MALLOC;
    }

    diag_ser_header_t *header = (diag_ser_header_t *)buf;
    header->ser_id = DIAG_SER_FILE_TRANFER;
    header->cmd_id = cmd_id;
    header->src = DIAG_FRAME_FID_MCU;
    header->dst = option->peer_addr;

    header->crc_en = true;
    header->ack_en = false;
    header->length = buf_len - (uint16_t)sizeof(diag_ser_header_t);

    diag_ser_frame_t *frame = (diag_ser_frame_t *)(buf + sizeof(diag_ser_header_t));
    frame->module_id = DIAG_SER_FILE_TRANFER;
    frame->cmd_id = cmd_id;

    memcpy_s(((uint8_t *)frame + sizeof(diag_ser_frame_t)), pkt_size, pkt, pkt_size);

    transmit_printf_send_frame(cmd_id, pkt, (uint16_t)pkt_size, option, down_machine);
    ret = uapi_diag_service_send_data((diag_ser_data_t *)buf);
    dfx_free(0, buf);
    return ret;
}
