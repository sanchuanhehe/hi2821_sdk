/**
 * Copyright (c) @CompanyNameMagicTag 2022-2023. All rights reserved. \n
 *
 * Description: Provides qdec driver source \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-09-05, Create file. \n
 */
#include "common_def.h"
#include "securec.h"
#include "soc_osal.h"
#include "qdec_porting.h"
#include "qdec.h"

static bool g_qdec_inited = false;
static bool g_qdec_enabled = false;

hal_qdec_funcs_t *g_hal_func = NULL;

errcode_t uapi_qdec_init(qdec_config_t const *config)
{
    if (unlikely(config == NULL)) {
        return ERRCODE_QDEC_INVALID_PARAMETER;
    }

    if (unlikely(g_qdec_inited)) {
        return ERRCODE_QDEC_INVALID_STATE;
    }

    qdec_port_register_hal_funcs();
    g_hal_func = hal_qdec_get_funcs();
    g_hal_func->init(config);
    g_qdec_inited = true;

    return ERRCODE_SUCC;
}

errcode_t uapi_qdec_deinit(void)
{
    if (unlikely(g_qdec_inited == false)) {
        return ERRCODE_SUCC;
    }

    if (unlikely(g_qdec_enabled)) {
        uapi_qdec_disable();
    }

    g_hal_func->deinit();
    g_qdec_inited = false;

    return ERRCODE_SUCC;
}

errcode_t uapi_qdec_enable(void)
{
    if (unlikely(!g_qdec_inited)) {
        return ERRCODE_QDEC_INVALID_STATE;
    }

    if (unlikely(g_qdec_enabled == true)) {
        return ERRCODE_SUCC;
    }

    g_hal_func->enable();
    g_qdec_enabled = true;

    return ERRCODE_SUCC;
}

errcode_t uapi_qdec_disable(void)
{
    if (unlikely(g_qdec_inited == false)) {
        return ERRCODE_QDEC_INVALID_STATE;
    }

    g_hal_func->disable();
    g_qdec_enabled = false;

    return ERRCODE_SUCC;
}

errcode_t uapi_qdec_read_accumulators(int16_t *acc, int16_t *accdbl)
{
    if (unlikely((acc == NULL) || (accdbl == NULL))) {
        return ERRCODE_QDEC_INVALID_PARAMETER;
    }

    if (unlikely((!g_qdec_inited) || (!g_qdec_enabled))) {
        return ERRCODE_QDEC_INVALID_STATE;
    }

    g_hal_func->read_accumulators(acc, accdbl);

    return ERRCODE_SUCC;
}

errcode_t uapi_qdec_register_callback(qdec_callback_t callback)
{
    if (unlikely(callback == NULL)) {
        return ERRCODE_QDEC_INVALID_PARAMETER;
    }

    if (unlikely(g_qdec_inited == false)) {
        return ERRCODE_QDEC_INVALID_STATE;
    }

    g_hal_func->register_callback(callback);

    return ERRCODE_SUCC;
}