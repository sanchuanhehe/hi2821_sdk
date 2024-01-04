/******************************************************************************
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: runtime adapter \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-04-10, Create file. \n
 ******************************************************************************/

#include "cmsis_os2.h"
#include "los_builddef.h"
#include "los_printf.h"
#include "los_base.h"
#include "los_memory.h"
#include "los_sys.h"
#include "runtime_monitor.h"
#ifdef LIBLOG
//lint -efile( 766, log_def.h)
#include "log_common.h"
#include "log_def.h"
#include "log_printf.h"
#endif

INT32 g_wSysStatus = SYS_OS_READY;
osTimerId_t g_statusCheckTimerId = NULL;
static volatile uint32_t g_workCount = 0;
#define BIT(n)              (1ul << ((UINT32)(n)))

typedef struct tagStatus {
    BOOL bNeedCheck;
    UINT64 uwRunTick;
    UINT32 uwTickThreshold;
    UINT32 uwBlockCount;
} runtime_status_cb_s;

static runtime_status_cb_s g_statusCB[MON_TASK_END] = {
#if (CORE == APPS)
   /* app task runtime monitor begin */
    {true, 0, 2000, 0},      /* rpc_task */
    {true, 0, 2000, 0},      /* aging_test_task */
    {true, 0, 2000, 0},      /* app_task */
    {true, 0, 2000, 0},      /* basic_sensor_task */
    {true, 0, 2000, 0},      /* commu_task */
    {true, 0, 2000, 0},      /* device_misc_task */
    {true, 0, 2000, 0},      /* forward_task */
    {true, 0, 2000, 0},      /* file_tran_task */
    {true, 0, 2000, 0},      /* protocol_task */
    {true, 0, 2000, 0},      /* gesture_task */
    {true, 0, 2000, 0},      /* maintenance_task */
    {true, 0, 2000, 0},      /* manufacture_task */
    {true, 0, 2000, 0},      /* power_task */
    {true, 0, 2000, 0},      /* sys_task */
    {true, 0, 2000, 0},      /* ux_task */
    {true, 0, 2000, 0},      /* wear_task */
    {true, 0, 2000, 0},      /* bt_task */
    {true, 0, 2000, 0},      /* audio_thread_task */
    {true, 0, 2000, 0},      /* sensors_data_task */
#if (APP_BTN_TASK_MONITOR_ENABLE == YES)
    {true, 0, 2000, 0},      /* button_task */
#endif
    {true, 0, 1500, 0},      /* log_task */
    /* app task runtime monitor end */
#else
    {false, 0, 500, 0},      /* bt_demo */
    {true, 0, 50,   0},       /* rpc_task */
    {true, 0, 50,  0},       /* btc_task */
    {true, 0, 500, 0},       /* bth_init_task */
    {true, 0, 500, 0},       /* bth_rpc_task */
    {true, 0, 500, 0},       /* bth_gdk_task */
    {true, 0, 500, 0},       /* bth_sdk_task */
    {true, 0, 500, 0},       /* bth_recv_msg_task */
#if (BTH_WITH_SMART_WEAR == YES)
    {true, 0, 500, 0},       /* bt_tran_task */
    {true, 0, 500, 0},       /* a2dp_service_task, set time threshold 500ms */
#endif /* BTH_WITH_SMART_WEAR */
#endif
};

/**
* @ingroup osMonitor
* @brief check all task work normal or not, if one task was blocked, warn
* @param [input] void
* @retval void
*/
void statuscheck_timer_callback(void)
{
    UINT32 uwIndex;
    UINT32 uwBlockTime;

    if (SYS_READY != g_wSysStatus) {
        return;
    }

#if (CORE == APPS)
    SysProcCallback();
#endif

    for (uwIndex = MON_TASK_BEGIN; uwIndex < MON_TASK_END; uwIndex++) {
        if ((g_statusCB[uwIndex].bNeedCheck) && ((unsigned)g_workCount & BIT(uwIndex))) {
            uwBlockTime = (uint32_t)(LOS_TickCountGet() - g_statusCB[uwIndex].uwRunTick);
            if (uwBlockTime >= g_statusCB[uwIndex].uwTickThreshold) {
                g_statusCB[uwIndex].uwBlockCount = g_statusCB[uwIndex].uwBlockCount + 1;
#ifdef LIBLOG
                oml_pf_log_print3(LOG_BCORE_PLT_INFO_OS, LOG_NUM_INFO_OS, LOG_LEVEL_ERROR, \
                                  "[osMonitor]No.%d task was blocked tick %ld, count %ld\r\n", \
                                  uwIndex, uwBlockTime, g_statusCB[uwIndex].uwBlockCount);
#endif
            } else {
                g_statusCB[uwIndex].uwBlockCount = 0;
            }

            if (g_statusCB[uwIndex].uwBlockCount >= STATUS_CHECK_MAX_FAIL_COUNT) {
#ifdef LIBLOG
                oml_pf_log_print1(LOG_BCORE_PLT_INFO_OS, LOG_NUM_INFO_OS, LOG_LEVEL_ERROR,
                                  "[osMonitor]No.%d task was blocked up to limited times!\r\n", uwIndex);
#endif
            }
        }
    }
}

void RuntimeMonitorTimerStop(void)
{
    (VOID)osTimerStop(g_statusCheckTimerId);
}

void RuntimeMonitorTimerStart(void)
{
    (VOID)osTimerStart(g_statusCheckTimerId, STATUS_CHECK_INTERVAL);
}

LITE_OS_SEC_TEXT_INIT void runtime_monitor_init(void)
{
    g_statusCheckTimerId = osTimerNew((osTimerFunc_t)statuscheck_timer_callback, osTimerPeriodic, NULL, NULL);
    if (g_statusCheckTimerId == NULL) {
        return;
    }

    /* Start the StatusCheck timer */
    (VOID)osTimerStart(g_statusCheckTimerId, STATUS_CHECK_INTERVAL);
}


VOID osTskMonBegin(UINT32 uwMailID)
{
    if (uwMailID < (UINT32)MON_TASK_END) {
        g_workCount |= BIT(uwMailID);
        g_statusCB[uwMailID].uwRunTick = LOS_TickCountGet();
    }
}

VOID osTskMonEnd(UINT32 uwMailID)
{
    UINT32 uwWorkTime;
    if (uwMailID < (UINT32)MON_TASK_END) {
        g_workCount &= ~BIT(uwMailID);/*lint !e502*/
        uwWorkTime = (uint32_t)(LOS_TickCountGet() - g_statusCB[uwMailID].uwRunTick);
        if (uwWorkTime >= g_statusCB[uwMailID].uwTickThreshold) {
#ifdef LIBLOG
            oml_pf_log_print3(LOG_BCORE_PLT_INFO_OS, LOG_NUM_INFO_OS, LOG_LEVEL_WARNING,
                              "[osMonitor]No.%d task work overtime! threshold tick %d, work time %d\r\n",
                              uwMailID, g_statusCB[uwMailID].uwTickThreshold, uwWorkTime);
#endif
        }
    }
}
