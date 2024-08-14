/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: BLE UART Sample Source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-07-20, Create file. \n
 */
#include "cmsis_os2.h"
#include "common_def.h"
#include "osal_debug.h"
#include "app_init.h"
#include "uart.h"

#if defined(CONFIG_SAMPLE_SUPPORT_BLE_UART_SERVER)
#include "bts_gatt_server.h"
#include "ble_uart_server/ble_uart_server.h"
#elif defined(CONFIG_SAMPLE_SUPPORT_BLE_UART_CLIENT)
#include "bts_gatt_client.h"
#include "ble_uart_client/ble_uart_client.h"
#endif /* CONFIG_SAMPLE_SUPPORT_BLE_UART_CLIENT */

#define BLE_UART_TASK_STACK_SIZE 4096
#define BLE_UART_TASK_PRIO (osPriority_t)(17)
#define BLE_UART_TASK_DURATION_MS 10000
#define BLE_UART_BT_STACK_POWER_MS 10000

#if defined(CONFIG_SAMPLE_SUPPORT_BLE_UART_SERVER)
static void ble_uart_read_int_handler(const void *buffer, uint16_t length, bool error)
{
    unused(error);
    if (ble_uart_get_connection_state() != 0) {
        ble_uart_server_send_input_report((const uint8_t *)buffer, length);
    }
}

static void *ble_uart_server_task(const char *arg)
{
    unused(arg);
    osDelay(BLE_UART_BT_STACK_POWER_MS);
    ble_uart_server_init();
    errcode_t ret = uapi_uart_register_rx_callback(CONFIG_BLE_UART_BUS,
                                                   UART_RX_CONDITION_FULL_OR_SUFFICIENT_DATA_OR_IDLE,
                                                   1, ble_uart_read_int_handler);
    if (ret != ERRCODE_SUCC) {
        osal_printk("Register uart callback fail.");
        return NULL;
    }

    while (1) {
        osDelay(BLE_UART_TASK_DURATION_MS);
    }

    return NULL;
}
#elif defined(CONFIG_SAMPLE_SUPPORT_BLE_UART_CLIENT)
static void ble_uart_read_int_handler(const void *buffer, uint16_t length, bool error)
{
    unused(error);
    static uint16_t write_handle = 0;
    uapi_uart_write(CONFIG_BLE_UART_BUS, (uint8_t *)buffer, length, 0);
    osal_printk("ble_uart_read_int_handler length = %d\n", length);
    if (write_handle == 0) {
        write_handle = ble_uart_get_write_vlaue_handle();
    }
    if (write_handle != 0) {
        ble_uart_client_write_req(buffer, length, write_handle);
    }
}

static void *ble_uart_client_task(const char *arg)
{
    unused(arg);
    osDelay(BLE_UART_BT_STACK_POWER_MS);
    ble_uart_client_init();
    osDelay(BLE_UART_BT_STACK_POWER_MS);
    errcode_t ret = uapi_uart_register_rx_callback(CONFIG_BLE_UART_BUS,
                                                   UART_RX_CONDITION_FULL_OR_SUFFICIENT_DATA_OR_IDLE,
                                                   1, ble_uart_read_int_handler);
    if (ret != ERRCODE_SUCC) {
        osal_printk("Register uart callback fail.");
        return NULL;
    }
    while (1) {
        osDelay(BLE_UART_TASK_DURATION_MS);
    }
    return NULL;
}
#endif

static void ble_uart_entry(void)
{
    osThreadAttr_t attr;

    attr.name = "BLEUartTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = BLE_UART_TASK_STACK_SIZE;
    attr.priority = BLE_UART_TASK_PRIO;
#if defined(CONFIG_SAMPLE_SUPPORT_BLE_UART_SERVER)
    if (osThreadNew((osThreadFunc_t)ble_uart_server_task, NULL, &attr) == NULL) {
        /* Create task fail. */
    }
#elif defined(CONFIG_SAMPLE_SUPPORT_BLE_UART_CLIENT)
    if (osThreadNew((osThreadFunc_t)ble_uart_client_task, NULL, &attr) == NULL) {
        /* Create task fail. */
    }
#endif /* CONFIG_SAMPLE_SUPPORT_BLE_UART_CLIENT */
}

/* Run the ble_uart_entry. */
app_run(ble_uart_entry);