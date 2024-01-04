/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Test i2s source \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-03-10 Create file. \n
 */

#include "osal_debug.h"
#include "test_suite.h"
#include "test_suite_errors.h"
#include "hal_sio.h"
#include "tcxo.h"
#include "i2s.h"
#include "hal_sio.h"
#include "test_suite_uart.h"
#include "osal_addr.h"
#include "securec.h"
#include "dma.h"
#include "hal_dma.h"
#include "watchdog.h"
#include "test_i2s.h"

#define DATA_WIDTH              24
#define NUMBER_OF_CHANNELS      2
#define TX_TOTAL_NUM            64
#define WRITE_NUM_FIRST         16

static uint64_t g_time_before = 0;
static uint32_t g_left_data[TX_TOTAL_NUM];
static uint32_t g_right_data[TX_TOTAL_NUM];
static i2s_tx_data_t g_write_data = {
    .left_buff = g_left_data,
    .right_buff = g_right_data,
    .length = TX_TOTAL_NUM,
};

void i2s_rx_callbcak(uint32_t *left_buff, uint32_t *right_buff, uint32_t length)
{
    UNUSED(left_buff);
    UNUSED(right_buff);
    i2s_rx_data_t i2s_read_data = { 0 };
    uapi_i2s_get_data(SIO_BUS_0, &i2s_read_data);
    for (uint32_t i = 0; i < length; i++) {
        osal_printk("\r\nleft rx data is:%0x", i2s_read_data.left_buff[i]);
        osal_printk("\r\nright rx data is:%0x", i2s_read_data.right_buff[i]);
    }
    g_time_before = uapi_tcxo_get_us();
}

static int test_i2s_loop_trans(int argc, char *argv[])
{
    UNUSED(argc);
    UNUSED(argv);
    i2s_config_t config = {
        .drive_mode= MASTER,
        .transfer_mode = STD_MODE,
        .data_width = TWENTY_FOUR_BIT,
        .channels_num = TWO_CH,
        .timing = NONE_MODE,
        .clk_edge = NONE_EDGE,
        .div_number = DATA_WIDTH,
        .number_of_channels = NUMBER_OF_CHANNELS,
    };
    uint32_t g_first_i2s_data = 0x100000;    // The 24-bit width starting address sent by i2s
    for (uint32_t i = 0; i < TX_TOTAL_NUM; i++) {
        g_left_data[i] = g_first_i2s_data;
        g_right_data[i] = g_first_i2s_data;
        g_first_i2s_data++;
    }
    uapi_i2s_set_config(SIO_BUS_0, &config);
    uapi_i2s_loop_trans(SIO_BUS_0, &g_write_data);
    return TEST_SUITE_OK;
}

static int test_i2s_write(int argc, char *argv[])
{
    UNUSED(argc);
    UNUSED(argv);
    i2s_config_t config = {
        .drive_mode= MASTER,
        .transfer_mode = STD_MODE,
        .data_width = TWENTY_FOUR_BIT,
        .channels_num = TWO_CH,
        .timing = NONE_MODE,
        .clk_edge = NONE_EDGE,
        .div_number = DATA_WIDTH,
        .number_of_channels = NUMBER_OF_CHANNELS,
    };
    uint32_t g_first_i2s_data = 0x100000;
    for (uint32_t i = 0; i < TX_TOTAL_NUM; i++) {
        g_left_data[i] = g_first_i2s_data;
        g_right_data[i] = g_first_i2s_data;
        g_first_i2s_data++;
    }
    uapi_i2s_set_config(SIO_BUS_0, &config);
    uint64_t time_before = uapi_tcxo_get_us();
    uapi_i2s_write_data(SIO_BUS_0, &g_write_data);
    osal_printk("uapi i2s write 16 data time: %uus, length: %u\r\n",
                (uint32_t)(uapi_tcxo_get_us() - time_before), WRITE_NUM_FIRST);
    return TEST_SUITE_OK;
}

static int test_i2s_read(int argc, char *argv[])
{
    UNUSED(argc);
    UNUSED(argv);
    i2s_config_t config = {
        .drive_mode= SLAVE,
        .transfer_mode = STD_MODE,
        .data_width = TWENTY_FOUR_BIT,
        .channels_num = TWO_CH,
        .timing = NONE_MODE,
        .clk_edge = NONE_EDGE,
        .div_number = DATA_WIDTH,
        .number_of_channels = NUMBER_OF_CHANNELS,
    };
    uapi_i2s_set_config(SIO_BUS_0, &config);
    g_time_before = uapi_tcxo_get_us();
    uapi_i2s_read_start(SIO_BUS_0);
    return TEST_SUITE_OK;
}

#if defined(CONFIG_I2S_SUPPORT_DMA)
#define I2S_DMA_WRITE_SIZE 1024
uint32_t g_i2s_w_buf[I2S_DMA_WRITE_SIZE];
static int test_i2s_dma_write(int argc, char *argv[])
{
    if (argc != 1) {
        return TEST_SUITE_ERROR_BAD_PARAMS;
    }
    uint16_t len = (uint16_t)strtol(argv[0], NULL, 0);
    i2s_dma_config_t dma_cfg = {
        .src_width = 2,
        .dest_width = 2,
        .burst_length = 0,
        .priority = 0,
    };
    memset_s(g_i2s_w_buf, sizeof(uint32_t) * I2S_DMA_WRITE_SIZE, 1, sizeof(uint32_t) * I2S_DMA_WRITE_SIZE);
    if (uapi_i2s_merge_write_by_dma(SIO_BUS_0, g_i2s_w_buf, len, &dma_cfg, (uintptr_t)NULL, true) < 0) {
        return TEST_SUITE_TEST_FAILED;
    }
    osal_printk("i2s_dma_write ok\n");
    return TEST_SUITE_OK;
}
#define I2S_DMA_READ_SIZE 1024
#define I2S_DMA_READ_EXTRA_LEN 16
#define TEST_SUIT_PARAMS_NUM_2 2
uint32_t g_i2s_dma_buf[I2S_DMA_READ_SIZE] = { 0 };
static int test_i2s_dma_read(int argc, char *argv[])
{
    if (argc != TEST_SUIT_PARAMS_NUM_2) {
        return TEST_SUITE_ERROR_BAD_PARAMS;
    }
    uint16_t len = (uint16_t)strtol(argv[0], NULL, 0);
    uint16_t burst = (uint16_t)strtol(argv[1], NULL, 0);
    i2s_dma_config_t dma_cfg = {
        .src_width = 2,
        .dest_width = 2,
        .burst_length = burst,
        .priority = 0,
    };
    PRINT("len %u, burst %u\r\n", len, burst);
    int32_t ret = uapi_i2s_merge_read_by_dma(SIO_BUS_0, g_i2s_dma_buf, len, &dma_cfg, (uintptr_t)NULL, true);
    if (ret < 0) {
        return TEST_SUITE_TEST_FAILED;
    }
    PRINT("receive data len %u\r\n", ret);
    for (uint32_t i = 0; i < len; i++) {
        PRINT("%u\r\n", g_i2s_dma_buf[i]);
    }
    return TEST_SUITE_OK;
}

#if defined(CONFIG_SIO_USING_V151)
void hal_sio_v151_txrx_disable(sio_bus_t bus);
void hal_sio_v151_txrx_enable(sio_bus_t bus);
#endif
static int test_i2s_dma_rw(int argc, char *argv[])
{
    if (argc != 1) {
        return TEST_SUITE_ERROR_BAD_PARAMS;
    }
    i2s_dma_config_t dma_cfg = {
        .src_width = 2,
        .dest_width = 2,
        .burst_length = 0,
        .priority = 0,
    };
    uint16_t len = (uint16_t)strtol(argv[0], NULL, 0);
    PRINT("len %u\r\n", len);
#if defined(CONFIG_SIO_USING_V151)
    hal_sio_v151_txrx_disable(0);
#endif
    memset_s(g_i2s_w_buf, sizeof(uint32_t) * I2S_DMA_WRITE_SIZE, 1, sizeof(uint32_t) * I2S_DMA_WRITE_SIZE);
    uapi_i2s_merge_write_by_dma(SIO_BUS_0, g_i2s_w_buf, len, &dma_cfg, (uintptr_t)NULL, false);
    memset_s(g_i2s_dma_buf, I2S_DMA_READ_SIZE * sizeof(uint32_t), 0, I2S_DMA_READ_SIZE * sizeof(uint32_t));
    uapi_i2s_merge_read_by_dma(SIO_BUS_0, g_i2s_dma_buf, len, &dma_cfg, (uintptr_t)NULL, false);
    PRINT("dma_rw ok\r\n");
#if defined(CONFIG_SIO_USING_V151)
    hal_sio_v151_txrx_enable(0);
#endif
    return TEST_SUITE_OK;
}

#define I2S_DMA_SHOW_DATA_STEP 2
static int test_i2s_dma_show(int argc, char *argv[])
{
    if (argc != 1) {
        return TEST_SUITE_ERROR_BAD_PARAMS;
    }
    uint16_t len = (uint16_t)strtol(argv[0], NULL, 0);
    uint16_t i = 0;
    for (; i < len + I2S_DMA_READ_EXTRA_LEN; i += I2S_DMA_SHOW_DATA_STEP) {
        osal_printk("[%u]:%x %x\r\n", i, g_i2s_dma_buf[i], g_i2s_dma_buf[i+1]);
    }
    return TEST_SUITE_OK;
}

#endif

static int test_i2s_set_loop(int argc, char *argv[])
{
    UNUSED(argc);
    UNUSED(argv);
    uapi_i2s_loopback(SIO_BUS_0, true);
    return TEST_SUITE_OK;
}

static int test_i2s_init(int argc, char *argv[])
{
    UNUSED(argc);
    UNUSED(argv);
    uapi_i2s_init(SIO_BUS_0, i2s_rx_callbcak);
    sio_porting_i2s_pinmux();
    return TEST_SUITE_OK;
}

void add_i2s_test_case(void)
{
    uapi_test_suite_add_function("test_i2s_init", "i2s init.", test_i2s_init);
    uapi_test_suite_add_function("test_i2s_set_loop", "i2s set loop mode.", test_i2s_set_loop);
    uapi_test_suite_add_function(
        "test_i2s_loop_trans", "i2s loop trans, it must run test_i2s_set_loop first.", test_i2s_loop_trans);
    uapi_test_suite_add_function("test_i2s_write", "i2s write.", test_i2s_write);
    uapi_test_suite_add_function("test_i2s_read", "i2s read.", test_i2s_read);
#if defined(CONFIG_I2S_SUPPORT_DMA)
    test_i2s_init(0, 0);
    uapi_dma_deinit();
    uapi_dma_init();
    uapi_dma_open();
    i2s_dma_attr_t attr = {
        .tx_dma_enable = 1,
        .tx_int_threshold = 8,
        .rx_dma_enable = 1,
        .rx_int_threshold = 8,
    };
    i2s_config_t config = {
        .drive_mode= MASTER,
        .transfer_mode = STD_MODE,
        .data_width = TWENTY_FOUR_BIT,
        .channels_num = TWO_CH,
        .timing = NONE_MODE,
        .clk_edge = NONE_EDGE,
        .div_number = DATA_WIDTH,
        .number_of_channels = NUMBER_OF_CHANNELS,
    };
    uapi_i2s_set_config(SIO_BUS_0, &config);
    uapi_i2s_dma_config(SIO_BUS_0, &attr);
    uapi_i2s_loopback(SIO_BUS_0, true);
    add_function("test_i2s_dma_write", "i2s dma write case.", test_i2s_dma_write);
    add_function("test_i2s_dma_read", "i2s read case.", test_i2s_dma_read);
    add_function("test_i2s_dma_rw", "test_i2s_dma_rw.", test_i2s_dma_rw);
    add_function("test_i2s_dma_show", "test_i2s_dma_show.", test_i2s_dma_show);
#endif
}