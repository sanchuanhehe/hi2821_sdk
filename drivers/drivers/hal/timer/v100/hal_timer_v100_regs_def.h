/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides V100 timer register \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-11-07, Create file. \n
 */
#ifndef HAL_TIMER_V100_REGS_DEF_H
#define HAL_TIMER_V100_REGS_DEF_H

#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_timer_V100_regs_def TIMER V100 Regs Definition
 * @ingroup  drivers_hal_timer
 * @{
 */

#define TIME_REGS_MAX_NUM      8

/**
 * @brief  This union represents the bit fields in the Control
 *         Register.  Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union timer_control_reg_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t enable         : 1;    /*!< Timer enable bit. */
        uint32_t mode           : 1;    /*!< Timer mode bit. */
        uint32_t int_mask       : 1;    /*!< Timer interrupt mask bit. */
        uint32_t reserved3_31   : 29;
    } b;                                /*!< Register bits. */
} timer_control_reg_data_t;

/**
 * @brief  This union represents the bit fields in the  End-of-Interrupt
 *         Register.  Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union timer_eoi_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t eoi            : 1;    /*!< Reading from this register returns zeroes(0) and clear the interrupt. */
        uint32_t reserved1_31   : 31;
    } b;                                /*!< Register bits. */
} timer_eoi_data_t;

/**
 * @brief  This union represents the bit fields in the Interrupt Status
 *         Register.  Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union timer_int_status_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t int_status     : 1;    /*!< Contains the interrupt status for Timer. */
        uint32_t reserved1_31   : 31;
    } b;                                /*!< Register bits. */
} timer_int_status_data_t;

/**
 * @brief  Registers associated with Timer.
 */
typedef struct timer_regs_info {
    volatile uint32_t load_count;       /*!< Value to be loaded into Timer.             <i>Offset: 00h</i>. */
    volatile uint32_t current_value;    /*!< Current Value of Timer.                    <i>Offset: 04h</i>. */
    volatile uint32_t control_reg;      /*!< Control Register for Timer.                <i>Offset: 08h</i>. */
    volatile uint32_t eoi;              /*!< Clears the interrupt from Timer.           <i>Offset: 0Ch</i>. */
    volatile uint32_t int_status;       /*!< Contains the interrupt status for Timer.   <i>Offset: 10h</i>. */
} timer_regs_info_t;

typedef struct timers_regs {
    volatile timer_regs_info_t  g_timer_regs_info[TIME_REGS_MAX_NUM];
    volatile uint32_t           int_status;       /*!< Contains the interrupt status of all timers in the component.
                                                       <i>Offset: A0h</i>. */
    volatile uint32_t           eoi;              /*!< Returns all zeroes (0) and clears all active interrupts.
                                                       <i>Offset: A4h</i>. */
    volatile uint32_t           raw_int_status;   /*!< Contains the unmasked interrupt status of all timers
                                                       in the component.<i>Offset: A8h</i>. */
    volatile uint32_t           version;          /*!< Current revision number of the DW_apb_timers component.
                                                       <i>Offset: ACh</i>. */
} timer_regs_t;

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
