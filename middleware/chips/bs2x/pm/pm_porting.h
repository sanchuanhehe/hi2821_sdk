/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Provides pm port \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-01-12， Create file. \n
 */
#ifndef PM_PORTING_H
#define PM_PORTING_H

#include "systick.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup middleware_chips_pm_port PM port
 * @ingroup  middleware_chips_pm
 * @{
 */

#define PM_GET_CURRENT_MS   uapi_systick_get_ms()

void pm_record_time_before_sleep(void);

void pm_record_time_after_sleep(void);

uint32_t pm_get_time_before_sleep(void);

uint32_t pm_get_total_work_time(void);

uint32_t pm_get_time_after_sleep(void);

uint32_t pm_get_total_idle_time(void);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif