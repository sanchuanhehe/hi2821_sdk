/**
 * Copyright (c) @CompanyNameMagicTag 2022-2023. All rights reserved. \n
 *
 * Description: Provides HAL qdec \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-09-15, Create file. \n
 */
#include <stdio.h>
#include "common_def.h"
#include "errcode.h"
#include "hal_qdec.h"

static hal_qdec_funcs_t *g_hal_qdec_funcs = NULL;

errcode_t hal_qdec_register_funcs(hal_qdec_funcs_t *funcs)
{
    if (unlikely(funcs == NULL)) {
        return ERRCODE_QDEC_INVALID_PARAMETER;
    }
    g_hal_qdec_funcs = funcs;
    return ERRCODE_SUCC;
}

void hal_qdec_unregister_funcs(void)
{
    g_hal_qdec_funcs = NULL;
}

hal_qdec_funcs_t *hal_qdec_get_funcs(void)
{
    return g_hal_qdec_funcs;
}