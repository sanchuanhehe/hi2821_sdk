/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Provides V151 HAL PDM. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-01-31, Create file. \n
 */
#ifndef HAL_PDM_V151_H
#define HAL_PDM_V151_H

#include "hal_pdm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_pdm_v151 PDM V151
 * @ingroup  drivers_hal_pdm
 * @{
 */

/**
 * @if Eng
 * @brief  Get the interface of PDM V151.
 * @return The interface of PDM V151.
 * @else
 * @brief  获取PDM V151接口实例。
 * @return PDM V151接口实例。
 * @endif
 */
hal_pdm_funcs_t *hal_pdm_v151_funcs_get(void);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif