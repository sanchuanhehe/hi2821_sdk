/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Provides V150 HAL i2c \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-03-06, Create file. \n
 */

#ifndef HAL_I2C_V150_H
#define HAL_I2C_V150_H

#include <stdint.h>
#include "hal_i2c.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_i2c_v150 I2C_V150
 * @ingroup  drivers_hal_i2c
 * @{
 */

/**
 * @if Eng
 * @brief  Get the funcs of I2C_V150, see @ref hal_i2c_funcs_t
 * @return The funcs of i2c_v150_hal, see @ref hal_i2c_funcs_t
 * @else
 * @brief  I2C获取V150的函数。参考 @ref hal_i2c_funcs_t
 * @return I2C_V150的hal层函数。参考 @ref hal_i2c_funcs_t
 * @endif
 */
hal_i2c_funcs_t *hal_i2c_v150_funcs_get(void);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif

