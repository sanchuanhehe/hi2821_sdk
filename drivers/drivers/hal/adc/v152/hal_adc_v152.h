/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Provides V152 HAL adc \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-03-30， Create file. \n
 */
#ifndef HAL_ADC_V152_H
#define HAL_ADC_V152_H

#include "hal_adc.h"
#include "hal_adc_v152_regs_op.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_adc_v152 ADC V152
 * @ingroup  drivers_hal_adc
 * @{
 */


/**
 * @if Eng
 * @brief  Get the instance of v152.
 * @return The instance of v152.
 * @else
 * @brief  获取v152实例。
 * @return v152实例。
 * @endif
 */
hal_adc_funcs_t *hal_adc_v152_funcs_get(void);

/**
 * @if Eng
 * @brief  Handler of the adc done interrupt request.
 * @param  [in]  afe_scan_mode AFE mode which is using.
 * @else
 * @brief  adc采样完成中断处理函数。
 * @param  [in]  afe_scan_mode 使用的AFE模式。
 * @endif
 */
void hal_adc_done_irq_handler(afe_scan_mode_t afe_scan_mode);

/**
 * @if Eng
 * @brief  Handler of the adc alarm interrupt request.
 * @param  [in]  afe_scan_mode AFE mode which is using.
 * @else
 * @brief  adc采样告警中断处理函数。
 * @param  [in]  afe_scan_mode 使用的AFE模式。
 * @endif
 */
void hal_adc_alarm_irq_handler(afe_scan_mode_t afe_scan_mode);

/**
 * @if Eng
 * @brief ADC calibration api.
 * @param [in]  afe_scan_mode AFE mode to use.
 * @param [in]  os_cali OS calibration enable.
 * @param [in]  cdac_cali CDAC calibration enable.
 * @param [in]  dcoc_cali DCOC calibration enable(Only enable for HADC).
 * @return errcode_t  Calibration result.
 * @else
 * @brief  adc校准接口。
 * @param  [in]  afe_scan_mode 要校准的AFE模式。
 * @param [in]  os_cali OS 校准开关.
 * @param [in]  cdac_cali CDAC 校准开关.
 * @param [in]  dcoc_cali DCOC 校准开关(仅限高精度ADC).
 * @return errcode_t  校准结果.
 * @endif
 */
errcode_t hal_adc_v152_cali(afe_scan_mode_t afe_scan_mode, bool os_cali, bool cdac_cali, bool dcoc_cali);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif