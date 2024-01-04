/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Provide HAL drv rtc \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-03-02, Create file. \n
 */
#include "common_def.h"
#include "errcode.h"
#include "hal_rtc.h"

uintptr_t g_rtc_comm_regs = NULL;
uintptr_t g_rtc_regs[CONFIG_RTC_MAX_NUM] = {NULL};
static hal_rtc_funcs_t *g_hal_rtc_funcs[CONFIG_RTC_MAX_NUM] = {NULL};

errcode_t hal_rtc_regs_init(rtc_index_t index)
{
    if (unlikely(rtc_porting_base_addr_get(index) == 0)) {
        return ERRCODE_RTC_REG_ADDR_INVALID;
    }

    g_rtc_regs[index] = rtc_porting_base_addr_get(index);
    g_rtc_comm_regs = rtc_porting_comm_addr_get();

    return ERRCODE_SUCC;
}

void hal_rtc_regs_deinit(rtc_index_t index)
{
    g_rtc_regs[index] = NULL;
}

errcode_t hal_rtc_register_funcs(rtc_index_t index, hal_rtc_funcs_t *funcs)
{
    if (unlikely(funcs == NULL)) {
        return ERRCODE_INVALID_PARAM;
    }

    g_hal_rtc_funcs[index] = funcs;
    return ERRCODE_SUCC;
}

void hal_rtc_unregister_funcs(rtc_index_t index)
{
    g_hal_rtc_funcs[index] = NULL;
}

hal_rtc_funcs_t *hal_rtc_get_funcs(rtc_index_t index)
{
    return g_hal_rtc_funcs[index];
}