/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Microphone Codec Header \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-08-21, Create file. \n
 */
#ifndef VDT_CODEC_H
#define VDT_CODEC_H

#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

void vdt_codec_init(void);
void vdt_codec_deinit(void);
uint32_t vdt_codec_encode(uint8_t *pcm_data, uint8_t **enc_data);
uint32_t vdt_codec_decode(uint8_t *enc_data, uint8_t **pcm_data);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif