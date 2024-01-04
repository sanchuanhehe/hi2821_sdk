/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides V100 HAL i2c \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-08-15, Create file. \n
 */
#ifndef HAL_I2C_V100_H
#define HAL_I2C_V100_H

#include "hal_i2c.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_i2c_v100 I2C V100
 * @ingroup  drivers_hal_i2c
 * @{
 */

/**
 * @if Eng
 * @brief  Get the funcs of i2c_v100, see @ref hal_i2c_funcs_t
 * @return The funcs of i2c_v100_hal, see @ref hal_i2c_funcs_t
 * @else
 * @brief  I2C获取v100的函数。参考 @ref hal_i2c_funcs_t
 * @return Dw_apb的hal层函数。参考 @ref hal_i2c_funcs_t
 * @endif
 */
hal_i2c_funcs_t *hal_i2c_v100_funcs_get(void);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
