/**
 * Copyright (c) @CompanyNameMagicTag 2022-2023. All rights reserved. \n
 *
 * Description: BLE Mouse BAS and DIS Service Server SAMPLE. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-08-20, Create file. \n
 */

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "osal_addr.h"
#include "securec.h"
#include "errcode.h"
#include "test_suite_uart.h"
#include "bts_def.h"
#include "bts_gatt_stru.h"
#include "bts_gatt_server.h"
#include "bts_le_gap.h"
#include "ble_hid_mouse_server.h"

#define BAS_ELEMENT_NUM 4
#define SERVICE_HANDLE_ELEMENT_NUM 3
#define BLE_SERVICE_INVALID_HANDLE 0
#define GATT_PERMISSION_REDA 0x1
#define GATT_PERMISSION_WRITE 0x2
#define UUID_SIZE 16

static uint16_t g_service_handle = 0;
static uint16_t g_bas_character_value_handle = 0; /* 特征值句柄，GATT规范里的句柄 */
static uint16_t g_bas_status_value_handle = 0; /* 特征值句柄，GATT规范里的句柄 */

static uint8_t g_bas_level_status_nocharge[] = {
    2, 0xC0, 0x20, 80
};
static uint8_t g_bas_level_status_unknow[] = {
    2, 0x00, 0x00, 81
};

typedef enum {
    BAS_SERVICE_UUID,
    BAS_CHARACTERRISTIC_UUID,
    BAS_CCCD_UUID,
    BAS_STATUS_CHARACTERRISTIC_UUID
} bas_uuid_enum_t;

typedef enum {
    PRIMARY_SERVICE,
    SECONDARY_SERVICE,
    INCLUDED_SERVICE,
    CHARACTERISTIC,
    DESCRIPTOR
} service_gatt_element_type_t;

typedef enum {
    BAS_ID_IDX0,
    BAS_ID_IDX1,
    BAS_ID_IDX2
} basid_enum_t;

#define BLE_SERVICE_INVALID_HANDLE 0
#define SERVICE_HANDLE_ELEMENT_NUM 3

typedef enum {
    SERVICE_TYPE,
    UUID_HANDLE,
    VALUE_HANDLE
} service_handle_group_t;

typedef struct {
    uint16_t service_handle_in;
    uint16_t handle_out;
    uint16_t value_handle_out;
} chara_handle_t;

typedef struct {
    uint16_t service_handle_in;
    uint16_t handle_out;
} descriptor_handle_t;

#define BAS_INIT_LEVEL_VALUE 80

uint8_t bas_init_level = BAS_INIT_LEVEL_VALUE;

uint16_t g_bas_service_handle_map[BAS_ELEMENT_NUM][SERVICE_HANDLE_ELEMENT_NUM] = {
    {PRIMARY_SERVICE, 0, BLE_SERVICE_INVALID_HANDLE},
    {CHARACTERISTIC, 0, 0},
    {DESCRIPTOR, 0, BLE_SERVICE_INVALID_HANDLE},
    {CHARACTERISTIC, 0, 0},
};
static uint8_t g_bas_group_uuid[BAS_ELEMENT_NUM][UUID_SIZE] = {
    /* Battery service UUID. */
    { 0xFB, 0x34, 0x9B, 0x5F, 0x80, 0x00, 0x00, 0x80,
      0x00, 0x10, 0x00, 0x00, 0x0F, 0x18, 0x00, 0x00 },
    /* Battery Level characteristic UUID. */
    { 0xFB, 0x34, 0x9B, 0x5F, 0x80, 0x00, 0x00, 0x80,
      0x00, 0x10, 0x00, 0x00, 0x19, 0x2A, 0x00, 0x00 },
    /* CCC descriptor, 0x2902 */
    { 0xFB, 0x34, 0x9B, 0x5F, 0x80, 0x00, 0x00, 0x80,
      0x00, 0x10, 0x00, 0x00, 0x02, 0x29, 0x00, 0x00 },
    /* Battery Level Status characteristic UUID. */
    { 0xFB, 0x34, 0x9B, 0x5F, 0x80, 0x00, 0x00, 0x80,
      0x00, 0x10, 0x00, 0x00, 0xED, 0x2B, 0x00, 0x00 },
};

uint32_t adpt_gatt_add_service(uint8_t *srvc_uuid, bool is_primary, uint16_t *handle)
{
    bt_uuid_t service_uuid = {0};
    service_uuid.uuid_len = UUID_SIZE;
    if (memcpy_s(service_uuid.uuid, UUID_SIZE, srvc_uuid, UUID_SIZE) != EOK) {
        test_suite_uart_sendf("adpt gatt add service memcpy fail\n");
        return ERRCODE_BT_MEMCPY_FAIL;
    }
    test_suite_uart_sendf("adpt_gatt_add_service \n");
    return gatts_add_service_sync(ble_get_server_id(), &service_uuid, is_primary, handle);
}

uint32_t adpt_gatt_add_characteristic(uint8_t properties, uint8_t *srvc_uuid, uint16_t data_size,
    uint8_t *data, chara_handle_t *handle_gather)
{
    test_suite_uart_sendf("adpt_gatt_add_characteristic \n");
    uint32_t ret = ERRCODE_BT_FAIL;
    if ((data == NULL) || (handle_gather == NULL)) {
        test_suite_uart_sendf("adpt_gatt_add_characteristic param is NULL \r\n");
        return ret;
    }
    gatts_add_character_result_t handle_result = {0};
    handle_result.handle = handle_gather->handle_out;
    handle_result.value_handle = handle_gather->value_handle_out;
    gatts_add_chara_info_t character = {0};
    character.chara_uuid.uuid_len = UUID_SIZE;
    if (memcpy_s(character.chara_uuid.uuid, UUID_SIZE, srvc_uuid, UUID_SIZE) != EOK) {
        test_suite_uart_sendf("Adpt Add Chara uuid memcpy fail \r\n");
        return ERRCODE_BT_MALLOC_FAIL;
    }
    character.permissions = GATT_PERMISSION_REDA | GATT_PERMISSION_WRITE;
    character.properties = properties;
    character.value_len = data_size;
    character.value = data;
    ret = gatts_add_characteristic_sync(ble_get_server_id(), handle_gather->service_handle_in, &character,
        &handle_result);
    handle_gather->handle_out = handle_result.handle;
    handle_gather->value_handle_out = handle_result.value_handle;
    return ret;
}

uint32_t adpt_gatts_add_descriptor(uint8_t *srvc_uuid, uint16_t data_size, uint8_t *data,
    uint8_t permissions, descriptor_handle_t *handle_gather)
{
    if ((data == NULL) || (handle_gather == NULL)) {
        test_suite_uart_sendf("adpt_gatts_add_descriptor param is NULL \r\n");
        return ERRCODE_BT_PARAM_ERR;
    }
    gatts_add_desc_info_t descriptor = {0};
    descriptor.desc_uuid.uuid_len = UUID_SIZE;
    if (memcpy_s(descriptor.desc_uuid.uuid, UUID_SIZE, srvc_uuid, UUID_SIZE) != EOK) {
        test_suite_uart_sendf("Adpt Add descrip uuid memcpy fail \r\n");
    }
    descriptor.permissions = permissions;
    descriptor.value_len = data_size;
    descriptor.value = data;
    return gatts_add_descriptor_sync(ble_get_server_id(), handle_gather->service_handle_in, &descriptor,
        &(handle_gather->handle_out));
}

void profs_bas_server_element_value_init(void)
{
    adpt_gatt_add_service(&g_bas_group_uuid[BAS_SERVICE_UUID][0], 1,
        &g_bas_service_handle_map[BAS_SERVICE_UUID][UUID_HANDLE]);
    test_suite_uart_sendf("bas service handle %x\r\n", g_bas_service_handle_map[BAS_SERVICE_UUID][UUID_HANDLE]);
    chara_handle_t chara_handle_t_gather = {0};
    chara_handle_t_gather.service_handle_in = g_bas_service_handle_map[BAS_SERVICE_UUID][UUID_HANDLE];
    uint8_t bas_properties = GATT_CHARACTER_PROPERTY_BIT_READ | GATT_CHARACTER_PROPERTY_BIT_NOTIFY;
    adpt_gatt_add_characteristic(bas_properties, g_bas_group_uuid[BAS_CHARACTERRISTIC_UUID], sizeof(uint8_t),
        &bas_init_level, &chara_handle_t_gather);
    g_bas_service_handle_map[BAS_CHARACTERRISTIC_UUID][UUID_HANDLE] = chara_handle_t_gather.handle_out;
    g_bas_service_handle_map[BAS_CHARACTERRISTIC_UUID][VALUE_HANDLE] = chara_handle_t_gather.value_handle_out;
    g_bas_character_value_handle = chara_handle_t_gather.value_handle_out;

    descriptor_handle_t descrip_handle_gather = {0};
    uint8_t data_descrip[] = {0x00, 0x00};
    descrip_handle_gather.service_handle_in = g_bas_service_handle_map[BAS_SERVICE_UUID][UUID_HANDLE];
    uint8_t permissions = GATT_PERMISSION_REDA | GATT_PERMISSION_WRITE;
    adpt_gatts_add_descriptor(g_bas_group_uuid[BAS_CCCD_UUID], sizeof(data_descrip), data_descrip, permissions,
        &descrip_handle_gather);

    g_bas_service_handle_map[BAS_CCCD_UUID][UUID_HANDLE] = descrip_handle_gather.handle_out;
    adpt_gatt_add_characteristic(bas_properties, g_bas_group_uuid[BAS_STATUS_CHARACTERRISTIC_UUID],
        sizeof(g_bas_level_status_nocharge), g_bas_level_status_nocharge, &chara_handle_t_gather);
    g_bas_status_value_handle = chara_handle_t_gather.value_handle_out;
    test_suite_uart_sendf("bas star battery service handle = %x\r\n", g_bas_status_value_handle);
    adpt_gatts_add_descriptor(g_bas_group_uuid[BAS_CCCD_UUID], sizeof(data_descrip), data_descrip, permissions,
        &descrip_handle_gather);
    errcode_t ret = ERRCODE_BT_FAIL;

    ret = gatts_start_service(ble_get_server_id(), g_bas_service_handle_map[BAS_SERVICE_UUID][UUID_HANDLE]);
    if (ret == ERRCODE_BT_SUCCESS) {
        test_suite_uart_sendf("Adpt bas start service uuid success\r\n");
    } else {
        test_suite_uart_sendf("Adpt bas start service uuid fail \r\n");
    }
}
/* 服务添加回调 */
static void ble_bas_server_service_add_cbk(uint8_t server_id, bt_uuid_t *uuid, uint16_t handle, errcode_t status)
{
    int8_t i = 0;
    test_suite_uart_sendf("[bas]ServiceAddCallback server: %d srv_handle: %d uuid_len: %d\n",
        server_id, handle, uuid->uuid_len);
    test_suite_uart_sendf("uuid:");
    for (i = 0; i < uuid->uuid_len ; i++) {
        test_suite_uart_sendf("%02x", uuid->uuid[i]);
    }
    test_suite_uart_sendf("\n");
    g_service_handle = handle;
    test_suite_uart_sendf("service handle:%d\n", status);
    test_suite_uart_sendf("status:%d\n", status);
    test_suite_uart_sendf("[bas][INFO]beginning add characters and descriptors\r\n");
}
/* 特征添加回调 */
static void  ble_bas_server_characteristic_add_cbk(uint8_t server_id, bt_uuid_t *uuid, uint16_t service_handle,
    gatts_add_character_result_t *result, errcode_t status)
{
    test_suite_uart_sendf("[bas]CharacteristicAddCallback server: %d srvc_hdl: %d "
        "char_hdl: %d char_val_hdl: %d uuid_len: %d \n",
        server_id, service_handle, result->handle, result->value_handle, uuid->uuid_len);
    test_suite_uart_sendf("uuid:");
    for (int8_t i = 0; i < uuid->uuid_len ; i++) {
        test_suite_uart_sendf("%02x", uuid->uuid[i]);
    }
    test_suite_uart_sendf("\n");
    test_suite_uart_sendf("status:%d\n", status);
}
/* 描述符添加回调 */
static void  ble_bas_server_descriptor_add_cbk(uint8_t server_id, bt_uuid_t *uuid, uint16_t service_handle,
    uint16_t handle, errcode_t status)
{
    test_suite_uart_sendf("[bas]DescriptorAddCallback server: %d srv_hdl: %d desc_hdl: %d "
        "uuid_len:%d\n", server_id, service_handle, handle, uuid->uuid_len);
    test_suite_uart_sendf("uuid:");
    for (int8_t i = 0; i < uuid->uuid_len ; i++) {
        test_suite_uart_sendf("%02x", (uint8_t)uuid->uuid[i]);
    }
    test_suite_uart_sendf("\n");
    test_suite_uart_sendf("status:%d\n", status);
}
/* 开始服务回调 */
static void ble_bas_server_service_start_cbk(uint8_t server_id, uint16_t handle, errcode_t status)
{
    test_suite_uart_sendf("[bas]ServiceStartCallback server: %d srv_hdl: %d status: %d\n",
        server_id, handle, status);
}
static void ble_bas_receive_write_req_cbk(uint8_t server_id, uint16_t conn_id, gatts_req_write_cb_t *write_cb_para,
    errcode_t status)
{
    test_suite_uart_sendf("[bas]ReceiveWriteReqCallback--server_id:%d conn_id:%d\n", server_id, conn_id);
    test_suite_uart_sendf("request_id:%d att_handle:%d offset:%d need_rsp:%d need_authorize:%d is_prep:%d\n",
        write_cb_para->request_id, write_cb_para->handle, write_cb_para->offset, write_cb_para->need_rsp,
        write_cb_para->need_authorize, write_cb_para->is_prep);
    test_suite_uart_sendf("data_len:%d data:\n", write_cb_para->length);
    for (uint8_t i = 0; i < write_cb_para->length; i++) {
        test_suite_uart_sendf("%02x ", write_cb_para->value[i]);
    }
    test_suite_uart_sendf("\n");
    test_suite_uart_sendf("status:%d\n", status);
}
static void ble_bas_receive_read_req_cbk(uint8_t server_id, uint16_t conn_id, gatts_req_read_cb_t *read_cb_para,
    errcode_t status)
{
    test_suite_uart_sendf("[bas]ReceiveReadReq--server_id:%d conn_id:%d\n", server_id, conn_id);
    test_suite_uart_sendf("request_id:%d att_handle:%d offset:%d need_rsp:%d need_authorize:%d is_long:%d\n",
        read_cb_para->request_id, read_cb_para->handle, read_cb_para->offset, read_cb_para->need_rsp,
        read_cb_para->need_authorize, read_cb_para->is_long);
    test_suite_uart_sendf("status:%d\n", status);
}
static void ble_bas_mtu_changed_cbk(uint8_t server_id, uint16_t conn_id, uint16_t mtu_size, errcode_t status)
{
    test_suite_uart_sendf("[bas]MtuChanged--server_id:%d conn_id:%d\n", server_id, conn_id);
    test_suite_uart_sendf("mtusize:%d\n", mtu_size);
    test_suite_uart_sendf("status:%d\n", status);
}
static errcode_t ble_bas_server_register_gatt_callbacks(void)
{
    errcode_t ret = ERRCODE_BT_FAIL;
    gatts_callbacks_t cb = {0};
    cb.add_service_cb = ble_bas_server_service_add_cbk;
    cb.add_characteristic_cb = ble_bas_server_characteristic_add_cbk;
    cb.add_descriptor_cb = ble_bas_server_descriptor_add_cbk;
    cb.start_service_cb = ble_bas_server_service_start_cbk;
    cb.read_request_cb = ble_bas_receive_read_req_cbk;
    cb.write_request_cb = ble_bas_receive_write_req_cbk;
    cb.mtu_changed_cb = ble_bas_mtu_changed_cbk;
    ret = gatts_register_callbacks(&cb);
    if (ret != ERRCODE_BT_SUCCESS) {
        test_suite_uart_sendf("bas register server callback fail\n");
    }
    return ret;
}
void bth_ble_bas_server_init(void)
{
    test_suite_uart_sendf("bth_ble_bas_server_init \n");
    ble_bas_server_register_gatt_callbacks();
    profs_bas_server_element_value_init();
}

errcode_t ble_bas_server_battery_level_notify(void)
{
    bas_init_level--;
    if (bas_init_level == 0) {
        bas_init_level = BAS_INIT_LEVEL_VALUE;
    }
    gatts_ntf_ind_t param = {0};
    param.attr_handle = g_bas_character_value_handle;
    param.value_len = sizeof(uint8_t);
    param.value = &bas_init_level;
    gatts_notify_indicate(ble_get_server_id(), 0, &param);
    return ERRCODE_BT_SUCCESS;
}

errcode_t ble_bas_server_battery_level_status_notify(void)
{
    gatts_ntf_ind_t param = {0};
    param.attr_handle = g_bas_status_value_handle;
    param.value_len = sizeof(g_bas_level_status_unknow);
    param.value = g_bas_level_status_unknow;

    errcode_t ret = ERRCODE_BT_FAIL;
    ret = gatts_notify_indicate(ble_get_server_id(), 0, &param);
    return ret;
}

// DIS
#define DIS_ELEMENT_NUM 8
#define DIS_PNP_CHARACTER_UUID_INDEX 6
static uint8_t g_dis_group_uuid[DIS_ELEMENT_NUM][UUID_SIZE] = {
    /* DIS service UUID. */
    { 0xFB, 0x34, 0x9B, 0x5F, 0x80, 0x00, 0x00, 0x80,
      0x00, 0x10, 0x00, 0x00, 0x0A, 0x18, 0x00, 0x00 },
    /* Model Num characteristic UUID */
    { 0xFB, 0x34, 0x9B, 0x5F, 0x80, 0x00, 0x00, 0x80,
      0x00, 0x10, 0x00, 0x00, 0x24, 0x2A, 0x00, 0x00 },
    /* Firmware Revision characteristic UUID */
    { 0xFB, 0x34, 0x9B, 0x5F, 0x80, 0x00, 0x00, 0x80,
      0x00, 0x10, 0x00, 0x00, 0x26, 0x2A, 0x00, 0x00 },
    /* Hardware Revision characteristic UUID */
    { 0xFB, 0x34, 0x9B, 0x5F, 0x80, 0x00, 0x00, 0x80,
      0x00, 0x10, 0x00, 0x00, 0x27, 0x2A, 0x00, 0x00 },
    /* Software Revision characteristic UUID */
    { 0xFB, 0x34, 0x9B, 0x5F, 0x80, 0x00, 0x00, 0x80,
      0x00, 0x10, 0x00, 0x00, 0x28, 0x2A, 0x00, 0x00 },
    /* Manufact Name characteristic UUID */
    { 0xFB, 0x34, 0x9B, 0x5F, 0x80, 0x00, 0x00, 0x80,
      0x00, 0x10, 0x00, 0x00, 0x29, 0x2A, 0x00, 0x00 },
    /* Pnp Id characteristic UUID */
    { 0xFB, 0x34, 0x9B, 0x5F, 0x80, 0x00, 0x00, 0x80,
      0x00, 0x10, 0x00, 0x00, 0x50, 0x2A, 0x00, 0x00 },
    /* Serial Num characteristic UUID */
    { 0xFB, 0x34, 0x9B, 0x5F, 0x80, 0x00, 0x00, 0x80,
      0x00, 0x10, 0x00, 0x00, 0x25, 0x2A, 0x00, 0x00 },
};

#define BLE_DIS_PNP_ID_LEN 7
static uint8_t encoded_pnp_id[BLE_DIS_PNP_ID_LEN] = {
    0x1,
    0x26, 0xCD,
    0xA5, 0x10,
    0x01, 0x00,
};

void ble_pro_dis_server_init(void)
{
    test_suite_uart_sendf("[ACore] ble_pro_dis_server_init beginning add service\r\n");
    adpt_gatt_add_service(&g_dis_group_uuid[0][0], 1, &g_service_handle);
    chara_handle_t chara_handle_t_gather = {0};
    chara_handle_t_gather.service_handle_in = g_service_handle;
    uint8_t bas_properties = GATT_CHARACTER_PROPERTY_BIT_READ;
    adpt_gatt_add_characteristic(bas_properties, &g_dis_group_uuid[DIS_PNP_CHARACTER_UUID_INDEX][0], BLE_DIS_PNP_ID_LEN,
        &encoded_pnp_id[0], &chara_handle_t_gather);
    errcode_t ret = ERRCODE_BT_FAIL;
    ret = gatts_start_service(ble_get_server_id(), g_service_handle);
    if (ret == ERRCODE_BT_SUCCESS) {
        test_suite_uart_sendf("Adpt bas start service uuid success\r\n");
    } else {
        test_suite_uart_sendf("Adpt bas start service uuid fail \r\n");
    }
}
