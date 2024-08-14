/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Mouse sensorspi source \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-08-01, Create file. \n
 */

#include "spi.h"
#include "spi_porting.h"
#include "pinctrl.h"
#include "gpio.h"
#include "tcxo.h"
#include "mouse_sensor.h"
#include "mouse_sensor_port.h"
#include "lpm_dev_ops.h"
#include "mouse_sensor_spi.h"

#define POWER_UP_MS                      50
#define SPI_MOUSE_SEND_DATA_LEN          2
#define SHORTEST_TIME_BETWEEN_WRITE_US   45
#define HS_MOUSE_WRITE_8BIT              0x80
#define INIT_DELAY_MS                    20
#define SPI_TIMEOUT                      100
#define MOUSE_OPRATION_DELAY_US          1000

void spi_porting_clock_en(void) __attribute__((weak, alias("spi_porting_clock_en_none")));

void spi_porting_clock_en_none(void)
{
}

/* Use a GPIO instead of SPI CS interface. */
static void mouse_spi_init_cs(void)
{
    uapi_pin_set_mode(CONFIG_MOUSE_PIN_SPI_CS, (pin_mode_t)HAL_PIO_FUNC_GPIO);
    uapi_gpio_set_dir(CONFIG_MOUSE_PIN_SPI_CS, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(CONFIG_MOUSE_PIN_SPI_CS, 1);
    uapi_tcxo_delay_ms(POWER_UP_MS);
    uapi_gpio_set_val(CONFIG_MOUSE_PIN_SPI_CS, 0);
    uapi_tcxo_delay_ms(INIT_DELAY_MS);
}

static void mouse_spi_set_pinctrl(void)
{
    uapi_pin_set_mode(CONFIG_MOUSE_PIN_SPI_MISO, SPI_PIN_MISO_PINMUX);
    uapi_pin_set_mode(CONFIG_MOUSE_PIN_SPI_MOSI, SPI_PIN_MOSI_PINMUX);
    uapi_pin_set_mode(CONFIG_MOUSE_PIN_SPI_CLK, SPI_PIN_CLK_PINMUX);
    uapi_pin_set_mode(CONFIG_MOUSE_PIN_SPI_CS, SPI_PIN_CS_PINMUX);
#if defined(CONFIG_PINCTRL_SUPPORT_IE)
    uapi_pin_set_ie(CONFIG_MOUSE_PIN_SPI_MISO, PIN_IE_1);
#endif
}


void mouse_sensor_spi_open(uint8_t frame_format, uint8_t clk_polarity, uint8_t clk_phase, uint8_t mhz)
{
    uapi_spi_deinit(MOUSE_SPI);
    spi_attr_t config = { 0 };
    spi_extra_attr_t ext_config = { 0 };
    ext_config.sspi_param.wait_cycles = 0x10;
    config.freq_mhz = mhz;
    config.is_slave = false;
    config.frame_size = HAL_SPI_FRAME_SIZE_8;
    config.slave_num = 1;
    config.spi_frame_format = HAL_SPI_FRAME_FORMAT_STANDARD;
    config.bus_clk = SPI_CLK_FREQ;
    config.frame_format = frame_format;
    config.tmod = 0;
    config.sste = 0;
    config.clk_phase = clk_phase;
    config.clk_polarity = clk_polarity;
    mouse_spi_init_cs();
    mouse_spi_set_pinctrl();

    spi_porting_clock_en();
    (void)uapi_spi_init(MOUSE_SPI, &config, &ext_config);
}

uint8_t mouse_spi_read_reg(uint8_t reg_addr)
{
    uint8_t addr = reg_addr;
    uint8_t value;
    spi_xfer_data_t mouse_recv_xfer = { 0 };
    mouse_recv_xfer.rx_buff = &value;
    mouse_recv_xfer.rx_bytes = 1;
    mouse_recv_xfer.tx_buff = &addr;
    mouse_recv_xfer.tx_bytes = 1;
    spi_porting_set_rx_mode(MOUSE_SPI, 1);
    uapi_spi_master_writeread(MOUSE_SPI, &mouse_recv_xfer, SPI_TIMEOUT);
    return value;
}

void mouse_spi_burst_read(uint8_t reg_addr, uint8_t *buf, uint8_t lenth)
{
    uint8_t addr = reg_addr;
    spi_xfer_data_t mouse_recv_xfer = { 0 };
    mouse_recv_xfer.rx_buff = buf;
    mouse_recv_xfer.rx_bytes = lenth;
    mouse_recv_xfer.tx_buff = &addr;
    mouse_recv_xfer.tx_bytes = 1;

    spi_porting_set_rx_mode(MOUSE_SPI, lenth);
    uapi_spi_master_writeread(MOUSE_SPI, &mouse_recv_xfer, SPI_TIMEOUT);
}

void mouse_spi_write_reg(uint8_t reg_addr, uint8_t val)
{
    uint8_t cmd_send[SPI_MOUSE_SEND_DATA_LEN] = {reg_addr, val};
    spi_xfer_data_t mouse_send_xfer = { 0 };
    mouse_send_xfer.tx_buff = cmd_send;
    mouse_send_xfer.tx_bytes = SPI_MOUSE_SEND_DATA_LEN;
    spi_porting_set_tx_mode(MOUSE_SPI);
    uapi_spi_master_write(MOUSE_SPI, &mouse_send_xfer, SPI_TIMEOUT);
}

void mouse_sensor_spi_opration(const spi_mouse_cfg_t *cfg, int16_t lenth)
{
    uint8_t cmd_recv_value;
    unused(cmd_recv_value);
    if (cfg == NULL) {
        return ;
    }
    for (int16_t i = 0; i < lenth; i++) {
        uapi_tcxo_delay_us(SHORTEST_TIME_BETWEEN_WRITE_US); // Sensor SPI time between write commands
        switch (cfg[i].opration) {
            case READ:
                cmd_recv_value = mouse_spi_read_reg(cfg[i].addr);
                break;
            case WRITE:
                mouse_spi_write_reg(cfg[i].addr | HS_MOUSE_WRITE_8BIT, cfg[i].value);
                uapi_tcxo_delay_us(MOUSE_OPRATION_DELAY_US);
                cmd_recv_value = mouse_spi_read_reg(cfg[i].addr);
                break;
            case DELAY:
                uapi_tcxo_delay_us(cfg[i].addr);
                break;
            case RUN_FUNC:
                if (cfg[i].func == NULL) {
                    return;
                }
                cfg[i].func();
                break;
            default:
                return;
        }
    }
}
