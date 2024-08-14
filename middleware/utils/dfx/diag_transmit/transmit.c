/*
 * Copyright (c) @CompanyNameMagicTag 2021-2023. All rights reserved.
 * Description: transmit
 * This file should be changed only infrequently and with great care.
 */
#include "transmit.h"
#include "diag.h"
#include "transmit_cmd_id.h"
#include "transmit_cmd_ls.h"
#include "transmit_cmd_dump_by_name.h"
#include "transmit_cmd_delete_file.h"
#include "transmit_send_recv_pkt.h"
#include "transmit_debug.h"

typedef struct {
    uint8_t cmd_id;
    transmit_pkt_recv_hook handler;
} transmit_cmd_ind_item_t;

#if CONFIG_DFX_SUPPORT_TRANSMIT_FILE == DFX_YES
STATIC transmit_cmd_ind_item_t g_transmit_cmd_id_tbl[] = {
    { DIAG_CMD_ID_TRANSMIT_START,      transmit_receiver_start},
    { DIAG_CMD_ID_TRANSMIT_NEGOTIATE,  transmit_receiver_negotiate },
    { DIAG_CMD_ID_TRANSMIT_REQUEST,    transmit_receiver_data_request },
    { DIAG_CMD_ID_TRANSMIT_REPLY,      transmit_receiver_data_reply },
    { DIAG_CMD_ID_TRANSMIT_NOTIFY,     transmit_receiver_notify },
    { DIAG_CMD_ID_TRANSMIT_STOP,       transmit_receiver_stop },
#if (CONFIG_DFX_SUPPORT_FILE_SYSTEM == DFX_YES)
    { DIAG_CMD_ID_TRANSMIT_LS,         transmit_cmd_ls},
    { DIAG_CMD_ID_TRANSMIT_DUMP_FILE,  transmit_cmd_dump_by_file_name},
    { DIAG_CMD_ID_TRANSMIT_DEL_FILE,   transmit_cmd_delete_file}
#endif
};

STATIC errcode_t transmit_cmd_receiver(uint8_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option)
{
    uint32_t i;
    for (i = 0; i < sizeof(g_transmit_cmd_id_tbl) / sizeof(g_transmit_cmd_id_tbl[0]); i++) {
        transmit_cmd_ind_item_t *item = &g_transmit_cmd_id_tbl[i];
        if (item->cmd_id == cmd_id && item->handler != NULL) {
            transmit_printf_receive_frame(cmd_id, cmd_param, cmd_param_size, option, true);
            item->handler(cmd_id, cmd_param, cmd_param_size, option, true);
            return ERRCODE_SUCC;
        }
    }
    return ERRCODE_NOT_SUPPORT;
}

STATIC errcode_t transmit_service_process(diag_ser_data_t *data)
{
    diag_ser_frame_t *req = (diag_ser_frame_t *)((uint8_t *)data + sizeof(diag_ser_data_t));
    uint8_t *usr_data = (uint8_t *)((uint8_t *)req + sizeof(diag_ser_frame_t));
    uint16_t size = data->header.length - (uint16_t)sizeof(diag_ser_frame_t);

    diag_option_t option = DIAG_OPTION_INIT_VAL;
    option.peer_addr = data->header.src;
    return transmit_cmd_receiver(req->cmd_id, usr_data, size, &option);
}

errcode_t uapi_transmit_init(void)
{
    errcode_t ret;
    ret = transmit_item_module_init();
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    ret = uapi_diag_service_register(DIAG_SER_FILE_TRANFER, transmit_service_process);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    return ERRCODE_SUCC;
}
#endif
