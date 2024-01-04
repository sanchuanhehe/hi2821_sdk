/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: UPG lzma Interface Header.
 */

#ifndef UPG_LZMADEC_H
#define UPG_LZMADEC_H

#include <stdint.h>
#include "LzmaDec.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define OUT_BUF_SIZE 0x1000
#define IN_BUF_SIZE 0x1000

typedef struct upg_lzma_decode2_data {
    uint8_t *inbuf;
    uint8_t *outbuf;
    uint32_t image_id;
    uint32_t in_offset; // 已解压长度偏移
    uint32_t out_offset;
    uint32_t compress_len; // 解压包总长度
    uint32_t decompress_len;
} upg_lzma_decode2_data_t;

uint32_t upg_lzma_init(CLzmaDec *p, upg_lzma_decode2_data_t *val_data, const Byte *props, uint32_t props_len);
void upg_lzma_deinit(CLzmaDec *p, upg_lzma_decode2_data_t *val_data);
uint32_t upg_lzma_decode(CLzmaDec *p, upg_lzma_decode2_data_t *data);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* UPG_LZMADEC_H */
