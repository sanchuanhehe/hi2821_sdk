/*
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: ota upgrade.
 * This file should be changed only infrequently and with great care.
 */

#include "ota_upgrade.h"
#include "ota_upgrade_handle.h"
#include "diag_service.h"

#define OTA_UPGRADE_PREPARE  0x1
#define OTA_UPGRADE_REQUEST  0x2
#define OTA_UPGRADE_START    0x3
#define OTA_UPGRADE_STOP     0x4
#define OTA_UPGRADE_GETINFO  0x5


typedef struct {
    uint8_t cmd_id;
    upgrade_pkt_recv_hook handler;
} upgrade_cmd_ind_item_t;

static upgrade_cmd_ind_item_t g_upgrade_cmd_id_tbl[] = {
    { OTA_UPGRADE_PREPARE,      ota_upgrade_prepare},
    { OTA_UPGRADE_REQUEST,  ota_upgrade_request },
    { OTA_UPGRADE_START,    ota_upgrade_start },
    { OTA_UPGRADE_STOP,      NULL },
    { OTA_UPGRADE_GETINFO,     ota_upgrade_getinfo },
};

static errcode_t ota_upgrade_cmd_receiver(uint8_t cmd_id, uint8_t *cmd_param, uint16_t cmd_param_size, uint8_t dst)
{
    uint8_t i;
    for (i = 0; i < sizeof(g_upgrade_cmd_id_tbl) / sizeof(g_upgrade_cmd_id_tbl[0]); i++) {
        upgrade_cmd_ind_item_t *item = &g_upgrade_cmd_id_tbl[i];
        if (item->cmd_id == cmd_id && item->handler != NULL) {
            item->handler(cmd_id, cmd_param, cmd_param_size, dst);
            return ERRCODE_SUCC;
        }
    }
    return ERRCODE_NOT_SUPPORT;
}

errcode_t ota_upgrade_service_process(diag_ser_data_t *data)
{
    diag_ser_frame_t *request = (diag_ser_frame_t *)((uint8_t *)data + sizeof(diag_ser_data_t));
    tlv_t *tlv_data = (tlv_t *)((uint8_t *)request + sizeof(diag_ser_frame_t));
    uint16_t size = (uint16_t)(data->header.length - sizeof(diag_ser_frame_t));
    uint8_t dst = data->header.src;
    return ota_upgrade_cmd_receiver(request->cmd_id, (uint8_t *)tlv_data, size, dst);
}

errcode_t uapi_upgrade_init(void)
{
    errcode_t ret;
    ret = uapi_diag_service_register(DIAG_SER_OTA, ota_upgrade_service_process);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    return ERRCODE_SUCC;
}
