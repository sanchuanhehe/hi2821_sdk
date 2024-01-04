/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: USB Microphone Sample Source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-08-12, Create file. \n
 */
#include <stdbool.h>
#include "cmsis_os2.h"
#include "soc_osal.h"
#include "osal_debug.h"
#include "osal_semaphore.h"
#include "common_def.h"
#include "app_init.h"
#include "securec.h"
#include "hal_dma.h"
#include "pdm.h"
#include "dma.h"
#include "tcxo.h"
#include "watchdog.h"

#include "vdt_codec/vdt_codec.h"
#include "vdt_usb/vdt_usb.h"
#include "vdt_pdm/vdt_pdm.h"

#define USB_VDT_TASK_PRIO (osPriority_t)(17)
#define USB_VDT_TASK_STACK_SIZE 0xC00
#define USB_VDT_TASK_DURATION_MS 2000
#define USB_MICPHONE_OFFSET_16 16
#define USB_MICPHONE_OFFSET_24 24
#define USB_MICPHONE_ONCE_TRANSFER_LENGTH (CONFIG_USB_PDM_TRANSFER_LEN_OF_DMA * 2)

static void usb_vdt_dma_transfer_done_callback(uint8_t intr, uint8_t channel, uintptr_t arg);

static uint32_t g_pcm_dma_data[CONFIG_USB_PDM_TRANSFER_LEN_OF_DMA] = { 0 };
static uint8_t g_pcm_buffer[CONFIG_USB_UAC_MAX_RECORD + 2] = { 0 };
static uint8_t g_pcm_usb_buffer[CONFIG_USB_UAC_MAX_RECORD + 2] = { 0 };
static uint32_t g_pcm_usb_buffer_filled_count = 0;
static uint32_t g_vdt_dma_cnt = 0;
static uint32_t g_vdt_trans_cnt = 0;

static void usb_vdt_dma_transfer_restart(void)
{
    for (uint32_t i = 0; i < CONFIG_USB_PDM_TRANSFER_LEN_OF_DMA; i++) {
        g_pcm_usb_buffer[g_pcm_usb_buffer_filled_count++] = (uint8_t)(g_pcm_dma_data[i] >> USB_MICPHONE_OFFSET_16);
        g_pcm_usb_buffer[g_pcm_usb_buffer_filled_count++] = (uint8_t)(g_pcm_dma_data[i] >> USB_MICPHONE_OFFSET_24);
    }
    g_vdt_dma_cnt++;
    if (vdt_pdm_start_dma_transfer(g_pcm_dma_data, usb_vdt_dma_transfer_done_callback) != 0) {
        return;
    }
}

static void usb_vdt_dma_transfer_done_callback(uint8_t intr, uint8_t channel, uintptr_t arg)
{
    unused(channel);
    unused(arg);

    switch (intr) {
        case HAL_DMA_INTERRUPT_TFR:
            usb_vdt_dma_transfer_restart();
            break;
        case HAL_DMA_INTERRUPT_ERR:
            osal_printk("%s DMA transfer error.\r\n", VDT_USB_LOG);
            break;
        default:
            break;
    }
}

static void *usb_vdt_task(const char *arg)
{
    unused(arg);

    if (vdt_usb_uac_init() != 0) {
        osal_printk("%s Init the USB UAC fail.\r\n", VDT_USB_LOG);
    }
    if (vdt_pdm_init() != 0) {
        osal_printk("%s Init the PDM fail.\r\n", VDT_USB_LOG);
    }

    if (uapi_pdm_start() != ERRCODE_SUCC) {
        osal_printk("%s Start the PDM fail.\r\n", VDT_USB_LOG);
    }

    osDelay(USB_VDT_TASK_DURATION_MS);

    uapi_dma_init();
    uapi_dma_open();
    vdt_codec_init();

    if (vdt_pdm_start_dma_transfer(g_pcm_dma_data, usb_vdt_dma_transfer_done_callback) != 0) {
        return NULL;
    }

    while (1) {
        if (g_vdt_trans_cnt >= g_vdt_dma_cnt) {
            uapi_watchdog_kick();
            continue;
        }

        uint8_t *enc_output_data = NULL;
        uint8_t *output_pcm_data = NULL;
        vdt_codec_encode(g_pcm_usb_buffer + (g_vdt_trans_cnt * USB_MICPHONE_ONCE_TRANSFER_LENGTH),
                         &enc_output_data);
        vdt_codec_decode(enc_output_data, &output_pcm_data);
        memcpy_s(g_pcm_buffer + (g_vdt_trans_cnt * USB_MICPHONE_ONCE_TRANSFER_LENGTH),
                 USB_MICPHONE_ONCE_TRANSFER_LENGTH, output_pcm_data, USB_MICPHONE_ONCE_TRANSFER_LENGTH);
        g_vdt_trans_cnt++;
        if (g_pcm_usb_buffer_filled_count < CONFIG_USB_UAC_MAX_RECORD) {
            continue;
        }

        if ((vdt_usb_uac_send_data(g_pcm_buffer, CONFIG_USB_UAC_MAX_RECORD) != 0)) {
            osal_printk("%s Send UAV to USB fail.\r\n", VDT_USB_LOG);
            continue;
        }

        g_vdt_dma_cnt = 0;
        g_vdt_trans_cnt = 0;
        g_pcm_usb_buffer_filled_count = 0;
    }

    return NULL;
}

static void usb_vdt_entry(void)
{
    osThreadAttr_t attr;

    attr.name = "USBMicrophoneTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = USB_VDT_TASK_STACK_SIZE;
    attr.priority = USB_VDT_TASK_PRIO;
    if (osThreadNew((osThreadFunc_t)usb_vdt_task, NULL, &attr) == NULL) {
        /* Create task fail. */
    }
}

/* Run the usb_vdt_entry. */
app_run(usb_vdt_entry);