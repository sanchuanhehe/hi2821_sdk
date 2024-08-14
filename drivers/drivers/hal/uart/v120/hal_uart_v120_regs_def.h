/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides V120 uart register \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-06-05, Create file. \n
 */
#ifndef HAL_UART_PL001_REGS_DEF_H
#define HAL_UART_PL001_REGS_DEF_H

#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_uart_v120_regs_def UART V120 Regs Definition
 * @ingroup  drivers_hal_uart
 * @{
 */

/**
 * @brief  This union represents the bit fields in the data
 *         Register.  Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union uartdr_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t data           : 8;    /*!< Receive (read)/Transmit (write) data character. */
        uint32_t frame_error    : 1;    /*!< When this bit is set to 1, it indicates that the received character
                                             did not have a valid stop bit (a valid stop bit is 1). \n
                                             In FIFO mode, this error is associated with the character
                                             at the top of the FIFO. */
        uint32_t parity_error   : 1;    /*!< When this bit is set to 1, it indicates that the parity of the
                                             received data character does not match the parity selected
                                             as defined by bits 2 and 7 of the UARTLCR_H register. \n
                                             In FIFO mode, this error is associated with the character
                                             at the top of the FIFO. */
        uint32_t break_error    : 1;    /*!< This bit is set to 1 if a break condition was detected, indicating that
                                             the received data input was held LOW for longer than a full-word
                                             transmission time (defined as start, data, parity and stop bits). \n
                                             In FIFO mode, this error is associated with the character
                                             at the top of the FIFO. When a break occurs, only one 0 character
                                             is loaded into the FIFO. The next character is only enabled after
                                             the receive data input goes to a 1 (marking state),
                                             and the next valid start bit is received. */
        uint32_t overrun_error  : 1;    /*!< This bit is set to 1 if data is received and the receive FIFO is
                                             already full. \n
                                             This is cleared to 0 once there is an empty space in the FIFO and a new
                                             character can be written to it. */
        uint32_t reserved12_15  : 4;
    } b;                                /*!< Register bits. */
} uartdr_data_t;

/**
 * @brief  This union represents the bit fields in the receive status register/error clear register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union uartrsr_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t frame_error    : 1;    /*!< When this bit is set to 1, it indicates that the received character
                                             did not have a valid stop bit (a valid stop bit is 1). \n
                                             This bit is cleared to 0 by a write to UARTECR. \n
                                             In FIFO mode, this error is associated with the character
                                             at the top of the FIFO. */
        uint32_t parity_error   : 1;    /*!< When this bit is set to 1, it indicates that the parity of the
                                             received data character does not match the parity selected as
                                             defined by bits 2 and 7 of the UARTLCR_H register. \n
                                             This bit is cleared to 0 by a write to UARTECR. \n
                                             In FIFO mode, this error is associated with the character
                                             at the top of the FIFO. */
        uint32_t break_error    : 1;    /*!< This bit is set to 1 if a break condition was detected, indicating that
                                             the received data input was held LOW for longer than a full-word
                                             transmission time (defined as start, data, parity, and stop bits). \n
                                             This bit is cleared to 0 after a write to UARTECR. \n
                                             In FIFO mode, this error is associated with the character
                                             at the top of the FIFO. \n
                                             When a break occurs, only one 0 character is loaded into the FIFO. The next
                                             character is only enabled after the receive data input goes to a 1
                                             (marking state) and the next valid start bit is received. */
        uint32_t overrun_error  : 1;    /*!< This bit is set to 1 if data is received and the FIFO is already full. \n
                                             This bit is cleared to 0 by a write to UARTECR. \n
                                             The FIFO contents remain valid since no further data is written
                                             when the FIFO is full, only the contents of the shift register are
                                             overwritten. The CPU must now read the data in order to empty the FIFO. */
    } rb;                               /*!< Read Register bits. */
    struct {
        uint32_t wdata          : 8;    /*!< A write to this register clears the framing, parity, break,
                                             and overrun errors. The data value is not important. */
    } wb;                               /*!< Write Register bits. */
} uartrsr_data_t;

/**
 * @brief  This union represents the bit fields in the flag register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union uartfr_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t cts            : 1;    /*!< This bit is the complement of the PrimeCell UART clear to
                                             send (<b>nUARTCTS</b>) modem status input. That is, the bit is 1
                                             when the modem status input is 0. */
        uint32_t dsr            : 1;    /*!< This bit is the complement of the PrimeCell UART data set ready
                                             (<b>nUARTDSR</b>) modem status input. That is, the bit is 1
                                             when the modem status input is 0. */
        uint32_t dcd            : 1;    /*!< This bit is the complement of the PrimeCell UART data carrier detect
                                             (<b>nUARTDCD</b>) modem status input. That is, the bit is 1
                                             when the modem status input is 0. */
        uint32_t busy           : 1;    /*!< If this bit is set to 1, the PrimeCell UART is busy transmitting data.
                                             This bit remains set until the complete byte, including all the stop bits,
                                             has been sent from the shift register. \n
                                             This bit is set as soon as the transmit FIFO becomes non-empty
                                             (regardless of whether the PrimeCell UART is enabled or not) */
        uint32_t rxfe           : 1;    /*!< The meaning of this bit depends on the state of the FEN bit in the
                                             UARTLCR_H register. \n
                                             If the FIFO is disabled, this bit is set when the receive
                                             holding register is empty. \n
                                             If the FIFO is enabled, the RXFE bit is set when the
                                             receive FIFO is empty. */
        uint32_t txff           : 1;    /*!< The meaning of this bit depends on the state of the FEN bit in the
                                             UARTLCR_H register. \n
                                             If the FIFO is disabled, this bit is set when the transmit
                                             holding register is full. \n
                                             If the FIFO is enabled, the TXFF bit is set when the
                                             transmit FIFO is full. */
        uint32_t rxff           : 1;    /*!< The meaning of this bit depends on the state of the FEN bit in the
                                             UARTLCR_H register. \n
                                             If the FIFO is disabled, this bit is set when the receive
                                             holding register is full. \n
                                             If the FIFO is enabled, the RXFF bit is set when the
                                             receive FIFO is full. */
        uint32_t txfe           : 1;    /*!< The meaning of this bit depends on the state of the FEN bit in the
                                             UARTLCR_H register. \n
                                             If the FIFO is disabled, this bit is set when the transmit
                                             holding register is empty. \n
                                             If the FIFO is enabled, the TXFE bit is set when the
                                             transmit FIFO is empty. */
        uint32_t ri             : 1;    /*!< This bit is the complement of the PrimeCell UART ring
                                             indicator (<b>nUARTRI</b>) modem status input.
                                             That is, the bit is 1 when the modem status input is 0. */
        uint32_t reserved9_15   : 7;
    } b;                                /*!< Register bits. */
} uartfr_data_t;

/**
 * @brief  This union represents the bit fields in the IrDA low-power counter register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union ilpr_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t ilpdvsr        : 8;    /*!< 8-bit low-power divisor value. \n These bits are cleared to 0 at reset. */
    } b;                                /*!< Register bits. */
} ilpr_data_t;

/**
 * @brief  This union represents the bit fields in the integer part of the baud rate divisor value register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union uartibrd_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t baud_divint    : 16;   /*!< The integer baud rate divisor. \n These bits are cleared to 0 on reset. */
    } b;                                /*!< Register bits. */
} uartibrd_data_t;

/**
 * @brief  This union represents the bit fields in the fractional part of the baud rate divisor value register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union uartfbrd_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t baud_divfrac   : 6;    /*!< The fractional baud rate divisor. \n
                                             These bits are cleared to 0 on reset. */
    } b;                                /*!< Register bits. */
} uartfbrd_data_t;

/**
 * @brief  This union represents the bit fields in the line control register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union uartlcr_h_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t brk            : 1;    /*!< If this bit is set to 1, a low-level is continually output on the UARTTXD
                                             output, after completing transmission of the current character. For the
                                             proper execution of the break command, the software must set this bit for
                                             at least two complete frames. \n
                                             For normal use, this bit must be cleared to 0. */
        uint32_t parity_en      : 1;    /*!< If this bit is set to 1, parity checking and generation is enabled,
                                             else parity is disabled and no parity bit added to the data frame.  */
        uint32_t eps            : 1;    /*!< If this bit is set to 1, even parity generation and checking is performed
                                             during transmission and reception, which checks for an even number of
                                             1s in data and parity bits. When cleared to 0 then odd parity is performed
                                             which checks for an odd number of 1s. This bit has no effect when parity
                                             is disabled by <b>Parity Enable</b> (bit 1) being cleared to 0. */
        uint32_t stp2           : 1;    /*!< If this bit is set to 1, two stop bits are transmitted at the
                                             end of the frame.
                                             The receive logic does not check for two stop bits being received. */
        uint32_t en_fifos       : 1;    /*!< If this bit is set to 1, transmit and receive FIFO buffers are enabled
                                             (FIFO mode). When cleared to 0 the FIFOs are disabled (character mode) that
                                             is, the FIFOs become 1-byte-deep holding registers. */
        uint32_t wlen           : 2;    /*!< The select bits indicate the number of data bits transmitted or received
                                             in a frame as follows: \n
                                             11 = 8 bits \n
                                             10 = 7 bits \n
                                             01 = 6 bits \n
                                             00 = 5 bits. */
        uint32_t sps            : 1;    /*!< When bits 1, 2, and 7 of the UARTLCR_H register are set, the parity bit
                                             is transmitted and checked as a 0. When bits 1 and 7 are set, and bit 2 is
                                             0, the parity bit is transmitted and checked as a 1.
                                             When this bit is cleared stick parity is disabled. */
        uint32_t reserved8_15   : 8;
    } b;                                /*!< Register bits. */
} uartlcr_h_data_t;

/**
 * @brief  This union represents the bit fields in the control register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union uartcr_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t uarten         : 1;    /*!< If this bit is set to 1, the ARM PrimeCell UART is enabled. Data
                                             transmission and reception occurs for either PrimeCell UART signals
                                             or SIR signals according to the setting of SIR Enable (bit 1). When the
                                             PrimeCell UART is disabled in the middle of transmission or
                                             reception, it completes the current character before stopping. */
        uint32_t siren          : 1;    /*!< If this bit is set to 1, the IrDA SIR ENDEC is enabled. This bit has no
                                             effect if the UART is not enabled by bit 0 being set to 1.
                                             When the IrDA SIR ENDEC is enabled, data is transmitted and
                                             received on <b>nSIROUT</b> and <b>SIRIN</b>. <b>UARTTXD</b> remains in the
                                             marking state (set to 1). Signal transitions on <b>UARTRXD</b> or modem
                                             status inputs have no effect.
                                             When the IrDA SIR ENDEC is disabled, <b>nSIROUT</b> remains cleared
                                             to 0 (no light pulse generated), and signal transitions on
                                             <b>SIRIN</b> have no effect. */
        uint32_t sirlp          : 1;    /*!< This bit selects the IrDA encoding mode. If this bit is cleared to 0,
                                             low-level bits are transmitted as an active high pulse with a width of
                                             3/16th of the bit period. If this bit is set to 1, low-level bits are
                                             transmitted with a pulse width which is 3 times the period of the
                                             <b>IrLPBaud16</b> input signal, regardless of the selected bit rate.
                                             Setting this bit uses less power, but might reduce transmission
                                             distances. */
        uint32_t reserved3_6    : 4;
        uint32_t lbe            : 1;    /*!< If this bit is set to 1 and the SIR Enable bit is set to 1 and the test
                                             register UARTTCR bit 2 (SIRTEST) is set to 1, then the <b>nSIROUT</b>
                                             path is inverted, and fed through to the <b>SIRIN</b> path. The SIRTEST bit
                                             in the test register must be set to 1 to override the normal half-duplex
                                             SIR operation. This must be the requirement for accessing the test
                                             registers during normal operation, and SIRTEST must be cleared to 0
                                             when loopback testing is finished.This feature reduces the amount of
                                             external coupling required during system test.
                                             If this bit is set to 1, and the SIRTEST bit is set to 0, the UARTTXD
                                             path is fed through to the UARTRXD path.
                                             In either SIR mode or normal mode, when this bit is set, the modem
                                             outputs are also fed through to the modem inputs.
                                             This bit is cleared to 0 on reset, which disables the loopback mode. */
        uint32_t txe            : 1;    /*!< If this bit is set to 1, the transmit section of the PrimeCell UART is
                                             enabled. Data transmission occurs for either PrimeCell UART signals,
                                             or SIR signals according to the setting of SIR Enable (bit 1). When the
                                             PrimeCell UART is disabled in the middle of transmission, it
                                             completes the current character before stopping. */
        uint32_t rxe            : 1;    /*!< If this bit is set to 1, the receive section of the PrimeCell UART is
                                             enabled. Data reception occurs for either PrimeCell UART signals or
                                             SIR signals according to the setting of SIR Enable (bit 1). When the
                                             PrimeCell UART is disabled in the middle of reception, it completes
                                             the current character before stopping. */
        uint32_t dtr            : 1;    /*!< This bit is the complement of the PrimeCell UART data transmit ready
                                             (nUARTDTR) modem status output. That is, when the bit is
                                             programmed to a 1, the output is 0. */
        uint32_t rst            : 1;    /*!< This bit is the complement of the PrimeCell UART request to send
                                             (nUARTRTS) modem status output. That is, when the bit is
                                             programmed to a 1, the output is 0. */
        uint32_t out1           : 1;    /*!< This bit is the complement of the PrimeCell UART Out1
                                             (nUARTOut1) modem status output. That is, when the bit is
                                             programmed to a 1 the output is 0. For DTE this can be used as Data
                                             <i>Carrier Detect</i> (DCD). */
        uint32_t out2           : 1;    /*!< This bit is the complement of the PrimeCell UART Out2
                                             (nUARTOut2) modem status output. That is, when the bit is
                                             programmed to a 1, the output is 0. For DTE this can be used as <i>Ring
                                             Indicator</i> (RI). */
        uint32_t rts_en         : 1;    /*!< If this bit is set to 1, RTS hardware flow control is enabled. Data is only
                                             requested when there is space in the receive FIFO for it to be received. */
        uint32_t cts_en         : 1;    /*!< If this bit is set to 1, CTS hardware flow control is enabled. Data is only
                                             transmitted when the <b>nUARTCTS</b> signal is asserted. */
    } b;                                /*!< Register bits. */
} uartcr_data_t;

