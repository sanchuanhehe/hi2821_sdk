/**
 * Copyright (c) @CompanyNameMagicTag 2022-2023. All rights reserved. \n
 *
 * Description: Provides V150 HAL qdec \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-09-15, Create file. \n
 */
#include <stdint.h>
#include "debug/osal_debug.h"
#include "common_def.h"
#include "hal_qdec_v150_regs_op.h"
#include "hal_qdec_v150.h"

static qdec_callback_t g_qdec_report = NULL;
static int8_t g_qdec_actual_acc = 0;
static int8_t g_qdec_per_roll = 1;

static void hal_qdec_v150_init(qdec_config_t const *config)
{
    qdec_port_regs_init();
    qdec_port_register_irq();
    hal_qdec_v150_regs_soft_rst();
    hal_qdec_v150_regs_set_sampleper(config->sampleper);

    /* Config LED functions */
    if (config->pselled != QDEC_LED_NOT_CONNECTED) {
        hal_qdec_v150_regs_set_ledpol_en();
        hal_qdec_v150_regs_set_ledpre(config->ledpre);
        hal_qdec_v150_regs_set_ledpol_pol(config->ledpol);
    }

    /* Config debounce filter */
    if (config->defence) {
        hal_qdec_v150_regs_defen_en();
        hal_qdec_v150_regs_defen_num(config->defen_num);
    } else {
        hal_qdec_v150_regs_defen_off();
    }

    /* Enable ready interrupt */
    hal_qdec_v150_regs_set_acc_int_en();
    if (config->reportper != HAL_QDEC_REPORTPER_DISABLED) {
        hal_qdec_v150_regs_set_reportper(config->reportper);
        hal_qdec_v150_regs_set_report_int_en();
        hal_qdec_v150_regs_set_dbl_int_en();
    }
    if (config->sample_inten) {
        hal_qdec_v150_regs_set_sample_int_en();
    }
}

static void hal_qdec_v150_disable(void)
{
    hal_qdec_v150_regs_stop();
    hal_qdec_v150_regs_clr_en();
}

static void hal_qdec_v150_deinit(void)
{
    hal_qdec_v150_disable();
    qdec_port_unregister_irq();
    hal_qdec_v150_regs_deinit();
    qdec_port_unregister_hal_funcs();
    g_qdec_report = NULL;
}

static void hal_qdec_v150_enable(void)
{
    hal_qdec_v150_regs_en();
    hal_qdec_v150_regs_start();
    g_qdec_per_roll = qdec_port_get_acc_per_roll();
}

static void hal_qdec_v150_read_accumulators(int16_t *acc, int16_t *accdbl)
{
    *acc = hal_qdec_v150_regs_get_acc();
    *accdbl = hal_qdec_v150_regs_get_accdbl();
}

static void hal_qdec_event_handler(qdec_event_data_t *event)
{
    if (unlikely(g_qdec_report == NULL) || unlikely(event == NULL)) {
        return;
    }
    g_qdec_report(event->report.acc, NULL);
}

static void hal_qdec_get_acc_equal_roll(qdec_event_data_t *event)
{
    // Some QDEC report one during once roll, others may report two or four.
    // Divide this numbers to make sure roll once will report one.
    hal_qdec_v150_regs_rd_clr_acc();
    g_qdec_actual_acc += hal_qdec_regs_get_accread();
    event->report.acc = g_qdec_actual_acc / g_qdec_per_roll;
    if (g_qdec_actual_acc % g_qdec_per_roll == 0) {
        g_qdec_actual_acc = 0;
    }
}

void hal_qdec_irq_handler(void)
{
    qdec_event_data_t event = { 0 };
    for (uint8_t event_type = 0; event_type < HAL_QDEC_INT_SAMPLERDY_TASK; event_type++) {
        if ((hal_qdec_regs_int_enable_check(event_type) != 0) && (hal_qdec_regs_int_event_check(event_type) != 0)) {
            event.type = event_type;
            if (event_type == HAL_QDEC_INT_REPORTRDY_TASK) {
                hal_qdec_get_acc_equal_roll(&event);
            } else if (event_type == HAL_QDEC_INT_DBLRDY_TASK) {
                hal_qdec_v150_regs_rd_clr_dbl();
                osal_printk("Double transition Error!\r\n");
            }
            hal_qdec_regs_int_clr(event_type);
            hal_qdec_event_handler(&event);
        }
    }
}

static void hal_qdec_register_callback(qdec_callback_t callback)
{
    g_qdec_report = callback;
}

static hal_qdec_funcs_t g_hal_qdec_v150_funcs = {
    .init = hal_qdec_v150_init,
    .deinit = hal_qdec_v150_deinit,
    .enable = hal_qdec_v150_enable,
    .disable = hal_qdec_v150_disable,
    .read_accumulators = hal_qdec_v150_read_accumulators,
    .register_callback = hal_qdec_register_callback,
};

hal_qdec_funcs_t *hal_qdec_v150_get_funcs(void)
{
    return &g_hal_qdec_v150_funcs;
}
