/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description:  \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-03-02, Create file. \n
 */
#ifndef HAL_RTC_H
#define HAL_RTC_H

#include <stdint.h>
#include "errcode.h"
#include "rtc_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_rtc_api RTC
 * @ingroup  drivers_hal_rtc
 * @{
 */

/**
 * @if Eng
 * @brief  Callback of RTC.
 * @param  [in]  index Index of the hardware rtc. For detail, see @ref rtc_index_t.
 * @else
 * @brief  RTC的回调函数。
 * @param  [in]  index 硬件定时器索引值，参考 @ref rtc_index_t 。
 * @endif
 */
typedef void (*hal_rtc_callback_t)(rtc_index_t index);

/**
 * @if Eng
 * @brief  HAL RTC initialize interface.
 * @param  [in]  index Index of the hardware rtc. For detail, see @ref rtc_index_t.
 * @param  [in]  callback Callback of RTC.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t
 * @else
 * @brief  HAL层RTC的初始化接口。
 * @param  [in]  index 硬件定时器索引值，参考 @ref rtc_index_t 。
 * @param  [in]  callback RTC的回调函数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
typedef errcode_t (*hal_rtc_init_t)(rtc_index_t index, hal_rtc_callback_t callback);

/**
 * @if Eng
 * @brief  HAL RTC deinitialize interface.
 * @param  [in]  index Index of the hardware rtc. For detail, see @ref rtc_index_t.
 * @else
 * @brief  HAL层RTC的去初始化接口。
 * @param  [in]  index 硬件定时器索引值，参考 @ref rtc_index_t 。
 * @endif
 */
typedef void (*hal_rtc_deinit_t)(rtc_index_t index);

/**
 * @if Eng
 * @brief  HAL RTC start the load count of hardware RTC interface.
 * @param  [in]  index Index of low layer RTC. For detail, see @ref rtc_index_t.
 * @else
 * @brief  HAL层启动硬件定时器计数的接口。
 * @param  [in]  index RTC底层索引值，参考 @ref rtc_index_t 。
 * @endif
 */
typedef void (*hal_rtc_start_t)(rtc_index_t index);

/**
 * @if Eng
 * @brief  HAL RTC stop the load count of hardware RTC interface.
 * @param  [in]  index Index of low layer RTC. For detail, see @ref rtc_index_t.
 * @else
 * @brief  HAL层停止硬件定时器计数的接口。
 * @param  [in]  index rtc底层索引值，参考 @ref rtc_index_t 。
 * @endif
 */
typedef void (*hal_rtc_stop_t)(rtc_index_t index);

/**
 * @if Eng
 * @brief  HAL RTC set the load count of hardware RTC interface.
 * @param  [in]  index Index of low layer RTC. For detail, see @ref rtc_index_t.
 * @param  [in]  delay_count Time for load count.
 * @else
 * @brief  HAL层设置硬件计时器计数的接口。
 * @param  [in]  index rtc底层索引值，参考 @ref rtc_index_t 。
 * @param  [in]  delay_count 计时的时间。
 * @endif
 */
typedef void (*hal_rtc_set_load_count_t)(rtc_index_t index, uint64_t delay_count);

/**
 * @if Eng
 * @brief  HAL RTC get the current value of hardware RTC interface.
 * @param  [in]  index Index of low layer RTC. For detail, see @ref rtc_index_t.
 * @return RTC load count.
 * @else
 * @brief  HAL层获取硬件当时计时器剩余计数的接口。
 * @param  [in]  index rtc底层索引值，参考 @ref rtc_index_t 。
 * @return rtc计数值。
 * @endif
 */
typedef uint64_t (*hal_rtc_get_current_count_t)(rtc_index_t index);

/**
 * @if Eng
 * @brief  HAL RTC get the count of RTC IRQ.
 * @return RTC irq count.
 * @else
 * @brief  HAL层获取RTC发生中断的次数的值。
 * @return rtc中断计数值。
 * @endif
 */
typedef uint32_t (*hal_rtc_get_int_cnt_record_t)(void);

/**
 * @if Eng
 * @brief  HAL RTC get the current interrupt status of hardware RTC interface.
 * @param  [in]  index Index of low layer RTC. For detail, see @ref rtc_index_t.
 * @return RTC load count.
 * @else
 * @brief  HAL层获取硬件当时计时器中断状态。
 * @param  [in]  index rtc底层索引值，参考 @ref rtc_index_t 。
 * @return rtc计数值。
 * @endif
 */
typedef uint32_t (*hal_rtc_get_int_sts_t)(rtc_index_t index);

/**
 * @if Eng
 * @brief  Functions interface between RTC driver and RTC HAL.
 * @else
 * @brief  Driver层rtc和HAL层rtc的函数接口。
 * @endif
 */
typedef struct {
    hal_rtc_init_t                 init;                       /*!< @if Eng Init RTC interface.
                                                                      @else   HAL层RTC的初始化接口。 @endif */
    hal_rtc_deinit_t               deinit;                     /*!< @if Eng Deinit RTC interface.
                                                                      @else   HAL层RTC去初始化接口。 @endif */
    hal_rtc_start_t                start;                      /*!< @if Eng Start RTC interface.
                                                                      @else   HAL层RTC启动接口。 @endif */
    hal_rtc_stop_t                 stop;                       /*!< @if Eng Stop RTC interface.
                                                                      @else   HAL层RTC停止接口。 @endif */
    hal_rtc_set_load_count_t       config_load;                /*!< @if Eng Config init interface.
                                                                      @else   HAL层RTC载入计数值并使能。 @endif */
    hal_rtc_get_current_count_t    get_current_count;          /*!< @if Eng get load count.
                                                                      @else   HAL层RTC获取计数值。 @endif */
#if defined(CONFIG_RTC_SUPPORT_LPM)
    hal_rtc_get_int_sts_t          get_int_sts;                /*!< @if Eng get current interrupt status.
                                                                      @else   HAL层RTC获取中断状态。 @endif */
#endif /* CONFIG_RTC_SUPPORT_LPM */
    hal_rtc_get_int_cnt_record_t   get_int_cnt_record;         /*!< @if Eng get irq count.
                                                                      @else   HAL层RTC获取中断计数值。 @endif */
} hal_rtc_funcs_t;

/**
 * @if Eng
 * @brief  Register @ref hal_rtc_funcs_t into the g_hal_rtcs_funcs.
 * @param  [in]  index Index of low layer RTC. For detail, see @ref rtc_index_t.
 * @param  [in]  funcs Interface between RTC driver and RTC HAL.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t.
 * @else
 * @brief  注册 @ref hal_rtc_funcs_t 到 g_hal_rtcs_funcs
 * @param  [in]  index rtc底层索引值，参考 @ref rtc_index_t 。
 * @param  [in]  funcs Driver层RTC和HAL层RTC的接口实例。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t hal_rtc_register_funcs(rtc_index_t index, hal_rtc_funcs_t *funcs);

/**
 * @if Eng
 * @brief  Unregister @ref hal_rtc_funcs_t from the g_hal_rtcs_funcs.
 * @param  [in]  index Index of low layer RTC. For detail, see @ref rtc_index_t.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t.
 * @else
 * @brief  从g_hal_rtcs_funcs注销 @ref hal_rtc_funcs_t 。
 * @param  [in]  index rtc底层索引值，参考 @ref rtc_index_t 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
void hal_rtc_unregister_funcs(rtc_index_t index);

/**
 * @if Eng
 * @brief  Get interface between RTC driver and RTC HAL. For detail, see @ref hal_rtc_funcs_t.
 * @param  [in]  index Index of low layer RTC. For detail, see @ref rtc_index_t.
 * @return Interface between RTC driver and RTC HAL. For detail, see @ref hal_rtc_funcs_t.
 * @else
 * @brief  获取Driver层rtc和HAL层RTC的接口实例，参考 @ref hal_rtc_funcs_t 。
 * @param  [in]  index rtc底层索引值，参考 @ref rtc_index_t 。
 * @return Driver层rtc和HAL层RTC的接口实例，参考 @ref hal_rtc_funcs_t 。
 * @endif
 */
hal_rtc_funcs_t *hal_rtc_get_funcs(rtc_index_t index);

/**
 * @if Eng
 * @brief  Init the RTC which will set the base address of registers.
 * @param  [in]  index Index of low layer RTC. For detail, see @ref rtc_index_t.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t.
 * @else
 * @brief  初始化rtc，设置寄存器的基地址。
 * @param  [in]  index rtc底层索引值，参考 @ref rtc_index_t 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t hal_rtc_regs_init(rtc_index_t index);

/**
 * @if Eng
 * @brief  Deinit the hal_drv_rtc which will clear the base address of registers has been
 *         set by @ref hal_rtc_regs_init.
 * @param  [in]  index Index of low layer RTC. For detail, see @ref rtc_index_t.
 * @else
 * @brief  去初始化，然后清除在 @ref hal_rtc_regs_init 中设置的寄存器地址。
 * @param  [in]  index rtc底层索引值，参考 @ref rtc_index_t 。
 * @endif
 */
void hal_rtc_regs_deinit(rtc_index_t index);


/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif