/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: at plt cmd func \n
 * Author: @CompanyNameTag \n
 */

#include "test_suite_uart.h"
#include "uart.h"
#include "platform_core.h"
#include "soc_osal.h"
#include "common_def.h"
#include "at_config.h"
#include "at_product.h"
#include "at_porting.h"

#if defined (AT_COMMAND) && defined (TEST_SUITE)
#define AT_UART_BUS                 TEST_SUITE_UART_BUS
#define AT_UART_TX_PIN              TEST_SUITE_UART_TX_PIN
#define AT_UART_RX_PIN              TEST_SUITE_UART_RX_PIN
#define AT_UART_BAUD_RATE           TEST_SUITE_UART_BAUD_RATE
#else
#define AT_UART_BUS                 TEST_SUITE_UART_BUS
#define AT_UART_TX_PIN              TEST_SUITE_UART_TX_PIN
#define AT_UART_RX_PIN              TEST_SUITE_UART_RX_PIN
#define AT_UART_BAUD_RATE           TEST_SUITE_UART_BAUD_RATE
#endif
#define AT_RX_BUFF_SIZE             128
#define CRLF_STR                    "\r\n"
#define CR_ASIC_II                  0xD

static osal_task g_at_task = { 0 };
static osal_task g_testsuite_task = { 0 };
static uint8_t g_at_uart_rx_buffer[AT_RX_BUFF_SIZE];

/* at uart write port */
static void at_write_func(const char *data)
{
    uapi_uart_write(AT_UART_BUS, (const uint8_t *)data, strlen(data), 0);
}

/* at uart receive port */
static void at_uart_rx_callback(const void *buffer, uint16_t length, bool error)
{
    unused(error);
    if (length == 0) {
        return;
    }
    if (((char *)buffer)[0] == CR_ASIC_II) {
        uapi_uart_write(AT_UART_BUS, (uint8_t *)CRLF_STR, (uint16_t)strlen(CRLF_STR), 0);
    } else {
        uapi_uart_write(AT_UART_BUS, (const uint8_t *)buffer, (uint32_t)length, 0);
    }
    uapi_at_channel_data_recv(AT_UART_PORT, (uint8_t *)buffer, (uint32_t)length);
}

/* at uart init port */
static void at_uart_init(void)
{
    uart_buffer_config_t uart_buffer_config;
    uart_pin_config_t uart_pin_config = {
        .tx_pin = AT_UART_TX_PIN,
        .rx_pin = AT_UART_RX_PIN,
        .cts_pin = PIN_NONE,
        .rts_pin = PIN_NONE
    };
    uart_attr_t uart_line_config = {
        .baud_rate = AT_UART_BAUD_RATE,
        .data_bits = UART_DATA_BIT_8,
        .stop_bits = UART_STOP_BIT_1,
        .parity = UART_PARITY_NONE
    };
    uart_buffer_config.rx_buffer_size = sizeof(g_at_uart_rx_buffer);
    uart_buffer_config.rx_buffer = g_at_uart_rx_buffer;
    uapi_uart_init(AT_UART_BUS, &uart_pin_config, &uart_line_config, NULL, &uart_buffer_config);
    uapi_uart_unregister_rx_callback(AT_UART_BUS);
    uapi_uart_register_rx_callback(AT_UART_BUS, UART_RX_CONDITION_FULL_OR_SUFFICIENT_DATA_OR_IDLE,
        1, at_uart_rx_callback);
}

static void at_base_api_queue_create(uint32_t msg_count, uint32_t msg_size, unsigned long *queue_id)
{
    osal_msg_queue_create(NULL, (unsigned short)msg_count, queue_id, 0, (unsigned short)msg_size);
}

static uint32_t at_base_api_msg_queue_write(unsigned long queue_id, void *msg_ptr,
                                            uint32_t msg_size, uint32_t timeout)
{
    return osal_msg_queue_write_copy(queue_id, msg_ptr, msg_size, timeout);
}

static uint32_t at_base_api_msg_queue_read(unsigned long queue_id, void *buf_ptr,
                                           uint32_t *buf_size, uint32_t timeout)
{
    return osal_msg_queue_read_copy(queue_id, buf_ptr, buf_size, timeout);
}

static void at_base_api_task_pause(void)
{
    osal_yield();
}

static void* at_base_api_malloc(uint32_t size)
{
    return osal_kmalloc(size, OSAL_GFP_ATOMIC);
}

static void at_base_api_free(void *addr)
{
    osal_kfree(addr);
}

static void at_base_api_register(void)
{
    at_base_api_t base_api = {
        .msg_queue_create_func = at_base_api_queue_create,
        .msg_queue_write_func = at_base_api_msg_queue_write,
        .msg_queue_read_func = at_base_api_msg_queue_read,
        .task_pause_func = at_base_api_task_pause,
        .malloc_func = at_base_api_malloc,
        .free_func = at_base_api_free,
    };
    uapi_at_base_api_register(base_api);
}


static void at_cmd_sw_callback(void)
{
    static volatile bool g_at_cmd_status = false;
    if (g_testsuite_task.task == NULL) {
        g_testsuite_task.task = (void *)osal_get_current_tid();
    }
    g_at_cmd_status = !g_at_cmd_status;
    if (g_at_cmd_status) {
        uapi_uart_deinit(TEST_SUITE_UART_BUS);
        uapi_at_cmd_init();
        osal_kthread_resume(&g_at_task);
        osal_kthread_suspend(&g_testsuite_task);
    } else {
        uapi_uart_deinit(AT_UART_BUS);
        test_suite_uart_init();
        osal_kthread_resume(&g_testsuite_task);
        osal_kthread_suspend(&g_at_task);
    }
}

void uapi_set_at_task(uint32_t *at_task_id)
{
    g_at_task.task = (void *)at_task_id;
}

int uapi_testsuite_sw_at(int argc, char **argv)
{
    unused(argc);
    unused(argv);
    at_cmd_sw_callback();
    return 0;
}

at_ret_t uapi_at_sw_testsuite(void)
{
    at_cmd_sw_callback();
    return AT_RET_OK;
}

void uapi_at_cmd_init(void)
{
    at_uart_init();
    at_base_api_register();
    uapi_at_channel_write_register(AT_UART_PORT, at_write_func);
    uapi_at_plt_register_cmd(uapi_get_plt_at_table(), uapi_get_plt_table_size());
    uapi_at_bt_register_cmd(uapi_get_bt_at_table(), uapi_get_bt_table_size());
    at_write_func("at cmd init.\r\n");
}

uint32_t uapi_at_plt_register_cmd(const at_cmd_entry_t *cmd_tbl, uint16_t cmd_num)
{
    uint32_t ret = ERRCODE_FAIL;

    if ((cmd_tbl == NULL) || (cmd_num == 0)) {
        return ERRCODE_INVALID_PARAM;
    }

    ret = uapi_at_cmd_table_register(cmd_tbl, (uint32_t)cmd_num, AT_PARA_MAX_STRUCT_NUM);
    if (ret != ERRCODE_SUCC) {
        osal_printk("plt at register ret = %x \r\n", ret);
    }
    return ret;
}

uint32_t uapi_at_bt_register_cmd(const at_cmd_entry_t *cmd_tbl, uint16_t cmd_num)
{
    uint32_t ret = ERRCODE_FAIL;

    if ((cmd_tbl == NULL) || (cmd_num == 0)) {
        return ERRCODE_INVALID_PARAM;
    }

    ret = uapi_at_cmd_table_register(cmd_tbl, (uint32_t)cmd_num, AT_PARA_MAX_STRUCT_NUM);
    if (ret != ERRCODE_SUCC) {
        osal_printk("bt at register ret = %x \r\n", ret);
    }
    return ret;
}

__attribute__((weak)) at_cmd_entry_t* uapi_get_plt_at_table(void)
{
    return NULL;
}

__attribute__((weak)) uint32_t uapi_get_plt_table_size(void)
{
    return 0;
}

__attribute__((weak)) at_cmd_entry_t *uapi_get_bt_at_table(void)
{
    return NULL;
}

__attribute__((weak)) uint32_t uapi_get_bt_table_size(void)
{
    return 0;
}