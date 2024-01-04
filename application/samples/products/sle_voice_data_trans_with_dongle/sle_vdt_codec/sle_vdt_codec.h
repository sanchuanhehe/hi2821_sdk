/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: SLE VDT Codec Header \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-09-23, Create file. \n
 */
#ifndef SLE_VDT_CODEC_H
#define SLE_VDT_CODEC_H

#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define ENC_FREAM_16K_SBC_SIZE 256
#define ENC_FREAM_16K_MSBC_SIZE 240
#define ENC_FREAM_16K_OPUS_SIZE 320
#define ENC_FREAM_16K_L2HC_SIZE 320

#define ENC_FREAM_48K_SBC_SIZE 256
#define ENC_FREAM_48K_MSBC_SIZE 240
#define ENC_FREAM_48K_OPUS_SIZE 960
#define ENC_FREAM_48K_L2HC_SIZE 960

#define DEC_FREAM_16K_SBC_SIZE 68
#define DEC_FREAM_16K_MSBC_SIZE 60
#define DEC_FREAM_16K_OPUS_SIZE 100
#define DEC_FREAM_16K_L2HC_SIZE 84

#define DEC_FREAM_48K_SBC_SIZE 68
#define DEC_FREAM_48K_MSBC_SIZE 60
#define DEC_FREAM_48K_OPUS_SIZE 120
#define DEC_FREAM_48K_L2HC_SIZE 84

void sle_vdt_codec_init(void);
void sle_vdt_codec_deinit(void);
uint32_t sle_vdt_codec_encode(uint8_t *pcm_data, uint8_t **enc_data);
uint32_t sle_vdt_codec_decode(uint8_t *enc_data, uint8_t **pcm_data);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif