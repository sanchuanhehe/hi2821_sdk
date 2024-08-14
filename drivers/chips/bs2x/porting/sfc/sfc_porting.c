/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides sfc port template \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-11-30， Create file. \n
 */
#include "hal_sfc_v150.h"
#ifndef BUILD_NOOSAL
#include "soc_osal.h"
#endif
#include "sfc_porting.h"

static uint32_t g_sfc_delay_once_us = SFC_DELAY_ONCE_US;
static uint32_t g_sfc_delay_times = SFC_DELAY_TIMES;

#ifndef BUILD_NOOSAL
static osal_mutex g_sfc_mutex = { NULL };
static bool g_sfc_mutex_inited = false;
#endif

uint32_t sfc_port_get_delay_times(void)
{
    return (uintptr_t)g_sfc_delay_times;
}

void sfc_port_set_delay_times(uint32_t delay_times)
{
    g_sfc_delay_times = delay_times;
}

void sfc_port_set_delay_once_time(uint32_t delay_us)
{
    g_sfc_delay_once_us = delay_us;
}

uint32_t sfc_port_get_delay_once_time(void)
{
    return (uintptr_t)g_sfc_delay_once_us;
}

void sfc_port_lock_init(void)
{
#ifndef BUILD_NOOSAL
    if (!g_sfc_mutex_inited) {
        osal_mutex_init(&g_sfc_mutex);
        g_sfc_mutex_inited = true;
    }
#endif
}

uint32_t sfc_port_lock(void)
{
#ifndef BUILD_NOOSAL
    if (g_sfc_mutex_inited && !osal_in_interrupt()) {
        return osal_mutex_lock_timeout(&g_sfc_mutex, OSAL_MUTEX_WAIT_FOREVER);
    }
#endif
    return ERRCODE_SUCC;
}

void sfc_port_unlock(uint32_t lock_sts)
{
    unused(lock_sts);
#ifndef BUILD_NOOSAL
    if (g_sfc_mutex_inited && !osal_in_interrupt()) {
        osal_mutex_unlock(&g_sfc_mutex);
    }
#endif
}