/**
 * @brief  This union represents the bit fields in the interrupt FIFO level select register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union uartifls_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t txiflsel       : 3;    /*!< The trigger points for the transmit interrupt are as follows: \n
                                             000 = Transmit FIFO becomes <= 1/8 full \n
                                             001 = Transmit FIFO becomes <= 1/4 full \n
                                             010 = Transmit FIFO becomes <= 1/2 full \n
                                             011 = Transmit FIFO becomes <= 3/4 full \n
                                             100 = Transmit FIFO becomes <= 7/8 full \n
                                             101:111 = reserved. */
        uint32_t rxiflsel       : 3;    /*!< The trigger points for the receive interrupt are as follows: \n
                                             000 = Receive FIFO becomes >= 1/8 full \n
                                             001 = Receive FIFO becomes >= 1/4 full \n
                                             010 = Receive FIFO becomes >= 1/2 full \n
                                             011 = Receive FIFO becomes >= 3/4 full \n
                                             100 = Receive FIFO becomes >= 7/8 full \n
                                             101:111 = reserved. */
        uint32_t reserved6_15   : 10;
    } b;                                /*!< Register bits. */
} uartifls_data_t;

/**
 * @brief  This union represents the bit fields in the interrupt.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union uarti_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t rimi           : 1;    /*!< nUARTRI Modem Interrupt. */
        uint32_t ctsmi          : 1;    /*!< nUARTCTS Modem Interrupt. */
        uint32_t dcdmi          : 1;    /*!< nUARTDCD Modem Interrupt. */
        uint32_t dsrmi          : 1;    /*!< nUARTDSR Modem Interrupt. */
        uint32_t rxi            : 1;    /*!< Receive Interrupt. */
        uint32_t txi            : 1;    /*!< Transmit Interrupt. */
        uint32_t rti            : 1;    /*!< Receive Timeout Interrupt. */
        uint32_t frame_err_i    : 1;    /*!< Framing Error Interrupt. */
        uint32_t parity_err_i   : 1;    /*!< Parity Error Interrupt . */
        uint32_t break_err_i    : 1;    /*!< Break Error Interrupt. */
        uint32_t ovweeun_err_i  : 1;    /*!< Overrun Error Interrupt. */
        uint32_t reserved11_15  : 5;
    } b;                                /*!< Register bits. */
} uarti_data_t;

/**
 * @brief  This union represents the bit fields in the interrupt mask set/clear register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef uarti_data_t uartimsc_data_t;

/**
 * @brief  This union represents the bit fields in the raw interrupt status register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef uarti_data_t uartris_data_t;

/**
 * @brief  This union represents the bit fields in the masked interrupt status register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef uarti_data_t uartmis_data_t;

/**
 * @brief  This union represents the bit fields in the interrupt clear register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef uarti_data_t uarticr_data_t;

/**
 * @brief  This union represents the bit fields in the DMA control register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union uartdmacr_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t rxdmaen        : 1;    /*!< If this bit is set to 1, DMA for the receive FIFO is enabled. */
        uint32_t txdmaen        : 1;    /*!< If this bit is set to 1, DMA for the transmit FIFO is enabled. */
        uint32_t dmaonerr       : 1;    /*!< If this bit is set to 1, the DMA receive request outputs,
                                             <b>UARTRXDMASREQ</b> or <b>UARTRXDMABREQ</b>, are disabled
                                             when the UART error interrupt is asserted. */
        uint32_t reserved3_15   : 13;
    } b;                                /*!< Register bits. */
} uartdmacr_data_t;

/**
 * @brief  Registers associated with Uart.
 */
typedef struct uart_regs {
    volatile uint32_t uartdr;           /*!< Data read or written from the
                                             interface. It is 12 bits wide on a
                                             read, and 8 on a write.  <i>Offset: 00h</i>. */
    volatile uint32_t uartrsr;          /*!< Receive status register (read)/
                                             error clear register (write).  <i>Offset: 04h</i>. */
    volatile uint32_t res0;             /*!< Reserved.  <i>Offset: 08h</i>. */
    volatile uint32_t res1;             /*!< Reserved.  <i>Offset: 0Ch</i>. */
    volatile uint32_t res2;             /*!< Reserved.  <i>Offset: 10h</i>. */
    volatile uint32_t res3;             /*!< Reserved.  <i>Offset: 14h</i>. */
    volatile uint32_t uartfr;           /*!< Flag register (read only).  <i>Offset: 18h</i>. */
    volatile uint32_t res4;             /*!< Reserved.  <i>Offset: 1Ch</i>. */
    volatile uint32_t uartilpr;         /*!< IrDA low-power counter register.  <i>Offset: 20h</i>. */
    volatile uint32_t uartibrd;         /*!< Integer baud rate divisor register.  <i>Offset: 24h</i>. */
    volatile uint32_t uartfbrd;         /*!< Fractional baud rate divisor register.  <i>Offset: 28h</i>. */
    volatile uint32_t uartlcr_h;        /*!< Line control register, HIGH byte  <i>Offset: 2Ch</i>. */
    volatile uint32_t uartcr;           /*!< Control register.  <i>Offset: 30h</i>. */
    volatile uint32_t uartifls;         /*!< Interrupt FIFO level select register.  <i>Offset: 34h</i>. */
    volatile uint32_t uartimsc;         /*!< Interrupt mask set/clear.  <i>Offset: 38h</i>. */
    volatile uint32_t uartris;          /*!< Raw interrupt status.  <i>Offset: 3Ch</i>. */
    volatile uint32_t uartmis;          /*!< Masked interrupt status.  <i>Offset: 40h</i>. */
    volatile uint32_t uarticr;          /*!< Interrupt clear register.  <i>Offset: 44h</i>. */
    volatile uint32_t uartdmacr;        /*!< DMA control register.  <i>Offset: 48h</i>. */
} uart_regs_t;

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif