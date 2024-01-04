/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: SLE VDT Sample Source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-09-23, Create file. \n
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
#include "hal_dma.h"
#include "pdm.h"
#include "dma.h"
#include "tcxo.h"
#include "watchdog.h"
#include "dma_porting.h"
#include "pm_clock.h"

#include "sle_errcode.h"
#include "sle_ssap_client.h"
#include "sle_device_discovery.h"
#include "sle_connection_manager.h"
#include "sle_vdt_client/sle_vdt_client.h"
#include "sle_vdt_server/sle_vdt_server.h"
#include "sle_vdt_server/sle_vdt_server_adv.h"

#include "sle_vdt_codec/sle_vdt_codec.h"
#include "sle_vdt_usb/sle_vdt_usb.h"
#include "sle_vdt_pdm/sle_vdt_pdm.h"

#define SLE_VDT_TASK_STACK_SIZE 0x4000
#define SLE_VDT_TASK_PRIO (osPriority_t)(17)
#define SLE_VDT_TASK_DURATION_MS 2000
#define SLE_VDT_WAIT_SSAPS_READY 500

#define SLE_VDT_MIC_OFFSET_16 16
#define SLE_VDT_MIC_OFFSET_24 24
#define SLE_VDT_ONCE_TRANSFER_LENGTH (CONFIG_SLE_PDM_TRANSFER_LEN_OF_DMA * 2)

#if defined(CONFIG_SAMPLE_SUPPORT_SLE_DONGLE)
#define SLE_VDT_SERVER_DELAY_COUNT 5
#define SLE_VDT_SERVER_MSG_QUEUE_LEN 5
#define SLE_VDT_SERVER_MSG_QUEUE_MAX_SIZE 32
#define SLE_VDT_SERVER_QUEUE_DELAY 0xFFFFFFFF
#define SLE_VDT_SERVER_LOG "[sle vdt dongle]"

uint32_t g_total_ssapc_req_count = 0;

static void ssaps_server_read_request_cbk(uint8_t server_id, uint16_t conn_id, ssaps_req_read_cb_t *read_cb_para,
                                          errcode_t status)
{
    osal_printk("%s ssaps read request cbk callback server_id:%x, conn_id:%x, handle:%x, status:%x\r\n",
        SLE_VDT_SERVER_LOG,
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

    if ((sle_vdt_usb_uac_send_data(write_cb_para->value, write_cb_para->length) != 0)) {
        osal_printk("%s Send UAV to USB fail.\r\n", MICROPHONE_USB_LOG);
    }
}

static void *sle_vdt_server_task(const char *arg)
{
    unused(arg);

    if (uapi_clock_crg_config(CLOCK_CRG_ID_MCU_CORE, CLOCK_CLK_SRC_FNPLL_MCU_HS, 0x1) == ERRCODE_SUCC) {
        osal_printk("Config succ.\r\n");
    } else {
        osal_printk("Config fail.\r\n");
    }

    if (sle_vdt_usb_uac_init() != 0) {
        osal_printk("%s Init the USB UAC fail.\r\n", MICROPHONE_USB_LOG);
    }

    sle_vdt_server_init(ssaps_server_read_request_cbk, ssaps_server_write_request_cbk);

    while (get_g_ssaps_ready() == false) {
        osDelay(SLE_VDT_WAIT_SSAPS_READY);
    }
    osal_printk("get_g_ssaps_ready.\r\n");
    while (get_g_conn_update() == 0) {
        osDelay(SLE_VDT_TASK_DURATION_MS);
    }
    osal_printk("get_g_conn_update.\r\n");

    while (g_total_ssapc_req_count == 0) {
        osDelay(SLE_VDT_TASK_DURATION_MS);
    }
    osal_printk("g_total_ssapc_req_count.\r\n");
    while (1) {
        uapi_watchdog_kick();
        osDelay(SLE_VDT_TASK_DURATION_MS);
    }
    return NULL;
}

#elif defined(CONFIG_SAMPLE_SUPPORT_SLE_CLIENT)
#define RING_BUFFER_NUMBER 4
static uint8_t g_pcm_sle_buffer[CONFIG_USB_UAC_MAX_RECORD + 2] = { 0 };
static uint32_t g_pcm_dma_data0[CONFIG_SLE_PDM_TRANSFER_LEN_OF_DMA] = { 0 };
static uint32_t g_pcm_dma_data1[CONFIG_SLE_PDM_TRANSFER_LEN_OF_DMA] = { 0 };
static uint32_t g_pcm_dma_data2[CONFIG_SLE_PDM_TRANSFER_LEN_OF_DMA] = { 0 };
static uint32_t g_pcm_dma_data3[CONFIG_SLE_PDM_TRANSFER_LEN_OF_DMA] = { 0 };
static uint32_t *g_pcm_dma_data[RING_BUFFER_NUMBER] = {
    g_pcm_dma_data0, g_pcm_dma_data1, g_pcm_dma_data2, g_pcm_dma_data3};
static uint8_t g_write_buffer_state = 0;
static uint8_t g_read_buffer_state = 0;
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

static void usb_sle_vdt_dma_transfer_done_callback(uint8_t intr, uint8_t channel, uintptr_t arg);

static void usb_sle_vdt_dma_transfer_restart(void)
{
    g_transfer_done = 1;
    g_write_buffer_state = (g_write_buffer_state + 1) % RING_BUFFER_NUMBER;
    if (sle_vdt_pdm_start_dma_transfer(
        g_pcm_dma_data[g_write_buffer_state],
        usb_sle_vdt_dma_transfer_done_callback) != 0) {
        return;
    }
}

static void usb_sle_vdt_dma_transfer_done_callback(uint8_t intr, uint8_t channel, uintptr_t arg)
{
    unused(channel);
    unused(arg);

    switch (intr) {
        case HAL_DMA_INTERRUPT_TFR:
            usb_sle_vdt_dma_transfer_restart();
            break;
        case HAL_DMA_INTERRUPT_ERR:
            osal_printk("%s DMA transfer error.\r\n", MICROPHONE_USB_LOG);
            break;
        default:
            break;
    }
}
static void sle_vdt_notification_cb(uint8_t client_id, uint16_t conn_id, ssapc_handle_value_t *data, errcode_t status)
{
    unused(client_id);
    unused(conn_id);
    unused(status);
    osal_printk("\n sle vdt recived data : %s\r\n", data->data);
}

static void sle_vdt_indication_cb(uint8_t client_id, uint16_t conn_id, ssapc_handle_value_t *data, errcode_t status)
{
    unused(client_id);
    unused(conn_id);
    unused(status);
    osal_printk("\n sle vdt recived data : %s\r\n", data->data);
}

static void sle_vdt_set_phy_param(void)
{
    bth_gle_set_phy_t param = {0};
    param.co_handle = get_g_sle_vdt_conn_id();
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
        return;
    }
    osal_printk("uapi_gle_set_phy\r\n");
}

static void sle_vdt_set_connect_param(void)
{
    sle_connection_param_update_t params = { 0 };
    params.conn_id = get_g_sle_vdt_conn_id();
    params.interval_min = 0xA;  // set it accroding to SLE_VDT_ONCE_TRANSFER_LENGTH
    params.interval_max = 0xA;
    params.max_latency = 0;
    params.supervision_timeout = 0x1F4;
    sle_update_connect_param(&params);
    osal_printk("sle_update_connect_param\r\n");
}

static void *sle_vdt_client_task(const char *arg)
{
    unused(arg);
    if (uapi_clock_crg_config(CLOCK_CRG_ID_MCU_CORE, CLOCK_CLK_SRC_FNPLL_MCU_HS, 0x1) != ERRCODE_SUCC) {
        osal_printk("Config fail.\r\n");
    }
    sle_vdt_client_init(sle_vdt_notification_cb, sle_vdt_indication_cb);

    while (get_g_ssap_find_ready() == 0) {
        osDelay(SLE_VDT_WAIT_SSAPS_READY);
    }
    osal_printk("get_g_ssap_find_ready.\r\n");
    sle_vdt_set_phy_param();
    sle_vdt_set_connect_param();
    if (sle_vdt_pdm_init() != 0) {
        osal_printk("%s Init the PDM fail.\r\n", MICROPHONE_USB_LOG);
    }

    if (uapi_pdm_start() != ERRCODE_SUCC) {
        osal_printk("%s Start the PDM fail.\r\n", MICROPHONE_USB_LOG);
    }

    uapi_dma_init();
    uapi_dma_open();
    sle_vdt_codec_init();

    if (sle_vdt_pdm_start_dma_transfer(g_pcm_dma_data[g_write_buffer_state],
        usb_sle_vdt_dma_transfer_done_callback) != 0) {
        return NULL;
    }

    ssapc_write_param_t *sle_vdt_send_param = get_g_sle_vdt_send_param();
    sle_vdt_send_param->data_len = SLE_VDT_ONCE_TRANSFER_LENGTH;
    sle_vdt_send_param->data = g_pcm_sle_buffer;
    uint32_t buffer_filled_count = 0;
    while (1) {
        uapi_watchdog_kick();
        while (g_transfer_done != 1) {
            uapi_tcxo_delay_us(SLE_VDT_WAIT_SSAPS_READY);
        }
        g_transfer_done = 0;
        for (uint32_t i = 0; i < CONFIG_SLE_PDM_TRANSFER_LEN_OF_DMA; i++) {
            g_pcm_sle_buffer[buffer_filled_count++] =
                (uint8_t)(g_pcm_dma_data[g_read_buffer_state][i] >> SLE_VDT_MIC_OFFSET_16);
            g_pcm_sle_buffer[buffer_filled_count++] =
                (uint8_t)(g_pcm_dma_data[g_read_buffer_state][i] >> SLE_VDT_MIC_OFFSET_24);
        }
        g_read_buffer_state = (g_read_buffer_state + 1) % RING_BUFFER_NUMBER;
        buffer_filled_count = 0;
        ssapc_write_cmd(0, get_g_sle_vdt_conn_id(), sle_vdt_send_param);
    }

    return NULL;
}
#endif /* CONFIG_SAMPLE_SUPPORT_SLE_CLIENT */

static void sle_vdt_with_dongle_entry(void)
{
    osThreadAttr_t attr;

    attr.name = "SLEVdtTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = SLE_VDT_TASK_STACK_SIZE;
    attr.priority = SLE_VDT_TASK_PRIO;
#if defined(CONFIG_SAMPLE_SUPPORT_SLE_DONGLE)
    if (osThreadNew((osThreadFunc_t)sle_vdt_server_task, NULL, &attr) == NULL) {
        /* Create task fail. */
    }
#elif defined(CONFIG_SAMPLE_SUPPORT_SLE_CLIENT)
    if (osThreadNew((osThreadFunc_t)sle_vdt_client_task, NULL, &attr) == NULL) {
        /* Create task fail. */
    }
#endif /* CONFIG_SAMPLE_SUPPORT_SLE_CLIENT */
}

/* Run the sle_vdt_with_dongle_entry. */
app_run(sle_vdt_with_dongle_entry);