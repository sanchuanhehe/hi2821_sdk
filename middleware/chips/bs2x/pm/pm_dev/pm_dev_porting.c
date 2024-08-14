/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Provides pm device port \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-01-13， Create file. \n
 */
#include "common_def.h"
#include "msg_chl.h"
#include "non_os.h"
#include "errcode.h"
#include "mpu.h"
#include "watchdog.h"
#include "watchdog_porting.h"
#include "tcxo.h"
#include "pm_dev_porting.h"

#if defined(CONFIG_PM_POWER_GATING_ENABLE)
errcode_t mpu_cache_suspend(uintptr_t arg)
{
    unused(arg);
    return ERRCODE_SUCC;
}

errcode_t mpu_cache_resume(uintptr_t arg)
{
    unused(arg);
    ArchICacheFlush();
    ArchDCacheInvalid();
    ArchICacheEnable(CACHE_4KB);
    ArchICachePrefetchEnable(CACHE_PREF_1_LINES);
    ArchDCacheEnable(CACHE_8KB);
    uapi_tcxo_delay_us(2); // delay 2us
    return ERRCODE_SUCC;
}
#endif

#if defined(CONFIG_WATCHDOG_SUPPORT_LPM)
errcode_t watchdog_resume(uintptr_t arg)
{
    unused(arg);
    /* turnon watchdog's clock for fpga */
    watchdog_turnon_clk();
    non_os_nmi_config(NMI_CWDT, true);
    uapi_watchdog_resume(0);
    return ERRCODE_SUCC;
}
#endif  /* CONFIG_WDT_SUPPORT_LPM */