/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Microphone USB Source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-08-12, Create file. \n
 */
#include "osal_debug.h"
#include "tcxo.h"
#include "gadget/f_uac.h"
#include "implementation/usb_init.h"
#include "vdt_usb.h"

#define UAC_MANUFACTURER        { 'H', 0, 'H', 0, 'H', 0, 'H', 0, 'l', 0, 'i', 0, 'c', 0, 'o', 0, 'n', 0 }
#define UAC_MANUFACTURER_LEN    20
#define UAC_PRODUCT             { 'H', 0, 'H', 0, '6', 0, '6', 0, '6', 0, '6', 0, ' ', 0, 'U', 0, 'S', 0, 'B', 0 }
#define UAC_PRODUCT_LEN         22
#define UAC_SERIAL              { '2', 0, '0', 0, '2', 0, '0', 0, '0', 0, '6', 0, '2', 0, '4', 0 }
#define UAC_SERIAL_LEN          16

int32_t vdt_usb_uac_init(void)
{
    const char manufacturer[UAC_MANUFACTURER_LEN] = UAC_MANUFACTURER;
    struct device_string str_manufacturer = {
        .str = manufacturer,
        .len = UAC_MANUFACTURER_LEN
    };

    const char product[UAC_PRODUCT_LEN] = UAC_PRODUCT;
    struct device_string str_product = {
        .str = product,
        .len = UAC_PRODUCT_LEN
    };

    const char serial[UAC_SERIAL_LEN] = UAC_SERIAL;
    struct device_string str_serial_number = {
        .str = serial,
        .len = UAC_SERIAL_LEN
    };

    struct device_id dev_uac_id = {
        .vendor_id = 0x1111,
        .product_id = 0x0009,
        .release_num = 0x0318
    };

    uint32_t ret = usbd_set_device_info(DEV_UAC, &str_manufacturer, &str_product, &str_serial_number, dev_uac_id);
    if (ret != 0) {
        osal_printk("%s set device info fail!\n", VDT_USB_LOG);
        return 1;
    }

    ret = usb_init(DEVICE, DEV_UAC);
    if (ret != 0) {
        osal_printk("%s usb_init failed!\n", VDT_USB_LOG);
        return 1;
    }

    ret = (uint32_t)uac_wait_host(UAC_WAIT_HOST_FOREVER);
    if (ret == 0) {
        osal_printk("%s host connectted, start test\r\n", VDT_USB_LOG);
    } else {
        osal_printk("%s host can`t connect\r\n", VDT_USB_LOG);
        return 1;
    }
    return 0;
}

int32_t vdt_usb_uac_send_data(const uint8_t *buf, int len)
{
    return fuac_send_message((void *)(uintptr_t)buf, len);
}