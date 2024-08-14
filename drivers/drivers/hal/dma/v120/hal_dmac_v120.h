/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides V120 HAL dma \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-12-16， Create file. \n
 */
#ifndef HAL_DMAC_V120_H
#define HAL_DMAC_V120_H

#include "hal_dma.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_dma_v120 DMA V120
 * @ingroup  drivers_hal_dma
 * @{
 */

hal_dma_funcs_t *hal_dmac_v120_funcs_get(void);

/**
 * @if Eng
 * @brief  Handler of the DMA interrupt request.
 * @else
 * @brief  DMA中断处理函数。
 * @endif
 */
void hal_dma_v120_irq_handler(void);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif