/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: SLE GAMEPAD sample of client. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-09-28, Create file. \n
 */
#ifndef SLE_GAMEPAD_CLIENT_H
#define SLE_GAMEPAD_CLIENT_H

#include "sle_ssap_client.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

void sle_gamepad_client_init(ssapc_notification_callback notification_cb, ssapc_indication_callback indication_cb);
void sle_gamepad_start_scan(void);
uint16_t get_sle_gamepad_conn_id(void);
ssapc_write_param_t get_sle_gamepad_send_param(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif