/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides hal efuse \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-07-26, Create file. \n
 */
#ifndef HAL_EFUSE_IP0_H
#define HAL_EFUSE_IP0_H

#include "hal_efuse.h"

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
hal_efuse_funcs_t *hal_efuse_v100_funcs_get(void);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif

