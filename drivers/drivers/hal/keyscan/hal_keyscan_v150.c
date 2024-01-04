/**
 * Copyright (c) @CompanyNameMagicTag 2022-2023. All rights reserved. \n
 *
 * Description: Provides v150 hal keyscan \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-09-16， Create file. \n
 */

#include <stdint.h>
#include <stdio.h>
#include "securec.h"
#include "oal_interface.h"
#include "errcode.h"
#include "common_def.h"
#include "chip_core_irq.h"
#include "keyscan_porting.h"
#include "hal_keyscan.h"
#include "hal_keyscan_v150_regs_op.h"
#include "hal_keyscan_v150.h"

static hal_keyscan_callback_t g_hal_keyscan_callback = NULL;
static keyscan_config_t g_keyscan_config = KEYSCAN_DEFAULT_CONFIG;
static errcode_t hal_keyscan_v150_ctrl_disable(hal_keyscan_ctrl_id_t id, uintptr_t param);

static void hal_keyscan_v150_init(keyscan_pulse_time_t time, keyscan_mode_t mode, keyscan_int_t event_type,
                                  hal_keyscan_callback_t callback)
{
    g_hal_keyscan_callback = callback;
    hal_keyscan_v150_regs_init();
    hal_keyscan_soft_rst();
    hal_keyscan_regs_set_clk_keep(0);
    if (keyscan_porting_get_type_sel() == FULL_KEYS_TYPE) {
        hal_keyscan_pin_enable((CONFIG_KEYSCAN_ENABLE_ALL_ROW - 1), (CONFIG_KEYSCAN_ENABLE_ALL_CLO - 1));
    } else if (keyscan_porting_get_type_sel() == SIX_KEYS_TYPE) {
        hal_keyscan_pin_enable(KEYSCAN_ENABLE_THREE_ROW, KEYSCAN_ENABLE_TWO_CLO);
    }
    hal_keyscan_config_pulse_time(time);
    hal_keyscan_config_mode(mode);
    hal_keyscan_config_direction(g_keyscan_config.direction);
    hal_keyscan_config_wait_time(g_keyscan_config.wait_time);
    hal_keyscan_config_idle_time(g_keyscan_config.idle_time);
    hal_keyscan_config_defence_num(g_keyscan_config.defence_time);
    hal_keyscan_config_ghost_check(g_keyscan_config.ghost_check);
    hal_keyscan_config_io_de(g_keyscan_config.io_de);
    hal_keyscan_config_key_select_num(g_keyscan_config.select_num);
    keyscan_port_register_irq(KEY_SCAN_IRQN);
#if defined (CONFIG_KEYSCAN_SUPPORT_SLEEP)
    hal_keyscan_enable_int(bit(event_type) + bit(KEYSCAN_INT_PRESS_AON));
#else
    hal_keyscan_enable_int(bit(event_type));
#endif
    keyscan_porting_pin_set();
}

static void hal_keyscan_v150_deinit(void)
{
    hal_keyscan_v150_ctrl_disable(KEYSCAN_CTRL_DISABLE, 0);
    keyscan_port_unregister_irq(KEY_SCAN_IRQN);
    hal_keyscan_v150_regs_deinit();
}

static errcode_t hal_keyscan_v150_ctrl_enable(hal_keyscan_ctrl_id_t id, uintptr_t param)
{
    unused(id);
    unused(param);
    hal_keyscan_regs_set_enable(1);
    hal_keyscan_regs_set_clk_ena(1);
    hal_keyscan_regs_set_start();
    for (uint32_t i = 0; i < KEYSCAN_INT_MAX_NUM; i++) {
        hal_keyscan_clr_int(bit(i));
    }
    return ERRCODE_KEYSCAN_POWER_ON;
}

static errcode_t hal_keyscan_v150_ctrl_disable(hal_keyscan_ctrl_id_t id, uintptr_t param)
{
    unused(id);
    unused(param);
    hal_keyscan_regs_set_enable(0);
    hal_keyscan_regs_set_clk_ena(0);
    return ERRCODE_SUCC;
}

#if defined (CONFIG_KEYSCAN_SUPPORT_LPM)
static errcode_t hal_keyscan_v150_ctrl_suspend(hal_keyscan_ctrl_id_t id, uintptr_t param)
{
    unused(id);
    unused(param);
    return ERRCODE_SUCC;
}

static errcode_t hal_keyscan_v150_ctrl_resume(hal_keyscan_ctrl_id_t id, uintptr_t param)
{
    unused(id);
    unused(param);
    return ERRCODE_SUCC;
}
#endif

void hal_keyscan_v150_irq(void)
{
    if (!g_hal_keyscan_callback) {
        return;
    }
    for (uint32_t event_type = 0; event_type < KEYSCAN_INT_MAX_NUM; event_type++) {
        if (hal_keyscan_int_enable_check(event_type) == 1 && hal_keyscan_int_check(event_type) == 1) {
            hal_keyscan_clr_int(bit(event_type));
            g_hal_keyscan_callback(hal_keyscan_get_key_value());
        }
    }
}

static hal_keyscan_ctrl_t g_hal_keyscan_ctrl_func_array[KEYSCAN_CTRL_MAX] = {
    hal_keyscan_v150_ctrl_enable,
    hal_keyscan_v150_ctrl_disable,
#if defined (CONFIG_KEYSCAN_SUPPORT_LPM)
    hal_keyscan_v150_ctrl_suspend,
    hal_keyscan_v150_ctrl_resume,
#endif
};

static errcode_t hal_keyscan_v150_ctrl(hal_keyscan_ctrl_id_t id, uintptr_t param)
{
    return g_hal_keyscan_ctrl_func_array[id](id, param);
}

static hal_keyscan_funcs_t g_hal_keyscan_v150_funcs = {
    .init = hal_keyscan_v150_init,
    .deinit = hal_keyscan_v150_deinit,
    .ctrl = hal_keyscan_v150_ctrl
};

hal_keyscan_funcs_t *hal_keyscan_v150_funcs_get(void)
{
    return &g_hal_keyscan_v150_funcs;
}