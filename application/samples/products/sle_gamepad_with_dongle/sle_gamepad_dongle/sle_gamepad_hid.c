/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: SLE GAMEPAD Dongle Hid Source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-09-28, Create file. \n
 */
#include "gadget/f_hid.h"

/* Main items */
#define input(size)                 (0x80 | (size))
#define output(size)                (0x90 | (size))
#define feature(size)               (0xB0 | (size))
#define collection(size)            (0xA0 | (size))
#define end_collection(size)        (0xC0 | (size))

/* Global items */
#define usage_page(size)            (0x04 | (size))
#define logical_minimum(size)       (0x14 | (size))
#define logical_maximum(size)       (0x24 | (size))
#define physical_minimum(size)      (0x34 | (size))
#define physical_maximum(size)      (0x44 | (size))
#define uint_exponent(size)         (0x54 | (size))
#define uint(size)                  (0x64 | (size))
#define report_size(size)           (0x74 | (size))
#define report_id(size)             (0x84 | (size))
#define report_count(size)          (0x94 | (size))
#define push(size)                  (0xA4 | (size))
#define pop(size)                   (0xB4 | (size))

/* Local items */
#define usage(size)                 (0x08 | (size))
#define usage_minimum(size)         (0x18 | (size))
#define usage_maximum(size)         (0x28 | (size))
#define designator_index(size)      (0x38 | (size))
#define designator_minimum(size)    (0x48 | (size))
#define designator_maximum(size)    (0x58 | (size))
#define string_index(size)          (0x78 | (size))
#define string_minimum(size)        (0x88 | (size))
#define string_maximum(size)        (0x98 | (size))
#define delimiter(size)             (0xA8 | (size))

static uint8_t g_report_desc_hid[] = {
    // gamepad
    usage_page(1),       0x01,       // USAGE_PAGE (Generic Desktop) 用途：通用桌面设备
    usage(1),            0x05,       // USAGE (Game Controls Page) 子用途：游戏手柄
    collection(1),       0x01,       // COLLECTION (Application) 创建一个集合
    report_id(1),        0x03,       // REPORT_ID (3)

    // 14按键
    usage_page(1),       0x09,       // USAGE_PAGE (Button) 数据用途：按键
    usage_minimum(1),    0x01,       // USAGE_MINIMUM (Button 1) 数据最小值
    usage_maximum(1),    0x0E,       // USAGE_MAXIMUM (Button 14) 数据最大值
    logical_minimum(1),  0x00,       // LOGICAL_MINIMUM (0) 数据逻辑最小值
    logical_maximum(1),  0x01,       // LOGICAL_MAXIMUM (1) 数据逻辑最大值
    report_size(1),      0x01,       // REPORT_SIZE (1) 数据长度1bit
    report_count(1),     0x0E,       // REPORT_COUNT (14) 数据个数14
    input(1),            0x02,       // INPUT (Data, Var, Abs) 数据方向：输入(变量，数值，绝对值)
    report_count(1),     0x01,
    report_size(1),      0x02,
    input(1),            0x01,

    // 十字键
    usage_page(1),       0x01,
    usage(1),            0x39,
    logical_minimum(1),  0x00,
    logical_maximum(1),  0x07,
    physical_minimum(1), 0x00,
    physical_maximum(2), 0x3B, 0x01,
    uint(1),             0x14,
    report_size(1),      0x04,
    report_count(1),     0x01,
    input(1),            0x02,
    report_count(1),     0x01,
    report_size(1),      0x04,
    input(1),            0x01,

    // X,Y,Z
    logical_minimum(1),  0x00,
    logical_maximum(2),  0xFF, 0x00,
    report_size(1),      0x08,
    usage(1),            0x01,
    collection(1),       0x00,
    usage(1),            0x30,
    usage(1),            0x31,
    usage(1),            0x32,
    usage(1),            0x35,
    report_count(1),     0x04,
    input(1),            0x02,
    end_collection(0),
    end_collection(0),                // END_COLLECTION 关集合
};

int32_t sle_gamepad_dongle_set_report_desc_hid(void)
{
    return hid_add_report_descriptor(g_report_desc_hid, sizeof(g_report_desc_hid), 0);
}