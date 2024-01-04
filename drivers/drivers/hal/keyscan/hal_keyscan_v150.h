/**
 * Copyright (c) @CompanyNameMagicTag 2022-2023. All rights reserved. \n
 *
 * Description: Provides v150 hal keyscan \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-09-16， Create file. \n
 */
#ifndef HAL_KEYSCAN_V150_H
#define HAL_KEYSCAN_V150_H

#include "hal_keyscan.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_keyscan_v150 Keyscan V150
 * @ingroup  drivers_hal_keyscan
 * @{
 */

/**
 * @if Eng
 * @brief  Get interface between keyscan driver and keyscan hal.
 * @else
 * @brief  获取Driver层KEYSCAN和HAL层KEYSCAN的接口。
 * @endif
 */
hal_keyscan_funcs_t *hal_keyscan_v150_funcs_get(void);

/**
 * @if Eng
 * @brief  Handler of the keyscan interrupt request.
 * @else
 * @brief  KEYSCAN中断处理函数。
 * @endif
 */
void hal_keyscan_v150_irq(void);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif