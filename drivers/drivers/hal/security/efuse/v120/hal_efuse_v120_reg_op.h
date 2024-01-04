/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides efuse register operation api \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-07-25， Create file. \n
 */
#ifndef HAL_EFUSE_IP0_REGS_OP_H
#define HAL_EFUSE_IP0_REGS_OP_H

#include <stdint.h>
#include "hal_efuse_v120_reg_def.h"
#include "efuse_porting.h"

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

extern efuse_regs_t *g_efuse_v120_regs[EFUSE_REGION_NUM];

/**
 * @brief  Init the efuse which will set the base address of registers.
 */
void hal_efuse_v120_regs_init(void);

/**
 * @brief  Deinit the hal_efuse which will clear the base address of registers has been set
 *         by @ref hal_efuse_regs_init.
 */
void hal_efuse_v120_regs_deinit(void);

/**
 * @brief  Get the value of @ref efuse_ctl_data.efuse_wr_rd.
 * @return The value of @ref efuse_ctl_data.efuse_wr_rd.
 */
static inline uint32_t hal_efuse_ctl_get_wr_rd(hal_efuse_region_t region)
{
    efuse_ctl_data_t efuse_ctl;
    efuse_ctl.d32 = g_efuse_v120_regs[region]->efuse_ctl_data.d32;
    return efuse_ctl.b.efuse_wr_rd;
}

/**
* @brief  Set the value of @ref efuse_ctl_data.efuse_wr_rd.
 * @param  [in]  val The value of @ref efuse_ctl_data.efuse_wr_rd.
 */
static inline void hal_efuse_ctl_set_wr_rd(hal_efuse_region_t region, uint32_t val)
{
    efuse_ctl_data_t efuse_ctl;
    efuse_ctl.d32 = g_efuse_v120_regs[region]->efuse_ctl_data.d32;
    efuse_ctl.b.efuse_wr_rd = val;
    g_efuse_v120_regs[region]->efuse_ctl_data.d32 = efuse_ctl.d32;
}

/**
 * @brief  Get the value of @ref efuse_clk_period_data.clk.
 * @return The value of @ref efuse_clk_period_data.clk.
 */
static inline uint32_t hal_efuse_clk_period_get_clk(hal_efuse_region_t region)
{
    efuse_period_data_t efuse_period;
    efuse_period.d32 = g_efuse_v120_regs[region]->efuse_clk_period_data.d32;
    return efuse_period.b.clk;
}

/**
 * @brief  Set the value of @ref efuse_clk_period_data.clk.
 * @param  [in]  val The value of @ref efuse_clk_period_data.clk
 */
static inline void hal_efuse_clk_period_set_clk(hal_efuse_region_t region, uint32_t val)
{
    efuse_period_data_t efuse_period;
    efuse_period.d32 = g_efuse_v120_regs[region]->efuse_clk_period_data.d32;
    efuse_period.b.clk = val;
    g_efuse_v120_regs[region]->efuse_clk_period_data.d32 = efuse_period.d32;
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
