/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides V100 watchdog register \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-07-21, Create file. \n
 */
#ifndef HAL_WATCHDOG_V100_REGS_DEF_H
#define HAL_WATCHDOG_V100_REGS_DEF_H

#include <stdint.h>
#include "watchdog_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_watchdog_v100_regs_def Watchdog V100 Regs Definition
 * @ingroup  drivers_hal_watchdog
 * @{
 */

/**
 * @brief  This union represents the bit fields in the Control Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union wdt_cr_data {
    uint32_t d32;                              /*!< Raw register data. */
    struct  {
        uint32_t wdt_en                : 1;    /*!< Watchdog enable bit. */
        uint32_t rmod                  : 1;    /*!< watchdog mode bit. */
        uint32_t rpl                   : 3;
        uint32_t no_name               : 1;    /*!< redundant r/w bit. */
        uint32_t rsvd_wdt_cr6_31       : 26;
    } b;                                       /*!< Register bits. */
} wdt_cr_data_t;

/**
 * @brief  This union represents the bit fields in the Timeout range Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union wdt_torr_data {
    uint32_t d32;                                  /*!< Raw register data. */
    struct  {
        uint32_t top                    : 4;    /*!< Timeout period.. */
        uint32_t top_init               : 4;    /*!< Timeout period for initialization. */
        uint32_t reserved8_31           : 24;
    } b;                                           /*!< Register bits. */
} wdt_torr_data_t;

/**
 * @brief  This union represents the bit fields in the Current counter value register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union wdt_ccvr_data {
    uint32_t d32;                                  /*!< Raw register data. */
    struct  {
        uint32_t wdt_ccvr                  : WDT_CNT_WIDTH - 1;    /*!< Timeout period.. */
        uint32_t rsvd_gpio_wdt_ccvry_31    : 32 - WDT_CNT_WIDTH + 1;    /*!< Timeout period for initialization. */
    } b;                                           /*!< Register bits. */
} wdt_ccvr_data_t;

/**
 * @brief  This union represents the bit fields in the Counter restart register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union wdt_crr_data {
    uint32_t d32;                                  /*!< Raw register data. */
    struct  {
        uint32_t wdt_crr                  : 8;    /*!< If this are written 0x76, Watchdog timer is restarted. */
        uint32_t rsvd_wdt_crr8_31         : 24;
    } b;                                           /*!< Register bits. */
} wdt_crr_data_t;

/**
 * @brief  This union represents the bit fields in the Interrupt status registerr.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union wdt_stat_data {
    uint32_t d32;                                  /*!< Raw register data. */
    struct  {
        uint32_t wdt_stat                  : 1;    /*!< This register shows the interrupt status of the WDT. */
        uint32_t rsvd_wdt_stat1_31         : 31;
    } b;                                           /*!< Register bits. */
} wdt_stat_data_t;

/**
 * @brief  This union represents the bit fields in the Interrupt clear register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union wdt_eoi_data {
    uint32_t d32;                                  /*!< Raw register data. */
    struct  {
        uint32_t wdt_eoi                   : 1;    /*!< Reading from this register returns zeroes(0) and clear the
                                                        interrupt. */
        uint32_t rsvd_wdt_eoi1_31          : 31;
    } b;                                           /*!< Register bits. */
} wdt_eoi_data_t;

/**
 * @brief  Registers associated with Watchdog.
 */
typedef struct watchdog_regs {
    volatile uint32_t wdt_cr;                  /*!< Control register.  <i>Offset: 00h</i>. */
    volatile uint32_t wdt_torr;                /*!< Timeout range register.  <i>offset: 04h</i>. */
    volatile uint32_t wdt_ccvr;                /*!< Current counter value register.  <i>Offset: 08h</i>. */
    volatile uint32_t wdt_crr;                 /*!< Counter restart register.  <i>Offset: 0ch</i>. */
    volatile uint32_t wdt_stat;                /*!< Interrupt status register.  <i>Offset: 10h</i>. */
    volatile uint32_t wdt_eoi;                 /*!< Interrupt clear register.  <i>Offset: 14h</i>. */
} watchdog_regs_t;

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif