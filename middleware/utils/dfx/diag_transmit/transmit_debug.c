/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: transmit
 * This file should be changed only infrequently and with great care.
 */
#include "transmit_debug.h"
#if defined DEBUG_TRANSMIT
void transmit_printf_item(char *info, transmit_item_t *item)
{
    dfx_assert(info);
    dfx_assert(item);
    dfx_log_debug("++++++++++++++++++++%s start++++++++++++++++++++\r\n", info);
    dfx_log_debug("transmit_id=0x%x\r\n", item->transmit_id);
    dfx_log_debug("used=0x%x\r\n", item->used);
    dfx_log_debug("init_fail=0x%x\r\n", item->init_fail);
    dfx_log_debug("permanent=0x%x\r\n", item->permanent);
    dfx_log_debug("local_src=0x%x\r\n", item->local_src);
    dfx_log_debug("remote_type=0x%x\r\n", item->remote_type);
    dfx_log_debug("local_type=0x%x\r\n", item->local_type);
    dfx_log_debug("down_machine=0x%x\r\n", item->down_machine);
    dfx_log_debug("received_size=0x%x\r\n", item->received_size);
    dfx_log_debug("total_size=0x%x\r\n", item->total_size);
    dfx_log_debug("write_read=0x%x\r\n", item->write_read);
    dfx_log_debug("usr_wr_data=0x%x\r\n", item->usr_wr_data);
    dfx_log_debug("bus_addr=0x%x\r\n", item->bus_addr);
    dfx_log_debug("expiration=0x%x\r\n", item->expiration);
    dfx_log_debug("last_rcv_pkt_time=0x%x\r\n", item->last_rcv_pkt_time);
    dfx_log_debug("last_send_pkt_time=0x%x\r\n", item->last_send_pkt_time);
    dfx_log_debug("option=0x%x\r\n", item->option.peer_addr);
    if (item->file_name) {
        dfx_log_debug("file_name=%s\r\n", item->file_name);
    }
    dfx_log_debug("--------------------%s start--------------------\r\n", info);
    unused(info);
}

STATIC void transmit_printf_receive_data_request(void *cmd_param)
{
    transmit_data_request_pkt_t *req = (transmit_data_request_pkt_t *)cmd_param;
    dfx_log_debug("[RECEIVER_REQUEST][id=%d][cnt=%d][0ffset=0x%x][size=0x%x]\r\n", req->transmit_id, req->cnt,
        req->item[0].offset, req->item[0].size);
    unused(req);
}

STATIC void transmit_printf_receive_data_reply(void *cmd_param)
{
    transmit_data_reply_pkt_t *reply = (transmit_data_reply_pkt_t *)cmd_param;
    dfx_log_debug("[RECEIVER_REPLY][id=%d][ret=%d][0ffset=0x%x][size=0x%x][crc=0x%x]\r\n", reply->transmit_id,
        reply->ret, reply->offset, reply->size, reply->crc);
    unused(reply);
}

STATIC void transmit_printf_receive_notify(void *cmd_param)
{
    transmit_state_notify_pkt_t *notify = (transmit_state_notify_pkt_t *)cmd_param;
    dfx_log_debug("[RECEIVER_REPLY][id=%d][code=%d][len=0x%x]\r\n", notify->transmit_id, notify->state_code,
        notify->len);
    unused(notify);
}

void transmit_printf_receive_frame(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option,
    bool from_upper_machine)
{
    unused(cmd_param_size);
    unused(option);
    unused(from_upper_machine);

    switch (cmd_id) {
        case DIAG_CMD_ID_TRANSMIT_REQUEST:
            transmit_printf_receive_data_request(cmd_param);
            break;
        case DIAG_CMD_ID_TRANSMIT_REPLY:
            transmit_printf_receive_data_reply(cmd_param);
            break;
        case DIAG_CMD_ID_TRANSMIT_START:
            break;
        case DIAG_CMD_ID_STATE_NOTIFY:
            transmit_printf_receive_notify(cmd_param);
            break;
        default:
            break;
    }
}

void transmit_printf_send_frame(uint8_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option,
    bool down_machine)
{
    unused(cmd_id);
    unused(cmd_param);
    unused(cmd_param_size);
    unused(option);
    unused(down_machine);
}
#endif
