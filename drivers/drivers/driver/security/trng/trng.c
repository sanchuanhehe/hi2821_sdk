/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides trng driver source \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-08-09, Create file. \n
 */

#include "trng.h"
#include "drv_trng.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


errcode_t uapi_drv_cipher_trng_get_random(uint32_t *randnum)
{
    return drv_cipher_trng_get_random((uint8_t *)randnum, sizeof(uint32_t));
}

errcode_t uapi_drv_cipher_trng_get_random_bytes(uint8_t *randnum, uint32_t size)
{
    return drv_cipher_trng_get_random(randnum, size);
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
