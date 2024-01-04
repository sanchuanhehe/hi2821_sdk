/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides spi port UT \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-08-18， Create file. \n
 */
#include "hal_spi_v151_regs_def.h"
#include "hal_spi_v151_regs_op.h"
#include "hal_spi_v151.h"
#include "hal_spi.h"
#include "osal_interrupt.h"
#include "chip_io.h"
#include "std_def.h"
#include "platform_core.h"
#include "osal_interrupt.h"
#include "chip_core_irq.h"
#include "arch_port.h"
#if defined(CONFIG_SPI_SUPPORT_LPC)
#include "pm_clock.h"
#endif
#include "spi_porting.h"

/** -----------------------------------------------------
 *          Reg Bit fild Max and Shift Value
 * ---------------------------------------------------- */
 /**
  * @brief  CTRLR0 : Control register 0.
  */
#define HAL_SPI_CTRLR0_REG_MAX       0xFFFFFFFF
#define HAL_SPI_CE_LIN_TOGGLE_ENABLE (BIT(24))

/**
 * @brief  FRF : Frame format.
 */
#define HAL_SPI_FRAME_FORMAT_MAX   0x03
#define HAL_SPI_FRAME_FORMAT_SHIFT 0x15

/**
 * @brief  DFS : Data frame size.
 */
#define HAL_SPI_FRAME_SIZE_MAX   0x1F
#define HAL_SPI_FRAME_SIZE_SHIFT 0x10
#define HAL_SPI_FRAME_SIZE_8     0x07
#define HAL_SPI_FRAME_SIZE_16    0x0F
#define HAL_SPI_FRAME_SIZE_32    0x1F

/**
 * @brief  TMOD : Trans mode.
 */
#define HAL_SPI_TRANS_MODE_MAX    0x03
#define HAL_SPI_TRANS_MODE_SHIFT  0x08
#define HAL_SPI_TRANS_MODE_TXRX   0x00
#define HAL_SPI_TRANS_MODE_TX     0x01
#define HAL_SPI_TRANS_MODE_RX     0x02
#define HAL_SPI_TRANS_MODE_EEPROM 0x03

/**
 * @brief  CLOCK : SCPOL and SCPH.
 */
#define HAL_SPI_CLKS_MODE_MAX   0x03
#define HAL_SPI_CLKS_MODE_SHIFT 0x06

/**
 * @brief  CTRLR1 : Control register 1.
 */
#define HAL_SPI_RECEIVED_DATA_REG_MAX 0xFFFF

/**
 * @brief  SSIENR : SSI enable register.
 */
#define HAL_SPI_ENABLE 0x01

/**
 * @brief  SER : Slave enable register.
 */
#define HAL_SPI_SLAVE_ENABLE_REG_MAX 0xFFFFFFFF

/**
 * @brief  BAUDR : Baud rate select.
 */
#define HAL_SPI_CLK_DIV_REG_MAX 0xFFFF

/**
 * @brief  SR : Status register.
 */
#define HAL_SPI_RX_FIFO_FULL_FLAG      (BIT(4))
#define HAL_SPI_RX_FIFO_NOT_EMPTY_FLAG (BIT(3))
#define HAL_SPI_TX_FIFO_EMPTY_FLAG     (BIT(2))
#define HAL_SPI_TX_FIFO_NOT_FULL_FLAG  (BIT(1))
#define HAL_SPI_BUSY_FLAG              (BIT(0))

/**
 * @brief IMR : Interrupt mask register.
 */
#define HAL_SPI_INTERRUPT_REG_MAX 0x3F


/**
 * @brief  SPI_CTRLR0:  SPI control register.
 */
#define HAL_QSPI_CTRLR0_REG_MAX 0xFFFFFFFF

/**
 * @brief  Wait cycles.
 */
#define HAL_QSPI_WAIT_CYCLE_MAX   0x1F
#define HAL_QSPI_WAIT_CYCLE_SHIFT 0x0B
#define HAL_QSPI_WAIT_CYCLE_6     0x06
#define HAL_QSPI_WAIT_CYCLE_4     0x04
#define HAL_QSPI_WAIT_CYCLE_2     0x02

#define HAL_SPI_RX_SAMPLE_DLY_MAX   0xFF
#define HAL_SPI_RX_SAMPLE_DLY_SHIFT 0

/**
 * @brief  Command length.
 */
#define HAL_QSPI_CMD_LENTH_MAX   0x03
#define HAL_QSPI_CMD_LENTH_SHIFT 0x08
#define HAL_QSPI_CMD_LENTH_8     0x02

/**
 * @brief  Address length.
 */
#define HAL_QSPI_ADDR_LENTH_MAX   0x0F
#define HAL_QSPI_ADDR_LENTH_SHIFT 0x02
#define HAL_QSPI_ADDR_LENTH_24    0x06

/**
 * @brief  Trans type.
 */
#define HAL_QSPI_TRANS_TYPES_MAX          0x03
#define HAL_QSPI_TRANS_TYPES_SHIFT        0x00
#define HAL_QSPI_TRANS_TYPES_CMD_S_ADDR_Q 0x01

#define HAL_SPI_BUS_MAX_NUM (3)

#define HAL_SPI_DR_REG_SIZE 36

/** -----------------------------------------------------
 *          SPI Register Address
 * ----------------------------------------------------
 */
#define HAL_SPI_BUS_1_DEVICE_MODE_SET_BIT       0x00
#define HAL_SPI_BUS_1_DEVICE_MODE_SET_BIT_MAX   0x01
#define HAL_SPI_BUS_1_DEVICE_MODE_SET_BIT_SHIFT 0x00
#define HAL_SPI_BUS_2_DEVICE_MODE_MASTER        0x01
#define HAL_SPI_BUS_2_DEVICE_MODE_SET_BIT_MAX   0x01
#define HAL_SPI_BUS_2_DEVICE_MODE_SET_BIT_SHIFT 0x01

#define MCU_PERP_SPI_CR                    (0x5200055c)
#define MCU_PERP_SPI_CR_CLK_EN             (0x3)

#define HAL_QSPI_DMA_CFG        (*(volatile unsigned short *)(0x5C000404))
#define HAL_QSPI_1_DMA_CFG_MASK 0x0FU
#define HAL_QSPI_1_DMA_SEC_CORE 0x05
#define HAL_QSPI_1_DMA_APP_CORE 0x0A
#define HAL_QSPI_1_DMA_DSP_CORE 0x0F
#define HAL_QSPI_2_DMA_CFG_MASK 0xF0U
#define HAL_QSPI_2_DMA_SEC_CORE 0x50
#define HAL_QSPI_2_DMA_APP_CORE 0xA0
#define HAL_QSPI_2_DMA_DSP_CORE 0xF0

#define HAL_QSPI_INT_CFG             (*(volatile unsigned short *)(0x5C000408))
#define HAL_QSPI_1_INT_SEC_CORE_MASK (BIT(0))
#define HAL_QSPI_1_INT_APP_CORE_MASK (BIT(2))
#define HAL_QSPI_2_INT_SEC_CORE_MASK (BIT(4))
#define HAL_QSPI_2_INT_APP_CORE_MASK (BIT(6))

#define HAL_SPI_DATA_FRAME_SIZE_BIT     16
#define HAL_SPI_DATA_FRAME_SIZE_BITFILD 5
#define hal_spi_frame_size_trans_to_frame_bytes(x) (((x) + 1) >> 0x03)

#define HAL_SPI_MINUMUM_CLK_DIV 2
#define HAL_SPI_MAXIMUM_CLK_DIV 65534

#define hal_spi_mhz_to_hz(x) ((x) * 1000000)

#define HAL_SPI_RXDS_EN     BIT(18)
#define HAL_SPI_INST_DDR_EN BIT(17)
#define HAL_SPI_DDR_EN      BIT(16)
#define HAL_SPI_INST_L_POSE 8
#define HAL_SPI_ADDR_L_POSE 2

#define HAL_SPI_RSVD_NONE   0
#define HAL_SPI_RSVD_X8     0
#define HAL_SPI_RSVD_X8_X8  1
#define HAL_SPI_RSVD_X16    0x11
#define HAL_SPI_CLK_DIV_2   2
#define HAL_SPI_CLK_DIV_4   4
#define HAL_SPI_CLK_DIV_20  20

#define HAL_SPI_BUS_1_DEVICE_MODE_SET_BIT       0x00
#define HAL_SPI_BUS_1_DEVICE_MODE_SET_BIT_MAX   0x01
#define HAL_SPI_BUS_1_DEVICE_MODE_SET_BIT_SHIFT 0x00
#define HAL_SPI_BUS_2_DEVICE_MODE_MASTER        0x01
#define HAL_SPI_BUS_2_DEVICE_MODE_SET_BIT_MAX   0x01
#define HAL_SPI_BUS_2_DEVICE_MODE_SET_BIT_SHIFT 0x01

#define HAL_SPI_BUS_LINE_NUM_SHIFT              4
#define SPI_REC_OFFSET 1

#define SPI_DMA_TX_DATA_LEVEL_4     4
#define QSPI_DMA_TX_DATA_LEVEL_8    8

#define DMA_CFG_OFFSET              0x2c0
#define SPI2_TX_HANDSHARK_BIT       2
#define SPI2_RX_HANDSHARK_BIT       3

spi_v151_regs_t *g_spi_base_addrs[SPI_BUS_MAX_NUM] = {
    (spi_v151_regs_t *)SPI_BUS_0_BASE_ADDR,
    (spi_v151_regs_t *)SPI_BUS_1_BASE_ADDR,
    (spi_v151_regs_t *)SPI_BUS_2_BASE_ADDR,
};

void spi_port_register_hal_funcs(void)
{
    hal_spi_register_funcs(SPI_BUS_0, hal_spi_v151_funcs_get());
    hal_spi_register_funcs(SPI_BUS_1, hal_spi_v151_funcs_get());
    hal_spi_register_funcs(SPI_BUS_2, hal_spi_v151_funcs_get());
}

uintptr_t spi_porting_base_addr_get(spi_bus_t index)
{
    return (uintptr_t)g_spi_base_addrs[index];
}

uint32_t spi_porting_max_slave_select_get(spi_bus_t bus)
{
    unused(bus);
    return (uint32_t)SPI_SLAVE1;
}

void spi_porting_set_device_mode(spi_bus_t bus, spi_mode_t mode)
{
    if (mode == SPI_MODE_MASTER) {
        reg32_setbit(HAL_SPI_DEVICE_MODE_SET_REG, bus);
    } else {
        reg32_clrbit(HAL_SPI_DEVICE_MODE_SET_REG, bus);
    }
}

spi_mode_t spi_porting_get_device_mode(spi_bus_t bus)
{
    spi_mode_t mode = SPI_MODE_NONE;
    mode = reg32_getbit(HAL_SPI_DEVICE_MODE_SET_REG, bus);
    if (mode == 0) {
        return SPI_MODE_SLAVE;
    }
    return SPI_MODE_MASTER;
}

void spi_porting_set_sspi_mode(spi_bus_t bus, bool val)
{
    if (val) {
        reg32_setbit(HAL_SPI_DEVICE_MODE_SET_REG, bus + HAL_SPI_BUS_LINE_NUM_SHIFT);
    } else {
        reg32_clrbit(HAL_SPI_DEVICE_MODE_SET_REG, bus + HAL_SPI_BUS_LINE_NUM_SHIFT);
    }
}

bool spi_porting_is_sspi_mode(spi_bus_t bus)
{
    return (bool)reg32_getbit(HAL_SPI_DEVICE_MODE_SET_REG, bus + HAL_SPI_BUS_LINE_NUM_SHIFT);
}

void spi_porting_set_sspi_waite_cycle(spi_bus_t bus, uint32_t waite_cycle)
{
    g_spi_base_addrs[bus]->spi_rsvd = waite_cycle;
}

void spi_porting_set_rx_mode(spi_bus_t bus, uint16_t num)
{
    if (spi_porting_is_sspi_mode(bus)) {
        return;
    }
    g_spi_base_addrs[bus]->spi_er = 0;
    hal_spi_v151_spi_ctra_set_trsm(bus, HAL_SPI_TRANS_MODE_EEPROM);
    hal_spi_v151_spi_ctrb_set_nrdf(bus, num - 1);
    g_spi_base_addrs[bus]->spi_er = 1;
}

void spi_porting_set_tx_mode(spi_bus_t bus)
{
    if (spi_porting_is_sspi_mode(bus)) {
        return;
    }
    g_spi_base_addrs[bus]->spi_er = 0;
    hal_spi_v151_spi_ctra_set_trsm(bus, HAL_SPI_TRANS_MODE_TX);
    g_spi_base_addrs[bus]->spi_er = 1;
}

void spi_porting_clock_en(void)
{
    writel(MCU_PERP_SPI_CR, MCU_PERP_SPI_CR_CLK_EN);
}

// opi使用cfbb_spi接口适配
bool hal_opi_set_fifo_threshold(spi_bus_t bus, uint32_t threshold)
{
    if (bus >= HAL_SPI_BUS_MAX_NUM) {
        return false;
    }

    g_spi_base_addrs[bus]->spi_twlr = threshold;
    g_spi_base_addrs[bus]->spi_rwlr = threshold;
    return true;
}

bool hal_opi_reset_config(spi_bus_t bus)
{
    if (bus >= HAL_SPI_BUS_MAX_NUM) {
        return false;
    }

    // reset register
    g_spi_base_addrs[bus]->spi_ctra &= ~HAL_SPI_CTRLR0_REG_MAX;
    g_spi_base_addrs[bus]->spi_enhctl &= ~HAL_QSPI_CTRLR0_REG_MAX;
    return true;
}

bool hal_opi_set_frame_size(spi_bus_t bus, uint32_t frame_size)
{
    if (bus >= HAL_SPI_BUS_MAX_NUM) {
        return false;
    }

    reg32_setbits(&g_spi_base_addrs[bus]->spi_ctra, HAL_SPI_DATA_FRAME_SIZE_BIT,
                  HAL_SPI_DATA_FRAME_SIZE_BITFILD, frame_size);

    return true;
}

bool hal_opi_set_frame_format(spi_bus_t bus, hal_spi_frame_format_t frame_format)
{
    if (bus >= HAL_SPI_BUS_MAX_NUM) {
        return false;
    }

    switch (frame_format) {
        case HAL_SPI_FRAME_FORMAT_STANDARD:
            g_spi_base_addrs[bus]->spi_ctra |= (HAL_SPI_FRAME_FORMAT_STANDARD << HAL_SPI_FRAME_FORMAT_SHIFT);
            g_spi_base_addrs[bus]->spi_rsvd = HAL_SPI_RSVD_NONE;
            break;
        case HAL_SPI_FRAME_FORMAT_QUAD:
            g_spi_base_addrs[bus]->spi_ctra |= (HAL_SPI_FRAME_FORMAT_QUAD << HAL_SPI_FRAME_FORMAT_SHIFT);
            g_spi_base_addrs[bus]->spi_enhctl |= (HAL_QSPI_CMD_LENTH_8 << HAL_QSPI_CMD_LENTH_SHIFT);
            g_spi_base_addrs[bus]->spi_enhctl |= (HAL_QSPI_ADDR_LENTH_24 << HAL_QSPI_ADDR_LENTH_SHIFT);
            g_spi_base_addrs[bus]->spi_enhctl |= (HAL_QSPI_TRANS_TYPES_CMD_S_ADDR_Q << HAL_QSPI_TRANS_TYPES_SHIFT);
            g_spi_base_addrs[bus]->spi_rsvd = HAL_SPI_RSVD_NONE;
            break;
#if (SPI_WITH_OPI == YES)
        case HAL_SPI_FRAME_FORMAT_OCTAL:
            g_spi_base_addrs[bus]->spi_ctra |= (HAL_SPI_FRAME_FORMAT_OCTAL << HAL_SPI_FRAME_FORMAT_SHIFT);
            g_spi_base_addrs[bus]->spi_rsvd = HAL_SPI_RSVD_X8;
            break;
        case HAL_SPI_FRAME_FORMAT_DOUBLE_OCTAL:
            g_spi_base_addrs[bus]->spi_ctra |= (HAL_SPI_FRAME_FORMAT_OCTAL << HAL_SPI_FRAME_FORMAT_SHIFT);
            g_spi_base_addrs[bus]->spi_rsvd = HAL_SPI_RSVD_X8_X8;
            break;
        case HAL_SPI_FRAME_FORMAT_SIXT:
            g_spi_base_addrs[bus]->spi_ctra |= (HAL_SPI_FRAME_FORMAT_OCTAL << HAL_SPI_FRAME_FORMAT_SHIFT);
            g_spi_base_addrs[bus]->spi_rsvd = HAL_SPI_RSVD_X16;
            break;
#endif
        default:
            return false;
    }
    return true;
}

