/*
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: USB port for project
 * Author: @CompanyNameTag
 * Create:  2023-06-28
 */

#include "stdint.h"
#include "stdlib.h"
#include "gadget/f_hid.h"
#include "osal_interrupt.h"
#include "usb_porting.h"

#define USB_MEM_LEN  (1024 - 128)
uint64_t g_usb_mem[USB_MEM_LEN];
uintptr_t g_usb_mem_addr_start  = (uintptr_t)&g_usb_mem;
unsigned long g_usb_mem_size  = sizeof(g_usb_mem);

usb_sof_cb g_sof_cb = NULL;

void usb_register_callback(usb_sof_cb sof_cb)
{
    uint32_t ret = osal_irq_lock();
    g_sof_cb = sof_cb;
    osal_irq_restore(ret);
}

void usb_unregister_callback(void)
{
    uint32_t ret = osal_irq_lock();
    g_sof_cb = NULL;
    osal_irq_restore(ret);
}

void usb_sof_intr_callback(void)
{
    uint8_t *data = NULL;
    uint16_t len = 0;
    uint8_t device_index = 0;
    if (g_sof_cb == NULL) {
        return;
    }

    g_sof_cb(&data, &len, &device_index);
    // It will not send data if len == 0 or data == NULL
    fhid_send_data(device_index, (char *)data, len);
}
