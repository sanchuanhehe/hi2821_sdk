/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: i2s Sample Source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-09-18, Create file. \n
 */

#include "osal_debug.h"
#include "tcxo.h"
#include "i2s.h"
#include "hal_sio.h"
#include "securec.h"
#include "dma.h"
#include "hal_dma.h"
#include "watchdog.h"
#include "cmsis_os2.h"
#include "app_init.h"

#define I2S_TASK_STACK_SIZE         0xc00
#define I2S_TASK_PRIO               (osPriority_t)(17)
#define DATA_WIDTH                  24
#define NUMBER_OF_CHANNELS          2

#define I2S_TRANS_SRC_ADDR          0x520300a0

static uint32_t g_i2s_receive_dma_data[CONFIG_I2S_TRANSFER_LEN_OF_DMA] = { 0 };
static uint32_t g_i2s_receive_buffer[CONFIG_I2S_DMA_MAX_RECORD + 2] = { 0 };
static uint32_t g_i2s_receive_buffer_filled_count = 0;
static uint32_t g_i2s_receive_dma_cnt = 0;
static uint32_t g_i2s_receive_trans_cnt = 0;

static void i2s_dma_transfer_restart(void);

static int32_t i2s_start_dma_transfer(uint32_t *i2s_buffer, dma_transfer_cb_t trans_done)
{
    dma_ch_user_peripheral_config_t transfer_config;
    uint8_t channel = 0;

    unused(transfer_config);

    transfer_config.src = I2S_TRANS_SRC_ADDR;
    transfer_config.dest = (uint32_t)(uintptr_t)i2s_buffer;
    transfer_config.transfer_num = (uint16_t)CONFIG_I2S_TRANSFER_LEN_OF_DMA;
    transfer_config.src_handshaking = 0xc;
    transfer_config.dest_handshaking = 0;
    transfer_config.trans_type = HAL_DMA_TRANS_PERIPHERAL_TO_MEMORY_DMA;
    transfer_config.trans_dir = HAL_DMA_TRANSFER_DIR_PERIPHERAL_TO_MEM;
    transfer_config.priority = 0;
    transfer_config.src_width = HAL_DMA_TRANSFER_WIDTH_32;
    transfer_config.dest_width = HAL_DMA_TRANSFER_WIDTH_32;
    transfer_config.burst_length = 0;
    transfer_config.src_increment = HAL_DMA_ADDRESS_INC_NO_CHANGE;
    transfer_config.dest_increment = HAL_DMA_ADDRESS_INC_INCREMENT;
    transfer_config.protection = HAL_DMA_PROTECTION_CONTROL_BUFFERABLE;

    errcode_t ret = uapi_dma_configure_peripheral_transfer_single(&transfer_config, &channel,
                                                                  trans_done, (uintptr_t)NULL);
    if (ret != ERRCODE_SUCC) {
        osal_printk("%s Configure the DMA fail. %x\r\n", "i2s dma", ret);
        return 1;
    }
    ret = uapi_dma_start_transfer(channel);
    if (ret != ERRCODE_SUCC) {
        osal_printk("%s Start the DMA fail. %x\r\n", "i2s dma", ret);
        return 1;
    }

    return 0;
}

static void i2s_dma_trans_done_callback(uint8_t intr, uint8_t channel, uintptr_t arg)
{
    unused(channel);
    unused(arg);
    switch (intr) {
        case HAL_DMA_INTERRUPT_TFR:
            i2s_dma_transfer_restart();
            break;
        case HAL_DMA_INTERRUPT_ERR:
            osal_printk("i2s DMA transfer error.\r\n");
            break;
        default:
            break;
    }
}

static void i2s_dma_transfer_restart(void)
{
    for (uint32_t i = 0; i < CONFIG_I2S_TRANSFER_LEN_OF_DMA; i++) {
        g_i2s_receive_buffer[g_i2s_receive_buffer_filled_count++] = g_i2s_receive_dma_data[i];
    }
    g_i2s_receive_dma_cnt++;
    if (i2s_start_dma_transfer(g_i2s_receive_dma_data, i2s_dma_trans_done_callback) != 0) {
        return;
    }
}

static void *i2s_task(const char *arg)
{
    unused(arg);

    uapi_i2s_init(SIO_BUS_0, NULL);
    i2s_config_t config = {
        .drive_mode= SLAVE,
        .transfer_mode = STD_MODE,
        .data_width = TWENTY_FOUR_BIT,
        .channels_num = TWO_CH,
        .timing = NONE_MODE,
        .clk_edge = FALLING_EDGE,
        .div_number = DATA_WIDTH,
        .number_of_channels = NUMBER_OF_CHANNELS,
    };
    uapi_i2s_set_config(SIO_BUS_0, &config);
    sio_porting_i2s_pinmux();

    i2s_dma_attr_t attr = {
        .tx_dma_enable = 1,
        .tx_int_threshold = 8,
        .rx_dma_enable = 1,
        .rx_int_threshold = 8,
    };
    uapi_i2s_dma_config(SIO_BUS_0, &attr);

    /* DMA init. */
    uapi_dma_init();
    uapi_dma_open();
    if (i2s_start_dma_transfer(g_i2s_receive_dma_data, i2s_dma_trans_done_callback) != 0) {
        return NULL;
    }
    osal_printk("DMA transfer start.\r\n");
    while (1) {
        if (g_i2s_receive_trans_cnt >= g_i2s_receive_dma_cnt) {
            uapi_watchdog_kick();
            continue;
        }
        g_i2s_receive_trans_cnt++;
        if (g_i2s_receive_buffer_filled_count < CONFIG_I2S_DMA_MAX_RECORD) {
            continue;
        }

        g_i2s_receive_trans_cnt = 0;
        g_i2s_receive_dma_cnt = 0;
        g_i2s_receive_buffer_filled_count = 0;
    }

    return NULL;
}

static void i2s_entry(void)
{
    osThreadAttr_t attr;

    attr.name = "I2sTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = I2S_TASK_STACK_SIZE;
    attr.priority = I2S_TASK_PRIO;

    if (osThreadNew((osThreadFunc_t)i2s_task, NULL, &attr) == NULL) {
        /* Create task fail. */
    }
}

/* Run the i2s_entry. */
app_run(i2s_entry);