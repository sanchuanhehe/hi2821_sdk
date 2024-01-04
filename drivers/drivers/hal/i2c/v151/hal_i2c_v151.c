/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides HAL i2c \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-08-15, Create file. \n
 */

#include "hal_i2c.h"
#include "hal_i2c_v151_master.h"
#include "hal_i2c_v151_slave.h"
#include "hal_i2c_v151.h"

static hal_i2c_funcs_t g_hal_i2c_v151_funcs = {

#if defined(CONFIG_I2C_SUPPORT_MASTER)
    .master_init = hal_i2c_v151_master_init,
#else
    .master_init = NULL,
#endif
#if defined(CONFIG_I2C_SUPPORT_SLAVE)
    .slave_init = hal_i2c_v151_slave_init,
#else
    .slave_init = NULL,
#endif
    .deinit = hal_i2c_v151_deinit,
    .write = hal_i2c_v151_write,
    .read = hal_i2c_v151_read,
    .ctrl = hal_i2c_v151_ctrl
};

hal_i2c_funcs_t *hal_i2c_v151_funcs_get(void)
{
    return &g_hal_i2c_v151_funcs;
}
