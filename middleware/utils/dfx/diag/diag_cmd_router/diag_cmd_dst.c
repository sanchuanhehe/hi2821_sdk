/*
 * Copyright (c) @CompanyNameMagicTag 2021-2023. All rights reserved.
 * Description: diag cmd process
 * This file should be changed only infrequently and with great care.
 */

#include "diag_cmd_dst.h"
#include "diag.h"
#include "zdiag_adapt_layer.h"
#include "dfx_adapt_layer.h"
#include "errcode.h"

typedef struct {
    const diag_cmd_reg_obj_t *user_cmd_list[CONFIG_DIAG_CMD_TBL_NUM];
    uint16_t aus_usert_cmd_num[CONFIG_DIAG_CMD_TBL_NUM]; /* cmd obj num */
} diag_cmd_ctrl_t;

STATIC diag_cmd_ctrl_t g_diag_cmd_ctrl;

static inline diag_cmd_ctrl_t *diag_get_cmd_ctrl(void)
{
    return &g_diag_cmd_ctrl;
}

STATIC errcode_t diag_cmd_tbl_check(const diag_cmd_reg_obj_t *cmd_tbl, uint16_t cmd_num)
{
    unused(cmd_tbl);
    unused(cmd_num);

    return ERRCODE_SUCC;
}

STATIC diag_cmd_f diag_find_usr_cmd_proc_func(uint32_t cmd_id)
{
    diag_cmd_ctrl_t *cmd_ctrl = diag_get_cmd_ctrl();
    uint32_t i;
    uint16_t k;
    for (i = 0; i < CONFIG_DIAG_CMD_TBL_NUM; i++) {
        for (k = 0; k < cmd_ctrl->aus_usert_cmd_num[i]; k++) {
            const diag_cmd_reg_obj_t *cmd_tbl = cmd_ctrl->user_cmd_list[i];
            const diag_cmd_reg_obj_t *cmd_list = &cmd_tbl[k];

            if ((cmd_id >= cmd_list->min_id) && (cmd_id <= cmd_list->max_id)) {
                diag_cmd_f cmd = (diag_cmd_f)cmd_list->fn_input_cmd;
                return cmd;
            }
        }
    }
    return NULL;
}

errcode_t uapi_diag_register_cmd(const diag_cmd_reg_obj_t *cmd_tbl, uint16_t cmd_num)
{
    errcode_t ret;
    uint32_t lock_stat;
    int i;

    diag_cmd_ctrl_t *cmd_ctrl = diag_get_cmd_ctrl();
    lock_stat = dfx_int_lock();
    ret = diag_cmd_tbl_check(cmd_tbl, cmd_num);
    if (ret != ERRCODE_SUCC) {
        dfx_fault_event_data(FAULT_DFX_DIAG_REGISTER_CMD_FAIL, NULL, 0);
        goto end;
    }

    for (i = 0; i < CONFIG_DIAG_CMD_TBL_NUM; i++) {
        if ((cmd_ctrl->user_cmd_list[i] == NULL) || (cmd_ctrl->aus_usert_cmd_num[i] == 0)) {
            ret = ERRCODE_SUCC;
            cmd_ctrl->user_cmd_list[i] = cmd_tbl;
            cmd_ctrl->aus_usert_cmd_num[i] = cmd_num;
            goto end;
        }
    }
    ret = ERRCODE_FAIL;
end:
    dfx_fault_event_data(FAULT_DFX_DIAG_REGISTER_CMD_FAIL, NULL, 0);
    dfx_int_restore(lock_stat);
    return ret;
}

errcode_t uapi_diag_unregister_cmd(const diag_cmd_reg_obj_t *cmd_tbl, uint16_t cmd_num)
{
    errcode_t ret;
    uint32_t lock_stat;
    int i;

    diag_cmd_ctrl_t *cmd_ctrl = diag_get_cmd_ctrl();
    lock_stat = dfx_int_lock();
    ret = diag_cmd_tbl_check(cmd_tbl, cmd_num);
    if (ret != ERRCODE_SUCC) {
        dfx_fault_event_data(FAULT_DFX_DIAG_UNREGISTER_CMD_FAIL, NULL, 0);
        goto end;
    }

    for (i = 0; i < CONFIG_DIAG_CMD_TBL_NUM; i++) {
        if ((cmd_ctrl->user_cmd_list[i] == cmd_tbl) && (cmd_ctrl->aus_usert_cmd_num[i] == cmd_num)) {
            ret = ERRCODE_SUCC;
            cmd_ctrl->user_cmd_list[i] = NULL;
            cmd_ctrl->aus_usert_cmd_num[i] = 0;
            goto end;
        }
    }
    ret = ERRCODE_FAIL;
end:
    dfx_int_restore(lock_stat);
    return ret;
}

errcode_t diag_pkt_router_run_cmd(diag_ser_data_t *data, diag_option_t *option)
{
    diag_head_req_stru_t *req = (diag_head_req_stru_t *)((uint8_t *)data + sizeof(diag_ser_data_t));
    uint8_t *usr_data = (uint8_t *)((uint8_t *)req + sizeof(diag_head_req_stru_t));

    uint16_t size = data->header.length - (uint16_t)sizeof(diag_head_req_stru_t);
    uint16_t cmd_id = makeu16(req->cmd_id, req->mod_id);

    dfx_log_debug("diag_pkt_router_run_cmd: cmd_id = 0x%x param_size = %u\r\n", cmd_id, size);
    diag_cmd_f cmd_f = diag_find_usr_cmd_proc_func(cmd_id);
    if (cmd_f) {
        diag_option_t temp_option = *option;
        dfx_log_debug("diag_pkt_router_run_cmd: call cmd func! cmd_id=0x%x param_size=%u\r\n", cmd_id, size);
        cmd_f(cmd_id, usr_data, size, &temp_option);
        return ERRCODE_SUCC;
    }
    return ERRCODE_FAIL;
}
