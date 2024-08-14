/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: SLE GAMEPAD Hid Config. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-09-28, Create file. \n
 */

#ifndef SLE_GAMEPAD_HID_H
#define SLE_GAMEPAD_HID_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define USB_HID_GAMEPAD_MAX_KEY_LENTH      7
#define SLE_GAMEPAD_DONGLE_OK              0
#define SLE_GAMEPAD_DONGLE_FAILED          1

typedef struct {
    uint8_t x_axis;             // 左摇杆X
    uint8_t y_axis;             // 左摇杆Y
    uint8_t z_axis;             // 右摇杆X
    uint8_t z_axisrotation;     // 右摇杆Y；Z轴旋转
} joystick_status_t;

typedef union gamepad_key {
    uint16_t d16;
    struct {
        uint16_t y_key      : 1;
        uint16_t b_key      : 1;
        uint16_t a_key      : 1;
        uint16_t x_key      : 1;
        uint16_t l_key      : 1;
        uint16_t r_key      : 1;
        uint16_t zl_key     : 1;
        uint16_t zr_key     : 1;
        uint16_t n_key      : 1;   // negative
        uint16_t p_key      : 1;   // postive
        uint16_t l_joystick : 1;
        uint16_t r_joystick : 1;
        uint16_t home_key   : 1;
        uint16_t photo_key  : 1;
        uint16_t reserved   : 2;
    } b;
} usb_hid_gamepad_key_t;

/**
 * @if Eng
 * @brief Definitaion of usb hid gamepad report struct.
 * @else
 * @brief 定义USB HID上报的结构体。
 * @endif
 */
typedef struct usb_hid_gamepad_report {
    uint8_t kind;
    uint8_t data[USB_HID_GAMEPAD_MAX_KEY_LENTH];
} usb_hid_gamepad_report_t;

int32_t sle_gamepad_dongle_set_report_desc_hid(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif