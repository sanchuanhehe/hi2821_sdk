/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Calendar Sample Source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-09-18, Create file. \n
 */
#include "calendar.h"
#include "osal_debug.h"
#include "cmsis_os2.h"
#include "app_init.h"
#include "common_def.h"

#define CALENDAR_DATE_AFTER_SEC         30
#define CALENDAR_DATE_AFTER_MIN         30
#define CALENDAR_DATE_AFTER_HOUR        1
#define CALENDAR_DATE_AFTER_DAY         1
#define CALENDAR_DATE_AFTER_MON         1
#define CALENDAR_DATE_AFTER_YEAR        2000
#define CALENDAR_TIMESTAM_AFTER         1694766630

#define CALENDAR_TASK_STACK_SIZE        0x1000
#define CALENDAR_TASK_PRIO              (osPriority_t)(17)

static void *calendar_task(const char *arg)
{
    unused(arg);
    calendar_t date_base;
    calendar_t date_current;
    calendar_t date_after = { CALENDAR_DATE_AFTER_SEC, CALENDAR_DATE_AFTER_MIN, CALENDAR_DATE_AFTER_HOUR,
                              CALENDAR_DATE_AFTER_DAY, CALENDAR_DATE_AFTER_MON, CALENDAR_DATE_AFTER_YEAR };
    uint64_t timestamp = CALENDAR_TIMESTAM_AFTER; /* This value is a timestamp in milliseconds */
    uint64_t timestamp_current;

    uapi_calendar_init();

    if (uapi_calendar_get_datetime(&date_base) == ERRCODE_SUCC) {
        /* Get the current time of the calendar, the default value is 1970-1-1 0:0:0 */
        osal_printk("get date_base success: %d-%d-%d %d:%d:%d\r\n", date_base.year, date_base.mon,
                    date_base.day, date_base.hour, date_base.min, date_base.sec);
    }

    if (uapi_calendar_set_timestamp(timestamp) == ERRCODE_SUCC) {
        osal_printk("set timestamp success: %llu\r\n", timestamp);
    }

    if (uapi_calendar_get_datetime(&date_current) == ERRCODE_SUCC) {
        osal_printk("get date_current success: %d-%d-%d %d:%d:%d\r\n", date_current.year, date_current.mon,
                    date_current.day, date_current.hour, date_current.min, date_current.sec);
    }

    if (uapi_calendar_get_timestamp(&timestamp_current) == ERRCODE_SUCC) {
        osal_printk("get timestamp_current success: %llu\r\n", timestamp_current);
    }

    if (uapi_calendar_set_datetime(&date_after) == ERRCODE_SUCC) {
        osal_printk("set date_after success: %d-%d-%d %d:%d:%d\r\n", date_after.year, date_after.mon, date_after.day,
                    date_after.hour, date_after.min, date_after.sec);
    }

    return NULL;
}

static void calendar_entry(void)
{
    osThreadAttr_t attr;

    attr.name = "CalendarTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = CALENDAR_TASK_STACK_SIZE;
    attr.priority = CALENDAR_TASK_PRIO;

    if (osThreadNew((osThreadFunc_t)calendar_task, NULL, &attr) == NULL) {
        /* Create task fail. */
    }
}

/* Run the calendar_entry. */
app_run(calendar_entry);