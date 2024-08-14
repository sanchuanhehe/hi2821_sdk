/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: SLE mouse server Source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-08-01, Create file. \n
 */
#include <stdint.h>
#include "securec.h"
#include "errcode.h"
#include "soc_osal.h"
#include "osal_debug.h"
#include "osal_addr.h"
#include "sle_common.h"
#include "sle_errcode.h"
#include "bts_le_gap.h"
#include "sle_connection_manager.h"
#include "sle_device_discovery.h"
#include "sle_ssap_server.h"
#include "sle_mouse_server_adv.h"
#include "sle_mouse_server.h"

#define SLE_ADV_HANDLE_DEFAULT     1
#define USB_MOUSE_TASK_DELAY_MS    2000
#define SLE_MOUSE_SSAP_RPT_HANDLE    2
#define SLE_MOUSE_DEFAULT_SERVER_ID  1
#define SLE_MOUSE_DEFAULT_CONNECT_ID 0

#define APP_UUID_LEN               2
#define UUID_LEN_2                 2
#define BT_INDEX_4                 4
#define BT_INDEX_5                 5
#define BT_INDEX_0                 0
#define HANDLE_NUM                 2
#define HID_ELEMENT_NUM            6
#define SLE_MOUSE_REPORT_LENGTH    4
#define SLE_SRV_ENCODED_REPORT_LEN 8
#define MOUSE_APPEARANCE_LENGTH    3
static uint8_t g_app_uuid[APP_UUID_LEN] = {0x0, 0x0};
static uint8_t g_server_id = 0;
static uint8_t g_mouse_sle_conn_hdl = 0;
static uint32_t g_mouse_sle_pair_status = 0;
static uint8_t g_sle_input_report[SLE_MOUSE_REPORT_LENGTH] = {0};
static uint8_t g_sle_hid_control_point = 1;
static sle_item_handle_t g_service_hdl[HID_ELEMENT_NUM] = {0};
static uint8_t g_cccd[2] = {0x00, 0x0};
static uint8_t g_input_report_descriptor[SLE_SRV_ENCODED_REPORT_LEN] = {0};
/* Hid Information characteristic not defined */
static uint8_t g_sle_hid_group_uuid[HID_ELEMENT_NUM][SLE_UUID_LEN] = {
    /* Human Interface Device service UUID. */
    { 0x37, 0xBE, 0xA8, 0x80, 0xFC, 0x70, 0x11, 0xEA,
      0xB7, 0x20, 0x00, 0x00, 0x00, 0x00, 0x06, 0x0B },
    /* Report characteristic UUID. 输入报告信息 */
    { 0x37, 0xBE, 0xA8, 0x80, 0xFC, 0x70, 0x11, 0xEA,
      0xB7, 0x20, 0x00, 0x00, 0x00, 0x00, 0x10, 0x3C },
    /* CCCD */
    { 0xFB, 0x34, 0x9B, 0x5F, 0x80, 0x00, 0x00, 0x80,
      0x00, 0x10, 0x00, 0x00, 0x02, 0x29, 0x00, 0x00 },
    /* Report Reference characteristic UUID. 报告索引信息 */
    { 0x37, 0xBE, 0xA8, 0x80, 0xFC, 0x70, 0x11, 0xEA,
      0xB7, 0x20, 0x00, 0x00, 0x00, 0x00, 0x10, 0x3B },
    /* Report Map characteristic UUID. 类型和格式描述 */
    { 0x37, 0xBE, 0xA8, 0x80, 0xFC, 0x70, 0x11, 0xEA,
      0xB7, 0x20, 0x00, 0x00, 0x00, 0x00, 0x10, 0x39 },
    /* Hid Control Point characteristic UUID.  工作状态指示 */
    { 0x37, 0xBE, 0xA8, 0x80, 0xFC, 0x70, 0x11, 0xEA,
      0xB7, 0x20, 0x00, 0x00, 0x00, 0x00, 0x10, 0x3A },
};

static uint8_t g_hid_service_property[HID_ELEMENT_NUM] = {
    0,
    SSAP_OPERATE_INDICATION_BIT_READ | SSAP_OPERATE_INDICATION_BIT_NOTIFY,
    SSAP_OPERATE_INDICATION_BIT_READ | SSAP_OPERATE_INDICATION_BIT_WRITE_NO_RSP | SSAP_OPERATE_INDICATION_BIT_WRITE,
    SSAP_OPERATE_INDICATION_BIT_READ | SSAP_OPERATE_INDICATION_BIT_WRITE,
    SSAP_OPERATE_INDICATION_BIT_READ | SSAP_OPERATE_INDICATION_BIT_NOTIFY,
    SSAP_OPERATE_INDICATION_BIT_WRITE_NO_RSP,
};

typedef enum {
    SLE_UUID_INDEX0, // hid service
    SLE_UUID_INDEX1, // input report
    SLE_UUID_INDEX2, // CCCD
    SLE_UUID_INDEX3, // Report Reference
    SLE_UUID_INDEX4, // Report Map
    SLE_UUID_INDEX5, // Hid Control Point
} sle_uuid_index_t;

static uint8_t g_sle_report_map_datas[] = {
    0x00,                       /* type indicate */
    0x05, 0x01,                 /* Usage Page (Generic Desktop)             */
    0x09, 0x02,                 /* Usage (Mouse)                            */
    0xA1, 0x01,                 /* Collection (Application)                 */
    0x09, 0x01,                 /*  Usage (Pointer)                         */
    0xA1, 0x00,                 /*  Collection (Physical)                   */
    0x85, 0x01,                 /*   Report ID  */
    0x05, 0x09,                 /*      Usage Page (Buttons)                */
    0x19, 0x01,                 /*      Usage Minimum (01)                  */
    0x29, 0x03,                 /*      Usage Maximum (03)                  */
    0x15, 0x00,                 /*      Logical Minimum (0)                 */
    0x25, 0x01,                 /*      Logical Maximum (1)                 */
    0x95, 0x03,                 /*      Report Count (3)                    */
    0x75, 0x01,                 /*      Report Size (1)                     */
    0x81, 0x02,                 /*      Input (Data, Variable, Absolute)    */
    0x95, 0x01,                 /*      Report Count (1)                    */
    0x75, 0x05,                 /*      Report Size (5)                     */
    0x81, 0x01,                 /*      Input (Constant)    ;5 bit padding  */
    0x05, 0x01,                 /*      Usage Page (Generic Desktop)        */
    0x09, 0x30,                 /*      Usage (X)                           */
    0x09, 0x31,                 /*      Usage (Y)                           */
    0x16, 0x01, 0xF8,           /*      Logical Minimum (-2047)              */
    0x26, 0xFF, 0x07,           /*      Logical Maximum (2047)               */
    0x75, 0x0C,                 /*      Report Size (12)                     */
    0x95, 0x02,                 /*      Report Count (2)                    */
    0x81, 0x06,                 /*      Input (Data, Variable, Relative)    */
    0x05, 0x01,                 /*      Usage Page (Generic Desktop)        */
    0x09, 0x38,                 /*      Usage (Wheel)                       */
    0x15, 0x81,                 /*      Logical Minimum (-127)              */
    0x25, 0x7F,                 /*      Logical Maximum (127)               */
    0x75, 0x08,                 /*      Report Size (8)                     */
    0x95, 0x01,                 /*      Report Count (1)                    */
    0x81, 0x06,                 /*      Input (Data, Variable, Relative)    */
    0xC0,                       /* End Collection,End Collection            */
    0xC0,                       /* End Collection,End Collection            */
};

static bool g_ssap_passage_supprot = false;
static uint8_t g_sle_mouse_server_conn_state = SLE_ACB_STATE_NONE;
#define DIS_ELEMENT_NUM 4
static uint8_t g_sle_dis_uuid[DIS_ELEMENT_NUM][SLE_UUID_LEN] = {
    /* DIS service UUID. 设备信息管理 */
    { 0x37, 0xBE, 0xA8, 0x80, 0xFC, 0x70, 0x11, 0xEA,
      0xB7, 0x20, 0x00, 0x00, 0x00, 0x00, 0x06, 0x09 },
    /* Device name characteristic UUID 设备名称 */
    { 0x37, 0xBE, 0xA8, 0x80, 0xFC, 0x70, 0x11, 0xEA,
      0xB7, 0x20, 0x00, 0x00, 0x00, 0x00, 0x10, 0x3F},
    /* Device appearance characteristic 设备外观 */
    { 0x37, 0xBE, 0xA8, 0x80, 0xFC, 0x70, 0x11, 0xEA,
      0xB7, 0x20, 0x00, 0x00, 0x00, 0x00, 0x10, 0x40},
      /* Pnp Id characteristic UUID（设备序列号） */
    { 0x37, 0xBE, 0xA8, 0x80, 0xFC, 0x70, 0x11, 0xEA,
      0xB7, 0x20, 0x00, 0x00, 0x00, 0x00, 0x10, 0x2E },
};

static sle_item_handle_t g_dis_service_hdl[DIS_ELEMENT_NUM] = {0};
static uint8_t g_local_device_name[] = { 's', 'l', 'e', '_', 'm', 'o', 'u', 's', 'e'};
static bool g_sle_enable = false;
#define MANUFACTURE_PNP_ID (uint8_t *)"HUAWEI-103F-12D1-0001"
#define MANUFACTURE_PNP_ID_LENGTH 21
typedef enum {
    SLE_DIS_INDEX0, // dis service
    SLE_DIS_INDEX1, // name
    SLE_DIS_INDEX2, // appearance
    SLE_DIS_INDEX3, // pnp id
} sle_dis_index_t;

errcode_t get_g_sle_mouse_pair_state(uint32_t *pair_state)
{
    *pair_state = g_mouse_sle_pair_status;
    return ERRCODE_SLE_SUCCESS;
}

errcode_t get_g_sle_mouse_server_conn_state(uint8_t *conn_state)
{
    *conn_state = g_sle_mouse_server_conn_state;
    return ERRCODE_SLE_SUCCESS;
}

errcode_t get_g_read_ssap_support(bool *param)
{
    *param = g_ssap_passage_supprot;
    return ERRCODE_SLE_SUCCESS;
}

static void ssaps_read_request_cbk(uint8_t server_id, uint16_t conn_id, ssaps_req_read_cb_t *read_cb_para,
    errcode_t status)
{
    osal_printk("[uuid server] ssaps read request cbk server_id:%x, conn_id:%x, handle:%x, status:%x\r\n",
        server_id, conn_id, read_cb_para->handle, status);
}

static void ssaps_write_request_cbk(uint8_t server_id, uint16_t conn_id, ssaps_req_write_cb_t *write_cb_para,
    errcode_t status)
{
    g_ssap_passage_supprot = true;
    osal_printk("[uuid server] ssaps write request cbk server_id:%x, conn_id:%x, handle:%x, status:%x length:%x\r\n",
        server_id, conn_id, write_cb_para->handle, status, write_cb_para->length);
}

static void ssaps_mtu_changed_cbk(uint8_t server_id, uint16_t conn_id,  ssap_exchange_info_t *mtu_size,
    errcode_t status)
{
    osal_printk("[uuid server] ssaps write request cbk server_id:%x, conn_id:%x, mtu_size:%x, status:%x\r\n",
        server_id, conn_id, mtu_size->mtu_size, status);
}

static void ssaps_start_service_cbk(uint8_t server_id, uint16_t handle, errcode_t status)
{
    osal_printk("[uuid server] start service cbk server_id:%x, handle:%x, status:%x\r\n",
        server_id, handle, status);
}

static void sle_connect_param_update_cbk(uint16_t conn_id, errcode_t status,
    const sle_connection_param_update_evt_t *param)
{
    osal_printk("%s connect up back conn_id:0x%02x, interval:0x%x, latency:0x%x, supervision:0x%x\r\n",
        SLE_MOUSE_DONGLE_SERVER_LOG, conn_id, param->interval, param->latency, param->supervision);
    osal_printk("[uuid server] sle_connect_param_update_cbk:0x%x\r\n", status);
}

static void sle_connect_state_changed_cbk(uint16_t conn_id, const sle_addr_t *addr,
                                          sle_acb_state_t conn_state, sle_pair_state_t pair_state,
                                          sle_disc_reason_t disc_reason)
{
    osal_printk("%s connect state changed conn_id:0x%02x, conn_state:0x%x, pair_state:0x%x, disc_reason:0x%x\r\n",
                SLE_MOUSE_DONGLE_SERVER_LOG, conn_id, conn_state, pair_state, disc_reason);
    osal_printk("remote addr:");
    for (uint8_t i = 0; i < SLE_ADDR_LEN; i++) {
        osal_printk("%02x ", addr->addr[i]);
    }
    osal_printk("\r\n");
    sle_connection_param_update_t con_param = {0};
    con_param.conn_id = conn_id;
    con_param.interval_max = 100; // 100个slot  每个0.125ms
    con_param.interval_min = 100; // 100个slot  每个0.125ms
    con_param.max_latency = 0;
    con_param.supervision_timeout = 500; // 设置连接延迟500ms
    if (conn_state == SLE_ACB_STATE_CONNECTED) {
        sle_update_connect_param(&con_param);
    } else if (conn_state == SLE_ACB_STATE_DISCONNECTED) {
        g_ssap_passage_supprot = false;
        sle_start_announce(SLE_ADV_HANDLE_DEFAULT);
    }
    g_sle_mouse_server_conn_state = conn_state;
    g_mouse_sle_conn_hdl = conn_id;
}

static void sle_pair_complete_cbk(uint16_t conn_id, const sle_addr_t *addr, errcode_t status)
{
    osal_printk("[uuid server] pair complete conn_id:%02x, status:%x\r\n",
        conn_id, status);
    osal_printk("[uuid server] pair complete addr:%02x:**:**:**:%02x:%02x\r\n",
        addr->addr[BT_INDEX_0], addr->addr[BT_INDEX_4], addr->addr[BT_INDEX_5]);
    g_mouse_sle_pair_status = status;
}

static void sle_conn_register_cbks(void)
{
    sle_connection_callbacks_t conn_cbks = { 0 };
    conn_cbks.connect_state_changed_cb = sle_connect_state_changed_cbk;
    conn_cbks.pair_complete_cb = sle_pair_complete_cbk;
    conn_cbks.connect_param_update_cb = sle_connect_param_update_cbk;
    sle_connection_register_callbacks(&conn_cbks);
}

static void sle_mouse_ssaps_register_cbks(void)
{
    ssaps_callbacks_t ssaps_cbk = {0};
    ssaps_cbk.start_service_cb = ssaps_start_service_cbk;
    ssaps_cbk.mtu_changed_cb = ssaps_mtu_changed_cbk;
    ssaps_cbk.read_request_cb = ssaps_read_request_cbk;
    ssaps_cbk.write_request_cb = ssaps_write_request_cbk;
    ssaps_register_callbacks(&ssaps_cbk);
}

static uint8_t sle_get_server_id(void)
{
    return g_server_id;
}

static errcode_t sle_register_server(void)
{
    // register server
    errcode_t ret;
    sle_uuid_t app_uuid = {0};
    app_uuid.len = sizeof(g_app_uuid);
    if (memcpy_s(app_uuid.uuid, app_uuid.len, g_app_uuid, sizeof(g_app_uuid)) != EOK) {
        return ERRCODE_SLE_MEMCPY_FAIL;
    }
    ret = ssaps_register_server(&app_uuid, &g_server_id);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("[uuid server] sle reg server fail, ret:%x\r\n", ret);
        return ERRCODE_SLE_FAIL;
    }
    return ERRCODE_SLE_SUCCESS;
}

static errcode_t sle_sample_set_uuid(uint8_t *uuid, sle_uuid_t *service_uuid)
{
    if (memcpy_s(service_uuid->uuid, SLE_UUID_LEN, uuid, SLE_UUID_LEN) != EOK) {
        osal_printk("sle mouse hid set uuid fail\r\n");
        return ERRCODE_SLE_MEMCPY_FAIL;
    }
    service_uuid->len = SLE_UUID_LEN;
    return ERRCODE_SLE_SUCCESS;
}

static errcode_t sle_mouse_service_add(void)
{
    errcode_t ret;
    sle_uuid_t service_uuid = {0};
    ret = sle_sample_set_uuid(g_sle_hid_group_uuid[SLE_UUID_INDEX0], &service_uuid);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("[uuid server] sle mouse uuid set fail, ret:%x\r\n", ret);
        return ERRCODE_SLE_FAIL;
    }

    ret = ssaps_add_service_sync(sle_get_server_id(), &service_uuid, true, &g_service_hdl[SLE_UUID_INDEX0].handle_out);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("[uuid server] sle mouse add service fail, ret:%x\r\n", ret);
        return ERRCODE_SLE_FAIL;
    }
    g_service_hdl[SLE_UUID_INDEX1].handle_in = g_service_hdl[SLE_UUID_INDEX0].handle_out;
    g_service_hdl[SLE_UUID_INDEX3].handle_in = g_service_hdl[SLE_UUID_INDEX0].handle_out;
    g_service_hdl[SLE_UUID_INDEX4].handle_in = g_service_hdl[SLE_UUID_INDEX0].handle_out;
    g_service_hdl[SLE_UUID_INDEX5].handle_in = g_service_hdl[SLE_UUID_INDEX0].handle_out;
    return ERRCODE_SLE_SUCCESS;
}

static errcode_t sle_sample_add_descriptor_interface(uint32_t properties, uint16_t service_handle,
    uint16_t property_handle, uint16_t len, uint8_t *data)
{
    if (data == NULL) {
        osal_printk("sle sample add descriptor interface param is NULL\r\n");
        return ERRCODE_SLE_FAIL;
    }
    ssaps_desc_info_t descriptor = {0};
    descriptor.permissions = SSAP_PERMISSION_READ | SSAP_PERMISSION_WRITE;
    descriptor.operate_indication = properties;
    descriptor.type = SSAP_DESCRIPTOR_CLIENT_CONFIGURATION;
    descriptor.value_len = len;
    descriptor.value = data;
    return ssaps_add_descriptor_sync(sle_get_server_id(), service_handle, property_handle, &descriptor);
}

static errcode_t sle_sample_add_property_interface(uint32_t properties, uint8_t *uuid, uint16_t len, uint8_t *data,
    sle_item_handle_t* service_hdl)
{
    if ((data == NULL) || (service_hdl == NULL)) {
        osal_printk("sle sample add property interface param is NULL\r\n");
        return ERRCODE_SLE_FAIL;
    }
    ssaps_property_info_t property = {0};
    errcode_t ret = sle_sample_set_uuid(uuid, &property.uuid);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("[uuid server] sle mouse uuid set fail, ret:%x\r\n", ret);
        return ERRCODE_SLE_FAIL;
    }
    property.permissions = SSAP_PERMISSION_READ | SSAP_PERMISSION_WRITE;
    property.operate_indication = properties;
    property.value_len = len;
    property.value = data;
    return ssaps_add_property_sync(sle_get_server_id(), service_hdl->handle_in, &property, &service_hdl->handle_out);
}

static errcode_t sle_mouse_property_and_descriptor_add(void)
{
    errcode_t ret = ERRCODE_SLE_SUCCESS;
    ret = sle_sample_add_property_interface(g_hid_service_property[SLE_UUID_INDEX1],
        g_sle_hid_group_uuid[SLE_UUID_INDEX1], SLE_MOUSE_REPORT_LENGTH, g_sle_input_report,
        &g_service_hdl[SLE_UUID_INDEX1]);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("[uuid server] sle mouse add report fail, ret:%x, indet:%x\r\n", ret, SLE_UUID_INDEX1);
        return ERRCODE_SLE_FAIL;
    }
    osal_printk("[uuid server] sle mouse add report, proterty hdl:%x\r\n",
        g_service_hdl[SLE_UUID_INDEX1].handle_out);

    ret = sle_sample_add_descriptor_interface(g_hid_service_property[SLE_UUID_INDEX2],
        g_service_hdl[SLE_UUID_INDEX0].handle_out, g_service_hdl[SLE_UUID_INDEX1].handle_out, sizeof(g_cccd), g_cccd);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("[uuid server] sle mouse add cccd fail, ret:%x, indet:%x\r\n", ret, SLE_UUID_INDEX2);
        return ERRCODE_SLE_FAIL;
    }

    g_input_report_descriptor[0] = 0x1;   // [1] : report id
    g_input_report_descriptor[1] = 0x1;   // [1] : input
    g_input_report_descriptor[2] = g_service_hdl[SLE_UUID_INDEX1].handle_out; // [2] rpt handle low
    g_input_report_descriptor[3] = 0;     // [3] rpt handle high
    ret = sle_sample_add_property_interface(g_hid_service_property[SLE_UUID_INDEX3],
        g_sle_hid_group_uuid[SLE_UUID_INDEX3], SLE_SRV_ENCODED_REPORT_LEN, g_input_report_descriptor,
        &g_service_hdl[SLE_UUID_INDEX3]);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("[uuid server] sle mouse add report ref fail, ret:%x, indet:%x\r\n", ret, SLE_UUID_INDEX3);
        return ERRCODE_SLE_FAIL;
    }

    ret = sle_sample_add_property_interface(g_hid_service_property[SLE_UUID_INDEX4],
        g_sle_hid_group_uuid[SLE_UUID_INDEX4], sizeof(g_sle_report_map_datas), g_sle_report_map_datas,
        &g_service_hdl[SLE_UUID_INDEX4]);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("[uuid server] sle mouse add report map ref fail, ret:%x, indet:%x\r\n", ret,
            SLE_UUID_INDEX4);
        return ERRCODE_SLE_FAIL;
    }

    ret = sle_sample_add_property_interface(g_hid_service_property[SLE_UUID_INDEX5],
        g_sle_hid_group_uuid[SLE_UUID_INDEX5], sizeof(uint8_t), &g_sle_hid_control_point,
        &g_service_hdl[SLE_UUID_INDEX5]);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("[uuid server] sle mouse add hid ctrl point fail, ret:%x, indet:%x\r\n", ret,
            SLE_UUID_INDEX5);
        return ERRCODE_SLE_FAIL;
    }
    return ERRCODE_SLE_SUCCESS;
}

static errcode_t sle_mouse_server_add(void)
{
    errcode_t ret;
    ret = sle_register_server();
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("[uuid server] sle pen reg server fail, ret:%x\r\n", ret);
        return ERRCODE_SLE_FAIL;
    }

    if (sle_mouse_service_add() != ERRCODE_SLE_SUCCESS) {
        ssaps_unregister_server(sle_get_server_id());
        return ERRCODE_SLE_FAIL;
    }

    if (sle_mouse_property_and_descriptor_add() != ERRCODE_SLE_SUCCESS) {
        ssaps_unregister_server(sle_get_server_id());
        return ERRCODE_SLE_FAIL;
    }
    osal_printk("[uuid server] sle pen add service, server_id:%x, service_handle:%x\r\n",
        g_server_id, g_service_hdl[SLE_UUID_INDEX0].handle_out);
    ret = ssaps_start_service(g_server_id, g_service_hdl[SLE_UUID_INDEX0].handle_out);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("[uuid server] sle pen start service fail, ret:%x\r\n", ret);
        return ERRCODE_SLE_FAIL;
    }
    osal_printk("[uuid server] sle uuid add service out\r\n");
    return ERRCODE_SLE_SUCCESS;
}

static errcode_t sle_dis_service_add(void)
{
    errcode_t ret;
    sle_uuid_t service_uuid = {0};
    ret = sle_sample_set_uuid(g_sle_dis_uuid[SLE_DIS_INDEX0], &service_uuid);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("[uuid server] sle mouse uuid set fail, ret:%x\r\n", ret);
        return ERRCODE_SLE_FAIL;
    }

    ret = ssaps_add_service_sync(sle_get_server_id(), &service_uuid, 1, &g_dis_service_hdl[SLE_DIS_INDEX0].handle_out);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("[uuid server] sle mouse add service fail, ret:%x\r\n", ret);
        return ERRCODE_SLE_FAIL;
    }
    g_dis_service_hdl[SLE_DIS_INDEX1].handle_in = g_dis_service_hdl[SLE_DIS_INDEX0].handle_out;
    g_dis_service_hdl[SLE_DIS_INDEX2].handle_in = g_dis_service_hdl[SLE_DIS_INDEX0].handle_out;
    g_dis_service_hdl[SLE_DIS_INDEX3].handle_in = g_dis_service_hdl[SLE_DIS_INDEX0].handle_out;
    return ERRCODE_SLE_SUCCESS;
}

static errcode_t sle_dis_property_and_descriptor_add(void)
{
    errcode_t ret = ERRCODE_SLE_SUCCESS;
    uint32_t properties = SSAP_OPERATE_INDICATION_BIT_READ;
    ret = sle_sample_add_property_interface(properties, g_sle_dis_uuid[SLE_DIS_INDEX1], sizeof(g_local_device_name),
        g_local_device_name, &g_dis_service_hdl[SLE_DIS_INDEX1]);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("[dis server] sle dis add name fail, ret:%x, indet:%x\r\n", ret, SLE_DIS_INDEX1);
        return ERRCODE_SLE_FAIL;
    }

    uint8_t appearance_value[MOUSE_APPEARANCE_LENGTH] = {0x00, 0x05, 0x02}; // mouse appearance 0x00, 0x05, 0x02

    ret = sle_sample_add_property_interface(properties, g_sle_dis_uuid[SLE_DIS_INDEX2], sizeof(appearance_value),
        appearance_value, &g_dis_service_hdl[SLE_DIS_INDEX2]);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("[uuid server] sle dis add appearance fail, ret:%x, indet:%x\r\n", ret, SLE_DIS_INDEX2);
        return ERRCODE_SLE_FAIL;
    }

    uint8_t *facturer_pnp_id = (uint8_t*)MANUFACTURE_PNP_ID;

    ret = sle_sample_add_property_interface(properties, g_sle_dis_uuid[SLE_DIS_INDEX3], MANUFACTURE_PNP_ID_LENGTH,
        facturer_pnp_id, &g_dis_service_hdl[SLE_DIS_INDEX3]);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("[uuid server] sle dis add appearance fail, ret:%x, indet:%x\r\n", ret, SLE_DIS_INDEX2);
        return ERRCODE_SLE_FAIL;
    }
    return ERRCODE_SLE_SUCCESS;
}

errcode_t sle_sample_dis_server_add(void)
{
    errcode_t ret = ERRCODE_SLE_SUCCESS;
    ret = sle_set_local_name(g_local_device_name, sizeof(g_local_device_name));
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("[dis server] set local name fail, ret:%x\r\n", ret);
        return ERRCODE_SLE_FAIL;
    }
    if (sle_dis_service_add() != ERRCODE_SLE_SUCCESS) {
        ssaps_unregister_server(sle_get_server_id());
        return ERRCODE_SLE_FAIL;
    }

    if (sle_dis_property_and_descriptor_add() != ERRCODE_SLE_SUCCESS) {
        ssaps_unregister_server(sle_get_server_id());
        return ERRCODE_SLE_FAIL;
    }
    osal_printk("[dis server] sle dis add service, server_id:%x, service_handle:%x\r\n",
        sle_get_server_id(), g_dis_service_hdl[SLE_DIS_INDEX0].handle_out);
    ret = ssaps_start_service(sle_get_server_id(), g_dis_service_hdl[SLE_DIS_INDEX0].handle_out);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("[dis server] sle dis start service fail, ret:%x\r\n", ret);
        return ERRCODE_SLE_FAIL;
    }
    osal_printk("[dis server] sle add dis service out\r\n");
    return ERRCODE_SLE_SUCCESS;
}

/* device向host发送数据：input report */
errcode_t sle_hid_mouse_server_send_input_report(ssap_mouse_key_t *mouse_data)
{
    ssaps_ntf_ind_t param = { 0 };
    param.handle = SLE_MOUSE_SSAP_RPT_HANDLE;
    param.value_len = sizeof(ssap_mouse_key_t);
    param.value = osal_vmalloc(sizeof(ssap_mouse_key_t));
    if (param.value == NULL) {
        osal_printk("send input report new fail\r\n");
        return ERRCODE_SLE_MALLOC_FAIL;
    }
    if (memcpy_s(param.value, param.value_len, mouse_data, sizeof(ssap_mouse_key_t)) != EOK) {
        osal_printk("send input report memcpy fail\r\n");
        osal_vfree(param.value);
        return ERRCODE_SLE_MEMCPY_FAIL;
    }
    ssaps_notify_indicate(SLE_MOUSE_DEFAULT_SERVER_ID, SLE_MOUSE_DEFAULT_CONNECT_ID, &param);
    osal_vfree(param.value);
    return ERRCODE_SLE_SUCCESS;
}

static void ble_enable_cbk(errcode_t status)
{
    osal_printk("enable status:%d\r\n", status);
    g_sle_enable = true;
}

static void bt_core_enable_cb_register(void)
{
    sle_announce_seek_callbacks_t seek_cbks = { 0 };
    seek_cbks.sle_enable_cb = ble_enable_cbk;
    if (sle_announce_seek_register_callbacks(&seek_cbks) != ERRCODE_BT_SUCCESS) {
        osal_printk("register sle_enable failed\r\n");
    }
}

errcode_t sle_mouse_server_init(void)
{
    bt_core_enable_cb_register();
    while (g_sle_enable == false) {
        osal_msleep(USB_MOUSE_TASK_DELAY_MS);
        enable_sle();
    }
    sle_conn_register_cbks();
    sle_mouse_ssaps_register_cbks();
    sle_mouse_server_add();
    sle_sample_dis_server_add();
    sle_mouse_server_adv_init();
    osal_printk("%s init ok\r\n", SLE_MOUSE_DONGLE_SERVER_LOG);
    return ERRCODE_SLE_SUCCESS;
}