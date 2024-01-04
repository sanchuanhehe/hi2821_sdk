/*
 * Copyright (c) @CompanyNameMagicTag 2018-2021. All rights reserved.
 * Description:  HAL QSPI DRIVER
 * Author: @CompanyNameTag
 * Create:  2018-10-15
 */

#ifndef HAL_QSPI_H
#define HAL_QSPI_H

#include "core.h"
/** @defgroup connectivity_drivers_hal_qspi QSPI
  * @ingroup  connectivity_drivers_hal
  * @{
  */

#define QSPI_RX_FIFO_DEPTH  16
#define QSPI_TX_FIFO_DEPTH  16

#define QSPI_BAUD_CLK_DIV_2  2
#define QSPI_BAUD_CLK_DIV_4  4
#define QSPI_BAUD_CLK_DIV_6  6
#define QSPI_BAUD_CLK_DIV_8  8
#define QSPI_BAUD_CLK_DIV_10 10
#define QSPI_BAUD_CLK_DIV_16 16
#define QSPI_BAUD_CLK_DIV_20 20
#define QSPI_BAUD_CLK_DIV_40 40

#define SPI_FRF_OFFSET 21

#define DFR_32_FRAME_08BITS 0x7
#define DFR_32_FRAME_16BITS 0xF
#define DFR_32_FRAME_24BITS 0x17
#define DFR_32_FRAME_32BITS 0x1F

/**
 * @brief  Number of wait cycles in Dual/Quad/Octal mode between control frames transmit and data reception.
 */
#define WAIT_CYCLES_0  0x0
#define WAIT_CYCLES_1  0x1
#define WAIT_CYCLES_2  0x2
#define WAIT_CYCLES_3  0x3
#define WAIT_CYCLES_4  0x4
#define WAIT_CYCLES_5  0x5
#define WAIT_CYCLES_6  0x6
#define WAIT_CYCLES_7  0x7
#define WAIT_CYCLES_8  0x8
#define WAIT_CYCLES_9  0x9
#define WAIT_CYCLES_10 0x0A

/**
 * @brief  This bit defines length of address to be transmitted. For example, QSPI_ADDR_L_8:8-bit address width.
 */
typedef enum QSPI_ADDR_L_E {
    QSPI_ADDR_L_0 = 0,
    QSPI_ADDR_L_4,
    QSPI_ADDR_L_8,
    QSPI_ADDR_L_12,
    QSPI_ADDR_L_16,
    QSPI_ADDR_L_20,
    QSPI_ADDR_L_24,
    QSPI_ADDR_L_28,
    QSPI_ADDR_L_32,
    QSPI_ADDR_L_36,
    QSPI_ADDR_L_40,
    QSPI_ADDR_L_44,
    QSPI_ADDR_L_48,
    QSPI_ADDR_L_52,
    QSPI_ADDR_L_56,
    QSPI_ADDR_L_60
} qspi_addr_l_m_t;

/**
 * @brief  Selects data frame format for Transmitting/Receiving the data.
 */
typedef enum SPI_FRF_E {
    SPI_STD_SPI_FRF    = 0,         //!< Standard spi frame format.
    SPI_DUAL_SPI_FRF   = 1,         //!< Dual spi frame format.
    SPI_QUAD_SPI_FRF   = 2,         //!< Quad spi frame format.
    SPI_OCTAL_SPI_FRF3 = 3,         //!< Octal spi frame format.
    SPI_FRF_M_MAX,
} spi_frf_m_t;

/**
 * @brief  Selects the mode of transfer for serial communication.
 */
typedef enum SPI_TMOD_E {
    SPI_TX_AND_RX_TMOD = 0,         //!< Transmit&Receive mode.
    SPI_TX_ONLY_TMOD   = 1,         //!< Transmit only mode.
    SPI_RX_ONLY_TMOD   = 2,         //!< Receive only mode.
    SPI_EEPROM_TMOD    = 3,         //!< Eeprom read mode.
} spi_tmod_m_t;

/**
 * @brief  Dual/Quad/Octal mode instruction length in bits.
 */
typedef enum SPI_INST_L_E {
    SPI_INST_L_0_BITS  = 0,        //!< 0-bit(no instruction).
    SPI_INST_L_4_BITS  = 1,        //!< 4-bit instruction.
    SPI_INST_L_8_BITS  = 2,        //!< 8-bit instruction.
    SPI_INST_L_16_BITS = 3,        //!< 16-bit instruction.
} spi_inst_l_m_t;

/**
 * @brief  Address and Instruction transfer format.
 */
typedef enum QSPI_TRANS_TYPE_L_E {
    //!< Instruction and address will be sent in standard spi mode.
    QSPI_TRANS_MODE0 = 0,
    //!< Instruction will be sent in standard spi mode and address will be sent in the mode specified by CTRLR0.SPI_FRF.
    QSPI_TRANS_MODE1 = 1,
    //!< Both instruction and address will be sent in the mode specified by CTRLR0.SPI_FRF.
    QSPI_TRANS_MODE2 = 2,
} qspi_trans_type_l_m_t;

typedef union _hal_spi_interface_ctrlr0_reg {
    struct {
        uint32_t dfs : 4;
        uint32_t frf : 2;
        uint32_t scph : 1;
        uint32_t scpol : 1;
        uint32_t tmod : 2;
        uint32_t slv_oe : 1;
        uint32_t srl : 1;
        uint32_t cfs : 4;
        uint32_t dfs_32 : 5;
        uint32_t spi_frf : 2;
        uint32_t rsvd_clt0_23 : 1;
        uint32_t sste : 1;
        uint32_t rsvd0 : 7;
    } d;
    uint32_t d32;
} hal_spi_interface_ctrlr0_reg;

typedef union _hal_spi_interface_spi_ctrlr0_reg {
    struct {
        uint32_t trans_type : 2;
        uint32_t addr_l : 4;
        uint32_t rsvd_spi_ctrlr0_6_7 : 2;
        uint32_t inst_l : 2;
        uint32_t rsvd_spi_ctrlr0_10 : 1;
        uint32_t wait_cycles : 5;
        uint32_t spi_ddr_en : 1;
        uint32_t inst_ddr_en : 1;
        uint32_t spi_rxds_en : 1;
        uint32_t rsvd_spi_ctrlr0 : 13;
    } d;
    uint32_t d32;
} hal_spi_interface_spi_ctrlr0_reg;

typedef enum {
    QSPI_INTERFACE_SINGLE_BIDIR,
    QSPI_INTERFACE_DOUBLE,
    QSPI_INTERFACE_QUAD,
    QSPI_INTERFACE_SINGLE_UNIDIR,
    QSPI_INTERFACE_NONE,
} qspi_interface_t;

typedef enum {
    QSPI_RET_OK,      //!< The operation has completed successfully.
    QSPI_RET_BUSY,    //!< The operation is in process.
    QSPI_RET_UNINIT,  //!< The operation fail because of not config.
    QSPI_RET_TIMEOUT, //!< The operation timeout.
    QSPI_RET_ERROR,   //!< The operation fail because of other reasons.
} qspi_ret_t;

typedef enum {
    HAL_QSPI_XIP_CONNECT_NONE = 0,
    HAL_QSPI_XIP_CONNECT_BDMA,
    HAL_QSPI_XIP_CONNECT_MDMA,
    HAL_QSPI_XIP_CONNECT_DDMA,
} hal_qspi_xip_dma_connect_t;

typedef enum {
    HAL_QSPI_DMA_CONTROL_DISABLE = 0,
    HAL_QSPI_DMA_CONTROL_RX_ENABLE = 1,
    HAL_QSPI_DMA_CONTROL_RX_DISABLE = 2,
    HAL_QSPI_DMA_CONTROL_TX_ENABLE = 3,
    HAL_QSPI_DMA_CONTROL_TX_DISABLE = 4,
    HAL_QSPI_DMA_CONTROL_MAX_NUM,
    HAL_QSPI_DMA_CONTROL_NONE = HAL_QSPI_DMA_CONTROL_MAX_NUM,
} hal_qspi_dma_control_t;

/* QSPI CTRL REGS */
typedef struct {
    uint32_t qspi_ctrlr0;
    uint32_t qspi_ctrlr1;
    uint32_t qspi_ssiner;
    uint32_t qspi_mwcr;
    uint32_t qspi_ser;
    uint32_t qspi_baudr;
    uint32_t qspi_txftlr;
    uint32_t qspi_rxftlr;
    uint32_t qspi_txflr;
    uint32_t qspi_rxflr;
    uint32_t qspi_sr;
    uint32_t qspi_imr;
    uint32_t qspi_isr;
    uint32_t qspi_risr;
    uint32_t qspi_txoicr;
    uint32_t qspi_rxoicr;
    uint32_t qspi_rxuicr;
    uint32_t qspi_msticr;
    uint32_t qspi_icr;
    uint32_t qspi_dmacr;
    uint32_t qspi_dmatdlr;
    uint32_t qspi_dmardlr;
    uint32_t qspi_idr;
    uint32_t qspi_ssi_version_id;
    uint32_t qspi_dr;
    uint32_t reserved[35];
    uint32_t qspi_rx_sample_dly;
    uint32_t qspi_spi_ctrlr0;
    uint32_t qspi_txd_drive_edge;
} hal_qspi_regs_t;

/**
 * @brief  Get the number of valid data entries in the transmit fifo memory.
 * @param  id qspi id.
 * @return The number of valid data entries in the transmit fifo memory.
 */
uint32_t hal_qspi_get_tx_fifo_num(qspi_bus_t id);

/**
 * @brief  Get the number of valid data entries in receive fifo memory.
 * @param  id qspi id.
 * @return The number of valid data entries in receive fifo memory.
 */
uint32_t hal_qspi_get_rx_fifo_num(qspi_bus_t id);

/**
 * @brief  Get current transfer status, fifo status, and any transmission/reception errors that may have occurred.
 * @param  id qspi id.
 * @return The value of the status register.
 */
uint32_t hal_qspi_get_state(qspi_bus_t id);

/**
 * @brief  Reads data from the data register.
 * @param  id qspi id.
 * @return The value of the data register.
 */
uint32_t hal_qspi_read_data(qspi_bus_t id);

/**
 * @brief  Write data to the data register.
 * @param  id qspi id.
 * @param  data The data you want to write.
 */
void hal_qspi_write_data(qspi_bus_t id, uint32_t data);

/**
 * @brief  Set baud rate.
 * @param  id qspi id.
 * @param  clk_div Clock divider.
 */
void hal_qspi_baud_set_clk_div(qspi_bus_t id, uint32_t clk_div);

/**
 * @brief  QSPI enable.
 * @param  id qspi id.
 */
void hal_qspi_enable(qspi_bus_t id);

/**
 * @brief  QSPI disable.
 * @param  id qspi id.
 */
void hal_qspi_disable(qspi_bus_t id);

/**
 * @brief  Set configuration register 0.
 * @param  id qspi id.
 * @param  cfg The value of configuration register 0.
 */
void hal_qspi_ctlr0_cfg(qspi_bus_t id, uint32_t cfg);

/**
 * @brief  Set spi control register.
 * @param  id qspi id.
 * @param  cfg The value of spi control register.
 */
void hal_qspi_spi_ctlr0_cfg(qspi_bus_t id, uint32_t cfg);

/**
 * @brief  Set configuration register 1.
 * @param  id qspi id.
 * @param  cfg The value of configuration register 1.
 */
void hal_qspi_ctlr1_cfg(qspi_bus_t id, uint32_t cfg);

/**
 * @brief  Qspi is configured as a slave.
 * @param  id qspi id.
 */
void hal_qspi_slave_enable(qspi_bus_t id);

/**
 * @brief  Qspi disable slave function.
 * @param  id qspi id.
 */
void hal_qspi_slave_disable(qspi_bus_t id);

/**
 * @brief  Set transmit fifo threshold level.
 * @param  id qspi id.
 * @param  value The value of the transmit fifo threshold level.
 */
void hal_qspi_set_tx_ftlr(qspi_bus_t id, uint32_t value);

/**
 * @brief  Set receive fifo threshold level.
 * @param  id qspi id.
 * @param  value The value of the receive fifo threshold level.
 */
void hal_qspi_set_rx_ftlr(qspi_bus_t id, uint32_t value);

/**
 * @brief  Set interrupt mask register.
 * @param  id qspi id.
 * @param  value The value of interrupt mask register.
 */
void hal_qspi_set_int_mask(qspi_bus_t id, uint32_t value);

/**
 * @brief  Set rx sample delay register.
 * @param  id qspi id.
 * @param  value The number of delayed clocks.
 */
void hal_qspi_set_sample_delay(qspi_bus_t id, uint32_t value);

/**
 * @brief  Set qspi dma receive data level register; watermark level = cfg + 1.
 * @param  id qspi id.
 * @param  cfg The value of receive data level register.
 */
void hal_qspi_dmardlr_cfg(qspi_bus_t id, uint32_t cfg);

/**
 * @brief  Set qspi dma transmit data level register.
 * @param  id qspi id.
 * @param  cfg The value of transmit data level register.
 */
void hal_qspi_dmatdlr_cfg(qspi_bus_t id, uint32_t cfg);

/**
 * @brief  Set qspi dma control register.
 * @param  id qspi id.
 * @param  cfg The value of dma control register.
 */
void hal_qspi_dmacr_cfg(qspi_bus_t id, uint32_t cfg);

/**
 * @brief  Enabel/Disable qspi dma transmit/receive.
 * @param  id qspi id.
 * @param  control The value of structure variable(hal_qspi_dma_control_t).
 */
void hal_qspi_dma_control(qspi_bus_t id, hal_qspi_dma_control_t control);

/**
 * @brief  Qspi enter sleep mode.
 */
void hal_qspi_enter_sleep_mode(void);

/**
 * @brief  Qspi exit sleep mode.
 */
void hal_qspi_exit_sleep_mode(void);

/**
 * @brief  Connect qspi interrupt to a core, interrupt can connect multiple core.
 * @param  core The core that interrupt signal connection.
 */
void hal_qspi_config_dma_interrupt_connection(cores_t core);

/**
 * @brief  Disconnect qspi interrupt to a core, interrupt can connect multiple core.
 * @param  core The core that interrrupt signal disconnect.
 */
void hal_qspi_config_dma_interrupt_disconnection(cores_t core);

/**
 * @brief  Connect qspi dma tx/rx hardware handshake to a core, handshake only can connect one core.
 * @param  core The core that qspi dma tx/rx hardware handshake connect.
 */
void hal_qspi_config_dma_connection(cores_t core);

/**
  * @}
  */
#endif
