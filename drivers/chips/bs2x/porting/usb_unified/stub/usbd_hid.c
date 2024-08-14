/*
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: USB HID Class Source.
 * Author:  @CompanyNameTag
 * Create:  2023-03-29
 */
#include "common_def.h"
#include "usbd_hid.h"

bool usb_hid_send_report(uint8_t *data, uint16_t length, uint8_t interface_index)
{
    unused(data);
    unused(length);
    unused(interface_index);
    return true;
}