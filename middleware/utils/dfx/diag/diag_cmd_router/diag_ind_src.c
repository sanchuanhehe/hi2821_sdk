/*
 * Copyright (c) @CompanyNameMagicTag 2021-2023. All rights reserved.
 * Description: zdiag ind producer
 * This file should be changed only infrequently and with great care.
 */

#include "diag_ind_src.h"
#include "securec.h"
#include "diag_mem.h"
#include "diag_pkt_router.h"
#include "diag_debug.h"
#include "soc_diag_cmd_id.h"
#include "zdiag_adapt_layer.h"
#include "diag_dfx.h"
#include "diag_filter.h"
#include "dfx_adapt_layer.h"
#include "diag_pkt.h"
#include "errcode.h"
#include "log_file.h"
#include "diag_msg.h"
#ifdef CONFIG_DIAG_WITH_SECURE
#include "diag_secure.h"
#endif

#if (CONFIG_DFX_SUPPORT_DIAG_MINIMAL_MSG == DFX_YES)
    #ifndef DIAG_CMD_MSG_RPT_SYS
    #define DIAG_CMD_MSG_RPT_SYS DIAG_CMD_MSG_RPT_SYS_MINI_LOG
    #endif
    #define DIAG_LOG_HEADER_SIZE DIAG_MINIMAL_LOG_HEADER_SIZE
#elif (CONFIG_DFX_SUPPORT_DIAG_NORMAL_MSG == DFX_YES)
    #ifndef DIAG_CMD_MSG_RPT_SYS
    #define DIAG_CMD_MSG_RPT_SYS DIAG_CMD_MSG_RPT_SYS_NORMAL_LOG
    #endif
    #define DIAG_LOG_HEADER_SIZE DIAG_NORMAL_LOG_HEADER_SIZE
#elif (CONFIG_DFX_SUPPORT_DIAG_EXTEND_MSG == DFX_YES)
    #ifndef DIAG_CMD_MSG_RPT_SYS
    #define DIAG_CMD_MSG_RPT_SYS DIAG_CMD_MSG_RPT_SYS_EXTEND_LOG
    #endif
    #define DIAG_LOG_HEADER_SIZE DIAG_EXTEND_LOG_HEADER_SIZE
#else
    #ifndef DIAG_CMD_MSG_RPT_SYS
    #define DIAG_CMD_MSG_RPT_SYS DIAG_CMD_MSG_RPT_SYS_FULLY_LOG
    #endif
    #define DIAG_LOG_HEADER_SIZE DIAG_FULL_LOG_HEADER_SIZE
#endif

#define PKT_DATA_CNT_2 2

static diag_msg_flow_control_hook g_flow_control_hook = NULL;

STATIC bool zdiag_msg_permit(uint32_t module_id, uint8_t level)
{
    if ((zdiag_is_enable() == false) && (uapi_zdiag_offline_log_is_enable() != true)) {
        return false;
    }
    return zdiag_log_enable(level, module_id);
}

#ifdef CONFIG_DIAG_WITH_SECURE
static errcode_t diag_check_secure_connect(void)
{
    diag_secure_ctx_t *secure_ctx = diag_get_secure_ctx();
    uint8_t key_check[AES128_KEY_LEN];
    memset_s(key_check, AES128_KEY_LEN, 0, AES128_KEY_LEN);
    if (memcmp(secure_ctx->srp_info.key, key_check, AES128_KEY_LEN) != 0) {
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

static errcode_t uapi_diag_report_packet_secure(uint16_t cmd_id, diag_option_t *option, const uint8_t *packet,
    uint16_t packet_size, bool sync)
{
    diag_pkt_handle_t pkt;
    diag_pkt_process_param_t process_param = {.dst_addr = DIAG_FRAME_FID_PC, .src_addr = DIAG_FRAME_FID_MCU};
    if (option != NULL) {
        process_param.dst_addr = option->peer_addr;
    }
    uint16_t iv_tag_ext_size = 0;
    bool need_to_encrypt = diag_need_secure(cmd_id);
    if (need_to_encrypt) {
        if (diag_check_secure_connect() != ERRCODE_SUCC) {
            return ERRCODE_FAIL;
        }
        iv_tag_ext_size = AES_IV_LEN + AES_TAG_LEN;
    }
    uint8_t buf[DIAG_ROUTER_HEADER_LEN + iv_tag_ext_size];
    diag_router_frame_t *frame = (diag_router_frame_t *)buf;
    uint16_t packet_encrypt_size = DIAG_IND_HEADER_SIZE + packet_size;
    uint8_t *packet_encrypt = dfx_malloc(0, packet_encrypt_size);
    if (packet_encrypt == NULL) {
        return ERRCODE_DIAG_NOT_ENOUGH_MEMORY;
    }
    diag_head_ind_stru_t *ind = (diag_head_ind_stru_t *)(packet_encrypt);
    if (memcpy_s(packet_encrypt + DIAG_IND_HEADER_SIZE, packet_size, packet, packet_size) != EOK) {
        dfx_free(0, packet_encrypt);
        return ERRCODE_FAIL;
    }
    diag_mk_frame_header_1(frame, iv_tag_ext_size + DIAG_IND_HEADER_SIZE + packet_size);
    diag_mk_ind_header(ind, cmd_id);
    if (need_to_encrypt) {
        frame->ctrl |= 1 << DIAG_ROUTER_CTAL_SECURE_FLAG_BIT;
        uint8_t *iv = &buf[DIAG_ROUTER_HEADER_LEN];
        uint8_t *tag = &buf[DIAG_ROUTER_HEADER_LEN + AES_IV_LEN];
        if (diag_aes_gcm_encrypt_inplace(packet_encrypt, packet_encrypt_size, iv, tag) != ERRCODE_SUCC) {
            dfx_free(0, packet_encrypt);
            return ERRCODE_FAIL;
        }
    }
    diag_pkt_handle_init(&pkt, PKT_DATA_CNT_2);
    diag_pkt_handle_set_data(&pkt, DIAG_PKT_DATA_ID_0, (uint8_t *)frame,
        DIAG_ROUTER_HEADER_LEN + iv_tag_ext_size, DIAG_PKT_STACK_DATA);
    diag_pkt_handle_set_data(&pkt, DIAG_PKT_DATA_ID_1, packet_encrypt, packet_encrypt_size, DIAG_PKT_STACK_DATA);
    if (sync) {
        process_param.cur_proc = DIAG_PKT_PROC_SYNC;
    } else {
        process_param.cur_proc = DIAG_PKT_PROC_ASYNC;
    }
    diag_pkt_router(&pkt, &process_param);
    dfx_free(0, packet_encrypt);
    return ERRCODE_SUCC;
}

static errcode_t uapi_diag_report_packet_direct_secure(uint16_t cmd_id, diag_option_t *option, const uint8_t *packet,
    uint16_t packet_size)
{
    diag_pkt_handle_t pkt;
    diag_pkt_process_param_t process_param = {.dst_addr = DIAG_FRAME_FID_PC, .src_addr = DIAG_FRAME_FID_MCU};
    if (option != NULL) {
        process_param.dst_addr = option->peer_addr;
    }
    uint16_t iv_tag_ext_size = 0;
    bool need_to_encrypt = diag_need_secure(cmd_id);
    if (need_to_encrypt) {
        diag_secure_ctx_t *secure_ctx = diag_get_secure_ctx();
        uint8_t key_check[AES128_KEY_LEN];
        memset_s(key_check, AES128_KEY_LEN, 0, AES128_KEY_LEN);
        if (memcmp(secure_ctx->srp_info.key, key_check, AES128_KEY_LEN) != 0) {
            return ERRCODE_FAIL;
        }
        iv_tag_ext_size = AES_IV_LEN + AES_TAG_LEN;
    }
    uint8_t buf[DIAG_ROUTER_HEADER_LEN + iv_tag_ext_size];
    diag_router_frame_t *frame = (diag_router_frame_t *)buf;
    uint16_t packet_encrypt_size = DIAG_IND_HEADER_SIZE + packet_size;
    uint8_t *packet_encrypt = dfx_malloc(0, packet_encrypt_size);
    if (packet_encrypt == NULL) {
        return ERRCODE_DIAG_NOT_ENOUGH_MEMORY;
    }
    diag_head_ind_stru_t *ind = (diag_head_ind_stru_t *)(packet_encrypt);
    if (memcpy_s(packet_encrypt + DIAG_IND_HEADER_SIZE, packet_size, packet, packet_size) != EOK) {
        dfx_free(0, packet_encrypt);
        return ERRCODE_FAIL;
    }
    diag_mk_frame_header_1(frame, iv_tag_ext_size + DIAG_IND_HEADER_SIZE + packet_size);
    diag_mk_ind_header(ind, cmd_id);
    if (need_to_encrypt) {
        frame->ctrl |= 1 << DIAG_ROUTER_CTAL_SECURE_FLAG_BIT;
        uint8_t *iv = &buf[DIAG_ROUTER_HEADER_LEN];
        uint8_t *tag = &buf[DIAG_ROUTER_HEADER_LEN + AES_IV_LEN];
        if (diag_aes_gcm_encrypt_inplace(packet_encrypt, packet_encrypt_size, iv, tag) != ERRCODE_SUCC) {
            dfx_free(0, packet_encrypt);
            return ERRCODE_FAIL;
        }
    }
    diag_pkt_handle_init(&pkt, PKT_DATA_CNT_2);
    diag_pkt_handle_set_data(&pkt, DIAG_PKT_DATA_ID_0, (uint8_t *)frame,
        DIAG_ROUTER_HEADER_LEN + iv_tag_ext_size, DIAG_PKT_STACK_DATA);
    diag_pkt_handle_set_data(&pkt, DIAG_PKT_DATA_ID_1, packet_encrypt, packet_encrypt_size, DIAG_PKT_STACK_DATA);
    process_param.cur_proc = DIAG_PKT_PROC_SYNC;
    diag_pkt_router(&pkt, &process_param);
    dfx_free(0, packet_encrypt);
    return ERRCODE_SUCC;
}
#endif

void uapi_diag_register_msg_flow_control_hook(diag_msg_flow_control_hook func)
{
    g_flow_control_hook = func;
}

errcode_t uapi_diag_report_packet(uint16_t cmd_id, diag_option_t *option, const uint8_t *packet,
    uint16_t packet_size, bool sync)
{
    dfx_log_debug("uapi_diag_report_packet in. cmd_id = 0x%x packet_size = %d.\r\n", cmd_id, packet_size);
    if (zdiag_is_enable() == false) {
        dfx_log_info("diag_unconnect\r\n");
        return ERRCODE_FAIL;
    }
#ifndef CONFIG_DIAG_WITH_SECURE
    errcode_t ret = ERRCODE_SUCC;
    diag_pkt_handle_t pkt;
    diag_pkt_process_param_t process_param = {.dst_addr = DIAG_FRAME_FID_PC, .src_addr = DIAG_FRAME_FID_MCU};
    if (option != NULL) {
        process_param.dst_addr = option->peer_addr;
    }
    uint8_t buf[DIAG_ROUTER_HEADER_LEN + DIAG_IND_HEADER_SIZE];
    diag_router_frame_t *frame = (diag_router_frame_t *)buf;
    diag_head_ind_stru_t *ind = (diag_head_ind_stru_t *)(buf + DIAG_ROUTER_HEADER_LEN);

    diag_pkt_handle_init(&pkt, PKT_DATA_CNT_2); /* data_cnt 为2 */
    diag_pkt_handle_set_data(&pkt, DIAG_PKT_DATA_ID_0, (uint8_t *)frame,
        DIAG_ROUTER_HEADER_LEN + DIAG_IND_HEADER_SIZE, DIAG_PKT_STACK_DATA);
    diag_pkt_handle_set_data(&pkt, DIAG_PKT_DATA_ID_1, (uint8_t *)packet, packet_size, DIAG_PKT_STACK_DATA);

    diag_mk_ind_header(ind, cmd_id);

    diag_mk_frame_header_1(frame, DIAG_IND_HEADER_SIZE + packet_size);
    if (sync) {
        process_param.cur_proc = DIAG_PKT_PROC_SYNC;
    } else {
        process_param.cur_proc = DIAG_PKT_PROC_ASYNC;
    }
    diag_pkt_router(&pkt, &process_param);
    return ret;
#else
    return uapi_diag_report_packet_secure(cmd_id, option, packet, packet_size, sync);
#endif
}

errcode_t uapi_diag_report_packet_direct(uint16_t cmd_id, diag_option_t *option, const uint8_t *packet,
    uint16_t packet_size)
{
    unused(option);
#ifndef CONFIG_DIAG_WITH_SECURE
    errcode_t ret = ERRCODE_SUCC;
    diag_pkt_handle_t pkt;
    diag_pkt_process_param_t process_param = {.dst_addr = DIAG_FRAME_FID_PC, .src_addr = DIAG_FRAME_FID_MCU};
    if (option != NULL) {
        process_param.dst_addr = option->peer_addr;
    }
    uint8_t buf[DIAG_ROUTER_HEADER_LEN + DIAG_IND_HEADER_SIZE];
    diag_router_frame_t *frame = (diag_router_frame_t *)buf;
    diag_head_ind_stru_t *ind = (diag_head_ind_stru_t *)(buf + DIAG_ROUTER_HEADER_LEN);

    diag_pkt_handle_init(&pkt, PKT_DATA_CNT_2); /* data_cnt 为2 */
    diag_pkt_handle_set_data(&pkt, DIAG_PKT_DATA_ID_0, (uint8_t *)frame,
        DIAG_ROUTER_HEADER_LEN + DIAG_IND_HEADER_SIZE, DIAG_PKT_STACK_DATA);
    diag_pkt_handle_set_data(&pkt, DIAG_PKT_DATA_ID_1, (uint8_t *)packet, packet_size, DIAG_PKT_STACK_DATA);

    diag_mk_ind_header(ind, cmd_id);

    diag_mk_frame_header_1(frame, DIAG_IND_HEADER_SIZE + packet_size);
    process_param.cur_proc = DIAG_PKT_PROC_SYNC;
    diag_pkt_router(&pkt, &process_param);
    return ret;
#else
    return uapi_diag_report_packet_direct_secure(cmd_id, option, packet, packet_size);
#endif
}

STATIC errcode_t zdiag_report_packets(uint16_t cmd_id, diag_option_t *option, diag_report_packet *report_packet,
    uint8_t critical)
{
    errcode_t ret = ERRCODE_SUCC;
    uint8_t **packet = report_packet->packet;
    uint16_t *packet_size = report_packet->packet_size;
    uint8_t pkt_cnt = report_packet->pkt_cnt;
    diag_pkt_handle_t pkt;
    diag_pkt_process_param_t process_param = {.dst_addr = DIAG_FRAME_FID_PC, .src_addr = DIAG_FRAME_FID_MCU};
    if (option != NULL) {
        process_param.dst_addr = option->peer_addr;
    }
    uint8_t buf[DIAG_ROUTER_HEADER_LEN + DIAG_IND_HEADER_SIZE];
    diag_router_frame_t *frame = (diag_router_frame_t *)buf;
    diag_head_ind_stru_t *ind = (diag_head_ind_stru_t *)(buf + DIAG_ROUTER_HEADER_LEN);

    uint16_t usr_data_size = 0;

    dfx_log_debug("zdiag_report_packets in. cmd_id = 0x%x\r\n", cmd_id);
    if (zdiag_is_enable() == false) {
        dfx_log_info("diag_unconnect\r\n");
        return ERRCODE_FAIL;
    }

    if (pkt_cnt > DIAG_PKT_DATA_ID_MAX - 1) {
        dfx_log_err("[ERROR] zdiag_report_packets pkt_cnt invalid.\r\n");
        return ERRCODE_FAIL;
    }

    diag_pkt_handle_init(&pkt, pkt_cnt + 1);
    if (critical != 0) {
        diag_pkt_set_critical(&pkt);
    }
    diag_pkt_handle_set_data(&pkt, DIAG_PKT_DATA_ID_0, (uint8_t *)frame, DIAG_ROUTER_HEADER_LEN + DIAG_IND_HEADER_SIZE,
        DIAG_PKT_STACK_DATA);
    for (uint8_t i = 0; i < pkt_cnt; i++) {
        diag_pkt_handle_set_data(&pkt, i + 1, (uint8_t *)packet[i], packet_size[i], DIAG_PKT_STACK_DATA);
        usr_data_size += packet_size[i];
    }

    diag_mk_ind_header(ind, cmd_id);

    diag_mk_frame_header_1(frame, DIAG_IND_HEADER_SIZE + usr_data_size);

    process_param.cur_proc = DIAG_PKT_PROC_SYNC;
    diag_pkt_router(&pkt, &process_param);
    return ret;
}

errcode_t uapi_diag_report_packets_critical(uint16_t cmd_id, diag_option_t *option, uint8_t **packet,
    uint16_t *packet_size, uint8_t pkt_cnt)
{
    diag_report_packet pkt;
    pkt.packet = packet;
    pkt.packet_size = packet_size;
    pkt.pkt_cnt = pkt_cnt;
    return zdiag_report_packets(cmd_id, option, &pkt, 1);
}

errcode_t uapi_diag_report_packets_normal(uint16_t cmd_id, diag_option_t *option, uint8_t **packet,
    uint16_t *packet_size, uint8_t pkt_cnt)
{
    diag_report_packet pkt;
    pkt.packet = packet;
    pkt.packet_size = packet_size;
    pkt.pkt_cnt = pkt_cnt;
    return zdiag_report_packets(cmd_id, option, &pkt, 0);
}

errcode_t uapi_zdiag_report_sys_msg_instance(uint32_t module_id, uint32_t msg_id, const uint8_t *packet,
    uint16_t packet_size, uint8_t level)
{
    diag_pkt_handle_t pkt;
    diag_pkt_process_param_t process_param = {.dst_addr = DIAG_FRAME_FID_PC, .src_addr = DIAG_FRAME_FID_MCU};
    uint8_t buf[DIAG_ROUTER_HEADER_LEN + DIAG_IND_HEADER_SIZE + DIAG_LOG_HEADER_SIZE];
    diag_router_frame_t *frame = (diag_router_frame_t *)buf;
    diag_head_ind_stru_t *ind = (diag_head_ind_stru_t *)(buf + DIAG_ROUTER_HEADER_LEN);
    uint8_t *log_header = (uint8_t *)(buf + DIAG_ROUTER_HEADER_LEN + DIAG_IND_HEADER_SIZE);
    uint8_t *pkt_header = NULL;
    uint16_t pkt_header_len;

    if (zdiag_msg_permit(module_id, level) == false) {
        return ERRCODE_FAIL;
    }

    if (g_flow_control_hook != NULL && g_flow_control_hook((uint8_t)module_id, level) == false) {
        return ERRCODE_FAIL;
    }

    diag_pkt_handle_init(&pkt, 1); /* data_cnt 为1 */

    if (uapi_zdiag_offline_log_is_enable()) {
        pkt_header = log_header;
        pkt_header_len = (uint16_t)DIAG_LOG_HEADER_SIZE;
        diag_pkt_set_output_type(&pkt, 1);
    } else {
        pkt_header = (uint8_t *)frame;
        pkt_header_len = (uint16_t)(DIAG_ROUTER_HEADER_LEN + DIAG_IND_HEADER_SIZE + DIAG_LOG_HEADER_SIZE);
    }

    diag_pkt_handle_set_data(&pkt, DIAG_PKT_DATA_ID_0, pkt_header, pkt_header_len, DIAG_PKT_STACK_DATA);
    if (packet_size != 0) {
        diag_pkt_cnt_increase(&pkt, 1);
        diag_pkt_handle_set_data(&pkt, DIAG_PKT_DATA_ID_1, (uint8_t *)packet, packet_size, DIAG_PKT_STACK_DATA);
    }

    diag_mk_ind_header(ind, DIAG_CMD_MSG_RPT_SYS);

#if (CONFIG_DFX_SUPPORT_DIAG_MINIMAL_MSG == DFX_YES)
    diag_cmd_minimal_log_layer_t *log = (diag_cmd_minimal_log_layer_t *)log_header;
    diag_mk_min_log_pkt(log, msg_id);
#elif (CONFIG_DFX_SUPPORT_DIAG_NORMAL_MSG == DFX_YES)
    diag_cmd_normal_log_layer_t *log = (diag_cmd_normal_log_layer_t *)log_header;
    diag_mk_normal_log_pkt(log, (uint8_t)module_id, msg_id);
#elif (CONFIG_DFX_SUPPORT_DIAG_EXTEND_MSG == DFX_YES)
    diag_cmd_extend_log_layer_t *log = (diag_cmd_extend_log_layer_t *)log_header;
    diag_mk_extend_log_pkt(log, (uint8_t)module_id, msg_id);
#else
    diag_cmd_log_layer_stru_t *log = (diag_cmd_log_layer_stru_t *)log_header;
    diag_mk_log_pkt(log, module_id, msg_id);
#endif

    diag_mk_frame_header_1(frame, DIAG_IND_HEADER_SIZE + DIAG_LOG_HEADER_SIZE + packet_size);

    process_param.cur_proc = DIAG_PKT_PROC_ASYNC;
    return diag_pkt_router(&pkt, &process_param);
}

errcode_t uapi_zdiag_report_sys_msg_instance_sn(uint32_t module_id, uint32_t msg_id,
    diag_report_sys_msg_packet *report_sys_msg_packet, uint8_t level, uint32_t sn)
{
    uint8_t *packet = report_sys_msg_packet->packet;
    uint16_t packet_size = report_sys_msg_packet->packet_size;
    diag_pkt_handle_t pkt;
    diag_pkt_process_param_t process_param = {.dst_addr = DIAG_FRAME_FID_PC, .src_addr = DIAG_FRAME_FID_MCU};
    uint8_t buf[DIAG_ROUTER_HEADER_LEN + DIAG_IND_HEADER_SIZE + DIAG_LOG_HEADER_SIZE];
    diag_router_frame_t *frame = (diag_router_frame_t *)buf;
    diag_head_ind_stru_t *ind = (diag_head_ind_stru_t *)(buf + DIAG_ROUTER_HEADER_LEN);
    uint8_t *log_header = (uint8_t *)(buf + DIAG_ROUTER_HEADER_LEN + DIAG_IND_HEADER_SIZE);
    uint8_t *pkt_header = NULL;
    uint16_t pkt_header_len;

    if (zdiag_msg_permit(module_id, level) == false) {
        return ERRCODE_FAIL;
    }

    diag_pkt_handle_init(&pkt, 1); /* data_cnt 为1 */

    if (uapi_zdiag_offline_log_is_enable()) {
        pkt_header = log_header;
        pkt_header_len = (uint16_t)DIAG_LOG_HEADER_SIZE;
        diag_pkt_set_output_type(&pkt, 1);
    } else {
        pkt_header = (uint8_t *)frame;
        pkt_header_len = (uint16_t)(DIAG_ROUTER_HEADER_LEN + DIAG_IND_HEADER_SIZE + DIAG_LOG_HEADER_SIZE);
    }

    diag_pkt_handle_set_data(&pkt, DIAG_PKT_DATA_ID_0, pkt_header, pkt_header_len, DIAG_PKT_STACK_DATA);
    if (packet_size != 0) {
        diag_pkt_cnt_increase(&pkt, 1);
        diag_pkt_handle_set_data(&pkt, DIAG_PKT_DATA_ID_1, (uint8_t *)packet, packet_size, DIAG_PKT_STACK_DATA);
    }

    diag_mk_ind_header(ind, DIAG_CMD_MSG_RPT_SYS);

#if (CONFIG_DFX_SUPPORT_DIAG_MINIMAL_MSG == DFX_YES)
    diag_cmd_minimal_log_layer_t *log = (diag_cmd_minimal_log_layer_t *)log_header;
    diag_mk_min_log_pkt_sn(log, msg_id, (uint16_t)sn);
#elif (CONFIG_DFX_SUPPORT_DIAG_NORMAL_MSG == DFX_YES)
    diag_cmd_normal_log_layer_t *log = (diag_cmd_normal_log_layer_t *)log_header;
    diag_mk_normal_log_pkt_sn(log, (uint8_t)module_id, msg_id, (uint16_t)sn);
#elif (CONFIG_DFX_SUPPORT_DIAG_EXTEND_MSG == DFX_YES)
    diag_cmd_extend_log_layer_t *log = (diag_cmd_extend_log_layer_t *)log_header;
    diag_mk_extend_log_pkt_sn(log, (uint8_t)module_id, msg_id, (uint8_t)sn);
#else
    diag_cmd_log_layer_stru_t *log = (diag_cmd_log_layer_stru_t *)log_header;
    diag_mk_log_pkt_sn(log, module_id, msg_id, sn);
#endif

    diag_mk_frame_header_1(frame, DIAG_IND_HEADER_SIZE + DIAG_LOG_HEADER_SIZE + packet_size);

    process_param.cur_proc = DIAG_PKT_PROC_SYNC;
    return diag_pkt_router(&pkt, &process_param);
}