bool hal_opi_set_trans_mode(spi_bus_t bus, uint32_t tmod)
{
    if (bus >= HAL_SPI_BUS_MAX_NUM) {
        return false;
    }

    g_spi_base_addrs[bus]->spi_ctra &= ~(HAL_SPI_TRANS_MODE_MAX << HAL_SPI_TRANS_MODE_SHIFT);
    g_spi_base_addrs[bus]->spi_ctra |= (tmod << HAL_SPI_TRANS_MODE_SHIFT);
    return true;
}

bool hal_opi_set_freq(spi_bus_t bus, uint32_t clk_in_mhz)
{
    if (bus >= HAL_SPI_BUS_MAX_NUM) {
        return false;
    }

    uint32_t clk_div;
    uint32_t bus_clk;
#ifdef BUILD_APPLICATION_ATE
    bus_clk = 0;
#else
    bus_clk = 0;
#endif

    clk_div = (uint32_t)(bus_clk / hal_spi_mhz_to_hz(clk_in_mhz));
    if (clk_div < HAL_SPI_MINUMUM_CLK_DIV) {
        clk_div = HAL_SPI_MINUMUM_CLK_DIV;
    }

    if (clk_div > HAL_SPI_MAXIMUM_CLK_DIV) {
        clk_div = HAL_SPI_MAXIMUM_CLK_DIV;
    }

    g_spi_base_addrs[bus]->spi_brs &= ~HAL_SPI_CLK_DIV_REG_MAX;
#ifdef PRE_ASIC
    g_spi_base_addrs[bus]->spi_brs |= clk_div;
#else
    g_spi_base_addrs[bus]->spi_brs |= HAL_SPI_CLK_DIV_20;
    UNUSED(clk_div);
#endif

#if (SPI_WITH_OPI == YES)
    if (bus == OPI_BUS) {
        g_spi_base_addrs[bus]->spi_brs = HAL_SPI_CLK_DIV_2;
    }
#endif

#ifdef BUILD_APPLICATION_ATE
    if (bus == SPI_BUS_3) {
        g_spi_base_addrs[bus]->spi_brs = HAL_SPI_CLK_DIV_4;
    }
#endif
    return true;
}

void hal_opi_set_inst_len(spi_bus_t bus, hal_spi_inst_len_t inst_len)
{
    g_spi_base_addrs[bus]->spi_enhctl &= (~(HAL_SPI_INST_LEN_MAX << HAL_SPI_INST_L_POSE));
    g_spi_base_addrs[bus]->spi_enhctl |= ((uint32_t)inst_len << HAL_SPI_INST_L_POSE);
}

void hal_opi_set_addr_len(spi_bus_t bus, hal_spi_addr_len_t addr_len)
{
    g_spi_base_addrs[bus]->spi_enhctl &= (~(0xf << HAL_SPI_ADDR_L_POSE));
    g_spi_base_addrs[bus]->spi_enhctl |= ((uint32_t)addr_len << HAL_SPI_ADDR_L_POSE);
}

void hal_opi_cmd_trans_mode(spi_bus_t bus, uint32_t mode)
{
    g_spi_base_addrs[bus]->spi_enhctl &= (~0x3);
    g_spi_base_addrs[bus]->spi_enhctl |= (uint32_t)mode;
}

bool hal_opi_set_received_data_num(spi_bus_t bus, uint32_t number)
{
    if (bus >= HAL_SPI_BUS_MAX_NUM) {
        return false;
    }
    g_spi_base_addrs[bus]->spi_ctrb &= ~HAL_SPI_RECEIVED_DATA_REG_MAX;
    g_spi_base_addrs[bus]->spi_ctrb = number - SPI_REC_OFFSET;
    return true;
}

bool hal_opi_set_tx_fifo_threshold(spi_bus_t bus, uint32_t threshold)
{
    if (bus >= HAL_SPI_BUS_MAX_NUM) {
        return false;
    }
    g_spi_base_addrs[bus]->spi_twlr = threshold;
    return true;
}

void hal_opi_ddr_en(spi_bus_t bus, bool on)
{
    if (on) {
        g_spi_base_addrs[bus]->spi_enhctl |= (HAL_SPI_DDR_EN | HAL_SPI_RXDS_EN | HAL_SPI_INST_DDR_EN);
    } else {
        g_spi_base_addrs[bus]->spi_enhctl &= (~(HAL_SPI_DDR_EN | HAL_SPI_RXDS_EN | HAL_SPI_INST_DDR_EN));
    }
}

