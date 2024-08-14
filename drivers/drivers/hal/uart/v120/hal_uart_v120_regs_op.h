/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides V120 uart register operation api \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-06-01， Create file. \n
 */
#ifndef HAL_UART_V120_REGS_OP_H
#define HAL_UART_V120_REGS_OP_H

#include <stdint.h>
#include "errcode.h"
#include "hal_uart_v120_regs_def.h"
#include "uart_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_uart_v120_regs_op UART V120 Regs Operation
 * @ingroup  drivers_hal_uart
 * @{
 */

/**
 * @brief  Definition of the UART interrupts.
 */
typedef enum uart_int {
    UART_INT_RIMI,              /*!< nUARTRI Modem Interrupt. */
    UART_INT_CTSMI,             /*!< nUARTCTS Modem Interrupt. */
    UART_INT_DCDMI,             /*!< nUARTDCD Modem Interrupt. */
    UART_INT_DSRMI,             /*!< nUARTDSR Modem Interrupt. */
    UART_INT_RXI,               /*!< Receive Interrupt. */
    UART_INT_TXI,               /*!< Transmit Interrupt. */
    UART_INT_RTI,               /*!< Receive Timeout Interrupt. */
    UART_INT_FRAME_ERR_I,       /*!< Framing Error Interrupt. */
    UART_INT_PARITY_ERR_I,      /*!< Parity Error Interrupt . */
    UART_INT_BREAK_ERR_I,       /*!< Break Error Interrupt. */
    UART_INT_OVERRUN_ERR_I,     /*!< Overrun Error Interrupt. */
    UART_INT_ALL                /*!< All of the Interrupt. */
} uart_int_t;

/**
 * @brief  The registers list about interrupt.
 */
typedef enum uart_int_reg {
    UARTIMSC,                   /*!< Interrupt mask set/clear register. */
    UARTRIS,                    /*!< Raw interrupt status register. */
    UARTMIS,                    /*!< Masked interrupt status register. */
    UARTICR                     /*!< Interrupt clear register. */
} uart_int_reg_t;

extern uintptr_t g_hal_uarts_regs[UART_BUS_MAX_NUM];

/**
 * @brief  Get the value of @ref uartdr_data.data.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @return The value of @ref uartdr_data.data.
 */
static inline uint32_t hal_uart_uartdr_get_data(uart_bus_t bus)
{
    uartdr_data_t uartdr;
    uartdr.d32 = ((uart_regs_t *)g_hal_uarts_regs[bus])->uartdr;
    return uartdr.b.data;
}

/**
 * @brief  Set the value of @ref uartdr_data.data.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @param  [in]  val The value of @ref uartdr_data.data
 */
static inline void hal_uart_uartdr_set_data(uart_bus_t bus, uint32_t val)
{
    uartdr_data_t uartdr;
    uartdr.b.data = val;
    ((uart_regs_t *)g_hal_uarts_regs[bus])->uartdr = uartdr.d32;
}

/**
 * @brief  Get the value of @ref uartdr_data.frame_error.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @return The value of @ref uartdr_data.frame_error.
 */
static inline uint32_t hal_uart_uartdr_get_frame_error(uart_bus_t bus)
{
    uartdr_data_t uartdr;
    uartdr.d32 = ((uart_regs_t *)g_hal_uarts_regs[bus])->uartdr;
    return uartdr.b.frame_error;
}

/**
 * @brief  Get the value of @ref uartdr_data.parity_error.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @return The value of @ref uartdr_data.parity_error.
 */
static inline uint32_t hal_uart_uartdr_get_parity_error(uart_bus_t bus)
{
    uartdr_data_t uartdr;
    uartdr.d32 = ((uart_regs_t *)g_hal_uarts_regs[bus])->uartdr;
    return uartdr.b.parity_error;
}

/**
 * @brief  Get the value of @ref uartdr_data.break_error.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @return The value of @ref uartdr_data.break_error.
 */
static inline uint32_t hal_uart_uartdr_get_break_error(uart_bus_t bus)
{
    uartdr_data_t uartdr;
    uartdr.d32 = ((uart_regs_t *)g_hal_uarts_regs[bus])->uartdr;
    return uartdr.b.break_error;
}

/**
 * @brief  Get the value of @ref uartdr_data.overrun_error.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @return The value of @ref uartdr_data.overrun_error.
 */
static inline uint32_t hal_uart_uartdr_get_overrun_error(uart_bus_t bus)
{
    uartdr_data_t uartdr;
    uartdr.d32 = ((uart_regs_t *)g_hal_uarts_regs[bus])->uartdr;
    return uartdr.b.overrun_error;
}

/**
 * @brief  Get the value of @ref uartrsr_data.frame_error.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @return The value of @ref uartrsr_data.frame_error.
 */
static inline uint32_t hal_uart_uartrsr_get_frame_error(uart_bus_t bus)
{
    uartrsr_data_t uartrsr;
    uartrsr.d32 = ((uart_regs_t *)g_hal_uarts_regs[bus])->uartrsr;
    return uartrsr.rb.frame_error;
}

/**
 * @brief  Get the value of @ref uartrsr_data.parity_error.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @return The value of @ref uartrsr_data.parity_error.
 */
static inline uint32_t hal_uart_uartrsr_get_parity_error(uart_bus_t bus)
{
    uartrsr_data_t uartrsr;
    uartrsr.d32 = ((uart_regs_t *)g_hal_uarts_regs[bus])->uartrsr;
    return uartrsr.rb.parity_error;
}

/**
 * @brief  Get the value of @ref uartrsr_data.break_error.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @return The value of @ref uartrsr_data.break_error.
 */
static inline uint32_t hal_uart_uartrsr_get_break_error(uart_bus_t bus)
{
    uartrsr_data_t uartrsr;
    uartrsr.d32 = ((uart_regs_t *)g_hal_uarts_regs[bus])->uartrsr;
    return uartrsr.rb.break_error;
}

/**
 * @brief  Get the value of @ref uartrsr_data.overrun_error.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @return The value of @ref uartrsr_data.overrun_error.
 */
static inline uint32_t hal_uart_uartrsr_get_overrun_error(uart_bus_t bus)
{
    uartrsr_data_t uartrsr;
    uartrsr.d32 = ((uart_regs_t *)g_hal_uarts_regs[bus])->uartrsr;
    return uartrsr.rb.overrun_error;
}

/**
 * @brief  Set the value of @ref uartrsr_data.wdata.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @param  [in]  val The value of @ref uartrsr_data.wdata
 */
static inline void hal_uart_uartrsr_set_wdata(uart_bus_t bus, uint32_t val)
{
    uartrsr_data_t uartrsr;
    uartrsr.d32 = ((uart_regs_t *)g_hal_uarts_regs[bus])->uartrsr;
    uartrsr.wb.wdata = val;
    ((uart_regs_t *)g_hal_uarts_regs[bus])->uartrsr = uartrsr.d32;
}

/**
 * @brief  Get the value of @ref uartfr_data.cts.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @return The value of @ref uartfr_data.cts.
 */
static inline uint32_t hal_uart_uartfr_get_cts(uart_bus_t bus)
{
    uartfr_data_t uartfr;
    uartfr.d32 = ((uart_regs_t *)g_hal_uarts_regs[bus])->uartfr;
    return uartfr.b.cts;
}

/**
 * @brief  Get the value of @ref uartfr_data.dsr.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @return The value of @ref uartfr_data.dsr.
 */
static inline uint32_t hal_uart_uartfr_get_dsr(uart_bus_t bus)
{
    uartfr_data_t uartfr;
    uartfr.d32 = ((uart_regs_t *)g_hal_uarts_regs[bus])->uartfr;
    return uartfr.b.dsr;
}

/**
 * @brief  Get the value of @ref uartfr_data.dcd.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @return The value of @ref uartfr_data.dcd.
 */
static inline uint32_t hal_uart_uartfr_get_dcd(uart_bus_t bus)
{
    uartfr_data_t uartfr;
    uartfr.d32 = ((uart_regs_t *)g_hal_uarts_regs[bus])->uartfr;
    return uartfr.b.dcd;
}

/**
 * @brief  Get the value of @ref uartfr_data.busy.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @return The value of @ref uartfr_data.busy.
 */
static inline uint32_t hal_uart_uartfr_get_busy(uart_bus_t bus)
{
    uartfr_data_t uartfr;
    uartfr.d32 = ((uart_regs_t *)g_hal_uarts_regs[bus])->uartfr;
    return uartfr.b.busy;
}

/**
 * @brief  Get the value of @ref uartfr_data.rxfe.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @return The value of @ref uartfr_data.rxfe.
 */
static inline uint32_t hal_uart_uartfr_get_rxfe(uart_bus_t bus)
{
    uartfr_data_t uartfr;
    uartfr.d32 = ((uart_regs_t *)g_hal_uarts_regs[bus])->uartfr;
    return uartfr.b.rxfe;
}

/**
 * @brief  Get the value of @ref uartfr_data.txff.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @return The value of @ref uartfr_data.txff.
 */
static inline uint32_t hal_uart_uartfr_get_txff(uart_bus_t bus)
{
    uartfr_data_t uartfr;
    uartfr.d32 = ((uart_regs_t *)g_hal_uarts_regs[bus])->uartfr;
    return uartfr.b.txff;
}

/**
 * @brief  Get the value of @ref uartfr_data.rxff.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @return The value of @ref uartfr_data.rxff.
 */
static inline uint32_t hal_uart_uartfr_get_rxff(uart_bus_t bus)
{
    uartfr_data_t uartfr;
    uartfr.d32 = ((uart_regs_t *)g_hal_uarts_regs[bus])->uartfr;
    return uartfr.b.rxff;
}

/**
 * @brief  Get the value of @ref uartfr_data.txfe.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @return The value of @ref uartfr_data.txfe.
 */
static inline uint32_t hal_uart_uartfr_get_txfe(uart_bus_t bus)
{
    uartfr_data_t uartfr;
    uartfr.d32 = ((uart_regs_t *)g_hal_uarts_regs[bus])->uartfr;
    return uartfr.b.txfe;
}

/**
 * @brief  Get the value of @ref uartfr_data.ri.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @return The value of @ref uartfr_data.ri.
 */
static inline uint32_t hal_uart_uartfr_get_ri(uart_bus_t bus)
{
    uartfr_data_t uartfr;
    uartfr.d32 = ((uart_regs_t *)g_hal_uarts_regs[bus])->uartfr;
    return uartfr.b.ri;
}

/**
 * @brief  Get the value of @ref ilpr_data.ilpdvsr.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @return The value of @ref ilpr_data.ilpdvsr.
 */
static inline uint32_t hal_uart_ilpr_get_ilpdvsr(uart_bus_t bus)
{
    ilpr_data_t ilpr;
    ilpr.d32 = ((uart_regs_t *)g_hal_uarts_regs[bus])->uartilpr;
    return ilpr.b.ilpdvsr;
}

/**
 * @brief  Set the value of @ref ilpr_data.ilpdvsr.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @param  [in]  val The value of @ref ilpr_data.ilpdvsr
 */
static inline void hal_uart_ilpr_set_ilpdvsr(uart_bus_t bus, uint32_t val)
{
    ilpr_data_t ilpr;
    ilpr.d32 = ((uart_regs_t *)g_hal_uarts_regs[bus])->uartilpr;
    ilpr.b.ilpdvsr = val;
    ((uart_regs_t *)g_hal_uarts_regs[bus])->uartilpr = ilpr.d32;
}

/**
 * @brief  Get the value of @ref uartibrd_data.baud_divint.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @return The value of @ref uartibrd_data.baud_divint.
 */
static inline uint32_t hal_uart_uartibrd_get_baud_divint(uart_bus_t bus)
{
    uartibrd_data_t uartibrd;
    uartibrd.d32 = ((uart_regs_t *)g_hal_uarts_regs[bus])->uartibrd;
    return uartibrd.b.baud_divint;
}

/**
 * @brief  Set the value of @ref uartibrd_data.baud_divint.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @param  [in]  val The value of @ref uartibrd_data.baud_divint
 */
static inline void hal_uart_uartibrd_set_baud_divint(uart_bus_t bus, uint32_t val)
{
    uartibrd_data_t uartibrd;
    uartibrd.d32 = ((uart_regs_t *)g_hal_uarts_regs[bus])->uartibrd;
    uartibrd.b.baud_divint = val;
    ((uart_regs_t *)g_hal_uarts_regs[bus])->uartibrd = uartibrd.d32;
}

/**
 * @brief  Get the value of @ref uartfbrd_data.baud_divfrac.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @return The value of @ref uartfbrd_data.baud_divfrac.
 */
static inline uint32_t hal_uart_uartfbrd_get_baud_divfrac(uart_bus_t bus)
{
    uartfbrd_data_t uartfbrd;
    uartfbrd.d32 = ((uart_regs_t *)g_hal_uarts_regs[bus])->uartfbrd;
    return uartfbrd.b.baud_divfrac;
}

/**
 * @brief  Set the value of @ref uartfbrd_data.baud_divfrac.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @param  [in]  val The value of @ref uartfbrd_data.baud_divfrac
 */
static inline void hal_uart_uartfbrd_set_baud_divfrac(uart_bus_t bus, uint32_t val)
{
    uartfbrd_data_t uartfbrd;
    uartfbrd.d32 = ((uart_regs_t *)g_hal_uarts_regs[bus])->uartfbrd;
    uartfbrd.b.baud_divfrac = val;
    ((uart_regs_t *)g_hal_uarts_regs[bus])->uartfbrd = uartfbrd.d32;
}

/**
 * @brief  Get the value of @ref uartlcr_h_data.
 * @param  [in]  bus The index of uart.
 * @return The value of @ref uartlcr_h_data.
 */
static inline uint32_t hal_uart_uartlcr_h_get(uart_bus_t bus)
{
    uartlcr_h_data_t uartlcr_h;
    uartlcr_h.d32 = ((uart_regs_t *)g_hal_uarts_regs[bus])->uartlcr_h;
    return uartlcr_h.d32;
}

/**
 * @brief  Set the value of @ref uartlcr_h_data.
 * @param  [in]  bus The index of uart.
 * @param  [in]  val The value of @ref uartlcr_h_data
 */
static inline void hal_uart_uartlcr_h_set(uart_bus_t bus, uint32_t val)
{
    uartlcr_h_data_t uartlcr_h;
    uartlcr_h.d32 = ((uart_regs_t *)g_hal_uarts_regs[bus])->uartlcr_h;
    uartlcr_h.d32 = val;
    ((uart_regs_t *)g_hal_uarts_regs[bus])->uartlcr_h = uartlcr_h.d32;
}

/**
 * @brief  Get the value of @ref uartlcr_h_data.brk.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @return The value of @ref uartlcr_h_data.brk.
 */
static inline uint32_t hal_uart_uartlcr_h_get_brk(uart_bus_t bus)
{
    uartlcr_h_data_t uartlcr_h;
    uartlcr_h.d32 = ((uart_regs_t *)g_hal_uarts_regs[bus])->uartlcr_h;
    return uartlcr_h.b.brk;
}

/**
 * @brief  Set the value of @ref uartlcr_h_data.brk.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @param  [in]  val The value of @ref uartlcr_h_data.brk
 */
static inline void hal_uart_uartlcr_h_set_brk(uart_bus_t bus, uint32_t val)
{
    uartlcr_h_data_t uartlcr_h;
    uartlcr_h.d32 = ((uart_regs_t *)g_hal_uarts_regs[bus])->uartlcr_h;
    uartlcr_h.b.brk = val;
    ((uart_regs_t *)g_hal_uarts_regs[bus])->uartlcr_h = uartlcr_h.d32;
}

/**
 * @brief  Get the value of @ref uartlcr_h_data.parity_en.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @return The value of @ref uartlcr_h_data.parity_en.
 */
static inline uint32_t hal_uart_uartlcr_h_get_parity_en(uart_bus_t bus)
{
    uartlcr_h_data_t uartlcr_h;
    uartlcr_h.d32 = ((uart_regs_t *)g_hal_uarts_regs[bus])->uartlcr_h;
    return uartlcr_h.b.parity_en;
}

/**
 * @brief  Set the value of @ref uartlcr_h_data.parity_en.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @param  [in]  val The value of @ref uartlcr_h_data.parity_en
 */
static inline void hal_uart_uartlcr_h_set_parity_en(uart_bus_t bus, uint32_t val)
{
    uartlcr_h_data_t uartlcr_h;
    uartlcr_h.d32 = ((uart_regs_t *)g_hal_uarts_regs[bus])->uartlcr_h;
    uartlcr_h.b.parity_en = val;
    ((uart_regs_t *)g_hal_uarts_regs[bus])->uartlcr_h = uartlcr_h.d32;
}

/**
 * @brief  Get the value of @ref uartlcr_h_data.eps.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @return The value of @ref uartlcr_h_data.eps.
 */
static inline uint32_t hal_uart_uartlcr_h_get_eps(uart_bus_t bus)
{
    uartlcr_h_data_t uartlcr_h;
    uartlcr_h.d32 = ((uart_regs_t *)g_hal_uarts_regs[bus])->uartlcr_h;
    return uartlcr_h.b.eps;
}

/**
 * @brief  Set the value of @ref uartlcr_h_data.eps.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @param  [in]  val The value of @ref uartlcr_h_data.eps
 */
static inline void hal_uart_uartlcr_h_set_eps(uart_bus_t bus, uint32_t val)
{
    uartlcr_h_data_t uartlcr_h;
    uartlcr_h.d32 = ((uart_regs_t *)g_hal_uarts_regs[bus])->uartlcr_h;
    uartlcr_h.b.eps = val;
    ((uart_regs_t *)g_hal_uarts_regs[bus])->uartlcr_h = uartlcr_h.d32;
}

/**
 * @brief  Get the value of @ref uartlcr_h_data.stp2.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @return The value of @ref uartlcr_h_data.stp2.
 */
static inline uint32_t hal_uart_uartlcr_h_get_stp2(uart_bus_t bus)
{
    uartlcr_h_data_t uartlcr_h;
    uartlcr_h.d32 = ((uart_regs_t *)g_hal_uarts_regs[bus])->uartlcr_h;
    return uartlcr_h.b.stp2;
}

/**
 * @brief  Set the value of @ref uartlcr_h_data.stp2.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @param  [in]  val The value of @ref uartlcr_h_data.stp2
 */
static inline void hal_uart_uartlcr_h_set_stp2(uart_bus_t bus, uint32_t val)
{
    uartlcr_h_data_t uartlcr_h;
    uartlcr_h.d32 = ((uart_regs_t *)g_hal_uarts_regs[bus])->uartlcr_h;
    uartlcr_h.b.stp2 = val;
    ((uart_regs_t *)g_hal_uarts_regs[bus])->uartlcr_h = uartlcr_h.d32;
}

/**
 * @brief  Get the value of @ref uartlcr_h_data.en_fifos.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @return The value of @ref uartlcr_h_data.en_fifos.
 */
static inline uint32_t hal_uart_uartlcr_h_get_en_fifos(uart_bus_t bus)
{
    uartlcr_h_data_t uartlcr_h;
    uartlcr_h.d32 = ((uart_regs_t *)g_hal_uarts_regs[bus])->uartlcr_h;
    return uartlcr_h.b.en_fifos;
}

/**
 * @brief  Set the value of @ref uartlcr_h_data.en_fifos.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @param  [in]  val The value of @ref uartlcr_h_data.en_fifos
 */
static inline void hal_uart_uartlcr_h_set_en_fifos(uart_bus_t bus, uint32_t val)
{
    uartlcr_h_data_t uartlcr_h;
    uartlcr_h.d32 = ((uart_regs_t *)g_hal_uarts_regs[bus])->uartlcr_h;
    uartlcr_h.b.en_fifos = val;
    ((uart_regs_t *)g_hal_uarts_regs[bus])->uartlcr_h = uartlcr_h.d32;
}

/**
 * @brief  Get the value of @ref uartlcr_h_data.wlen.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @return The value of @ref uartlcr_h_data.wlen.
 */
static inline uint32_t hal_uart_uartlcr_h_get_wlen(uart_bus_t bus)
{
    uartlcr_h_data_t uartlcr_h;
    uartlcr_h.d32 = ((uart_regs_t *)g_hal_uarts_regs[bus])->uartlcr_h;
    return uartlcr_h.b.wlen;
}

/**
 * @brief  Set the value of @ref uartlcr_h_data.wlen.
 * @param  [in]  bus The index of uart.
 * @param  [in]  val The value of @ref uartlcr_h_data.wlen
 */
static inline void hal_uart_uartlcr_h_set_wlen(uart_bus_t bus, uint32_t val)
{
    uartlcr_h_data_t uartlcr_h;
    uartlcr_h.d32 = ((uart_regs_t *)g_hal_uarts_regs[bus])->uartlcr_h;
    uartlcr_h.b.wlen = val;
    ((uart_regs_t *)g_hal_uarts_regs[bus])->uartlcr_h = uartlcr_h.d32;
}

/**
 * @brief  Get the value of @ref uartlcr_h_data.sps.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @return The value of @ref uartlcr_h_data.sps.
 */
static inline uint32_t hal_uart_uartlcr_h_get_sps(uart_bus_t bus)
{
    uartlcr_h_data_t uartlcr_h;
    uartlcr_h.d32 = ((uart_regs_t *)g_hal_uarts_regs[bus])->uartlcr_h;
    return uartlcr_h.b.sps;
}

/**
 * @brief  Set the value of @ref uartlcr_h_data.sps.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @param  [in]  val The value of @ref uartlcr_h_data.sps
 */
static inline void hal_uart_uartlcr_h_set_sps(uart_bus_t bus, uint32_t val)
{
    uartlcr_h_data_t uartlcr_h;
    uartlcr_h.d32 = ((uart_regs_t *)g_hal_uarts_regs[bus])->uartlcr_h;
    uartlcr_h.b.sps = val;
    ((uart_regs_t *)g_hal_uarts_regs[bus])->uartlcr_h = uartlcr_h.d32;
}

/**
 * @brief  Get the value of @ref uartcr_data.
 * @param  [in]  bus The index of uart.
 * @return The value of @ref uartcr_data.
 */
static inline uint32_t hal_uart_uartcr_get(uart_bus_t bus)
{
    uartcr_data_t uartcr;
    uartcr.d32 = ((uart_regs_t *)g_hal_uarts_regs[bus])->uartcr;
    return uartcr.d32;
}

/**
 * @brief  Set the value of @ref uartcr_data.
 * @param  [in]  bus The index of uart.
 * @param  [in]  val The value of @ref uartcr_data
 */
static inline void hal_uart_uartcr_set(uart_bus_t bus, uint32_t val)
{
    uartcr_data_t uartcr;
    uartcr.d32 = ((uart_regs_t *)g_hal_uarts_regs[bus])->uartcr;
    uartcr.d32 = val;
    ((uart_regs_t *)g_hal_uarts_regs[bus])->uartcr = uartcr.d32;
}

/**
 * @brief  Get the value of @ref uartcr_data.uarten.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @return The value of @ref uartcr_data.uarten.
 */
static inline uint32_t hal_uart_uartcr_get_uarten(uart_bus_t bus)
{
    uartcr_data_t uartcr;
    uartcr.d32 = ((uart_regs_t *)g_hal_uarts_regs[bus])->uartcr;
    return uartcr.b.uarten;
}

/**
 * @brief  Set the value of @ref uartcr_data.uarten.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @param  [in]  val The value of @ref uartcr_data.uarten
 */
static inline void hal_uart_uartcr_set_uarten(uart_bus_t bus, uint32_t val)
{
    uartcr_data_t uartcr;
    uartcr.d32 = ((uart_regs_t *)g_hal_uarts_regs[bus])->uartcr;
    uartcr.b.uarten = val;
    ((uart_regs_t *)g_hal_uarts_regs[bus])->uartcr = uartcr.d32;
}

/**
 * @brief  Get the value of @ref uartcr_data.siren.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @return The value of @ref uartcr_data.siren.
 */
static inline uint32_t hal_uart_uartcr_get_siren(uart_bus_t bus)
{
    uartcr_data_t uartcr;
    uartcr.d32 = ((uart_regs_t *)g_hal_uarts_regs[bus])->uartcr;
    return uartcr.b.siren;
}

/**
 * @brief  Set the value of @ref uartcr_data.siren.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @param  [in]  val The value of @ref uartcr_data.siren
 */
static inline void hal_uart_uartcr_set_siren(uart_bus_t bus, uint32_t val)
{
    uartcr_data_t uartcr;
    uartcr.d32 = ((uart_regs_t *)g_hal_uarts_regs[bus])->uartcr;
    uartcr.b.siren = val;
    ((uart_regs_t *)g_hal_uarts_regs[bus])->uartcr = uartcr.d32;
}

/**
 * @brief  Get the value of @ref uartcr_data.sirlp.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @return The value of @ref uartcr_data.sirlp.
 */
static inline uint32_t hal_uart_uartcr_get_sirlp(uart_bus_t bus)
{
    uartcr_data_t uartcr;
    uartcr.d32 = ((uart_regs_t *)g_hal_uarts_regs[bus])->uartcr;
    return uartcr.b.sirlp;
}

/**
 * @brief  Set the value of @ref uartcr_data.sirlp.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @param  [in]  val The value of @ref uartcr_data.sirlp
 */
static inline void hal_uart_uartcr_set_sirlp(uart_bus_t bus, uint32_t val)
{
    uartcr_data_t uartcr;
    uartcr.d32 = ((uart_regs_t *)g_hal_uarts_regs[bus])->uartcr;
    uartcr.b.sirlp = val;
    ((uart_regs_t *)g_hal_uarts_regs[bus])->uartcr = uartcr.d32;
}

/**
 * @brief  Get the value of @ref uartcr_data.lbe.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @return The value of @ref uartcr_data.lbe.
 */
static inline uint32_t hal_uart_uartcr_get_lbe(uart_bus_t bus)
{
    uartcr_data_t uartcr;
    uartcr.d32 = ((uart_regs_t *)g_hal_uarts_regs[bus])->uartcr;
    return uartcr.b.lbe;
}

/**
 * @brief  Set the value of @ref uartcr_data.lbe.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @param  [in]  val The value of @ref uartcr_data.lbe
 */
static inline void hal_uart_uartcr_set_lbe(uart_bus_t bus, uint32_t val)
{
    uartcr_data_t uartcr;
    uartcr.d32 = ((uart_regs_t *)g_hal_uarts_regs[bus])->uartcr;
    uartcr.b.lbe = val;
    ((uart_regs_t *)g_hal_uarts_regs[bus])->uartcr = uartcr.d32;
}

/**
 * @brief  Get the value of @ref uartcr_data.txe.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @return The value of @ref uartcr_data.txe.
 */
static inline uint32_t hal_uart_uartcr_get_txe(uart_bus_t bus)
{
    uartcr_data_t uartcr;
    uartcr.d32 = ((uart_regs_t *)g_hal_uarts_regs[bus])->uartcr;
    return uartcr.b.txe;
}

/**
 * @brief  Set the value of @ref uartcr_data.txe.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @param  [in]  val The value of @ref uartcr_data.txe
 */
static inline void hal_uart_uartcr_set_txe(uart_bus_t bus, uint32_t val)
{
    uartcr_data_t uartcr;
    uartcr.d32 = ((uart_regs_t *)g_hal_uarts_regs[bus])->uartcr;
    uartcr.b.txe = val;
    ((uart_regs_t *)g_hal_uarts_regs[bus])->uartcr = uartcr.d32;
}

/**
 * @brief  Get the value of @ref uartcr_data.rxe.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @return The value of @ref uartcr_data.rxe.
 */
static inline uint32_t hal_uart_uartcr_get_rxe(uart_bus_t bus)
{
    uartcr_data_t uartcr;
    uartcr.d32 = ((uart_regs_t *)g_hal_uarts_regs[bus])->uartcr;
    return uartcr.b.rxe;
}

/**
 * @brief  Set the value of @ref uartcr_data.rxe.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @param  [in]  val The value of @ref uartcr_data.rxe
 */
static inline void hal_uart_uartcr_set_rxe(uart_bus_t bus, uint32_t val)
{
    uartcr_data_t uartcr;
    uartcr.d32 = ((uart_regs_t *)g_hal_uarts_regs[bus])->uartcr;
    uartcr.b.rxe = val;
    ((uart_regs_t *)g_hal_uarts_regs[bus])->uartcr = uartcr.d32;
}

/**
 * @brief  Get the value of @ref uartcr_data.dtr.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @return The value of @ref uartcr_data.dtr.
 */
static inline uint32_t hal_uart_uartcr_get_dtr(uart_bus_t bus)
{
    uartcr_data_t uartcr;
    uartcr.d32 = ((uart_regs_t *)g_hal_uarts_regs[bus])->uartcr;
    return uartcr.b.dtr;
}

/**
 * @brief  Set the value of @ref uartcr_data.dtr.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @param  [in]  val The value of @ref uartcr_data.dtr
 */
static inline void hal_uart_uartcr_set_dtr(uart_bus_t bus, uint32_t val)
{
    uartcr_data_t uartcr;
    uartcr.d32 = ((uart_regs_t *)g_hal_uarts_regs[bus])->uartcr;
    uartcr.b.dtr = val;
    ((uart_regs_t *)g_hal_uarts_regs[bus])->uartcr = uartcr.d32;
}

/**
 * @brief  Get the value of @ref uartcr_data.rst.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @return The value of @ref uartcr_data.rst.
 */
static inline uint32_t hal_uart_uartcr_get_rst(uart_bus_t bus)
{
    uartcr_data_t uartcr;
    uartcr.d32 = ((uart_regs_t *)g_hal_uarts_regs[bus])->uartcr;
    return uartcr.b.rst;
}

/**
 * @brief  Set the value of @ref uartcr_data.rst.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @param  [in]  val The value of @ref uartcr_data.rst
 */
static inline void hal_uart_uartcr_set_rst(uart_bus_t bus, uint32_t val)
{
    uartcr_data_t uartcr;
    uartcr.d32 = ((uart_regs_t *)g_hal_uarts_regs[bus])->uartcr;
    uartcr.b.rst = val;
    ((uart_regs_t *)g_hal_uarts_regs[bus])->uartcr = uartcr.d32;
}

/**
 * @brief  Get the value of @ref uartcr_data.out1.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @return The value of @ref uartcr_data.out1.
 */
static inline uint32_t hal_uart_uartcr_get_out1(uart_bus_t bus)
{
    uartcr_data_t uartcr;
    uartcr.d32 = ((uart_regs_t *)g_hal_uarts_regs[bus])->uartcr;
    return uartcr.b.out1;
}

/**
 * @brief  Set the value of @ref uartcr_data.out1.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @param  [in]  val The value of @ref uartcr_data.out1
 */
static inline void hal_uart_uartcr_set_out1(uart_bus_t bus, uint32_t val)
{
    uartcr_data_t uartcr;
    uartcr.d32 = ((uart_regs_t *)g_hal_uarts_regs[bus])->uartcr;
    uartcr.b.out1 = val;
    ((uart_regs_t *)g_hal_uarts_regs[bus])->uartcr = uartcr.d32;
}

/**
 * @brief  Get the value of @ref uartcr_data.out2.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @return The value of @ref uartcr_data.out2.
 */
static inline uint32_t hal_uart_uartcr_get_out2(uart_bus_t bus)
{
    uartcr_data_t uartcr;
    uartcr.d32 = ((uart_regs_t *)g_hal_uarts_regs[bus])->uartcr;
    return uartcr.b.out2;
}

/**
 * @brief  Set the value of @ref uartcr_data.out2.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @param  [in]  val The value of @ref uartcr_data.out2
 */
static inline void hal_uart_uartcr_set_out2(uart_bus_t bus, uint32_t val)
{
    uartcr_data_t uartcr;
    uartcr.d32 = ((uart_regs_t *)g_hal_uarts_regs[bus])->uartcr;
    uartcr.b.out2 = val;
    ((uart_regs_t *)g_hal_uarts_regs[bus])->uartcr = uartcr.d32;
}

/**
 * @brief  Get the value of @ref uartcr_data.rts_en.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @return The value of @ref uartcr_data.rts_en.
 */
static inline uint32_t hal_uart_uartcr_get_rts_en(uart_bus_t bus)
{
    uartcr_data_t uartcr;
    uartcr.d32 = ((uart_regs_t *)g_hal_uarts_regs[bus])->uartcr;
    return uartcr.b.rts_en;
}

/**
 * @brief  Set the value of @ref uartcr_data.rts_en.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @param  [in]  val The value of @ref uartcr_data.rts_en
 */
static inline void hal_uart_uartcr_set_rts_en(uart_bus_t bus, uint32_t val)
{
    uartcr_data_t uartcr;
    uartcr.d32 = ((uart_regs_t *)g_hal_uarts_regs[bus])->uartcr;
    uartcr.b.rts_en = val;
    ((uart_regs_t *)g_hal_uarts_regs[bus])->uartcr = uartcr.d32;
}

/**
 * @brief  Get the value of @ref uartcr_data.cts_en.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @return The value of @ref uartcr_data.cts_en.
 */
static inline uint32_t hal_uart_uartcr_get_cts_en(uart_bus_t bus)
{
    uartcr_data_t uartcr;
    uartcr.d32 = ((uart_regs_t *)g_hal_uarts_regs[bus])->uartcr;
    return uartcr.b.cts_en;
}

/**
 * @brief  Set the value of @ref uartcr_data.cts_en.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @param  [in]  val The value of @ref uartcr_data.cts_en
 */
static inline void hal_uart_uartcr_set_cts_en(uart_bus_t bus, uint32_t val)
{
    uartcr_data_t uartcr;
    uartcr.d32 = ((uart_regs_t *)g_hal_uarts_regs[bus])->uartcr;
    uartcr.b.cts_en = val;
    ((uart_regs_t *)g_hal_uarts_regs[bus])->uartcr = uartcr.d32;
}

/**
 * @brief  Get the value of @ref uartifls_data.txiflsel.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @return The value of @ref uartifls_data.txiflsel.
 */
static inline uint32_t hal_uart_uartifls_get_txiflsel(uart_bus_t bus)
{
    uartifls_data_t uartifls;
    uartifls.d32 = ((uart_regs_t *)g_hal_uarts_regs[bus])->uartifls;
    return uartifls.b.txiflsel;
}

/**
 * @brief  Set the value of @ref uartifls_data.txiflsel.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @param  [in]  val The value of @ref uartifls_data.txiflsel
 */
static inline void hal_uart_uartifls_set_txiflsel(uart_bus_t bus, uint32_t val)
{
    uartifls_data_t uartifls;
    uartifls.d32 = ((uart_regs_t *)g_hal_uarts_regs[bus])->uartifls;
    uartifls.b.txiflsel = val;
    ((uart_regs_t *)g_hal_uarts_regs[bus])->uartifls = uartifls.d32;
}

/**
 * @brief  Get the value of @ref uartifls_data.rxiflsel.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @return The value of @ref uartifls_data.rxiflsel.
 */
static inline uint32_t hal_uart_uartifls_get_rxiflsel(uart_bus_t bus)
{
    uartifls_data_t uartifls;
    uartifls.d32 = ((uart_regs_t *)g_hal_uarts_regs[bus])->uartifls;
    return uartifls.b.rxiflsel;
}

/**
 * @brief  Set the value of @ref uartifls_data.rxiflsel.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @param  [in]  val The value of @ref uartifls_data.rxiflsel
 */
static inline void hal_uart_uartifls_set_rxiflsel(uart_bus_t bus, uint32_t val)
{
    uartifls_data_t uartifls;
    uartifls.d32 = ((uart_regs_t *)g_hal_uarts_regs[bus])->uartifls;
    uartifls.b.rxiflsel = val;
    ((uart_regs_t *)g_hal_uarts_regs[bus])->uartifls = uartifls.d32;
}

/**
 * @brief  Get the value of specified intterupt. @ref uarti_data
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @param  [in]  reg The register wants to set. @ref uart_int_reg_t
 * @param  [in]  int_id The interrpt want to read. @ref uart_int_t
 * @return he value of specified intterupt.  @ref uarti_data
 */
uint32_t hal_uart_int_get(uart_bus_t bus, uart_int_reg_t reg, uart_int_t int_id);

/**
 * @brief  Get the value of specified intterupt. @ref uarti_data
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @param  [in]  reg The register wants to set. @ref uart_int_reg_t
 * @param  [in]  int_id The interrpt want to read. @ref uart_int_t
 * @param  [in]  val The value of specified intterupt. @ref uarti_data
 */
void hal_uart_int_set(uart_bus_t bus, uart_int_reg_t reg, uart_int_t int_id,  uint32_t val);

/**
 * @brief  Get the value of @ref uartdmacr_data.rxdmaen.
 * @param  [in]  bus The index of uart.
 * @return The value of @ref uartdmacr_data.rxdmaen.
 */
static inline uint32_t hal_uart_uartdmacr_get_rxdmaen(uart_bus_t bus)
{
    uartdmacr_data_t uartdmacr;
    uartdmacr.d32 = ((uart_regs_t *)g_hal_uarts_regs[bus])->uartdmacr;
    return uartdmacr.b.rxdmaen;
}

/**
 * @brief  Set the value of @ref uartdmacr_data.rxdmaen.
 * @param  [in]  bus The index of uart.
 * @param  [in]  val The value of @ref uartdmacr_data.rxdmaen
 */
static inline void hal_uart_uartdmacr_set_rxdmaen(uart_bus_t bus, uint32_t val)
{
    uartdmacr_data_t uartdmacr;
    uartdmacr.d32 = ((uart_regs_t *)g_hal_uarts_regs[bus])->uartdmacr;
    uartdmacr.b.rxdmaen = val;
    ((uart_regs_t *)g_hal_uarts_regs[bus])->uartdmacr = uartdmacr.d32;
}

/**
 * @brief  Get the value of @ref uartdmacr_data.txdmaen.
 * @param  [in]  bus The index of uart.
 * @return The value of @ref uartdmacr_data.txdmaen.
 */
static inline uint32_t hal_uart_uartdmacr_get_txdmaen(uart_bus_t bus)
{
    uartdmacr_data_t uartdmacr;
    uartdmacr.d32 = ((uart_regs_t *)g_hal_uarts_regs[bus])->uartdmacr;
    return uartdmacr.b.txdmaen;
}

/**
 * @brief  Set the value of @ref uartdmacr_data.txdmaen.
 * @param  [in]  bus The index of uart.
 * @param  [in]  val The value of @ref uartdmacr_data.txdmaen
 */
static inline void hal_uart_uartdmacr_set_txdmaen(uart_bus_t bus, uint32_t val)
{
    uartdmacr_data_t uartdmacr;
    uartdmacr.d32 = ((uart_regs_t *)g_hal_uarts_regs[bus])->uartdmacr;
    uartdmacr.b.txdmaen = val;
    ((uart_regs_t *)g_hal_uarts_regs[bus])->uartdmacr = uartdmacr.d32;
}

/**
 * @brief  Get the value of @ref uartdmacr_data.dmaonerr.
 * @param  [in]  bus The index of uart.
 * @return The value of @ref uartdmacr_data.dmaonerr.
 */
static inline uint32_t hal_uart_uartdmacr_get_dmaonerr(uart_bus_t bus)
{
    uartdmacr_data_t uartdmacr;
    uartdmacr.d32 = ((uart_regs_t *)g_hal_uarts_regs[bus])->uartdmacr;
    return uartdmacr.b.dmaonerr;
}

/**
 * @brief  Set the value of @ref uartdmacr_data.dmaonerr.
 * @param  [in]  bus The index of uart.
 * @param  [in]  val The value of @ref uartdmacr_data.dmaonerr
 */
static inline void hal_uart_uartdmacr_set_dmaonerr(uart_bus_t bus, uint32_t val)
{
    uartdmacr_data_t uartdmacr;
    uartdmacr.d32 = ((uart_regs_t *)g_hal_uarts_regs[bus])->uartdmacr;
    uartdmacr.b.dmaonerr = val;
    ((uart_regs_t *)g_hal_uarts_regs[bus])->uartdmacr = uartdmacr.d32;
}

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif