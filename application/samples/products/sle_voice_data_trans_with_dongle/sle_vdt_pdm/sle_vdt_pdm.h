/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: SLE_VDT PDM Source \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-09-23, Create file. \n
 */
#ifndef SLE_VDT_PDM_H
#define SLE_VDT_PDM_H

#include <stdint.h>
#include "dma.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define PDM_16K_BUFFER_LEN 32

int32_t sle_vdt_pdm_init(void);
int32_t sle_vdt_pdm_start_dma_transfer(uint32_t *pcm_buffer, dma_transfer_cb_t trans_done);
uint32_t sle_vdt_pdm_get_fifo_deepth(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif