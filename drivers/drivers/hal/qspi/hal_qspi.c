/*
 * Copyright (c) @CompanyNameMagicTag 2018-2020. All rights reserved.
 * Description:   HAL QSPI DRIVER
 * Author: @CompanyNameTag
 * Create:  2018-10-15
 */

#include "hal_qspi.h"
#include "chip_io.h"

#define HAL_QSPI_XIP_DMA_HANDSHAKE_CONNECT_CFG    0x5C000404
#define HAL_QSPI_XIP_DMA_TX_HANDSHAKE_CFG_BIT     8
#define HAL_QSPI_XIP_DMA_TX_HANDSHAKE_CFG_BITFILD 2
#define HAL_QSPI_XIP_DMA_RX_HANDSHAKE_CFG_BIT     10
#define HAL_QSPI_XIP_DMA_RX_HANDSHAKE_CFG_BITFILD 2

#define HAL_QSPI_XIP_INT_CONNECT_CFG     0x5C000408
#define HAL_QSPI_XIP_INT_CONNECT_BIT     8
#define HAL_QSPI_XIP_INT_CONNECT_BITFILD 3

#define HAL_QSPI_XIP_INT_CONNECT_BCPU 8
#define HAL_QSPI_XIP_INT_CONNECT_MCPU 9
#define HAL_QSPI_XIP_INT_CONNECT_DCPU 10
#define HAL_QSPI_DMA_TX_BIT 1
#define HAL_QSPI_DMA_RX_BIT 0

#define HAL_QSPI_MINUMUM_CLK_DIV 2
#define HAL_QSPI_MAXIMUM_CLK_DIV 65534
#define hal_qspi_mhz_to_hz(x)    ((x) * 1000000)

volatile hal_qspi_regs_t *g_hal_qspi_base[QSPI_MAX_NUMBER] = {
    (volatile hal_qspi_regs_t *)QSPI_0_BASE_ADDR,
#if QSPI_MAX_NUMBER > 1
    (volatile hal_qspi_regs_t *)QSPI_1_BASE_ADDR,
#endif
#if QSPI_MAX_NUMBER > 2
    (volatile hal_qspi_regs_t *)QSPI_2_BASE_ADDR,
#endif
#if QSPI_MAX_NUMBER > 3
    (volatile hal_qspi_regs_t *)QSPI_3_BASE_ADDR,
#endif
};

uint32_t hal_qspi_get_tx_fifo_num(qspi_bus_t id)
{
    return (uint32_t)g_hal_qspi_base[id]->qspi_txflr;
}

uint32_t hal_qspi_get_rx_fifo_num(qspi_bus_t id)
{
    return (uint32_t)g_hal_qspi_base[id]->qspi_rxflr;
}

uint32_t hal_qspi_get_state(qspi_bus_t id)
{
    return (uint32_t)g_hal_qspi_base[id]->qspi_sr;
}

uint32_t hal_qspi_read_data(qspi_bus_t id)
{
    return (uint32_t)g_hal_qspi_base[id]->qspi_dr;
}

void hal_qspi_write_data(qspi_bus_t id, uint32_t data)
{
    g_hal_qspi_base[id]->qspi_dr = data;
}

void hal_qspi_baud_set_clk_div(qspi_bus_t id, uint32_t clk_div)
{
    g_hal_qspi_base[id]->qspi_baudr = clk_div;
}

void hal_qspi_set_tx_ftlr(qspi_bus_t id, uint32_t value)
{
    g_hal_qspi_base[id]->qspi_txftlr = value;
}

void hal_qspi_set_rx_ftlr(qspi_bus_t id, uint32_t value)
{
    g_hal_qspi_base[id]->qspi_rxftlr = value;
}

void hal_qspi_set_int_mask(qspi_bus_t id, uint32_t value)
{
    g_hal_qspi_base[id]->qspi_imr = value;
}

void hal_qspi_set_sample_delay(qspi_bus_t id, uint32_t value)
{
    g_hal_qspi_base[id]->qspi_rx_sample_dly = value;
}

void hal_qspi_enable(qspi_bus_t id)
{
    g_hal_qspi_base[id]->qspi_ssiner = 0x1;
}

void hal_qspi_disable(qspi_bus_t id)
{
    g_hal_qspi_base[id]->qspi_ssiner = 0x0;
}

void hal_qspi_slave_enable(qspi_bus_t id)
{
    g_hal_qspi_base[id]->qspi_ser = 0x1;
}

void hal_qspi_slave_disable(qspi_bus_t id)
{
    g_hal_qspi_base[id]->qspi_ser = 0x0;
}

void hal_qspi_ctlr0_cfg(qspi_bus_t id, uint32_t cfg)
{
    g_hal_qspi_base[id]->qspi_ctrlr0 = cfg;
}
void hal_qspi_ctlr1_cfg(qspi_bus_t id, uint32_t cfg)
{
    g_hal_qspi_base[id]->qspi_ctrlr1 = cfg;
}

void hal_qspi_spi_ctlr0_cfg(qspi_bus_t id, uint32_t cfg)
{
    g_hal_qspi_base[id]->qspi_spi_ctrlr0 = cfg;
}

void hal_qspi_dmardlr_cfg(qspi_bus_t id, uint32_t cfg)
{
    g_hal_qspi_base[id]->qspi_dmardlr = cfg;
}

void hal_qspi_dmatdlr_cfg(qspi_bus_t id, uint32_t cfg)
{
    g_hal_qspi_base[id]->qspi_dmatdlr = cfg;
}

void hal_qspi_dmacr_cfg(qspi_bus_t id, uint32_t cfg)
{
    g_hal_qspi_base[id]->qspi_dmacr = cfg;
}

void hal_qspi_dma_control(qspi_bus_t id, hal_qspi_dma_control_t control)
{
    switch (control) {
        case HAL_QSPI_DMA_CONTROL_RX_ENABLE:
            g_hal_qspi_base[id]->qspi_dmacr |= 1 << HAL_QSPI_DMA_RX_BIT;
            break;
        case HAL_QSPI_DMA_CONTROL_TX_ENABLE:
            g_hal_qspi_base[id]->qspi_dmacr |= 1 << HAL_QSPI_DMA_TX_BIT;
            break;
        case HAL_QSPI_DMA_CONTROL_DISABLE:
            g_hal_qspi_base[id]->qspi_dmacr = 0;
            break;
        case HAL_QSPI_DMA_CONTROL_RX_DISABLE:
            g_hal_qspi_base[id]->qspi_dmacr &= ~(1 << HAL_QSPI_DMA_RX_BIT);
            break;
        case HAL_QSPI_DMA_CONTROL_TX_DISABLE:
            g_hal_qspi_base[id]->qspi_dmacr &= ~(1 << HAL_QSPI_DMA_TX_BIT);
            break;
        default:
            break;
    }
}

void hal_qspi_enter_sleep_mode(void)
{
    return;
}

void hal_qspi_exit_sleep_mode(void)
{
    return;
}

void hal_qspi_config_dma_interrupt_connection(cores_t core)
{
    UNUSED(core);
}

void hal_qspi_config_dma_interrupt_disconnection(cores_t core)
{
    UNUSED(core);
}

void hal_qspi_config_dma_connection(cores_t core)
{
    UNUSED(core);
}