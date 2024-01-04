/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: RISCV31 idle task config for LiteOS
 * Author: @CompanyNameTag
 * Create: 2021-10-20
 */
#include "idle_config.h"
#include "core.h"
#include "los_task.h"

#include "cmsis_os2.h"

#if CORE == MASTER_BY_ALL
#if ((USE_COMPRESS_LOG_INSTEAD_OF_SDT_LOG == NO) && defined(LIBLOG))
#include "log_oam_msg.h"
#endif
#else
#include "watchdog.h"
#endif
#if (ENABLE_LOW_POWER == YES)
#include "pm_porting.h"
#include "pm_sleep.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

static void idle_task_process(void)
{
#if ((USE_COMPRESS_LOG_INSTEAD_OF_SDT_LOG == NO) && defined(LIBLOG))
#if CORE == MASTER_BY_ALL
    log_oam_prase_message();
#endif
#endif

#if (ENABLE_LOW_POWER == YES)
#if defined(PM_MCPU_MIPS_STATISTICS_ENABLE) && (PM_MCPU_MIPS_STATISTICS_ENABLE == YES)
    pm_record_time_before_sleep();
#endif
    uapi_pm_enter_sleep();
#if defined(PM_MCPU_MIPS_STATISTICS_ENABLE) && (PM_MCPU_MIPS_STATISTICS_ENABLE == YES)
    pm_record_time_after_sleep();
#endif
#endif
}

void idle_task_config(void)
{
    LOS_IdleHandlerHookReg(idle_task_process);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
