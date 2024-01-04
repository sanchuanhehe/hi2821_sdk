/*
 * Copyright (c) hisilicon (shanghai) Technologies Co. 2022. All rights reserved.
 *
 * Description: BT HID Service Server module.
 */

/**
 * @defgroup bluetooth_bts_hid_server HID SERVER API
 * @ingroup  bluetooth
 * @{
 */
#ifndef BLE_HID_SERVER_MOUSE_H
#define BLE_HID_SERVER_MOUSE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Human Interface Device UUID */
#define BLE_UUID_HUMAN_INTERFACE_DEVICE 0x1812
/* HID Information UUID */
#define BLE_UUID_HID_INFORMATION 0x2A4A
/* Report Map UUID */
#define BLE_UUID_REPORT_MAP 0x2A4B
/* HID Control Point UUID */
#define BLE_UUID_HID_CONTROL_POINT 0x2A4C
/* Boot mouse Input Report UUID */
#define BLE_UUID_BOOT_MOUSE_INPUT_REPORT 0x2A33
/* Boot mouse Output Report UUID */
#define BLE_UUID_BOOT_MOUSE_OUTPUT_REPORT 0x2A32
/* Report UUID */
#define BLE_UUID_REPORT 0x2A4D
/* Protocol Mode UUID */
#define BLE_UUID_PROTOCOL_MODE 0x2A4E
/* Client Characteristic Configuration UUID */
#define BLE_UUID_CLIENT_CHARACTERISTIC_CONFIGURATION 0x2902
/* External Report Reference UUID */
#define BLE_UUID_EXTERNAL_REPORT_REFERENCE 0x2907
/* Report Reference UUID */
#define BLE_UUID_REPORT_REFERENCE 0x2908

#ifndef BTH_BLE_HCI_ONETRACK
#pragma pack (1)
#endif
/* Mouse transmission data struct */
typedef struct {
    uint16_t button_mask;
    int16_t x; /* mouse的x坐标 */
    int16_t y; /* mouse的y坐标 */
    uint8_t wheel;
    uint8_t ac_pan;
} ble_hid_high_mouse_event_st;
#ifndef BTH_BLE_HCI_ONETRACK
#pragma pack ()
#endif

uint8_t ble_get_server_id(void);
void ble_pro_dis_server_init(void);
void bth_ble_bas_server_init(void);
errcode_t ble_bas_server_battery_level_notify(void);
errcode_t ble_bas_server_battery_level_status_notify(void);
/**
 * @if Eng
 * @brief  Use this funtion to change name value.
 * @par name, len.
 * @attention  NULL
 * @param  NULL
 * @retval NULL
 * @par Dependency:
 * @li ble_hid_server.h
 * @else
 * @brief  设置注册服务时的name
 * @par NULL
 * @attention  NULL
 * @param  NULL
 * @retval NULL
 * @par 依赖:
 * @li ble_hid_server.h
 * @endif
 */
void ble_hid_set_mouse_name_value(const uint8_t *name, const uint8_t len);

/**
 * @if Eng
 * @brief  Use this funtion to change appearance value.
 * @par appearance.
 * @attention  NULL
 * @param  NULL
 * @retval NULL
 * @par Dependency:
 * @li ble_hid_server.h
 * @else
 * @brief  设置注册服务时的appearance
 * @par NULL
 * @attention  NULL
 * @param  NULL
 * @retval NULL
 * @par 依赖:
 * @li ble_hid_server.h
 * @endif
 */
void ble_hid_set_mouse_appearance_value(uint16_t appearance);

/**
 * @if Eng
 * @brief  Use this funtion to init HID service.
 * @par Description:add HID service, character and character descriptor, then start service.
 * @attention  NULL
 * @param  NULL
 * @retval NULL
 * @par Dependency:
 * @li ble_hid_server.h
 * @else
 * @brief  初始化HID服务。
 * @par 说明:添加HID服务，特征，特征描述符，然后启动服务。
 * @attention  NULL
 * @param  NULL
 * @retval NULL
 * @par 依赖:
 * @li ble_hid_server.h
 * @endif
 */
void ble_hid_mouse_server_init(void);

/**
 * @if Eng
 * @brief  Notify remote device the press button value.
 * @par Description:Notify remote device the press button value..
 * @attention  NULL
 * @param  [in]  value  Press button value.
 * @param  [in]  len    Length of press button value。
 * @retval ERRCODE_BT_SUCCESS    Excute successfully
 * @retval ERRCODE_BT_FAIL       Execute fail
 * @par Dependency:
 * @li ble_hid_server.h
 * @else
 * @brief  通知对端按键变化。
 * @par 说明：通知对端按键变化。
 * @attention  NULL
 * @param  [in]  data   按键值。
 * @param  [in]  len    按键值数据长度。
 * @retval ERRCODE_BT_SUCCESS    执行成功
 * @retval ERRCODE_BT_FAIL       执行失败
 * @par 依赖:
 * @li ble_hid_server.h
 * @endif
 */
errcode_t ble_hid_mouse_server_send_input_report(const ble_hid_high_mouse_event_st *mouse_data);

/**
 * @if Eng
 * @brief  Notify remote device the press button value.
 * @par Description:Notify remote device the press button value..
 * @attention  NULL
 * @param  [in]  data  Press button value.
 * @param  [in]  len   Length of press button value。
 * @retval ERRCODE_BT_SUCCESS    Excute successfully
 * @retval ERRCODE_BT_FAIL       Execute fail
 * @par Dependency:
 * @li ble_hid_server.h
 * @else
 * @brief  通知对端按键变化。
 * @par 说明：通知对端按键变化。
 * @attention  NULL
 * @param  [in]  data   按键值。
 * @param  [in]  len    按键值数据长度。
 * @retval ERRCODE_BT_SUCCESS    执行成功
 * @retval ERRCODE_BT_FAIL       执行失败
 * @par 依赖:
 * @li ble_hid_server.h
 * @endif
 */
errcode_t ble_hid_mouse_server_send_input_report_by_uuid(const ble_hid_high_mouse_event_st *mouse_data);

/**
 * @}
 */
#ifdef __cplusplus
}
#endif
#endif
