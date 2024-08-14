/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Test mouse sensor source \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-2-09, Create file. \n
 */

#include "spi.h"
#include "spi_porting.h"
#include "pinctrl.h"
#include "gpio.h"
#include "tcxo.h"
#include "high_speed_mouse.h"
#include "lpm_dev_ops.h"
#include "mouse_spi.h"

#define POWER_UP_MS                      50
#define SPI_MOUSE_SEND_DATA_LEN          2
#define SHORTEST_TIME_BETWEEN_WRITE_US   45
#ifdef CHIP_FPGA
#define SPI_FREQUENCY                    4
#else
#define SPI_FREQUENCY                    4
#endif
#define HS_MOUSE_WRITE_8BIT              0x80
#define INIT_DELAY_MS                    20
#define SPI_TIMEOUT                      100
#define MOUSE_OPRATION_DELAY_US          1000

void spi_porting_clock_en(void) __attribute__((weak, alias("mouse_spi_none")));

void mouse_spi_none(void)
{
}

static void spi_gpio_init(void)
{
    uapi_pin_set_mode(SPI_PIN_CS, (pin_mode_t)HAL_PIO_FUNC_GPIO);
    uapi_gpio_set_dir(SPI_PIN_CS, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(SPI_PIN_CS, 1);
    uapi_tcxo_delay_ms(POWER_UP_MS);
    uapi_gpio_set_val(SPI_PIN_CS, 0);
    uapi_tcxo_delay_ms(INIT_DELAY_MS);
}

static void test_spi_mouse_pinmux(void)
{
    uapi_pin_set_mode(SPI_PIN_MISO, SPI_PIN_MISO_PINMUX);
    uapi_pin_set_mode(SPI_PIN_MOSI, SPI_PIN_MOSI_PINMUX);
    uapi_pin_set_mode(SPI_PIN_CLK, SPI_PIN_CLK_PINMUX);
    uapi_pin_set_mode(SPI_PIN_CS, SPI_PIN_CS_PINMUX);
#if defined(CONFIG_PINCTRL_SUPPORT_IE)
    uapi_pin_set_ie(SPI_PIN_MISO, PIN_IE_1);
#endif
}

void mouse_spi_open(void)
{
    spi_attr_t config = { 0 };
    spi_extra_attr_t ext_config = { 0 };
    ext_config.sspi_param.wait_cycles = 0x10;
    config.freq_mhz = SPI_FREQUENCY;
    config.is_slave = false;
    config.frame_size = HAL_SPI_FRAME_SIZE_8;
    config.slave_num = 1;
    config.spi_frame_format = HAL_SPI_FRAME_FORMAT_STANDARD;
    config.bus_clk = SPI_CLK_FREQ;
    config.frame_format = 0;
    config.tmod = 0;
    config.sste = 0;
    spi_gpio_init();
    test_spi_mouse_pinmux();

    spi_porting_clock_en();
    errcode_t err = uapi_spi_init(MOUSE_SPI, &config, &ext_config);
    if (err != ERRCODE_SUCC) {
        print("err:%x\r\n", err);
    }
}

uint8_t mouse_read_reg(uint8_t reg_addr)
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

void mouse_burst_read(uint8_t reg_addr, uint8_t *buf, uint8_t lenth)
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

void mouse_write_reg(uint8_t reg_addr, uint8_t val)
{
    uint8_t cmd_send[SPI_MOUSE_SEND_DATA_LEN] = {reg_addr, val};
    spi_xfer_data_t mouse_send_xfer = { 0 };
    mouse_send_xfer.tx_buff = cmd_send;
    mouse_send_xfer.tx_bytes = SPI_MOUSE_SEND_DATA_LEN;
    spi_porting_set_tx_mode(MOUSE_SPI);
    uapi_spi_master_write(MOUSE_SPI, &mouse_send_xfer, SPI_TIMEOUT);
}

void mouse_opration(const spi_mouse_cfg_t *cfg, int16_t lenth)
{
    uint8_t cmd_recv_value;

    for (int16_t i = 0; i < lenth; i++) {
        print("%d/%d: ", i + 1, lenth);
        uapi_tcxo_delay_us(SHORTEST_TIME_BETWEEN_WRITE_US); // Sensor SPI time between write commands
        switch (cfg[i].dict) {
            case READ:
                cmd_recv_value = mouse_read_reg(cfg[i].opt.addr);
                print("r 0x%02X: 0x%02X\r\n", cfg[i].opt.addr, cmd_recv_value);
                break;
            case WRITE:
                mouse_write_reg(cfg[i].opt.addr | HS_MOUSE_WRITE_8BIT, cfg[i].opt.value);
                uapi_tcxo_delay_us(MOUSE_OPRATION_DELAY_US);
                cmd_recv_value = mouse_read_reg(cfg[i].opt.addr);
                print("w 0x%02X: 0x%02X, ", cfg[i].opt.addr, cfg[i].opt.value);
                print("r 0x%02X %s.\r\n",
                    cmd_recv_value, cfg[i].opt.value == cmd_recv_value ? "y" : "n");
                break;
            case DELAY:
                print("d %d us\r\n", cfg[i].opt.addr);
                uapi_tcxo_delay_us(cfg[i].opt.addr);
                break;
            default:
                return;
        }
    }
}
