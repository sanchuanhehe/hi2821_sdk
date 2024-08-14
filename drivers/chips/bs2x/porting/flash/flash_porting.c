/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Provides flash porting \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-10-26， Create file. \n
 */
#include <common_def.h>
#include "flash.h"
#include "debug_print.h"
#include "chip_io.h"
#include "soc_osal.h"
#if defined(CONFIG_FLASH_SUPPORT_LPC)
#include "pm_clock.h"
#include "pm_pmu.h"
#endif
#include "pinctrl.h"
#include "spi_porting.h"
#include "flash_porting.h"

static uint32_t g_flash_size = 0;
extern flash_cfg_t g_flash_config[FLASH_MAX];

static flash_cfg_t g_flash_porting_config[FLASH_MAX] = {
    {
        .isinit = 0,
        .flash_manufacturer = FLASH_MANUFACTURER_MAX,
        .unique_id = 0,
        .bus = SPI_BUS_0,
        .mode = HAL_SPI_FRAME_FORMAT_STANDARD,
        .is_xip = false,
        .attr = {
            .is_slave = false,
            .slave_num = 1,
            .bus_clk = SPI_CLK_FREQ,
            .freq_mhz = 4,
            .clk_polarity = (uint32_t)SPI_CFG_CLK_CPOL_0,
            .clk_phase = (uint32_t)SPI_CFG_CLK_CPHA_0,
            .frame_format = SPI_CFG_FRAME_FORMAT_MOTOROLA_SPI,
            .spi_frame_format = (uint32_t)HAL_SPI_FRAME_FORMAT_STANDARD,
            .frame_size = (uint32_t)HAL_SPI_FRAME_SIZE_8,
            .sste = SPI_CFG_SSTE_DISABLE,
        },
        .extra_attr = {
            .tx_use_dma = false,
            .rx_use_dma = false,
            .qspi_param = { 0 },
        },
    },
};

void flash_porting_get_config(flash_id_t id, flash_cfg_t *config)
{
    config[id] = g_flash_porting_config[id];
}

static void flash_0_enter_qspi_clk_div(flash_id_t id, spi_attr_t *attr)
{
    UNUSED(id);
    UNUSED(attr);
}

static set_clk_div_t g_flash_enter_qspi_clk_div[FLASH_MAX] = {
    flash_0_enter_qspi_clk_div,
};

void flash_porting_set_enter_qspi_clk_div(flash_id_t id, spi_attr_t *attr)
{
    g_flash_enter_qspi_clk_div[id](id, attr);
}

static void flash_0_power_on(flash_id_t id)
{
    unused(id);
}

static power_on_t g_flash_power_on_array[FLASH_MAX] = {
    flash_0_power_on,
};

void flash_porting_power_on(flash_id_t id)
{
    g_flash_power_on_array[id](id);
}

void flash_porting_spi_lock_create(flash_id_t id)
{
    unused(id);
}

void flash_porting_spi_lock_delete(flash_id_t id)
{
    unused(id);
}

uint32_t flash_porting_spi_lock(flash_id_t id)
{
    unused(id);
    return osal_irq_lock();
}

void flash_porting_spi_unlock(flash_id_t id, uint32_t status)
{
    unused(id);
    osal_irq_restore(status);
}

uint32_t flash_config_get_end(void)
{
#if !defined(BUILD_APPLICATION_ROM) || defined(SUPPORT_EXTERN_FLASH)
    flash_info_t flash_info;
    if (g_flash_size == 0) {
        uapi_flash_get_info(0, &flash_info);
        g_flash_size = flash_info.flash_size;
    }
    return (g_flash_size + FLASH_START);
#else
    return FLASH_END;
#endif
}

uint16_t flash_config_get_pages(void)
{
#if !defined(BUILD_APPLICATION_ROM) || defined(SUPPORT_EXTERN_FLASH)
    flash_info_t flash_info;
    if (g_flash_size == 0) {
        uapi_flash_get_info(0, &flash_info);
        g_flash_size = flash_info.flash_size;
    }
    return (uint16_t)(g_flash_size / FLASH_PAGE_SIZE);
#else
    return FLASH_PAGES;
#endif
}

uint32_t flash_config_get_start_addr(void)
{
    return FLASH_START;
}

uint16_t flash_get_page(uint32_t address)
{
    return (uint16_t)((address - FLASH_START) / FLASH_PAGE_SIZE);
}

errcode_t test_uapi_flash_init(flash_id_t id)
{
    if (g_flash_config[id].isinit == 1) {
        return ERRCODE_SUCC;
    }

    flash_porting_get_config(id, g_flash_config);

    /* spi钩子和基地址 */
    uapi_spi_init(g_flash_config[id].bus, &(g_flash_config[id].attr), &(g_flash_config[id].extra_attr));
    g_flash_config[id].isinit = 1;
    return ERRCODE_SUCC;
}

bool flash_porting_switch_flash_id(flash_id_t *id, uint32_t *base_addr, uint32_t addr)
{
    if (addr >= DATA_FLASH_START && addr < DATA_FLASH_MAX_END) {
        *id = (flash_id_t)0;
        *base_addr = (uint32_t)DATA_FLASH_START;
        return true;
    }
    return false;
}

#if defined(CONFIG_FLASH_SUPPORT_LPC)
void flash_port_clock_enable(bool on)
{
    if (on) {
        uapi_clock_control(CLOCK_CONTROL_MCLKEN_ENABLE, CLOCK_APERP_SPI0_M_CLKEN);
    } else {
        uapi_clock_control(CLOCK_CONTROL_MCLKEN_DISABLE, CLOCK_APERP_SPI0_M_CLKEN);
    }
}
#endif

void flash_porting_pinmux_cfg(flash_id_t id)
{
    if (id >= FLASH_MAX) {
        return;
    }

    if (id == FLASH_0) {
        uapi_pin_set_mode(S_MGPIO11, HAL_PIO_SPI0_TXD);
        uapi_pin_set_mode(S_MGPIO2, HAL_PIO_SPI0_CS0);
        uapi_pin_set_mode(S_MGPIO3, HAL_PIO_SPI0_RXD);
        uapi_pin_set_mode(S_MGPIO9, HAL_PIO_SPI0_SCLK);
#if defined(CONFIG_PINCTRL_SUPPORT_IE)
        uapi_pin_set_ie(S_MGPIO3, PIN_IE_1);
#endif
    }
}