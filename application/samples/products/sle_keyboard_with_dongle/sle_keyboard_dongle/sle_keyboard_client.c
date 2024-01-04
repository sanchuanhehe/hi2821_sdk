/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: SLE keyboard sample of client. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-04-03, Create file. \n
 */
#include "securec.h"
#include "string.h"
#include "common_def.h"
#include "cmsis_os2.h"
#include "osal_debug.h"
#include "osal_task.h"
#include "bts_le_gap.h"
#include "sle_device_discovery.h"
#include "sle_connection_manager.h"
#include "sle_ssap_client.h"
#include "sle_keyboard_client.h"

#define SLE_MTU_SIZE_DEFAULT                        300
#define SLE_SEEK_INTERVAL_DEFAULT                   100
#define SLE_SEEK_WINDOW_DEFAULT                     100
#define UUID_16BIT_LEN                              2
#define UUID_128BIT_LEN                             16
#define SLE_KEYBOARD_TASK_DELAY_MS                  1000
#define SLE_KEYBOARD_WAIT_SLE_CORE_READY_MS         5000
#define SLE_KEYBOARD_WAIT_SLE_ENABLE_MS             2000
#ifndef SLE_KEYBOARD_SERVER_NAME
#define SLE_KEYBOARD_SERVER_NAME                    "sle_keyboard_server"
#endif
#define SLE_KEYBOARD_DONGLE_LOG                     "[sle keyboard dongle]"
#define ADDR_INDEX_0                                0
#define ADDR_INDEX_4                                4
#define ADDR_INDEX_5                                5

static ssapc_find_service_result_t g_sle_keyboard_find_service_result = { 0 };
static sle_announce_seek_callbacks_t g_sle_keyboard_seek_cbk = { 0 };
static sle_connection_callbacks_t g_sle_keyboard_connect_cbk = { 0 };
static ssapc_callbacks_t g_sle_keyboard_ssapc_cbk = { 0 };
static sle_addr_t g_sle_keyboard_remote_addr = { 0 };
static ssapc_write_param_t g_sle_keyboard_send_param = { 0 };
static uint16_t g_sle_keyboard_conn_id = 0;

uint16_t get_sle_keyboard_conn_id(void)
{
    return g_sle_keyboard_conn_id;
}

ssapc_write_param_t get_sle_keyboard_send_param(void)
{
    return g_sle_keyboard_send_param;
}

void sle_keyboard_start_scan(void)
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
    osal_mdelay(SLE_KEYBOARD_TASK_DELAY_MS);
}

static void sle_keyboard_client_sample_sle_enable_cbk(errcode_t status)
{
    if (status != 0) {
        osal_printk("%s sle_keyboard_client_sample_sle_enable_cbk,status error\r\n", SLE_KEYBOARD_DONGLE_LOG);
    } else {
        osal_printk("%s enter callback of sle enable,start scan!\r\n", SLE_KEYBOARD_DONGLE_LOG);
        sle_keyboard_start_scan();
    }
}

static void sle_keyboard_client_sample_seek_enable_cbk(errcode_t status)
{
    if (status != 0) {
        osal_printk("%s sle_keyboard_client_sample_seek_enable_cbk,status error\r\n", SLE_KEYBOARD_DONGLE_LOG);
    }
}

static void sle_keyboard_client_sample_seek_result_info_cbk(sle_seek_result_info_t *seek_result_data)
{
    if (seek_result_data == NULL || seek_result_data->data == NULL) {
        osal_printk("%s status error\r\n", SLE_KEYBOARD_DONGLE_LOG);
        return;
    }
    osal_printk("%s sle keyboard scan data :%s\r\n", SLE_KEYBOARD_DONGLE_LOG, seek_result_data->data);
    if (strstr((const char *)seek_result_data->data, SLE_KEYBOARD_SERVER_NAME) != NULL) {
        memcpy_s(&g_sle_keyboard_remote_addr, sizeof(sle_addr_t), &seek_result_data->addr, sizeof(sle_addr_t));
        sle_stop_seek();
        osal_mdelay(SLE_KEYBOARD_TASK_DELAY_MS);
    }
}

static void sle_keyboard_client_sample_seek_disable_cbk(errcode_t status)
{
    if (status != 0) {
        osal_printk("%s sle_keyboard_client_sample_seek_disable_cbk,status error\r\n", SLE_KEYBOARD_DONGLE_LOG);
    } else {
        sle_remove_paired_remote_device(&g_sle_keyboard_remote_addr);
        sle_connect_remote_device(&g_sle_keyboard_remote_addr);
        osal_mdelay(SLE_KEYBOARD_TASK_DELAY_MS);
    }
}

static void sle_keyboard_client_sample_seek_cbk_register(void)
{
    g_sle_keyboard_seek_cbk.sle_enable_cb = sle_keyboard_client_sample_sle_enable_cbk;
    g_sle_keyboard_seek_cbk.seek_enable_cb = sle_keyboard_client_sample_seek_enable_cbk;
    g_sle_keyboard_seek_cbk.seek_result_cb = sle_keyboard_client_sample_seek_result_info_cbk;
    g_sle_keyboard_seek_cbk.seek_disable_cb = sle_keyboard_client_sample_seek_disable_cbk;
    sle_announce_seek_register_callbacks(&g_sle_keyboard_seek_cbk);
}

static void sle_keyboard_client_sample_connect_state_changed_cbk(uint16_t conn_id, const sle_addr_t *addr,
                                                                 sle_acb_state_t conn_state,
                                                                 sle_pair_state_t pair_state,
                                                                 sle_disc_reason_t disc_reason)
{
    unused(addr);
    unused(pair_state);
    osal_printk("%s conn state changed,connect_state:%d, pair_state:%d, disc_reason:%d\r\n", SLE_KEYBOARD_DONGLE_LOG,
                conn_state, pair_state, disc_reason);
    g_sle_keyboard_conn_id = conn_id;
    if (conn_state == SLE_ACB_STATE_CONNECTED) {
        osal_printk("SLE_ACB_STATE_CONNECTED\r\n", SLE_KEYBOARD_DONGLE_LOG);
        if (pair_state == SLE_PAIR_NONE) {
            sle_pair_remote_device(&g_sle_keyboard_remote_addr);
        }
    } else if (conn_state == SLE_ACB_STATE_NONE) {
        osal_printk("%s SLE_ACB_STATE_NONE\r\n", SLE_KEYBOARD_DONGLE_LOG);
    } else if (conn_state == SLE_ACB_STATE_DISCONNECTED) {
        osal_printk("%s SLE_ACB_STATE_DISCONNECTED\r\n", SLE_KEYBOARD_DONGLE_LOG);
        sle_remove_paired_remote_device(&g_sle_keyboard_remote_addr);
        sle_keyboard_start_scan();
    } else {
        osal_printk("%s status error\r\n", SLE_KEYBOARD_DONGLE_LOG);
    }
}

void  sle_keyboard_client_sample_pair_complete_cbk(uint16_t conn_id, const sle_addr_t *addr, errcode_t status)
{
    osal_printk("%s pair complete conn_id:%d, addr:%02x***%02x%02x\n", SLE_KEYBOARD_DONGLE_LOG, conn_id,
                addr->addr[ADDR_INDEX_0], addr->addr[ADDR_INDEX_4], addr->addr[ADDR_INDEX_5]);
    if (status == 0) {
        ssap_exchange_info_t info = {0};
        info.mtu_size = SLE_MTU_SIZE_DEFAULT;
        info.version = 1;
        ssapc_exchange_info_req(1, g_sle_keyboard_conn_id, &info);
    }
}

static void sle_keyboard_client_sample_connect_cbk_register(void)
{
    g_sle_keyboard_connect_cbk.connect_state_changed_cb = sle_keyboard_client_sample_connect_state_changed_cbk;
    g_sle_keyboard_connect_cbk.pair_complete_cb =  sle_keyboard_client_sample_pair_complete_cbk;
    sle_connection_register_callbacks(&g_sle_keyboard_connect_cbk);
}

static void sle_keyboard_client_sample_exchange_info_cbk(uint8_t client_id, uint16_t conn_id,
                                                         ssap_exchange_info_t *param, errcode_t status)
{
    osal_printk("%s exchange_info_cbk,pair complete client id:%d status:%d\r\n", SLE_KEYBOARD_DONGLE_LOG,
                client_id, status);
    osal_printk("%s exchange mtu, mtu size: %d, version: %d.\r\n", SLE_KEYBOARD_DONGLE_LOG,
                param->mtu_size, param->version);
    ssapc_find_structure_param_t find_param = { 0 };
    find_param.type = SSAP_FIND_TYPE_PROPERTY;
    find_param.start_hdl = 1;
    find_param.end_hdl = 0xFFFF;
    ssapc_find_structure(0, conn_id, &find_param);
    osal_mdelay(SLE_KEYBOARD_TASK_DELAY_MS);
}

static void sle_keyboard_client_sample_find_structure_cbk(uint8_t client_id, uint16_t conn_id,
                                                          ssapc_find_service_result_t *service, errcode_t status)
{
    osal_printk("%s find structure cbk client: %d conn_id:%d status: %d \r\n", SLE_KEYBOARD_DONGLE_LOG,
                client_id, conn_id, status);
    osal_printk("%s find structure start_hdl:[0x%02x], end_hdl:[0x%02x], uuid len:%d\r\n", SLE_KEYBOARD_DONGLE_LOG,
                service->start_hdl, service->end_hdl, service->uuid.len);
    g_sle_keyboard_find_service_result.start_hdl = service->start_hdl;
    g_sle_keyboard_find_service_result.end_hdl = service->end_hdl;
    memcpy_s(&g_sle_keyboard_find_service_result.uuid, sizeof(sle_uuid_t), &service->uuid, sizeof(sle_uuid_t));
}

static void sle_keyboard_client_sample_find_property_cbk(uint8_t client_id, uint16_t conn_id,
                                                         ssapc_find_property_result_t *property, errcode_t status)
{
    osal_printk("%s sle_keyboard_client_sample_find_property_cbk, client id: %d, conn id: %d, operate ind: %d, "
                "descriptors count: %d status:%d property->handle %d\r\n", SLE_KEYBOARD_DONGLE_LOG,
                client_id, conn_id, property->operate_indication,
                property->descriptors_count, status, property->handle);
    g_sle_keyboard_send_param.handle = property->handle;
    g_sle_keyboard_send_param.type = SSAP_PROPERTY_TYPE_VALUE;
}

static void sle_keyboard_client_sample_find_structure_cmp_cbk(uint8_t client_id, uint16_t conn_id,
                                                              ssapc_find_structure_result_t *structure_result,
                                                              errcode_t status)
{
    unused(conn_id);
    osal_printk("%s sle_keyboard_client_sample_find_structure_cmp_cbk,client id:%d status:%d type:%d uuid len:%d \r\n",
                SLE_KEYBOARD_DONGLE_LOG, client_id, status, structure_result->type, structure_result->uuid.len);
}

static void sle_keyboard_client_sample_write_cfm_cb(uint8_t client_id, uint16_t conn_id,
                                                    ssapc_write_result_t *write_result, errcode_t status)
{
    osal_printk("%s sle_keyboard_client_sample_write_cb, conn_id:%d client id:%d status:%d handle:%02x type:%02x\r\n",
                SLE_KEYBOARD_DONGLE_LOG, conn_id, client_id, status, write_result->handle, write_result->type);
}

static void sle_keyboard_client_sample_ssapc_cbk_register(ssapc_notification_callback notification_cb,
                                                          ssapc_notification_callback indication_cb)
{
    g_sle_keyboard_ssapc_cbk.exchange_info_cb = sle_keyboard_client_sample_exchange_info_cbk;
    g_sle_keyboard_ssapc_cbk.find_structure_cb = sle_keyboard_client_sample_find_structure_cbk;
    g_sle_keyboard_ssapc_cbk.ssapc_find_property_cbk = sle_keyboard_client_sample_find_property_cbk;
    g_sle_keyboard_ssapc_cbk.find_structure_cmp_cb = sle_keyboard_client_sample_find_structure_cmp_cbk;
    g_sle_keyboard_ssapc_cbk.write_cfm_cb = sle_keyboard_client_sample_write_cfm_cb;
    g_sle_keyboard_ssapc_cbk.notification_cb = notification_cb;
    g_sle_keyboard_ssapc_cbk.indication_cb = indication_cb;
    ssapc_register_callbacks(&g_sle_keyboard_ssapc_cbk);
}

void ble_enable_cb(errcode_t status)
{
    osal_printk("%s enable status: %d\n", SLE_KEYBOARD_DONGLE_LOG, status);
}

void bt_core_enable_cbk_register(void)
{
    errcode_t ret;
    gap_ble_callbacks_t gap_cb = {0};
    gap_cb.ble_enable_cb = ble_enable_cb;
    ret = gap_ble_register_callbacks(&gap_cb);
    if (ret != ERRCODE_BT_SUCCESS) {
        osal_printk("%s register ble_enable_cb failed\r\n", SLE_KEYBOARD_DONGLE_LOG);
    }
}

void sle_keyboard_client_init(ssapc_notification_callback notification_cb, ssapc_indication_callback indication_cb)
{
    osal_mdelay(SLE_KEYBOARD_WAIT_SLE_ENABLE_MS);
    sle_remove_all_pairs();
    bt_core_enable_cbk_register();
    osal_mdelay(SLE_KEYBOARD_WAIT_SLE_CORE_READY_MS);
    enable_sle();
    sle_keyboard_client_sample_seek_cbk_register();
    sle_keyboard_client_sample_connect_cbk_register();
    sle_keyboard_client_sample_ssapc_cbk_register(notification_cb, indication_cb);
}