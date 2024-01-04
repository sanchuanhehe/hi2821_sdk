/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides V100 timer register operation API \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-11-07， Create file. \n
 */
#ifndef HAL_TIMER_V100_REGS_OP_H
#define HAL_TIMER_V100_REGS_OP_H

#include <stdint.h>
#include "errcode.h"
#include "timer_porting.h"
#include "hal_timer_v100_regs_def.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_timer_v100_regs_op TIMER V100 Regs Operation
 * @ingroup  drivers_hal_timer
 * @{
 */

extern uintptr_t g_timer_comm_regs;
extern uintptr_t g_timer_regs[CONFIG_TIMER_MAX_NUM];

/**
 * @brief  Get the value of @ref timer_control_reg_data.enable.
 * @param  [in]  index Index for one of the timer.
 * @return The value of @ref timer_control_reg_data.enable.
 */
static inline uint32_t hal_timer_control_reg_get_enable(timer_index_t index)
{
    timer_control_reg_data_t timer_control_reg;
    timer_control_reg.d32 = ((timer_regs_info_t *)g_timer_regs[index])->control_reg;
    return timer_control_reg.b.enable;
}

/**
 * @brief  Set the value of @ref timer_control_reg_data.enable.
 * @param  [in]  index Index for one of the timer.
 * @param  [in]  val Enable or not.
 */
static inline void hal_timer_control_reg_set_enable(timer_index_t index, uint32_t val)
{
    timer_control_reg_data_t timer_control_reg;
    timer_control_reg.d32 = ((timer_regs_info_t *)g_timer_regs[index])->control_reg;
    timer_control_reg.b.enable = val;
    ((timer_regs_info_t *)g_timer_regs[index])->control_reg = timer_control_reg.d32;
}

/**
 * @brief  Get the value of @ref timer_control_reg_data.mode.
 * @param  [in]  index Index for one of the timer.
 * @return The value of @ref timer_control_reg_data.mode.
 */
static inline uint32_t hal_timer_control_reg_get_mode(timer_index_t index)
{
    timer_control_reg_data_t timer_control_reg;
    timer_control_reg.d32 = ((timer_regs_info_t *)g_timer_regs[index])->control_reg;
    return timer_control_reg.b.mode;
}

/**
 * @brief  Set the value of @ref timer_control_reg_data.mode.
 * @param  [in]  index Index for one of the timer.
 * @param  [in]  val The work mode of timer.
 */
static inline void hal_timer_control_reg_set_mode(timer_index_t index, uint32_t val)
{
    timer_control_reg_data_t timer_control_reg;
    timer_control_reg.d32 = ((timer_regs_info_t *)g_timer_regs[index])->control_reg;
    timer_control_reg.b.mode = val;
    ((timer_regs_info_t *)g_timer_regs[index])->control_reg = timer_control_reg.d32;
}

/**
 * @brief  Get the value of @ref timer_control_reg_data.int_mask.
 * @param  [in]  index Index for one of the timer.
 * @return The value of @ref timer_control_reg_data.int_mask.
 */
static inline uint32_t hal_timer_control_reg_get_int_mask(timer_index_t index)
{
    timer_control_reg_data_t timer_control_reg;
    timer_control_reg.d32 = ((timer_regs_info_t *)g_timer_regs[index])->control_reg;
    return timer_control_reg.b.int_mask;
}

/**
 * @brief  Set the value of @ref timer_control_reg_data.int_mask.
 * @param  [in]  index Index for one of the timer.
 * @param  [in]  val Open int mask or not.
 */
static inline void hal_timer_control_reg_set_int_mask(timer_index_t index, uint32_t val)
{
    timer_control_reg_data_t timer_control_reg;
    timer_control_reg.d32 = ((timer_regs_info_t *)g_timer_regs[index])->control_reg;
    timer_control_reg.b.int_mask = val;
    ((timer_regs_info_t *)g_timer_regs[index])->control_reg = timer_control_reg.d32;
}

/**
 * @brief  Get the value of @ref timer_eoi_data.eoi.
 * @return The value of @ref timer_eoi_data.eoi.
 */
static inline uint32_t hal_timer_get_eoi(void)
{
    timer_eoi_data_t timer_eoi;
    timer_eoi.d32 = ((timer_regs_t *)g_timer_comm_regs)->eoi;
    return timer_eoi.b.eoi;
}

/**
 * @brief  Get the value of @ref timer_int_status_data.int_status.
 * @return The value of @ref timer_int_status_data.int_status.
 */
static inline uint32_t hal_timer_get_int_status(void)
{
    timer_int_status_data_t timer_int_status;
    timer_int_status.d32 = ((timer_regs_t *)g_timer_comm_regs)->int_status;
    return timer_int_status.b.int_status;
}

/**
 * @brief  Get the value of @ref timer_regs_info.load_count.
 * @param  [in]  index Index for one of the timer.
 * @return The value of @ref timer_regs_info.load_count.
 */
static inline uint32_t hal_timer_load_count_get(timer_index_t index)
{
    return ((timer_regs_info_t *)g_timer_regs[index])->load_count;
}

/**
 * @brief  Set the value of @ref timer_regs_info.load_count.
 * @param  [in]  index Index for one of the timer.
 * @param  [in]  val The load count value.
 */
static inline void hal_timer_load_count_set(timer_index_t index, uint32_t val)
{
    ((timer_regs_info_t *)g_timer_regs[index])->load_count = val;
}

/**
 * @brief  Get the value of @ref timer_regs_info.current_value.
 * @param  [in]  index Index for one of the timer.
 * @return The value of @ref timer_regs_info.current_value.
 */
static inline uint32_t hal_timer_get_current_value(timer_index_t index)
{
    return ((timer_regs_info_t *)g_timer_regs[index])->current_value;
}

/**
 * @brief  Get the value of @ref timers_regs.int_status.
 * @return The value of @ref timers_regs.int_status.
 */
static inline uint32_t hal_timer_timers_get_int_status(void)
{
    return ((timer_regs_t *)g_timer_comm_regs)->int_status;
}

/**
 * @brief  Get the value of @ref timers_regs.eoi.
 * @return The value of @ref timers_regs.eoi.
 */
static inline uint32_t hal_timer_timers_get_eoi(void)
{
    return ((timer_regs_t *)g_timer_comm_regs)->eoi;
}

/**
 * @brief  Get the value of @ref timers_regs.raw_int_status.
 * @return The value of @ref timers_regs.raw_int_status.
 */
static inline uint32_t hal_timer_timers_raw_int_status_get(void)
{
    return ((timer_regs_t *)g_timer_comm_regs)->raw_int_status;
}

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
