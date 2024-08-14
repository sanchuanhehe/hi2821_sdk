/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: SLE KEYBOARD sample of client. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-04-03, Create file. \n
 */
#ifndef SLE_KEYBOARD_CLIENT_H
#define SLE_KEYBOARD_CLIENT_H

#include "sle_ssap_client.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

void sle_keyboard_client_init(ssapc_notification_callback notification_cb, ssapc_indication_callback indication_cb);
void sle_keyboard_start_scan(void);
uint16_t get_sle_keyboard_conn_id(void);
ssapc_write_param_t get_sle_keyboard_send_param(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif