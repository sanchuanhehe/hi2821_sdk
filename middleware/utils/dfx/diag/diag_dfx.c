/*
 * Copyright (c) @CompanyNameMagicTag 2021-2023. All rights reserved.
 * Description: zdiag dfx
 * This file should be changed only infrequently and with great care.
 */

#include "diag_dfx.h"
#include "panic.h"
#include "assert.h"
#include "hal_reboot.h"
#include "soc_module.h"
#include "diag.h"
#include "soc_diag_cmd_id.h"
#include "errcode.h"
#include "diag_log.h"
#include "soc_diag_msg_id.h"
#include "dfx_adapt_layer.h"
#include "soc_diag_wdk.h"
#include "log_module_id.h"
#ifdef CONFIG_NV_FEATURE_SUPPORT
#include "nv_debug.h"
#endif

#ifndef THIS_FILE_ID
#define THIS_FILE_ID DIAG_FILE_ID_TEST_DIAG_D
#endif

#ifndef THIS_MOD_ID
#define THIS_MOD_ID LOG_PFMODULE
#endif

typedef enum {
    DIAG_DFX_CMD_CASE_GET_STAT  = 0,
    DIAG_DFX_CMD_CASE_REPORT_MSG = 1,
    DIAG_DFX_CMD_CASE_REPORT_FIX_MSG = 2,
    DIAG_DFX_CMD_CASE_LAST_DUMP = 3,
    DIAG_DFX_CMD_CASE_SYS_MSG = 4,
    DIAG_DFX_CMD_CASE_FAULT_MOCKED = 5,
    DIAG_DFX_CMD_CASE_SET_LOG_LEVEL = 6,
    DIAG_DFX_CMD_CASE_REGISTER_STAT = 7,
} diag_dfx_cmd_case_id_t;

zdiag_dfx_stat_t g_zdiag_dfx_stat;

void dfx_last_dump_data(uint32_t dump_id, uintptr_t addr, uint32_t size);

zdiag_dfx_stat_t *uapi_zdiag_get_dfx_stat(void)
{
    return &g_zdiag_dfx_stat;
}

STATIC errcode_t diag_dfx_report_sys_msg(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size,
                                         diag_option_t *option)
{
    uint32_t data[] = {1, 2, 3, 4}; /* test val 1 2 3 4 */
    unused(cmd_id);
    unused(cmd_param);
    unused(cmd_param_size);
    unused(option);

    uapi_diag_report_sys_msg(0, 0x1, (uint8_t*)data, sizeof(data), 1);  /* 0 0x1 1 test val */
    uapi_diag_report_sys_msg(1, 0xfeb40645, (uint8_t*)data, sizeof(data), 2); /* 1 0xfeb40645 2 test val */
    return ERRCODE_SUCC;
}


STATIC errcode_t diag_dfx_last_dump(uint16_t cmd_id, void * cmd_param, uint16_t cmd_param_size, diag_option_t *option)
{
    unused(cmd_id);
    unused(cmd_param);
    unused(cmd_param_size);
    unused(option);

    diag_dfx_cmd_req_st_t *req = cmd_param;

    dfx_last_dump_data(req->data[0], req->data[1], req->data[2]); /* 0 1 2 为data 下标 */
    return ERRCODE_SUCC;
}

STATIC errcode_t diag_dfx_cmd_get_stat(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size,
                                       diag_option_t *option)
{
    unused(cmd_id);
    unused(cmd_param);
    unused(cmd_param_size);
    return uapi_diag_report_packet(DIAG_CMD_ID_DIAG_DFX_START, option, (uint8_t *)&g_zdiag_dfx_stat,
        sizeof(zdiag_dfx_stat_t), true);
}

STATIC errcode_t diag_dfx_cmd_report_msg(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size,
                                         diag_option_t *option)
{
    uapi_diag_error_log(0, "test_error_log. p1 = %d", 1);        /* 3 test val */
    uapi_diag_error_log1(102, "test_error_log1. p1 = %x", 3);    /* 102 3 test val */
    uapi_diag_warning_log(0x101, "test_warning_log p1 = %d", 5); /* 0x101 5 test val */
    uapi_diag_warning_log1(0, "test_warning_log1 p1 = %d", 2);   /* 2 test val */
    uapi_diag_info_log(0, "test_info_log p1 = 0x%x", 1);
    uapi_diag_info_log1(0x100, "test_info_log1 p1 = 0x%x", 4);   /* 0x100 4 test val */
    uapi_diag_debug_log(0, "test_debug_log p1 = %d", 6);         /* 6 test val */
    uapi_diag_debug_log1(0, "test_debug_log1 p1 = %d", 1);

    unused(cmd_id);
    unused(cmd_param_size);
    return uapi_diag_report_packet(cmd_id, option, (uint8_t *)cmd_param, sizeof(diag_dfx_cmd_req_st_t), false);
}

STATIC errcode_t diag_dfx_cmd_report_fix_msg(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size,
                                             diag_option_t *option)
{
    uint32_t data[] = {1, 2, 3, 4}; /* test val 1 2 3 4 */
    uapi_diag_report_sys_msg(THIS_MOD_ID, SOC_DIAG_MSG_ID_DIAG_TEST_U8_ARRAY, (uint8_t *)data, sizeof(data), 1);
    uapi_diag_report_sys_msg(THIS_MOD_ID, SOC_DIAG_MSG_ID_DIAG_TEST_U32_ARRAY, (uint8_t *)data, sizeof(data), 1);

    unused(cmd_id);
    unused(cmd_param);
    unused(cmd_param_size);
    unused(option);
    return ERRCODE_SUCC;
}

#ifndef NDEBUG
STATIC errcode_t diag_dfx_cmd_fault_mocked(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size,
                                           diag_option_t *option)
{
    diag_dfx_cmd_req_st_t *req = cmd_param;

    if (req->data[0] == 0) {
        if (req->data[1] == 0) {
            uint32_t *null_pointer = NULL;
            *null_pointer = req->case_id;
        } else if (req->data[1] == 1) {    /* 1 mean fault type is circulation */
            while (true) {}
        } else if (req->data[1] == 2) {    /* 2 mean fault type is assert */
            dfx_assert(req->case_id != 5); /* fault mocked case id is 5 */
        } else if (req->data[1] == 3) {    /* 3 mean fault type is panic */
            panic(PANIC_LOG, __LINE__);
        }
    } else if (req->data[0] == 1) {
        hal_reboot_chip();
    }
    return ERRCODE_SUCC;
}
#endif

STATIC errcode_t diag_dfx_cmd_log_level(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size,
                                        diag_option_t *option)
{
    diag_dfx_cmd_req_st_t *req = cmd_param;
    diag_dfx_cmd_ind_st_t ind = {0};

    if (req->data[0] == 0) {
        /* 查询 */
        ind.case_id = req->case_id;
        if (req->data[1] == 0) {
            ind.data[0] = diag_get_debug_level();
        } else if (req->data[1] == 1) {
#ifdef CONFIG_NV_FEATURE_SUPPORT
            ind.data[0] = nv_get_debug_level();
#endif
        }
        uapi_diag_report_packet(cmd_id, option, (uint8_t *)&ind, sizeof(diag_dfx_cmd_ind_st_t), true);
    } else if (req->data[0] == 1) {
        /* 设置 */
        if (req->data[1] == 0) {
            /* DFX log level */
            diag_set_debug_level(req->data[2]); /* The level to be set is stored in data[2] */
            ind.data[0] = diag_get_debug_level();
        } else {
#ifdef CONFIG_NV_FEATURE_SUPPORT
            /* NV log level */
            nv_set_debug_level(req->data[2]); /* The level to be set is stored in data[2] */
            ind.data[0] = nv_get_debug_level();
#endif
        }
        ind.case_id = req->case_id;
        uapi_diag_report_packet(cmd_id, option, (uint8_t *)&ind, sizeof(diag_dfx_cmd_ind_st_t), true);
    }
    return ERRCODE_SUCC;
}

STATIC errcode_t diag_dfx_cmd_register_stat(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size,
                                            diag_option_t *option)
{
    diag_sys_stat_obj_t *obj = dfx_malloc(0, sizeof(diag_sys_stat_obj_t));
    diag_dfx_cmd_req_st_t *req = cmd_param;
    diag_dfx_cmd_ind_st_t ind = {0};
    errcode_t ret;
    if (obj != NULL) {
        obj->id = DIAG_CMD_ID_DIAG_DFX_START;
        obj->array_cnt = 1;
        obj->stat_packet_size = (uint32_t)sizeof(zdiag_dfx_stat_t);
        obj->stat_packet = (void *)&g_zdiag_dfx_stat;
        ret = uapi_diag_register_stat_obj(obj, 1);
    } else {
        ret = ERRCODE_MALLOC;
    }

    ind.case_id = req->case_id;
    ind.data[0] = ret;
    uapi_diag_report_packet(cmd_id, option, (uint8_t *)&ind, sizeof(diag_dfx_cmd_ind_st_t), true);
    return ERRCODE_SUCC;
}

errcode_t diag_dfx_cmd(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option)
{
    diag_dfx_cmd_req_st_t *req = cmd_param;
    unused(cmd_param_size);
    switch (req->case_id) {
        case DIAG_DFX_CMD_CASE_GET_STAT:
            return diag_dfx_cmd_get_stat(cmd_id, cmd_param, cmd_param_size, option);
        case DIAG_DFX_CMD_CASE_REPORT_MSG:
            return diag_dfx_cmd_report_msg(cmd_id, cmd_param, cmd_param_size, option);
        case DIAG_DFX_CMD_CASE_REPORT_FIX_MSG:
            return diag_dfx_cmd_report_fix_msg(cmd_id, cmd_param, cmd_param_size, option);
        case DIAG_DFX_CMD_CASE_LAST_DUMP:
            return diag_dfx_last_dump(cmd_id, cmd_param, cmd_param_size, option);
        case DIAG_DFX_CMD_CASE_SYS_MSG:
            return diag_dfx_report_sys_msg(cmd_id, cmd_param, cmd_param_size, option);
#ifndef NDEBUG
        case DIAG_DFX_CMD_CASE_FAULT_MOCKED:
            return diag_dfx_cmd_fault_mocked(cmd_id, cmd_param, cmd_param_size, option);
#endif
        case DIAG_DFX_CMD_CASE_SET_LOG_LEVEL:
            return diag_dfx_cmd_log_level(cmd_id, cmd_param, cmd_param_size, option);
        case DIAG_DFX_CMD_CASE_REGISTER_STAT:
            return diag_dfx_cmd_register_stat(cmd_id, cmd_param, cmd_param_size, option);
        default:
            return uapi_diag_report_packet(cmd_id, option, (uint8_t *)req, sizeof(diag_dfx_cmd_req_st_t), false);
    }
    return ERRCODE_SUCC;
}