bool hal_qspi_set_wait_cycles(spi_bus_t bus, uint32_t wait_cyc)
{
    if (bus >= HAL_SPI_BUS_MAX_NUM) {
        return false;
    }
    g_spi_base_addrs[bus]->spi_enhctl &= ~(HAL_QSPI_WAIT_CYCLE_MAX << HAL_QSPI_WAIT_CYCLE_SHIFT);
    g_spi_base_addrs[bus]->spi_enhctl |= (wait_cyc << HAL_QSPI_WAIT_CYCLE_SHIFT);
    return true;
}

bool hal_opi_set_trans_type(spi_bus_t bus, hal_spi_trans_type_t trans_type)
{
    if (bus >= HAL_SPI_BUS_MAX_NUM) {
        return false;
    }

    g_spi_base_addrs[bus]->spi_enhctl &= (~0x3);
    g_spi_base_addrs[bus]->spi_enhctl |= (uint32_t)trans_type;
    return true;
}

void hal_opi_disable_slave(spi_bus_t bus)
{
    // Disable all slave
    g_spi_base_addrs[bus]->spi_slenr &= ~HAL_SPI_SLAVE_ENABLE_REG_MAX;
}

uint32_t hal_opi_read_data(spi_bus_t bus)
{
    if (bus >= HAL_SPI_BUS_MAX_NUM) {
        return false;
    }
    return (g_spi_base_addrs[bus]->spi_drnm[0]);
}

bool hal_opi_write_data(spi_bus_t bus, uint32_t data)
{
    if (bus >= HAL_SPI_BUS_MAX_NUM) {
        return false;
    }
    g_spi_base_addrs[bus]->spi_drnm[0] = data;
    return true;
}

bool hal_opi_select_slave(spi_bus_t bus, uint32_t slave_num)
{
    if (bus >= HAL_SPI_BUS_MAX_NUM) {
        return false;
    }

    // Disable all slave
    g_spi_base_addrs[bus]->spi_slenr &= ~HAL_SPI_SLAVE_ENABLE_REG_MAX;
    g_spi_base_addrs[bus]->spi_slenr = BIT(slave_num);

    return true;
}

bool hal_opi_disable(spi_bus_t bus)
{
    if (bus >= HAL_SPI_BUS_MAX_NUM) {
        return false;
    }
    g_spi_base_addrs[bus]->spi_er &= ~HAL_SPI_ENABLE;
    return true;
}


bool hal_opi_enable(spi_bus_t bus)
{
    if (bus >= HAL_SPI_BUS_MAX_NUM) {
        return false;
    }

    g_spi_base_addrs[bus]->spi_ctra &= ~HAL_SPI_CE_LIN_TOGGLE_ENABLE;
    g_spi_base_addrs[bus]->spi_er |= HAL_SPI_ENABLE;
    return true;
}

bool hal_opi_tx_fifo_is_not_full(spi_bus_t bus)
{
    if (bus >= HAL_SPI_BUS_MAX_NUM) {
        return false;
    }
    return (bool)hal_spi_v151_spi_wsr_get_tfnf(bus);
}

bool hal_opi_tx_fifo_is_empty(spi_bus_t bus)
{
    if (bus >= HAL_SPI_BUS_MAX_NUM) {
        return false;
    }
    return (g_spi_base_addrs[bus]->spi_wsr & HAL_SPI_TX_FIFO_EMPTY_FLAG);
}

bool hal_opi_rx_fifo_is_full(spi_bus_t bus)
{
    if (bus >= HAL_SPI_BUS_MAX_NUM) {
        return false;
    }
    return (g_spi_base_addrs[bus]->spi_wsr & HAL_SPI_RX_FIFO_FULL_FLAG);
}

bool hal_opi_rx_fifo_is_not_empty(spi_bus_t bus)
{
    if (bus >= HAL_SPI_BUS_MAX_NUM) {
        return false;
    }
    return (g_spi_base_addrs[bus]->spi_wsr & HAL_SPI_RX_FIFO_NOT_EMPTY_FLAG);
}

bool hal_opi_is_busy(spi_bus_t bus)
{
    if (bus >= HAL_SPI_BUS_MAX_NUM) {
        return false;
    }
    return (g_spi_base_addrs[bus]->spi_wsr & HAL_SPI_BUSY_FLAG);
}

bool hal_opi_set_rx_sample_dly(spi_bus_t bus, uint8_t delay)
{
    if (bus >= HAL_SPI_BUS_MAX_NUM) {
        return false;
    }

    g_spi_base_addrs[bus]->spi_rsdr &= ~(HAL_SPI_RX_SAMPLE_DLY_MAX << HAL_SPI_RX_SAMPLE_DLY_SHIFT);
    g_spi_base_addrs[bus]->spi_rsdr |= (delay << HAL_SPI_RX_SAMPLE_DLY_SHIFT);
    return true;
}

bool hal_opi_set_dma_rx_data_level(spi_bus_t bus, uint32_t data_level)
{
    if (bus >= HAL_SPI_BUS_MAX_NUM) {
        return false;
    }

    g_spi_base_addrs[bus]->spi_drdl = data_level;
    return true;
}

bool hal_opi_dma_control(spi_bus_t bus, hal_spi_dma_control_t operation)
{
    if (bus >= HAL_SPI_BUS_MAX_NUM) {
        return false;
    }

    if (operation >= HAL_SPI_DMA_CONTROL_MAX_NUM) {
        return false;
    }

    g_spi_base_addrs[bus]->spi_dcr = (uint32_t)operation;
    return true;
}

uint32_t spi_porting_lock(spi_bus_t bus)
{
    unused(bus);
    return osal_irq_lock();
}

void spi_porting_unlock(spi_bus_t bus, uint32_t irq_sts)
{
    unused(bus);
    osal_irq_restore(irq_sts);
}

uint8_t spi_port_get_dma_trans_dest_handshaking(spi_bus_t bus)
{
    switch (bus) {
        case SPI_BUS_0:
            return (uint8_t)DMA_HANDSHAKE_SPI_BUS_0_TX;
        case SPI_BUS_1:
            return (uint8_t)DMA_HANDSHAKE_SPI_BUS_1_TX;
        case SPI_BUS_2:
            writel(M_CTL_RB_BASE + DMA_CFG_OFFSET, SPI2_TX_HANDSHARK_BIT);     // i2s default, need change to spi2
            return (uint8_t)DMA_HANDSHAKE_SPI_BUS_2_TX;
        default:
            return (uint8_t)HAL_DMA_HANDSHAKING_MAX_NUM;
    }
}

uint8_t spi_port_get_dma_trans_src_handshaking(spi_bus_t bus)
{
    switch (bus) {
        case SPI_BUS_0:
            return (uint8_t)DMA_HANDSHAKE_SPI_BUS_0_RX;
        case SPI_BUS_1:
            return (uint8_t)DMA_HANDSHAKE_SPI_BUS_1_RX;
        case SPI_BUS_2:
            writel(M_CTL_RB_BASE + DMA_CFG_OFFSET, SPI2_RX_HANDSHARK_BIT);     // i2s default, need change to spi2
            return (uint8_t)DMA_HANDSHAKE_SPI_BUS_2_RX;
        default:
            return (uint8_t)HAL_DMA_HANDSHAKING_MAX_NUM;
    }
}

uint8_t spi_port_tx_data_level_get(spi_bus_t bus)
{
    switch (bus) {
        case SPI_BUS_0:
        case SPI_BUS_1:
        case SPI_BUS_2:
            return (uint8_t)SPI_DMA_TX_DATA_LEVEL_4;
        default:
            return 0;
    }
}

uint8_t spi_port_rx_data_level_get(spi_bus_t bus)
{
    unused(bus);
    return 0;
}

#if defined(CONFIG_SPI_SUPPORT_LPC)
void spi_port_clock_enable(spi_bus_t bus, bool on)
{
    clock_control_type_t control_type;
    clock_mclken_aperp_type_t aperp_type;

    switch (bus) {
        case SPI_BUS_0:
            aperp_type = CLOCK_APERP_SPI0_M_CLKEN;
            break;

        case SPI_BUS_1:
            aperp_type = CLOCK_APERP_SPI1_M_CLKEN;
            break;

        case SPI_BUS_2:
            aperp_type = CLOCK_APERP_SPI2_M_CLKEN;
            break;
        default:
            return;
    }

    if (on) {
        control_type = CLOCK_CONTROL_MCLKEN_ENABLE;
    } else {
        control_type = CLOCK_CONTROL_MCLKEN_DISABLE;
    }
    clock_ccrg_spi_enable((clock_ccrg_spi_used_t)(bus), on);
    uapi_clock_control(control_type, aperp_type);
}
#endif
