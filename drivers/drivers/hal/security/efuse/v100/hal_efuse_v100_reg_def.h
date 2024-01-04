/**
 * Copyright (c) CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides efuse register \n
 * Author: CompanyName \n
 * History: \n
 * 2022-10-21, Create file. \n
 */
#ifndef HAL_EFUSE_IP0_REG_DEF_H
#define HAL_EFUSE_IP0_REG_DEF_H

#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_efuse_ip0_reg_def Efuse Regs Definition
 * @ingroup  drivers_hal_efuse
 * @{
 */

/**
 * @brief  This union represents the bit fields in the Control Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union efuse_ctl_data {
    uint32_t d32;                              /*!< Raw register data. */
    struct {
        uint32_t efuse_wr_rd            : 16;    /*!< efuse read/write unlock register. */
    } b;                                       /*!< Register bits. */
} efuse_ctl_data_t;

/**
 * @brief  This union represents the bit fields in the Timeout range Register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union efuse_clk_period_data {
    uint32_t d32;                                  /*!< Raw register data. */
    struct {
        uint32_t clk                    : 8;    /*!< clk select. */
        uint32_t reserved8_15           : 8;
    } b;                                           /*!< Register bits. */
} efuse_period_data_t;

/**
 * @brief  This union represents the bit fields in the Current counter value register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union efuse_mr_data {
    uint32_t d32;                                  /*!< Raw register data. */
    struct {
        uint32_t mode                  : 1;    /*!< Efuse read mode */
        uint32_t reserved2_15                : 15;
    } b;                                           /*!< Register bits. */
} efuse_mr_data_t;

/**
 * @brief  Registers associated with Efuse.
 */
typedef struct efuse_regs {
    volatile efuse_ctl_data_t efuse_ctl_data;                   /*!< Control register.  <i>Offset: 00h</i>. */
    volatile efuse_period_data_t efuse_clk_period_data;            /*!< Clk period register.  <i>offset: 04h</i>. */
    volatile efuse_mr_data_t efuse_mr_data;                    /*!< Efuse read mode register.  <i>Offset: 08h</i>. */
} efuse_regs_t;

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
