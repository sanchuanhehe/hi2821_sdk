/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: diag oam log
 * This file should be changed only infrequently and with great care.
 */
#include "securec.h"
#include "log_oam_msg.h"
#include "log_oam_logger.h"
#include "log_oam_status.h"
#include "log_oam_ota.h"
#include "log_oam_pcm.h"
#include "log_oml_exception.h"
#include "diag.h"
#include "soc_diag_cmd_id.h"
#include "soc_diag_util.h"
#include "diag_common.h"
#include "diag_filter.h"
#include "zdiag_adapt_layer.h"
#include "sample_data_adapt.h"
#include "log_types.h"
#include "diag_ind_src.h"
#ifdef SUPPORT_CONNECTIVITY
#include "connectivity_log.h"
#endif
#include "zdiag_adapt_sdt.h"

enum {
    LOG_STATUS_MODULE = MODULEID_BUTT,
    LOG_OTA_MODULE,
} log_adapt_module;

typedef enum {
    SDT_MSG_STEP_HEAD, /* sdt msg head */
    SDT_MSG_STEP_BODY, /* sdt msg body */
    STD_MSG_STEP_TAIL, /* sdt msg tail */
} sdt_msg_pack_step_t;

typedef struct {
    uint8_t *msg;
    uint32_t len;
} zdiag_adapt_sdt_msg_t;

typedef struct diag_msg_para_t {
    uint32_t module_id;
    uint32_t msg_id;
    uint32_t no;
    uint8_t *buf;
    uint16_t buf_size;
    uint8_t level;
} diag_msg_para_t;

oam_cmd_handle_callback g_diag_debug_proc = NULL;
uint8_t g_recv_msg_step = SDT_MSG_STEP_HEAD;
uint8_t *g_recv_buf = NULL;          /* 组包buf */
uint32_t g_frame_len = 0;          /* om包长度 */
uint32_t g_recv_len = 0;

static uint32_t log_level_trans(uint32_t sdt_level)
{
    /* sdt 日志等级4种， hso日志等级8种，需要转换一次 */
    uint32_t level[LOG_LEVEL_MAX] = {
        DIAG_LOG_LEVEL_ALERT,
        DIAG_LOG_LEVEL_ERROR,
        DIAG_LOG_LEVEL_WARN,
        DIAG_LOG_LEVEL_INFO,
        DIAG_LOG_LEVEL_DBG,
    };

    if (sdt_level < LOG_LEVEL_MAX) {
        return level[sdt_level];
    } else {
        return LOG_LEVEL_NONE;
    }
}

/*
For std log:
1bit        | 4bit   | 10bit   | 14bit   | 3bit  |
logFlag = 1 | mod_id | file_id | line_no | level |
for other:
1bit        | 7bit     | 8bit  | 16bit  |
logFlag = 0 | resv = 0 | type  | msg_id |
*/

static void zdiag_adapt_sdt_om_log_parse(uint8_t *sdt_buf, uint16_t sdt_buf_size, diag_msg_para_t *para)
{
    om_log_header_t* om_log = (om_log_header_t*)sdt_buf;

    uint32_t file_id = (om_log->file_idx_high << 2) + om_log->bit2_file_idx_low;
    uint32_t line_no = (om_log->bit6_line_no_high << 8) + om_log->line_no_low;
    uint32_t sdt_level = om_log->mod_print_lev_info >> 6;   /* level 2 bit */
    uint32_t mod_id = om_log->header.prime_id >> 4; /* 高4 bit 为mod_id */

    para->module_id = mod_id;

    para->msg_id = (1 << 31);                    /* bit31 为logFlag */
    para->msg_id += mod_id << 27;                /* bit27 ~ bit30 为mod_id */
    para->msg_id += (file_id & 0x3FF) << 17;     /* bit17 ~ bit26 为file_id */
    para->msg_id += (line_no & 0x3FFF) << 3;     /* bit3 ~ bit16 为line_no */
    para->msg_id += log_level_trans(sdt_level);

    para->no = om_log->header.sn;
    para->buf = sdt_buf + sizeof(om_log_header_t);
    para->buf_size = sdt_buf_size - sizeof(om_log_header_t) - sizeof(uint8_t);
    para->level = log_level_trans(sdt_level);
}

static void zdiag_adapt_sdt_om_status_parse(uint8_t *sdt_buf, uint16_t sdt_buf_size, diag_msg_para_t *para)
{
    om_status_data_stru_t *om_status = (om_status_data_stru_t*)sdt_buf;

    unused(sdt_buf_size);
    para->module_id = LOG_STATUS_MODULE;
    para->msg_id = (OM_MSG_TYPE_STATUS << 16) | om_status->msg_id; /* 高16 bit 为MSG_TYPE */
    para->no = om_status->header.sn;
    para->buf = om_status->data;
    para->buf_size = om_status->data_len;
    para->level = DIAG_LOG_LEVEL_INFO;
}

static void zdiag_adapt_sdt_om_ota_parse(uint8_t *sdt_buf, uint16_t sdt_buf_size, diag_msg_para_t *para)
{
    om_ota_header_t *om_ota = (om_ota_header_t*)sdt_buf;
    para->module_id = LOG_OTA_MODULE;
    para->msg_id = (OM_MSG_TYPE_OTA << 16) | om_ota->msg_id; /* 高16 bit 为MSG_TYPE */
    para->no = om_ota->header.sn;
    para->buf = sdt_buf + sizeof(om_ota_header_t);
    para->buf_size = sdt_buf_size - sizeof(om_ota_header_t) - sizeof(uint8_t);
    para->level = DIAG_LOG_LEVEL_INFO;
}

static void zdiag_adapt_sdt_om_last_parse(uint8_t *sdt_buf, uint16_t sdt_buf_size, diag_msg_para_t *para)
{
    om_exception_info_stru_t *om_last = (om_exception_info_stru_t*)sdt_buf;
    para->module_id = LOG_BTMODULE;
    para->msg_id = (OM_MSG_TYPE_LAST << 16) | om_last->msg_header.prime_id; /* 高16 bit 为MSG_TYPE */
    para->no = om_last->msg_header.sn;
    para->buf = sdt_buf + sizeof(om_msg_header_stru_t);
    para->buf_size = sdt_buf_size - sizeof(om_msg_header_stru_t) - sizeof(uint8_t);
    para->level = DIAG_LOG_LEVEL_INFO;
}

static void zdiag_adapt_sdt_msg_report(uint8_t *sdt_buf, uint16_t sdt_buf_size)
{
    om_msg_header_stru_t *om_msg_header;
    diag_msg_para_t para;

    om_msg_header = (om_msg_header_stru_t*)sdt_buf;
    if (om_msg_header->func_type == OM_MSG_TYPE_OTA) {
        zdiag_adapt_sdt_om_ota_parse(sdt_buf, sdt_buf_size, &para);
        diag_report_sys_msg_packet packet;
        packet.packet = para.buf;
        packet.packet_size = para.buf_size;
        uapi_zdiag_report_sys_msg_instance_sn(para.module_id, para.msg_id, &packet, para.level, para.no);
    } else if (om_msg_header->func_type == OM_BT_SAMPLE_DATA) {
        zdiag_adapt_sample_data_report(sdt_buf, sdt_buf_size);
    } else {
        return;
    }
}

static errcode_t zdiag_adapt_msg_pack_check(uint8_t *data, uint32_t data_len)
{
    om_msg_header_stru_t *om_msg_header = (om_msg_header_stru_t*)data;
    if (data_len < sizeof(om_msg_header_stru_t)) {
        return ERRCODE_FAIL;
    }

    if (om_msg_header->frame_start != OM_FRAME_DELIMITER) {
        return ERRCODE_FAIL;
    }

    if (om_msg_header->func_type == OM_MSG_TYPE_OTA) {
        if (om_msg_header->frame_len > OTA_DATA_MAX_SIZE + sizeof(om_msg_header_stru_t) + 1) {
            return ERRCODE_FAIL;
        }
    } else if (om_msg_header->func_type == OM_BT_SAMPLE_DATA) {
        if (om_msg_header->frame_len > BT_SAMPLE_DATA_MAX_SIZE + sizeof(om_pcm_header_t) + 1) {
            return ERRCODE_FAIL;
        }
    } else {
        return ERRCODE_FAIL;
    }

    return ERRCODE_SUCC;
}
static void zdiag_adapt_msg_pack_para_reinit(void)
{
    g_recv_msg_step = SDT_MSG_STEP_HEAD;
    if (g_recv_buf != NULL) {
        dfx_free(0, g_recv_buf);
        g_recv_buf = NULL;
    }
}

errcode_t zdiag_adapt_sdt_msg_dispatch(uint32_t msg_id, uint8_t *data, uint32_t data_len)
{
    zdiag_adapt_sdt_msg_t *msg = (zdiag_adapt_sdt_msg_t*)data;
    uint32_t check_valid;
    unused(msg_id);
    unused(data_len);

    if (g_recv_msg_step == SDT_MSG_STEP_HEAD) {
        check_valid = zdiag_adapt_msg_pack_check(msg->msg, msg->len);
        if (check_valid == ERRCODE_SUCC) {
            g_frame_len = ((om_msg_header_stru_t *)msg->msg)->frame_len;
            g_recv_buf = dfx_malloc(0, g_frame_len);
            if (g_recv_buf != NULL) {
                (void)memcpy_s(g_recv_buf, g_frame_len, msg->msg, msg->len);
                g_recv_len = msg->len;
                g_recv_msg_step = SDT_MSG_STEP_BODY;
            }
        }
    } else if (g_recv_msg_step == SDT_MSG_STEP_BODY) {
        if (g_frame_len - g_recv_len > 0) {
            (void)memcpy_s(g_recv_buf + g_recv_len, g_frame_len - g_recv_len, msg->msg, msg->len);
        }
        g_recv_len += msg->len;
        g_recv_msg_step = STD_MSG_STEP_TAIL;
    } else {
        if (g_frame_len - g_recv_len > 0) {
            (void)memcpy_s(g_recv_buf + g_recv_len, g_frame_len - g_recv_len, msg->msg, msg->len);
        }
        zdiag_adapt_sdt_msg_report(g_recv_buf, g_frame_len);
        zdiag_adapt_msg_pack_para_reinit();
    }
    dfx_free(0, msg->msg);
    return ERRCODE_SUCC;
}

static bool zdiag_adapt_sdt_short_msg_report(uint8_t *data, uint32_t data_len)
{
    om_msg_header_stru_t *om_msg_header = NULL;
    diag_msg_para_t para;
    bool pack = true;
    bool report = false;

    if (data_len >= sizeof(om_msg_header_stru_t)) {
        om_msg_header = (om_msg_header_stru_t*)data;
        if ((om_msg_header->frame_start == OM_FRAME_DELIMITER) &&
            (om_msg_header->frame_len == data_len) &&
            (data[data_len - 1] == OM_FRAME_DELIMITER)) {
            if (om_msg_header->func_type ==  OM_MSG_TYPE_LOG) {
                zdiag_adapt_sdt_om_log_parse(data, data_len, &para);
                report = true;
            } else if (om_msg_header->func_type == OM_MSG_TYPE_STATUS) {
                zdiag_adapt_sdt_om_status_parse(data, data_len, &para);
                report = true;
            } else if (om_msg_header->func_type == OM_MSG_TYPE_LAST) {
                zdiag_adapt_sdt_om_last_parse(data, data_len, &para);
                report = true;
            }
            pack = false;
            /* OTA, BT SAMPLE 三包数据一起写buffer，不会夹杂其他数据，若出现丢弃已pack的部分包 */
            zdiag_adapt_msg_pack_para_reinit();
        }
    }

    if (report) {
        diag_report_sys_msg_packet packet;
        packet.packet = para.buf;
        packet.packet_size = para.buf_size;
        uapi_zdiag_report_sys_msg_instance_sn(para.module_id, para.msg_id, &packet, para.level, para.no);
    }

    return pack;
}

void zdiag_adapt_sdt_msg_proc(uint8_t *buf1, uint32_t len1, uint8_t *buf2, uint32_t len2)
{
    zdiag_adapt_sdt_msg_t msg;
    uint32_t len = len1 + len2;
    uint8_t *buf = NULL;
    uint32_t pos = 0;
    uint32_t ret;
    bool pack;

    if (zdiag_is_enable() == false) {
        return;
    }

    if (len > 0) {
        buf = dfx_malloc(0, len);
    }

    if (buf == NULL) {
        return;
    }

    if (len1 > 0) {
        if (memcpy_s(buf + pos, len1, buf1, len1) != 0) {
            dfx_free(0, buf);
            return;
        }
        pos += len1;
    }

    if (len2 > 0) {
        if (memcpy_s(buf + pos, len2, buf2, len2) != 0) {
            dfx_free(0, buf);
            return;
        }
    }

    /* 如果是非整包数据，入队列，做pack处理 */
    pack = zdiag_adapt_sdt_short_msg_report(buf, len);
    if (pack) {
        msg.msg = buf;
        msg.len = len;
        ret = dfx_msg_write(DFX_MSG_ID_SDT_MSG, (uint8_t *)&msg, sizeof(zdiag_adapt_sdt_msg_t), false);
        if (ret != ERRCODE_SUCC) {
            dfx_log_err("sdt msg write to queue error\r\n");
            dfx_free(0, buf);
        }
    } else {
        dfx_free(0, buf);
    }
}

void diag_register_debug_cmd_callback(oam_cmd_handle_callback func)
{
    g_diag_debug_proc =  func;
}

void diag_debug_cmd_proc(uint8_t *data, uint32_t len)
{
    if (g_diag_debug_proc != NULL) {
        g_diag_debug_proc(data, len);
    }
}

#ifndef FORBID_AUTO_LOG_REPORT
void diag_auto_log_report_enable(void)
{
    uint32_t level[] = {
        DIAG_LOG_LEVEL_ERROR,
        DIAG_LOG_LEVEL_WARN,
        DIAG_LOG_LEVEL_INFO,
        DIAG_LOG_LEVEL_DBG,
    };
    uint32_t mod_id[] = {
        LOG_BTMODULE,
        LOG_PFMODULE,
        LOG_NFCMODULE,
        LOG_BTHMODULE,
        LOG_GLPMODULE,
        LOG_STATUS_MODULE,
        LOG_OTA_MODULE,
    };

    /* 全局使能 */
    zdiag_set_enable(1, diag_adapt_get_default_dst());

    /* module 使能 */
    for (uint32_t i = 0; i < sizeof(mod_id) / sizeof(uint32_t); i++) {
        zdiag_set_id_enable(mod_id[i], 1);
    }

    /* level 使能 */
    for (uint32_t i = 0; i < sizeof(level) / sizeof(uint32_t); i++) {
        zdiag_set_level_enable(level[i], 1);
    }
}
#endif

#if (CORE_NUMS > 1)
void zdiag_level_proc(uint8_t level)
{
    log_level_e core_level = LOG_LEVEL_NONE;
    log_level_e temp;
    cores_t cores[] = { CORES_APPS_CORE, CORES_BT_CORE };

    if (level & (1 << DIAG_LOG_LEVEL_DBG)) {
        core_level = LOG_LEVEL_DBG;
    } else if (level & (1 << DIAG_LOG_LEVEL_INFO)) {
        core_level = LOG_LEVEL_INFO;
    } else if (level & (1 << DIAG_LOG_LEVEL_WARN)) {
        core_level = LOG_LEVEL_WARNING;
    } else if (level & (1 << DIAG_LOG_LEVEL_ERROR)) {
        core_level = LOG_LEVEL_WARNING;
    }

    for (uint32_t i = 0; i < sizeof(cores) / sizeof(cores_t); i++) {
        get_log_level(cores[i], &temp);
        if (temp == core_level) {
            continue;
        }
        if (cores[i] != CORES_APPS_CORE) {
            if (ipc_check_status(cores[i]) != IPC_STATUS_OK) {
                continue;
            }
        }
        set_log_level(cores[i], core_level);
        dfx_log_info("core log level %d, %d, %d\r\n", cores[i], temp, core_level);
    }
}
#endif