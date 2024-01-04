/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides RISCV mpu register operation api \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-09-26， Create file. \n
 */

#ifndef HAL_PMP_RISCV70_REGS_OP_H
#define HAL_PMP_RISCV70_REGS_OP_H

#include <stdint.h>
#include "pmp_porting.h"
#include "hal_pmp_riscv70_regs_def.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_pmp_riscv70_regs_op PMP RISCV70 Regs Operation
 * @ingroup  drivers_hal_pmp
 * @{
 */

#define MEMXATTR_VECTOR         2

/**
 * @brief  Set address
 * @param  [in]  idx Region number
 * @param  [in]  pmpaddr Base address, First Address in NAPOT mode and End Address in TOR mode.
 */
void hal_pmp_riscv70_regs_set_pmpaddr(uint32_t idx, uint32_t pmpaddr);

/**
 * @brief  Set address
 * @param  [in]  idx Region number
 * @param  [in]  attr Read-write mode configuration.
 */
void hal_pmp_riscv70_regs_set_memxattr(uint32_t idx, uint8_t attr);

/**
 * @brief  Set address
 * @param  [in]  idx Region number
 * @param  [in]  cfg PMP region configuration.
 */
void hal_pmp_riscv70_regs_set_pmpxcfg(uint32_t idx, hal_pmpx_config_t cfg);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif