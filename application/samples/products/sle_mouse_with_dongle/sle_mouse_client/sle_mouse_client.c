/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: SLE Uart Client Source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-08-01, Create file. \n
 */
#include "string.h"
#include "common_def.h"
#include "osal_debug.h"
#include "osal_task.h"
#include "cmsis_os2.h"
#include "securec.h"
#include "bts_le_gap.h"
#include "sle_device_discovery.h"
#include "sle_connection_manager.h"
#include "sle_ssap_client.h"
#include "sle_mouse_client.h"

#define SLE_SEEK_INTERVAL_DEFAULT 100
#define SLE_SEEK_WINDOW_DEFAULT 100
#define SLE_UART_TASK_DELAY_MS 1000

#define SLE_MOUSE_DONGLE_CLIENT_LOG                     "[sle mouse dongle client]"

static sle_announce_seek_callbacks_t g_sle_mouse_client_seek_cbk = { 0 };
static sle_connection_callbacks_t g_sle_mouse_client_connect_cbk = { 0 };
static sle_addr_t g_sle_mouse_server_addr = { 0 };
static uint16_t g_sle_mouse_client_conn_id = 0;
static uint8_t g_sle_mouse_client_conn_state = SLE_ACB_STATE_NONE;
static bool g_sle_enable = 0;

uint8_t get_g_sle_mouse_client_conn_state(void)
{
    return g_sle_mouse_client_conn_state;
}

uint16_t get_g_sle_mouse_client_conn_id(void)
{
    return g_sle_mouse_client_conn_id;
}

static void sle_mouse_client_start_scan(void)
{
    sle_seek_param_t param = { 0 };
    param.own_addr_type = 0;
    param.filter_duplicates = 0;
    param.seek_filter_policy = 0;
    param.seek_phys = 1;
    param.seek_type[0] = 1;
    param.seek_interval[0] = SLE_SEEK_INTERVAL_DEFAULT;
    param.seek_window[0] = SLE_SEEK_WINDOW_DEFAULT;
    sle_set_seek_param(&param);
    sle_start_seek();
    osDelay(SLE_UART_TASK_DELAY_MS);
}

static void sle_mouse_client_sle_enable_cbk(errcode_t status)
{
    if (status != 0) {
        osal_printk("%s sle_mouse_client_sle_enable_cbk, status error\r\n", SLE_MOUSE_DONGLE_CLIENT_LOG);
    } else {
        g_sle_enable = true;
        sle_mouse_client_start_scan();
        osDelay(SLE_UART_TASK_DELAY_MS);
    }
}

static void sle_mouse_client_seek_enable_cbk(errcode_t status)
{
    if (status != 0) {
        osal_printk("%s sle_mouse_client_seek_enable_cbk, status error\r\n", SLE_MOUSE_DONGLE_CLIENT_LOG);
    }
}

static void sle_mouse_client_seek_result_info_cbk(sle_seek_result_info_t *seek_result_data)
{
    osal_printk("sle mouse pattern:%s\r\n", "sle_mouse");
    osal_printk("%s sle mouse scan data:%s\r\n", SLE_MOUSE_DONGLE_CLIENT_LOG, seek_result_data->data);
    if (seek_result_data == NULL) {
        osal_printk("%s status error\r\n", SLE_MOUSE_DONGLE_CLIENT_LOG);
    } else if (strstr((const char *)seek_result_data->data, (const char *)"sle_mouse") != NULL) {
        if (memcpy_s(&g_sle_mouse_server_addr, sizeof(sle_addr_t),
                     &seek_result_data->addr, sizeof(sle_addr_t)) != EOK) {
            osal_printk("%s sle seek result data addr memcpy fail\r\n", SLE_MOUSE_DONGLE_CLIENT_LOG);
        }

        sle_stop_seek();
        osDelay(SLE_UART_TASK_DELAY_MS);
    }
}

static void sle_mouse_client_seek_disable_cbk(errcode_t status)
{
    if (status != 0) {
        osal_printk("%s sle_mouse_client_seek_disable_cbk, status error\r\n", SLE_MOUSE_DONGLE_CLIENT_LOG);
    } else {
        sle_connect_remote_device(&g_sle_mouse_server_addr);
        osDelay(SLE_UART_TASK_DELAY_MS);
    }
}

static void sle_mouse_client_seek_cbk_register(void)
{
    g_sle_mouse_client_seek_cbk.sle_enable_cb = sle_mouse_client_sle_enable_cbk;
    g_sle_mouse_client_seek_cbk.seek_enable_cb = sle_mouse_client_seek_enable_cbk;
    g_sle_mouse_client_seek_cbk.seek_result_cb = sle_mouse_client_seek_result_info_cbk;
    g_sle_mouse_client_seek_cbk.seek_disable_cb = sle_mouse_client_seek_disable_cbk;
    if (sle_announce_seek_register_callbacks(&g_sle_mouse_client_seek_cbk) != ERRCODE_BT_SUCCESS) {
        osal_printk("%s register ble_client_enable_cb failed\r\n", SLE_MOUSE_DONGLE_CLIENT_LOG);
    }
}

static void sle_mouse_client_connect_state_changed_cbk(uint16_t conn_id, const sle_addr_t *addr,
                                                       sle_acb_state_t conn_state, sle_pair_state_t pair_state,
                                                       sle_disc_reason_t disc_reason)
{
    unused(addr);
    unused(pair_state);
    osal_printk("%s conn state changed disc_reason:0x%x\r\n", SLE_MOUSE_DONGLE_CLIENT_LOG, disc_reason);
    g_sle_mouse_client_conn_id = conn_id;
    g_sle_mouse_client_conn_state = conn_state;
    if (conn_state == SLE_ACB_STATE_CONNECTED) {
        osal_printk("%s SLE_ACB_STATE_CONNECTED\r\n", SLE_MOUSE_DONGLE_CLIENT_LOG);
    } else if (conn_state == SLE_ACB_STATE_NONE) {
        osal_printk("%s SLE_ACB_STATE_NONE\r\n", SLE_MOUSE_DONGLE_CLIENT_LOG);
    } else if (conn_state == SLE_ACB_STATE_DISCONNECTED) {
        osal_printk("%s SLE_ACB_STATE_DISCONNECTED\r\n", SLE_MOUSE_DONGLE_CLIENT_LOG);
        sle_mouse_client_start_scan();
    } else {
        osal_printk("%s status error\r\n", SLE_MOUSE_DONGLE_CLIENT_LOG);
    }
}

static void sle_mouse_client_connect_cbk_register(void)
{
    g_sle_mouse_client_connect_cbk.connect_state_changed_cb = sle_mouse_client_connect_state_changed_cbk;
    sle_connection_register_callbacks(&g_sle_mouse_client_connect_cbk);
}

void sle_mouse_client_init(void)
{
    sle_mouse_client_seek_cbk_register();
    while (g_sle_enable == false) {
        osDelay(SLE_UART_TASK_DELAY_MS);
        enable_sle();
    }
    sle_mouse_client_connect_cbk_register();
}