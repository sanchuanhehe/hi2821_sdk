/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides HAL xip \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-09-13, Create file. \n
 */
#ifndef HAL_XIP_H
#define HAL_XIP_H

#include <stdint.h>
#include <stdbool.h>
#include "hal_xip_v150_regs_op.h"
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_xip_api XIP
 * @ingroup  drivers_hal_xip
 * @{
 */

/**
 * @if Eng
 * @brief  Definition of the mode ID of hal xip.
 * @else
 * @brief  XIP模式定义
 * @endif
 */
typedef enum {
    XIP_MODE_DISABLE = 0,                 /*!< @if Eng disable mode.  @else  禁用模式 @endif */
    XIP_MODE_NORMAL = 1,                  /*!< @if Eng normal mode.  @else  正常模式 @endif */
    XIP_MODE_BYPASS = 2,                  /*!< @if Eng bypass mode.  @else  旁路模式 @endif */
} xip_mode_t;

/**
 * @if Eng
 * @brief  Definition of the div of hal xip.
 * @else
 * @brief  XIP时钟分频比
 * @endif
 */
typedef enum {
    XIP_DIV_1 = 1,
    XIP_DIV_2 = 2,
    XIP_DIV_3 = 3,
    XIP_DIV_4 = 4,
    XIP_DIV_5 = 5,
    XIP_DIV_6 = 6,
    XIP_DIV_7 = 7,
    XIP_DIV_8 = 0,
    XIP_DIV_MAX = 8,
} xip_div_t;

/**
 * @if Eng
 * @brief  Definition of the intr of hal xip.
 * @else
 * @brief  XIP中断类型
 * @endif
 */
typedef enum {
    XIP_INTR_NMI = 0,                   /*!< @if Eng NMI.  @else  NMI 中断 @endif */
    XIP_INTR_HARDFAULT = 1,             /*!< @if Eng HardFault.  @else  HardFault 中断 @endif */
} xip_intr_types_t;

/**
 * @if Eng
 * @brief  Definition of the id of hal xip.
 * @else
 * @brief  XIP ID
 * @endif
 */
typedef enum xip_id {
    XIP_0,
    XIP_1,
    XIP_MAX
} xip_id_t;

/**
 * @if Eng
 * @brief  Definition of the cache of hal xip.
 * @else
 * @brief  XIP缓存统计类型
 * @endif
 */
typedef struct {
    uint64_t hit_count;                  /*!< @if Eng Xip cache hiting count.
                                         @else   HAL层XIP缓存统计命中数 @endif */
    uint64_t miss_count;                 /*!< @if Eng Xip cache missing count.
                                         @else   HAL层XIP缓存统计丢失数 @endif */
    uint64_t total_count;                /*!< @if Eng Xip cache total count.
                                         @else   HAL层XIP缓存统计总数 @endif */
} xip_cache_count_t;

/**
 * @if Eng
 * @brief  Initialize device for hal xip.
 * @else
 * @brief  HAL层XIP的初始化接口.
 * @endif
 */
void hal_xip_init(void);

/**
 * @if Eng
 * @brief  Deinitialize device for hal xip.
 * @else
 * @brief  HAL层XIP的去初始化接口.
 * @endif
 */
void hal_xip_deinit(void);

/**
 * @if Eng
 * @brief  Enable xip function.
 * @param  [in]  xip_index USE xip0 or xip1.
 * @param  [in]  xip_mode_code Region of xip mode.
 * @param  [in]  enable_addr_32bit Use 32bits address or 24bits address.
 * @param  [in]  enable_wrap Support EFLASH WRAP or not.
 * @param  [in]  cmd_mode Support CMD or not.
 * @else
 * @brief  HAL层XIP的使能接口
 * @param  [in]  xip_index 使用XIP0或者XIP1.
 * @param  [in]  xip_mode_code xip模式的区域.
 * @param  [in]  enable_addr_32bit 使用32位地址或24位地址.
 * @param  [in]  enable_wrap 是否支持EFLASH封装.
 * @param  [in]  cmd_mode 是否支持CMD.
 * @endif
 */
void hal_xip_enable(xip_id_t xip_index, uint8_t xip_mode_code, bool enable_addr_32bit, bool enable_wrap, bool cmd_mode);

/**
 * @if Eng
 * @brief  Disable xip function.
 * @param  [in]  xip_index USE xip0 or xip1.
 * @else
 * @brief  HAL层XIP的去使能
 * @param  [in]  xip_index 使用XIP0或者XIP1.
 * @endif
 */
void hal_xip_disable(xip_id_t xip_index);

/**
 * @if Eng
 * @brief  Query xip enable state.
 * @param  [in]  xip_index USE xip0 or xip1.
 * @retval true  enable, false disable.
 * @else
 * @brief  HAL层XIP的使能状态
 * @param  [in]  xip_index 使用XIP0或者XIP1.
 * @retval true  使能, false 未使能
 * @endif
 */
bool hal_xip_is_enable(xip_id_t xip_index);

/**
 * @if Eng
 * @brief  Clear xip tag.
 * @else
 * @brief  XIP CACHE全地址的INVALID请求
 * @endif
 */
void hal_xip_clear_tag(void);

/**
 * @if Eng
 * @brief  Xip soft reset.
 * @else
 * @brief  HAL层XIP软复位
 * @endif
 */
void hal_xip_soft_rst(void);

/**
 * @if Eng
 * @brief  Xip cache missing statistic enable.
 * @else
 * @brief  HAL层XIP缓存丢失统计功能使能
 * @endif
 */
void hal_xip_cache_miss_en(void);

/**
 * @if Eng
 * @brief  Xip cache missing statistic load lock.
 * @else
 * @brief  HAL层XIP缓存统计锁存
 * @endif
 */
void hal_xip_cache_miss_load_lock(void);

/**
 * @if Eng
 * @brief  Xip cache total count.
 * @retval Cache total count.
 * @else
 * @brief  HAL层XIP缓存统计总数
 * @retval 缓存总数
 * @endif
 */
uint64_t hal_xip_get_cache_total_count(void);

/**
 * @if Eng
 * @brief  Xip cache hiting count.
 * @retval Cache hit count.
 * @else
 * @brief  HAL层XIP缓存统计命中数
 * @retval 缓存命中数
 * @endif
 */
uint64_t hal_xip_get_cache_hit_count(void);

/**
 * @if Eng
 * @brief  Xip cache missing count.
 * @retval Cache missing count.
 * @else
 * @brief  HAL层XIP缓存统计丢失数
 * @retval 缓存丢失数
 * @endif
 */
uint64_t hal_xip_get_cache_miss_count(void);

/**
 * @if Eng
 * @brief  Get xip cache count statistic infomation.
 * @param  [out]  cache_count The xip cache statistic infomation.
 * @else
 * @brief  HAL层XIP的缓存统计
 * @param  [out]  cache_count 缓存统计信息.
 * @endif
 */
void hal_xip_get_cache_count(xip_cache_count_t *cache_count);

/**
 * @if Eng
 * @brief  Enable xip opi mode.
 * @param  [in]  dummy_cycle The write redundant cycles.
 * @else
 * @brief  HAL层XIP使能opi模式
 * @param  [in]  dummy_cycle 写入冗余周期.
 * @endif
 */
void hal_xip_opi_enable(uint8_t dummy_cycle);

/**
 * @if Eng
 * @brief  Disable xip opi mode.
 * @else
 * @brief  HAL层XIP去使能opi模式
 * @endif
 */
void hal_xip_opi_disable(void);

/**
 * @if Eng
 * @brief  Mask xip ctrl error response.
 * @param  [in]  xip_index USE xip0 or xip1.
 * @param  [in]  mask To mask the error response or not.
 * @else
 * @brief  HAL层XIP屏蔽ctrl错误响应
 * @param  [in]  xip_index 使用XIP0或者XIP1.
 * @param  [in]  mask 是否屏蔽错误响应
 * @endif
 */
void hal_xip_mask_ctrl_error_resp(xip_id_t xip_index, bool mask);

/**
 * @if Eng
 * @brief  Mask xip cache error response.
 * @param  [in]  mask To mask the error response or not.
 * @else
 * @brief  HAL层XIP屏蔽cache错误响应
 * @param  [in]  mask 是否屏蔽错误响应
 * @endif
 */
void hal_xip_mask_cache_error_resp(bool mask);

/**
 * @if Eng
 * @brief  divide xip clk.
 * @param  [in]  bus The xip bus to choose.
 * @param  [in]  div The div ratio.
 * @else
 * @brief  HAL层XIP时钟分频
 * @param  [in]  bus xip bus选择.
 * @param  [in]  div 时钟分频.
 * @endif
 */
void hal_xip_set_div(xip_bus_t bus, xip_div_t div);

/**
 * @if Eng
 * @brief  Get xip clk divide.
 * @param  [in]  bus The xip bus to choose.
 * @retval The div ratio.
 * @else
 * @brief  获取xip时钟分频
 * @param  [in]  bus xip bus选择
 * @retval xip时钟分频
 * @endif
 */
uint8_t hal_xip_get_div(xip_bus_t bus);

/**
 * @if Eng
 * @brief  Set current xip mode.
 * @param  [in]  xip_index USE xip0 or xip1.
 * @param  [in]  mode The mode to set.
 * @else
 * @brief  HAL层设置xip模式
 * @param  [in]  xip_index 使用XIP0或者XIP1.
 * @param  [in]  mode xip模式
 * @endif
 */
void hal_xip_set_cur_mode(xip_id_t index, xip_mode_t mode);

/**
 * @if Eng
 * @brief  Get current xip mode.
 * @param  [in]  xip_index USE xip0 or xip1.
 * @retval Current xip mode.
 * @else
 * @brief  HAL层获取xip模式
 * @param  [in]  xip_index 使用XIP0或者XIP1.
 * @retval 当前xip模式
 * @endif
 */
xip_mode_t hal_xip_get_cur_mode(xip_id_t index);

/**
 * @if Eng
 * @brief  Config xip interrupt type.
 * @param  [in]  xip_index USE xip0 or xip1.
 * @param  [in]  types NMI or HardFault.
 * @else
 * @brief  HAL层设置xip中断类型
 * @param  [in]  xip_index 使用XIP0或者XIP1.
 * @param  [in]  types 不可屏蔽中断或者硬故障.
 * @endif
 */
void hal_xip_config_interrupt_type(xip_id_t xip_index, xip_intr_types_t types);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
