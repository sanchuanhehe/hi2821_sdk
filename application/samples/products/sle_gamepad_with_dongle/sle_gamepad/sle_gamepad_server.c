/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: SLE GAMEPAD Server Source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-09-28, Create file. \n
 */

#include "securec.h"
#include "common_def.h"
#include "osal_debug.h"
#include "osal_addr.h"
#include "osal_task.h"
#include "sle_errcode.h"
#include "sle_common.h"
#include "sle_connection_manager.h"
#include "sle_device_discovery.h"
#include "sle_gamepad_server_adv.h"
#include "sle_gamepad_server.h"

#define OCTET_BIT_LEN                   8
#define UUID_LEN_2                      2
#define UUID_INDEX                      14
#define BT_INDEX_4                      4
#define BT_INDEX_0                      0
#define UUID_16BIT_LEN                  2
#define UUID_128BIT_LEN                 16
#define SLE_ADV_HANDLE_DEFAULT          1
#define SLE_GAMEPAD_STATE_DISCONNECT    0
#define SLE_GAMEPAD_STATE_CONNECTED     1

/* sle server app uuid */
static uint8_t g_sle_uuid_app_uuid[UUID_LEN_2] = { 0x12, 0x34 };
/* server notify property uuid */
static uint8_t g_sle_property_value[OCTET_BIT_LEN] = { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };
/* sle connect acb handle */
static uint16_t g_sle_conn_handle = 0;
/* sle server handle */
static uint8_t g_server_id = 0;
/* sle service handle */
static uint16_t g_service_handle = 0;
/* sle ntf property handle */
static uint16_t g_property_handle = 0;
/* sle pair acb handle */
static uint16_t g_sle_pair_handle;
/* sle gamepad conn state */
static uint8_t g_sle_gamepad_server_conn_state = SLE_GAMEPAD_STATE_DISCONNECT;

static uint8_t g_sle_gamepad_base[] = { 0x37, 0xBE, 0xA8, 0x80, 0xFC, 0x70, 0x11, 0xEA, \
    0xB7, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

static void encode2byte_little(uint8_t *_ptr, uint16_t data)
{
    *(uint8_t *)((_ptr) + 1) = (uint8_t)((data) >> 0x8);
    *(uint8_t *)(_ptr) = (uint8_t)(data);
}

static void sle_uuid_set_base(sle_uuid_t *out)
{
    if (memcpy_s(out->uuid, SLE_UUID_LEN, g_sle_gamepad_base, SLE_UUID_LEN) != EOK) {
        sample_print("%s sle_uuid_set_base memcpy fail\n", SLE_GAMEPAD_SERVER_LOG);
        out->len = 0;
        return ;
    }
    out->len = UUID_LEN_2;
}

static void sle_uuid_setu2(uint16_t u2, sle_uuid_t *out)
{
    sle_uuid_set_base(out);
    out->len = UUID_LEN_2;
    encode2byte_little(&out->uuid[UUID_INDEX], u2);
}

static void ssaps_mtu_changed_cbk(uint8_t server_id, uint16_t conn_id,  ssap_exchange_info_t *mtu_size,
                                  errcode_t status)
{
    sample_print("%s ssaps ssaps_mtu_changed_cbk callback server_id:%x, conn_id:%x, mtu_size:%x, status:%x\r\n",
                 SLE_GAMEPAD_SERVER_LOG, server_id, conn_id, mtu_size->mtu_size, status);
    if (g_sle_pair_handle == 0) {
        g_sle_pair_handle =  1;
    }
}

static void ssaps_start_service_cbk(uint8_t server_id, uint16_t handle, errcode_t status)
{
    sample_print("%s start service cbk callback server_id:%d, handle:%x, status:%x\r\n", SLE_GAMEPAD_SERVER_LOG,
                 server_id, handle, status);
}

static void ssaps_add_service_cbk(uint8_t server_id, sle_uuid_t *uuid, uint16_t handle, errcode_t status)
{
    unused(uuid);
    sample_print("%s add service cbk callback server_id:%x, handle:%x, status:%x\r\n", SLE_GAMEPAD_SERVER_LOG,
                 server_id, handle, status);
}

static void ssaps_add_property_cbk(uint8_t server_id, sle_uuid_t *uuid, uint16_t service_handle,
    uint16_t handle, errcode_t status)
{
    unused(uuid);
    sample_print("%s add property cbk callback server_id:%x, service_handle:%x,handle:%x, status:%x\r\n",
                 SLE_GAMEPAD_SERVER_LOG, server_id, service_handle, handle, status);
}

static void ssaps_add_descriptor_cbk(uint8_t server_id, sle_uuid_t *uuid, uint16_t service_handle,
                                     uint16_t property_handle, errcode_t status)
{
    unused(uuid);
    sample_print("%s add descriptor cbk callback server_id:%x, service_handle:%x, property_handle:%x, \
                 status:%x\r\n", SLE_GAMEPAD_SERVER_LOG, server_id, service_handle, property_handle, status);
}

static void ssaps_delete_all_service_cbk(uint8_t server_id, errcode_t status)
{
    sample_print("%s delete all service callback server_id:%x, status:%x\r\n", SLE_GAMEPAD_SERVER_LOG,
                 server_id, status);
}

static errcode_t sle_ssaps_register_cbks(ssaps_read_request_callback ssaps_read_callback,
                                         ssaps_write_request_callback ssaps_write_callback)
{
    errcode_t ret;
    ssaps_callbacks_t ssaps_cbk = {0};
    ssaps_cbk.add_service_cb = ssaps_add_service_cbk;
    ssaps_cbk.add_property_cb = ssaps_add_property_cbk;
    ssaps_cbk.add_descriptor_cb = ssaps_add_descriptor_cbk;
    ssaps_cbk.start_service_cb = ssaps_start_service_cbk;
    ssaps_cbk.delete_all_service_cb = ssaps_delete_all_service_cbk;
    ssaps_cbk.mtu_changed_cb = ssaps_mtu_changed_cbk;
    ssaps_cbk.read_request_cb = ssaps_read_callback;
    ssaps_cbk.write_request_cb = ssaps_write_callback;
    ret = ssaps_register_callbacks(&ssaps_cbk);
    if (ret != ERRCODE_SLE_SUCCESS) {
        sample_print("%s sle_ssaps_register_cbks,ssaps_register_callbacks fail :%x\r\n", SLE_GAMEPAD_SERVER_LOG,
                     ret);
        return ret;
    }
    return ERRCODE_SLE_SUCCESS;
}

static errcode_t sle_uuid_server_service_add(void)
{
    errcode_t ret;
    sle_uuid_t service_uuid = {0};
    sle_uuid_setu2(SLE_UUID_SERVER_SERVICE, &service_uuid);
    ret = ssaps_add_service_sync(g_server_id, &service_uuid, 1, &g_service_handle);
    if (ret != ERRCODE_SLE_SUCCESS) {
        sample_print("%s sle uuid add service fail, ret:%x\r\n", SLE_GAMEPAD_SERVER_LOG, ret);
        return ERRCODE_SLE_FAIL;
    }
    return ERRCODE_SLE_SUCCESS;
}

static errcode_t sle_uuid_server_property_add(void)
{
    errcode_t ret;
    ssaps_property_info_t property = {0};
    ssaps_desc_info_t descriptor = {0};
    uint8_t ntf_value[] = { 0x01, 0x02 };

    property.permissions = SLE_UUID_TEST_PROPERTIES;
    property.operate_indication = SLE_UUID_TEST_OPERATION_INDICATION;
    sle_uuid_setu2(SLE_UUID_SERVER_NTF_REPORT, &property.uuid);
    property.value = (uint8_t *)osal_vmalloc(sizeof(g_sle_property_value));
    if (property.value == NULL) {
        return ERRCODE_SLE_FAIL;
    }
    if (memcpy_s(property.value, sizeof(g_sle_property_value), g_sle_property_value,
        sizeof(g_sle_property_value)) != EOK) {
        osal_vfree(property.value);
        return ERRCODE_SLE_FAIL;
    }
    ret = ssaps_add_property_sync(g_server_id, g_service_handle, &property,  &g_property_handle);
    if (ret != ERRCODE_SLE_SUCCESS) {
        sample_print("%s sle gamepad add property fail, ret:%x\r\n", SLE_GAMEPAD_SERVER_LOG, ret);
        osal_vfree(property.value);
        return ERRCODE_SLE_FAIL;
    }
    descriptor.permissions = SLE_UUID_TEST_DESCRIPTOR;
    descriptor.type = SSAP_DESCRIPTOR_CLIENT_CONFIGURATION;
    descriptor.operate_indication = SLE_UUID_TEST_OPERATION_INDICATION;
    descriptor.value = (uint8_t *)osal_vmalloc(sizeof(ntf_value));
    if (descriptor.value == NULL) {
        osal_vfree(property.value);
        return ERRCODE_SLE_FAIL;
    }
    if (memcpy_s(descriptor.value, sizeof(ntf_value), ntf_value, sizeof(ntf_value)) != EOK) {
        osal_vfree(property.value);
        osal_vfree(descriptor.value);
        return ERRCODE_SLE_FAIL;
    }
    ret = ssaps_add_descriptor_sync(g_server_id, g_service_handle, g_property_handle, &descriptor);
    if (ret != ERRCODE_SLE_SUCCESS) {
        sample_print("%s sle gamepad add descriptor fail, ret:%x\r\n", SLE_GAMEPAD_SERVER_LOG, ret);
        osal_vfree(property.value);
        osal_vfree(descriptor.value);
        return ERRCODE_SLE_FAIL;
    }
    osal_vfree(property.value);
    osal_vfree(descriptor.value);
    return ERRCODE_SLE_SUCCESS;
}

static errcode_t sle_gamepad_server_add(void)
{
    errcode_t ret;
    sle_uuid_t app_uuid = { 0 };

    sample_print("%s sle gamepad add service in\r\n", SLE_GAMEPAD_SERVER_LOG);
    app_uuid.len = sizeof(g_sle_uuid_app_uuid);
    if (memcpy_s(app_uuid.uuid, app_uuid.len, g_sle_uuid_app_uuid, sizeof(g_sle_uuid_app_uuid)) != EOK) {
        return ERRCODE_SLE_FAIL;
    }
    ssaps_register_server(&app_uuid, &g_server_id);

    if (sle_uuid_server_service_add() != ERRCODE_SLE_SUCCESS) {
        ssaps_unregister_server(g_server_id);
        return ERRCODE_SLE_FAIL;
    }
    if (sle_uuid_server_property_add() != ERRCODE_SLE_SUCCESS) {
        ssaps_unregister_server(g_server_id);
        return ERRCODE_SLE_FAIL;
    }
    sample_print("%s sle gamepad add service, server_id:%x, service_handle:%x, property_handle:%x\r\n",
                 SLE_GAMEPAD_SERVER_LOG, g_server_id, g_service_handle, g_property_handle);
    ret = ssaps_start_service(g_server_id, g_service_handle);
    if (ret != ERRCODE_SLE_SUCCESS) {
        sample_print("%s sle gamepad add service fail, ret:%x\r\n", SLE_GAMEPAD_SERVER_LOG, ret);
        return ERRCODE_SLE_FAIL;
    }
    sample_print("%s sle gamepad add service out\r\n", SLE_GAMEPAD_SERVER_LOG);
    return ERRCODE_SLE_SUCCESS;
}

/* device通过uuid向host发送数据：report */
errcode_t sle_gamepad_server_send_report_by_uuid(const uint8_t *data, uint8_t len)
{
    errcode_t ret;
    ssaps_ntf_ind_by_uuid_t param = {0};
    param.type = SSAP_PROPERTY_TYPE_VALUE;
    param.start_handle = g_service_handle;
    param.end_handle = g_property_handle;
    param.value_len = len;
    param.value = (uint8_t *)osal_vmalloc(len);
    if (param.value == NULL) {
        sample_print("%s send report new fail\r\n", SLE_GAMEPAD_SERVER_LOG);
        return ERRCODE_SLE_FAIL;
    }
    if (memcpy_s(param.value, param.value_len, data, len) != EOK) {
        sample_print("%s send input report memcpy fail\r\n", SLE_GAMEPAD_SERVER_LOG);
        osal_vfree(param.value);
        return ERRCODE_SLE_FAIL;
    }
    sle_uuid_setu2(SLE_UUID_SERVER_NTF_REPORT, &param.uuid);
    ret = ssaps_notify_indicate_by_uuid(g_server_id, g_sle_conn_handle, &param);
    if (ret != ERRCODE_SLE_SUCCESS) {
        sample_print("%s sle_gamepad_server_send_report_by_uuid,ssaps_notify_indicate_by_uuid fail :%x\r\n",
                     SLE_GAMEPAD_SERVER_LOG, ret);
        osal_vfree(param.value);
        return ret;
    }
    osal_vfree(param.value);
    return ERRCODE_SLE_SUCCESS;
}

/* device通过handle向host发送数据：report */
errcode_t sle_gamepad_server_send_report_by_handle(const uint8_t *data, uint8_t len)
{
    ssaps_ntf_ind_t param = {0};
    errcode_t ret;
    param.handle = g_property_handle;
    param.type = SSAP_PROPERTY_TYPE_VALUE;
    param.value = (uint8_t *)osal_vmalloc(len);
    param.value_len = len;
    if (param.value == NULL) {
        sample_print("%s send report fail,osal_vmalloc fail! \r\n", SLE_GAMEPAD_SERVER_LOG);
        return ERRCODE_SLE_FAIL;
    }
    if (memcpy_s(param.value, param.value_len, data, len) != EOK) {
        sample_print("%s send input report memcpy fail\r\n", SLE_GAMEPAD_SERVER_LOG);
        osal_vfree(param.value);
        return ERRCODE_SLE_FAIL;
    }
    ret = ssaps_notify_indicate(g_server_id, g_sle_conn_handle, &param);
    if (ret != ERRCODE_SLE_SUCCESS) {
        sample_print("%s sle_gamepad_server_send_report_by_handle,ssaps_notify_indicate fail :%x\r\n",
                     SLE_GAMEPAD_SERVER_LOG, ret);
        osal_vfree(param.value);
        return ret;
    }
    osal_vfree(param.value);
    return ERRCODE_SLE_SUCCESS;
}

static void sle_connect_state_changed_cbk(uint16_t conn_id, const sle_addr_t *addr, sle_acb_state_t conn_state,
                                          sle_pair_state_t pair_state, sle_disc_reason_t disc_reason)
{
    sample_print("%s connect state changed callback conn_id:0x%02x, conn_state:0x%x, pair_state:0x%x, \
                 disc_reason:0x%x\r\n", SLE_GAMEPAD_SERVER_LOG,conn_id, conn_state, pair_state, disc_reason);
    sample_print("%s connect state changed callback addr:%02x:**:**:**:%02x:%02x\r\n", SLE_GAMEPAD_SERVER_LOG,
                 addr->addr[BT_INDEX_0], addr->addr[BT_INDEX_4]);
    if (conn_state == SLE_ACB_STATE_CONNECTED) {
        g_sle_conn_handle = conn_id;
        g_sle_gamepad_server_conn_state = SLE_GAMEPAD_STATE_CONNECTED;
    } else if (conn_state == SLE_ACB_STATE_DISCONNECTED) {
        g_sle_conn_handle = 0;
        g_sle_pair_handle = 0;
        g_sle_gamepad_server_conn_state = SLE_GAMEPAD_STATE_DISCONNECT;
        if (sle_start_announce(SLE_ADV_HANDLE_DEFAULT) == ERRCODE_SLE_SUCCESS) {
            g_sle_gamepad_server_conn_state = SLE_GAMEPAD_STATE_CONNECTED;
        }
    }
}

static void sle_pair_complete_cbk(uint16_t conn_id, const sle_addr_t *addr, errcode_t status)
{
    sample_print("%s pair complete conn_id:%02x, status:%x\r\n", SLE_GAMEPAD_SERVER_LOG,
                 conn_id, status);
    sample_print("%s pair complete addr:%02x:**:**:**:%02x:%02x\r\n", SLE_GAMEPAD_SERVER_LOG,
                 addr->addr[BT_INDEX_0], addr->addr[BT_INDEX_4]);
    g_sle_pair_handle = 1;
}

static errcode_t sle_conn_register_cbks(void)
{
    errcode_t ret;
    sle_connection_callbacks_t conn_cbks = {0};
    conn_cbks.connect_state_changed_cb = sle_connect_state_changed_cbk;
    conn_cbks.pair_complete_cb = sle_pair_complete_cbk;
    ret = sle_connection_register_callbacks(&conn_cbks);
    if (ret != ERRCODE_SLE_SUCCESS) {
        sample_print("%s sle_conn_register_cbks,sle_connection_register_callbacks fail :%x\r\n",
                     SLE_GAMEPAD_SERVER_LOG, ret);
        return ret;
    }
    return ERRCODE_SLE_SUCCESS;
}

uint16_t sle_gamepad_client_is_connected(void)
{
    return g_sle_pair_handle;
}

/* 初始化uuid server */
errcode_t sle_gamepad_server_init(ssaps_read_request_callback ssaps_read_callback,
                                  ssaps_write_request_callback ssaps_write_callback)
{
    errcode_t ret;
    ret = sle_gamepad_announce_register_cbks();
    if (ret != ERRCODE_SLE_SUCCESS) {
        sample_print("%s sle_gamepad_server_init,sle_gamepad_announce_register_cbks fail :%x\r\n",
                     SLE_GAMEPAD_SERVER_LOG, ret);
        return ret;
    }
    ret = enable_sle();
    if (ret != ERRCODE_SLE_SUCCESS) {
        sample_print("%s sle_gamepad_server_init,enable_sle fail :%x\r\n", SLE_GAMEPAD_SERVER_LOG, ret);
        return ret;
    }
    ret = sle_conn_register_cbks();
    if (ret != ERRCODE_SLE_SUCCESS) {
        sample_print("%s sle_gamepad_server_init,sle_conn_register_cbks fail :%x\r\n", SLE_GAMEPAD_SERVER_LOG, ret);
        return ret;
    }
    osal_mdelay(SLE_SERVER_INIT_DELAY_MS);
    ret = sle_ssaps_register_cbks(ssaps_read_callback, ssaps_write_callback);
    if (ret != ERRCODE_SLE_SUCCESS) {
        sample_print("%s sle_gamepad_server_init,sle_ssaps_register_cbks fail :%x\r\n", SLE_GAMEPAD_SERVER_LOG, ret);
        return ret;
    }
    ret = sle_gamepad_server_add();
    if (ret != ERRCODE_SLE_SUCCESS) {
        sample_print("%s sle_gamepad_server_init,sle_gamepad_server_add fail :%x\r\n", SLE_GAMEPAD_SERVER_LOG, ret);
        return ret;
    }
    ret = sle_gamepad_server_adv_init();
    if (ret != ERRCODE_SLE_SUCCESS) {
        sample_print("%s sle_gamepad_server_init,sle_gamepad_server_adv_init fail :%x\r\n",
                     SLE_GAMEPAD_SERVER_LOG, ret);
        return ret;
    }
    sample_print("%s init ok\r\n", SLE_GAMEPAD_SERVER_LOG);
    return ERRCODE_SLE_SUCCESS;
}

errcode_t get_g_sle_gamepad_server_conn_state(uint8_t *conn_state)
{
    *conn_state = g_sle_gamepad_server_conn_state;
    return ERRCODE_SLE_SUCCESS;
}