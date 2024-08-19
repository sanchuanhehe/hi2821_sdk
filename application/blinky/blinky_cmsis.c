/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: SPI Sample Source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-06-25, Create file. \n
 */
#include "app_init.h"
#include "cmsis_os2.h"
#include "dma.h"
#include "osal_debug.h"
#include "pinctrl.h"
#include "soc_osal.h"
#include "spi.h"

#define SPI_SLAVE_NUM 1
#define SPI_FREQUENCY 2
#define SPI_CLK_POLARITY 0
#define SPI_CLK_PHASE 0
#define SPI_FRAME_FORMAT 0
#define SPI_FRAME_FORMAT_STANDARD 0
#define SPI_FRAME_SIZE_8 0x1f
#define SPI_TMOD 0
#define SPI_WAIT_CYCLES 0x10

#define SPI_TASK_STACK_SIZE 0x2000
#define SPI_TASK_DURATION_MS 500
#define SPI_TASK_PRIO OSAL_TASK_PRIORITY_HIGH

#define SPI_TRANSFER_LEN 20

#ifndef debug
#define debug
#endif

static void app_spi_init_pin(void) {
  errcode_t ret = uapi_pin_set_mode(S_MGPIO11, HAL_PIO_SPI0_TXD);
  if (ret != ERRCODE_SUCC) {
    osal_printk("set pin mode failed .\n");
  }
}

static void app_spi_master_init_config(void) {
  // #ifdef debug
  osal_printk("spi%d master init start!\r\n", SPI_BUS_0);//TODO:debug
  // #endif
  spi_attr_t config = {0};
  spi_extra_attr_t ext_config = {0};

  config.is_slave = false;
  config.slave_num = SPI_SLAVE_NUM;
  config.bus_clk = SPI_CLK_FREQ;
  config.freq_mhz = SPI_FREQUENCY;
  config.clk_polarity = SPI_CLK_POLARITY;
  config.clk_phase = SPI_CLK_PHASE;
  config.frame_format = SPI_FRAME_FORMAT;
  config.spi_frame_format = HAL_SPI_FRAME_FORMAT_STANDARD;
  config.frame_size = SPI_FRAME_SIZE_8;
  config.tmod = SPI_TMOD;
  config.sste = 1;

  ext_config.qspi_param.wait_cycles = SPI_WAIT_CYCLES;
  errcode_t ret;
  // ret = uapi_dma_init();
  // if (ret != ERRCODE_SUCC) {
  //   osal_printk("uapi_dma_init failed .\n");
  // }
  // ret = uapi_dma_open();
  // if (ret != ERRCODE_SUCC) {
  //   osal_printk("uapi_dma_init failed .\n");
  // }
  ret = uapi_spi_init(SPI_BUS_0, &config, &ext_config);
  if (ret != ERRCODE_SUCC) {
    osal_printk("uapi_spi_init failed .\n");
    osal_printk("errcode = %d\n", ret);
  } else {
    osal_printk("uapi_spi_init success .\n");
  }
  // #ifdef debug
  osal_printk("spi%d master init end!\r\n", SPI_BUS_0);//TODO:debug
  // #endif
}

static void *spi_master_task(const char *arg) {
  unused(arg);
  /* SPI pinmux. */
  app_spi_init_pin();

  /* SPI master init config. */
  app_spi_master_init_config();

  /* SPI data config. */
  uint8_t tx_data[SPI_TRANSFER_LEN] = {0};
  for (uint32_t loop = 0; loop < SPI_TRANSFER_LEN; loop++) {
    tx_data[loop] = (loop & 0xFF);
  }

  spi_xfer_data_t data = {
      .tx_buff = tx_data,
      .tx_bytes = SPI_TRANSFER_LEN,
  };

  while (1) {
    osDelay(SPI_TASK_DURATION_MS);
    osal_printk("spi%d master send start!\r\n", SPI_BUS_0);
    if (uapi_spi_master_write(SPI_BUS_0, &data, 0xFFFFFFFF) == ERRCODE_SUCC) {
      osal_printk("spi%d master send succ!\r\n", SPI_BUS_0);
    } else {
      continue;
    }
  }

  return NULL;
}

static void spi_master_entry(void) {
  int ret;
  osal_task *taskid;
  // 创建任务调度
  osal_kthread_lock();
  // 创建任务
  taskid = osal_kthread_create((osal_kthread_handler)spi_master_task, NULL,
                               "spi_master_task", SPI_TASK_STACK_SIZE);
  if (taskid == NULL) {
    osal_printk("create spi_master_task failed .\n");
    return;
  }
  ret = osal_kthread_set_priority(taskid, SPI_TASK_PRIO);
  if (ret != OSAL_SUCCESS) {
    osal_printk("set spi_master_task priority failed .\n");
  }
  osal_kthread_unlock();
}

/* Run the spi_master_entry. */
app_run(spi_master_entry);