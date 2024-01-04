/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: SLE rcu hid Config. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-09-21, Create file. \n
 */

#ifndef SLE_RCU_HID_H
#define SLE_RCU_HID_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define USB_HID_RCU_MAX_KEY_LENTH      6
#define SLE_RCU_DONGLE_OK              0
#define SLE_RCU_DONGLE_FAILED          1

typedef union mouse_key {
    struct {
        uint8_t left_key   : 1;
        uint8_t right_key  : 1;
        uint8_t mid_key    : 1;
        uint8_t reserved   : 5;
    } b;
    uint8_t d8;
} mouse_key_t;

/**
 * @if Eng
 * @brief Definitaion of usb hid rcu report struct.
 * @else
 * @brief 定义USB MOUSE HID上报的结构体。
 * @endif
 */
typedef struct usb_hid_rcu_mouse_report {
    uint8_t kind;
    mouse_key_t key;
    int8_t x;                 /* A negative value indicates that the mouse moves left. */
    int8_t y;                 /* A negative value indicates that the mouse moves up. */
    int8_t wheel;             /* A negative value indicates that the wheel roll forward. */
} usb_hid_rcu_mouse_report_t;

/**
 * @if Eng
 * @brief Definitaion of usb hid rcu report struct.
 * @else
 * @brief 定义USB KEYBOARD HID上报的结构体。
 * @endif
 */
typedef struct usb_hid_rcu_keyboard_report {
    uint8_t kind;
    uint8_t special_key;                         /*!< 8bit special key(Lctrl Lshift Lalt Lgui Rctrl Rshift Ralt Rgui) */
    uint8_t reserve;
    uint8_t key[USB_HID_RCU_MAX_KEY_LENTH]; /*!< Normal key */
} usb_hid_rcu_keyboard_report_t;

/**
 * @if Eng
 * @brief Definitaion of usb hid rcu report struct.
 * @else
 * @brief 定义USB CONSUMER HID上报的结构体。
 * @endif
 */
typedef struct usb_hid_rcu_consumer_report {
    uint8_t kind;
    uint8_t comsumer_key0;
    uint8_t comsumer_key1;
} usb_hid_rcu_consumer_report_t;

int32_t sle_rcu_dongle_set_report_desc_hid(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif