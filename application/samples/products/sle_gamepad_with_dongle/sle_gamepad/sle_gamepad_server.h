/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: SLE GAMEPAD Server Config. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-09-28, Create file. \n
 */

#ifndef SLE_GAMEPAD_SERVER_H
#define SLE_GAMEPAD_SERVER_H

#include <stdint.h>
#include "errcode.h"
#include "osal_debug.h"
#include "sle_ssap_server.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/* Service UUID */
#define SLE_UUID_SERVER_SERVICE        0x2222

/* Property UUID */
#define SLE_UUID_SERVER_NTF_REPORT     0x2323

/* Property Property */
#define SLE_UUID_TEST_PROPERTIES  (SSAP_PERMISSION_READ | SSAP_PERMISSION_WRITE)

/* Operation indication */
#define SLE_UUID_TEST_OPERATION_INDICATION  (SSAP_OPERATE_INDICATION_BIT_READ | SSAP_OPERATE_INDICATION_BIT_WRITE)

/* Descriptor Property */
#define SLE_UUID_TEST_DESCRIPTOR   (SSAP_PERMISSION_READ | SSAP_PERMISSION_WRITE)

#define sample_print(fmt, args...)  osal_printk(fmt, ##args)
#define SLE_GAMEPAD_SERVER_LOG      "[sle gamepad server]"
#define SLE_SERVER_INIT_DELAY_MS    1000

errcode_t sle_gamepad_server_init(ssaps_read_request_callback ssaps_read_callback,
                                  ssaps_write_request_callback ssaps_write_callback);
errcode_t sle_gamepad_server_send_report_by_uuid(const uint8_t *data, uint8_t len);
errcode_t sle_gamepad_server_send_report_by_handle(const uint8_t *data, uint8_t len);
uint16_t sle_gamepad_client_is_connected(void);
errcode_t get_g_sle_gamepad_server_conn_state(uint8_t *conn_state);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif