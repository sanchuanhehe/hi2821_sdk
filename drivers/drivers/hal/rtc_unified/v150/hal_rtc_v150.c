/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides V150 HAL rtc \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-12-08, Create file. \n
 */
#include <stdint.h>
#include <stdbool.h>
#include "common_def.h"
#include "hal_rtc_v150_regs_op.h"
#include "rtc_porting.h"
#include "hal_rtc_v150.h"

#if defined(CONFIG_RTC_STOP_DELAY_SUPPORT)
#if defined(CONFIG_RTC_STOP_DELAY_USING_OSAL)
#include "soc_osal.h"
#endif
#if defined(CONFIG_RTC_STOP_DELAY_USING_TCXO)
#include "tcxo.h"
#endif
#if defined(CONFIG_RTC_STOP_DELAY_USING_SYSTICK)
#include "systick.h"
#endif
#endif

#if defined(CONFIG_RTC_START_DELAY_SUPPORT)
#if defined(CONFIG_RTC_START_DELAY_USING_OSAL)
#include "soc_osal.h"
#endif
#if defined(CONFIG_RTC_START_DELAY_USING_TCXO)
#include "tcxo.h"
#endif
#if defined(CONFIG_RTC_START_DELAY_USING_SYSTICK)
#include "systick.h"
#endif
#endif

#define RTC_COUNT_HIGH_32BIT_RIGHT_SHIFT 32
#define RTC_LOAD_COUNT1_LOCK_VALUE 0x5A5A5A5A
#define RTC_CURRENT_COUNT_LOCK_TIMEOUT 0xFFFF

#if defined(CONFIG_RTC_STOP_DELAY_SUPPORT)
#define RTC_STOP_DELAY_US 32
#endif

#if defined(CONFIG_RTC_START_DELAY_SUPPORT)
#define RTC_START_DELAY_US 65
#endif

STATIC hal_rtc_callback_t g_hal_rtc_callback[RTC_MAX_NUM] = {NULL};
STATIC uint32_t g_rtc_int_cnt_record = 0;

STATIC bool g_hal_rtc_width[RTC_MAX_NUM + 1] = {
    CONFIG_RTC_0_WIDTH_64 == 1 ? true : false,
#if defined(CONFIG_RTC_1_WIDTH_64)
    CONFIG_RTC_1_WIDTH_64 == 1 ? true : false,
#endif  /* CONFIG_RTC_1_WIDTH_64 */
#if defined(CONFIG_RTC_2_WIDTH_64)
    CONFIG_RTC_2_WIDTH_64 == 1 ? true : false,
#endif  /* CONFIG_RTC_2_WIDTH_64 */
#if defined(CONFIG_RTC_3_WIDTH_64)
    CONFIG_RTC_3_WIDTH_64 == 1 ? true : false,
#endif  /* CONFIG_RTC_3_WIDTH_64 */
#if defined(CONFIG_RTC_4_WIDTH_64)
    CONFIG_RTC_4_WIDTH_64 == 1 ? true : false,
#endif  /* CONFIG_RTC_4_WIDTH_64 */
#if defined(CONFIG_RTC_5_WIDTH_64)
    CONFIG_RTC_5_WIDTH_64 == 1 ? true : false,
#endif  /* CONFIG_RTC_5_WIDTH_64 */
#if defined(CONFIG_RTC_6_WIDTH_64)
    CONFIG_RTC_6_WIDTH_64 == 1 ? true : false,
#endif  /* CONFIG_RTC_6_WIDTH_64 */
#if defined(CONFIG_RTC_7_WIDTH_64)
    CONFIG_RTC_7_WIDTH_64 == 1 ? true : false,
#endif  /* CONFIG_RTC_7_WIDTH_64 */
    false
};

STATIC errcode_t hal_rtc_v150_init(rtc_index_t index, hal_rtc_callback_t callback)
{
    if ((hal_rtc_regs_init(index)) != ERRCODE_SUCC) {
        return ERRCODE_RTC_REG_ADDR_INVALID;
    }
    g_hal_rtc_callback[index] = callback;
    return ERRCODE_SUCC;
}

STATIC void hal_rtc_v150_deinit(rtc_index_t index)
{
    g_hal_rtc_callback[index] = NULL;
    hal_rtc_regs_deinit(index);
}

STATIC void hal_rtc_v150_start(rtc_index_t index)
{
    hal_rtc_v150_control_reg_set_enable(index, 1);
#if defined(CONFIG_RTC_START_DELAY_SUPPORT)
    /* RTC_V150要求开始后的start信号同步到32Khz，经历了65us的时间 */
#if defined(CONFIG_RTC_START_DELAY_USING_OSAL)
    osal_udelay(RTC_START_DELAY_US);
#endif
#if defined(CONFIG_RTC_START_DELAY_USING_TCXO)
    uapi_tcxo_delay_us(RTC_START_DELAY_US);
#endif
#if defined(CONFIG_RTC_START_DELAY_USING_SYSTICK)
    uapi_systick_delay_us(RTC_START_DELAY_US);
#endif
#endif
}

STATIC void hal_rtc_v150_stop(rtc_index_t index)
{
    hal_rtc_v150_control_reg_set_enable(index, 0);

#if defined(CONFIG_RTC_STOP_DELAY_SUPPORT)
    /* RTC_V150要求停止到再次启动的间隔大于等于一个时钟周期 */
#if defined(CONFIG_RTC_STOP_DELAY_USING_OSAL)
    osal_udelay(RTC_STOP_DELAY_US);
#endif
#if defined(CONFIG_RTC_STOP_DELAY_USING_TCXO)
    uapi_tcxo_delay_us(RTC_STOP_DELAY_US);
#endif
#if defined(CONFIG_RTC_STOP_DELAY_USING_SYSTICK)
    uapi_systick_delay_us(RTC_STOP_DELAY_US);
#endif
#endif
}

STATIC void hal_rtc_v150_config_load(rtc_index_t index, uint64_t delay_count)
{
    hal_rtc_v150_control_reg_set_mode(index, (uint32_t)RTC_V150_MODE_ONE_SHOT);
    hal_rtc_v150_set_load_count0(index, (uint32_t)delay_count);
    if (g_hal_rtc_width[index]) {
        hal_rtc_v150_set_load_lock(index, (uint32_t)RTC_LOAD_COUNT1_LOCK_VALUE);
        hal_rtc_v150_set_load_count1(index, (uint32_t)(delay_count >> RTC_COUNT_HIGH_32BIT_RIGHT_SHIFT));
        hal_rtc_v150_set_load_lock(index, (uint32_t)0);
    }
}

STATIC uint64_t hal_rtc_v150_get_current_value(rtc_index_t index)
{
    uint64_t count = 0;
    uint32_t timeout = 0;

    /*
     * RTC_V150 使能信号无效时读取到的当前COUNT值会保持原值不变，
     * 因此判断enable不为1时认为时钟已到期，返回0。
     */
    if (hal_rtc_v150_control_reg_get_enable(index) != 1) {
        return 0;
    }

    hal_rtc_v150_ctrl_set_cnt_req(index);
    while (timeout < RTC_CURRENT_COUNT_LOCK_TIMEOUT) {
        if (hal_rtc_v150_ctrl_get_cnt_lock(index) == 1) {
            count = (uint64_t)hal_rtc_v150_get_current_value0(index);
            if (g_hal_rtc_width[index]) {
                count += ((uint64_t)hal_rtc_v150_get_current_value1(index) << RTC_COUNT_HIGH_32BIT_RIGHT_SHIFT);
            }
            break;
        }
        timeout++;
    }
    return count;
}

#if defined(CONFIG_RTC_SUPPORT_LPM)
STATIC uint32_t hal_rtc_v150_get_current_int_sts(rtc_index_t index)
{
    return hal_rtc_v150_get_int_sts(index);
}
#endif /* CONFIG_RTC_SUPPORT_LPM */

void hal_rtc_v150_interrupt_clear(rtc_index_t index)
{
    if (index < RTC_MAX_NUM) {
        hal_rtc_v150_int_clr(index);
    }
}

void hal_rtc_v150_irq_handler(rtc_index_t index)
{
    g_rtc_int_cnt_record++;
    hal_rtc_v150_int_clr(index);
    if (g_hal_rtc_callback[index]) {
        g_hal_rtc_callback[index](index);
    }
}

STATIC uint32_t hal_rtc_v150_int_cnt_record_get(void)
{
    return g_rtc_int_cnt_record;
}

STATIC hal_rtc_funcs_t g_hal_rtc_v150_funcs = {
    .init = hal_rtc_v150_init,
    .deinit = hal_rtc_v150_deinit,
    .start = hal_rtc_v150_start,
    .stop = hal_rtc_v150_stop,
    .config_load = hal_rtc_v150_config_load,
    .get_current_count = hal_rtc_v150_get_current_value,
#if defined(CONFIG_RTC_SUPPORT_LPM)
    .get_int_sts = hal_rtc_v150_get_current_int_sts,
#endif /* CONFIG_RTC_SUPPORT_LPM */
    .get_int_cnt_record = hal_rtc_v150_int_cnt_record_get,
};

hal_rtc_funcs_t *hal_rtc_v150_get_funcs(void)
{
    return &g_hal_rtc_v150_funcs;
}
