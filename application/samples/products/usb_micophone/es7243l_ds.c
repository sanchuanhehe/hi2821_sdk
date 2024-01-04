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
#include "micophone_usb/micophone_usb.h"
#include "pinctrl.h"
#include "test_suite_task.h"
#include "test_suite_uart.h"

#include "gpio.h"
#include "hal_gpio.h"
#include "i2c.h"
#include "diag_cmd_get_mem_info.h"
#include "pm_clock.h"
#include "eflash.h"

#define I2C_MODE                    1                                           /* 1:master 0:slave */
#define TEST_I2C I2C_BUS_4
#define I2C_MASTER_ADDR             0x0
#define I2C_SLAVE_ADDR              0x10
#define I2C_BAUDRATE                100000                                      /* 100khz */
#define I2C_PIN_CLK_PINMUX          HAL_PIO_FUNC_I2C4_M2
#define I2C_PIN_DAT_PINMUX          HAL_PIO_FUNC_I2C4_M2
#define I2C_PIN_CLK S_MGPIO14
#define I2C_PIN_DAT S_MGPIO15
#define CONFIG_I2C_TRANSFER_LEN     8
#define TASKS_TEST_DURATION_MS      5000

#define I2S_TASK_STACK_SIZE         0xF00
#define I2S_TASK_PRIO               (osPriority_t)(32)
#define DATA_WIDTH                  32
#define NUMBER_OF_CHANNELS          2

#define USB_MICPHONE_OFFSET_0       0
#define USB_MICPHONE_OFFSET_8       8
#define USB_MICPHONE_OFFSET_16      16
#define USB_MICPHONE_OFFSET_24      24

#define I2S_TRANS_SRC_ADDR          0x520300a0
#define UAC_SEND_DATA_LEN           1920
#define UAC_FIFO_BUF_NUM            4
#define I2C_WRITE_DATA_LEN          2
#define FILLED_COUNT                2

static uint8_t g_dma_fifo_buf_index =  0 ;
static uint8_t g_uac_fifo_buf_index =  0 ;
static uint32_t g_receive_buff0[CONFIG_ES7243L_DS_TRANSFER_LEN_OF_DMA + 8] = { 0 };
static uint32_t g_receive_buff1[CONFIG_ES7243L_DS_TRANSFER_LEN_OF_DMA + 8] = { 0 };
static uint32_t g_receive_buff2[CONFIG_ES7243L_DS_TRANSFER_LEN_OF_DMA + 8] = { 0 };
static uint32_t g_receive_buff3[CONFIG_ES7243L_DS_TRANSFER_LEN_OF_DMA + 8] = { 0 };

static uint32_t *g_i2s_receive_dma_data[4] = {g_receive_buff0, g_receive_buff1, g_receive_buff2, g_receive_buff3};

static uint8_t g_i2s_receive_buffer[CONFIG_ES7243L_DS_MAX_RECORD + 9] = { 0 };
static uint32_t g_i2s_receive_buffer_filled_count = 0;
static uint32_t g_i2s_receive_dma_cnt = 0;
static uint32_t g_i2s_receive_trans_cnt = 0;

static void i2s_dma_transfer_restart(void);

errcode_t sample_i2c_init(void)
{
    uapi_pin_set_mode(S_MGPIO13, HAL_PIO_FUNC_GPIO);
    uapi_gpio_set_dir(S_MGPIO13, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(S_MGPIO13, 1);

    uapi_pin_set_mode(S_MGPIO13, HAL_PIO_FUNC_GPIO);
    uapi_gpio_set_dir(S_MGPIO13, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(S_MGPIO13, 1);

    uapi_pin_set_mode(I2C_PIN_CLK, I2C_PIN_CLK_PINMUX); /* 设置 i2c clk pinmux */
    uapi_pin_set_mode(I2C_PIN_DAT, I2C_PIN_DAT_PINMUX); /* 设置 i2c dat pinmux */
    /* 初始化 i2c */
    if (I2C_MODE == 1) {
        return uapi_i2c_master_init(TEST_I2C, I2C_BAUDRATE, I2C_MASTER_ADDR); /* 初始化 i2c0 */
    } else {
        return uapi_i2c_slave_init(TEST_I2C, I2C_BAUDRATE, I2C_SLAVE_ADDR); /* 初始化 i2c0 */
    }
}

errcode_t sample_i2c_write(uint8_t *data, uint8_t len)
{
    i2c_data_t i2c_send_data = { 0 };
    i2c_send_data.send_buf = data;                         /* 设置 tx buff */
    i2c_send_data.send_len = len;                          /* 设置 tx buff 长度 */
    if (I2C_MODE == 1) {
        return uapi_i2c_master_write(TEST_I2C, I2C_SLAVE_ADDR, &i2c_send_data); /* 发送数据 */
    } else {
        return uapi_i2c_slave_write(TEST_I2C, &i2c_send_data); /* 发送数据 */
    }
}

static int32_t i2s_start_dma_transfer(uint32_t *i2s_buffer, dma_transfer_cb_t trans_done)
{
    dma_ch_user_peripheral_config_t transfer_config;
    uint8_t channel = 0;

    unused(transfer_config);

    transfer_config.src = I2S_TRANS_SRC_ADDR;
    transfer_config.dest = (uint32_t)(uintptr_t)i2s_buffer;
    transfer_config.transfer_num = (uint16_t)CONFIG_ES7243L_DS_TRANSFER_LEN_OF_DMA;
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
    g_i2s_receive_dma_cnt++;
    g_dma_fifo_buf_index = (g_dma_fifo_buf_index + 1) % UAC_FIFO_BUF_NUM;
    if (i2s_start_dma_transfer(g_i2s_receive_dma_data[g_dma_fifo_buf_index], i2s_dma_trans_done_callback) != 0) {
        return;
    }
}

static void i2c_write_date1(void)
{
    /* I2C data config. */
    uint8_t tx_buff[CONFIG_I2C_TRANSFER_LEN] = {0};
    tx_buff[0] = 0x01;
    tx_buff[1] = 0x3A;
    sample_i2c_write(tx_buff, I2C_WRITE_DATA_LEN);
    tx_buff[0] = 0x00;
    tx_buff[1] = 0x80;
    sample_i2c_write(tx_buff, I2C_WRITE_DATA_LEN);
    tx_buff[0] = 0x04;
    tx_buff[1] = 0x03;
    sample_i2c_write(tx_buff, I2C_WRITE_DATA_LEN);
    tx_buff[0] = 0x04;
    tx_buff[1] = 0x03;
    sample_i2c_write(tx_buff, I2C_WRITE_DATA_LEN);
    tx_buff[0] = 0xF9;
    tx_buff[1] = 0x00;
    sample_i2c_write(tx_buff, I2C_WRITE_DATA_LEN);
    tx_buff[0] = 0x00;
    tx_buff[1] = 0x1E;
    sample_i2c_write(tx_buff, I2C_WRITE_DATA_LEN);
    tx_buff[0] = 0x01;
    tx_buff[1] = 0x00;
    sample_i2c_write(tx_buff, I2C_WRITE_DATA_LEN);
    tx_buff[0] = 0x02;
    tx_buff[1] = 0x80;
    sample_i2c_write(tx_buff, I2C_WRITE_DATA_LEN);
    tx_buff[0] = 0x03;
    tx_buff[1] = 0x20;
    sample_i2c_write(tx_buff, I2C_WRITE_DATA_LEN);
    tx_buff[0] = 0x0D;
    tx_buff[1] = 0x00;
    sample_i2c_write(tx_buff, I2C_WRITE_DATA_LEN);
    tx_buff[0] = 0xF9;
    tx_buff[1] = 0x00;
    sample_i2c_write(tx_buff, I2C_WRITE_DATA_LEN);
    tx_buff[0] = 0x04;
    tx_buff[1] = 0x03;
    sample_i2c_write(tx_buff, I2C_WRITE_DATA_LEN);
}

static void i2c_write_date2(void)
{
    /* I2C data config. */
    uint8_t tx_buff[CONFIG_I2C_TRANSFER_LEN] = {0};
    tx_buff[0] = 0x04;
    tx_buff[1] = 0x03;
    sample_i2c_write(tx_buff, I2C_WRITE_DATA_LEN);
    tx_buff[0] = 0x05;
    tx_buff[1] = 0x00;
    sample_i2c_write(tx_buff, I2C_WRITE_DATA_LEN);
    tx_buff[0] = 0x07;
    tx_buff[1] = 0x00;
    sample_i2c_write(tx_buff, I2C_WRITE_DATA_LEN);
    tx_buff[0] = 0x09;
    tx_buff[1] = 0xC5;
    sample_i2c_write(tx_buff, I2C_WRITE_DATA_LEN);
    tx_buff[0] = 0x0A;
    tx_buff[1] = 0x81;
    sample_i2c_write(tx_buff, I2C_WRITE_DATA_LEN);
    tx_buff[0] = 0x0B;
    tx_buff[1] = 0x10;
    sample_i2c_write(tx_buff, I2C_WRITE_DATA_LEN);
    tx_buff[0] = 0x0E;
    tx_buff[1] = 0xBF;
    sample_i2c_write(tx_buff, I2C_WRITE_DATA_LEN);
    tx_buff[0] = 0x0F;
    tx_buff[1] = 0xA0;
    sample_i2c_write(tx_buff, I2C_WRITE_DATA_LEN);
    tx_buff[0] = 0x10;
    tx_buff[1] = 0x18;
    sample_i2c_write(tx_buff, I2C_WRITE_DATA_LEN);
    tx_buff[0] = 0x11;
    tx_buff[1] = 0x16;
    sample_i2c_write(tx_buff, I2C_WRITE_DATA_LEN);
    tx_buff[0] = 0x14;
    tx_buff[1] = 0x0C;
    sample_i2c_write(tx_buff, I2C_WRITE_DATA_LEN);
    tx_buff[0] = 0x15;
    tx_buff[1] = 0x0C;
    sample_i2c_write(tx_buff, I2C_WRITE_DATA_LEN);
}

static void i2c_write_date3(void)
{
    /* I2C data config. */
    uint8_t tx_buff[CONFIG_I2C_TRANSFER_LEN] = {0};
    tx_buff[0] = 0x17;
    tx_buff[1] = 0x02;
    sample_i2c_write(tx_buff, I2C_WRITE_DATA_LEN);
    tx_buff[0] = 0x18;
    tx_buff[1] = 0x24;
    sample_i2c_write(tx_buff, I2C_WRITE_DATA_LEN);
    tx_buff[0] = 0x19;
    tx_buff[1] = 0x88;
    sample_i2c_write(tx_buff, I2C_WRITE_DATA_LEN);
    tx_buff[0] = 0x1A;
    tx_buff[1] = 0x88;
    sample_i2c_write(tx_buff, I2C_WRITE_DATA_LEN);
    tx_buff[0] = 0x1B;
    tx_buff[1] = 0x66;
    sample_i2c_write(tx_buff, I2C_WRITE_DATA_LEN);
    tx_buff[0] = 0x1C;
    tx_buff[1] = 0x44;
    sample_i2c_write(tx_buff, I2C_WRITE_DATA_LEN);
    tx_buff[0] = 0x1D;
    tx_buff[1] = 0xF0;
    sample_i2c_write(tx_buff, I2C_WRITE_DATA_LEN);
    tx_buff[0] = 0x1E;
    tx_buff[1] = 0x00;
    sample_i2c_write(tx_buff, I2C_WRITE_DATA_LEN);
    tx_buff[0] = 0x1F;
    tx_buff[1] = 0x0C;
    sample_i2c_write(tx_buff, I2C_WRITE_DATA_LEN);
    tx_buff[0] = 0x20;
    tx_buff[1] = 0x17;
    sample_i2c_write(tx_buff, I2C_WRITE_DATA_LEN);
    tx_buff[0] = 0x21;
    tx_buff[1] = 0x17;
    sample_i2c_write(tx_buff, I2C_WRITE_DATA_LEN);
    tx_buff[0] = 0x00;
    tx_buff[1] = 0x80;
    sample_i2c_write(tx_buff, I2C_WRITE_DATA_LEN);
    tx_buff[0] = 0x01;
    tx_buff[1] = 0x3A;
    sample_i2c_write(tx_buff, I2C_WRITE_DATA_LEN);
    tx_buff[0] = 0x16;
    tx_buff[1] = 0x3F;
    sample_i2c_write(tx_buff, I2C_WRITE_DATA_LEN);
    tx_buff[0] = 0x16;
    tx_buff[1] = 0x00;
    sample_i2c_write(tx_buff, I2C_WRITE_DATA_LEN);
}

static void i2s_dma_init(void)
{
    i2s_dma_attr_t attr = {
        .tx_dma_enable = 1,
        .tx_int_threshold = 8,
        .rx_dma_enable = 1,
        .rx_int_threshold = 1,
    };
    uapi_i2s_dma_config(0, &attr);

    if (microphone_usb_uac_init() != 0) {
        osal_printk("%s Init the USB UAC fail.\r\n", MICROPHONE_USB_LOG);
    }

    /* DMA init. */
    uapi_dma_init();
    uapi_dma_open();
    if (i2s_start_dma_transfer(g_i2s_receive_dma_data[g_dma_fifo_buf_index], i2s_dma_trans_done_callback) != 0) {
        return;
    }
    osal_printk("DMA transfer start.\r\n");
}

static void sample_i2s_init(void)
{
    sio_porting_i2s_pinmux();
    uapi_i2s_init(0, NULL);
    i2s_config_t config = {
        .drive_mode= MASTER,
        .transfer_mode = STD_MODE,
        .data_width = TWENTY_FOUR_BIT,
        .channels_num = TWO_CH,
        .timing = NONE_MODE,
        .clk_edge = RISING_EDGE,
        .div_number = DATA_WIDTH,
        .number_of_channels = NUMBER_OF_CHANNELS,
    };
    uapi_i2s_set_config(0, &config);
}

static void *i2s_task(const char *arg)
{
    unused(arg);
    errcode_t ret = sample_i2c_init();
    osal_printk("i2c slave init %d\r\n", ret);

    sample_i2s_init();
    uapi_pin_set_mode(S_AGPIO7, PIN_MODE_1);
    uapi_clock_crg_config(CLOCK_CRG_ID_PAD_OUT0, CLOCK_CLK_SRC_ULPFLL_MCU_CORE, 0x7);
    uapi_pin_set_mode(S_AGPIO12, PIN_MODE_1);
    uapi_pin_set_mode(S_AGPIO13, PIN_MODE_1);

    /* ADC需要外部输入的时钟 */
    i2c_write_date1();
    i2c_write_date2();
    i2c_write_date3();

    osal_printk("i2c%d slave send succ!\r\n", TEST_I2C);
    i2s_dma_init();

    while (1) {
        if (g_i2s_receive_trans_cnt >= g_i2s_receive_dma_cnt) {
            uapi_watchdog_kick();
            continue;
        }
        for (uint32_t i = 0; i < CONFIG_ES7243L_DS_TRANSFER_LEN_OF_DMA; i = i + FILLED_COUNT) {
            g_i2s_receive_buffer[g_i2s_receive_buffer_filled_count++] =
                                (uint8_t)(g_i2s_receive_dma_data[g_uac_fifo_buf_index][i] >> USB_MICPHONE_OFFSET_16);
            g_i2s_receive_buffer[g_i2s_receive_buffer_filled_count++] =
                                (uint8_t)(g_i2s_receive_dma_data[g_uac_fifo_buf_index][i] >> USB_MICPHONE_OFFSET_8);
        }

        g_i2s_receive_trans_cnt++;
        g_uac_fifo_buf_index = (g_uac_fifo_buf_index + 1) % UAC_FIFO_BUF_NUM;

        if (g_i2s_receive_buffer_filled_count < UAC_SEND_DATA_LEN) {
            continue;
        }

        if ((microphone_usb_uac_send_data((uint8_t *)g_i2s_receive_buffer, UAC_SEND_DATA_LEN) != 0)) {
            osal_printk("%s Send UAV to USB fail.\r\n", MICROPHONE_USB_LOG);
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

    uint8_t clk_div = 2;
    uapi_clock_crg_config(CLOCK_CRG_ID_MCU_CORE, CLOCK_CLK_SRC_FNPLL_MCU_HS, 1);
    uapi_clock_crg_config(CLOCK_CRG_ID_MEM_BUS, CLOCK_CLK_SRC_FNPLL_MCU_HS, clk_div);
    uapi_clock_crg_config(CLOCK_CRG_ID_XIP_OPI, CLOCK_CLK_SRC_FNPLL_MCU_HS, clk_div);
    uapi_clock_crg_config(CLOCK_CRG_ID_COM_BUS, CLOCK_CLK_SRC_FNPLL_MCU_HS, clk_div);
    uapi_clock_crg_config(CLOCK_CRG_ID_COM_BUS, CLOCK_CLK_SRC_FNPLL_MCU_HS, clk_div);
    uapi_eflash_init(EMBED_FLASH_0);
    uapi_eflash_set_freq(EMBED_FLASH_0, CLOCK_96M);

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