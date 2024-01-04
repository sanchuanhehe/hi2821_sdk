/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Provides efuse register operation api \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-03-02， Create file. \n
 */
#ifndef HAL_EFUSE_V130_REGS_OP_H
#define HAL_EFUSE_V130_REGS_OP_H

#include <stdint.h>
#include "hal_efuse_v130_reg_def.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_efuse_regs_op Efuse Regs Operation
 * @ingroup  drivers_hal_efuse
 * @{
 */

extern efuse_regs_t *g_efuse_regs;

/**
 * @brief  Init the efuse which will set the base address of registers.
 * @param i efuse region
 * @return 0 indicates the base address of registers has been configured success, -1 indicates failure.
 */
int32_t hal_efuse_regs_init(int32_t i);

/**
 * @brief  Deinit the hal_efuse which will clear the base address of registers has been set
 *         by @ref hal_efuse_regs_init.
 */
void hal_efuse_regs_deinit(void);

/**
 * @brief  Get the value of @ref efuse_boot_done_data.boot_done.
 * @return The value of @ref efuse_boot_done_data.boot_done.
 */
static inline uint32_t hal_efuse_boot_done_data_get_boot_done(void)
{
    efuse_boot_done_data_t efuse_boot_done;
    efuse_boot_done.d32 = g_efuse_regs->efuse_boot_done_data;
    return efuse_boot_done.b.boot_done;
}

/**
 * @brief  Get the value of @ref efuse_boot_done_data.
 * @return The value of @ref efuse_boot_done_data.
 */
static inline uint32_t hal_efuse_boot_done_data_get(void)
{
    efuse_boot_done_data_t efuse_boot_done;
    efuse_boot_done.d32 = g_efuse_regs->efuse_boot_done_data;
    return efuse_boot_done.d32;
}

/**
 * @brief  Get the value of @ref efuse_ctl_data.
 * @return The value of @ref efuse_ctl_data.
 */
static inline uint32_t hal_efuse_ctl_get(void)
{
    efuse_ctl_data_t efuse_ctl;
    efuse_ctl.d32 = g_efuse_regs->efuse_ctl_data;
    return efuse_ctl.d32;
}

/**
 * @brief  Get the value of @ref efuse_ctl_data.efuse_wr_rd.
 * @return The value of @ref efuse_ctl_data.efuse_wr_rd.
 */
static inline uint32_t hal_efuse_ctl_get_wr_rd(void)
{
    efuse_ctl_data_t efuse_ctl;
    efuse_ctl.d32 = g_efuse_regs->efuse_ctl_data;
    return efuse_ctl.b.efuse_wr_rd;
}

/**
* @brief  Set the value of @ref efuse_ctl_data.efuse_wr_rd.
 * @param  [in]  val The value of @ref efuse_ctl_data.efuse_wr_rd.
 */
static inline void hal_efuse_ctl_set_wr_rd(uint32_t val)
{
    efuse_ctl_data_t efuse_ctl;
    efuse_ctl.d32 = g_efuse_regs->efuse_ctl_data;
    efuse_ctl.b.efuse_wr_rd = val;
    g_efuse_regs->efuse_ctl_data = efuse_ctl.d32;
}

/**
 * @brief  Get the value of @ref efuse_clk_period_data.
 * @return The value of @ref efuse_clk_period_data.
 */
static inline uint32_t hal_efuse_clk_period_get(void)
{
    efuse_period_data_t efuse_period;
    efuse_period.d32 = g_efuse_regs->efuse_clk_period_data;
    return efuse_period.d32;
}

/**
 * @brief  Get the value of @ref efuse_clk_period_data.clk.
 * @return The value of @ref efuse_clk_period_data.clk.
 */
static inline uint32_t hal_efuse_clk_period_get_clk(void)
{
    efuse_period_data_t efuse_period;
    efuse_period.d32 = g_efuse_regs->efuse_clk_period_data;
    return efuse_period.b.clk;
}

/**
 * @brief  Set the value of @ref efuse_clk_period_data.clk.
 * @param  [in]  val The value of @ref efuse_clk_period_data.clk
 */
static inline void hal_efuse_clk_period_set_clk(uint32_t val)
{
    efuse_period_data_t efuse_period;
    efuse_period.d32 = g_efuse_regs->efuse_clk_period_data;
    efuse_period.b.clk = val;
    g_efuse_regs->efuse_clk_period_data = efuse_period.d32;
}

/**
 * @brief  Get the value of @ref efuse_en_switch.en_switch.
 * @return The value of @ref efuse_en_switch.en_switch.
 */
static inline uint32_t hal_efuse_en_switch_data_get_en_switch(void)
{
    efuse_en_switch_data_t efuse_en_switch;
    efuse_en_switch.d32 = g_efuse_regs->efuse_en_switch_data;
    return efuse_en_switch.b.en_switch;
}

/**
 * @brief  Get the value of @ref efuse_en_switch_data.
 * @return The value of @ref efuse_en_switch_data.
 */
static inline uint32_t hal_efuse_en_switch_data_get(void)
{
    efuse_en_switch_data_t efuse_en_switch;
    efuse_en_switch.d32 = g_efuse_regs->efuse_en_switch_data;
    return efuse_en_switch.d32;
}

/**
 * @brief  Set the value of @ref efuse_en_switch.en_switch.
 * @param  [in]  val The value of @ref efuse_en_switch.en_switch.
 */
static inline void hal_efuse_en_switch_data_set_en_switch(uint32_t val)
{
    efuse_en_switch_data_t efuse_en_switch;
    efuse_en_switch.d32 = g_efuse_regs->efuse_en_switch_data;
    efuse_en_switch.b.en_switch = val;
    g_efuse_regs->efuse_en_switch_data = efuse_en_switch.d32;
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
