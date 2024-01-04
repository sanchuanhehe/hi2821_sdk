/*
 * Copyright (c) @CompanyNameMagicTag 2021-2023. All rights reserved.
 * Description: transmit
 * This file should be changed only infrequently and with great care.
 */
#include "transmit_dst.h"
#include "transmit_st.h"
#include "transmit_item.h"
#include "transmit_send_recv_pkt.h"
#include "transmit_write_read.h"
#include "transmit_debug.h"
#include "transmit_cmd_id.h"
#include "zdiag_adapt_layer.h"
#include "uapi_crc.h"
#include "errcode.h"
#include "transmit_file_operation.h"
#include "transmit_resume.h"
#ifdef CONFIG_DFX_SUPPORT_PARTITION
#if (CONFIG_DFX_SUPPORT_PARTITION == DFX_YES)
#include "partition.h"
#endif
#endif
#if (CONFIG_DFX_SUPPORT_FILE_SYSTEM == DFX_YES)
#include "sys/vfs.h"
#endif

#define DFX_FILE_LEN            10
#define DFX_FS_HEAD_SPACE       16

STATIC errcode_t transmit_storage_init(transmit_item_t *item)
{
    unused(item);
#if (CONFIG_DFX_SUPPORT_FILE_SYSTEM == DFX_YES)
    item->file_fd = transmit_file_open_for_write(item->file_name);
    if (item->file_fd < 0) {
        return ERRCODE_FAIL;
    }
#endif
    return ERRCODE_SUCC;
}

STATIC errcode_t transmit_dst_item_update_data(transmit_item_t *item, uint32_t offset, uint8_t *buf, uint32_t size)
{
    dfx_assert(item);
    dfx_assert(buf);
    transmit_write_hook write_handler = (transmit_write_hook)item->write_read;
    transmit_result_hook result_hook = transmit_item_get_dst_result_hook();
    int32_t len;
    if (item->received_size != offset) {
        return ERRCODE_FAIL;
    }

    len = write_handler(item->usr_wr_data, offset, buf, size);
    if (len <= 0) {
        dfx_log_err("[ERR][transmit dst]write len %d\r\n", len);
#if (CONFIG_DFX_SUPPORT_FILE_SYSTEM == DFX_YES)
        transmit_file_close(item->file_fd);
        item->file_fd = transmit_file_open_for_rewrite(item->file_name);
        if (item->file_fd < 0) {
            dfx_log_err("[ERR][transmit dst]file : %s open failed, fd = %d\r\n", item->file_name, item->file_fd);
            transmit_send_failed_pkt(item->transmit_id, &item->option, item->down_machine);
            return ERRCODE_FAIL;
        }

        len = write_handler(item->usr_wr_data, offset, buf, size);
        if (len <= 0) {
            transmit_send_failed_pkt(item->transmit_id, &item->option, item->down_machine);
            return ERRCODE_FAIL;
        }
#else
        transmit_send_failed_pkt(item->transmit_id, &item->option, item->down_machine);
        return ERRCODE_FAIL;
#endif /* CONFIG_DFX_SUPPORT_FILE_SYSTEM */
    }

    item->received_size += (uint32_t)len;
    dfx_log_info("$$$$$$ received_size = 0x%x $$$$$$\r\n", item->received_size);
    if (item->received_size == item->total_size) {
        transmit_send_finish_pkt(item->transmit_id, &item->option, item->down_machine);
        transmit_item_finish(item, TRANSMIT_DISABLE_RECV_ALL_DATA);
        if (result_hook != NULL) {
            result_hook(true, (uintptr_t)NULL);
        }
    } else if (item->received_size == item->request_size) {
        uint32_t cur_time = dfx_get_cur_second();
#if (CONFIG_DFX_SUPPORT_CONTINUOUSLY_TRANSMIT == DFX_YES)
        transmit_record_progress(item->remote_type, item->received_size);
#endif
        transmit_dst_item_send_data_request_frame(item, cur_time);
    }
    return ERRCODE_SUCC;
}

errcode_t transmit_dst_item_process_data_reply_frame(transmit_data_reply_pkt_t *reply, diag_option_t *option,
    bool from_upper_machine)
{
    dfx_assert(reply);
    uint32_t cur_time = dfx_get_cur_second();
    transmit_item_t *item = transmit_item_match_id(reply->transmit_id);
    if (item == NULL) {
        dfx_log_err("[ERR][transmit dst]reply match id failed!, id = 0x%x\r\n", reply->transmit_id);
        return transmit_send_invalid_id(reply->transmit_id, option, from_upper_machine);
    }

    item->last_rcv_pkt_time = cur_time;

    return transmit_dst_item_update_data(item, reply->offset, reply->data, reply->size);
}

static void transmit_dst_item_init_info(transmit_item_t *item, uint16_t transmit_type,
                                        uint32_t total_size, bool re_trans)
{
    uint32_t offset = 0;
    transmit_item_init_permanent(item, false);
    transmit_item_init_local_start(item, false);
    transmit_item_init_local_src(item, false);
    transmit_item_init_local_type(item, TRANSMIT_LOCAL_TYPE_SAVE_FILE);
    transmit_item_init_write_handler(item, file_write_data, (uintptr_t)item);
    transmit_item_init_remote_type(item, transmit_type);
    if (re_trans) {
#if (CONFIG_DFX_SUPPORT_CONTINUOUSLY_TRANSMIT == DFX_YES)
        transmit_get_progress(transmit_type, &offset);
#endif
    }
    transmit_item_init_received_size(item, offset);
    transmit_item_init_total_size(item, total_size);
}

static bool transmit_item_check_free_space(transmit_item_t *item)
{
#if (CONFIG_DFX_SUPPORT_FILE_SYSTEM == DFX_YES)
    struct statfs sfs;
    int result;
    uint8_t file_path[DFX_FILE_LEN] = {0};
    uint64_t free_size;
    (void)memset_s(&sfs, sizeof(sfs), 0, sizeof(sfs));

    for (int i = 0; i < DFX_FILE_LEN; i++) {
        file_path[i] = (uint8_t)(item->file_name[i]);

        if (file_path[i] == '/' && i != 0) {
            break;
        }
    }

    result = statfs((const char *)file_path, &sfs);
    if (result != 0 || sfs.f_type == 0) {
        dfx_log_err("statfs failed. file_path = %s\r\n", file_path);
        return false;
    }

#ifdef CFG_DRIVERS_NANDFLASH
    free_size = (uint64_t)sfs.f_bsize * (sfs.f_bfree - DFX_FS_HEAD_SPACE);
#else
    free_size = (uint64_t)sfs.f_bsize * sfs.f_bfree;
#endif

    return ((uint64_t)item->total_size > free_size) ? false : true;
#else
    return true;
#endif
}

static transmit_item_t *transmit_dst_item_init(uint32_t transmit_id)
{
    transmit_item_t *item = transmit_item_match_id(transmit_id);
    if (item != NULL) {
        transmit_item_disable(item, 0);
        transmit_item_deinit(item);
    }

    item = transmit_item_init(transmit_id);
    return item;
}

static errcode_t transmit_dst_item_start_transmit_request(transmit_item_t *item)
{
    uint32_t cur_time = dfx_get_cur_second();

    if (transmit_item_init_is_success(item) == false) {
        transmit_item_deinit(item);
        dfx_log_err("[ERR][transmit dst]init is failed\r\n");
        return ERRCODE_FAIL;
    }

    if (transmit_item_check_free_space(item) == false) {
        dfx_log_err("[ERR][transmit dst]no enough space\r\n");
        transmit_send_failed_pkt(item->transmit_id, &item->option, item->down_machine);
        transmit_item_deinit(item);
        return ERRCODE_FAIL;
    }

    if (transmit_storage_init(item) != ERRCODE_SUCC) {
        dfx_log_err("[ERR][transmit dst]file : %s open failed, fd = %d\r\n", item->file_name, item->file_fd);
        transmit_send_failed_pkt(item->transmit_id, &item->option, item->down_machine);
        transmit_item_deinit(item);
        return ERRCODE_FAIL;
    }

    transmit_item_enable(item);
    if (item->total_size == 0) {
        transmit_send_finish_pkt(item->transmit_id, &item->option, item->down_machine);
        transmit_item_finish(item, TRANSMIT_DISABLE_RECV_ALL_DATA);
    } else {
        transmit_dst_item_send_data_request_frame(item, cur_time);
    }
    return ERRCODE_SUCC;
}

