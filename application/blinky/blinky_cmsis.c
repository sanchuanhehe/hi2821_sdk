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
#include "osal_debug.h"
#include "pinctrl.h"
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

#define SPI_TASK_STACK_SIZE 0x1000
#define SPI_TASK_DURATION_MS 500
#define SPI_TASK_PRIO (osPriority_t)(17)

#define SPI_TRANSFER_LEN 20

static void app_spi_init_pin(void) {
  uapi_pin_set_mode(S_MGPIO11, HAL_PIO_SPI0_TXD);
}

static void app_spi_master_init_config(void) {
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
  uapi_dma_init();
  uapi_dma_open();
  uapi_spi_init(SPI_BUS_0, &config, &ext_config);
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
    if (uapi_spi_master_write(SPI_BUS_0, &data, 0xFFFFFFFF) ==
        ERRCODE_SUCC) {
      osal_printk("spi%d master send succ!\r\n", SPI_BUS_0);
    } else {
      continue;
    }
  }

  return NULL;
}

static void spi_master_entry(void) {
  osThreadAttr_t attr;

  attr.name = "SpiMasterTask";
  attr.attr_bits = 0U;
  attr.cb_mem = NULL;
  attr.cb_size = 0U;
  attr.stack_mem = NULL;
  attr.stack_size = SPI_TASK_STACK_SIZE;
  attr.priority = SPI_TASK_PRIO;

  if (osThreadNew((osThreadFunc_t)spi_master_task, NULL, &attr) == NULL) {
    /* Create task fail. */
  }
}

/* Run the spi_master_entry. */
app_run(spi_master_entry);