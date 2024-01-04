/*
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: ota upgrade header file
 */

#ifndef OTA_UPGRADE_H
#define OTA_UPGRADE_H

#include "diag_service.h"

typedef errcode_t (*upgrade_pkt_recv_hook)(uint8_t cmd_id, uint8_t *cmd_param, uint16_t cmd_param_size, uint8_t dst);

errcode_t ota_upgrade_service_process(diag_ser_data_t *data);

errcode_t uapi_upgrade_init(void);

#endif
