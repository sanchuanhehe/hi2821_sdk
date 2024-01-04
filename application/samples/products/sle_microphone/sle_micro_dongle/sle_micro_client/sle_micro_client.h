/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: SLE MICRO sample of client. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-09-23, Create file. \n
 */
#ifndef SLE_MICRO_CLIENT_H
#define SLE_MICRO_CLIENT_H

#include "sle_ssap_client.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

uint16_t get_sle_micro_conn_id(void);
ssapc_write_param_t *get_sle_micro_send_param(void);
uint8_t get_ssap_find_ready(void);
void sle_micro_client_init(ssapc_notification_callback notification_cb, ssapc_indication_callback indication_cb);
void sle_micro_start_scan(void);
uint8_t get_sle_micro_get_connect_state(void);
uint8_t get_ssap_param_update_ready(void);
#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif