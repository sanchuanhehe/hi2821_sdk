/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: transmit data
 * This file should be changed only infrequently and with great care.
 */
#include "transmit_item.h"
#include "securec.h"
#include "transmit_st.h"
#include "transmit_send_recv_pkt.h"
#include "soc_osal.h"
#include "zdiag_adapt_layer.h"
#include "transmit_src.h"
#include "transmit_dst.h"
#include "dfx_adapt_layer.h"
#include "errcode.h"
#include "transmit_file_operation.h"
#include "transmit_debug.h"
#include "transmit_cmd_id.h"
#include "transmit.h"
#include "diag_msg.h"
#include "diag_dfx.h"

#define TRANSMIT_ITEM_CNT 3
#define TRANSMIT_TIMER_PERIOD 1000
#define TRANSMIT_ITEM_BUF_LEN 0x600
#define TRANSMIT_MSG_PROC_MAX 10

typedef struct {
    void *is_busy;
    uint32_t transmit_id;
    uint8_t used_cnt;
    uint8_t period_cnt;
    uint32_t buf_used : 1;
    uint32_t pkt_size;
    uint8_t *pkt_buf;
    uint8_t timer_cnt;
    uintptr_t dst_result_hook;
    osal_timer timer;
    transmit_item_t item[TRANSMIT_ITEM_CNT];
} transmit_ctrl_t;

transmit_ctrl_t g_transmit_ctrl;
static transmit_msg_proc_t g_transmit_msg_proc[TRANSMIT_MSG_PROC_MAX] = {0};

STATIC void transmit_timer_stop(void);
errcode_t transmit_send_invalid_id(uint32_t transmit_id, diag_option_t *option, bool down_machine)
{
    errcode_t ret;
    uint32_t pkt_size = (uint32_t)sizeof(transmit_pkt_tlv_t) + (uint32_t)sizeof(transmit_state_notify_pkt_t);
    transmit_pkt_tlv_t *tlv = transmit_item_get_pkt_buf(NULL, pkt_size);
    if (tlv == NULL) {
        return ERRCODE_MALLOC;
    }
    tlv->type =  transmit_build_tlv_type(1, 1);
    tlv->len = transmit_build_tlv_len(sizeof(transmit_state_notify_pkt_t));
    transmit_state_notify_pkt_t *pkt = (transmit_state_notify_pkt_t *)tlv->data;

    pkt->transmit_id = transmit_id;
    pkt->state_code = TRANSMIT_STATE_NOTIFY_INVALID_ID;
    pkt->len = 0;
    ret = transmit_send_packet(DIAG_CMD_ID_TRANSMIT_NOTIFY, (uint8_t *)tlv, pkt_size, option, down_machine);
    transmit_item_free_pkt_buf(NULL, tlv);
    return ret;
}

errcode_t transmit_send_finish_pkt(uint32_t transmit_id, diag_option_t *option, bool down_machine)
{
    errcode_t ret;
    uint32_t pkt_size = (uint32_t)sizeof(transmit_pkt_tlv_t) + (uint32_t)sizeof(transmit_state_notify_pkt_t);
    transmit_pkt_tlv_t *tlv = transmit_item_get_pkt_buf(NULL, pkt_size);
    if (tlv == NULL) {
        return ERRCODE_MALLOC;
    }
    tlv->type =  transmit_build_tlv_type(1, 1);
    tlv->len = transmit_build_tlv_len(sizeof(transmit_state_notify_pkt_t));
    transmit_state_notify_pkt_t *pkt = (transmit_state_notify_pkt_t *)tlv->data;

    pkt->transmit_id = transmit_id;
    pkt->state_code = TRANSMIT_STATE_NOTIFY_FINISH;
    pkt->len = 0;
    ret = transmit_send_packet(DIAG_CMD_ID_TRANSMIT_NOTIFY, (uint8_t *)tlv, pkt_size, option, down_machine);
    transmit_item_free_pkt_buf(NULL, tlv);
    return ret;
}

errcode_t transmit_send_failed_pkt(uint32_t transmit_id, diag_option_t *option, bool down_machine)
{
    errcode_t ret;
    uint32_t pkt_size = (uint32_t)sizeof(transmit_pkt_tlv_t) + (uint32_t)sizeof(transmit_state_notify_pkt_t);
    transmit_pkt_tlv_t *tlv = transmit_item_get_pkt_buf(NULL, pkt_size);
    if (tlv == NULL) {
        return ERRCODE_MALLOC;
    }
    tlv->type =  transmit_build_tlv_type(1, 1);
    tlv->len = transmit_build_tlv_len(sizeof(transmit_state_notify_pkt_t));
    transmit_state_notify_pkt_t *pkt = (transmit_state_notify_pkt_t *)tlv->data;

    pkt->transmit_id = transmit_id;
    pkt->state_code = TRANSMIT_STATE_NOTIFY_SAVE_FAILED_ID;
    pkt->len = 0;
    ret = transmit_send_packet(DIAG_CMD_ID_TRANSMIT_NOTIFY, (uint8_t *)tlv, pkt_size, option, down_machine);
    transmit_item_free_pkt_buf(NULL, tlv);
    return ret;
}

void transmit_item_process_notify_frame(transmit_state_notify_pkt_t *pkt, diag_option_t *option,
    bool from_upper_machine)
{
    dfx_assert(pkt);
    transmit_item_t *item = transmit_item_match_id(pkt->transmit_id);
    if (item == NULL) {
        if (pkt->state_code != TRANSMIT_STATE_NOTIFY_INVALID_ID) {
            dfx_log_err("[ERR]notify_frame match id failed!, id = 0x%x\r\n", pkt->transmit_id);
            transmit_send_invalid_id(pkt->transmit_id, option, from_upper_machine);
        }
        return;
    }
    transmit_result_hook result_hook = (transmit_result_hook)item->result_hook;

    switch (pkt->state_code) {
        case TRANSMIT_STATE_NOTIFY_INVALID_ID:
            transmit_item_finish(item, TRANSMIT_DISABLE_RECV_INVALID_ID);
            break;
        case TRANSMIT_STATE_NOTIFY_FINISH:
        case TRANSMIT_STATE_NOTIFY_FINISH_2:
            transmit_item_finish(item, TRANSMIT_DISABLE_RECV_FINISH);
            if (item->remote_type == TRANSMIT_TYPE_READ_FILE || item->remote_type == TRANSMIT_TYPE_DUMP) {
                transmit_send_finish_pkt(item->transmit_id, &item->option, item->down_machine);
            }
            if (result_hook != NULL && item->local_start == true) {
                result_hook(true, (uintptr_t)NULL);
            }
            break;
        case TRANSMIT_STATE_NOTIFY_DUPLICATE_ID:
            break;
        default:
            break;
    }
}

transmit_item_t *transmit_item_match_id(uint32_t transmit_id)
{
    transmit_item_t *item = NULL;
    for (int i = 0; i < TRANSMIT_ITEM_CNT; i++) {
        item = &g_transmit_ctrl.item[i];
        if ((item->used != 0) && (item->transmit_id == transmit_id) && (transmit_id != 0)) {
            return item;
        }
    }
    return NULL;
}

transmit_item_t *transmit_item_match_type_and_dst(transmit_type_t transmit_type, diag_addr dst)
{
    transmit_item_t *item = NULL;
    for (int i = 0; i < TRANSMIT_ITEM_CNT; i++) {
        item = &g_transmit_ctrl.item[i];
        if ((item->used != 0) && (item->remote_type == transmit_type) && (item->option.peer_addr == dst)) {
            return item;
        }
    }
    return NULL;
}

transmit_item_t *transmit_item_init(uint32_t transmit_id)
{
    transmit_item_t *item = NULL;
    for (int i = 0; i < TRANSMIT_ITEM_CNT; i++) {
        item = &g_transmit_ctrl.item[i];
        if (item->used == true) {
            continue;
        }
        memset_s(item, sizeof(transmit_item_t), 0x0, sizeof(transmit_item_t));

        if (transmit_id == 0) {
            item->transmit_id = g_transmit_ctrl.transmit_id++;
        } else {
            item->transmit_id = transmit_id;
        }

        item->used = true;
        if (item->pm_veto == 0) {
            (void)dfx_pm_add_sleep_veto();
            item->pm_veto = 1;
        }
        return item;
    }
    return NULL;
}

void transmit_item_init_permanent(transmit_item_t *item, bool permanent)
{
    dfx_assert(item);
    item->permanent = permanent;
}

void transmit_item_init_local_start(transmit_item_t *item, bool local_start)
{
    dfx_assert(item);
    item->local_start = local_start;
}

void transmit_item_init_local_src(transmit_item_t *item, bool local_src)
{
    dfx_assert(item);
    item->local_src = local_src;
}

void transmit_item_init_remote_type(transmit_item_t *item, transmit_type_t remote_type)
{
    dfx_assert(item);
    item->remote_type = remote_type;
}

void transmit_item_init_local_type(transmit_item_t *item, transmit_local_type_t local_type)
{
    dfx_assert(item);
    item->local_type = local_type;
}

void transmit_item_init_write_handler(transmit_item_t *item, transmit_write_hook handler, uintptr_t usr_data)
{
    dfx_assert(item);
    item->write_read = (uintptr_t)handler;
    item->usr_wr_data = usr_data;
}

void transmit_item_init_read_handler(transmit_item_t *item, transmit_read_hook handler, uintptr_t usr_data)
{
    dfx_assert(item);
    item->write_read = (uintptr_t)handler;
    item->usr_wr_data = usr_data;
}

void transmit_item_init_result_handler(transmit_item_t *item, transmit_result_hook handler, uintptr_t usr_data)
{
    dfx_assert(item);
    unused(usr_data);
    item->result_hook = (uintptr_t)handler;
}

void transmit_item_init_local_bus_addr(transmit_item_t *item, uint32_t bus_addr)
{
    dfx_assert(item);
    item->bus_addr = bus_addr;
}

void transmit_item_init_file_name(transmit_item_t *item, const char *file_name, uint16_t name_len)
{
    dfx_assert(file_name);
    dfx_assert(item);
    uint32_t name_size = name_len + 1;
    item->file_name = dfx_malloc(0, name_size);
    if (item->file_name == NULL) {
        item->init_fail = true;
        return;
    }
    memset_s(item->file_name, name_size, 0, name_size);
    memcpy_s(item->file_name, name_len, file_name, name_len);
}

void transmit_item_init_option(transmit_item_t *item, diag_option_t *option)
{
    dfx_assert(item);
    item->option = *option;
}

void transmit_item_init_down_machine(transmit_item_t *item, bool down_machine)
{
    dfx_assert(item);
    item->down_machine = down_machine;
}

void transmit_item_init_received_size(transmit_item_t *item, uint32_t offset)
{
    dfx_assert(item);
    item->received_size = offset;
}

void transmit_item_init_total_size(transmit_item_t *item, uint32_t total_size)
{
    dfx_assert(item);
    item->total_size = total_size;
}

void transmit_item_init_re_trans(transmit_item_t *item, bool re_trans)
{
    dfx_assert(item);
    item->re_trans = re_trans;
}

void transmit_item_init_data_block_size(transmit_item_t *item, uint16_t data_block_size)
{
    dfx_assert(item);
    item->data_block_size = data_block_size;
}

void transmit_item_init_data_block_number(transmit_item_t *item, uint16_t data_block_number)
{
    dfx_assert(item);
    item->data_block_number = data_block_number;
}

bool transmit_item_init_is_success(const transmit_item_t *item)
{
    dfx_assert(item);
    if (item->init_fail != 0) {
        return false;
    } else {
        return true;
    }
}

void transmit_item_deinit(transmit_item_t *item)
{
    dfx_assert(item);
    if (item->file_name) {
        dfx_free(0, item->file_name);
        item->file_name = NULL;
    }

    if (item->pm_veto == 1) {
        (void)dfx_pm_remove_sleep_veto();
        item->pm_veto = 0;
    }

    item->used = false;
}

void transmit_item_disable(transmit_item_t *item, transmit_disable_reason_t reason)
{
    dfx_assert(item);
    if (item->permanent == false) {
        g_transmit_ctrl.timer_cnt--;
        if (g_transmit_ctrl.timer_cnt == 0) {
            transmit_timer_stop();
        }
    }
    unused(reason);
    item->enable = false;
}

void transmit_item_enable(transmit_item_t *item)
{
    dfx_assert(item);
    uint32_t cur_time = dfx_get_cur_second();

    if ((item->local_start) != 0) {
        item->step = TRANSMIT_STEP_START;
        transmit_src_item_send_start_frame(item, cur_time);
    } else {
        item->step = TRANSMIT_STEP_TRANSMIT;
    }

    if (item->permanent == false) {
        item->last_rcv_pkt_time = cur_time;
        item->last_send_pkt_time = cur_time;
        item->expiration = cur_time + TRANSMIT_RETRY_TIME;

        g_transmit_ctrl.timer_cnt++;
        if (g_transmit_ctrl.timer_cnt == 1) {
            transmit_timer_start();
        }
    }
    item->enable = true;
}

void transmit_item_finish(transmit_item_t *item, transmit_disable_reason_t reason)
{
    dfx_assert(item);
    transmit_item_disable(item, reason);
    if (item->file_fd != 0) {
#if CONFIG_DFX_SUPPORT_FILE_SYSTEM == DFX_YES
        transmit_file_fsync(item->file_fd);
        transmit_file_close(item->file_fd);
#endif
        item->file_fd = 0;
    }

    transmit_item_deinit(item);
}

void* transmit_item_get_pkt_buf(const transmit_item_t *item, uint32_t buf_size)
{
    dfx_assert(item);
    unused(item);

    if (g_transmit_ctrl.pkt_size == 0) {
        g_transmit_ctrl.pkt_size = TRANSMIT_ITEM_BUF_LEN;
        g_transmit_ctrl.pkt_buf = dfx_malloc(0, TRANSMIT_ITEM_BUF_LEN);
    }

    if ((buf_size < g_transmit_ctrl.pkt_size) && (g_transmit_ctrl.buf_used == 0)) {
        g_transmit_ctrl.buf_used = true;
        return g_transmit_ctrl.pkt_buf;
    }
    return NULL;
}

void transmit_item_free_pkt_buf(const transmit_item_t *item, const void *buf)
{
    g_transmit_ctrl.buf_used = false;

    unused(item);
    unused(buf);
}

transmit_result_hook transmit_item_get_dst_result_hook(void)
{
    return (transmit_result_hook)g_transmit_ctrl.dst_result_hook;
}

errcode_t transmit_item_dst_result_hook_register(transmit_result_hook dst_result_hook)
{
    if (((void *)dst_result_hook == NULL) || ((void *)g_transmit_ctrl.dst_result_hook != NULL)) {
        return ERRCODE_FAIL;
    }
    g_transmit_ctrl.dst_result_hook = (uintptr_t)dst_result_hook;
    return ERRCODE_SUCC;
}

void transmit_item_dst_result_hook_unregister(void)
{
    g_transmit_ctrl.dst_result_hook = (uintptr_t)NULL;
}

typedef struct {
    osal_timer g_transmit_timer;
    bool start;
} transmit_timer_t;

transmit_timer_t g_transmit_timer;


STATIC void transmit_item_time_out(transmit_item_t *item, uint32_t cur_time)
{
    dfx_assert(item);
    if ((item->local_src) != 0) {
        transmit_src_item_process_timer(item, cur_time);
    } else {
        transmit_dst_item_process_timer(item, cur_time);
    }
}
STATIC void transmit_period_msg_proc(void)
{
    transmit_item_t *item = NULL;
    uint32_t cur_sec = dfx_get_cur_second();
    for (int i = 0; i < TRANSMIT_ITEM_CNT; i++) {
        item = &g_transmit_ctrl.item[i];
        if ((item->enable != 0) && (item->permanent == false) && (cur_sec > item->expiration)) {
            transmit_item_time_out(item, cur_sec);
        }
    }
    return;
}

errcode_t transmit_msg_proc_hook_register(uint32_t msg_id_start, uint32_t msg_id_end, transmit_register_hook hook)
{
    if (msg_id_start >= msg_id_end || hook == NULL) {
        return ERRCODE_FAIL;
    }
    transmit_msg_proc_t *msg_proc;
    for (int i = 0; i < TRANSMIT_MSG_PROC_MAX; i++) {
        msg_proc = &g_transmit_msg_proc[i];
        if (!((msg_id_end < msg_proc->id_start) || (msg_id_start > msg_proc->id_end))) {
            // 有msg_id范围重叠
            return ERRCODE_FAIL;
        }
        if (msg_proc->used != 1) {
            msg_proc->id_start = msg_id_start;
            msg_proc->id_end = msg_id_end;
            msg_proc->used = 1;
            msg_proc->hook = (uintptr_t)hook;
            return ERRCODE_SUCC;
        }
    }
    return ERRCODE_FAIL;
}

errcode_t transmit_msg_proc_hook_unregister(transmit_register_hook hook)
{
    transmit_msg_proc_t *msg_proc;
    for (int i = 0; i < TRANSMIT_MSG_PROC_MAX; i++) {
        msg_proc = &g_transmit_msg_proc[i];
        if ((msg_proc->used == 1) && (((transmit_register_hook)msg_proc->hook) == hook)) {
            msg_proc->id_start = 0;
            msg_proc->id_end = 0;
            msg_proc->used = 0;
            msg_proc->hook = (uintptr_t)NULL;
            return ERRCODE_SUCC;
        }
    }
    return ERRCODE_FAIL;
}

static errcode_t transmit_run_hook(uint32_t msg_id, const uint8_t *msg, uint32_t msg_len)
{
    transmit_msg_proc_t *msg_proc;
    for (int i = 0; i < TRANSMIT_MSG_PROC_MAX; i++) {
        msg_proc = &g_transmit_msg_proc[i];
        if ((msg_proc->used == 1) && (msg_id > msg_proc->id_start) &&
            (msg_id < msg_proc->id_end) && (((transmit_register_hook)msg_proc->hook) != NULL)) {
            ((transmit_register_hook)msg_proc->hook)(msg_id, msg, msg_len);
            return ERRCODE_SUCC;
        }
    }
    return ERRCODE_FAIL;
}

errcode_t transmit_msg_proc(uint32_t msg_id, const uint8_t *msg, uint32_t msg_len)
{
    diag_dfx_transmit_rev_msg();

    if (msg_id > DFX_MSG_ID_RESERVE_MAX) {
        return transmit_run_hook(msg_id, msg, msg_len);
    }

    switch (msg_id) {
        case DFX_MSG_ID_DIAG_PKT:
            diag_msg_proc((uint16_t)msg_id, (uint8_t *)msg, msg_len);
            break;
        case DFX_MSG_ID_TRANSMIT_FILE:
            transmit_period_msg_proc();
            break;
        default:
            break;
    }
    return ERRCODE_SUCC;
}

STATIC void transmit_timer_handler(unsigned long data)
{
    osal_timer *timer = &g_transmit_ctrl.timer;

    transmit_msg_write(DFX_MSG_ID_TRANSMIT_FILE, (uint8_t *)(uintptr_t)data, sizeof(data), false);

    if ((g_transmit_ctrl.timer_cnt) != 0) {
        osal_timer_start(timer);
    }
}

void transmit_timer_start(void)
{
    osal_timer *timer = &g_transmit_ctrl.timer;
    osal_timer_start(timer);
}

STATIC void transmit_timer_stop(void) {}
STATIC errcode_t transmit_timer_init(void)
{
    osal_timer *timer = &g_transmit_ctrl.timer;
    timer->handler = transmit_timer_handler;
    timer->data = 0;
    timer->interval = TRANSMIT_TIMER_PERIOD;
    if (osal_timer_init(timer) < 0) {
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

errcode_t transmit_item_module_init(void)
{
    errcode_t ret;
    ret = transmit_timer_init();
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    g_transmit_ctrl.transmit_id = 0x10;
    return ERRCODE_SUCC;
}
