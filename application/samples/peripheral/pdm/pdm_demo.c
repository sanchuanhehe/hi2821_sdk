/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: PDM Sample Source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-08-25, Create file. \n
 */
#include "pinctrl.h"
#include "pdm.h"
#include "hal_pdm.h"
#include "dma.h"
#include "hal_dma.h"
#include "dma_porting.h"
#include "osal_debug.h"
#include "cmsis_os2.h"
#include "app_init.h"

#define PDM_USE_DMIC_CHANNEL        2
#define PDM_DMA_PRIORITY            3
#define PDM_OFFSET_16               16
#define PDM_OFFSET_24               24

#define PDM_TASK_STACK_SIZE         0x1000
#define PDM_TASK_PRIO               (osPriority_t)(17)

static uint32_t g_app_pdm_dma_data[CONFIG_PDM_TRANSFER_LEN_BY_DMA] = { 0 };
static uint8_t g_app_pdm_buffer[CONFIG_PDM_MAX_RECORD_DATA] = { 0 };
static uint32_t g_app_pdm_buffer_filled_count = 0;
static uint8_t g_dma_trans_done = 0;

static void app_pdm_init_pin(void)
{
    /* PDM pinmux. */
    uapi_pin_set_mode(CONFIG_PDM_D0_PIN, CONFIG_PDM_PIN_MODE);
    uapi_pin_set_mode(CONFIG_PDM_CLK0_PIN, CONFIG_PDM_PIN_MODE);
}

static void app_pdm_set_config(void)
{
    pdm_mic_t mic = PDM_USE_DMIC_CHANNEL;
    pdm_config_t config = { 0 };

    config.fs_ctrl_freq = PDM_MIC_FRE_8K;
    config.linear_select = 0;
    config.zero_num = 0x14;
    config.threshold_id = 0;
    config.noise_enable = 0;
    config.pga_bypass_enable = 0;
    config.fade_out_time = 0;
    config.fade_in_time = 0;
    config.little_signal = 0;
    config.anti_clip = 0;
    config.fade_in_out = 0;
    config.pga_gain = 0x28;
    config.srcdn_src_mode = DOUBLE_EXTRACT;

    if (uapi_pdm_set_attr(mic, &config) == ERRCODE_SUCC) {
        osal_printk("pdm config attr succ!\r\n");
    }
}

int32_t app_pdm_start_dma_transfer(uint32_t *pdm_buffer, dma_transfer_cb_t trans_done, dma_channel_t *channel)
{
    dma_ch_user_peripheral_config_t transfer_config;

    transfer_config.src = uapi_pdm_get_fifo_addr();
    transfer_config.dest = (uint32_t)(uintptr_t)pdm_buffer;
    transfer_config.transfer_num = CONFIG_PDM_TRANSFER_LEN_BY_DMA;
    transfer_config.src_handshaking = 0x6;
    transfer_config.dest_handshaking = 0;
    transfer_config.trans_type = HAL_DMA_TRANS_PERIPHERAL_TO_MEMORY_DMA;
    transfer_config.trans_dir = HAL_DMA_TRANSFER_DIR_PERIPHERAL_TO_MEM;
    transfer_config.priority = PDM_DMA_PRIORITY;
    transfer_config.src_width = HAL_DMA_TRANSFER_WIDTH_32;
    transfer_config.dest_width = HAL_DMA_TRANSFER_WIDTH_32;
    transfer_config.burst_length = 1;
    transfer_config.src_increment = HAL_DMA_ADDRESS_INC_NO_CHANGE;
    transfer_config.dest_increment = HAL_DMA_ADDRESS_INC_INCREMENT;
    transfer_config.protection = HAL_DMA_PROTECTION_CONTROL_BUFFERABLE;

    if (uapi_dma_configure_peripheral_transfer_single(&transfer_config, channel, trans_done,
                                                      (uintptr_t)NULL) != ERRCODE_SUCC) {
        osal_printk("DMA configure fail.\r\n");
        return 1;
    }

    if (uapi_dma_start_transfer((uint8_t)*channel) != ERRCODE_SUCC) {
        osal_printk("DMA start transfer fail.\r\n");
        return 1;
    }

    return 0;
}

static void app_dma_transfer_done_callback(uint8_t int_type, uint8_t channel, uintptr_t arg)
{
    unused(arg);
    unused(channel);
    switch (int_type) {
        case HAL_DMA_INTERRUPT_TFR:
            g_dma_trans_done = 1;
            break;
        case HAL_DMA_INTERRUPT_BLOCK:
            g_dma_trans_done = 1;
            break;
        case HAL_DMA_INTERRUPT_ERR:
            osal_printk("DMA transfer error.\r\n");
            break;
        default:
            break;
    }
}

static void *pdm_task(const char *arg)
{
    unused(arg);
    dma_channel_t channel;
    app_pdm_init_pin();
    /* PDM init. */
    uapi_pdm_init();

    app_pdm_set_config();

    if (uapi_pdm_start() == ERRCODE_SUCC) {
        osal_printk("pdm start transfer succ!\r\n");
    }

    uapi_dma_init();
    uapi_dma_open();

    while (1) {
        g_dma_trans_done = 0;
        if (app_pdm_start_dma_transfer(g_app_pdm_dma_data, app_dma_transfer_done_callback, &channel) != 0) {
            continue;
        }
        while (!g_dma_trans_done) { osal_printk("wait!!\r\n"); }
        uapi_dma_end_transfer(channel);
        for (uint32_t i = 0; i < CONFIG_PDM_TRANSFER_LEN_BY_DMA; i++) {
            g_app_pdm_buffer[g_app_pdm_buffer_filled_count++] = (uint8_t)(g_app_pdm_dma_data[i] >> PDM_OFFSET_16);
            g_app_pdm_buffer[g_app_pdm_buffer_filled_count++] = (uint8_t)(g_app_pdm_dma_data[i] >> PDM_OFFSET_24);
        }
        if (g_app_pdm_buffer_filled_count < CONFIG_PDM_MAX_RECORD_DATA) {
            osDelay(1);
            continue;
        } else {
            osal_printk("The current number of records in PDM exceeds the maximum allowed limit,addr:0x%x\r\n",
                        (uintptr_t)(&g_app_pdm_buffer));
            uapi_pdm_stop();
            return NULL;
        }
    }

    return NULL;
}

static void pdm_entry(void)
{
    osThreadAttr_t attr;

    attr.name = "PdmTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = PDM_TASK_STACK_SIZE;
    attr.priority = PDM_TASK_PRIO;

    if (osThreadNew((osThreadFunc_t)pdm_task, NULL, &attr) == NULL) {
        /* Create task fail. */
    }
}

/* Run the pdm_entry. */
app_run(pdm_entry);