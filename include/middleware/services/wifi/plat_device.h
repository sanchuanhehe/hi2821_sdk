/**
* @file plat_device.h
*
* Copyright (c) CompanyNameMagicTag 2023-2023.All rights reserved. \n
* Description: header file for plat device api. \n
* Author: CompanyName \n
* History: \n
* 2023-07-29，初始化该文件 \n
*/

#ifndef SERVICE_PLAT_DEVICE_C_H
#define SERVICE_PLAT_DEVICE_C_H

#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @defgroup middleware_service_plat_device PLAT Device
 * @ingroup  middleware_service_plat
 * @{
 */

/**
 * @if Eng
 * @brief  Set pm enable.
 *
 * @param  [in]  pm_switch        Set linkloss parameters. 1: open, 0 close.
 *
 * @retval EXT_ERR_SUCCESS        Execute successfully.
 * @retval Other                  Error code.
 * @else
 * @brief  设置平台低功耗开关。
 *
 * @param  [in]  pm_switch        开关参数。 1: 开启, 0 关闭。
 *
 * @retval EXT_ERR_SUCCESS        成功。
 * @retval Other                  失败。
 * @endif
 */
errcode_t plat_set_pm_mode(int32_t pm_switch);

/**
 * @if Eng
 * @brief  Reset board.
 *
 * @retval ERRCODE_SUCC        Execute successfully.
 * @retval Other               Execute failed.
 * @else
 * @brief  复位单板。
 *
 * @retval ERRCODE_SUCC        成功。
 * @retval Other               失败。
 * @endif
 */
errcode_t plat_reset_board(void);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif // SERVICE_PLAT_DEVICE_C_H
