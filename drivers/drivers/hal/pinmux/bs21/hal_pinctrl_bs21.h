/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides hal pinctrl \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-08-29, Create file. \n
 */
#ifndef HAL_PINCTRL_BS21_H
#define HAL_PINCTRL_BS21_H

#include "hal_pinctrl.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_pinctrl_bs21 Pinctrl BS21
 * @ingroup  drivers_hal_pinctrl
 * @{
 */

/**
 * @brief Get configuration functions of pins.
 * @return Configuration functions of pins. see @ref hal_pin_funcs_t
 */
hal_pin_funcs_t *hal_pin_bs21_funcs_get(void);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif