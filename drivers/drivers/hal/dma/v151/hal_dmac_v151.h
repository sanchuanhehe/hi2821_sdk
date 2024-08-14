/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Provides V151 HAL dma \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-2-5， Create file. \n
 */
#ifndef HAL_DMAC_V151_H
#define HAL_DMAC_V151_H

#include "hal_dma.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_dma_v151 DMA V151
 * @ingroup  drivers_hal_dma
 * @{
 */

hal_dma_funcs_t *hal_dmac_v151_funcs_get(void);

/**
 * @if Eng
 * @brief  Handler of the DMA interrupt request.
 * @else
 * @brief  DMA中断处理函数。
 * @endif
 */
void hal_dma_v151_irq_handler(void);

void dma_port_release_handshaking_source(dma_channel_t ch);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif