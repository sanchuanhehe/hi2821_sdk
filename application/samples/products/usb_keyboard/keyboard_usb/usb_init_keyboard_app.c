/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: USB Initialize Source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-07-07, Create file. \n
 */
#include <stdbool.h>
#include "gadget/f_hid.h"
#include "usb_init_keyboard_app.h"

#define USB_INIT_APP_MANUFACTURER  { 'H', 0, 'H', 0, 'H', 0, 'H', 0, 'l', 0, 'i', 0, 'c', 0, 'o', 0, 'n', 0 }
#define USB_INIT_APP_MANUFACTURER_LEN   20
#define USB_INIT_APP_PRODUCT  { 'H', 0, 'H', 0, '6', 0, '6', 0, '6', 0, '6', 0, ' ', 0, 'U', 0, 'S', 0, 'B', 0 }
#define USB_INIT_APP_PRODUCT_LEN        22
#define USB_INIT_APP_SERIAL   { '2', 0, '0', 0, '2', 0, '0', 0, '0', 0, '6', 0, '2', 0, '4', 0 }
#define USB_INIT_APP_SERIAL_LEN         16

#define input(size)             (0x80 | (size))
#define output(size)            (0x90 | (size))
#define feature(size)           (0xb0 | (size))
#define collection(size)        (0xa0 | (size))
#define end_collection(size)    (0xc0 | (size))

/* Global items */
#define usage_page(size)        (0x04 | (size))
#define logical_minimum(size)   (0x14 | (size))
#define logical_maximum(size)   (0x24 | (size))
#define physical_minimum(size)  (0x34 | (size))
#define physical_maximum(size)  (0x44 | (size))
#define uint_exponent(size)     (0x54 | (size))
#define uint(size)              (0x64 | (size))
#define report_size(size)       (0x74 | (size))
#define report_id(size)         (0x84 | (size))
#define report_count(size)      (0x94 | (size))
#define push(size)              (0xa4 | (size))
#define pop(size)               (0xb4 | (size))

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
#define delimiter(size)             (0xa8 | (size))

static bool g_usb_inited = false;

static uint8_t g_report_desc_hid[ ] = {
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

    usage_page(1),      0x01,
    usage(1),           0x02,
    collection(1),      0x01,
    report_id(1),       0x02,

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
    usage_page(2), 0xB1, 0xFF,
    usage(1),           0x1,
    collection(1),      0x01,
    report_id(1),       0x08,
    collection(1),      0x00,
    report_count(1),    0xc,
    report_size(1),     0x8,
    usage_minimum(1),   0x0,
    usage_maximum(1),   0xFF,
    logical_minimum(1), 0,
    logical_maximum(1), 0xff,
    output(1),           2,
    end_collection(0),
    end_collection(0),
};

int usb_init_keyboard_app(device_type dtype)
{
    int usb_hid_index = -1;

    if (g_usb_inited == true) {
        return -1;
    }

    const char manufacturer[USB_INIT_APP_MANUFACTURER_LEN] = USB_INIT_APP_MANUFACTURER;
    struct device_string str_manufacturer = {
        .str = manufacturer,
        .len = USB_INIT_APP_MANUFACTURER_LEN
    };

    const char product[USB_INIT_APP_PRODUCT_LEN] = USB_INIT_APP_PRODUCT;
    struct device_string str_product = {
        .str = product,
        .len = USB_INIT_APP_PRODUCT_LEN
    };

    const char serial[USB_INIT_APP_SERIAL_LEN] = USB_INIT_APP_SERIAL;
    struct device_string str_serial_number = {
        .str = serial,
        .len = USB_INIT_APP_SERIAL_LEN
    };

    struct device_id dev_id = {
        .vendor_id = 0x1111,
        .product_id = 0x0009,
        .release_num = 0x0800
    };

    if (dtype == DEV_HID) {
        usb_hid_index = hid_add_report_descriptor(g_report_desc_hid, sizeof(g_report_desc_hid), 0);
    }

    if (usbd_set_device_info(dtype, &str_manufacturer, &str_product, &str_serial_number, dev_id) != 0) {
        return -1;
    }

    if (usb_init(DEVICE, dtype) != 0) {
        return -1;
    }

    g_usb_inited = true;
    return usb_hid_index;
}

int usb_deinit_app(void)
{
    if (g_usb_inited == false) {
        return 0;
    }
    (void)usb_deinit();
    g_usb_inited = false;
    return 0;
}
