/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: SLE UART sample of client. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-04-03, Create file. \n
 */
#ifndef SLE_UART_CLIENT_H
#define SLE_UART_CLIENT_H

#include "sle_ssap_client.h"

void sle_uart_client_init(ssapc_notification_callback notification_cb, ssapc_indication_callback indication_cb);

void sle_uart_start_scan(void);

uint16_t get_g_sle_uart_conn_id(void);

ssapc_write_param_t *get_g_sle_uart_send_param(void);

#endif