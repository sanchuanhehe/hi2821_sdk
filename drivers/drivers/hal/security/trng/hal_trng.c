/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides trng hal funcs register and unregister handle\n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-06-05, Create file. \n
 */
#include "hal_trng.h"

static hal_trng_funcs_t *g_hal_trng_funcs = NULL;

errcode_t hal_trng_register_funcs(hal_trng_funcs_t *funcs)
{
    if (funcs == NULL) {
        return ERRCODE_INVALID_PARAM;
    }
    g_hal_trng_funcs = funcs;
    return ERRCODE_SUCC;
}

errcode_t hal_trng_unregister_funcs(void)
{
    g_hal_trng_funcs = NULL;
    return ERRCODE_SUCC;
}

hal_trng_funcs_t *hal_trng_get_funcs(void)
{
    return g_hal_trng_funcs;
}