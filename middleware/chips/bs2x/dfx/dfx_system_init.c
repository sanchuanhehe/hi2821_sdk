/*
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved.
 * Description: dfx system init
 * This file should be changed only infrequently and with great care.
 */

#include <stdint.h>
#include "dfx_adapt_layer.h"
#include "diag.h"
#include "soc_diag_cmd_id.h"
#include "diag_cmd_connect.h"
#include "diag_cmd_filter.h"
#include "diag_cmd_password.h"
#include "diag_cmd_beat_heart.h"
#include "diag_cmd_get_mem_info.h"
#include "diag_cmd_get_task_info.h"
#include "diag_cmd_mem_read_write.h"
#include "diag_mocked_shell.h"
#include "diag_bt_sample_data.h"
#include "osal_task.h"
#include "osal_msgqueue.h"
#include "osal_event.h"
#include "diag_ind_src.h"
#include "diag_filter.h"
#include "diag_msg.h"
#include "diag_dfx.h"
#ifdef SUPPORT_DIAG_V2_PROTOCOL
#include "diag_service.h"
#include "diag_cmd_dispatch.h"
#endif /* SUPPORT_DIAG_V2_PROTOCOL */
#include "diag_filter.h"
#include "diag_channel.h"
#include "transmit.h"
#include "diag_rom_api.h"
#include "zdiag_adapt_layer.h"
#include "zdiag_adapt_sdt.h"
#include "soc_log_uart_instance.h"
#include "sample_data_adapt.h"
#include "dfx_channel.h"
#include "nv.h"

#ifdef CONFIG_OTA_UPDATE_SUPPORT
#include "diag_update.h"
#endif

static const diag_cmd_reg_obj_t g_diag_default_cmd_tbl[] = {
    { DIAG_CMD_CONNECT_RANDOM, DIAG_CMD_CONNECT_M_CHECK, diag_cmd_password },
    { DIAG_CMD_HOST_CONNECT, DIAG_CMD_HOST_DISCONNECT, diag_cmd_hso_connect_disconnect },
    { DIAG_CMD_MSG_RPT_AIR, DIAG_CMD_MSG_RPT_USR, diag_cmd_filter_set },
    { DIAG_CMD_MSG_CFG_SET_AIR, DIAG_CMD_MSG_CFG_SET_LEVEL, diag_cmd_filter_set },
    { DIAG_CMD_HEART_BEAT, DIAG_CMD_HEART_BEAT, diag_cmd_beat_heart },
    { DIAG_CMD_GET_TASK_INFO, DIAG_CMD_GET_TASK_INFO, diag_cmd_get_task_info},
    { DIAG_CMD_GET_MEM_INFO, DIAG_CMD_GET_MEM_INFO, diag_cmd_get_mem_info},
    { DIAG_CMD_MEM_MEM32, DIAG_CMD_MEM_W4, diag_cmd_mem_operate },
    { DIAG_CMD_ID_SAMPLE, DIAG_CMD_ID_SAMPLE, diag_cmd_sample_data},
};

unsigned long g_dfx_osal_queue_id;
static errcode_t register_default_diag_cmd(void)
{
    return uapi_diag_register_cmd(g_diag_default_cmd_tbl,
        sizeof(g_diag_default_cmd_tbl) / sizeof(g_diag_default_cmd_tbl[0]));
}

int32_t msg_process_proc(uint32_t msg_id, uint8_t *data, uint16_t size)
{
    zdiag_dfx_rev_msg();
    switch (msg_id) {
        case DFX_MSG_ID_DIAG_PKT:
            zdiag_dfx_rev_pkt_msg();
            diag_msg_proc((uint16_t)msg_id, data, size);
            break;
        case DFX_MSG_ID_SDT_MSG:
            zdiag_adapt_sdt_msg_dispatch(msg_id, data, size);
            break;
        case DFX_MSG_ID_BEAT_HEART:
            zdiag_dfx_rev_beat_herat_msg();
            diag_beat_heart_process();
            break;
#if CONFIG_DFX_SUPPORT_TRANSMIT_FILE == DFX_YES
        case DFX_MSG_ID_TRANSMIT_FILE:
            transmit_msg_proc(msg_id, data, size);
            break;
#endif
        default:
            break;
    }
    return ERRCODE_SUCC;
}

static void cmd_shell_proc(uint8_t *data, uint32_t data_len)
{
    diag_debug_cmd_proc(data, data_len);
    dfx_log_debug("cmd shell: %s", data);
}

unsigned long dfx_get_osal_queue_id(void)
{
    return g_dfx_osal_queue_id;
}

errcode_t dfx_system_init(void)
{
    errcode_t ret;
    diag_rom_api_t rom_api;

#ifdef SUPPORT_DIAG_V2_PROTOCOL
    uapi_diag_service_init();
    uapi_diag_service_register(DIAG_SER_MAINTENANCE, uapi_zdiag_cmd_process);
#endif

    ret = register_default_diag_cmd();
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    zdiag_filter_init();

#ifndef FORBID_AUTO_LOG_REPORT
    diag_auto_log_report_enable();
#else
#if CONFIG_DFX_SUPPORT_DIAG_BEAT_HEART == DFX_YES
    ret = diag_beat_heart_init();
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
#endif
#endif
    ret = diag_register_channel();
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

#if (CONFIG_DFX_SUPPORT_DIAG_VRTTUAL_SHELL == DFX_YES)
    zdiag_mocked_shell_init();
    zdiag_mocked_shell_register_cmd_data_proc(cmd_shell_proc);
#endif /* CONFIG_DFX_SUPPORT_DIAG_VRTTUAL_SHELL */

    rom_api.report_sys_msg = uapi_zdiag_report_sys_msg_instance;
    diag_rom_api_register(&rom_api);

#if CONFIG_DFX_SUPPORT_TRANSMIT_FILE == DFX_YES
    uapi_transmit_init();
#endif

#ifdef CONFIG_OTA_UPDATE_SUPPORT
    ret = zdiag_update_init();
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
#endif

    return ERRCODE_SUCC;
}

void dfx_set_log_enable(void)
{
#ifdef CONFIG_NV_FEATURE_SUPPORT
    uint8_t flag = 0;
    uint16_t length;
    errcode_t ret = uapi_nv_read(NV_ID_OFFLINELOG_ENBALE_FLAG, sizeof(flag), &length, &flag);
    if (ret != ERRCODE_SUCC) {
        return;
    }
    if (flag != 0) {
        uapi_zdiag_set_offline_log_enable(true);
    } else {
        uapi_zdiag_set_offline_log_enable(false);
    }
#endif
}