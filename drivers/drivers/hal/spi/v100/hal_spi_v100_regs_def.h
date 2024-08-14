/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides V100 SPI register \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-08-08, Create file. \n
 */
#ifndef HAL_SPI_V100_REGS_DEF_H
#define HAL_SPI_V100_REGS_DEF_H

#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_spi_v100_regs_def SPI V100 Regs Definition
 * @ingroup  drivers_hal_spi
 * @{
 */

#define HAL_SPI_DR_REG_SIZE 36

/**
 * @brief  This union represents the bit fields in the Control Register 0. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union ctrlr0_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t dfs            :   4;  /*!< <b>Data Frame Size.</b> \n
                                             This register field is only valid when SSI_MAX_XFER_SIZE
                                             is configured to 16. If SSI_MAX_XFER_SIZE is configured to
                                             32, then writing to this field will not have any effect. \n
                                             Selects the data frame length. When the data frame size is
                                             programmed to be less than 16 bits, the receive data are
                                             automatically right-justified by the receive logic, with the
                                             upper bits of the receive FIFO zero-padded. \n
                                             You must right-justify transmit data before writing into the
                                             transmit FIFO. The transmit logic ignores the upper unused
                                             bits when transmitting the data. \n
                                             @note
                                             When SSI_SPI_MODE is either set to "Dual" or
                                             "Quad" or "Octal" mode and SPI_FRF is not set to 2'b00.
                                                - DFS value should be multiple of 2 if SPI_FRF = 01.
                                                - DFS value should be multiple of 4 if SPI_FRF = 10.
                                                - DFS value should be multiple of 8 if SPI_FRF = 11.
                                             @par
                                             <b>Values:</b>
                                                - 0x3 (FRAME_04BITS): 4-bit serial data transfer.
                                                - 0x4 (FRAME_05BITS): 5-bit serial data transfer.
                                                - 0x5 (FRAME_06BITS): 6-bit serial data transfer.
                                                - 0x6 (FRAME_07BITS): 7-bit serial data transfer.
                                                - 0x7 (FRAME_08BITS): 8-bit serial data transfer.
                                                - 0x8 (FRAME_09BITS): 9-bit serial data transfer.
                                                - 0x9 (FRAME_10BITS): 10-bit serial data transfer.
                                                - 0xa (FRAME_11BITS): 11-bit serial data transfer.
                                                - 0xb (FRAME_12BITS): 12-bit serial data transfer.
                                                - 0xc (FRAME_13BITS): 13-bit serial data transfer.
                                                - 0xd (FRAME_14BITS): 14-bit serial data transfer.
                                                - 0xe (FRAME_15BITS): 15-bit serial data transfer.
                                                - 0xf (FRAME_16BITS): 16-bit serial data transfer. */
        uint32_t frf            :   2;  /*!< <b>Frame Format.</b> \n
                                             Selects which serial protocol transfers the data. \n
                                             <b>Values:</b>
                                                - 0x0 (MOTOROLA_SPI): Motorolla SPI Frame Format.
                                                - 0x1 (TEXAS_SSP): Texas Instruments SSP FrameFormat.
                                                - 0x2 (NS_MICROWIRE): National Microwire FrameFormat.
                                                - 0x3 (RESERVED): Reserved value. */
        uint32_t scph           :   1;  /*!< <b>Serial Clock Phase.</b>
                                             Valid when the frame format (FRF) is set to Motorola SPI.
                                             The serial clock phase selects the relationship of the serial
                                             clock with the slave select signal. \n
                                             When SCPH = 0, data are captured on the first edge of the
                                             serial clock. When SCPH = 1, the serial clock starts toggling
                                             one cycle after the slave select line is activated, and data are
                                             captured on the second edge of the serial clock. \n
                                             <b>Values:</b>
                                                - 0x0 (SCPH_MIDDLE): Serial clock toggles in middle of first data bit.
                                                - 0x1 (SCPH_START): Serial clock toggles at start of first data bit. */
        uint32_t scpol          :   1;  /*!< <b>Serial Clock Polarity.</b> \n
                                             Valid when the frame format (FRF) is set to Motorola SPI.
                                             Used to select the polarity of the inactive serial clock, which
                                             is held inactive when the DW_apb_ssi master is not actively
                                             transferring data on the serial bus. \n
                                             <b>Values:</b>
                                                - 0x0 (SCLK_LOW): Inactive state of serial clock is low
                                                - 0x1 (SCLK_HIGH): Inactive state of serial clock is high */
        uint32_t tmod           :   2;  /*!< <b>Transfer Mode.</b> \n
                                             Selects the mode of transfer for serial communication. This
                                             field does not affect the transfer duplicity. Only indicates
                                             whether the receive or transmit data are valid. \n
                                             In transmit-only mode, data received from the external
                                             device is not valid and is not stored in the receive FIFO
                                             memory; it is overwritten on the next transfer. \n
                                             In receive-only mode, transmitted data are not valid. After
                                             the first write to the transmit FIFO, the same word is
                                             retransmitted for the duration of the transfer. \n
                                             In transmit-and-receive mode, both transmit and receive
                                             data are valid. The transfer continues until the transmit FIFO
                                             is empty. Data received from the external device are stored
                                             into the receive FIFO memory, where it can be accessed by
                                             the host processor. \n
                                             In eeprom-read mode, receive data is not valid while control
                                             data is being transmitted. When all control data is sent to the
                                             EEPROM, receive data becomes valid and transmit data
                                             becomes invalid. All data in the transmit FIFO is considered
                                             control data in this mode. This transfer mode is only valid
                                             when the DW_apb_ssi is configured as master device.
                                                - 00 - Transmit & Receive
                                                - 01 - Transmit Only
                                                - 10 - Receive Only
                                                - 11 - EEPROM Read
                                             When SSI_SPI_MODE is either set to "Dual" or "Quad" or
                                             "Octal" mode and SPI_FRF is not set to 2'b00. There are
                                             only two valid combinations:
                                                - 10 - Read
                                                - 01 - Write \n
                                             <b>Values:</b>
                                                - 0x0 (TX_AND_RX): Transmit & receive
                                                - 0x1 (TX_ONLY): Transmit only mode or Write (SPI_FRF != 2'b00)
                                                - 0x2 (RX_ONLY): Receive only mode or Read (SPI_FRF != 2'b00)
                                                - 0x3 (EEPROM_READ): EEPROM Read mode */
        uint32_t slv_oe         :   1;  /*!< <b>Slave Output Enable.</b> \n
                                             Relevant only when the DW_apb_ssi is
                                             configured as a serial-slave device. When configured as a
                                             serial master, this bit field has no functionality. This bit
                                             enables or disables the setting of the ssi_oe_n output from
                                             the DW_apb_ssi serial slave. When SLV_OE = 1, the
                                             ssi_oe_n output can never be active. When the ssi_oe_n
                                             output controls the tri-state buffer on the txd output from the
                                             slave, a high impedance state is always present on the slave
                                             txd output when SLV_OE = 1. \n
                                             This is useful when the master transmits in broadcast mode
                                             (master transmits data to all slave devices). Only one slave
                                             may respond with data on the master rxd line. This bit is
                                             enabled after reset and must be disabled by software (when
                                             broadcast mode is used), if you do not want this device to
                                             respond with data. \n
                                             <b>Values:</b>
                                                - 0x1 (DISABLED): Slave Output is disabled
                                                - 0x0 (ENABLED): Slave Output is enabled \n */
        uint32_t srl            :   1;  /*!< <b>Shift Register Loop.</b> \n
                                             Used for testing purposes only. When internally active,
                                             connects the transmit shift register output to the receive shift
                                             register input. \n
                                             Can be used in both serial-slave and serial-master modes.
                                             When the DW_apb_ssi is configured as a slave in loopback
                                             mode, the ss_in_n and ssi_clk signals must be provided by
                                             an external source. In this mode, the slave cannot generate
                                             these signals because there is nothing to which to loop back \n
                                             <b>Values:</b>
                                                - 0x1 (TESTING_MODE): Test mode: Tx & Rx shift reg connected
                                                - 0x0 (NORMAL_MODE): Normal mode operation */
        uint32_t cfs            :   4;  /*!< <b>Control Frame Size.</b>  \n
                                             Selects the length of the control word for the Microwire frame format. \n
                                             <b>Values:</b>
                                                - 0x0 (SIZE_01_BIT): 1-bit Control Word
                                                - 0x1 (SIZE_02_BIT): 2-bit Control Word
                                                - 0x2 (SIZE_03_BIT): 3-bit Control Word
                                                - 0x3 (SIZE_04_BIT): 4-bit Control Word
                                                - 0x4 (SIZE_05_BIT): 5-bit Control Word
                                                - 0x5 (SIZE_06_BIT): 6-bit Control Word
                                                - 0x6 (SIZE_07_BIT): 7-bit Control Word
                                                - 0x7 (SIZE_08_BIT): 8-bit Control Word
                                                - 0x8 (SIZE_09_BIT): 9-bit Control Word
                                                - 0x9 (SIZE_10_BIT): 10-bit Control Word
                                                - 0xa (SIZE_11_BIT): 11-bit Control Word
                                                - 0xb (SIZE_12_BIT): 12-bit Control Word
                                                - 0xc (SIZE_13_BIT): 13-bit Control Word
                                                - 0xd (SIZE_14_BIT): 14-bit Control Word
                                                - 0xe (SIZE_15_BIT): 15-bit Control Word
                                                - 0xf (SIZE_16_BIT): 16-bit Control Word */
        uint32_t dfs_32         :   5;  /*!< <b>Data Frame Size.</b> \n
                                             in 32-bit transfer size mode. Used to select
                                             the data frame size in 32-bit transfer mode. These bits are
                                             only valid when SSI_MAX_XFER_SIZE is configured to 32.
                                             When the data frame size is programmed to be less than 32
                                             bits, the receive data are automatically right-justified by the
                                             receive logic, with the upper bits of the receive FIFO zeropadded.
                                             You are responsible for making sure that transmit
                                             data is right-justified before writing into the transmit FIFO.
                                             The transmit logic ignores the upper unused bits when
                                             transmitting the data. \n
                                             @note
                                             When SSI_SPI_MODE is either set to "Dual" or
                                             "Quad" or "Octal" mode and SPI_FRF is not set to 2'b00.
                                                - DFS value should be multiple of 2 if SPI_FRF = 0x01,
                                                - DFS value should be multiple of 4 if SPI_FRF = 0x10,
                                                - DFS value should be multiple of 8 if SPI_FRF = 0x11.
                                             @par
                                            <b>Values:</b>
                                                - 0x3 (FRAME_04BITS): 4-bit serial data transfer
                                                - 0x4 (FRAME_05BITS): 5-bit serial data transfer
                                                - 0x5 (FRAME_06BITS): 6-bit serial data transfer
                                                - 0x6 (FRAME_07BITS): 7-bit serial data transfer
                                                - 0x7 (FRAME_08BITS): 8-bit serial data transfer
                                                - 0x8 (FRAME_09BITS): 9-bit serial data transfer
                                                - 0x9 (FRAME_10BITS): 10-bit serial data transfer
                                                - 0xa (FRAME_11BITS): 11-bit serial data transfer
                                                - 0xb (FRAME_12BITS): 12-bit serial data transfer
                                                - 0xc (FRAME_13BITS): 13-bit serial data transfer
                                                - 0xd (FRAME_14BITS): 14-bit serial data transfer
                                                - 0xe (FRAME_15BITS): 15-bit serial data transfer
                                                - 0xf (FRAME_16BITS): 16-bit serial data transfer
                                                - 0x10 (FRAME_17BITS): 17-bit serial data transfer
                                                - 0x11 (FRAME_18BITS): 18-bit serial data transfer
                                                - 0x12 (FRAME_19BITS): 19-bit serial data transfer
                                                - 0x13 (FRAME_20BITS): 20-bit serial data transfer
                                                - 0x14 (FRAME_21BITS): 21-bit serial data transfer
                                                - 0x15 (FRAME_22BITS): 22-bit serial data transfer
                                                - 0x16 (FRAME_23BITS): 23-bit serial data transfer
                                                - 0x17 (FRAME_24BITS): 24-bit serial data transfer
                                                - 0x18 (FRAME_25BITS): 25-bit serial data transfer
                                                - 0x19 (FRAME_26BITS): 26-bit serial data transfer
                                                - 0x1a (FRAME_27BITS): 27-bit serial data transfer
                                                - 0x1b (FRAME_28BITS): 28-bit serial data transfer
                                                - 0x1c (FRAME_29BITS): 29-bit serial data transfer
                                                - 0x1d (FRAME_30BITS): 30-bit serial data transfer
                                                - 0x1e (FRAME_31BITS): 31-bit serial data transfer
                                                - 0x1f (FRAME_32BITS): 32-bit serial data transfer */
        uint32_t spi_frf        :   2;  /*!< <b>SPI Frame Format.</b> \n
                                             Selects data frame format for Transmitting/Receiving the data
                                             Bits only valid when SSI_SPI_MODE is either set to "Dual"
                                             or "Quad" or "Octal" mode. \n
                                             When SSI_SPI_MODE is configured for "Dual Mode", 10/11
                                             combination is reserved. \n
                                             When SSI_SPI_MODE is configured for "Quad Mode", 11
                                             combination is reserved. \n
                                             <b>Values:</b>
                                                - 0x0 (STD_SPI_FRF): Standard SPI Frame Format
                                                - 0x1 (DUAL_SPI_FRF): Dual SPI Frame Format
                                                - 0x2 (QUAD_SPI_FRF): Quad SPI Frame Format
                                                - 0x3 (OCTAL_SPI_FRF): Octal SPI Frame Format
                                              */
        uint32_t reserved_23    :   1;
        uint32_t sste           :   1;  /*!< <b>Slave Select Toggle Enable.</b> \n
                                             When operating in SPI mode with clock phase (SCPH) set to
                                             0, this register controls the behavior of the slave select line
                                             (ss_*_n) between data frames. If this register field is set to 1
                                             the ss_*_n line will toggle between consecutive data frames,
                                             with the serial clock (sclk) being held to its default value while
                                             ss_*_n is high; if this register field is set to 0 the ss_*_n will
                                             stay low and sclk will run continuously for the duration of the
                                             transfer. \n
                                             @note
                                             This register is only valid when SSI_SCPH0_SSTOGGLE is set to 1.
                                             @par
                                             */
        uint32_t reserved25_31  :   7;
    } b;                                /*!< Register bits. */
} ctrlr0_data_t;

/**
 * @brief  This union represents the bit fields in the Control Register 1. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union ctrlr1_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t ndf            :   16; /*!< <b>Number of Data Frames.</b> \n
                                             When TMOD = 10 or TMOD = 11 , this register field sets the
                                             number of data frames to be continuously received by the
                                             DW_apb_ssi. The DW_apb_ssi continues to receive serial
                                             data until the number of data frames received is equal to this
                                             register value plus 1, which enables you to receive up to 64
                                             KB of data in a continuous transfer. \n
                                             When the DW_apb_ssi is configured as a serial slave, the
                                             transfer continues for as long as the slave is selected.
                                             Therefore, this register serves no purpose and is not present
                                             when the DW_apb_ssi is configured as a serial slave. */
    } b;                                /*!< Register bits. */
} ctrlr1_data_t;

/**
 * @brief  This union represents the bit fields in the SSI Enable Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union ssienr_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t ssi_en         :   1;  /*!< <b>SSI Enable.</b> \n
                                             Enables and disables all DW_apb_ssi operations. When
                                             disabled, all serial transfers are halted immediately. Transmit
                                             and receive FIFO buffers are cleared when the device is
                                             disabled. It is impossible to program some of the
                                             DW_apb_ssi control registers when enabled. When disabled,
                                             the ssi_sleep output is set (after delay) to inform the system
                                             that it is safe to remove the ssi_clk, thus saving power
                                             consumption in the system. \n
                                             <b>Values:</b>
                                                - 0x0 (DISABLE): Disables Serial Transfer
                                                - 0x1 (ENABLED): Enables Serial Transfer */
    } b;                                /*!< Register bits. */
} ssienr_data_t;

/**
 * @brief  This union represents the bit fields in the Microwire Control Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union mwcr_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t mwmod          :   1;  /*!< <b>Microwire Transfer Mode.</b> \n
                                             Defines whether the Microwire transfer is sequential or nonsequential.
                                             When sequential mode is used, only one control
                                             word is needed to transmit or receive a block of data words.
                                             When non-sequential mode is used, there must be a control
                                             word for each data word that is transmitted or received. \n
                                             <b>Values:</b>
                                                - 0x0 (NON_SEQUENTIAL): Non-Sequential Microwire Transfer
                                                - 0x1 (SEQUENTIAL): Sequential Microwire Transfer */
        uint32_t mdd            :   1;  /*!< <b>Microwire Control.</b> \n
                                             Defines the direction of the data word when the Microwire
                                             serial protocol is used. When this bit is set to 0, the data
                                             word is received by the DW_apb_ssi MacroCell from the
                                             external serial device. When this bit is set to 1, the data word
                                             is transmitted from the DW_apb_ssi MacroCell to the
                                             external serial device. \n
                                             <b>Values:</b>
                                                - 0x0 (RECEIVE): SSI receives data
                                                - 0x1 (TRANSMIT): SSI transmits data */
        uint32_t mhs            :   1;  /*!< <b>Microwire Handshaking.</b> \n
                                             Relevant only when the DW_apb_ssi is configured as a
                                             serial-master device. When configured as a serial slave, this
                                             bit field has no functionality. Used to enable and disable the
                                             busy/ready handshaking interface for the Microwire protocol.
                                             When enabled, the DW_apb_ssi checks for a ready status
                                             from the target slave, after the transfer of the last data/control
                                             bit, before clearing the BUSY status in the SR register. \n
                                             <b>Values:</b>
                                                - 0x0 (DISABLE): Handshaking interface is disabled
                                                - 0x1 (ENABLED): Handshaking interface is enabled
                                             */
    } b;                                /*!< Register bits. */
} mwcr_data_t;

/**
 * @brief  This union represents the bit fields in the Slave Enable Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union ser_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t ser            :   16; /*!< <b>Slave Select Enable Flag.</b> \n
                                             Each bit in this register corresponds to a slave select line
                                             (ss_x_n) from the DW_apb_ssi master. When a bit in this
                                             register is set (1), the corresponding slave select line from
                                             the master is activated when a serial transfer begins. It
                                             should be noted that setting or clearing bits in this register
                                             have no effect on the corresponding slave select outputs until
                                             a transfer is started. Before beginning a transfer, you should
                                             enable the bit in this register that corresponds to the slave
                                             device with which the master wants to communicate. When
                                             not operating in broadcast mode, only one bit in this field
                                             should be set. \n
                                             <b>Values:</b>
                                                - 0x0 (NOT_SELECTED): No slave selected
                                                - 0x1 (SELECTED): Slave is selected */
    } b;                                /*!< Register bits. */
} ser_data_t;

/**
 * @brief  This union represents the bit fields in the Slave Enable Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union baudr_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t sckdv          :   16; /*!< <b>SSI Clock Divider.</b> \n
                                             The LSB for this field is always set to 0 and is unaffected by a
                                             write operation, which ensures an even value is held in this
                                             register. If the value is 0, the serial output clock (sclk_out) is
                                             disabled. The frequency of the sclk_out is derived from the
                                             following equation:. \n
                                             Fsclk_out = Fssi_clk/SCKDV \n
                                             where SCKDV is any even value between 2 and 65534. For example: \n
                                             for Fssi_clk = 3.6864MHz and SCKDV =2, Fsclk_out = 3.6864/2 = 1.8432MHz. */
    } b;                                /*!< Register bits. */
} baudr_data_t;

/**
 * @brief  This union represents the bit fields in the Transmit FIFO Threshold Level. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union txftlr_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t tft;                   /*!< <b>Transmit FIFO Threshold.</b> \n
                                             Controls the level of entries (or below) at which the transmit
                                             FIFO controller triggers an interrupt. The FIFO depth is
                                             configurable in the range 2-256; this register is sized to the
                                             number of address bits needed to access the FIFO. If you
                                             attempt to set this value greater than or equal to the depth of
                                             the FIFO, this field is not written and retains its current value.
                                             When the number of transmit FIFO entries is less than or
                                             equal to this value, the transmit FIFO empty interrupt is
                                             triggered. For information on the Transmit FIFO Threshold
                                             values, see the "Master SPI and SSP Serial Transfers" in the
                                             DW_apb_ssi Databook. \n
                                             ssi_txe_intr is asserted when TFT or less data entries are
                                             present in transmit FIFO. */
    } b;                                /*!< Register bits. */
} txftlr_data_t;

/**
 * @brief  This union represents the bit fields in the Receive FIFO Threshold Level. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union rxftlr_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t rft;                   /*!< <b>Receive FIFO Threshold.</b> \n
                                             Controls the level of entries (or above) at which the receive
                                             FIFO controller triggers an interrupt. The FIFO depth is
                                             configurable in the range 2-256. This register is sized to the
                                             number of address bits needed to access the FIFO. If you
                                             attempt to set this value greater than the depth of the FIFO,
                                             this field is not written and retains its current value. When the
                                             number of receive FIFO entries is greater than or equal to
                                             this value + 1, the receive FIFO full interrupt is triggered. For
                                             information on the Receive FIFO Threshold values, see the
                                             "Master SPI and SSP Serial Transfers" in the DW_apb_ssi
                                             Databook. \n
                                             ssi_rxf_intr is asserted when RFT or more data entries are
                                             present in receive FIFO. */
    } b;                                /*!< Register bits. */
} rxftlr_data_t;

/**
 * @brief  This union represents the bit fields in the Transmit FIFO Level Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union txflr_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t txtfl;                 /*!< <b>Transmit FIFO Level.</b> \n
                                             Contains the number of valid data entries in the transmit FIFO. */
    } b;                                /*!< Register bits. */
} txflr_data_t;

/**
 * @brief  This union represents the bit fields in the Receive FIFO Level Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union rxflr_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t rxtfl;                 /*!< <b>Receive FIFO Level.</b> \n
                                             Contains the number of valid data entries in the receive FIFO. */
    } b;                                /*!< Register bits. */
} rxflr_data_t;

/**
 * @brief  This union represents the bit fields in the Receive FIFO Level Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union sr_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t busy           :   1;  /*!< <b>SSI Busy Flag.</b> \n
                                             When set, indicates that a serial transfer is in progress; when
                                             cleared indicates that the DW_apb_ssi is idle or disabled. \n
                                             <b>Values:</b>
                                                - 0x0 (INACTIVE): DW_apb_ssi is idle or disabled
                                                - 0x1 (ACTIVE): DW_apb_ssi is actively transferring data */
        uint32_t tfnf           :   1;  /*!< <b>Transmit FIFO Not Full.</b> \n
                                             Set when the transmit FIFO contains one or more empty
                                             locations, and is cleared when the FIFO is full. \n
                                             <b>Values:</b>
                                                - 0x0 (FULL): Transmit FIFO is full
                                                - 0x1 (NOT_FULL): Transmit FIFO is not Full */
        uint32_t tfe            :   1;  /*!< <b>Transmit FIFO Empty.</b> \n
                                             When the transmit FIFO is completely empty, this bit is set. \n
                                             When the transmit FIFO contains one or more valid entries,
                                             this bit is cleared. This bit field does not request an interrupt. \n
                                             <b>Values:</b>
                                                - 0x0 (NOT_EMPTY): Transmit FIFO is not empty
                                                - 0x1 (EMPTY): Transmit FIFO is empty */
        uint32_t rfne           :   1;  /*!< <b>Receive FIFO Not Empty.</b> \n
                                             Set when the receive FIFO contains one or more entries and
                                             is cleared when the receive FIFO is empty. This bit can be
                                             polled by software to completely empty the receive FIFO. \n
                                             <b>Values:</b>
                                                 - 0x0 (EMPTY): Receive FIFO is empty
                                                 - 0x1 (NOT_EMPTY): Receive FIFO is not empty */
        uint32_t rff            :   1;  /*!< <b>Receive FIFO Full.</b> \n
                                             When the receive FIFO is completely full, this bit is set.
                                             When the receive FIFO contains one or more empty location,
                                             this bit is cleared. \n
                                             <b>Values:</b>
                                                - 0x0 (NOT_FULL): Receive FIFO is not full
                                                - 0x1 (FULL): Receive FIFO is full */
        uint32_t txe            :   1;  /*!< <b>Transmission Error.</b> \n
                                             Set if the transmit FIFO is empty when a transfer is started.
                                             This bit can be set only when the DW_apb_ssi is configured
                                             as a slave device. Data from the previous transmission is
                                             resent on the txd line. This bit is cleared when read. \n
                                             <b>Values:</b>
                                                - 0x0 (NO_ERROR): No Error
                                                - 0x1 (TX_ERROR): Transmission Error */
        uint32_t dcol           :   1;  /*!< <b>Data Collision Error.</b> \n
                                             Relevant only when the DW_apb_ssi is configured as a
                                             master device. This bit will be set if ss_in_n input is asserted
                                             by other master, when the DW_apb_ssi master is in the
                                             middle of the transfer. This informs the processor that the
                                             last data transfer was halted before completion. This bit is
                                             cleared when read. \n
                                             <b>Values:</b>
                                                - 0x0 (NO_ERROR_CONDITION): No Error
                                                - 0x1 (TX_COLLISION_ERROR): Transmit Data Collision Error */
        uint32_t reserved7_31;
    } b;                                /*!< Register bits. */
} sr_data_t;

/**
 * @brief  This union represents the bit fields in the Interrupt Registers. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
union interrupt_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t txei           :   1;  /*!< Transmit FIFO Empty Interrupt Mask. \n
                                             <b>Values:</b>
                                                - 0x0 (SET): ssi_txe_intr interrupt is set
                                                - 0x1 (UNSET): ssi_txe_intr interrupt is not set */
        uint32_t txoi           :   1;  /*!< Transmit FIFO Overflow Interrupt Mask. \n
                                             <b>Values:</b>
                                                - 0x0 (SET): ssi_txo_intr interrupt is set
                                                - 0x1 (UNSET): ssi_txo_intr interrupt is not set */
        uint32_t rxui           :   1;  /*!< Receive FIFO Underflow Interrupt Mask. \n
                                             <b>Values:</b>
                                                - 0x0 (SET): ssi_rxu_intr interrupt is set
                                                - 0x1 (UNSET): ssi_rxu_intr interrupt is not set */
        uint32_t rxoi           :   1;  /*!< Receive FIFO Overflow Interrupt Mask. \n
                                             <b>Values:</b>
                                                - 0x0 (SET): ssi_rxo_intr interrupt is set
                                                - 0x1 (UNSET): ssi_rxo_intr interrupt is not set */
        uint32_t rxfi           :   1;  /*!< Receive FIFO Full Interrupt Mask. \n
                                             <b>Values:</b>
                                                - 0x0 (SET): ssi_rxf_intr interrupt is set
                                                - 0x1 (UNSET): ssi_rxf_intr interrupt is not set */
        uint32_t msti           :   1;  /*!< Multi-Master Contention Interrupt Mask. This bit field is not
                                             present if the DW_apb_ssi is configured as a serial-slave
                                             device. \n
                                             <b>Values:</b>
                                                - 0x0 (SET): ssi_mst_intr interrupt is set
                                                - 0x1 (UNSET): ssi_mst_intr interrupt is not set */
        uint32_t reserved6_31   :   26;
    } b;                                /*!< Register bits. */
};

/**
 * @brief  This union represents the bit fields in the Interrupt Mask Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union interrupt_data imr_data_t;

/**
 * @brief  This union represents the bit fields in the Interrupt Status Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union interrupt_data isr_data_t;

/**
 * @brief  This union represents the bit fields in the Raw Interrupt Status Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union interrupt_data risr_data_t;

/**
 * @brief  This union represents the bit fields in the Interrupt Clear Register Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union icr_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t icr            :   1;  /*!< <b>Clear Interrupt.</b> \n
                                             This register reflects the status of the interrupt. <b>A read from
                                             this register clears the ssi_txo_intr, ssi_rxu_intr, ssi_rxo_intr, and
                                             the ssi_mst_intr interrupt;</b> writing has no
                                             effect. */
        uint32_t reserved1_31   :   31;
    } b;                                /*!< Register bits. */
} icr_data_t;

/**
 * @brief  This union represents the bit fields in the Transmit FIFO Overflow Interrupt Clear Register Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union icr_data txoicr_data_t;

/**
 * @brief  This union represents the bit fields in the Receive FIFO Overflow Interrupt Clear Register Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union icr_data rxoicr_data_t;

/**
 * @brief  This union represents the bit fields in the Receive FIFO Underflow Interrupt Clear Register Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union icr_data rxuicr_data_t;

/**
 * @brief  This union represents the bit fields in the Multi-Master Interrupt Clear Register Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union icr_data msticr_data_t;

/**
 * @brief  This union represents the bit fields in the DMA Control Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union dmacr_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t rdmae          :   1;  /*!< <b>Receive DMA Enable.</b> \n
                                             This bit enables/disables the receive FIFO DMA channel. \n
                                             <b>Values:</b>
                                                - 0x0 (DISABLE): Transmit DMA disabled
                                                - 0x1 (ENABLED): Transmit DMA enabled */
        uint32_t tdmae          :   1;  /*!< <b>Transmit DMA Enable.</b> \n
                                             This bit enables/disables the transmit FIFO DMA channel. \n
                                             <b>Values:</b>
                                                - 0x0 (DISABLE): Transmit DMA disabled
                                                - 0x1 (ENABLED): Transmit DMA enabled */
        uint32_t reserved1_31   :   30;
    } b;                                /*!< Register bits. */
} dmacr_data_t;

/**
 * @brief  This union represents the bit fields in the DMA Data Level Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
union dmadlr_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t dmadl;                 /*!< <b>Data Level.</b> \n
                                             These bits field controls the level at which a DMA request is
                                             made by the transmit/receive logic. It is equal to the watermark level */
    } b;                                /*!< Register bits. */
};

/**
 * @brief  This union represents the bit fields in the DMA Transmit Data Level Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union dmadlr_data dmatdlr_data_t;

/**
 * @brief  This union represents the bit fields in the DMA Receive Data Level Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union dmadlr_data dmardlr_data_t;

/**
 * @brief  This union represents the bit fields in the Identification Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union idr_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t idcode;                /*!< <b>Identification code.</b> \n
                                             The register contains the peripheral's identification code,
                                             which is written into the register at configuration time using
                                             CoreConsultant. */
    } b;                                /*!< Register bits. */
} idr_data_t;

/**
 * @brief  This union represents the bit fields in the  coreKit version ID Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union ssi_version_id_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t ssi_comp_version;      /*!< Contains the hex representation of the Synopsys component
                                             version. Consists of ASCII value for each number in the
                                             version, followed by *. For example 32_30_31_2A represents
                                             the version 2.01*. */
    } b;                                /*!< Register bits. */
} ssi_version_id_data_t;

/**
 * @brief  This union represents the bit fields in the Data Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union dr_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t dr;                    /*!< Data Register. When writing to this register, you must rightjustify
                                             the data. Read data are automatically right-justified. If
                                             SSI_MAX_XFER_SIZE configuration parameter is set to 32,
                                             all 32 bits are valid. Otherwise, only 16 bits ([15:0]) of the
                                             register are valid. Read = Receive FIFO buffer Write =
                                             Transmit FIFO buffer. */
    } b;                                /*!< Register bits. */
} dr_data_t;

/**
 * @brief  This union represents the bit fields in the RX Sample Delay Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union rx_sample_dly_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t rsd            :   8;  /*!< <b>Rxd Sample Delay.</b> \n
                                             This register is used to delay the sample of the rxd input port.
                                             Each value represents a single ssi_clk delay on the sample of rxd. */
        uint32_t reserved8_31   :   24;
    } b;                                /*!< Register bits. */
} rx_sample_dly_data_t;

/**
 * @brief  This union represents the bit fields in the SPI Control 0 Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union spi_ctrlr0_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t trans_type     :   2;  /*!< <b>Address and instruction transfer format.</b> \n
                                             Selects whether DW_apb_ssi will transmit
                                             instruction/address either in Standard SPI mode or the SPI
                                             mode selected in CTRLR0.SPI_FRF field. 00 - Instruction
                                             and Address will be sent in Standard SPI Mode.
                                             01 - Instruction will be sent in Standard SPI Mode and
                                             Address will be sent in the mode specified by
                                             CTRLR0.SPI_FRF. \n
                                             10 - Both Instruction and Address will be sent in the mode
                                             specified by SPI_FRF. 11 - Reserved. */
        uint32_t addr_l         :   4;  /*!< <b>Address Length.</b> \n
                                             This bit defines Length of Address to be transmitted. Only
                                             after this much bits are programmed in to the FIFO the
                                             transfer can begin. For information on the ADDR_Ldecode
                                             value, see "Read Operation in Enhanced SPI Modes"
                                             section in the DW_apb_ssi Databook. \n
                                             <b>Values:</b>
                                                - 0x0 (ADDR_L_0): 0-bit Address Width
                                                - 0x1 (ADDR_L_1): 4-bit Address Width
                                                - 0x2 (ADDR_L_2): 8-bit Address Width
                                                - 0x3 (ADDR_L_3): 12-bit Address Width
                                                - 0x4 (ADDR_L_4): 16-bit Address Width
                                                - 0x5 (ADDR_L_5): 20-bit Address Width
                                                - 0x6 (ADDR_L_6): 24-bit Address Width
                                                - 0x7 (ADDR_L_7): 28-bit Address Width
                                                - 0x8 (ADDR_L_8): 32-bit Address Width
                                                - 0x9 (ADDR_L_9): 36-bit Address Width
                                                - 0xa (ADDR_L_10): 40-bit Address Width
                                                - 0xb (ADDR_L_11): 44-bit Address Width
                                                - 0xc (ADDR_L_12): 48-bit Address Width
                                                - 0xd (ADDR_L_13): 52-bit Address Width
                                                - 0xe (ADDR_L_14): 56-bit Address Width
                                                - 0xf (ADDR_L_15): 60-bit Address Width */
        uint32_t reserved6_7    :   2;
        uint32_t inst_l         :   2;  /*!< <b>Instruction Length.</b> \n
                                             Dual/Quad/Octal mode instruction length in bits. \n
                                             <b>Values:</b>
                                                - 0x0 (INST_L_0): 0-bit (No Instruction)
                                                - 0x1 (INST_L_1): 4-bit Instruction
                                                - 0x2 (INST_L_2): 8-bit Instruction
                                                - 0x3 (INST_L_3): 16-bit Instruction */
        uint32_t reserved8      :   1;
        uint32_t wait_cycles    :   5;  /*!< <b>Wait cycles.</b> \n
                                             Number of wait cycles in Dual/Quad/Octal mode between
                                             control frames transmit and data reception. This value is
                                             specified as number of SPI clock cycles. For information on
                                             the WAIT_CYCLES decode value, see "Read Operation in
                                             Enhanced SPI Modes" section in the DW_apb_ssi Databook. */
        uint32_t spi_ddr_en     :   1;  /*!< <b>SPI DDR Enable bit.</b> \n
                                             This will enable Dual-data rate transfers in Dual/Quad/Octal
                                             frame formats of SPI. */
        uint32_t inst_ddr_en    :   1;  /*!< <b>Instruction DDR Enable bit.</b> \n
                                             This will enable Dual-data rate transfer for Instruction phase. */
        uint32_t spi_rxds_en    :   1;  /*!< <b>Read data strobe enable bit.</b> \n
                                             Once this bit is set to 1 DW_apb_ssi will use Read data
                                             strobe (rxds) to capture read data in DDR mode. */
        uint32_t reserved19_31  :   13;
    } b;                                /*!< Register bits. */
} spi_ctrlr0_data_t;

/**
 * @brief  This union represents the bit fields in the Transmit Drive Edge Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union txd_drive_edge_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t tde            :   8;  /*!< TXD Drive edge - value of which decides the driving edge of
                                             tramit data. The maximum value of this regster is = (BAUDR/2) -1. */
        uint32_t reserved8_31   :   24;
    } b;                                /*!< Register bits. */
} txd_drive_edge_data_t;

/**
 * @brief  Registers associated with SSI.
 */
typedef struct ssi_regs {
    volatile uint32_t ctrlr0;           /*!< This register controls the serial data transfer. It is impossible
                                             to write to this register when... <i>Offset: 00h</i>. */
    volatile uint32_t ctrlr1;           /*!< This register exists only when the DW_apb_ssi is configured
                                             as a master device. When the DW_apb_ssi... <i>Offset: 04h</i>. */
    volatile uint32_t ssienr;           /*!< This register enables and disables the DW_apb_ssi. Reset
                                             Value: 0x0 <i>Offset: 08h</i>. */
    volatile uint32_t mwcr;             /*!< This register controls the direction of the data word for the
                                             half-duplex Microwire serial protocol... <i>Offset: 0Ch</i>. */
    volatile uint32_t ser;              /*!< This register is valid only when the DW_apb_ssi is
                                             configured as a master device. When the DW_apb_ssi...
                                             <i>Offset: 10h</i>. */
    volatile uint32_t baudr;            /*!< This register is valid only when the DW_apb_ssi is
                                             configured as a master device. When the DW_apb_ssi...
                                             <i>Offset: 14h</i>. */
    volatile uint32_t txftlr;           /*!< This register controls the threshold value for the transmit
                                             FIFO memory. The DW_apb_ssi is enabled... <i>Offset: 18h</i>. */
    volatile uint32_t rxftlr;           /*!< This register controls the threshold value for the receive
                                             FIFO memory. The DW_apb_ssi is enabled... <i>Offset: 1Ch</i>. */
    volatile uint32_t txflr;            /*!< This register contains the number of valid data entries in the
                                             transmit FIFO memory. Reset Value:.. <i>Offset: 20h</i>. */
    volatile uint32_t rxflr;            /*!< This register contains the number of valid data entries in the
                                             receive FIFO memory. This register... <i>Offset: 24h</i>. */
    volatile uint32_t sr;               /*!< This is a read-only register used to indicate the current
                                             transfer status, FIFO status, and any... <i>Offset: 28h</i>. */
    volatile uint32_t imr;              /*!< This read/write reigster masks or enables all interrupts
                                             generated by the DW_apb_ssi. When the DW_apb_ssi... <i>Offset: 2Ch</i>. */
    volatile uint32_t isr;              /*!< This register reports the status of the DW_apb_ssi interrupts
                                             after they have been masked. Reset... <i>Offset: 30h</i>. */
    volatile uint32_t risr;             /*!< This read-only register reports the status of the DW_apb_ssi
                                             interrupts prior to masking. Reset... <i>Offset: 34h</i>. */
    volatile uint32_t txoicr;           /*!< Transmit FIFO Overflow Interrupt Clear Register. Reset
                                             Value: 0x0 <i>Offset: 38h</i>. */
    volatile uint32_t rxoicr;           /*!< Receive FIFO Overflow Interrupt Clear Register. Reset
                                             Value: 0x0 <i>Offset: 3Ch</i>. */
    volatile uint32_t rxuicr;           /*!< Receive FIFO Underflow Interrupt Clear Register. Reset
                                             Value: 0x0 <i>Offset: 40h</i>. */
    volatile uint32_t msticr;           /*!< Multi-Master Interrupt Clear Register. Reset Value: 0x0
                                             <i>Offset: 44h</i>. */
    volatile uint32_t icr;              /*!< Interrupt Clear Register. Reset Value: 0x0 <i>Offset: 48h</i>. */
    volatile uint32_t dmacr;            /*!< This register is only valid when DW_apb_ssi is configured
                                             with a set of DMA Controller interface... <i>Offset: 4Ch</i>. */
    volatile uint32_t dmatdlr;          /*!< This register is only valid when the DW_apb_ssi is
                                             configured with a set of DMA interface signals... <i>Offset: 50h</i>. */
    volatile uint32_t dmardlr;          /*!< This register is only valid when DW_apb_ssi is configured
                                             with a set of DMA interface signals (SSI_HAS_DMA... <i>Offset: 54h</i>. */
    volatile uint32_t idr;              /*!< This register contains the peripherals identification code,
                                             which is written into the register at... <i>Offset: 58h</i>. */
    volatile uint32_t ssi_version_id;   /*!< This read-only register stores the specific DW_apb_ssi
                                             component version. Reset Value:... <i>Offset: 5Ch</i>. */
    volatile uint32_t dr[HAL_SPI_DR_REG_SIZE]; /*!< The DW_apb_ssi data register is a 16/32-bit (depending on
                                                    SSI_MAX_XFER_SIZE) read/write buffer for the...
                                                    <i>Offset: 60h</i>. */
    volatile uint32_t rx_sample_dly;    /*!< This register is only valid when the DW_apb_ssi is
                                             configured with rxd sample delay logic
                                             (SSI_HAS_RX_SAMPLE_DELAY==1).... <i>Offset: F0h</i>. */
    volatile uint32_t spi_ctrlr0;       /*!< This register is valid only when SSI_SPI_MODE is either set
                                             to "Dual" or "Quad" or "Octal" mode. This... <i>Offset: F4h</i>. */
    volatile uint32_t txd_drive_edge;   /*!< This Register is valid only when SSI_HAS_DDR is equal to 1.
                                             This register is used to control the... <i>Offset: F8h</i>. */
    volatile uint32_t rsvd;             /*!< Reserved. <i>Offset: FCh</i>. */
} ssi_regs_t;

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif