/*
 * Copyright (c) @CompanyNameMagicTag 2021-2023. All rights reserved.
 * Description: Function headers of different chips in the NV module.
 */

#ifndef NV_PORTING_H
#define NV_PORTING_H

#include "stdint.h"
#include "stdbool.h"
#include "errcode.h"
#include "securec.h"
#include "soc_osal.h"
#include "memory_config_common.h"
#include "osal_addr.h"
#include "nv_config.h"
#include "nv_key.h"

static inline void *kv_malloc(uint32_t x)
{
    return osal_kmalloc(x, 0);
}

static inline void kv_free(void *x)
{
    osal_kfree(x);
}

errcode_t kv_flash_read(const uint32_t flash_offset, const uint32_t size, uint8_t *ram_data);
errcode_t kv_flash_write(const uint32_t flash_offset, uint32_t size, const uint8_t *ram_data, bool do_erase);
errcode_t kv_flash_erase(const uint32_t flash_offset, uint32_t size);

/* 创建加解密通道 */
errcode_t nv_crypto_claim_aes(uint32_t *crypto_handle, const kv_key_header_t *header);

/* 释放加解密通道 */
void nv_crypto_release_aes(uint32_t crypto_handle);

/* NV数据加密 */
errcode_t nv_crypto_encode(uint32_t crypto_handle, const uintptr_t src, uintptr_t dest, uint32_t length);

/* NV数据解密 */
errcode_t nv_crypto_decode(uint32_t crypto_handle, const uintptr_t src, uintptr_t dest, uint32_t length);

/* 获取加密Tag */
errcode_t nv_crypto_get_tag(uint32_t crypto_handle, uint8_t *tag, uint32_t *tag_len);

/* 设置当前NV的Tag */
errcode_t nv_crypto_set_tag(uint32_t crypto_handle, uint8_t *tag, uint32_t tag_len);

/* 校验Tag */
errcode_t nv_crypto_validate_tag(uint32_t crypto_handle);

/* 获取随机数 */
void nv_crypto_generate_random(uint32_t *rnd);

/* 启动HASH计算 */
errcode_t nv_crypto_start_hash(void);

/* 更新HASH */
errcode_t nv_crypto_update_hash(const uint8_t *src, uint32_t length);

/* 完成HASH计算并返回HASH值 */
errcode_t nv_crypto_complete_hash(uint8_t *hash);

#ifdef KV_DEBUG
#define dprint(x...)                  dprintf(x)
#define nv_log_info(x...)
#define nv_log_debug(x...)
#define nv_log_err(x...)
#else
#define nv_log_err(x...)
#define nv_log_info(x...)
#define nv_log_debug(x...)
#endif

#endif /* NV_PORTING_H */
