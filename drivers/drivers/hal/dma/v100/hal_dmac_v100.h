/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides V100 HAL dma \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-10-16， Create file. \n
 */
#ifndef HAL_DMAC_V100_H
#define HAL_DMAC_V100_H

#include "hal_dma.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_dma_v100 DMA V100
 * @ingroup  drivers_hal_dma
 * @{
 */

/**
 * @if Eng
 * @brief  Get the instance of v100.
 * @return The instance of v100.
 * @else
 * @brief  获取v100实例。
 * @return v100实例。
 * @endif
 */
hal_dma_funcs_t *hal_dmac_v100_funcs_get(void);

/**
 * @if Eng
 * @brief  Handler of the DMA interrupt request.
 * @else
 * @brief  DMA中断处理函数。
 * @endif
 */
void hal_dma_irq_handler(void);

#if defined(CONFIG_DMA_SUPPORT_SMDMA)
/**
 * @if Eng
 * @brief  Handler of the SDMA interrupt request.
 * @else
 * @brief  SDMA中断处理函数。
 * @endif
 */
void hal_sdma_irq_handler(void);
#endif /* CONFIG_DMA_SUPPORT_SMDMA */

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif