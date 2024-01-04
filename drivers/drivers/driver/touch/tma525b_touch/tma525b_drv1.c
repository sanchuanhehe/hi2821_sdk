/*
 * Copyright (c) @CompanyNameMagicTag 2021-2022. All rights reserved.
 * Description: tma525b touch driver.
 * Author: @CompanyNameTag
 * Create: 2022-04-12
 */

#include "tma525b_drv1.h"
#include "debug_print.h"
#include "i2c.h"
#include "hal_i2c.h"
#include "tma525b_ctrl.h"
#include "tma525b_touch1.h"
#include "touch_screen_def.h"

static ext_errno tma525b_drv_init(tma525b_ctrl_ops *config)
{
    ext_errno ret;
    tma525b_ctrl_ops *ops = config;

    if (ops == NULL) {
        tp_err("peripheral ops is empty!");
        return EXT_ERR_FAILURE;
    }

    if (ops->bus_init == NULL) {
        tp_err("peripheral bus_init func is empty!");
        return EXT_ERR_FAILURE;
    }

    ret = ops->bus_init(&(ops->attr));
    if (ret != EXT_ERR_SUCCESS) {
        tp_err("TMA525B bus_init fail! ret = 0x%x", ret);
        return ret;
    }

    ret = tma525b_init();
    if (ret != EXT_ERR_SUCCESS) {
        tp_err("TMA525B init fail! ret = 0x%x", ret);
        return ret;
    }

    return EXT_ERR_SUCCESS;
}

static ext_errno tma525b_drv_deinit(void)
{
    return EXT_ERR_SUCCESS;
}

static ext_errno tma525b_drv_get_info(uint8_t *data, uint8_t data_len)
{
    return tma525b_irq_callback(data, data_len);
}

static ext_errno tma525b_drv_suspend(void)
{
    return tma525b_set_power_mode(MC_TP_SUSPEND_SCAN);
}

static ext_errno tma525b_drv_resume(void)
{
    return tma525b_set_power_mode(MC_TP_RESUME_SCAN);
}

static ext_errno tma525b_drv_sleep(void)
{
    return tma525b_set_power_mode(MC_TP_SLEEP_WORK_MODE);
}

touch_peripheral_api g_tma525b_api = {
    .touch_init = tma525b_drv_init,
    .touch_deinit = tma525b_drv_deinit,
    .touch_get_tpinfo = tma525b_drv_get_info,
    .touch_suspend = tma525b_drv_suspend,
    .touch_resume = tma525b_drv_resume,
    .touch_sleep = tma525b_drv_sleep,
    .touch_bus_init = tma525b_host_peripheral_init,
    .register_callback = tma525b_register_handle,
    .unregister_callback = tma525b_unregister_handle,
};

tma525b_ctrl_ops g_tma525b_ctrl = {
    .attr = {
        .int_gpio = TMA525B_INT_GPIO,
        .i2c_id = TMA525B_I2C_BUS,
        .i2c_speed = I2C_SPEED_MODE_SS,
    },
    .bus_init = (tma525b_bus_init)tma525b_host_peripheral_init,
};

void *touch_screen_get_api(void)
{
    return ((void *)&g_tma525b_api);
}

void *touch_screen_get_peri_attr(void)
{
    return ((void *)&g_tma525b_ctrl);
}