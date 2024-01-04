/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides V100 uart register \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-11-03, Create file. \n
 */
#ifndef HAL_UART_V100_REGS_DEF_H
#define HAL_UART_V100_REGS_DEF_H

#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_uart_v100_regs_def UART V100 Regs Definition
 * @ingroup  drivers_hal_uart
 * @{
 */

 /**
 * @brief  This union represents the bit fields in the Receive Buffer Register, Divisor Latch (Low) Register,
 *         Transmit Holding Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union rbr_dll_thr_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t rbr            : 8;    /*!< If in FIFO mode (FIFO_MODE != NONE) and FIFOs are enabled (FCR[0] set to 1)
                                             , this register accesses the head of the receive FIFO. If the receive FIFO
                                             is full and this register is not read before the next data character
                                             arrives, then the data already in the FIFO will be preserved but any
                                             incoming data will be lost and an over-run error occurs. */
        uint32_t reserved8_31   : 24;
    } rbr;                              /*!< Receive Buffer Register bits. */
    struct {
        uint32_t dll            : 8;    /*!< This register makes up the lower 8-bits of a 16-bit, read/write, Divisor
                                             Latch register that contains the baud rate divisor for the UART. */
        uint32_t reserved8_31   : 24;
    } dll;                              /*!< Divisor Latch (Low) Register bits. */
    struct {
        uint32_t thr            : 8;    /*!< This register contains data to be transmitted on the serial output port
                                             (sout) in UART mode or the serial infrared output (sir_out_n) in infrared
                                             mode. Data should only be written to the THR when the THR Empty (THRE) bit
                                             (LSR[5]) is set. If in non-FIFO mode or FIFO's are disabled (FCR[0] set to
                                             zero) and THRE is set, writing a single character to the THR clears the
                                             THRE. Any additional writes to the THR before the THRE is set again causes
                                             the THR data to be overwritten. If in FIFO mode and FIFO's are enabled
                                             (FCR[0] set to one) and THRE is set, x number of characters of data may be
                                             written to the THR before the FIFO is full. The number x (default=16) is
                                             determined by the value of FIFO Depth that is set during configuration. Any
                                             attempt to write data when the FIFO is full results in the write data being
                                             lost. */
        uint32_t reserved8_31   : 24;
    } thr;                              /*!< Transmit Holding Register bits. */
} rbr_dll_thr_data_t;

 /**
 * @brief  This union represents the bit fields in the Divisor Latch High (DLH) Register, Interrupt Enable Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union dlh_ier_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t dlh            : 8;    /*!< Upper 8-bits of a 16-bit, read/write, Divisor Latch register that contains
                                             the baud rate divisor for the UART. The output baud rate is equal to the
                                             serial clock (pclk if one clock design, sclk if two clock design
                                             (CLOCK_MODE == Enabled)) frequency divided by sixteen times the value of
                                             the baud rate divisor, as follows: baud rate = (serial clock freq) / (16 *
                                             divisor). */
        uint32_t reserved8_31   : 24;
    } dlh;                              /*!< Divisor Latch High (DLH) Register. */
    struct {
        uint32_t erbfi          : 1;    /*!< This is used to enable/disable the generation of Received Data Available
                                             Interrupt and the Character Timeout Interrupt (if in FIFO mode and FIFO's
                                             enabled). These are the second highest priority interrupts. */
        uint32_t etbei          : 1;    /*!< This is used to enable/disable the generation of Transmitter Holding
                                             Register Empty Interrupt. This is the third highest priority interrupt. */
        uint32_t elsi           : 1;    /*!< This is used to enable/disable the generation of Receiver Line Status
                                             Interrupt. This is the highest priority interrupt. */
        uint32_t edssi          : 1;    /*!< This is used to enable/disable the generation of Modem Status Interrupt.
                                             This is the fourth highest priority interrupt. */
        uint32_t elcolr         : 1;    /*!< Interrupt Enable Register: ELCOLR, this bit controls the method for
                                             clearing the status in the LSR register. This is applicable only for
                                             Overrun Error, Parity Error, Framing Error, and Break Interrupt status
                                             bits. 0 = LSR status bits are cleared either on reading Rx FIFO (RBR Read)
                                             or On reading LSR register. 1 = LSR status bits are cleared only on reading
                                             LSR register. Writeable only when LSR_STATUS_CLEAR == Enabled, always
                                             readable. */
        uint32_t reserved5_6    : 2;
        uint32_t ptime          : 1;    /*!< Writeable only when THRE_MODE_USER == Enabled, always readable. This is
                                             used to enable/disable the generation of THRE Interrupt. */
        uint32_t reserved8_31   : 24;
    } ier;                              /*!< Interrupt Enable Register. */
} dlh_ier_data_t;

 /**
 * @brief  This union represents the bit fields in the FIFO Control Register, Interrupt Identification Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union fcr_iir_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t fifoe          : 1;    /*!< This enables/disables the transmit (XMIT) and receive (RCVR) FIFOs.
                                             Whenever the value of this bit is changed both the XMIT and RCVR controller
                                             portion of FIFOs is reset. */
        uint32_t rfifor         : 1;    /*!< This resets the control portion of the receive FIFO and treats the FIFO as
                                             empty. This will also de-assert the DMA RX request and single signals when
                                             additional DMA handshaking signals are selected (DMA_EXTRA == YES). Note
                                             that this bit is 'self_clearing' and it is not necessary to clear this
                                             bit. */
        uint32_t xfifor         : 1;    /*!< This resets the control portion of the transmit FIFO and treats the FIFO as
                                             empty. This will also de-assert the DMA TX request and single signals when
                                             additional DMA handshaking signals are selected (DMA_EXTRA == YES). Note
                                             that this bit is 'self_clearing' and it is not necessary to clear this
                                             bit. */
        uint32_t dmam           : 1;    /*!< This determines the DMA signalling mode used for the dma_tx_req_n and
                                             dma_rx_req_n output signals when additional DMA handshaking signals are not
                                             selected (DMA_EXTRA == NO). */
        uint32_t tet            : 2;    /*!< Writes will have no effect when THRE_MODE_USER == Disabled. This is used to
                                             select the empty threshold level at which the THRE Interrupts will be
                                             generated when the mode is active. It also determines when the dma_tx_req_n
                                             signal will be asserted when in certain modes of operation. */
        uint32_t rt             : 2;    /*!< This is used to select the trigger level in the receiver FIFO at which the
                                             Received Data Available Interrupt will be generated. In auto flow control
                                             mode, it is used to determine when the rts_n signal will be de-asserted
                                             only when RTC_FCT is disabled. It also determines when the dma_rx_req_n
                                             signal will be asserted when in certain modes of operation. */
        uint32_t reserved8_31   : 24;
    } fcr;                              /*!< FIFO Control Register. */
    struct {
        uint32_t iid            : 4;    /*!< This indicates the highest priority pending interrupt which can be one of
                                             the following types specified in Values. */
        uint32_t reserved4_5    : 2;
        uint32_t fifose         : 2;    /*!< This is used to indicate whether the FIFOs are enabled or disabled. */
        uint32_t reserved8_31   : 24;
    } iir;                              /*!< Interrupt Identification Register. */
} fcr_iir_data_t;

 /**
 * @brief  This union represents the bit fields in the Line Control Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union lcr_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t dls            : 2;    /*!< - 0x0 (CHAR_5BITS): 5 data bits per character,
                                             - 0x1 (CHAR_6BITS): 6 data bits per character,
                                             - 0x2 (CHAR_7BITS): 7 data bits per character,
                                             - 0x2 (CHAR_7BITS): 8 data bits per character. */
        uint32_t stop           : 1;    /*!< - 0x0 (STOP_1BIT): 1 stop bit, 0x1 (STOP_1_5BIT_OR_2BIT): 1.5 stop bits
                                             when DLS(LCR[1:0]) is zero, else 2 stop bit. */
        uint32_t pen            : 1;    /*!< - 0x0 (DISABLED): disable parity, 0x1 (ENABLED): enable parity. */
        uint32_t eps            : 1;    /*!< - 0x0 (ODD_PARITY): an odd parity is transmitted or checked,
                                             - 0x1 (EVEN_PARITY): an even parity is transmitted orchecked. */
        uint32_t sp             : 1;    /*!< - 0x0 (DISABLED): Stick parity disabled, 0x1 (ENABLED): Stick parity
                                             enabled. */
        uint32_t bc             : 1;    /*!< - 0x0 (DISABLED): Serial output is released for data transmission,
                                             - 0x1 (ENABLED): Serial output is forced to spacing state. */
        uint32_t dlab           : 1;    /*!< - 0x0 (DISABLED): Divisor Latch register is writable only when UART Not
                                             BUSY,
                                             - 0x1 (ENABLED): Divisor Latch register is always readable and writable. */
        uint32_t reserved8_31   : 24;
    } b;                                /*!< Register bits. */
} lcr_data_t;

 /**
 * @brief  This union represents the bit fields in the Modem Control Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union mcr_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t dtr            : 1;    /*!< This is used to directly control the Data Terminal Ready (dtr_n) output.
                                             The value written to this location is inverted and driven out on dtr_n. The
                                             Data Terminal Ready output is used to inform the modem or data set that the
                                             UART is ready to establish communications. Note that in Loopback mode
                                             (MCR[4] set to one), the dtr_n output is held inactive high while the value
                                             of this location is internally looped back to an input. */
        uint32_t rts            : 1;    /*!< - 0x0 (INACTIVE): Request to Send rts_n de-asserted (logic 1),
                                             - 0x1 (ACTIVE): Request to Send rts_n asserted (logic 0). */
        uint32_t out1           : 1;    /*!< This is used to directly control the user-designated Output1 (out1_n)
                                             output. The value written to this location is inverted and driven out on
                                             out1_n. Note that in Loopback mode (MCR[4] set to one), the out1_n output
                                             is held inactive high while the value of this location is internally looped
                                             back to an input. */
        uint32_t out2           : 1;    /*!< This is used to directly control the user-designated Output2 (out2_n)
                                             output. The value written to this location is inverted and driven out on
                                             out2_n. Note that in Loopback mode (MCR[4] set to one), the out2_n output
                                             is held inactive high while the value of this location is internally looped
                                             back to an input. */
        uint32_t loopback       : 1;    /*!< This is used to put the UART into a diagnostic mode for test purposes. If
                                             operating in UART mode (SIR_MODE != Enabled OR NOT active, MCR[6] set to
                                             zero), data on the sout line is held high, while serial data output is
                                             looped back to the sin line, internally. In this mode all the interrupts
                                             are fully functional. Also, in loopback mode, the modem control inputs
                                             (dsr_n, cts_n, ri_n, dcd_n) are disconnected and the modem control outputs
                                             (dtr_n, rts_n, out1_n, out2_n) are looped back to the inputs, internally.
                                             If operating in infrared mode (SIR_MODE == Enabled AND active, MCR[6] set ]
                                             to one), data on the sir_out_n line is held low, while serial data output
                                             is inverted and looped back to the sir_in line. */
        uint32_t afce           : 1;    /*!< Writeable only when AFCE_MODE == Enabled, always readable. When FIFOs are
                                             enabled and the Auto Flow Control Enable (AFCE) bit is set, Auto Flow
                                             Control features are enabled as described in section 'Auto Flow Control' in
                                             data book. */
        uint32_t sire           : 1;    /*!< Writeable only when SIR_MODE == Enabled, always readable. This is used to
                                             enable/ disable the IrDA SIR Mode features as described in section
                                             'IrDA 1.0 SIR Protocol' in the databook. */
        uint32_t reserved7_31   : 25;
    } b;                                /*!< Register bits. */
} mcr_data_t;

 /**
 * @brief  This union represents the bit fields in the Line Status Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union lsr_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t dr             : 1;    /*!< This is used to indicate that the receiver contains at least one character
                                             in the RBR or the receiver FIFO. This bit is cleared when the RBR is read
                                             in the non-FIFO mode, or when the receiver FIFO is empty, in the FIFO
                                             mode. */
        uint32_t oe             : 1;    /*!< This is used to indicate the occurrence of an overrun error. This occurs if
                                             a new data character was received before the previous data was read.
                                             - 0x0 (NO_OVER_RUN_ERROR): no overrun error
                                             - 0x1 (OVER_RUN_ERROR): overrun error. */
        uint32_t pe             : 1;    /*!< This is used to indicate the occurrence of a parity error in the receiver
                                             if the Parity Enable (PEN) bit (LCR[3]) is set.
                                             - 0x0 (NO_PARITY_ERROR): no parity error
                                             - 0x1 (PARITY_ERROR): parity error. */
        uint32_t fe             : 1;    /*!< This is used to indicate the occurrence of a framing error in the receiver.
                                             A framing error occurs when the receiver does not detect a valid STOP bit
                                             in the received data.
                                             - 0x0 (NO_FRAMING_ERROR): no framing error
                                             - 0x1 (FRAMING_ERROR): framing error. */
        uint32_t bi             : 1;    /*!< This is used to indicate the detection of a break sequence on the serial
                                             input data.
                                             - 0x0 (NO_BREAK): No break sequence detected
                                             - 0x1 (BREAK): Break sequence detected. */
        uint32_t thre           : 1;    /*!< If THRE_MODE_USER = Disabled or THRE mode is disabled (IER[7] set to zero)
                                             and regardless of FIFO's being implemented/enabled or not, this bit
                                             indicates that the THR or TX FIFO is empty. */
        uint32_t temt           : 1;    /*!< If in FIFO mode (FIFO_MODE != NONE) and FIFO's enabled (FCR[0] set to one),
                                             this bit is set whenever the Transmitter Shift Register and the FIFO are
                                             both empty. If in the non-FIFO mode or FIFO's are disabled, this bit is set
                                             whenever the Transmitter Holding Register and the Transmitter Shift
                                             Register are both empty. */
        uint32_t rfe            : 1;    /*!< This bit is only relevant when FIFO_MODE != NONE AND FIFO's are enabled
                                             (FCR[0] set to one). This is used to indicate if there is at least one
                                             parity error, framing error, or break indication in the FIFO.
                                             This bit is cleared when the LSR is read and the character with the error
                                             is at the top of the receiver FIFO and there are no subsequent errors in
                                             the FIFO. */
        uint32_t addr_rcvd      : 1;    /*!< If 9Bit data mode (LCR_EXT[0]=1) is enabled, this bit is used to indicate
                                             the 9th bit of the receive data is set to 1. This bit can also be used to
                                             indicate whether the incoming character is address or data.
                                             - 1 = Indicates the character is address.
                                             - 0 = Indicates the character is data. */
        uint32_t reserved9_31   : 23;
    } b;                                /*!< Register bits. */
} lsr_data_t;

 /**
 * @brief  This union represents the bit fields in the Modem Status Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union msr_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t dcts           : 1;    /*!< This is used to indicate that the modem control line cts_n has changed
                                             since the last time the MSR was read. Reading the MSR clears the DCTS bit.
                                             In Loopback Mode (MCR[4] set to one), DCTS reflects changes on MCR[1]
                                             (RTS). */
        uint32_t ddsr           : 1;    /*!< This is used to indicate that the modem control line dsr_n has changed
                                             since the last time the MSR was read. Reading the MSR clears the DDSR bit.
                                             In Loopback Mode (MCR[4] set to one), DDSR reflects changes on MCR[0]
                                             (DTR). */
        uint32_t teri           : 1;    /*!< This is used to indicate that a change on the input ri_n (from an active
                                             low, to an inactive high state) has occurred since the last time the MSR
                                             was read. */
        uint32_t ddcd           : 1;    /*!< This is used to indicate that the modem control line dcd_n has changed
                                             since the last time the MSR was read. Reading the MSR clears the DDCD bit.
                                             In Loopback Mode (MCR[4] set to one), DDCD reflects changes on MCR[3]
                                             (Out2). */
        uint32_t cts            : 1;    /*!< This is used to indicate the current state of the modem control line cts_n.
                                             That is, this bit is the complement cts_n. When the Clear to Send input
                                             (cts_n) is asserted it is an indication that the modem or data set is ready
                                             to exchange data with the DW_apb_uart. In Loopback Mode (MCR[4] set to one)
                                             , CTS is the same as MCR[1] (RTS). */
        uint32_t dsr            : 1;    /*!< This is used to indicate the current state of the modem control line dsr_n.
                                             That is this bit is the complement dsr_n. When the Data Set Ready input
                                             (dsr_n) is asserted it is an indication that the modem or data set is ready
                                             to establish communications with the DW_apb_uart. In Loopback Mode (MCR[4]
                                             set to one), DSR is the same as MCR[0] (DTR). */
        uint32_t ri             : 1;    /*!< This is used to indicate the current state of the modem control line ri_n.
                                             That is this bit is the complement ri_n. When the Ring Indicator input
                                             (ri_n) is asserted it is an indication that a telephone ringing signal has
                                             been received by themodem or data set. In Loopback Mode (MCR[4] set to one)
                                             , RI is the same as MCR[2] (Out1). */
        uint32_t dcd            : 1;    /*!< This is used to indicate the current state of the modem control line dcd_n.
                                             That is this bit is the complement dcd_n. When the Data Carrier Detect
                                             input (dcd_n) is asserted it is an indication that the carrier has been
                                             detected by the modem or data set. In Loopback Mode (MCR[4] set to one),
                                             DCD is the same as MCR[3] (Out2). */
        uint32_t reserved8_31   : 24;
    } b;                                /*!< Register bits. */
} msr_data_t;

 /**
 * @brief  This union represents the bit fields in the Scratchpad Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union scr_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t scr            : 8;    /*!< This register is for programmers to use as a temporary storage space. It
                                             has no defined purpose in the DW_apb_uart. */
        uint32_t reserved8_31   : 24;
    } b;                                /*!< Register bits. */
} scr_data_t;

 /**
 * @brief  This union represents the bit fields in the Low Power Divisor Latch Low Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union lpdll_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t lpdll          : 8;    /*!< This register makes up the lower 8-bits of a 16-bit, read/write, Low Power
                                             Divisor Latch register that contains the baud rate divisor for the UART
                                             which must give a baud rate of 115.2K. This is required for SIR Low Power
                                             (minimum pulse width) detection at the receiver. The output low power baud
                                             rate is equal to the serial clock (sclk) frequency divided by sixteen times
                                             the value of the baud rate divisor, as follows:
                                             Low power baud rate = (serial clock freq) / (16 * divisor) Therefore a
                                             divisor must be selected to give a baud rate of 115.2K. */
        uint32_t reserved8_31   : 24;
    } b;                                /*!< Register bits. */
} lpdll_data_t;

 /**
 * @brief  This union represents the bit fields in the Low Power Divisor Latch High Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union lpdlh_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t lpdlh          : 8;    /*!< This register makes up the upper 8-bits of a 16-bit, read/write, Low Power
                                             Divisor Latch register that contains the baud rate divisor for the UART
                                             which must give a baud rate of 115.2K. This is required for SIR Low Power
                                             (minimum pulse width) detection at the receiver. The output low power baud
                                             rate is equal to the serial clock (sclk) frequency divided by sixteen times
                                             the value of the baud rate divisor, as follows:
                                             Low power baud rate = (serial clock freq) / (16 * divisor) Therefore a
                                             divisor must be selected to give a baud rate of 115.2K. */
        uint32_t reserved8_31   : 24;
    } b;                                /*!< Register bits. */
} lpdlh_data_t;

 /**
 * @brief  This union represents the bit fields in the Shadow Receive Buffer Register, Shadow Transmit Holding Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union srbrn_sthrn_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t srbrn          : 8;    /*!< This is a shadow register for the RBR and has been allocated sixteen 32-bit
                                             locations so as to accommodate burst accesses from the master. This
                                             register contains the data byte received on the serial input port (sin) in
                                             UART mode or the serial infrared input (sir_in) in infrared mode. The data
                                             in this register is valid only if the Data Ready (DR) bit in the Line
                                             status Register (LSR) is set. If in non-FIFO mode (FIFO_MODE == NONE) or
                                             FIFOs are disabled (FCR[0] set to zero), the data in the RBR must be read
                                             before the next data arrives, otherwise it will be overwritten, resulting
                                             in an overrun error. */
        uint32_t reserved8_31   : 24;
    } srbrn;                                /*!< Register bits. */

    struct {
        uint32_t sthrn          : 8;    /*!< This is a shadow register for the THR and has been allocated sixteen 32-bit
                                             locations so as to accommodate burst accesses from the master. This
                                             register contains data to be transmitted on the serial output port (sout)
                                             in UART mode or the serial infrared output (sir_out_n) in infrared mode.
                                             Data should only be written to the THR when the THR Empty (THRE) bit
                                             (LSR[5]) is set. */
        uint32_t reserved8_31   : 24;
    } sthrn;                            /*!< Register bits. */
} srbrn_sthrn_data_t;

 /**
 * @brief  This union represents the bit fields in the FIFO Access Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union far_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t far            : 1;    /*!< Writes will have no effect when FIFO_ACCESS == No, always readable. This
                                             register is use to enable a FIFO access mode for testing, so that the
                                             receive FIFO can be written by the master and the transmit FIFO can be read
                                             by the master when FIFO's are implemented and enabled. When FIFOs are not
                                             implemented or not enabled it allows the RBR to be written by the master
                                             and the THR to be read by the master. */
        uint32_t reserved1_31   : 24;
    } b;                                /*!< Register bits. */
} far_data_t;

 /**
 * @brief  This union represents the bit fields in the Transmit FIFO Read Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union tfr_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t tfr            : 8;    /*!< These bits are only valid when FIFO access mode is enabled (FAR[0] is set
                                             to one). When FIFO's are implemented and enabled, reading this register
                                             gives the data at the top of the transmit FIFO. Each consecutive read pops
                                             the transmit FIFO and gives the next data value that is currently at the
                                             top of the FIFO. When FIFO's are not implemented or not enabled, reading
                                             this register gives the data in the THR. */
        uint32_t reserved8_31   : 24;
    } b;                                /*!< Register bits. */
} tfr_data_t;

 /**
 * @brief  This union represents the bit fields in the Receive FIFO Write Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union rfw_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t rfwd           : 8;    /*!< These bits are only valid when FIFO access mode is enabled (FAR[0] is set
                                             to one). When FIFO's are implemented and enabled, the data that is written
                                             to the RFWD is pushed into the receive FIFO. Each consecutive write pushes
                                             the new data to the next write location in the receive FIFO. When FIFO's
                                             are not implemented or not enabled, the data that is written to the RFWD is
                                             pushed into the RBR. */
        uint32_t rfpe           : 1;    /*!< These bits are only valid when FIFO access mode is enabled (FAR[0] is set
                                             to one). When FIFO's are implemented and enabled, this bit is used to write
                                             parity error detection information to the receive FIFO. When FIFO's are not
                                             implemented or not enabled, this bit is used to write parity error
                                             detection information to the RBR. */
        uint32_t rffe           : 1;    /*!< These bits are only valid when FIFO access mode is enabled (FAR[0] is set
                                             to one). When FIFO's are implemented and enabled, this bit is used to write
                                             framing error detection information to the receive FIFO. When FIFO's are
                                             not implemented or not enabled, this bit is used to write framing error
                                             detection information to the RBR. */
        uint32_t reserved10_31  : 22;
    } b;                                /*!< Register bits. */
} rfw_data_t;

 /**
 * @brief  This union represents the bit fields in the UART Status Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union usr_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t busy           : 1;    /*!< - 0x0 (IDLE): DW_apb_uart is idle or inactive
                                             - 0x1 (BUSY): DW_apb_uart is busy (actively transferring data). */
        uint32_t tfnf           : 1;    /*!< This bit is only valid when FIFO_STAT == YES. This is used to indicate that
                                             the transmit FIFO in not full. This bit is cleared when the TX FIFO is
                                             full. */
        uint32_t tfe            : 1;    /*!< This bit is only valid when FIFO_STAT == YES. This is used to indicate that
                                             the transmit FIFO is completely empty. This bit is cleared when the TX FIFO
                                             is no longer empty. */
        uint32_t rfne           : 1;    /*!< This bit is only valid when FIFO_STAT == YES. This is used to indicate that
                                             the receive FIFO contains one or more entries. This bit is cleared when the
                                             RX FIFO is empty. */
        uint32_t rff            : 1;    /*!< This bit is only valid when FIFO_STAT == YES. This is used to indicate that
                                             the receive FIFO is completely full. That is: This bit is cleared when the
                                             RX FIFO is no longer full. */
        uint32_t reserved5_31   : 27;
    } b;                                /*!< Register bits. */
} usr_data_t;

 /**
 * @brief  This union represents the bit fields in the Transmit FIFO Level Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union tfl_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t tfl            : 8;    /*!< This indicates the number of data entries in the transmit FIFO. */
        uint32_t reserved8_31   : 24;
    } b;                                /*!< Register bits. */
} tfl_data_t;

 /**
 * @brief  This union represents the bit fields in the Receive FIFO Level Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union rfl_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t rfl            : 8;    /*!< This is indicates the number of data entries in the receive FIFO. */
        uint32_t reserved8_31   : 24;
    } b;                                /*!< Register bits. */
} rfl_data_t;

 /**
 * @brief  This union represents the bit fields in the Software Reset Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union srr_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t ur             : 1;    /*!< This asynchronously resets the DW_apb_uart and synchronously removes the
                                             reset assertion. For a two clock implementation both pclk and sclk domains
                                             will be reset. */
        uint32_t rfr            : 1;    /*!< Writes will have no effect when FIFO_MODE == NONE. This is a shadow
                                             register for the RCVR FIFO Reset bit (FCR[1]). This can be used to remove
                                             the burden on software having to store previously written FCR values
                                             (which are pretty static) just to reset the reeive FIFO. This resets the
                                             control portion of the receive FIFO and treats the FIFO as empty. This will
                                             also de-assert the DMA RX request and single signals when additional DMA
                                             handshaking signals are selected (DMA_EXTRA == YES). Note that this bit is
                                             'self_clearing' and it is not necessary to clear this bit. */
        uint32_t xfr            : 1;    /*!< Writes will have no effect when FIFO_MODE == NONE. This is a shadow
                                             register for the XMIT FIFO Reset bit (FCR[2]). This can be used to remove
                                             the burden on software having to store previously written FCR values (which
                                             are pretty static) just to reset the transmit FIFO. This resets the control
                                             portion of the transmit FIFO and treats the FIFO as empty. This will also
                                             de-assert the DMA TX request and single signals when additional DMA
                                             handshaking signals are selected (DMA_EXTRA = YES). Note that this bit is
                                             'self-clearing'. It is not necessary to clear this bit. */
        uint32_t reserved3_31   : 29;
    } b;                                /*!< Register bits. */
} srr_data_t;

 /**
 * @brief  This union represents the bit fields in the Receive FIFO Level Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union srts_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t strs           : 1;    /*!< This is a shadow register for the RTS bit (MCR[1]), this can be used to
                                             remove the burden of having to performing a read modify write on the MCR.
                                             This is used to directly control the Request to Send (rts_n) output. The
                                             Request To Send (rts_n) output is used to inform the modem or data set that
                                             the UART is ready to exchange data. */
        uint32_t reserved1_31   : 31;
    } b;                                /*!< Register bits. */
} strs_data_t;

 /**
 * @brief  This union represents the bit fields in the Shadow Break Control Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union sbcr_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t sbcb           : 1;    /*!< This is a shadow register for the Break bit (LCR[6]), this can be used to
                                             remove the burden of having to performing a read modify write on the LCR.
                                             This is used to cause a break condition to be transmitted to the receiving
                                             device. If set to one the serial output is forced to the spacing (logic 0)
                                             state. When not in Loopback Mode, as determined by MCR[4], the sout line is
                                             forced low until the Break bit is cleared. */
        uint32_t reserved1_31   : 31;
    } b;                                /*!< Register bits. */
} sbcr_data_t;

 /**
 * @brief  This union represents the bit fields in the Shadow DMA Mode Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union sdmam_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t sdmam          : 1;    /*!< This is a shadow register for the DMA mode bit (FCR[3]). This can be used
                                             to remove the burden of having to store the previously written value to the
                                             FCR in memory and having to mask this value so that only the DMA Mode bit
                                             gets updated. This determines the DMA signalling mode used for the
                                             dma_tx_req_n and dma_rx_req_n output signals when additional DMA
                                             handshaking signals are not selected (DMA_EXTRA == NO). See section 5.9 on
                                             page 54 for details on DMA support. */
        uint32_t reserved1_31   : 31;
    } b;                                /*!< Register bits. */
} sdmam_data_t;

 /**
 * @brief  This union represents the bit fields in the Shadow FIFO Enable Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union sfe_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t sfe            : 1;    /*!< This is a shadow register for the FIFO enable bit (FCR[0]). This can be
                                             used to remove the burden of having to store the previously written value
                                             to the FCR in memory and having to mask this value so that only the FIFO
                                             enable bit gets updated. This enables/disables the transmit (XMIT) and
                                             receive (RCVR) FIFO's. If this bit is set to zero (disabled) after being
                                             enabled then both the XMIT and RCVR controller portion of FIFO's will be
                                             reset. */
        uint32_t reserved1_31   : 31;
    } b;                                /*!< Register bits. */
} sfe_data_t;

 /**
 * @brief  This union represents the bit fields in the Shadow RCVR Trigger Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union srt_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t srt            : 2;    /*!< This is a shadow register for the RCVR trigger bits (FCR[7:6]). This can be
                                             used to remove the burden of having to store the previously written value
                                             to the FCR in memory and having to mask this value so that only the RCVR
                                             trigger bit gets updated.
                                             This is used to select the trigger level in the receiver FIFO at which the
                                             Received Data Available Interrupt will be generated. It also determines
                                             when the dma_rx_req_n signal will be asserted when DMA Mode (FCR[3]) is set
                                             to one. */
        uint32_t reserved2_31   : 30;
    } b;                                /*!< Register bits. */
} srt_data_t;

 /**
 * @brief  This union represents the bit fields in the Shadow TX Empty Trigger Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union stet_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t stet           : 2;    /*!< This is a shadow register for the TX empty trigger bits (FCR[5:4]). This
                                             can be used to remove the burden of having to store the previously written
                                             value to the FCR in memory and having to mask this value so that only the
                                             TX empty trigger bit gets updated. Writes will have no effect when
                                             THRE_MODE_USER == Disabled. This is used to select the empty threshold
                                             level at which the THRE Interrupts will be generated when the mode is
                                             active. */
        uint32_t reserved2_31   : 30;
    } b;                                /*!< Register bits. */
} stet_data_t;

 /**
 * @brief  This union represents the bit fields in the Halt TX Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union htx_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t htx            : 1;    /*!< Writes will have no effect when FIFO_MODE == NONE, always readable. This
                                             register is use to halt transmissions for testing, so that the transmit
                                             FIFO can be filled by the master when FIFO's are implemented and enabled.
                                             Note, if FIFO's are implemented and not enabled the setting of the halt TX
                                             register will have no effect on operation. */
        uint32_t reserved1_31   : 31;
    } b;                                /*!< Register bits. */
} htx_data_t;

 /**
 * @brief  This union represents the bit fields in the DMA Software Acknowledge Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union dmasa_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t dmasa          : 1;    /*!< Writes will have no effect when DMA_EXTRA == No. This register is use to
                                             perform DMA software acknowledge if a transfer needs to be terminated due
                                             to an error condition. For example, if the DMA disables the channel, then
                                             the DW_apb_uart should clear its request. This will cause the TX request,
                                             TX single, RX request and RX single signals to de_assert. Note that this
                                             bit is 'self-clearing' and it is not necessary to clear this bit. */
        uint32_t reserved1_31   : 31;
    } b;                                /*!< Register bits. */
} dmasa_data_t;

 /**
 * @brief  This union represents the bit fields in the Transceiver Control Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union tcr_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t rs485_en       : 1;    /*!< - 0 : In this mode, the transfers are still in the RS232 mode. All other
                                             fields in this register are reserved and register DE_EN/RE_EN/TAT are also
                                             reserved.
                                             - 1 : In this mode, the transfers will happen in RS485 mode. All other
                                             fields of this register are applicable. */
        uint32_t re_pol         : 1;    /*!< - 1: DE signal is active high, - 0: DE signal is active low. */
        uint32_t de_pol         : 1;    /*!< - 1: DE signal is active high, - 0: DE signal is active low. */
        uint32_t xfer_mode      : 2;    /*!< - 0: In this mode, transmit and receive can happen simultaneously. The user
                                             can enable DE_EN, RE_EN at any point of time. Turn around timing as
                                             programmed in the TAT register is not applicable in this mode.
                                             - 1: In this mode, DE and RE are mutually exclusive. Either DE or RE only
                                             one of them is expected to be enabled through programming. Hardware will
                                             consider the Turn Around timings which are programmed in the TAT register
                                             while switching from RE to DE or DE to RE. For transmission Hardware will
                                             wait if it is in middle of receiving any transfer, before it starts
                                             transmitting.
                                             2: In this mode, DE and RE are mutually exclusive. Once DE_EN/RE_EN is
                                             programed - by default 're' will be enabled and DW_apb_uart controller will
                                             be ready to receive. If the user programs the TX FIFO with the data then
                                             DW_apb_uart, after ensuring no receive is in progress, disable 're' and
                                             enable 'de' signal. */
        uint32_t reserved5_31   : 27;
    } b;                                /*!< Register bits. */
} tcr_data_t;

 /**
 * @brief  This union represents the bit fields in the Receiver Output Enable Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union de_en_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t de_enable      : 1;    /*!< The 'DE Enable' register bit is used to control assertion and de-assertion
                                             of 'de' signal. - 0: De-assert 'de' signal - 1: Assert 'de' signal. */
        uint32_t reserved1_31   : 31;
    } b;                                /*!< Register bits. */
} de_en_data_t;

 /**
 * @brief  This union represents the bit fields in the Driver Output Enable Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union re_en_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t re_enable      : 1;    /*!< The 'RE Enable' register bit is used to control assertion and de-assertion
                                             of 're' signal. - 0: De-assert 're' signal - 1: Assert 're' signal. */
        uint32_t reserved1_31   : 31;
    } b;                                /*!< Register bits. */
} re_en_data_t;

 /**
 * @brief  This union represents the bit fields in the Driver Output Enable Timing Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union det_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t de_assertion_time     : 8;    /*!< This field controls the amount of time (in terms of number of serial
                                                    clock periods) between the assertion of rising edge of Driver output
                                                    enable signal to serial transmit enable. Any data in transmit buffer
                                                    , will start on serial output (sout) after the transmit enable. */
        uint32_t reserved8_15          : 8;
        uint32_t de_de_assertion_time  : 8;    /*!< This field controls the amount of time (in terms of number of serial
                                                    clock periods) between the end of stop bit on the sout to the
                                                    falling edge of Driver output enable signal. */
        uint32_t reserved24_31         : 8;
    } b;                                /*!< Register bits. */
} det_data_t;

 /**
 * @brief  This union represents the bit fields in the TurnAround Timing Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union tat_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t de_to_re       : 16;   /*!< Turnaround time (in terms of serial clock) for DE De_assertion to RE
                                             assertion. */
        uint32_t re_to_de       : 16;   /*!< Turnaround time (in terms of serial clock) for RE De_assertion to DE
                                             assertion. */
    } b;                                /*!< Register bits. */
} tat_data_t;

 /**
 * @brief  This union represents the bit fields in the Divisor Latch Fraction Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union dlf_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t dlf            : CONFIG_UART_DLF_SIZE;  /*!< The fractional value is added to integer value set by DLH,
                                                              DLL. Fractional value is determined by (Divisor
                                                              Fractionvalue)/(2^DLF_SIZE). */
        uint32_t reserved       : 32 - CONFIG_UART_DLF_SIZE;
    } b;                                /*!< Register bits. */
} dlf_data_t;

 /**
 * @brief  This union represents the bit fields in the Receive Address Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union rar_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t rar            : 8;    /*!< This is an address matching register during receive mode. If the 9-th bit
                                             is set in the incoming character then the remaining 8-bits will be checked
                                             against this register value. If the match happens then sub-sequent
                                             characters with 9-th bit set to 0 will be treated as data byte until the
                                             next address byte is received. */
        uint32_t reserved8_31   : 24;
    } b;                                /*!< Register bits. */
} rar_data_t;

 /**
 * @brief  This union represents the bit fields in the Transmit Address Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union tar_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t tar            : 8;    /*!< This is an address matching register during transmit mode. If DLS_E
                                             (LCR_EXT[0]) bit is enabled, then DW_apb_uart will send the 9-bit character
                                             with 9-th bit set to 1 and remaining 8-bit address will be sent from this
                                             register provided 'SEND_ADDR' (LCR_EXT[2]) bit is set to 1. */
        uint32_t reserved8_31   : 24;
    } b;                                /*!< Register bits. */
} tar_data_t;

 /**
 * @brief  This union represents the bit fields in the Line Extended Control Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union lcr_ext_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t dls_e          : 1;    /*!< Extension for DLS. This bit is used to enable 9-bit data for transmit and
                                             receive transfers. */
        uint32_t addr_match     : 1;    /*!< Address Match Mode.This bit is used to enable the address match feature
                                             during receive. */
        uint32_t send_addr      : 1;    /*!< Send address control bit. This bit is used as a control knob for the user
                                             to determine when to send the address during transmit mode. */
        uint32_t transmit_mode  : 1;    /*!< Transmit mode control bit. This bit is used to control the type of transmit
                                             mode during 9-bit data transfers. */
        uint32_t reserved4_31   : 28;
    } b;                                /*!< Register bits. */
} lcr_ext_data_t;

 /**
 * @brief  This union represents the bit fields in the Component Parameter Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union cpr_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t apb_data_width             : 2;    /*!< - 0x0 (APB_8BITS): APB data width is 8 bits
                                                         - 0x1 (APB_16BITS): APB data width is 16 bits
                                                         - 0x2 (APB_32BITS): APB data width is 32 bits. */
        uint32_t reserved2_3                : 2;
        uint32_t afce_mode                  : 1;    /*!< - 0x0 (DISABLED): AFCE mode disabled
                                                         - 0x1 (ENABLED): AFCE mode enabled. */
        uint32_t thre_mode                  : 1;    /*!< - 0x0 (DISABLED): THRE mode disabled
                                                         - 0x1 (ENABLED): THRE mode enabled. */
        uint32_t sir_mode                   : 1;    /*!< - 0x0 (DISABLED): SIR mode disabled
                                                         - 0x1 (ENABLED): SIR mode enabled. */
        uint32_t sir_lp_mode                : 1;    /*!< - 0x0 (DISABLED): SIR_LP mode disabled
                                                         - 0x1 (ENABLED): SIR_LP mode enabled. */
        uint32_t additional_feat            : 1;    /*!< - 0x0 (DISABLED): Additional features disabled
                                                         - 0x1 (ENABLED): Additional features enabled. */
        uint32_t fifo_access                : 1;    /*!< - 0x0 (DISABLED): FIFO_ACCESS disabled
                                                         - 0x1 (ENABLED): FIFO_ACCESS enabled. */
        uint32_t fifo_stat                  : 1;    /*!< - 0x0 (DISABLED): FIFO_STAT disabled
                                                         - 0x1 (ENABLED): FIFO_STAT enabled. */
        uint32_t shadow                     : 1;    /*!< - 0x0 (DISABLED): SHADOW disabled
                                                         - 0x1 (ENABLED): SHADOW enabled. */
        uint32_t uart_add_encoded_params    : 1;    /*!< - 0x0 (DISABLED): UART_ADD_ENCODED_PARAMS disabled
                                                         - 0x1 (ENABLED): UART_ADD_ENCODED_PARAMS enabled. */
        uint32_t dma_extra                  : 1;    /*!< - 0x0 (DISABLED): DMA_EXTRA disabled
                                                         - 0x1 (ENABLED): DMA_EXTRA enabled. */
        uint32_t reserved14_15              : 2;
        uint32_t fifo_mode                  : 8;    /*!< - 0x0 (FIFO_MODE_0): FIFO mode is 0
                                                         - 0x1 (FIFO_MODE_16): FIFO mode is 16
                                                         - 0x2 (FIFO_MODE_32): FIFO mode is 32
                                                         - 0x4 (FIFO_MODE_64): FIFO mode is 64
                                                         - 0x8 (FIFO_MODE_128): FIFO mode is 128
                                                         - 0x10 (FIFO_MODE_256): FIFO mode is 256
                                                         - 0x20 (FIFO_MODE_512): FIFO mode is 512
                                                         - 0x40 (FIFO_MODE_1024): FIFO mode is 1024
                                                         - 0x80 (FIFO_MODE_2048): FIFO mode is 2048. */
        uint32_t reserved24_31              : 8;
    } b;                                /*!< Register bits. */
} cpr_data_t;

 /**
 * @brief  This union represents the bit fields in the UART Component Version Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union ucv_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t uart_component_version     : 32;    /*!< ASCII value for each number in the version, followed by *. For
                                                         example 32_30_31_2A represents the version 2.01*. */
    } b;                                /*!< Register bits. */
} ucv_data_t;

 /**
 * @brief  This union represents the bit fields in the Component Type Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union ctr_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t peripheral_id              : 32;    /*!< This register contains the peripherals identification code. */
    } b;                                /*!< Register bits. */
} ctr_data_t;

/**
 * @brief  Registers associated with Uart.
 */
typedef struct uart_v100_regs {
    volatile uint32_t rbr_dll_thr;      /*!< Receive Buffer Register./
                                             Divisor Latch (Low) Register./
                                             Transmit Holding Register.  <i>Offset: 00H</i>. */
    volatile uint32_t dlh_ier;          /*!< Divisor Latch High (DLH) Register./
                                             Interrupt Enable Register.  <i>Offset: 04H</i>. */
    volatile uint32_t fcr_iir;          /*!< FIFO Control Register./
                                             Interrupt Identification Register.  <i>Offset: 08H</i>. */
    volatile uint32_t lcr;              /*!< Line Control Register.  <i>Offset: 0CH</i>. */
    volatile uint32_t mcr;              /*!< Modem Control Register.  <i>Offset: 10H</i>. */
    volatile uint32_t lsr;              /*!< Line Status Register.  <i>Offset: 14H</i>. */
    volatile uint32_t msr;              /*!< Modem Status Register.  <i>Offset: 18H</i>. */
    volatile uint32_t scr;              /*!< Scratchpad Register.  <i>Offset: 1CH</i>. */
    volatile uint32_t lpdll;            /*!< Low Power Divisor Latch Low Register.  <i>Offset: 20H</i>. */
    volatile uint32_t lpdlh;            /*!< Low Power Divisor Latch High Register.  <i>Offset: 24H</i>. */
    volatile uint32_t reseved2[2];      /*!< reseved.  <i>Offset: 28H</i>. */
    volatile uint32_t srbrn_sthrn[16];  /*!< Shadow Receive Buffer Register./
                                             Shadow Transmit Holding Register  <i>Offset: 30H</i>. */
    volatile uint32_t far;              /*!< FIFO Access Register.  <i>Offset: 70H</i>. */
    volatile uint32_t tfr;              /*!< Transmit FIFO Read Register .  <i>Offset: 74H</i>. */
    volatile uint32_t rfw;              /*!< Receive FIFO Write register.  <i>Offset: 78H</i>. */
    volatile uint32_t usr;              /*!< UART Status register.  <i>Offset: 7CH</i>. */
    volatile uint32_t tfl;              /*!<  Transmit FIFO Level register.  <i>Offset: 80H</i>. */
    volatile uint32_t rfl;              /*!< Receive FIFO Level register.  <i>Offset: 84H</i>. */
    volatile uint32_t srr;              /*!< Software Reset Register.  <i>Offset: 88H</i>. */
    volatile uint32_t srts;             /*!< Shadow Request to Send Register.  <i>Offset: 8CH</i>. */
    volatile uint32_t sbcr;             /*!< Shadow Break Control Register.  <i>Offset: 90H</i>. */
    volatile uint32_t sdmam;            /*!< Shadow DMA Mode Register.  <i>Offset: 94H</i>. */
    volatile uint32_t sfe;              /*!< Shadow FIFO Enable Register.  <i>Offset: 98H</i>. */
    volatile uint32_t srt;              /*!< Shadow RCVR Trigger Register.  <i>Offset: 9CH</i>. */
    volatile uint32_t stet;             /*!< Shadow TX Empty Trigger Register.  <i>Offset: A0H</i>. */
    volatile uint32_t htx;              /*!< Halt TX Register.  <i>Offset: A4H</i>. */
    volatile uint32_t dmasa;            /*!< DMA Software Acknowledge Register.  <i>Offset: A8H</i>. */
    volatile uint32_t tcr;              /*!< Transceiver Control Register.  <i>Offset: ACH</i>. */
    volatile uint32_t de_en;            /*!< Driver Output Enable Register.  <i>Offset: B0H</i>. */
    volatile uint32_t re_en;            /*!< Receiver Output Enable Register.  <i>Offset: B4H</i>. */
    volatile uint32_t det;              /*!< Driver Output Enable Timing Register.  <i>Offset: B8H</i>. */
    volatile uint32_t tat;              /*!< TurnAround Timing Register .  <i>Offset: BCH</i>. */
    volatile uint32_t dlf;              /*!< Divisor Latch Fraction Register .  <i>Offset: C0H</i>. */
    volatile uint32_t rar;              /*!< Receive Address Register.  <i>Offset: C4H</i>. */
    volatile uint32_t tar;              /*!< Transmit Address Register .  <i>Offset: C8H</i>. */
    volatile uint32_t lcr_ext;          /*!< Line Extended Control Register .  <i>Offset: CCH</i>. */
    volatile uint32_t reseved9[9];      /*!< reseved.  <i>Offset: D0H</i>. */
    volatile uint32_t cpr;              /*!< Component Parameter Register.  <i>Offset: F4H</i>. */
    volatile uint32_t ucv;              /*!< UART Component Version Register.  <i>Offset: F8H</i>. */
    volatile uint32_t ctr;              /*!< Component Type Register.  <i>Offset: FCH</i>. */
} uart_v100_regs_t;

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif