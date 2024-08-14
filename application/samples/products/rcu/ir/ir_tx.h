/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: IR TX header \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-10-10, Create file. \n
 */
#ifndef IR_TX_H
#define IR_TX_H

#include <stdint.h>
#include "pinctrl.h"
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @if Eng
 * @brief  IR init port.
 * @param  [in] gpio IO, see @ref pin_t.
 * @else
 * @brief  红外初始化接口。
 * @param  [in] gpio IO， 参考 @ref pin_t 。
 * @endif
 */
void ir_init(pin_t gpio);

/**
 * @if Eng
 * @brief  IR TX port.
 * @param  [in] freq The IR carrier frequency in Hertz.
 * @param  [in] pattern The alternating on/off pattern in microseconds to transmit.
 * @param  [in] len Length of the pattern array.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t.
 * @else
 * @brief  红外发送接口。
 * @param  [in] freq IR载波频率（以赫兹为单位）。
 * @param  [in] pattern 要传输的交替高低电平（以微秒为单位）。
 * @param  [in] len pattern数组的长度。
 * @retval ERRCODE_SUCC   成功。
 * @retval Other          失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t ir_transmit(int freq, int *pattern, int len);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */


#endif