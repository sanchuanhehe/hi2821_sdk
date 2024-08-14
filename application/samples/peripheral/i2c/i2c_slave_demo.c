/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: I2C Sample Source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-05-25, Create file. \n
 */
#include "pinctrl.h"
#include "i2c.h"
#include "errcode.h"
#include "osal_debug.h"
#include "cmsis_os2.h"
#include "app_init.h"
#if defined(CONFIG_I2C_SUPPORT_DMA) && (CONFIG_I2C_SUPPORT_DMA == 1)
#include "dma.h"
#endif

#define I2C_SLAVE_ADDR                    0x8
#define I2C_SET_BAUDRATE                  500000

#if defined(CONFIG_I2C_SUPPORT_DMA) && (CONFIG_I2C_SUPPORT_DMA == 1)
#define I2C_DMA_TRANSFER_DELAY_MS         500
#endif

#define I2C_TASK_STACK_SIZE               0x1000
#define I2C_TASK_DURATION_MS              100
#define I2C_TASK_PRIO                     (osPriority_t)(17)

static void app_i2c_init_pin(void)
{
    /* I2C pinmux. */
    uapi_pin_set_mode(CONFIG_I2C_SCL_SLAVE_PIN, CONFIG_I2C_SLAVE_PIN_MODE);
    uapi_pin_set_mode(CONFIG_I2C_SDA_SLAVE_PIN, CONFIG_I2C_SLAVE_PIN_MODE);
}

void *i2c_slave_task(const char *arg)
{
    unused(arg);
    i2c_data_t data = { 0 };

    uint32_t baudrate = I2C_SET_BAUDRATE;
    uint16_t dev_addr = I2C_SLAVE_ADDR;

#if defined(CONFIG_I2C_SUPPORT_DMA) && (CONFIG_I2C_SUPPORT_DMA == 1)
    uapi_dma_init();
    uapi_dma_open();
#endif  /* CONFIG_I2C_SUPPORT_DMA */

    /* I2C slave init config. */
    app_i2c_init_pin();
    uapi_i2c_slave_init(CONFIG_I2C_SLAVE_BUS_ID, baudrate, dev_addr);

#if defined(CONFIG_I2C_SUPPORT_INT) && (CONFIG_I2C_SUPPORT_INT == 1)
    uapi_i2c_set_irq_mode(CONFIG_I2C_SLAVE_BUS_ID, 1);
#endif /* CONFIG_I2C_SUPPORT_INT */

    /* I2C data config. */
    uint8_t tx_buff[CONFIG_I2C_TRANSFER_LEN] = { 0 };
    for (uint32_t loop = 0; loop < CONFIG_I2C_TRANSFER_LEN; loop++) {
        tx_buff[loop] = (loop & 0xFF);
    }

    uint8_t rx_buff[CONFIG_I2C_TRANSFER_LEN] = { 0 };
    data.send_buf = tx_buff;
    data.send_len = CONFIG_I2C_TRANSFER_LEN;
    data.receive_buf = rx_buff;
    data.receive_len = CONFIG_I2C_TRANSFER_LEN;

    while (1) {
        osDelay(I2C_TASK_DURATION_MS);
        osal_printk("i2c%d slave receive start!\r\n", CONFIG_I2C_SLAVE_BUS_ID);
        if (uapi_i2c_slave_read(CONFIG_I2C_SLAVE_BUS_ID, &data) == ERRCODE_SUCC) {
            for (uint32_t i = 0; i < data.receive_len; i++) {
                osal_printk("i2c slave receive data is %x\r\n", data.receive_buf[i]);
            }
            osal_printk("i2c%d slave receive succ!\r\n", CONFIG_I2C_SLAVE_BUS_ID);
        }
        osal_printk("i2c%d slave send start!\r\n", CONFIG_I2C_SLAVE_BUS_ID);
#if defined(CONFIG_I2C_SUPPORT_DMA) && (CONFIG_I2C_SUPPORT_DMA == 1)
        osDelay(I2C_DMA_TRANSFER_DELAY_MS);
#endif
        if (uapi_i2c_slave_write(CONFIG_I2C_SLAVE_BUS_ID, &data) == ERRCODE_SUCC) {
            osal_printk("i2c%d slave send succ!\r\n", CONFIG_I2C_SLAVE_BUS_ID);
        }
    }

    return NULL;
}

static void i2c_slave_entry(void)
{
    osThreadAttr_t attr;

    attr.name = "I2cSlaveTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = I2C_TASK_STACK_SIZE;
    attr.priority = I2C_TASK_PRIO;

    if (osThreadNew((osThreadFunc_t)i2c_slave_task, NULL, &attr) == NULL) {
        /* Create task fail. */
    }
}

/* Run the i2c_slave_entry. */
app_run(i2c_slave_entry);