/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: SLE keyboard server Config. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-07-29, Create file. \n
 */

#ifndef SLE_KEYBOARD_SERVER_H
#define SLE_KEYBOARD_SERVER_H

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
#define SLE_KEYBOARD_SERVER_LOG     "[sle keyboard server]"
#define SLE_SERVER_INIT_DELAY_MS    1000

errcode_t sle_keyboard_server_init(ssaps_read_request_callback ssaps_read_callback,
                                   ssaps_write_request_callback ssaps_write_callback);
errcode_t sle_keyboard_server_send_report_by_uuid(const uint8_t *data, uint8_t len);
errcode_t sle_keyboard_server_send_report_by_handle(const uint8_t *data, uint8_t len);
uint16_t sle_keyboard_client_is_connected(void);
typedef void (*sle_keyboard_server_msg_queue)(uint8_t *buffer_addr, uint32_t buffer_size);
void sle_keyboard_server_register_msg(sle_keyboard_server_msg_queue sle_keyboard_server_msg);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif