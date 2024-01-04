/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides V100 watchdog register operation api \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-07-25， Create file. \n
 */
#ifndef HAL_WATCHDOG_V100_REGS_OP_H
#define HAL_WATCHDOG_V100_REGS_OP_H

#include <stdint.h>
#include "hal_watchdog_v100_regs_def.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_watchdog_v100_regs_op Watchdog V100 Regs Operation
 * @ingroup  drivers_hal_watchdog
 * @{
 */

extern uintptr_t g_watchdog_regs;

/**
 * @brief  Init the watchdog which will set the base address of registers.
 * @return 0 indicates the base address of registers has been configured success, -1 indicates failure.
 */
int32_t hal_watchdog_v100_regs_init(void);

/**
 * @brief  Deinit the hal_watchdog which will clear the base address of registers has been set
 *         by @ref hal_watchdog_v100_regs_init.
 */
void hal_watchdog_v100_regs_deinit(void);

/**
 * @brief  Get the value of @ref wdt_cr_data.
 * @return The value of @ref wdt_cr_data.
 */
static inline uint32_t hal_watchdog_wdt_cr_get(void)
{
    wdt_cr_data_t wdt_cr;
    wdt_cr.d32 = ((watchdog_regs_t *)g_watchdog_regs)->wdt_cr;
    return wdt_cr.d32;
}

/**
 * @brief  Get the value of @ref wdt_cr_data.wdt_en.
 * @return The value of @ref wdt_cr_data.wdt_en.
 */
static inline uint32_t hal_watchdog_wdt_cr_get_wdt_en(void)
{
    wdt_cr_data_t wdt_cr;
    wdt_cr.d32 = ((watchdog_regs_t *)g_watchdog_regs)->wdt_cr;
    return wdt_cr.b.wdt_en;
}

/**
 * @brief  Set the value of @ref wdt_cr_data.wdt_en.
 * @param  [in]  val The value of @ref wdt_cr_data.wdt_en
 */
static inline void hal_watchdog_wdt_cr_set_wdt_en(uint32_t val)
{
    wdt_cr_data_t wdt_cr;
    wdt_cr.d32 = ((watchdog_regs_t *)g_watchdog_regs)->wdt_cr;
    wdt_cr.b.wdt_en = val;
    ((watchdog_regs_t *)g_watchdog_regs)->wdt_cr = wdt_cr.d32;
}

/**
 * @brief  Get the value of @ref wdt_cr_data.rmod.
 * @return The value of @ref wdt_cr_data.rmod.
 */
static inline uint32_t hal_watchdog_wdt_cr_get_wdt_rmod(void)
{
    wdt_cr_data_t wdt_cr;
    wdt_cr.d32 = ((watchdog_regs_t *)g_watchdog_regs)->wdt_cr;
    return wdt_cr.b.rmod;
}

/**
 * @brief  Set the value of @ref wdt_cr_data.rmod.
 * @param  [in]  val The value of @ref wdt_cr_data.rmod
 */
static inline void hal_watchdog_wdt_cr_set_rmod(uint32_t val)
{
    wdt_cr_data_t wdt_cr;
    wdt_cr.d32 = ((watchdog_regs_t *)g_watchdog_regs)->wdt_cr;
    wdt_cr.b.rmod = val;
    ((watchdog_regs_t *)g_watchdog_regs)->wdt_cr = wdt_cr.d32;
}

/**
 * @brief  Get the value of @ref wdt_torr_data.
 * @return The value of @ref wdt_torr_data.
 */
static inline uint32_t hal_watchdog_wdt_torr_get(void)
{
    wdt_torr_data_t wdt_torr;
    wdt_torr.d32 = ((watchdog_regs_t *)g_watchdog_regs)->wdt_torr;
    return wdt_torr.d32;
}

/**
 * @brief  Get the value of @ref wdt_torr_data.top.
 * @return The value of @ref wdt_torr_data.top.
 */
static inline uint32_t hal_watchdog_wdt_torr_get_top(void)
{
    wdt_torr_data_t wdt_torr;
    wdt_torr.d32 = ((watchdog_regs_t *)g_watchdog_regs)->wdt_torr;
    return wdt_torr.b.top;
}

/**
 * @brief  Set the value of @ref wdt_torr_data.top.
 * @param  [in]  val The value of @ref wdt_torr_data.top
 */
static inline void hal_watchdog_wdt_torr_set_top(uint32_t val)
{
    wdt_torr_data_t wdt_torr;
    wdt_torr.d32 = ((watchdog_regs_t *)g_watchdog_regs)->wdt_torr;
    wdt_torr.b.top = val;
    ((watchdog_regs_t *)g_watchdog_regs)->wdt_torr = wdt_torr.d32;
}

/**
 * @brief  Get the value of @ref wdt_ccvr_data.wdt_ccvr.
 * @return The value of @ref wdt_ccvr_data.wdt_ccvr.
 */
static inline uint32_t hal_watchdog_wdt_ccvr_get_wdt_ccvr(void)
{
    wdt_ccvr_data_t wdt_ccvr;
    wdt_ccvr.d32 = ((watchdog_regs_t *)g_watchdog_regs)->wdt_ccvr;
    return wdt_ccvr.b.wdt_ccvr;
}

/**
 * @brief  Set the value of @ref wdt_crr_data.wdt_crr.
 * @param  [in]  val The value of @ref wdt_crr_data.wdt_crr
 */
static inline void hal_watchdog_wdt_crr_set_wdt_crr(uint32_t val)
{
    wdt_crr_data_t wdt_crr;
    wdt_crr.d32 = ((watchdog_regs_t *)g_watchdog_regs)->wdt_crr;
    wdt_crr.b.wdt_crr = val;
    ((watchdog_regs_t *)g_watchdog_regs)->wdt_crr = wdt_crr.d32;
}

/**
 * @brief  Get the value of @ref wdt_stat_data.wdt_stat.
 * @return The value of @ref wdt_stat_data.wdt_stat.
 */
static inline uint32_t hal_watchdog_wdt_stat_get_wdt_stat(void)
{
    wdt_stat_data_t wdt_stat;
    wdt_stat.d32 = ((watchdog_regs_t *)g_watchdog_regs)->wdt_stat;
    return wdt_stat.b.wdt_stat;
}

/**
 * @brief  Get the value of @ref wdt_eoi_data.wdt_eoi.
 * @return The value of @ref wdt_eoi_data.wdt_eoi.
 */
static inline uint32_t hal_watchdog_wdt_eoi_get_wdt_eoi(void)
{
    wdt_eoi_data_t wdt_eoi;
    wdt_eoi.d32 = ((watchdog_regs_t *)g_watchdog_regs)->wdt_eoi;
    return wdt_eoi.b.wdt_eoi;
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