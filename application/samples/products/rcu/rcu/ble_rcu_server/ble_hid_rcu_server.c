/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: BLE RCU HID Service Source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-9-10, Create file. \n
 */

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "osal_addr.h"
#include "securec.h"
#include "errcode.h"
#include "osal_debug.h"
#include "bts_def.h"
#include "bts_gatt_stru.h"
#include "bts_gatt_server.h"
#include "bts_le_gap.h"
#include "ble_hid_rcu_server.h"

/* HID information flag remote wakeup */
#define BLE_HID_INFO_FLAG_REMOTE_WAKE_UP_MSK                0x01
/* HID information flag normally connectable */
#define BLE_HID_INFO_FLAG_NORMALLY_CONNECTABLE_MSK          0x02
/* HID information country code */
#define BLE_HID_INFO_COUNTRY_CODE                           0x00
/* HID spec version 1.11 */
#define BLE_HID_VERSION                                     0x0101
/* HID keyboard input report id */
#define BLE_HID_KEYBOARD_REPORT_ID                          1
/* HID mouse input report id */
#define BLE_HID_MOUSE_REPORT_ID                             4
/* HID consumer input report id */
#define BLE_HID_CONSUMER_REPORT_ID                          3
/* HID power input report id */
#define BLE_HID_POWER_REPORT_ID                             2
/* HID input report type */
#define BLE_REPORT_REFERENCE_REPORT_TYPE_INPUT_REPORT       1
/* HID output report type */
#define BLE_REPORT_REFERENCE_REPORT_TYPE_OUTPUT_REPORT      2
/* HID gatt server id */
#define BLE_HID_SERVER_ID                                   1
/* HID ble connect id */
#define BLE_SINGLE_LINK_CONNECT_ID                          0
/* octets of 16 bits uuid */
#define UUID16_LEN                                          2
/* invalid attribute handle */
#define INVALID_ATT_HDL                                     0
/* invalid server ID */
#define INVALID_SERVER_ID                                   0
/* input count */
#define INPUT_KEYBOARD                                      0
#define INPUT_MOUSE                                         2
#define INPUT_CONSUMER                                      4
#define INPUT_POWER                                         6

#define input(size)                                         (0x80 | (size))
#define output(size)                                        (0x90 | (size))
#define feature(size)                                       (0xb0 | (size))
#define collection(size)                                    (0xa0 | (size))
#define end_collection(size)                                (0xc0 | (size))

/* Global items */
#define usage_page(size)                                    (0x04 | (size))
#define logical_minimum(size)                               (0x14 | (size))
#define logical_maximum(size)                               (0x24 | (size))
#define physical_minimum(size)                              (0x34 | (size))
#define physical_maximum(size)                              (0x44 | (size))
#define uint_exponent(size)                                 (0x54 | (size))
#define uint(size)                                          (0x64 | (size))
#define report_size(size)                                   (0x74 | (size))
#define report_id(size)                                     (0x84 | (size))
#define report_count(size)                                  (0x94 | (size))
#define push(size)                                          (0xa4 | (size))
#define pop(size)                                           (0xb4 | (size))

/* Local items */
#define usage(size)                                         (0x08 | (size))
#define usage_minimum(size)                                 (0x18 | (size))
#define usage_maximum(size)                                 (0x28 | (size))
#define designator_index(size)                              (0x38 | (size))
#define designator_minimum(size)                            (0x48 | (size))
#define designator_maximum(size)                            (0x58 | (size))
#define string_index(size)                                  (0x78 | (size))
#define string_minimum(size)                                (0x88 | (size))
#define string_maximum(size)                                (0x98 | (size))
#define delimiter(size)                                     (0xa8 | (size))

#define uint16_to_byte(n)                                   ((uint8_t)(n)), ((uint8_t)((n) >> 8))

enum {
    /* HID information characteristic properties */
    HID_INFORMATION_PROPERTIES   = GATT_CHARACTER_PROPERTY_BIT_READ,
    /* HID protocol mode characteristic properties */
    HID_PROTOCOL_MODE_PROPERTIES = GATT_CHARACTER_PROPERTY_BIT_READ | GATT_CHARACTER_PROPERTY_BIT_WRITE_NO_RSP,
    /* HID report map characteristic properties */
    HID_REPORT_MAP_PROPERTIES    = GATT_CHARACTER_PROPERTY_BIT_READ,
    /* HID input report characteristic properties */
    HID_INPUT_REPORT_PROPERTIES  = GATT_CHARACTER_PROPERTY_BIT_READ | GATT_CHARACTER_PROPERTY_BIT_NOTIFY |
                                   GATT_CHARACTER_PROPERTY_BIT_WRITE,
    /* HID output report characteristic properties */
    HID_OUTPUT_REPORT_PROPERTIES = GATT_CHARACTER_PROPERTY_BIT_READ | GATT_CHARACTER_PROPERTY_BIT_WRITE |
                                   GATT_CHARACTER_PROPERTY_BIT_WRITE_NO_RSP,
    /* HID control point characteristic properties */
    HID_CONTROL_POINT_PROPERTIES = GATT_CHARACTER_PROPERTY_BIT_WRITE_NO_RSP,
};

static uint16_t g_device_rcu_appearance_value = GAP_BLE_APPEARANCE_TYPE_KEYBOARD;
static uint8_t g_device_rcu_name_value[32] = {'R', 'c', 'u', '6', 'x', '\0'};
static uint8_t const g_device_rcu_name_len = 6;
/* HID information value for test */
static uint8_t g_hid_information_val[] = { uint16_to_byte(BLE_HID_VERSION), BLE_HID_INFO_COUNTRY_CODE,
    BLE_HID_INFO_FLAG_REMOTE_WAKE_UP_MSK | BLE_HID_INFO_FLAG_NORMALLY_CONNECTABLE_MSK };
/* HID control point value for test */
static uint8_t g_control_point_val[] = {0x00, 0x00};
/* HID client characteristic configuration value for test */
static uint8_t g_ccc_val[] = {0x00, 0x00};
/* HID keyboard input report reference value for test  [report id 1, input] */
static uint8_t g_keyboard_report_reference_val_input[] = {BLE_HID_KEYBOARD_REPORT_ID,
                                                          BLE_REPORT_REFERENCE_REPORT_TYPE_INPUT_REPORT};
/* HID keyboard output report reference value for test [report id 1, output] */
static uint8_t g_keyboard_report_reference_val_output[] = {BLE_HID_KEYBOARD_REPORT_ID,
                                                           BLE_REPORT_REFERENCE_REPORT_TYPE_OUTPUT_REPORT};

/* HID mouse input report reference value for test  [report id 4, input] */
static uint8_t g_mouse_report_reference_val_input[] = {BLE_HID_MOUSE_REPORT_ID,
                                                       BLE_REPORT_REFERENCE_REPORT_TYPE_INPUT_REPORT};
/* HID mouse output report reference value for test [report id 4, output] */
static uint8_t g_mouse_report_reference_val_output[] = {BLE_HID_MOUSE_REPORT_ID,
                                                        BLE_REPORT_REFERENCE_REPORT_TYPE_OUTPUT_REPORT};

/* HID consumer input report reference value for test  [report id 3, input] */
static uint8_t g_consumer_report_reference_val_input[] = {BLE_HID_CONSUMER_REPORT_ID,
                                                          BLE_REPORT_REFERENCE_REPORT_TYPE_INPUT_REPORT};
/* HID consumer output report reference value for test [report id 3, output] */
static uint8_t g_consumer_report_reference_val_output[] = {BLE_HID_CONSUMER_REPORT_ID,
                                                           BLE_REPORT_REFERENCE_REPORT_TYPE_OUTPUT_REPORT};

/* HID power input report reference value for test  [report id 2, input] */
static uint8_t g_power_report_reference_val_input[] = {BLE_HID_POWER_REPORT_ID,
                                                       BLE_REPORT_REFERENCE_REPORT_TYPE_INPUT_REPORT};
/* HID power output report reference value for test [report id 2, output] */
static uint8_t g_power_report_reference_val_output[] = {BLE_HID_POWER_REPORT_ID,
                                                        BLE_REPORT_REFERENCE_REPORT_TYPE_OUTPUT_REPORT};

/* HID keyboard input report value  for test
 * input report format:
 * data0 | data1 | data2 | data3 | data4 | data5 | data6
 * E0~E7 | key   | key   | key   | key   | key   | key
 */
static uint8_t g_keyboard_input_report_value[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
/* HID keyboard output report value for test */
static uint8_t g_keyboard_output_report_value[] = {0x00};

/* HID mouse input report value  for test
 * input report format:
 * data0  | data1 | data2 | data3
 * button | x     | y     | wheel
 */
static uint8_t g_mouse_input_report_value[] = {0x00, 0x00, 0x00, 0x00};
/* HID mouse output report value for test */
static uint8_t g_mouse_output_report_value[] = {0x00};

/* HID consumer input report value  for test
 * input report format:
 * data0         | data1
 * comsumer_key0 | comsumer_key1
 */
static uint8_t g_consumer_input_report_value[] = {0x00, 0x00};
/* HID consumer output report value for test */
static uint8_t g_consumer_output_report_value[] = {0x00};

/* HID power input report value  for test
 * input report format:
 * data0         | data1
 * comsumer_key0 | comsumer_key1
 */
static uint8_t g_power_input_report_value[] = {0x00, 0x00};
/* HID power output report value for test */
static uint8_t g_power_output_report_value[] = {0x00};

/* HID protocol mode value for test */
static uint8_t g_protocol_mode_val[] = {0x00, 0x00};
/* HID server app uuid for test */
static uint8_t g_server_app_uuid_for_test[] = {0x00, 0x00};
/* hid input report att handle */
static uint16_t g_hid_input_report_att_hdl = INVALID_ATT_HDL;
/* hid keyboard input report att handle */
static uint16_t g_hid_keyboard_input_report_att_hdl = INVALID_ATT_HDL;
/* hid mouse input report att handle */
static uint16_t g_hid_mouse_input_report_att_hdl = INVALID_ATT_HDL;
/* hid consumer input report att handle */
static uint16_t g_hid_consumer_input_report_att_hdl = INVALID_ATT_HDL;
/* hid power input report att handle */
static uint16_t g_hid_power_input_report_att_hdl = INVALID_ATT_HDL;
/* hid input report count */
static uint8_t g_input_report_count = 0;
/* gatt server ID */
static uint8_t g_server_id = INVALID_SERVER_ID;

/* Hid Report Map (Descriptor) */
static uint8_t g_srv_hid_rcu_report_map[] = {
    usage_page(1),      0x01,
    usage(1),           0x06,
    collection(1),      0x01,
    report_id(1),       0x01,
    usage_page(1),      0x07,
    usage_minimum(1),   0xE0,
    usage_maximum(1),   0xE7,
    logical_minimum(1), 0x00,
    logical_maximum(1), 0x01,
    report_size(1),     0x01,
    report_count(1),    0x08,
    input(1),           0x02,
    report_count(1),    0x01,
    report_size(1),     0x08,
    input(1),           0x01,
    report_count(1),    0x05,
    report_size(1),     0x01,
    usage_page(1),      0x08,
    usage_minimum(1),   0x01,
    usage_maximum(1),   0x05,
    output(1),          0x02,
    report_count(1),    0x01,
    report_size(1),     0x03,
    output(1),          0x01,
    report_count(1),    0x06,
    report_size(1),     0x08,
    logical_minimum(1), 0x00,
    logical_maximum(1), 0x65,
    usage_page(1),      0x07,
    usage_minimum(1),   0x00,
    usage_maximum(1),   0x65,
    input(1),           0x00,
    end_collection(0),

    usage_page(1),      0x0C,
    usage(1),           0x01,
    collection(1),      0x01,
    report_id(1),       0x03,
    logical_minimum(1), 0x00,
    logical_maximum(2), 0xff, 0x1f,
    usage_minimum(1),   0x00,
    usage_maximum(2),   0xff, 0x1f,
    report_size(1),     0x10,
    report_count(1),    0x01,
    input(1),           0x00,
    end_collection(0),

    usage_page(1),      0x01,
    usage(1),           0x09,
    collection(1),      0x01,
    report_id(1),       0x02,
    usage(1),           0x81,
    logical_minimum(1), 0x00,
    logical_maximum(2), 0xff, 0x1f,
    usage_minimum(1),   0x00,
    usage_maximum(2),   0xff, 0x1f,
    report_size(1),     0x10,
    report_count(1),    0x01,
    input(1),           0x00,
    end_collection(0),

    usage_page(1),      0x01,
    usage(1),           0x02,
    collection(1),      0x01,
    report_id(1),       0x04,
    usage(1),           0x01,
    collection(1),      0x00,
    report_count(1),    0x03,
    report_size(1),     0x01,
    usage_page(1),      0x09,
    usage_minimum(1),   0x1,
    usage_maximum(1),   0x3,
    logical_minimum(1), 0x00,
    logical_maximum(1), 0x01,
    input(1),           0x02,
    report_count(1),    0x01,
    report_size(1),     0x05,
    input(1),           0x01,
    report_count(1),    0x03,
    report_size(1),     0x08,
    usage_page(1),      0x01,
    usage(1),           0x30,
    usage(1),           0x31,
    usage(1),           0x38,
    logical_minimum(1), 0x81,
    logical_maximum(1), 0x7f,
    input(1),           0x06,
    end_collection(0),
    end_collection(0),
};

/* 将uint16的uuid数字转化为bt_uuid_t */
static void bts_data_to_uuid_len2(uint16_t uuid_data, bt_uuid_t *out_uuid)
{
    out_uuid->uuid_len = UUID16_LEN;
    out_uuid->uuid[0] = (uint8_t)(uuid_data >> 8); /* 8: octet bit num */
    out_uuid->uuid[1] = (uint8_t)(uuid_data);
}

/* 设置注册服务时的name */
void ble_hid_set_rcu_device_name_value(const uint8_t *name, const uint8_t len)
{
    size_t len_name = sizeof(g_device_rcu_name_value);
    if (memcpy_s(&g_device_rcu_name_value, len_name, name, len) != EOK) {
        osal_printk("[btsrv][ERROR] memcpy name fail\n");
    }
}

/* 设置注册服务时的appearance */
void ble_hid_set_rcu_device_appearance_value(uint16_t appearance)
{
    g_device_rcu_appearance_value = appearance;
}

/* 创建服务 */
static void ble_hid_add_service(void)
{
    bt_uuid_t hid_service_uuid = { 0 };
    bts_data_to_uuid_len2(BLE_UUID_HUMAN_INTERFACE_DEVICE, &hid_service_uuid);
    gatts_add_service(BLE_HID_SERVER_ID, &hid_service_uuid, true);
}

/* 添加特征：HID information */
static void ble_hid_add_character_hid_information(uint8_t server_id, uint16_t srvc_handle)
{
    bt_uuid_t hid_information_uuid = { 0 };
    bts_data_to_uuid_len2(BLE_UUID_HID_INFORMATION, &hid_information_uuid);
    gatts_add_chara_info_t character;
    character.chara_uuid = hid_information_uuid;
    character.permissions = GATT_ATTRIBUTE_PERMISSION_READ;
    character.properties = HID_INFORMATION_PROPERTIES;
    character.value_len = sizeof(g_hid_information_val);
    character.value = g_hid_information_val;
    gatts_add_characteristic(server_id, srvc_handle, &character);
}

/* 添加特征：HID report map */
static void ble_hid_add_character_report_map(uint8_t server_id, uint16_t srvc_handle)
{
    bt_uuid_t hid_report_map_uuid = { 0 };
    bts_data_to_uuid_len2(BLE_UUID_REPORT_MAP, &hid_report_map_uuid);
    gatts_add_chara_info_t character;
    character.chara_uuid = hid_report_map_uuid;
    character.permissions = GATT_ATTRIBUTE_PERMISSION_READ | GATT_ATTRIBUTE_PERMISSION_AUTHENTICATION_NEED;
    character.properties = HID_REPORT_MAP_PROPERTIES;
    character.value_len = sizeof(g_srv_hid_rcu_report_map);
    character.value = g_srv_hid_rcu_report_map;
    gatts_add_characteristic(server_id, srvc_handle, &character);
}

/* 添加特征：HID control point */
static void ble_hid_add_character_hid_control_point(uint8_t server_id, uint16_t srvc_handle)
{
    bt_uuid_t hid_control_point_uuid = { 0 };
    bts_data_to_uuid_len2(BLE_UUID_HID_CONTROL_POINT, &hid_control_point_uuid);
    gatts_add_chara_info_t character;
    character.chara_uuid = hid_control_point_uuid;
    character.permissions = GATT_ATTRIBUTE_PERMISSION_READ;
    character.properties = HID_CONTROL_POINT_PROPERTIES;
    character.value_len = sizeof(g_control_point_val);
    character.value = g_control_point_val;
    gatts_add_characteristic(server_id, srvc_handle, &character);
}

/* 添加描述符：客户端特性配置 */
static void ble_hid_add_descriptor_ccc(uint8_t server_id, uint16_t srvc_handle)
{
    bt_uuid_t ccc_uuid = { 0 };
    bts_data_to_uuid_len2(BLE_UUID_CLIENT_CHARACTERISTIC_CONFIGURATION, &ccc_uuid);
    gatts_add_desc_info_t descriptor;
    descriptor.desc_uuid = ccc_uuid;
    descriptor.permissions = GATT_ATTRIBUTE_PERMISSION_READ | GATT_ATTRIBUTE_PERMISSION_WRITE;
    descriptor.value_len = sizeof(g_ccc_val);
    descriptor.value = g_ccc_val;
    gatts_add_descriptor(server_id, srvc_handle, &descriptor);
}

/* 添加描述符：HID keyboard report reference */
static void ble_hid_add_descriptor_keyboard_report_reference(uint8_t server_id, uint16_t srvc_handle,
                                                             bool is_input_flag)
{
    bt_uuid_t hid_report_reference_uuid = { 0 };
    bts_data_to_uuid_len2(BLE_UUID_REPORT_REFERENCE, &hid_report_reference_uuid);
    gatts_add_desc_info_t descriptor;
    descriptor.desc_uuid = hid_report_reference_uuid;
    descriptor.permissions = GATT_ATTRIBUTE_PERMISSION_READ;
    if (is_input_flag) {
        descriptor.value = g_keyboard_report_reference_val_input;
        descriptor.value_len = sizeof(g_keyboard_report_reference_val_input);
    } else {
        descriptor.value = g_keyboard_report_reference_val_output;
        descriptor.value_len = sizeof(g_keyboard_report_reference_val_output);
    }
    gatts_add_descriptor(server_id, srvc_handle, &descriptor);
}

/* 添加描述符：HID mouse report reference */
static void ble_hid_add_descriptor_mouse_report_reference(uint8_t server_id, uint16_t srvc_handle,
                                                          bool is_input_flag)
{
    bt_uuid_t hid_report_reference_uuid = { 0 };
    bts_data_to_uuid_len2(BLE_UUID_REPORT_REFERENCE, &hid_report_reference_uuid);
    gatts_add_desc_info_t descriptor;
    descriptor.desc_uuid = hid_report_reference_uuid;
    descriptor.permissions = GATT_ATTRIBUTE_PERMISSION_READ;
    if (is_input_flag) {
        descriptor.value = g_mouse_report_reference_val_input;
        descriptor.value_len = sizeof(g_mouse_report_reference_val_input);
    } else {
        descriptor.value = g_mouse_report_reference_val_output;
        descriptor.value_len = sizeof(g_mouse_report_reference_val_output);
    }
    gatts_add_descriptor(server_id, srvc_handle, &descriptor);
}

/* 添加描述符：HID consumer report reference */
static void ble_hid_add_descriptor_consumer_report_reference(uint8_t server_id, uint16_t srvc_handle,
                                                             bool is_input_flag)
{
    bt_uuid_t hid_report_reference_uuid = { 0 };
    bts_data_to_uuid_len2(BLE_UUID_REPORT_REFERENCE, &hid_report_reference_uuid);
    gatts_add_desc_info_t descriptor;
    descriptor.desc_uuid = hid_report_reference_uuid;
    descriptor.permissions = GATT_ATTRIBUTE_PERMISSION_READ;
    if (is_input_flag) {
        descriptor.value = g_consumer_report_reference_val_input;
        descriptor.value_len = sizeof(g_consumer_report_reference_val_input);
    } else {
        descriptor.value = g_consumer_report_reference_val_output;
        descriptor.value_len = sizeof(g_consumer_report_reference_val_output);
    }
    gatts_add_descriptor(server_id, srvc_handle, &descriptor);
}

/* 添加描述符：HID power report reference */
static void ble_hid_add_descriptor_power_report_reference(uint8_t server_id, uint16_t srvc_handle,
                                                          bool is_input_flag)
{
    bt_uuid_t hid_report_reference_uuid = { 0 };
    bts_data_to_uuid_len2(BLE_UUID_REPORT_REFERENCE, &hid_report_reference_uuid);
    gatts_add_desc_info_t descriptor;
    descriptor.desc_uuid = hid_report_reference_uuid;
    descriptor.permissions = GATT_ATTRIBUTE_PERMISSION_READ;
    if (is_input_flag) {
        descriptor.value = g_power_report_reference_val_input;
        descriptor.value_len = sizeof(g_power_report_reference_val_input);
    } else {
        descriptor.value = g_power_report_reference_val_output;
        descriptor.value_len = sizeof(g_power_report_reference_val_output);
    }
    gatts_add_descriptor(server_id, srvc_handle, &descriptor);
}

/* 添加特征：HID keyboard input report(device to host) */
static void ble_hid_add_character_keyboard_input_report(uint8_t server_id, uint16_t srvc_handle)
{
    bt_uuid_t hid_report_uuid = { 0 };
    bts_data_to_uuid_len2(BLE_UUID_REPORT, &hid_report_uuid);
    gatts_add_chara_info_t character;
    character.chara_uuid = hid_report_uuid;
    character.permissions = GATT_ATTRIBUTE_PERMISSION_READ;
    character.properties = HID_INPUT_REPORT_PROPERTIES;
    character.value_len = sizeof(g_keyboard_input_report_value);
    character.value = g_keyboard_input_report_value;
    gatts_add_characteristic(server_id, srvc_handle, &character);
    ble_hid_add_descriptor_ccc(server_id, srvc_handle);
    ble_hid_add_descriptor_keyboard_report_reference(server_id, srvc_handle, true);
}

/* 添加特征：HID keyboard output report(host to device) */
static void ble_hid_add_character_keyboard_output_report(uint8_t server_id, uint16_t srvc_handle)
{
    bt_uuid_t hid_report_uuid = { 0 };
    bts_data_to_uuid_len2(BLE_UUID_REPORT, &hid_report_uuid);
    gatts_add_chara_info_t character;
    character.chara_uuid = hid_report_uuid;
    character.permissions = GATT_ATTRIBUTE_PERMISSION_READ;
    character.properties = HID_OUTPUT_REPORT_PROPERTIES;
    character.value_len = sizeof(g_keyboard_output_report_value);
    character.value = g_keyboard_output_report_value;
    gatts_add_characteristic(server_id, srvc_handle, &character);
    ble_hid_add_descriptor_keyboard_report_reference(server_id, srvc_handle, false);
}

/* 添加特征：HID mouse input report(device to host) */
static void ble_hid_add_character_mouse_input_report(uint8_t server_id, uint16_t srvc_handle)
{
    bt_uuid_t hid_report_uuid = { 0 };
    bts_data_to_uuid_len2(BLE_UUID_REPORT, &hid_report_uuid);
    gatts_add_chara_info_t character;
    character.chara_uuid = hid_report_uuid;
    character.permissions = GATT_ATTRIBUTE_PERMISSION_READ;
    character.properties = HID_INPUT_REPORT_PROPERTIES;
    character.value_len = sizeof(g_mouse_input_report_value);
    character.value = g_mouse_input_report_value;
    gatts_add_characteristic(server_id, srvc_handle, &character);
    ble_hid_add_descriptor_ccc(server_id, srvc_handle);
    ble_hid_add_descriptor_mouse_report_reference(server_id, srvc_handle, true);
}

/* 添加特征：HID mouse output report(host to device) */
static void ble_hid_add_character_mouse_output_report(uint8_t server_id, uint16_t srvc_handle)
{
    bt_uuid_t hid_report_uuid = { 0 };
    bts_data_to_uuid_len2(BLE_UUID_REPORT, &hid_report_uuid);
    gatts_add_chara_info_t character;
    character.chara_uuid = hid_report_uuid;
    character.permissions = GATT_ATTRIBUTE_PERMISSION_READ;
    character.properties = HID_OUTPUT_REPORT_PROPERTIES;
    character.value_len = sizeof(g_mouse_output_report_value);
    character.value = g_mouse_output_report_value;
    gatts_add_characteristic(server_id, srvc_handle, &character);
    ble_hid_add_descriptor_mouse_report_reference(server_id, srvc_handle, false);
}

/* 添加特征：HID consumer input report(device to host) */
static void ble_hid_add_character_consumer_input_report(uint8_t server_id, uint16_t srvc_handle)
{
    bt_uuid_t hid_report_uuid = { 0 };
    bts_data_to_uuid_len2(BLE_UUID_REPORT, &hid_report_uuid);
    gatts_add_chara_info_t character;
    character.chara_uuid = hid_report_uuid;
    character.permissions = GATT_ATTRIBUTE_PERMISSION_READ;
    character.properties = HID_INPUT_REPORT_PROPERTIES;
    character.value_len = sizeof(g_consumer_input_report_value);
    character.value = g_consumer_input_report_value;
    gatts_add_characteristic(server_id, srvc_handle, &character);
    ble_hid_add_descriptor_ccc(server_id, srvc_handle);
    ble_hid_add_descriptor_consumer_report_reference(server_id, srvc_handle, true);
}

/* 添加特征：HID consumer output report(host to device) */
static void ble_hid_add_character_consumer_output_report(uint8_t server_id, uint16_t srvc_handle)
{
    bt_uuid_t hid_report_uuid = { 0 };
    bts_data_to_uuid_len2(BLE_UUID_REPORT, &hid_report_uuid);
    gatts_add_chara_info_t character;
    character.chara_uuid = hid_report_uuid;
    character.permissions = GATT_ATTRIBUTE_PERMISSION_READ;
    character.properties = HID_OUTPUT_REPORT_PROPERTIES;
    character.value_len = sizeof(g_consumer_output_report_value);
    character.value = g_consumer_output_report_value;
    gatts_add_characteristic(server_id, srvc_handle, &character);
    ble_hid_add_descriptor_consumer_report_reference(server_id, srvc_handle, false);
}

/* 添加特征：HID power input report(device to host) */
static void ble_hid_add_character_power_input_report(uint8_t server_id, uint16_t srvc_handle)
{
    bt_uuid_t hid_report_uuid = { 0 };
    bts_data_to_uuid_len2(BLE_UUID_REPORT, &hid_report_uuid);
    gatts_add_chara_info_t character;
    character.chara_uuid = hid_report_uuid;
    character.permissions = GATT_ATTRIBUTE_PERMISSION_READ;
    character.properties = HID_INPUT_REPORT_PROPERTIES;
    character.value_len = sizeof(g_power_input_report_value);
    character.value = g_power_input_report_value;
    gatts_add_characteristic(server_id, srvc_handle, &character);
    ble_hid_add_descriptor_ccc(server_id, srvc_handle);
    ble_hid_add_descriptor_power_report_reference(server_id, srvc_handle, true);
}

/* 添加特征：HID power output report(host to device) */
static void ble_hid_add_character_power_output_report(uint8_t server_id, uint16_t srvc_handle)
{
    bt_uuid_t hid_report_uuid = { 0 };
    bts_data_to_uuid_len2(BLE_UUID_REPORT, &hid_report_uuid);
    gatts_add_chara_info_t character;
    character.chara_uuid = hid_report_uuid;
    character.permissions = GATT_ATTRIBUTE_PERMISSION_READ;
    character.properties = HID_OUTPUT_REPORT_PROPERTIES;
    character.value_len = sizeof(g_power_output_report_value);
    character.value = g_power_output_report_value;
    gatts_add_characteristic(server_id, srvc_handle, &character);
    ble_hid_add_descriptor_power_report_reference(server_id, srvc_handle, false);
}

/* 添加特征：HID protocol mode */
static void ble_hid_add_character_protocol_mode(uint8_t server_id, uint16_t srvc_handle)
{
    bt_uuid_t hid_protocol_mode_uuid = { 0 };
    bts_data_to_uuid_len2(BLE_UUID_PROTOCOL_MODE, &hid_protocol_mode_uuid);
    gatts_add_chara_info_t character;
    character.chara_uuid = hid_protocol_mode_uuid;
    character.permissions = GATT_ATTRIBUTE_PERMISSION_READ;
    character.properties = HID_PROTOCOL_MODE_PROPERTIES;
    character.value_len = sizeof(g_protocol_mode_val);
    character.value = g_protocol_mode_val;
    gatts_add_characteristic(server_id, srvc_handle, &character);
}

/* 添加HID服务的所有特征和描述符 */
static void ble_hid_add_characters_and_descriptors(uint8_t server_id, uint16_t srvc_handle)
{
    /* HID Information */
    ble_hid_add_character_hid_information(server_id, srvc_handle);
    /* Report Map */
    ble_hid_add_character_report_map(server_id, srvc_handle);
    /* Protocol Mode */
    ble_hid_add_character_protocol_mode(server_id, srvc_handle);
    /* Keyboard Input Report */
    ble_hid_add_character_keyboard_input_report(server_id, srvc_handle);
    /* Keyboard Output Report */
    ble_hid_add_character_keyboard_output_report(server_id, srvc_handle);
    /* Mouse Input Report */
    ble_hid_add_character_mouse_input_report(server_id, srvc_handle);
    /* Mouse Output Report */
    ble_hid_add_character_mouse_output_report(server_id, srvc_handle);
    /* Consumer Input Report */
    ble_hid_add_character_consumer_input_report(server_id, srvc_handle);
    /* Consmuer Output Report */
    ble_hid_add_character_consumer_output_report(server_id, srvc_handle);
    /* Power Input Report */
    ble_hid_add_character_power_input_report(server_id, srvc_handle);
    /* Power Output Report */
    ble_hid_add_character_power_output_report(server_id, srvc_handle);
    /* HID Control Point */
    ble_hid_add_character_hid_control_point(server_id, srvc_handle);
}

bool bts_rcu_compare_uuid(bt_uuid_t *uuid1, bt_uuid_t *uuid2)
{
    if (uuid1->uuid_len != uuid2->uuid_len) {
        return false;
    }
    if (memcmp(uuid1->uuid, uuid2->uuid, uuid1->uuid_len) != 0) {
        return false;
    }
    return true;
}

/* 服务添加回调 */
static void ble_hid_server_service_add_cbk(uint8_t server_id, bt_uuid_t *uuid, uint16_t handle, errcode_t status)
{
    int8_t i = 0;
    osal_printk("[hid]ServiceAddCallback server: %d srv_handle: %d uuid_len: %d\n",
        server_id, handle, uuid->uuid_len);
    osal_printk("uuid:");
    for (i = 0; i < uuid->uuid_len ; i++) {
        osal_printk("%02x", uuid->uuid[i]);
    }
    osal_printk("\n");
    osal_printk("status:%d\n", status);
    osal_printk("[hid][INFO]beginning add characters and descriptors\r\n");
    ble_hid_add_characters_and_descriptors(server_id, handle);
    osal_printk("[hid][INFO]beginning start service\r\n");
    gatts_start_service(server_id, handle);
}

/* 特征添加回调 */
static void  ble_hid_server_characteristic_add_cbk(uint8_t server_id, bt_uuid_t *uuid, uint16_t service_handle,
    gatts_add_character_result_t *result, errcode_t status)
{
    int8_t i = 0;
    bt_uuid_t report_uuid = {0};
    bts_data_to_uuid_len2(BLE_UUID_REPORT, &report_uuid);
    osal_printk("[hid]CharacteristicAddCallback server: %d srvc_hdl: %d "
        "char_hdl: %d char_val_hdl: %d uuid_len: %d \n",
        server_id, service_handle, result->handle, result->value_handle, uuid->uuid_len);
    osal_printk("uuid:");
    for (i = 0; i < uuid->uuid_len ; i++) {
        osal_printk("%02x", uuid->uuid[i]);
    }
    if (bts_rcu_compare_uuid(uuid, &report_uuid)) {
        if (g_input_report_count == INPUT_KEYBOARD) {
            g_hid_keyboard_input_report_att_hdl = result->value_handle;
        } else if (g_input_report_count == INPUT_MOUSE) {
            g_hid_mouse_input_report_att_hdl = result->value_handle;
        } else if (g_input_report_count == INPUT_CONSUMER) {
            g_hid_consumer_input_report_att_hdl = result->value_handle;
        } else if (g_input_report_count == INPUT_POWER) {
            g_hid_power_input_report_att_hdl = result->value_handle;
        }
        g_input_report_count++;
    }
    osal_printk("\nstatus:%d\n", status);
}

/* 描述符添加回调 */
static void  ble_hid_server_descriptor_add_cbk(uint8_t server_id, bt_uuid_t *uuid, uint16_t service_handle,
    uint16_t handle, errcode_t status)
{
    int8_t i = 0;
    osal_printk("[hid]DescriptorAddCallback server: %d srv_hdl: %d desc_hdl: %d "
        "uuid_len:%d\n", server_id, service_handle, handle, uuid->uuid_len);
    osal_printk("uuid:");
    for (i = 0; i < uuid->uuid_len ; i++) {
        osal_printk("%02x", (uint8_t)uuid->uuid[i]);
    }
    osal_printk("\n");
    osal_printk("status:%d\n", status);
}

/* 开始服务回调 */
static void ble_hid_server_service_start_cbk(uint8_t server_id, uint16_t handle, errcode_t status)
{
    osal_printk("[hid]ServiceStartCallback server: %d srv_hdl: %d status: %d\n",
        server_id, handle, status);
}

static void ble_hid_receive_write_req_cbk(uint8_t server_id, uint16_t conn_id, gatts_req_write_cb_t *write_cb_para,
    errcode_t status)
{
    osal_printk("[hid]ReceiveWriteReqCallback--server_id:%d conn_id:%d\n", server_id, conn_id);
    osal_printk("request_id:%d att_handle:%d offset:%d need_rsp:%d need_authorize:%d is_prep:%d\n",
        write_cb_para->request_id, write_cb_para->handle, write_cb_para->offset, write_cb_para->need_rsp,
        write_cb_para->need_authorize, write_cb_para->is_prep);
    osal_printk("data_len:%d data:\n", write_cb_para->length);
    for (uint8_t i = 0; i < write_cb_para->length; i++) {
        osal_printk("%02x ", write_cb_para->value[i]);
    }
    osal_printk("\n");
    osal_printk("status:%d\n", status);
}

static void ble_hid_receive_read_req_cbk(uint8_t server_id, uint16_t conn_id, gatts_req_read_cb_t *read_cb_para,
    errcode_t status)
{
    osal_printk("[hid]ReceiveReadReq--server_id:%d conn_id:%d\n", server_id, conn_id);
    osal_printk("request_id:%d att_handle:%d offset:%d need_rsp:%d need_authorize:%d is_long:%d\n",
        read_cb_para->request_id, read_cb_para->handle, read_cb_para->offset, read_cb_para->need_rsp,
        read_cb_para->need_authorize, read_cb_para->is_long);
    osal_printk("status:%d\n", status);
}

static void ble_hid_mtu_changed_cbk(uint8_t server_id, uint16_t conn_id, uint16_t mtu_size, errcode_t status)
{
    osal_printk("[hid]MtuChanged--server_id:%d conn_id:%d\n", server_id, conn_id);
    osal_printk("mtusize:%d\n", mtu_size);
    osal_printk("status:%d\n", status);
}

static errcode_t ble_hid_server_register_gatt_callbacks(void)
{
    errcode_t ret = ERRCODE_BT_SUCCESS;
    gatts_callbacks_t cb = {0};
    cb.add_service_cb = ble_hid_server_service_add_cbk;
    cb.add_characteristic_cb = ble_hid_server_characteristic_add_cbk;
    cb.add_descriptor_cb = ble_hid_server_descriptor_add_cbk;
    cb.start_service_cb = ble_hid_server_service_start_cbk;
    cb.read_request_cb = ble_hid_receive_read_req_cbk;
    cb.write_request_cb = ble_hid_receive_write_req_cbk;
    cb.mtu_changed_cb = ble_hid_mtu_changed_cbk;
    ret = gatts_register_callbacks(&cb);
    return ret;
}

/* 初始化HID device */
void ble_hid_rcu_server_init(void)
{
    ble_hid_server_register_gatt_callbacks();
    errcode_t ret = ERRCODE_BT_UNHANDLED;
    bt_uuid_t app_uuid = {0};
    app_uuid.uuid_len = sizeof(g_server_app_uuid_for_test);
    if (memcpy_s(app_uuid.uuid, app_uuid.uuid_len, g_server_app_uuid_for_test,
                 sizeof(g_server_app_uuid_for_test)) != EOK) {
        osal_printk("[hid][ERROR]add server app uuid memcpy failed\r\n");
        return;
    }
    osal_printk("[hid][INFO]beginning add server\r\n");
    enable_ble();
    gap_ble_set_local_name(g_device_rcu_name_value, g_device_rcu_name_len);
    gap_ble_set_local_appearance(g_device_rcu_appearance_value);
    ret = gatts_register_server(&app_uuid, &g_server_id);
    if ((ret != ERRCODE_BT_SUCCESS) || (g_server_id == INVALID_SERVER_ID)) {
        osal_printk("[hid][ERROR]add server failed\r\n");
        return;
    }
    osal_printk("[hid][INFO]beginning add service\r\n");
    ble_hid_add_service(); /* 添加HID服务 */
}

/* device向host发送数据：input report */
errcode_t ble_hid_rcu_server_send_input_report(const uint8_t *data, uint8_t len)
{
    gatts_ntf_ind_t param = {0};
    param.attr_handle = g_hid_input_report_att_hdl;
    param.value_len = len;
    param.value = osal_vmalloc(len);
    if (param.value == NULL) {
        osal_printk("[hid][ERROR]send input report new fail\r\n");
        return ERRCODE_BT_MALLOC_FAIL;
    }
    if (memcpy_s(param.value, param.value_len, data, len) != EOK) {
        osal_printk("[hid][ERROR]send input report memcpy fail\r\n");
        osal_vfree(param.value);
        return ERRCODE_BT_FAIL;
    }
    gatts_notify_indicate(BLE_HID_SERVER_ID, BLE_SINGLE_LINK_CONNECT_ID, &param);
    osal_vfree(param.value);
    return ERRCODE_BT_SUCCESS;
}

/* device向host发送数据by uuid：keyboard_input report */
errcode_t ble_hid_rcu_server_send_keyboard_input_report_by_uuid(const uint8_t *data, uint8_t len, uint16_t conn_id)
{
    gatts_ntf_ind_by_uuid_t param = {0};
    param.start_handle = g_hid_keyboard_input_report_att_hdl;
    param.end_handle = g_hid_keyboard_input_report_att_hdl;
    bts_data_to_uuid_len2(BLE_UUID_REPORT, &param.chara_uuid);
    param.value_len = len;
    param.value = osal_vmalloc(len);
    if (param.value == NULL) {
        osal_printk("[hid][ERROR]send input report new fail\r\n");
        return ERRCODE_BT_MALLOC_FAIL;
    }
    if (memcpy_s(param.value, param.value_len, data, len) != EOK) {
        osal_printk("[hid][ERROR]send input report memcpy fail\r\n");
        osal_vfree(param.value);
        return ERRCODE_BT_FAIL;
    }
    gatts_notify_indicate_by_uuid(BLE_HID_SERVER_ID, conn_id, &param);
    osal_vfree(param.value);
    return ERRCODE_BT_SUCCESS;
}

/* device向host发送数据by uuid：mouse input report */
errcode_t ble_hid_rcu_server_send_mouse_input_report_by_uuid(const uint8_t *data, uint8_t len, uint16_t conn_id)
{
    gatts_ntf_ind_by_uuid_t param = {0};
    param.start_handle = g_hid_mouse_input_report_att_hdl;
    param.end_handle = g_hid_mouse_input_report_att_hdl;
    bts_data_to_uuid_len2(BLE_UUID_REPORT, &param.chara_uuid);
    param.value_len = len;
    param.value = osal_vmalloc(len);
    if (param.value == NULL) {
        osal_printk("[hid][ERROR]send input report new fail\r\n");
        return ERRCODE_BT_MALLOC_FAIL;
    }
    if (memcpy_s(param.value, param.value_len, data, len) != EOK) {
        osal_printk("[hid][ERROR]send input report memcpy fail\r\n");
        osal_vfree(param.value);
        return ERRCODE_BT_FAIL;
    }
    gatts_notify_indicate_by_uuid(BLE_HID_SERVER_ID, conn_id, &param);
    osal_vfree(param.value);
    return ERRCODE_BT_SUCCESS;
}

/* device向host发送数据by uuid：consumer input report */
errcode_t ble_hid_rcu_server_send_consumer_input_report_by_uuid(const uint8_t *data, uint8_t len, uint16_t conn_id)
{
    gatts_ntf_ind_by_uuid_t param = {0};
    param.start_handle = g_hid_consumer_input_report_att_hdl;
    param.end_handle = g_hid_consumer_input_report_att_hdl;
    bts_data_to_uuid_len2(BLE_UUID_REPORT, &param.chara_uuid);
    param.value_len = len;
    param.value = osal_vmalloc(len);
    if (param.value == NULL) {
        osal_printk("[hid][ERROR]send input report new fail\r\n");
        return ERRCODE_BT_MALLOC_FAIL;
    }
    if (memcpy_s(param.value, param.value_len, data, len) != EOK) {
        osal_printk("[hid][ERROR]send input report memcpy fail\r\n");
        osal_vfree(param.value);
        return ERRCODE_BT_FAIL;
    }
    gatts_notify_indicate_by_uuid(BLE_HID_SERVER_ID, conn_id, &param);
    osal_vfree(param.value);
    return ERRCODE_BT_SUCCESS;
}

/* device向host发送数据by power input report */
errcode_t ble_hid_rcu_server_send_power_input_report_by_uuid(const uint8_t *data, uint8_t len, uint16_t conn_id)
{
    gatts_ntf_ind_by_uuid_t param = {0};
    param.start_handle = g_hid_power_input_report_att_hdl;
    param.end_handle = g_hid_power_input_report_att_hdl;
    bts_data_to_uuid_len2(BLE_UUID_REPORT, &param.chara_uuid);
    param.value_len = len;
    param.value = osal_vmalloc(len);
    if (param.value == NULL) {
        osal_printk("[hid][ERROR]send input report new fail\r\n");
        return ERRCODE_BT_MALLOC_FAIL;
    }
    if (memcpy_s(param.value, param.value_len, data, len) != EOK) {
        osal_printk("[hid][ERROR]send input report memcpy fail\r\n");
        osal_vfree(param.value);
        return ERRCODE_BT_FAIL;
    }
    gatts_notify_indicate_by_uuid(BLE_HID_SERVER_ID, conn_id, &param);
    osal_vfree(param.value);
    return ERRCODE_BT_SUCCESS;
}