/* 作为目的端（接收并保存数据的一端）处理START帧 */
errcode_t transmit_dst_item_process_start_frame(transmit_start_pkt_t *start_pkt, diag_option_t *option,
    bool from_upper_machine)
{
    dfx_assert(start_pkt);

    transmit_item_t *item = transmit_dst_item_init(start_pkt->transmit_id);
    if (item == NULL) {
        dfx_log_err("[ERR][transmit dst]init failed\r\n");
        return ERRCODE_FAIL;
    }

    transmit_dst_item_init_info(item, start_pkt->transmit_type, start_pkt->total_size, start_pkt->re_trans);

    transmit_item_init_option(item, option);
    transmit_item_init_down_machine(item, from_upper_machine);

    transmit_item_init_data_block_size(item, DEFAULT_TRANSMIT_BLOCK_SIZE);
    transmit_item_init_data_block_number(item, DEFAULT_TRANSMIT_BLOCK_NUMBER);

    if (start_pkt->info_size != 0) {
        transmit_save_info_t *save_file_info = (transmit_save_info_t *)start_pkt->info;
        transmit_item_init_file_name(item, save_file_info->file_name, save_file_info->name_size);
    }

    return transmit_dst_item_start_transmit_request(item);
}

/* 作为目的端（接收并保存数据的一端）处理NEGOTIATE帧 */
errcode_t transmit_dst_item_process_negotiate_frame(transmit_negotiate_pkt_t *negotiate_pkt, diag_option_t *option,
    bool from_upper_machine)
{
    dfx_assert(negotiate_pkt);

    transmit_item_t *item = transmit_dst_item_init(negotiate_pkt->transmit_id);
    if (item == NULL) {
        dfx_log_err("[ERR][transmit dst]init failed\r\n");
        return ERRCODE_FAIL;
    }

    transmit_dst_item_init_info(item, negotiate_pkt->transmit_type, negotiate_pkt->total_size, negotiate_pkt->re_trans);

    transmit_item_init_option(item, option);
    transmit_item_init_down_machine(item, from_upper_machine);
    transmit_item_init_data_block_size(item, negotiate_pkt->data_block_size);
    transmit_item_init_data_block_number(item, negotiate_pkt->data_block_number);

    if (negotiate_pkt->info_size != 0) {
        transmit_save_info_t *save_file_info = (transmit_save_info_t *)negotiate_pkt->info;
        transmit_item_init_file_name(item, save_file_info->file_name, save_file_info->name_size);
    }

    return transmit_dst_item_start_transmit_request(item);
}

errcode_t transmit_dst_item_process_stop_frame(transmit_stop_pkt_t *pkt, diag_option_t *option,
    bool from_upper_machine)
{
    errcode_t ret;
    transmit_item_t *item = transmit_item_match_id(pkt->transmit_id);
    if (item == NULL) {
        dfx_log_debug("stop match id failed!, id = 0x%x\r\n", pkt->transmit_id);
        return transmit_send_invalid_id(pkt->transmit_id, option, from_upper_machine);
    }

    transmit_item_finish(item, TRANSMIT_DISABLE_USER_STOP);

    uint32_t pkt_size = (uint32_t)sizeof(transmit_pkt_tlv_t) + (uint32_t)sizeof(transmit_stop_pkt_t);
    transmit_pkt_tlv_t *tlv = transmit_item_get_pkt_buf(item, pkt_size);
    if (tlv == NULL) {
        return ERRCODE_MALLOC;
    }
    tlv->type =  transmit_build_tlv_type(1, 2); /* the type is assigned 2 in ack command */
    tlv->len = transmit_build_tlv_len(sizeof(transmit_stop_pkt_t));
    transmit_stop_pkt_t *stop_pkt = (transmit_stop_pkt_t *)tlv->data;

    stop_pkt->transmit_id = pkt->transmit_id;
    stop_pkt->reason = ERRCODE_SUCC;

    ret = transmit_send_packet(DIAG_CMD_ID_TRANSMIT_STOP, (uint8_t *)tlv, pkt_size, option, from_upper_machine);
    transmit_item_free_pkt_buf(item, tlv);
    return ret;
}

