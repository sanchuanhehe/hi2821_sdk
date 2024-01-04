/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \r\n
 *
 * Description: Sle Air Mouse with dongle Client Source. \r\n
 * Author: @CompanyNameTag \r\n
 * History: \r\n
 * 2023-08-01, Create file. \r\n
 */
#include "sle_air_mouse_client.h"
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
#include "glp_interface.h"

#define UUID_16BIT_LEN 2
#define UUID_128BIT_LEN 16

#define SLE_SEEK_INTERVAL_DEFAULT 100
#define SLE_SEEK_WINDOW_DEFAULT 100
#define SLE_UART_TASK_DELAY_MS 1000

#define SLE_AIR_MOUSE_DONGLE_CLIENT_LOG                     "[sle air mouse dongle client]"

static sle_announce_seek_callbacks_t g_sle_air_mouse_client_seek_cbk = { 0 };
static sle_connection_callbacks_t g_sle_air_mouse_client_connect_cbk = { 0 };
ssapc_callbacks_t             g_ssapc_cbk = {0};
static sle_addr_t g_sle_air_mouse_server_addr = { 0 };
static uint16_t g_sle_air_mouse_client_conn_id = 0;
static uint8_t g_sle_air_mouse_client_conn_state = SLE_ACB_STATE_NONE;
static bool g_sle_enable = 0;
ssapc_find_service_result_t   g_find_service_result = {0};

uint8_t get_g_sle_air_mouse_client_conn_state(void)
{
    return g_sle_air_mouse_client_conn_state;
}

uint16_t get_g_sle_air_mouse_client_conn_id(void)
{
    return g_sle_air_mouse_client_conn_id;
}

static void sle_air_mouse_client_start_scan(void)
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
    osal_msleep(SLE_UART_TASK_DELAY_MS);
}

static void sle_air_mouse_client_sle_enable_cbk(errcode_t status)
{
    if (status != 0) {
        osal_printk("%s sle_air_mouse_client_sle_enable_cbk, status error\r\n", SLE_AIR_MOUSE_DONGLE_CLIENT_LOG);
    } else {
        g_sle_enable = true;
        sle_air_mouse_client_start_scan();
        osal_msleep(SLE_UART_TASK_DELAY_MS);
    }
}

static void sle_air_mouse_client_seek_enable_cbk(errcode_t status)
{
    if (status != 0) {
        osal_printk("%s sle_air_mouse_client_seek_enable_cbk, status error\r\n", SLE_AIR_MOUSE_DONGLE_CLIENT_LOG);
    }
}

static void sle_air_mouse_client_seek_result_info_cbk(sle_seek_result_info_t *seek_result_data)
{
    osal_printk("sle air mouse pattern:%s\r\n", "sle_air_mouse");
    osal_printk("%s sle air mouse scan data:%s\r\n", SLE_AIR_MOUSE_DONGLE_CLIENT_LOG, seek_result_data->data);
    if (seek_result_data == NULL) {
        osal_printk("%s status error\r\n", SLE_AIR_MOUSE_DONGLE_CLIENT_LOG);
    } else if (strstr((const char *)seek_result_data->data, (const char *)"sle_air_mouse") != NULL) {
        if (memcpy_s(&g_sle_air_mouse_server_addr, sizeof(sle_addr_t),
                     &seek_result_data->addr, sizeof(sle_addr_t)) != EOK) {
            osal_printk("%s sle seek result data addr memcpy fail\r\n", SLE_AIR_MOUSE_DONGLE_CLIENT_LOG);
        }

        sle_stop_seek();
        osal_msleep(SLE_UART_TASK_DELAY_MS);
    }
}

static void sle_air_mouse_client_seek_disable_cbk(errcode_t status)
{
    if (status != 0) {
        osal_printk("%s sle_air_mouse_client_seek_disable_cbk, status error\r\n", SLE_AIR_MOUSE_DONGLE_CLIENT_LOG);
    } else {
        sle_connect_remote_device(&g_sle_air_mouse_server_addr);
        osal_msleep(SLE_UART_TASK_DELAY_MS);
    }
}

static void sle_air_mouse_client_seek_cbk_register(void)
{
    g_sle_air_mouse_client_seek_cbk.sle_enable_cb = sle_air_mouse_client_sle_enable_cbk;
    g_sle_air_mouse_client_seek_cbk.seek_enable_cb = sle_air_mouse_client_seek_enable_cbk;
    g_sle_air_mouse_client_seek_cbk.seek_result_cb = sle_air_mouse_client_seek_result_info_cbk;
    g_sle_air_mouse_client_seek_cbk.seek_disable_cb = sle_air_mouse_client_seek_disable_cbk;
    if (sle_announce_seek_register_callbacks(&g_sle_air_mouse_client_seek_cbk) != ERRCODE_BT_SUCCESS) {
        osal_printk("%s register ble_client_enable_cb failed\r\n", SLE_AIR_MOUSE_DONGLE_CLIENT_LOG);
    }
}

static void sle_air_mouse_client_connect_state_changed_cbk(uint16_t conn_id, const sle_addr_t *addr,
    sle_acb_state_t conn_state, sle_pair_state_t pair_state, sle_disc_reason_t disc_reason)
{
    unused(addr);
    unused(pair_state);
    osal_printk("%s conn state changed disc_reason:0x%x\r\n", SLE_AIR_MOUSE_DONGLE_CLIENT_LOG, disc_reason);
    g_sle_air_mouse_client_conn_id = conn_id;
    g_sle_air_mouse_client_conn_state = conn_state;
    if (conn_state == SLE_ACB_STATE_CONNECTED) {
        osal_printk("%s SLE_ACB_STATE_CONNECTED\r\n", SLE_AIR_MOUSE_DONGLE_CLIENT_LOG);
    } else if (conn_state == SLE_ACB_STATE_NONE) {
        osal_printk("%s SLE_ACB_STATE_NONE\r\n", SLE_AIR_MOUSE_DONGLE_CLIENT_LOG);
    } else if (conn_state == SLE_ACB_STATE_DISCONNECTED) {
        osal_printk("%s SLE_ACB_STATE_DISCONNECTED\r\n", SLE_AIR_MOUSE_DONGLE_CLIENT_LOG);
        sle_air_mouse_client_start_scan();
    } else {
        osal_printk("%s status error\r\n", SLE_AIR_MOUSE_DONGLE_CLIENT_LOG);
    }
}

static void sle_air_mouse_client_connect_cbk_register(void)
{
    g_sle_air_mouse_client_connect_cbk.connect_state_changed_cb = sle_air_mouse_client_connect_state_changed_cbk;
    sle_connection_register_callbacks(&g_sle_air_mouse_client_connect_cbk);
}

static void ssapc_exchange_info_cbk(uint8_t client_id, uint16_t conn_id, ssap_exchange_info_t *param,
    errcode_t status)
{
    osal_printk("[ssap client] pair complete client id:%d status:%d\r\n", client_id, status);
    osal_printk("[ssap client] exchange mtu, mtu size: %d, version: %d.\r\n",
        param->mtu_size, param->version);

    ssapc_find_structure_param_t find_param = {0};
    find_param.type = SSAP_FIND_TYPE_PRIMARY_SERVICE;
    find_param.start_hdl = 1;
    find_param.end_hdl = 0xFFFF;
    ssapc_find_structure(0, conn_id, &find_param);
}

static void ssapc_find_structure_cbk(uint8_t client_id, uint16_t conn_id, ssapc_find_service_result_t *service,
    errcode_t status)
{
    osal_printk("[ssap client] find structure cbk client: %d conn_id:%d status: %d \r\n",
        client_id, conn_id, status);
    osal_printk("[ssap client] find structure start_hdl:[0x%02x], end_hdl:[0x%02x], uuid len:%d\r\n",
        service->start_hdl, service->end_hdl, service->uuid.len);
    if (service->uuid.len == UUID_16BIT_LEN) {
        osal_printk("[ssap client] structure uuid:[0x%02x][0x%02x]\r\n",
            service->uuid.uuid[14], service->uuid.uuid[15]); /* 14 15: uuid index */
    } else {
        for (uint8_t idx = 0; idx < UUID_128BIT_LEN; idx++) {
            osal_printk("[ssap client] structure uuid[%d]:[0x%02x]\r\n", idx, service->uuid.uuid[idx]);
        }
    }
    g_find_service_result.start_hdl = service->start_hdl;
    g_find_service_result.end_hdl = service->end_hdl;
    if (memcpy_s(&g_find_service_result.uuid, sizeof(sle_uuid_t), &service->uuid, sizeof(sle_uuid_t)) != EOK) {
        osal_printk("%s sle find service result fail\r\n", SLE_AIR_MOUSE_DONGLE_CLIENT_LOG);
    }
}

static void ssapc_find_structure_cmp_cbk(uint8_t client_id, uint16_t conn_id,
    ssapc_find_structure_result_t *structure_result, errcode_t status)
{
    osal_printk("[ssap client] find structure cmp cbk client id:%d status:%d type:%d uuid len:%d \r\n",
        client_id, status, structure_result->type, structure_result->uuid.len);
    if (structure_result->uuid.len == UUID_16BIT_LEN) {
        osal_printk("[ssap client] find structure cmp cbk structure uuid:[0x%02x][0x%02x]\r\n",
            structure_result->uuid.uuid[14], structure_result->uuid.uuid[15]); /* 14 15: uuid index */
    } else {
        for (uint8_t idx = 0; idx < UUID_128BIT_LEN; idx++) {
            osal_printk("[ssap client] find structure cmp cbk structure uuid[%d]:[0x%02x]\r\n", idx,
                structure_result->uuid.uuid[idx]);
        }
    }
    uint8_t data[] = {0x11, 0x22, 0x33, 0x44};
    uint8_t len = sizeof(data);
    ssapc_write_param_t param = {0};
    param.handle = g_find_service_result.start_hdl;
    param.type = SSAP_PROPERTY_TYPE_VALUE;
    param.data_len = len;
    param.data = data;
    ssapc_write_req(0, conn_id, &param);
}

static void ssapc_find_property_cbk(uint8_t client_id, uint16_t conn_id,
    ssapc_find_property_result_t *property, errcode_t status)
{
    osal_printk("[ssap client] find property cbk, client id: %d, conn id: %d, operate ind: %d, "
        "descriptors count: %d status:%d.\r\n", client_id, conn_id, property->operate_indication,
        property->descriptors_count, status);
    for (uint16_t idx = 0; idx < property->descriptors_count; idx++) {
        osal_printk("[ssap client] find property cbk, descriptors type [%d]: 0x%02x.\r\n",
            idx, property->descriptors_type[idx]);
    }
    if (property->uuid.len == UUID_16BIT_LEN) {
        osal_printk("[ssap client] find property cbk, uuid: %02x %02x.\r\n",
            property->uuid.uuid[14], property->uuid.uuid[15]); /* 14 15: uuid index */
    } else if (property->uuid.len == UUID_128BIT_LEN) {
        for (uint16_t idx = 0; idx < UUID_128BIT_LEN; idx++) {
            osal_printk("[ssap client] find property cbk, uuid [%d]: %02x.\r\n",
                idx, property->uuid.uuid[idx]);
        }
    }
}

static void ssapc_write_cfm_cbk(uint8_t client_id, uint16_t conn_id, ssapc_write_result_t *write_result,
    errcode_t status)
{
    osal_printk("[ssap client] write cfm cbk, client id: %d status:%d.\r\n", client_id, status);
    ssapc_read_req(0, conn_id, write_result->handle, write_result->type);
}

static void ssapc_read_cfm_cbk(uint8_t client_id, uint16_t conn_id, ssapc_handle_value_t *read_data,
    errcode_t status)
{
    osal_printk("[ssap client] read cfm cbk client id: %d conn id: %d status: %d\r\n",
        client_id, conn_id, status);
    osal_printk("[ssap client] read cfm cbk handle: %d, type: %d , len: %d\r\n",
        read_data->handle, read_data->type, read_data->data_len);
    for (uint16_t idx = 0; idx < read_data->data_len; idx++) {
        osal_printk("[ssap client] read cfm cbk[%d] 0x%02x\r\n", idx, read_data->data[idx]);
    }
}

static void ssapc_notification_cbk(uint8_t client_id, uint16_t conn_id, ssapc_handle_value_t *data,
    errcode_t status)
{
    osal_printk("[ssap client] notification cbk client id: %d conn id: %d status: %d\r\n",
        client_id, conn_id, status);
    osal_printk("[ssap client] notification cbk handle: %d, type: %d , len: %d\r\n",
        data->handle, data->type, data->data_len);
    for (uint16_t idx = 0; idx < data->data_len; idx++) {
        osal_printk("[ssap client] notification cbk[%d] 0x%02x\r\n", idx, data->data[idx]);
    }

    GlpRecvPayload(data->data, data->data_len);
}

static void sle_air_mouse_ssapc_cbk_register(void)
{
    g_ssapc_cbk.exchange_info_cb = ssapc_exchange_info_cbk;
    g_ssapc_cbk.find_structure_cb = ssapc_find_structure_cbk;
    g_ssapc_cbk.find_structure_cmp_cb = ssapc_find_structure_cmp_cbk;
    g_ssapc_cbk.ssapc_find_property_cbk = ssapc_find_property_cbk;
    g_ssapc_cbk.write_cfm_cb = ssapc_write_cfm_cbk;
    g_ssapc_cbk.read_cfm_cb = ssapc_read_cfm_cbk;
    g_ssapc_cbk.notification_cb = ssapc_notification_cbk;

    ssapc_register_callbacks(&g_ssapc_cbk);
}

void sle_air_mouse_client_init(void)
{
    sle_air_mouse_client_seek_cbk_register();
    while (g_sle_enable == false) {
        osal_msleep(SLE_UART_TASK_DELAY_MS);
        enable_sle();
    }
    sle_air_mouse_client_connect_cbk_register();

    sle_air_mouse_ssapc_cbk_register();
}