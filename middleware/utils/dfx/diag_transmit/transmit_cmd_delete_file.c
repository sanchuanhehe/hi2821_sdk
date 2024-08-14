/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: delete file cmd
 * This file should be changed only infrequently and with great care.
 */
#include "transmit_cmd_delete_file.h"
#include "transmit_cmd_ls.h"
#include "diag.h"
#include "transmit_cmd_delete_file_st.h"
#include "transmit_file_operation.h"
#include "transmit_debug.h"
#include "transmit_cmd_id.h"
#include "securec.h"
#include "errcode.h"
#include "transmit_send_recv_pkt.h"

errcode_t transmit_cmd_delete_file(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size,
                                   diag_option_t *option)
{
    unused(cmd_param_size);
    errcode_t ret_value;
    diag_del_cmd_t *req = (diag_del_cmd_t *)transmit_get_tlv_payload(cmd_param);

    uint32_t pkt_size = (uint32_t)sizeof(transmit_pkt_tlv_t) + (uint32_t)sizeof(diag_del_ind_t);
    transmit_pkt_tlv_t *tlv = transmit_item_get_pkt_buf(NULL, pkt_size);
    if (tlv == NULL) {
        return ERRCODE_MALLOC;
    }

    tlv->type =  transmit_build_tlv_type(1, 2); /* the type is assigned 2 in ack command */
    tlv->len = transmit_build_tlv_len(sizeof(diag_del_ind_t));
    diag_del_ind_t *ind = (diag_del_ind_t *)tlv->data;

    if (req->file_type == 0) {
        ret_value = transmit_file_delete((const char *)req->name);
    } else {
        ret_value = transmit_file_rmdir((const char *)req->name);
    }

    ind->ret_value = ret_value;
    (void)transmit_send_packet(DIAG_CMD_ID_TRANSMIT_DEL_FILE, (uint8_t *)tlv, pkt_size, option, true);
    if (ret_value == ERRCODE_SUCC) {
        dfx_log_debug("transmit_cmd_delete_file OK\r\n");
    } else {
        dfx_log_err("transmit_cmd_delete_file failed\r\n");
    }

    transmit_item_free_pkt_buf(NULL, tlv);
    return ERRCODE_SUCC;
}
