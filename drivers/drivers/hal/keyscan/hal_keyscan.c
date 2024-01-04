/**
 * Copyright (c) @CompanyNameMagicTag 2022-2023. All rights reserved. \n
 *
 * Description: Provides hal keyscan \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-09-16， Create file. \n
 */

#include <stdio.h>
#include "errcode.h"
#include "common_def.h"
#include "keyscan.h"
#include "hal_keyscan.h"

hal_keyscan_funcs_t *g_hal_keyscans_funcs = NULL;

errcode_t hal_keyscan_register_funcs(hal_keyscan_funcs_t *funcs)
{
    if (unlikely(funcs == NULL)) {
        return ERRCODE_INVALID_PARAM;
    }
    g_hal_keyscans_funcs = funcs;
    return ERRCODE_SUCC;
}

errcode_t hal_keyscan_unregister_funcs(void)
{
    g_hal_keyscans_funcs = NULL;
    return ERRCODE_SUCC;
}

hal_keyscan_funcs_t *hal_keyscan_get_funcs(void)
{
    return g_hal_keyscans_funcs;
}