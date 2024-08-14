/*
 * Copyright (c) @CompanyNameMagicTag 2021-2022. All rights reserved.
 * Description: ztw523 touch driver.
 * Author: @CompanyNameTag
 * Create: 2022-04-12
 */

#include "ztw523_drv1.h"
#include "touch_screen_def.h"
#include "ztw523_ctrl.h"
#include "i2c.h"
#include "debug_print.h"
#include "hal_i2c.h"

static ext_errno ztw523_drv_init(ztw523_ctrl_ops *config)
{
    ext_errno ret;
    ztw523_ctrl_ops *ops = config;

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
        tp_err("ZTW523 bus_init fail! ret = 0x%x", ret);
        return ret;
    }

    ret = ztw523_init();
    if (ret != EXT_ERR_SUCCESS) {
        tp_err("ZTW523 init fail! ret = 0x%x", ret);
        return ret;
    }

    return EXT_ERR_SUCCESS;
}

static ext_errno ztw523_drv_deinit(void)
{
    return EXT_ERR_SUCCESS;
}

static ext_errno ztw523_drv_get_info(uint8_t *data, uint8_t data_len)
{
    return ztw523_irq_callback(data, data_len);
}

static ext_errno ztw523_drv_suspend(void)
{
    return ztw523_set_power_mode(MC_TP_SUSPEND_SCAN);
}

static ext_errno ztw523_drv_resume(void)
{
    return ztw523_set_power_mode(MC_TP_RESUME_SCAN);
}

static ext_errno ztw523_drv_sleep(void)
{
    return ztw523_set_power_mode(MC_TP_SLEEP_WORK_MODE);
}

touch_peripheral_api g_ztw523_api = {
    .touch_init = ztw523_drv_init,
    .touch_deinit = ztw523_drv_deinit,
    .touch_get_tpinfo = ztw523_drv_get_info,
    .touch_suspend = ztw523_drv_suspend,
    .touch_resume = ztw523_drv_resume,
    .touch_sleep = ztw523_drv_sleep,
    .touch_bus_init = ztw523_host_peripheral_init,
    .register_callback = ztw523_register_handle,
    .unregister_callback = ztw523_unregister_handle,
};

ztw523_ctrl_ops g_ztw523_ctrl = {
    .attr = {
        .int_gpio = ZTW523_INT_GPIO,
        .i2c_id = ZTW523_I2C_BUS,
        .i2c_speed = I2C_SPEED_MODE_SS,
    },
    .bus_init = (ztw523_bus_init)ztw523_host_peripheral_init,
};

void *touch_screen_get_api(void)
{
    return ((void *)&g_ztw523_api);
}

void *touch_screen_get_peri_attr(void)
{
    return ((void *)&g_ztw523_ctrl);
}