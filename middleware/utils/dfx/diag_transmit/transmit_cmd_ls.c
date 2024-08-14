/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: ls cmd
 * This file should be changed only infrequently and with great care.
 */
#include "transmit_cmd_ls.h"
#include "diag.h"
#include "transmit_cmd_ls_st.h"
#include "transmit_file_operation.h"
#include "transmit_debug.h"
#include "transmit_send_recv_pkt.h"
#include "transmit_cmd_id.h"
#include "securec.h"
#include "errcode.h"

typedef struct {
    uint16_t cmd_id;
    diag_option_t option;
    uint32_t idx;
} usr_data_param_t;

STATIC void report_ls_info(transmit_file_ls_node_info_t *info, uintptr_t usr_data)
{
    usr_data_param_t *usr_param = (usr_data_param_t *)usr_data;

    uint32_t path_size = 0;
    if (info->name != NULL) {
        path_size = (uint8_t)(strlen(info->name) + 1);
    }
    uint32_t data_size = (uint32_t)sizeof(diag_ls_ind_t) + path_size;
    uint32_t tlv_ext_len = transmit_get_tlv_ext_len(data_size);
    uint32_t pkt_size = (uint32_t)sizeof(transmit_pkt_tlv_t) + tlv_ext_len + data_size;
    transmit_pkt_tlv_t *tlv = transmit_item_get_pkt_buf(NULL, pkt_size);
    if (tlv == NULL) {
        return;
    }

    tlv->type =  transmit_build_tlv_type(1, 2); /* the type is assigned 2 in ack command */
    tlv->len = transmit_build_tlv_len(data_size);
    if (tlv_ext_len > 0) {
        (void)memcpy_s(tlv->data, tlv_ext_len, ((uint8_t *)&data_size), tlv_ext_len);
    }

    diag_ls_ind_t *ind = (diag_ls_ind_t *)((uint8_t *)tlv + sizeof(transmit_pkt_tlv_t) + tlv_ext_len);

    if (path_size != 0) {
        ind->file_size = info->file_size;
        ind->idx = (uint16_t)(usr_param->idx++);
        ind->path_len = (uint8_t)path_size;
        ind->file_type = info->is_dir ? 1 : 0;

        (void)memset_s(ind->file_name, path_size, 0, path_size);
        if (memcpy_s(ind->file_name, path_size, info->name, strlen(info->name)) != EOK) {
            dfx_log_err("[ERR]report file : %s failed!\r\n", info->name);
            goto end;
        }
    } else {
        (void)memset_s(ind, sizeof(diag_ls_ind_t), 0xFF, sizeof(diag_ls_ind_t));
    }
end:
    (void)transmit_send_packet(DIAG_CMD_ID_TRANSMIT_LS, (uint8_t *)tlv, pkt_size, &usr_param->option, true);
    transmit_item_free_pkt_buf(NULL, tlv);
}

errcode_t transmit_cmd_ls(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option)
{
    unused(cmd_param_size);
    usr_data_param_t usr_param;
    diag_ls_cmd_t *req = (diag_ls_cmd_t *)transmit_get_tlv_payload(cmd_param);

    usr_param.cmd_id = cmd_id;
    usr_param.option = *option;
    usr_param.idx = 0;
    if (transmit_file_ls((const char *)req->path, report_ls_info, (uintptr_t)&usr_param) != ERRCODE_SUCC) {
        dfx_log_err("[ERR]get file list failed!\r\n");
        return ERRCODE_FAIL;
    }

    /* Send a packet with all 0xFF to indicate the end. */
    transmit_file_ls_node_info_t ls_node = {0};
    report_ls_info(&ls_node, (uintptr_t)&usr_param);
    return ERRCODE_SUCC;
}
