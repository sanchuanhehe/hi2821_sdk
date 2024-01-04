/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: transmit data
 * This file should be changed only infrequently and with great care.
 */
#include "transmit_src.h"
#include "transmit_dst.h"
#include "transmit_st.h"
#include "transmit_item.h"
#include "transmit_send_recv_pkt.h"
#include "transmit_write_read.h"
#include "transmit_file_operation.h"
#include "transmit_debug.h"
#include "transmit_cmd_id.h"
#include "string.h"
#include "securec.h"
#include "zdiag_adapt_layer.h"
#include "errcode.h"

STATIC uint32_t transmit_data_read(transmit_item_t *item, uint32_t offset, uint8_t *buf, uint32_t size)
{
    transmit_read_hook read_handler = (transmit_read_hook)item->write_read;
    return (uint32_t)read_handler(item->usr_wr_data, offset, buf, size);
}

/* 作为源端（读取数据并发送数据的一端）读取并发送数据 */
STATIC errcode_t transmit_process_data_request_one(transmit_item_t *item, uint32_t offset, uint32_t size,
                                                   diag_option_t *option, bool down_machine)
{
    transmit_data_reply_pkt_t *pkt = NULL;

    uint32_t data_size;
    uint32_t tlv_ext_len;
    uint32_t pkt_size;
    int32_t send_size = 0;
    int32_t left_size = (int32_t)size;
    int32_t read_size, readed_size;
    uint16_t block_size = (item->data_block_size == 0) ? DEFAULT_TRANSMIT_BLOCK_SIZE : item->data_block_size;
    transmit_pkt_tlv_t *tlv = transmit_item_get_pkt_buf(item,
        sizeof(transmit_pkt_tlv_t) + sizeof(uint16_t) + sizeof(transmit_data_reply_pkt_t) + block_size);
    if (tlv == NULL) {
        return ERRCODE_MALLOC;
    }

    while ((left_size) != 0) {
        read_size = uapi_min(left_size, block_size);
        data_size = (uint32_t)sizeof(transmit_data_reply_pkt_t) + (uint32_t)read_size;
        tlv_ext_len = transmit_get_tlv_ext_len(data_size);
        pkt_size = (uint32_t)sizeof(transmit_pkt_tlv_t) + tlv_ext_len + data_size;

        tlv->type =  transmit_build_tlv_type(1, 1);
        tlv->len = transmit_build_tlv_len(data_size);
        if (tlv_ext_len > 0) {
            (void)memcpy_s(tlv->data, tlv_ext_len, ((uint8_t *)&data_size), tlv_ext_len);
        }

        pkt = (transmit_data_reply_pkt_t *)((uint8_t *)tlv + sizeof(transmit_pkt_tlv_t) + tlv_ext_len);

        (void)memset_s(pkt->data, (uint32_t)read_size, 0, (uint32_t)read_size);
        readed_size = (int32_t)transmit_data_read(item, offset + send_size, pkt->data, (uint32_t)read_size);
        if (readed_size <= 0) {
            dfx_log_err("[ERR][transmit src] read data failed!, offset = 0x%x, read_size = %d readed_size = %d\r\n",
                offset + send_size, read_size, readed_size);
            transmit_send_failed_pkt(item->transmit_id, &item->option, item->down_machine);
            goto end;
        }

        pkt->transmit_id = item->transmit_id;
        pkt->offset = offset + (uint32_t)send_size;
        pkt->size = (uint32_t)readed_size;

        transmit_send_packet(DIAG_CMD_ID_TRANSMIT_REPLY, (uint8_t *)tlv, pkt_size, option, down_machine);

        left_size -= readed_size;
        send_size += readed_size;
    }
end:
    transmit_item_free_pkt_buf(item, (void *)tlv);
    return ERRCODE_SUCC;
}

/* 作为源端（读取数据并发送数据的一端）发送NEGOTIATE(协商)ACK帧 */
STATIC void transmit_src_item_send_negotiate_ack(transmit_item_t *item)
{
    uint32_t pkt_size = (uint32_t)sizeof(transmit_pkt_tlv_t) + (uint32_t)sizeof(transmit_negotiate_ack_pkt_t);
    transmit_pkt_tlv_t *tlv = transmit_item_get_pkt_buf(item, pkt_size);
    if (tlv == NULL) {
        return;
    }
    tlv->type =  transmit_build_tlv_type(1, 2); /* the type is assigned 2 in ack command */
    tlv->len = transmit_build_tlv_len(sizeof(transmit_negotiate_ack_pkt_t));
    transmit_negotiate_ack_pkt_t *pkt = (transmit_negotiate_ack_pkt_t *)tlv->data;

    (void)memset_s((uint8_t *)pkt, sizeof(transmit_negotiate_ack_pkt_t), 0, sizeof(transmit_negotiate_ack_pkt_t));
    pkt->transmit_id = item->transmit_id;
    pkt->data_block_number = item->data_block_number;
    pkt->data_block_size = item->data_block_size;
    pkt->info_size = 0;

    (void)transmit_send_packet(DIAG_CMD_ID_TRANSMIT_NEGOTIATE, (uint8_t *)tlv, pkt_size,
        &item->option, item->down_machine);
    transmit_item_free_pkt_buf(item, (void *)tlv);
}

STATIC void transmit_src_item_send_negotiate(transmit_item_t *item)
{
    uint32_t pkt_size = (uint32_t)sizeof(transmit_pkt_tlv_t) + (uint32_t)sizeof(transmit_negotiate_pkt_t);
    transmit_pkt_tlv_t *tlv = transmit_item_get_pkt_buf(item, pkt_size);
    if (tlv == NULL) {
        return;
    }
    tlv->type =  transmit_build_tlv_type(1, 1);
    tlv->len = transmit_build_tlv_len(sizeof(transmit_negotiate_pkt_t));
    transmit_negotiate_pkt_t *pkt = (transmit_negotiate_pkt_t *)tlv->data;

    (void)memset_s((uint8_t *)pkt, sizeof(transmit_negotiate_pkt_t), 0, sizeof(transmit_negotiate_pkt_t));
    pkt->transmit_id = item->transmit_id;
    pkt->src_send = 0;
    pkt->transmit_type = item->remote_type;
    pkt->total_size = item->total_size;

    (void)transmit_send_packet(DIAG_CMD_ID_TRANSMIT_NEGOTIATE, (uint8_t *)tlv, pkt_size,
        &item->option, item->down_machine);
    transmit_item_free_pkt_buf(item, (void *)tlv);
}

STATIC transmit_item_t *transmit_src_item_init(uint32_t transmit_id)
{
    transmit_item_t *item = transmit_item_match_id(transmit_id);
    if (item != NULL) {
        transmit_item_disable(item, 0);
        transmit_item_deinit(item);
    }

    item = transmit_item_init(transmit_id);
    return item;
}

STATIC void transmit_src_item_init_info(transmit_item_t *item, uint16_t transmit_type,
                                        uint32_t total_size, bool re_trans)
{
    unused(re_trans);
    transmit_item_init_permanent(item, false);
    transmit_item_init_local_start(item, false);
    transmit_item_init_local_src(item, true);
    transmit_item_init_remote_type(item, transmit_type);

    switch (transmit_type) {
        case TRANSMIT_TYPE_READ_FILE:
        case TRANSMIT_TYPE_DUMP:
        case TRANSMIT_TYPE_READ_MEMORY:
            transmit_item_init_local_type(item, TRANSMIT_LOCAL_TYPE_READ_FILE);
            break;
        case TRANSMIT_TYPE_READ_FLASH:
            transmit_item_init_local_type(item, TRANSMIT_LOCAL_TYPE_READ_DATA);
            break;
        default:
            break;
    }
    transmit_item_init_read_handler(item, file_read_data, (uintptr_t)item);
    transmit_item_init_total_size(item, total_size);
}

STATIC errcode_t transmit_src_item_start_negotiate_ack(transmit_item_t *item)
{
    if (transmit_item_init_is_success(item) == false) {
        transmit_item_deinit(item);
        dfx_log_err("[ERR][transmit src]init is failed\r\n");
        return ERRCODE_FAIL;
    }

#if (CONFIG_DFX_SUPPORT_FILE_SYSTEM == DFX_YES)
    item->file_fd = transmit_file_open_for_read(item->file_name);
    if (item->file_fd < 0) {
        dfx_log_err("[ERR][transmit src]file : %s open failed, fd = %d\r\n", item->file_name, item->file_fd);
        transmit_send_failed_pkt(item->transmit_id, &item->option, item->down_machine);
        item->file_fd = 0;
        transmit_item_deinit(item);
        return ERRCODE_FAIL;
    }
#endif

    transmit_item_enable(item);
    if (item->data_block_number == DEFAULT_TRANSMIT_BLOCK_NUMBER &&
        item->data_block_size == DEFAULT_TRANSMIT_BLOCK_SIZE) {
        transmit_src_item_send_negotiate(item);
    } else {
        transmit_src_item_send_negotiate_ack(item);
    }

    return ERRCODE_SUCC;
}

/* 作为源端（读取数据并发送数据的一端）处理REQUEST帧 */
errcode_t transmit_src_item_process_data_request_frame(transmit_data_request_pkt_t *request_pkt,
    diag_option_t *option, bool from_upper_machine)
{
    uint32_t i;
    uint32_t cur_time = dfx_get_cur_second();
    transmit_item_t *item = transmit_item_match_id(request_pkt->transmit_id);
    if (item == NULL) {
        dfx_log_err("[ERR][transmit src]request match id failed!, id = 0x%x\r\n", request_pkt->transmit_id);
        return transmit_send_invalid_id(request_pkt->transmit_id, option, from_upper_machine);
    }

    item->last_rcv_pkt_time = cur_time;
    item->last_send_pkt_time = cur_time;
    if ((item->local_start != 0) && (item->step == TRANSMIT_STEP_START)) {
        item->step = TRANSMIT_STEP_TRANSMIT;
    }

    for (i = 0; i < request_pkt->cnt; i++) {
        transmit_process_data_request_one(item, request_pkt->item[i].offset, request_pkt->item[i].size, option,
            from_upper_machine);
    }
    return ERRCODE_SUCC;
}

/* 作为源端（读取数据并发送数据的一端）处理START帧 */
errcode_t transmit_src_item_process_start_frame(transmit_start_pkt_t *start_pkt, diag_option_t *option,
    bool from_upper_machine)
{
    dfx_assert(start_pkt);
    transmit_save_info_t *file_info = (transmit_save_info_t *)start_pkt->info;
    transmit_save_data_info_t *data_info = (transmit_save_data_info_t *)start_pkt->info;

    transmit_item_t *item = transmit_src_item_init(start_pkt->transmit_id);
    if (item == NULL) {
        dfx_log_err("[ERR][transmit src]init failed\r\n");
        return ERRCODE_FAIL;
    }

    switch (start_pkt->transmit_type) {
        case TRANSMIT_TYPE_READ_FILE:
        case TRANSMIT_TYPE_DUMP:
        case TRANSMIT_TYPE_READ_MEMORY:
            transmit_src_item_init_info(item, start_pkt->transmit_type, data_info->size, start_pkt->re_trans);
            transmit_item_init_file_name(item, file_info->file_name, file_info->name_size);
            break;
        case TRANSMIT_TYPE_READ_FLASH:
            transmit_src_item_init_info(item, start_pkt->transmit_type, start_pkt->total_size, start_pkt->re_trans);
            transmit_item_init_local_bus_addr(item, data_info->start_addr);
            break;
        default:
            break;
    }
    transmit_item_init_option(item, option);
    transmit_item_init_down_machine(item, from_upper_machine);

    transmit_item_init_data_block_size(item, DEFAULT_TRANSMIT_BLOCK_SIZE);
    transmit_item_init_data_block_number(item, DEFAULT_TRANSMIT_BLOCK_NUMBER);

    return transmit_src_item_start_negotiate_ack(item);
}

/* 作为源端（读取数据并发送数据的一端）处理START帧 */
errcode_t transmit_src_item_process_negotiate_frame(transmit_negotiate_pkt_t *negotiate_pkt,
    diag_option_t *option, bool from_upper_machine)
{
    dfx_assert(negotiate_pkt);
    transmit_save_info_t *file_info = (transmit_save_info_t *)negotiate_pkt->info;

    transmit_item_t *item = transmit_src_item_init(negotiate_pkt->transmit_id);
    if (item == NULL) {
        dfx_log_err("[ERR][transmit src]init failed\r\n");
        return ERRCODE_FAIL;
    }

    transmit_src_item_init_info(item, negotiate_pkt->transmit_type, negotiate_pkt->total_size, negotiate_pkt->re_trans);

    transmit_item_init_option(item, option);
    transmit_item_init_down_machine(item, from_upper_machine);
    transmit_item_init_file_name(item, file_info->file_name, file_info->name_size);

    transmit_item_init_data_block_size(item, negotiate_pkt->data_block_size);
    transmit_item_init_data_block_number(item, negotiate_pkt->data_block_number);

    return transmit_src_item_start_negotiate_ack(item);
}

STATIC void zdiag_save_file_send_start_pkt(transmit_item_t *item)
{
    transmit_start_pkt_t *start_pkt = NULL;
    transmit_save_info_t *save_file_info = NULL;
    uint32_t name_size = 0;
    uint32_t info_size = 0;
    if (item->file_name != NULL) {
        name_size = (uint32_t)strlen(item->file_name) + 1;
        info_size = (uint32_t)sizeof(transmit_save_info_t) + name_size;
    }
    uint32_t data_size = (uint32_t)sizeof(transmit_start_pkt_t) + info_size;
    uint32_t tlv_ext_len = transmit_get_tlv_ext_len(data_size);
    uint32_t pkt_size = (uint32_t)sizeof(transmit_pkt_tlv_t) + tlv_ext_len + data_size;
    transmit_pkt_tlv_t *tlv = transmit_item_get_pkt_buf(item, pkt_size);
    if (tlv == NULL) {
        goto end;
    }

    tlv->type =  transmit_build_tlv_type(1, 1);
    tlv->len = transmit_build_tlv_len(data_size);
    if (tlv_ext_len > 0) {
        (void)memcpy_s(tlv->data, tlv_ext_len, ((uint8_t *)&data_size), tlv_ext_len);
    }

    start_pkt = (transmit_start_pkt_t *)((uint8_t *)tlv + sizeof(transmit_pkt_tlv_t) + tlv_ext_len);
    if (item->file_name != NULL) {
        save_file_info = (transmit_save_info_t *)start_pkt->info;
        save_file_info->name_size = (uint16_t)name_size;
        memcpy_s(save_file_info->file_name, name_size, item->file_name, name_size);
    }

    start_pkt->transmit_id = item->transmit_id;
    start_pkt->transmit_type = item->remote_type;
    start_pkt->total_size = item->total_size;
    start_pkt->info_size = info_size;
    start_pkt->pad = 0;
    start_pkt->re_trans = item->re_trans;
    start_pkt->src_send = 1;
    start_pkt->struct_ver = 0;
    transmit_send_packet(DIAG_CMD_ID_TRANSMIT_START, (uint8_t *)tlv, pkt_size, &item->option, item->down_machine);
end:
    transmit_item_free_pkt_buf(item, tlv);
    return;
}

void transmit_src_item_send_start_frame(transmit_item_t *item, uint32_t cur_time)
{
    switch (item->remote_type) {
        case TRANSMIT_TYPE_SAVE_FILE:
        case TRANSMIT_TYPE_SAVE_OTA_IMG:
        case TRANSMIT_TYPE_WRITE_MEMORY:
        case TRANSMIT_TYPE_WRITE_FLASH:
            zdiag_save_file_send_start_pkt(item);
            break;
        default:
            break;
    }
    item->last_send_pkt_time = cur_time;
    return;
}

void transmit_src_item_process_timer(transmit_item_t *item, uint32_t cur_time)
{
    uint32_t out_time = item->last_rcv_pkt_time + TRANSMIT_OUT_TIME;
    uint32_t retry_time = uapi_min(item->last_send_pkt_time, item->last_rcv_pkt_time) + TRANSMIT_RETRY_TIME;
    transmit_result_hook result_hook = (transmit_result_hook)item->result_hook;

    if (cur_time > out_time) {
        transmit_item_finish(item, TRANSMIT_DISABLE_TIME_OUT);
        if (result_hook != NULL && item->local_start == true) {
            result_hook(false, (uintptr_t)NULL);
        }
        return;
    }

    if ((item->local_start != 0) && (item->step == TRANSMIT_STEP_START) && (cur_time > retry_time)) {
        /* send pkt and modify retry time */
        transmit_src_item_send_start_frame(item, cur_time);
    }
}

errcode_t transmit_src_send_file_start(transmit_type_t transmit_type, diag_addr dst, uint32_t total_size,
    bool re_transmit, transmit_result_hook handler)
{
    diag_option_t option = DIAG_OPTION_INIT_VAL;
    option.peer_addr = dst;

    switch (transmit_type) {
        case TRANSMIT_TYPE_SAVE_FILE:
        case TRANSMIT_TYPE_SAVE_OTA_IMG:
        case TRANSMIT_TYPE_WRITE_MEMORY:
        case TRANSMIT_TYPE_WRITE_FLASH:
            break;
        default:
            return ERRCODE_INVALID_PARAM;
    }

    transmit_item_t *item = transmit_item_init(0);
    if (item == NULL) {
        return ERRCODE_FAIL;
    }

    transmit_item_init_permanent(item, false);
    transmit_item_init_local_start(item, true);
    transmit_item_init_local_src(item, true);
    transmit_item_init_remote_type(item, transmit_type);
    transmit_item_init_local_type(item, TRANSMIT_LOCAL_TYPE_READ_FILE);
    transmit_item_init_read_handler(item, file_read_data, (uintptr_t)item);
    transmit_item_init_option(item, &option);
    transmit_item_init_down_machine(item, false);
    transmit_item_init_total_size(item, total_size);
    transmit_item_init_result_handler(item, handler, (uintptr_t)item);
    transmit_item_init_re_trans(item, re_transmit);

    if (transmit_item_init_is_success(item) == false) {
        transmit_item_deinit(item);
        return ERRCODE_FAIL;
    }

    transmit_item_enable(item);
    return ERRCODE_SUCC;
}

errcode_t transmit_src_send_file_stop(transmit_type_t transmit_type, diag_addr dst)
{
    errcode_t ret;
    diag_option_t option = DIAG_OPTION_INIT_VAL;
    option.peer_addr = dst;
    transmit_item_t *item = transmit_item_match_type_and_dst(transmit_type, dst);
    if (item == NULL) {
        dfx_log_debug("stop match id failed!, type = 0x%x, dst = 0x%x\r\n", transmit_type, dst);
        return transmit_send_invalid_id(0, &option, false);
    }

    transmit_item_finish(item, TRANSMIT_DISABLE_USER_STOP);

    uint32_t pkt_size = (uint32_t)sizeof(transmit_pkt_tlv_t) + (uint32_t)sizeof(transmit_stop_pkt_t);
    transmit_pkt_tlv_t *tlv = transmit_item_get_pkt_buf(item, pkt_size);

    tlv->type =  transmit_build_tlv_type(1, 2); /* the type is assigned 2 in ack command */
    tlv->len = transmit_build_tlv_len(sizeof(transmit_stop_pkt_t));
    transmit_stop_pkt_t *stop_pkt = (transmit_stop_pkt_t *)tlv->data;

    stop_pkt->transmit_id = item->transmit_id;
    stop_pkt->reason = ERRCODE_SUCC;

    ret = transmit_send_packet(DIAG_CMD_ID_TRANSMIT_STOP, (uint8_t *)tlv, pkt_size, &option, false);
    transmit_item_free_pkt_buf(item, tlv);
    return ret;
}