void transmit_dst_item_send_data_request_frame(transmit_item_t *item, uint32_t cur_time)
{
    dfx_assert(item);

    uint32_t request_size_calc = item->data_block_size * item->data_block_number;
    if (request_size_calc == 0) {
        request_size_calc = DEFAULT_TRANSMIT_BLOCK_SIZE * DEFAULT_TRANSMIT_BLOCK_NUMBER;
    }
    uint32_t request_size = uapi_min(item->total_size - item->received_size, request_size_calc);
    uint32_t pkt_size = (uint32_t)sizeof(transmit_pkt_tlv_t) +
        (uint32_t)sizeof(transmit_data_request_pkt_t) + (uint32_t)sizeof(transmit_data_request_item_t);
    transmit_pkt_tlv_t *tlv = transmit_item_get_pkt_buf(item, pkt_size);
    if (tlv == NULL) {
        return;
    }
    tlv->type =  transmit_build_tlv_type(1, 1);
    tlv->len = transmit_build_tlv_len(sizeof(transmit_data_request_pkt_t) + sizeof(transmit_data_request_item_t));
    transmit_data_request_pkt_t *pkt = (transmit_data_request_pkt_t *)tlv->data;
    pkt->transmit_id = item->transmit_id;
    pkt->cnt = 1;
    pkt->item[0].offset = item->received_size;
    pkt->item[0].size = request_size;

    item->last_send_pkt_time = cur_time;
    item->request_size = item->received_size + request_size;
    (void)transmit_send_packet(DIAG_CMD_ID_TRANSMIT_REQUEST, (uint8_t *)tlv, pkt_size, &item->option,
                               item->down_machine);

    transmit_item_free_pkt_buf(item, tlv);
}

void transmit_dst_item_send_start_frame(transmit_item_t *item, uint32_t cur_time)
{
    dfx_assert(item);
    item->last_send_pkt_time = cur_time;
    return;
}

void transmit_dst_item_process_timer(transmit_item_t *item, uint32_t cur_time)
{
    dfx_assert(item);
    uint32_t out_time = item->last_rcv_pkt_time + TRANSMIT_OUT_TIME;
    uint32_t retry_time = uapi_min(item->last_send_pkt_time, item->last_rcv_pkt_time) + TRANSMIT_RETRY_TIME;
    transmit_result_hook result_hook = transmit_item_get_dst_result_hook();

    if (cur_time > out_time) {
        dfx_log_err("[ERR][transmit dst] receive frame timeout.\r\n");
        transmit_item_finish(item, TRANSMIT_DISABLE_TIME_OUT);
        if (result_hook != NULL) {
            result_hook(false, (uintptr_t)NULL);
        }
        return;
    }

    if ((item->local_start != 0) && (item->step == TRANSMIT_STEP_START) && (cur_time > retry_time)) {
        /* resend start frame */
        transmit_dst_item_send_start_frame(item, cur_time);
    }

    if ((item->step == TRANSMIT_STEP_TRANSMIT) && (cur_time > retry_time)) {
        /* resend data request frame */
        dfx_log_err("!!!!!!!!!!re request 0x%x 0x%0x\r\n", item->received_size,
            item->total_size - item->received_size);
        transmit_dst_item_send_data_request_frame(item, cur_time);
    }
}
