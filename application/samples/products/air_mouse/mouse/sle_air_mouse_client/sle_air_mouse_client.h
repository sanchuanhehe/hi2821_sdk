/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
* Description: Sle Air Mouse with dongle Client header \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-08-01, Create file. \n
 */
#ifndef SLE_AIR_MOUSE_CLIENT_H
#define SLE_AIR_MOUSE_CLIENT_H

#include "sle_ssap_client.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

void sle_air_mouse_client_init(void);

uint16_t get_g_sle_air_mouse_client_conn_id(void);

uint8_t get_g_sle_air_mouse_client_conn_state(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif