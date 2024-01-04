/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides HAL i2c \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-06-09, Create file. \n
 */

#include "common_def.h"
#include "hal_i2c.h"

static hal_i2c_funcs_t *g_hal_i2cs_funcs[I2C_BUS_MAX_NUM] = { 0 };

errcode_t hal_i2c_register_funcs(i2c_bus_t bus, hal_i2c_funcs_t *funcs)
{
    if (bus >= I2C_BUS_MAX_NUM || funcs == NULL) {
        return ERRCODE_INVALID_PARAM;
    }
    g_hal_i2cs_funcs[bus] = funcs;
    return ERRCODE_SUCC;
}

errcode_t hal_i2c_unregister_funcs(i2c_bus_t bus)
{
    if (bus >= I2C_BUS_MAX_NUM) {
        return ERRCODE_INVALID_PARAM;
    }
    g_hal_i2cs_funcs[bus] = NULL;
    return ERRCODE_SUCC;
}

hal_i2c_funcs_t *hal_i2c_get_funcs(i2c_bus_t bus)
{
    if (bus >= I2C_BUS_MAX_NUM) {
        return NULL;
    }
    return g_hal_i2cs_funcs[bus];
}
