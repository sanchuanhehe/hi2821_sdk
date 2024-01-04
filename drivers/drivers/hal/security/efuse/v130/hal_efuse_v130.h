/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Provides hal efuse \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-03-02, Create file. \n
 */
#ifndef HAL_EFUSE_V130_H
#define HAL_EFUSE_V130_H

#include "hal_efuse.h"

#define EFUSE_FIRST_REGION_BYTES    (OTP_FIRST_REGION_BITS >> 3)  // MAX_BIT / 8
#define EFUSE_MAX_BITS              (OTP_FIRST_REGION_BITS)
#define EFUSE_MAX_BYTES             (OTP_FIRST_REGION_BYTES)  // MAX_BIT / 8
#define EFUSE_MAX_BIT_POS           8U
#define EFUSE_CTL_RB_BASE           0x57028000

#define HAL_EFUSE0_CTRL             (EFUSE_CTL_RB_BASE + 0x40)
#define HAL_EFUSE0_BASE_ADDR        (EFUSE_CTL_RB_BASE + 0x400)
#define HAL_EFUSE_EN                0xa5a5
#define HAL_EFUSE_OFF               0x5a5a

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_efuse Efuse
 * @ingroup  drivers_hal_efuse
 * @{
 */

/**
 * @if Eng
 * @brief  Get functions of the efuse .
 * @else
 * @brief  获取 efuse的实例
 * @endif
 */
hal_efuse_funcs_t *hal_efuse_v130_funcs_get(void);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
