/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides HAL SPI \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-08-12, Create file. \n
 */
#include "common_def.h"
#include "hal_spi.h"

uintptr_t g_hal_spis_regs[SPI_BUS_MAX_NUM] = { 0 };
hal_spi_funcs_t *g_hal_spis_funcs[SPI_BUS_MAX_NUM] = { NULL };

errcode_t hal_spi_regs_init(void)
{
    for (int32_t i = 0; i < SPI_BUS_MAX_NUM; i++) {
        if (spi_porting_base_addr_get(i) == 0) {
            return ERRCODE_UART_REG_ADDR_INVALID;
        }
        g_hal_spis_regs[i] = spi_porting_base_addr_get(i);
    }
    return ERRCODE_SUCC;
}

void hal_spi_regs_deinit(void)
{
    for (uint32_t i = 0; i < SPI_BUS_MAX_NUM; i++) {
        g_hal_spis_regs[i] = 0;
    }
}

errcode_t hal_spi_register_funcs(spi_bus_t bus, hal_spi_funcs_t *funcs)
{
    if (bus >= SPI_BUS_MAX_NUM || funcs == NULL) {
        return ERRCODE_INVALID_PARAM;
    }
    g_hal_spis_funcs[bus] = funcs;
    return ERRCODE_SUCC;
}

errcode_t hal_spi_unregister_funcs(spi_bus_t bus)
{
    if (bus >= SPI_BUS_MAX_NUM) {
        return ERRCODE_INVALID_PARAM;
    }
    g_hal_spis_funcs[bus] = NULL;
    return ERRCODE_SUCC;
}

hal_spi_funcs_t *hal_spi_get_funcs(spi_bus_t bus)
{
    if (bus >= SPI_BUS_MAX_NUM) {
        return NULL;
    }
    return g_hal_spis_funcs[bus];
}