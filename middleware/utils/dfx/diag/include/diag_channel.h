/*
 * Copyright (c) @CompanyNameMagicTag 2021-2023. All rights reserved.
 * Description: diag channel adapt api header file
 * This file should be changed only infrequently and with great care.
 */
#ifndef DIAG_CHANNEL_H
#define DIAG_CHANNEL_H

#include <stdint.h>
#include <stdbool.h>
#include "errcode.h"
#include "zdiag_config.h"
#include "diag_common.h"
#include "dfx_write_interface.h"
#include "dfx_resource_id.h"

typedef enum {
    DIAG_CHANNEL_ATTR_NONE        = 0x0,
    DIAG_CHANNEL_ATTR_NEED_RX_BUF = 0x1,
} diag_channel_attribute_t;

typedef int32_t (*diag_channel_tx_hook)(void *fd, dfx_data_type_t data_type, uint8_t *data[], uint16_t len[],
                                        uint8_t cnt);
typedef errcode_t (*diag_channel_notify_hook)(uint32_t id, uint32_t data);

errcode_t uapi_diag_channel_init(diag_channel_id_t id, uint32_t attribute);

errcode_t uapi_diag_channel_set_tx_hook(diag_channel_id_t id, diag_channel_tx_hook hook);

errcode_t uapi_diag_channel_set_notify_hook(diag_channel_id_t id, diag_channel_notify_hook hook);

/* 字符串数据接收函数，data中是字符数据，可能需要组包(形成完整Diag帧数据)，也可能包含多个帧 */
int32_t uapi_diag_channel_rx_mux_char_data(diag_channel_id_t id, uint8_t *data, uint16_t size);

/* Diag帧数据接收函数，data中是一个完整的Diag帧数据，无需组包 */
errcode_t uapi_diag_channel_rx_frame_data(diag_channel_id_t id, uint8_t *data, uint16_t size);

errcode_t uapi_diag_channel_set_connect_hso_addr(diag_channel_id_t id, uint8_t hso_addr);
#endif
