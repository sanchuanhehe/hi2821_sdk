/**
 * Copyright (c) @CompanyNameMagicTag 2022-2023. All rights reserved. \n
 *
 * Description: Test dfu source \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-08-11, Create file. \n
 */

#include <stdint.h>
#include "gadget/f_dfu.h"
#include "osal_debug.h"
#include "osal_task.h"
#include "securec.h"
#include "watchdog.h"
#ifdef USE_EMBED_FLASH
#include "eflash.h"
#endif
#include "implementation/usb_init.h"
#include "memory_config_common.h"
#include "uapi_crc.h"
#include "cpu_utils.h"

#define DFU_MANUFACTURER  { 'H', 0, 'H', 0, 'H', 0, 'H', 0, 'l', 0, 'i', 0, 'c', 0, 'o', 0, 'n', 0 }
#define DFU_MANUFACTURER_LEN   20
#define DFU_PRODUCT  { 'H', 0, 'H', 0, '6', 0, '6', 0, '6', 0, '6', 0, ' ', 0, 'U', 0, 'S', 0, 'B', 0 }
#define DFU_PRODUCT_LEN        22
#define DFU_SERIAL   { '2', 0, '0', 0, '2', 0, '0', 0, '0', 0, '6', 0, '2', 0, '4', 0 }
#define DFU_SERIAL_LEN         16
#define DFU_DELAY_MS           (200)
#define DFU_TIMEOUT_MS         (1000 * 3 * 60)
#define FRAME_DOWNLOAD_OFFSET  0x101000
#define CRC_VERIFY_BUFFER_LEN  25

static uintptr_t g_dfu_data = (uintptr_t)FRAME_DOWNLOAD_OFFSET;
static uint32_t g_dfu_write_len = 0;

typedef struct {
    uint32_t start_flag;
    uint16_t packet_size;
    uint8_t frame_type;
    uint8_t frame_type_reverse;
    uint32_t file_addr;
    uint32_t file_len;
    uint32_t erase_size;
    uint8_t formal;
    uint8_t formal_reverse;
    uint16_t checksum;
} dfu_packge_header_t;

int usb_dfu_init(void)
{
    int ret;
    const char manufacturer[DFU_MANUFACTURER_LEN] = DFU_MANUFACTURER;
    struct device_string str_manufacturer = {
        .str = manufacturer,
        .len = DFU_MANUFACTURER_LEN
    };

    const char product[DFU_PRODUCT_LEN] = DFU_PRODUCT;
    struct device_string str_product = {
        .str = product,
        .len = DFU_PRODUCT_LEN
    };

    const char serial[DFU_SERIAL_LEN] = DFU_SERIAL;
    struct device_string str_serial_number = {
        .str = serial,
        .len = DFU_SERIAL_LEN
    };

    struct device_id dev_id = {
        .vendor_id = 0x1111,
        .product_id = 0xa,
        .release_num = 0x0119
    };

    ret = usbd_set_device_info(DEV_DFU, &str_manufacturer, &str_product, &str_serial_number, dev_id);
    if (ret != 0) {
        osal_printk("set device info fail!\n");
        return -1;
    }

    ret = usb_init(DEVICE, DEV_DFU);
    if (ret != 0) {
        osal_printk("usb_init fail!\n");
        return -1;
    }

    return 0;
}

void usb_dfu_download_fail(void)
{
    osal_printk("upgrade fail\r\n");

#ifdef USE_EMBED_FLASH
    uapi_eflash_erase(FRAME_DOWNLOAD_OFFSET, g_dfu_write_len);
#endif
    cpu_utils_reset_chip_with_log((cores_t)APPS, REBOOT_CAUSE_DFU_UPG_FAIL);
}

void usb_dfu_wait_ugrade_done_and_reset(void)
{
    uint32_t count = 0;
    while (1) {
#ifdef CONFIG_DRIVERS_USB_DFU_GADGET
        if (usb_dfu_update_status() == 1) {
            osal_printk("dfu done\n");
            cpu_utils_reset_chip_with_log((cores_t)APPS, REBOOT_CAUSE_APPLICATION_SYSRESETREQ);
        }
#endif
        if (count > (DFU_TIMEOUT_MS / DFU_DELAY_MS)) {
            osal_printk("dfu overtime\n");
            usb_dfu_download_fail();
        }
        osal_msleep(DFU_DELAY_MS);
        count++;
    }
}

void usb_dfu_download_callback(const uint8_t *buf, uint32_t len)
{
    static bool is_pack_header = true;
    if (buf == NULL || len == 0) {
        osal_printk("upgrade done\r\n");
        return;
    }

    if (is_pack_header) {
        is_pack_header = false;
        return;
    }
    osal_printk("          buf:");
    for (int i = 0; i < DFU_SERIAL_LEN; i++) {
        osal_printk("%x  ", buf[i]);
    }
    osal_printk("\n");
#ifdef USE_EMBED_FLASH
    uapi_eflash_erase((uint32_t)g_dfu_data, len);
    uapi_eflash_write((uint32_t)g_dfu_data, (uint32_t *)(uintptr_t)buf, len);
    g_dfu_write_len += len;
    if (uapi_crc16(0, buf, len) != uapi_crc16(0, (uint8_t *)(g_dfu_data + EMBED_FLASH_START), len)) {
        osal_printk("check fail!\r\n");
        usb_dfu_download_fail();
    }
#endif
    for (int i = 0; i < DFU_SERIAL_LEN; i++) {
        osal_printk("%x  ", *(char *)(g_dfu_data + EMBED_FLASH_START + i));
    }
    osal_printk("\ng_dfu_write_len:%u  \n", g_dfu_write_len);
    uapi_watchdog_kick();
    g_dfu_data += len;
}
