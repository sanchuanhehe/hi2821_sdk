/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: SLE keyboard hid Config. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-07-28, Create file. \n
 */

#ifndef SLE_KEYBOARD_HID_H
#define SLE_KEYBOARD_HID_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define USB_HID_KEYBOARD_MAX_KEY_LENTH      6
#define SLE_KEYBOARD_DONGLE_OK              0
#define SLE_KEYBOARD_DONGLE_FAILED          1

/**
 * @if Eng
 * @brief Definitaion of usb hid keyboard report struct.
 * @else
 * @brief 定义USB HID上报的结构体。
 * @endif
 */
typedef struct usb_hid_keyboard_report {
    uint8_t kind;
    uint8_t special_key;                         /*!< 8bit special key(Lctrl Lshift Lalt Lgui Rctrl Rshift Ralt Rgui) */
    uint8_t reversed;                            /*!< Reversed, Must be zero */
    uint8_t key[USB_HID_KEYBOARD_MAX_KEY_LENTH]; /*!< Normal key */
} usb_hid_keyboard_report_t;

int32_t sle_keyboard_dongle_set_report_desc_hid(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif