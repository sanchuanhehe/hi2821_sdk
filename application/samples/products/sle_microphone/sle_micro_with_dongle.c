/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: SLE MICRO Sample Source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-10-12, Create file. \n
 */
#include <stdbool.h>
#include "cmsis_os2.h"
#include "soc_osal.h"
#include "osal_debug.h"
#include "osal_semaphore.h"
#include "osal_msgqueue.h"
#include "common_def.h"
#include "app_init.h"
#include "securec.h"
#include "tcxo.h"
#include "watchdog.h"
#include "pm_clock.h"
#include "sle_errcode.h"
#include "sle_ssap_client.h"
#include "sle_device_discovery.h"
#include "sle_connection_manager.h"
#include "sle_micro/sle_micro_server/sle_micro_server.h"
#include "sle_micro/sle_micro_server/sle_micro_server_adv.h"
#include "sle_micro_dongle/sle_micro_client/sle_micro_client.h"
#include "sle_micro_dongle/sle_micro_usb/sle_micro_usb.h"
#include "sle_micro/sle_micro_i2s/sle_micro_i2s.h"
#include "hal_dma.h"
#include "dma.h"
#include "dma_porting.h"
#include "gpio.h"
#include "pinctrl.h"
#include "i2s.h"
#include "i2c.h"

#define SLE_MICRO_TASK_STACK_SIZE 0x2000
#define SLE_MICRO_TASK_PRIO (osPriority_t)(17)
#define SLE_MICRO_TASK_DURATION_MS 2000
#define SLE_MICRO_WAIT_SSAPS_READY 500

#if defined(CONFIG_SAMPLE_SUPPORT_SLE_MICROPHONE_SERVER)
#define SLE_MICRO_SERVER_LOG "[sle micro server]"

#define SLE_MICRO_TRANSFER_LEN_OF_PCM 480
#define SLE_MICRO_TRANSFER_LEN_OF_DMA 480
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
#define DATA_WIDTH                  32
#define NUMBER_OF_CHANNELS          2

#define USB_MICPHONE_OFFSET_0       0
#define USB_MICPHONE_OFFSET_8       8
#define USB_MICPHONE_OFFSET_16      16
#define USB_MICPHONE_OFFSET_24      24

#define I2S_TRANS_SRC_ADDR          0x520300a0
#define UAC_SEND_DATA_LEN           1920
#define UAC_FIFO_BUF_NUM            4
#define UAC_FIFO_CYCLE_NUM          3
#define I2C_WRITE_DATA_LEN          2
#define FILLED_COUNT                2
#define SLE_DMA_TASK_PRIO3          3
#define DELAY_DMA_100_US            100
#define SLE_MICRO_ONCE_TRANSFER_LENGTH (SLE_MICRO_TRANSFER_LEN_OF_DMA)
#define I2C_REG_NUM         39

static uint8_t i2c_config[I2C_REG_NUM][I2C_WRITE_DATA_LEN] = {
    {0x01, 0x3A},
    {0x00, 0x80},
    {0x04, 0x03},
    {0x04, 0x03},
    {0xF9, 0x00},
    {0x00, 0x1E},
    {0x01, 0x00},
    {0x02, 0x80},
    {0x03, 0x20},
    {0x0D, 0x00},
    {0xF9, 0x00},
    {0x04, 0x03},

    {0x04, 0x03},
    {0x05, 0x00},
    {0x07, 0x00},
    {0x09, 0xC5},
    {0x0A, 0x81},
    {0x0B, 0x0C},
    {0x0E, 0xBF},
    {0x0F, 0xA0},
    {0x10, 0x18},
    {0x11, 0x16},
    {0x14, 0x0C},
    {0x15, 0x0C},

    {0x17, 0x02},
    {0x18, 0x24},
    {0x19, 0x88},
    {0x1A, 0x88},
    {0x1B, 0x66},
    {0x1C, 0x44},
    {0x1D, 0xF0},
    {0x1E, 0x00},
    {0x1F, 0x0C},
    {0x20, 0x19},
    {0x21, 0x19},
    {0x00, 0x80},
    {0x01, 0x3A},
    {0x16, 0x3F},
    {0x16, 0x00}
};
static uint8_t g_pcm_sle_buffer[SLE_MICRO_TRANSFER_LEN_OF_PCM + 2] = { 0 };
static uint32_t g_pcm_dma_data0[SLE_MICRO_TRANSFER_LEN_OF_DMA] = { 0 };
static uint32_t g_pcm_dma_data1[SLE_MICRO_TRANSFER_LEN_OF_DMA] = { 0 };
static uint32_t g_pcm_dma_data2[SLE_MICRO_TRANSFER_LEN_OF_DMA] = { 0 };
static uint32_t g_pcm_dma_data3[SLE_MICRO_TRANSFER_LEN_OF_DMA] = { 0 };
static uint32_t *g_pcm_dma_data[UAC_FIFO_BUF_NUM] = {g_pcm_dma_data0, g_pcm_dma_data1,
    g_pcm_dma_data2, g_pcm_dma_data3};
static uint8_t g_buffer_state = 0;
static uint8_t g_transfer_done = 0;

/**
 * @brief  星闪设置PHY
 */
typedef struct {
    uint16_t co_handle;         /*!< 连接句柄 */
    uint8_t tx_format;          /*!< 发送无线帧类型，参考gle_radio_frame_type_t */
    uint8_t rx_format;          /*!< 接收无线帧类型，参考gle_radio_frame_type_t */
    uint8_t tx_phy;             /*!< 发送PHY，参考gle_tx_rx_phy_t */
    uint8_t rx_phy;             /*!< 接收PHY，参考gle_tx_rx_phy_t */
    uint8_t tx_pilot_density;   /*!< 发送导频密度指示，参考gle_tx_rx_pilot_density_t */
    uint8_t rx_pilot_density;   /*!< 接收导频密度指示，参考gle_tx_rx_pilot_density_t */
    uint8_t g_feedback;         /*!< 先发链路反馈类型指示，取值范围0-63。
                                     0：指示基于CBG的反馈
                                     1-25：指示不携带数据信息场景组播反馈信息的比特位置, 其中位置信息为指示信息的数值
                                     26：指示基于TB的反馈
                                     27-34：指示携带数据信息场景组播反馈信息的比特位置，其中位置信息为(指示信息的数值-26)
                                     35-63：预留 */
    uint8_t t_feedback;         /*!< 后发链路反馈类型指示，取值范围0-7
                                     0-5：指示半可靠组播反馈，并指示采用m序列的编号
                                     6：指示基于CBG的反馈
                                     7：指示基于TB的反馈 */
} bth_gle_set_phy_t;

extern errcode_t uapi_gle_set_phy(bth_gle_set_phy_t *param);

static void ssaps_server_read_request_cbk(uint8_t server_id, uint16_t conn_id, ssaps_req_read_cb_t *read_cb_para,
                                          errcode_t status)
{
    osal_printk("%s ssaps read request cbk callback server_id:%x, conn_id:%x, handle:%x, status:%x\r\n",
        SLE_MICRO_SERVER_LOG,
        server_id,
        conn_id,
        read_cb_para->handle,
        status);
}

static void ssaps_server_write_request_cbk(uint8_t server_id, uint16_t conn_id, ssaps_req_write_cb_t *write_cb_para,
                                           errcode_t status)
{
    unused(status);
    unused(conn_id);
    unused(server_id);
    if (write_cb_para == NULL || write_cb_para->value == NULL) {
        return;
    }
    osal_printk("sle micro write recv data:%s\r\n", write_cb_para->value);
}

int32_t sle_micro_adc_start_dma_transfer(uint32_t *pcm_buffer, dma_transfer_cb_t trans_done)
{
    dma_ch_user_peripheral_config_t transfer_config;
    uint8_t channel = 0;

    unused(transfer_config);

    transfer_config.src = I2S_TRANS_SRC_ADDR;
    transfer_config.dest = (uint32_t)(uintptr_t)pcm_buffer;
    transfer_config.transfer_num = SLE_MICRO_TRANSFER_LEN_OF_DMA;
    transfer_config.src_handshaking = 0xc;  /* MIC45_UPLINK_REQ: pdm两路mic的fifo握手通道 */
    transfer_config.dest_handshaking = 0;
    transfer_config.trans_type = HAL_DMA_TRANS_PERIPHERAL_TO_MEMORY_DMA;
    transfer_config.trans_dir = HAL_DMA_TRANSFER_DIR_PERIPHERAL_TO_MEM;
    transfer_config.priority = SLE_DMA_TASK_PRIO3;
    transfer_config.src_width = HAL_DMA_TRANSFER_WIDTH_32;
    transfer_config.dest_width = HAL_DMA_TRANSFER_WIDTH_32;
    transfer_config.burst_length = 0;
    transfer_config.src_increment = HAL_DMA_ADDRESS_INC_NO_CHANGE;
    transfer_config.dest_increment = HAL_DMA_ADDRESS_INC_INCREMENT;
    transfer_config.protection = HAL_DMA_PROTECTION_CONTROL_BUFFERABLE;

    errcode_t ret = uapi_dma_configure_peripheral_transfer_single(&transfer_config, &channel,
                                                                  trans_done, (uintptr_t)NULL);
    if (ret != ERRCODE_SUCC) {
        osal_printk("%s Configure the DMA fail. %x\r\n", SLE_MICRO_SERVER_LOG, ret);
        return 1;
    }

    ret = uapi_dma_start_transfer(channel);
    if (ret != ERRCODE_SUCC) {
        osal_printk("%s Start the DMA fail. %x\r\n", SLE_MICRO_SERVER_LOG, ret);
        return 1;
    }

    return 0;
}
static void sle_micro_dma_transfer_restart(void);
static void sle_micro_dma_transfer_done_callback(uint8_t intr, uint8_t channel, uintptr_t arg)
{
    unused(channel);
    unused(arg);

    switch (intr) {
        case HAL_DMA_INTERRUPT_TFR:
            sle_micro_dma_transfer_restart();
            break;
        case HAL_DMA_INTERRUPT_ERR:
            osal_printk("%s DMA transfer error.\r\n", SLE_MICRO_SERVER_LOG);
            break;
        default:
            break;
    }
}
static void sle_micro_dma_transfer_restart(void)
{
    g_transfer_done = 1;
    g_buffer_state++;
    g_buffer_state = g_buffer_state % UAC_FIFO_BUF_NUM;
    if (sle_micro_adc_start_dma_transfer(g_pcm_dma_data[g_buffer_state], sle_micro_dma_transfer_done_callback) != 0) {
        return;
    }
}

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
#if I2C_MODE
    return uapi_i2c_master_init(TEST_I2C, I2C_BAUDRATE, I2C_MASTER_ADDR); /* 初始化 i2c0 */
#else
    return uapi_i2c_slave_init(TEST_I2C, I2C_BAUDRATE, I2C_SLAVE_ADDR); /* 初始化 i2c0 */
#endif
}

errcode_t sample_i2c_write(uint8_t *data, uint8_t len)
{
    i2c_data_t i2c_send_data = { 0 };
    i2c_send_data.send_buf = data;                         /* 设置 tx buff */
    i2c_send_data.send_len = len;                          /* 设置 tx buff 长度 */
#if I2C_MODE == 1
    return uapi_i2c_master_write(TEST_I2C, I2C_SLAVE_ADDR, &i2c_send_data); /* 发送数据 */
#else
    return uapi_i2c_slave_write(TEST_I2C, &i2c_send_data); /* 发送数据 */
#endif
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

    uapi_pin_set_mode(S_AGPIO7, PIN_MODE_1);
    uapi_clock_crg_config(CLOCK_CRG_ID_PAD_OUT0, CLOCK_CLK_SRC_ULPFLL_MCU_CORE, 0x7);
    uapi_pin_set_mode(S_AGPIO12, PIN_MODE_1);
    uapi_pin_set_mode(S_AGPIO13, PIN_MODE_1);

    /* ADC需要外部输入的时钟 */
    for (uint8_t i = 0; i < I2C_REG_NUM; i++) {
        sample_i2c_write(i2c_config[i], I2C_WRITE_DATA_LEN);
    }
    i2s_dma_attr_t attr = {
        .tx_dma_enable = 1,
        .tx_int_threshold = 8,
        .rx_dma_enable = 1,
        .rx_int_threshold = 1,
    };
    uapi_i2s_dma_config(0, &attr);
    uapi_dma_init();
    uapi_dma_open();
}

static void set_phy_speed(void)
{
    bth_gle_set_phy_t param = {0};
    param.co_handle = get_sle_conn_hdl();
    param.tx_format = 1;         // 无线帧类型2
    param.rx_format = 1;         // 无线帧类型2
    param.tx_phy = 2;            // 0 1M 1 2M 2 4M
    param.rx_phy = 2;            //
    param.tx_pilot_density = 0;  // 导频密度4:1
    param.rx_pilot_density = 0;  // 导频密度4:1
    param.g_feedback = 0;
    param.t_feedback = 0;
    if (uapi_gle_set_phy(&param) != 0) {
        osal_printk("uapi_gle_set_phy fail\r\n");
    }
}

static void *sle_micro_server_task(const char *arg)
{
    unused(arg);
    if (uapi_clock_crg_config(CLOCK_CRG_ID_MCU_CORE, CLOCK_CLK_SRC_FNPLL_MCU_HS, 0x1) == ERRCODE_SUCC) {
        osal_printk("Config succ.\r\n");
    } else {
        osal_printk("Config fail.\r\n");
    }
    sle_micro_server_init(ssaps_server_read_request_cbk, ssaps_server_write_request_cbk);
    ssap_exchange_info_t parameter = { 0 };
    parameter.mtu_size = CONFIG_SLE_MTU_LENGTH;
    parameter.version = 1;
    ssaps_set_info(get_ssaps_server_id(), &parameter);
    while (get_ssaps_ready() == false) {
        osDelay(SLE_MICRO_WAIT_SSAPS_READY);
    }
    osal_printk("get_ssaps_ready.\r\n");
    while (get_conn_update() == 0) {
        osDelay(SLE_MICRO_TASK_DURATION_MS);
    }
    osal_printk("get_conn_update.\r\n");

    set_phy_speed();

    errcode_t ret = sample_i2c_init();
    osal_printk("i2s slave init %d\r\n", ret);
    sample_i2s_init();
    if (sle_micro_adc_start_dma_transfer(g_pcm_dma_data[g_buffer_state], sle_micro_dma_transfer_done_callback) != 0) {
        return NULL;
    }
    uint32_t buffer_filled_count = 0;
    while (1) {
        uapi_watchdog_kick();
        while (g_transfer_done != 1) {
            uapi_tcxo_delay_us(DELAY_DMA_100_US);
        }
        g_transfer_done = 0;
        for (uint32_t i = 0; i < SLE_MICRO_TRANSFER_LEN_OF_DMA; i += FILLED_COUNT) {
            g_pcm_sle_buffer[buffer_filled_count++] = (uint8_t)(g_pcm_dma_data[(g_buffer_state + UAC_FIFO_CYCLE_NUM) %
                UAC_FIFO_BUF_NUM][i] >> USB_MICPHONE_OFFSET_8);
            g_pcm_sle_buffer[buffer_filled_count++] = (uint8_t)(g_pcm_dma_data[(g_buffer_state + UAC_FIFO_CYCLE_NUM) %
                UAC_FIFO_BUF_NUM][i] >> USB_MICPHONE_OFFSET_16);
        }
        buffer_filled_count = 0;
        sle_micro_server_send_report_by_handle(g_pcm_sle_buffer, SLE_MICRO_ONCE_TRANSFER_LENGTH);
    }

    return NULL;
}

#elif defined(CONFIG_SAMPLE_SUPPORT_SLE_MICROPHONE_DONGLE)
#define SLE_MICRO_DONGLE_LOG "[sle micro dongle]"
static void sle_micro_notification_cb(uint8_t client_id, uint16_t conn_id, ssapc_handle_value_t *data, errcode_t status)
{
    unused(client_id);
    unused(status);
    if (data == NULL || data->data == NULL) {
        return;
    }
    if ((sle_micro_usb_uac_send_data(data->data, data->data_len) != 0)) {
        osal_printk("%s Send UAV to USB fail,conn_id:[%d]\r\n", MICROPHONE_USB_LOG, conn_id);
    }
}

static void sle_micro_indication_cb(uint8_t client_id, uint16_t conn_id, ssapc_handle_value_t *data, errcode_t status)
{
    unused(client_id);
    unused(conn_id);
    unused(status);
    if (data == NULL || data->data == NULL) {
        return;
    }
    osal_printk("%s sle micro indication recived data : %s\r\n", SLE_MICRO_DONGLE_LOG, data->data);
}
static void *sle_micro_client_task(const char *arg)
{
    unused(arg);
    if (uapi_clock_crg_config(CLOCK_CRG_ID_MCU_CORE, CLOCK_CLK_SRC_FNPLL_MCU_HS, 0x1) == ERRCODE_SUCC) {
        osal_printk("Config succ.\r\n");
    } else {
        osal_printk("Config fail.\r\n");
    }

    if (sle_micro_usb_uac_init() != 0) {
        osal_printk("%s Init the USB UAC fail.\r\n", MICROPHONE_USB_LOG);
    }
    sle_micro_client_init(sle_micro_notification_cb, sle_micro_indication_cb);
    while (get_ssap_find_ready() == 0) {
        osDelay(SLE_MICRO_WAIT_SSAPS_READY);
    }
    osal_printk("%s get_ssap_find_ready.\r\n", SLE_MICRO_DONGLE_LOG);
    // delay for param update complete
    osDelay(SLE_MICRO_TASK_DURATION_MS);
    while (1) {
        osDelay(SLE_MICRO_TASK_DURATION_MS);
    }
    return NULL;
}
#endif /* CONFIG_SAMPLE_SUPPORT_SLE_MICROPHONE_DONGLE */

static void sle_micro_with_dongle_entry(void)
{
    osThreadAttr_t attr;

    attr.name = "SLEMicroTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = SLE_MICRO_TASK_STACK_SIZE;
    attr.priority = SLE_MICRO_TASK_PRIO;

    uapi_clock_control(CLOCK_CONTROL_BT_CORE_FREQ_CONFIG, CLOCK_MCU_CTL_BT_64M);

#if defined(CONFIG_SAMPLE_SUPPORT_SLE_MICROPHONE_SERVER)
    if (osThreadNew((osThreadFunc_t)sle_micro_server_task, NULL, &attr) == NULL) {
        /* Create task fail. */
    }
#elif defined(CONFIG_SAMPLE_SUPPORT_SLE_MICROPHONE_DONGLE)
    if (osThreadNew((osThreadFunc_t)sle_micro_client_task, NULL, &attr) == NULL) {
        /* Create task fail. */
    }
#endif /* CONFIG_SAMPLE_SUPPORT_SLE_MICROPHONE_DONGLE */
}

/* Run the sle_micro_with_dongle_entry. */
app_run(sle_micro_with_dongle_entry);