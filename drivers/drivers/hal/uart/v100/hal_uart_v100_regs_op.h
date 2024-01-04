/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides V100 uart register operation api \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-11-03， Create file. \n
 */
#ifndef HAL_UART_V100_REGS_OP_H
#define HAL_UART_V100_REGS_OP_H

#include <stdint.h>
#include "errcode.h"
#include "uart_porting.h"
#include "hal_uart_v100_regs_def.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_uart_v100_regs_op UART V100 Regs Operation
 * @ingroup  drivers_hal_uart
 * @{
 */

extern uintptr_t g_hal_uarts_regs[UART_BUS_MAX_NUM];

/**
 * @brief  Get the value of @ref rbr_dll_thr_data.dll.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @return The value of @ref rbr_dll_thr_data.dll.
 */
static inline uint32_t hal_uart_rbr_dll_thr_get_dll(uart_bus_t bus)
{
    rbr_dll_thr_data_t dll;
    dll.d32 = ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->rbr_dll_thr;
    return dll.dll.dll;
}

/**
 * @brief  Set the value of @ref rbr_dll_thr_data.dll.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @param  [in]  val The value of @ref rbr_dll_thr_data.dll.
 */
static inline void hal_uart_rbr_dll_thr_set_dll(uart_bus_t bus, uint32_t val)
{
    rbr_dll_thr_data_t dll;
    dll.dll.dll = val;
    ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->rbr_dll_thr = dll.d32;
}

/**
 * @brief  Get the value of @ref rbr_dll_thr_data.thr.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @return The value of @ref rbr_dll_thr_data.thr.
 */
static inline uint32_t hal_uart_rbr_dll_thr_get_thr(uart_bus_t bus)
{
    rbr_dll_thr_data_t thr;
    thr.d32 = ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->rbr_dll_thr;
    return thr.thr.thr;
}

/**
 * @brief  Set the value of @ref rbr_dll_thr_data.thr.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @param  [in]  val The value of @ref rbr_dll_thr_data.thr.
 */
static inline void hal_uart_rbr_dll_thr_set_thr(uart_bus_t bus, uint32_t val)
{
    rbr_dll_thr_data_t thr;
    thr.thr.thr = val;
    ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->rbr_dll_thr = thr.d32;
}

/**
 * @brief  Get the value of @ref rbr_dll_thr_data.rbr.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @return The value of @ref rbr_dll_thr_data.rbr.
 */
static inline uint8_t hal_uart_rbr_dll_thr_get_rbr(uart_bus_t bus)
{
    rbr_dll_thr_data_t rbr;
    rbr.d32 = ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->rbr_dll_thr;
    return (uint8_t)rbr.rbr.rbr;
}

/**
 * @brief  Set the value of @ref rbr_dll_thr_data.rbr.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @param  [in]  val The value of @ref rbr_dll_thr_data.rbr.
 */
static inline void hal_uart_rbr_dll_thr_set_rbr(uart_bus_t bus, uint32_t val)
{
    rbr_dll_thr_data_t rbr;
    rbr.rbr.rbr = val;
    ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->rbr_dll_thr = rbr.d32;
}

/**
 * @brief  Get the value of @ref dlh_ier_data.dlh.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @return The value of @ref dlh_ier_data.dlh.
 */
static inline uint32_t hal_uart_dlh_ier_get_dlh(uart_bus_t bus)
{
    dlh_ier_data_t dlh;
    dlh.d32 = ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->dlh_ier;
    return dlh.dlh.dlh;
}

/**
 * @brief  Set the value of @ref dlh_ier_data.dlh.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @param  [in]  val The value of @ref dlh_ier_data.dlh.
 */
static inline void hal_uart_dlh_ier_set_dlh(uart_bus_t bus, uint32_t val)
{
    dlh_ier_data_t dlh;
    dlh.d32 = ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->dlh_ier;
    dlh.dlh.dlh = val;
    ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->dlh_ier = dlh.d32;
}

/**
 * @brief  Get the value of @ref fcr_iir_data.rt.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @return The value of @ref fcr_iir_data.rt.
 */
static inline uint32_t hal_uart_fcr_iir_get_rt(uart_bus_t bus)
{
    fcr_iir_data_t fcr;
    fcr.d32 = ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->fcr_iir;
    return fcr.fcr.rt;
}

/**
 * @brief  Set the value of @ref fcr_iir_data.rt.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @param  [in]  val The value of @ref fcr_iir_data.rt.
 */
static inline void hal_uart_fcr_iir_set_rt(uart_bus_t bus, uint32_t val)
{
    fcr_iir_data_t fcr;
    fcr.d32 = ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->fcr_iir;
    fcr.fcr.rt = val;
    ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->fcr_iir = fcr.d32;
}

/**
 * @brief  Get the value of @ref fcr_iir_data.tet.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @return The value of @ref fcr_iir_data.tet.
 */
static inline uint32_t hal_uart_fcr_iir_get_tet(uart_bus_t bus)
{
    fcr_iir_data_t fcr;
    fcr.d32 = ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->fcr_iir;
    return fcr.fcr.tet;
}

/**
 * @brief  Set the value of @ref fcr_iir_data.tet.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @param  [in]  val The value of @ref fcr_iir_data.tet.
 */
static inline void hal_uart_fcr_iir_set_tet(uart_bus_t bus, uint32_t val)
{
    fcr_iir_data_t fcr;
    fcr.d32 = ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->fcr_iir;
    fcr.fcr.tet = val;
    ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->fcr_iir = fcr.d32;
}

/**
 * @brief  Get the value of @ref fcr_iir_data.fifoe.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @return The value of @ref fcr_iir_data.fifoe.
 */
static inline uint32_t hal_uart_fcr_iir_get_fifoe(uart_bus_t bus)
{
    fcr_iir_data_t fcr;
    fcr.d32 = ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->fcr_iir;
    return fcr.fcr.fifoe;
}

/**
 * @brief  Set the value of @ref fcr_iir_data.fifoe.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @param  [in]  val The value of @ref fcr_iir_data.fifoe.
 */
static inline void hal_uart_fcr_iir_set_fifoe(uart_bus_t bus, uint32_t val)
{
    fcr_iir_data_t fcr;
    fcr.d32 = ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->fcr_iir;
    fcr.fcr.fifoe = val;
    ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->fcr_iir = fcr.d32;
}

/**
 * @brief  Get the value of @ref fcr_iir_data.iid.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @return The value of @ref fcr_iir_data.iid.
 */
static inline uint32_t hal_uart_fcr_iir_get_iid(uart_bus_t bus)
{
    fcr_iir_data_t iir;
    iir.d32 = ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->fcr_iir;
    return iir.iir.iid;
}

/**
 * @brief  Set the value of @ref fcr_iir_data.iid.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @param  [in]  val The value of @ref fcr_iir_data.iid.
 */
static inline void hal_uart_fcr_iir_set_iid(uart_bus_t bus, uint32_t val)
{
    fcr_iir_data_t iir;
    iir.d32 = ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->fcr_iir;
    iir.iir.iid = val;
    ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->fcr_iir = iir.d32;
}

/**
 * @brief  Get the value of @ref lcr_data.dlab.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @return The value of @ref lcr_data.dlab.
 */
static inline uint32_t hal_uart_lcr_get_dlab(uart_bus_t bus)
{
    lcr_data_t lcr;
    lcr.d32 = ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->lcr;
    return lcr.b.dlab;
}

/**
 * @brief  Set the value of @ref lcr_data.dlab.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @param  [in]  val The value of @ref lcr_data.dlab.
 */
static inline void hal_uart_lcr_set_dlab(uart_bus_t bus, uint32_t val)
{
    lcr_data_t lcr;
    lcr.d32 = ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->lcr;
    lcr.b.dlab = val;
    ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->lcr = lcr.d32;
}

/**
 * @brief  Get the value of @ref lcr_data.dls.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @return The value of @ref lcr_data.dls.
 */
static inline uint32_t hal_uart_lcr_get_dls(uart_bus_t bus)
{
    lcr_data_t lcr;
    lcr.d32 = ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->lcr;
    return lcr.b.dls;
}

/**
 * @brief  Set the value of @ref lcr_data.dls.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @param  [in]  val The value of @ref lcr_data.dls.
 */
static inline void hal_uart_lcr_set_dls(uart_bus_t bus, uint32_t val)
{
    lcr_data_t lcr;
    lcr.d32 = ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->lcr;
    lcr.b.dls = val;
    ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->lcr = lcr.d32;
}

/**
 * @brief  Get the value of @ref lcr_data.pen.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @return The value of @ref lcr_data.pen.
 */
static inline uint32_t hal_uart_lcr_get_pen(uart_bus_t bus)
{
    lcr_data_t lcr;
    lcr.d32 = ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->lcr;
    return lcr.b.pen;
}

/**
 * @brief  Set the value of @ref lcr_data.pen.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @param  [in]  val The value of @ref lcr_data.pen.
 */
static inline void hal_uart_lcr_set_pen(uart_bus_t bus, uint32_t val)
{
    lcr_data_t lcr;
    lcr.d32 = ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->lcr;
    lcr.b.pen = val;
    ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->lcr = lcr.d32;
}

/**
 * @brief  Get the value of @ref lcr_data.eps.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @return The value of @ref lcr_data.eps.
 */
static inline uint32_t hal_uart_lcr_get_eps(uart_bus_t bus)
{
    lcr_data_t lcr;
    lcr.d32 = ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->lcr;
    return lcr.b.eps;
}

/**
 * @brief  Set the value of @ref lcr_data.eps.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @param  [in]  val The value of @ref lcr_data.eps.
 */
static inline void hal_uart_lcr_set_eps(uart_bus_t bus, uint32_t val)
{
    lcr_data_t lcr;
    lcr.d32 = ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->lcr;
    lcr.b.eps = val;
    ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->lcr = lcr.d32;
}

/**
 * @brief  Get the value of @ref lcr_data.stop.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @return The value of @ref lcr_data.stop.
 */
static inline uint32_t hal_uart_lcr_get_stop(uart_bus_t bus)
{
    lcr_data_t lcr;
    lcr.d32 = ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->lcr;
    return lcr.b.stop;
}

/**
 * @brief  Set the value of @ref lcr_data.stop.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @param  [in]  val The value of @ref lcr_data.stop.
 */
static inline void hal_uart_lcr_set_stop(uart_bus_t bus, uint32_t val)
{
    lcr_data_t lcr;
    lcr.d32 = ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->lcr;
    lcr.b.stop = val;
    ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->lcr = lcr.d32;
}

/**
 * @brief  Get the value of @ref dlf_data.dlf.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @return The value of @ref dlf_data.dlf.
 */
static inline uint32_t hal_uart_dlf_get_dlf(uart_bus_t bus)
{
    dlf_data_t dlf;
    dlf.d32 = ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->dlf;
    return dlf.b.dlf;
}

/**
 * @brief  Set the value of @ref dlf_data.dlf.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @param  [in]  val The value of @ref dlf_data.dlf.
 */
static inline void hal_uart_dlf_set_dlf(uart_bus_t bus, uint32_t val)
{
    dlf_data_t dlf;
    dlf.d32 = ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->dlf;
    dlf.b.dlf = val;
    ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->dlf = dlf.d32;
}

/**
 * @brief  Get the value of @ref cpr_data.shadow.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @return The value of @ref cpr_data.shadow.
 */
static inline uint32_t hal_uart_cpr_get_shadow(uart_bus_t bus)
{
    cpr_data_t cpr;
    cpr.d32 = ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->cpr;
    return cpr.b.shadow;
}

/**
 * @brief  Set the value of @ref cpr_data.shadow.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @param  [in]  val The value of @ref cpr_data.shadow.
 */
static inline void hal_uart_cpr_set_shadow(uart_bus_t bus, uint32_t val)
{
    cpr_data_t cpr;
    cpr.d32 = ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->cpr;
    cpr.b.shadow = val;
    ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->cpr = cpr.d32;
}

/**
 * @brief  Get the value of @ref cpr_data.fifo_mode.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @return The value of @ref cpr_data.fifo_mode.
 */
static inline uint32_t hal_uart_cpr_get_fifo_mode(uart_bus_t bus)
{
    cpr_data_t cpr;
    cpr.d32 = ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->cpr;
    return cpr.b.fifo_mode;
}

/**
 * @brief  Set the value of @ref cpr_data.fifo_mode.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @param  [in]  val The value of @ref cpr_data.fifo_mode.
 */
static inline void hal_uart_cpr_set_fifo_mode(uart_bus_t bus, uint32_t val)
{
    cpr_data_t cpr;
    cpr.d32 = ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->cpr;
    cpr.b.fifo_mode = val;
    ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->cpr = cpr.d32;
}

/**
 * @brief  Get the value of @ref srr_data.ur.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @return The value of @ref srr_data.ur.
 */
static inline uint32_t hal_uart_srr_get_ur(uart_bus_t bus)
{
    srr_data_t srr;
    srr.d32 = ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->srr;
    return srr.b.ur;
}

/**
 * @brief  Set the value of @ref srr_data.ur.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @param  [in]  val The value of @ref srr_data.ur.
 */
static inline void hal_uart_ssr_uart_reset(uart_bus_t bus, uint32_t val)
{
    srr_data_t srr;
    srr.d32 = ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->srr;
    srr.b.ur = val;
    ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->srr = srr.d32;
}

/**
 * @brief  Get the value of @ref dlh_ier_data.etbei.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @return The value of @ref dlh_ier_data.etbei.
 */
static inline uint32_t hal_uart_dlh_ier_get_etbei(uart_bus_t bus)
{
    dlh_ier_data_t dlh_ier;
    dlh_ier.d32 = ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->dlh_ier;
    return dlh_ier.ier.etbei;
}

/**
 * @brief  Set the value of @ref dlh_ier_data.etbei.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @param  [in]  val The value of @ref dlh_ier_data.etbei.
 */
static inline void hal_uart_dlh_ier_set_etbei(uart_bus_t bus, uint32_t val)
{
    dlh_ier_data_t dlh_ier;
    dlh_ier.d32 = ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->dlh_ier;
    dlh_ier.ier.etbei = val;
    ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->dlh_ier = dlh_ier.d32;
}

/**
 * @brief  Get the value of @ref dlh_ier_data.erbfi.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @return The value of @ref dlh_ier_data.erbfi.
 */
static inline uint32_t hal_uart_dlh_ier_get_erbfi(uart_bus_t bus)
{
    dlh_ier_data_t dlh_ier;
    dlh_ier.d32 = ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->dlh_ier;
    return dlh_ier.ier.erbfi;
}

/**
 * @brief  Set the value of @ref dlh_ier_data.erbfi.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @param  [in]  val The value of @ref dlh_ier_data.erbfi.
 */
static inline void hal_uart_dlh_ier_set_erbfi(uart_bus_t bus, uint32_t val)
{
    dlh_ier_data_t dlh_ier;
    dlh_ier.d32 = ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->dlh_ier;
    dlh_ier.ier.erbfi = val;
    ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->dlh_ier = dlh_ier.d32;
}

/**
 * @brief  Get the value of @ref dlh_ier_data.elsi.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @return The value of @ref dlh_ier_data.elsi.
 */
static inline uint32_t hal_uart_dlh_ier_get_elsi(uart_bus_t bus)
{
    dlh_ier_data_t dlh_ier;
    dlh_ier.d32 = ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->dlh_ier;
    return dlh_ier.ier.elsi;
}

/**
 * @brief  Set the value of @ref dlh_ier_data.elsi.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @param  [in]  val The value of @ref dlh_ier_data.elsi.
 */
static inline void hal_uart_dlh_ier_set_elsi(uart_bus_t bus, uint32_t val)
{
    dlh_ier_data_t dlh_ier;
    dlh_ier.d32 = ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->dlh_ier;
    dlh_ier.ier.elsi = val;
    ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->dlh_ier = dlh_ier.d32;
}

/**
 * @brief  Get the value of @ref usr_data.busy.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @return The value of @ref usr_data.busy.
 */
static inline uint32_t hal_uart_usr_get_busy(uart_bus_t bus)
{
    usr_data_t usr;
    usr.d32 = ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->usr;
    return usr.b.busy;
}

/**
 * @brief  Get the value of @ref usr_data.tfnf.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @return The value of @ref usr_data.tfnf.
 */
static inline uint32_t hal_uart_usr_get_tfnf(uart_bus_t bus)
{
    usr_data_t usr;
    usr.d32 = ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->usr;
    return usr.b.tfnf;
}

/**
 * @brief  Set the value of @ref usr_data.tfnf.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @param  [in]  val The value of @ref usr_data.tfnf.
 */
static inline void hal_uart_usr_set_tfnf(uart_bus_t bus, uint32_t val)
{
    usr_data_t usr;
    usr.d32 = ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->usr;
    usr.b.tfnf = val;
    ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->usr = usr.d32;
}

/**
 * @brief  Get the value of @ref usr_data.rfne.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @return The value of @ref usr_data.rfne.
 */
static inline uint32_t hal_uart_usr_get_rfne(uart_bus_t bus)
{
    usr_data_t usr;
    usr.d32 = ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->usr;
    return usr.b.rfne;
}

/**
 * @brief  Set the value of @ref usr_data.rfne.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @param  [in]  val The value of @ref usr_data.rfne.
 */
static inline void hal_uart_usr_set_rfne(uart_bus_t bus, uint32_t val)
{
    usr_data_t usr;
    usr.d32 = ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->usr;
    usr.b.rfne = val;
    ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->usr = usr.d32;
}

/**
 * @brief  Get the value of @ref lsr_data.oe.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @return The value of @ref lsr_data.oe.
 */
static inline uint32_t hal_uart_lsr_get_oe(uart_bus_t bus)
{
    lsr_data_t lsr;
    lsr.d32 = ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->lsr;
    return lsr.b.oe;
}

/**
 * @brief  Set the value of @ref lsr_data.oe.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @param  [in]  val The value of @ref lsr_data.oe.
 */
static inline void hal_uart_lsr_set_oe(uart_bus_t bus, uint32_t val)
{
    lsr_data_t lsr;
    lsr.d32 = ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->lsr;
    lsr.b.oe = val;
    ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->lsr = lsr.d32;
}

/**
 * @brief  Get the value of @ref lsr_data.pe.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @return The value of @ref lsr_data.pe.
 */
static inline uint32_t hal_uart_lsr_get_pe(uart_bus_t bus)
{
    lsr_data_t lsr;
    lsr.d32 = ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->lsr;
    return lsr.b.pe;
}

/**
 * @brief  Set the value of @ref lsr_data.pe.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @param  [in]  val The value of @ref lsr_data.pe.
 */
static inline void hal_uart_lsr_set_pe(uart_bus_t bus, uint32_t val)
{
    lsr_data_t lsr;
    lsr.d32 = ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->lsr;
    lsr.b.pe = val;
    ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->lsr = lsr.d32;
}

/**
 * @brief  Get the value of @ref lsr_data.fe.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @return The value of @ref lsr_data.fe.
 */
static inline uint32_t hal_uart_lsr_get_fe(uart_bus_t bus)
{
    lsr_data_t lsr;
    lsr.d32 = ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->lsr;
    return lsr.b.fe;
}

/**
 * @brief  Set the value of @ref lsr_data.fe.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @param  [in]  val The value of @ref lsr_data.fe.
 */
static inline void hal_uart_lsr_set_fe(uart_bus_t bus, uint32_t val)
{
    lsr_data_t lsr;
    lsr.d32 = ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->lsr;
    lsr.b.fe = val;
    ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->lsr = lsr.d32;
}

/**
 * @brief  Get the value of @ref lsr_data.bi.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @return The value of @ref lsr_data.bi.
 */
static inline uint32_t hal_uart_lsr_get_bi(uart_bus_t bus)
{
    lsr_data_t lsr;
    lsr.d32 = ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->lsr;
    return lsr.b.bi;
}

/**
 * @brief  Set the value of @ref lsr_data.bi.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @param  [in]  val The value of @ref lsr_data.bi.
 */
static inline void hal_uart_lsr_set_bi(uart_bus_t bus, uint32_t val)
{
    lsr_data_t lsr;
    lsr.d32 = ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->lsr;
    lsr.b.bi = val;
    ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->lsr = lsr.d32;
}

/**
 * @brief  Get the value of @ref lsr_data.dr.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @return The value of @ref lsr_data.dr.
 */
static inline uint32_t hal_uart_lsr_get_dr(uart_bus_t bus)
{
    lsr_data_t lsr;
    lsr.d32 = ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->lsr;
    return lsr.b.dr;
}

/**
 * @brief  Set the value of @ref lsr_data.dr.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @param  [in]  val The value of @ref lsr_data.dr.
 */
static inline void hal_uart_lsr_set_dr(uart_bus_t bus, uint32_t val)
{
    lsr_data_t lsr;
    lsr.d32 = ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->lsr;
    lsr.b.dr = val;
    ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->lsr = lsr.d32;
}

/**
 * @brief  Get the value of @ref mcr_data.sire.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @return The value of @ref mcr_data.sire.
 */
static inline uint32_t hal_uart_mcr_get_sire(uart_bus_t bus)
{
    mcr_data_t mcr;
    mcr.d32 = ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->mcr;
    return mcr.b.sire;
}

/**
 * @brief  Set the value of @ref mcr_data.sire.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @param  [in]  val The value of @ref mcr_data.sire.
 */
static inline void hal_uart_mcr_set_sire(uart_bus_t bus, uint32_t val)
{
    mcr_data_t mcr;
    mcr.d32 = ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->mcr;
    mcr.b.sire = val;
    ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->mcr = mcr.d32;
}

/**
 * @brief  Get the value of @ref htx_data.htx.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @return The value of @ref htx_data.htx.
 */
static inline uint32_t hal_uart_htx_get_htx(uart_bus_t bus)
{
    htx_data_t htx;
    htx.d32 = ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->htx;
    return htx.b.htx;
}

/**
 * @brief  Set the value of @ref htx_data.htx.
 * @param  [in]  bus The index of uart. @ref uart_bus_t
 * @param  [in]  val The value of @ref htx_data.htx.
 */
static inline void hal_uart_htx_set_htx(uart_bus_t bus, uint32_t val)
{
    htx_data_t htx;
    htx.d32 = ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->htx;
    htx.b.htx = val;
    ((uart_v100_regs_t *)g_hal_uarts_regs[bus])->htx = htx.d32;
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