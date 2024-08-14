/**
 * Copyright (c) @CompanyNameMagicTag 2022-2023. All rights reserved. \n
 *
 * Description: Application core os initialize interface for standard \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-07-27, Create file. \n
 */

#include "soc_osal.h"
#include "log_common.h"
#include "log_def.h"
#include "log_uart.h"
#include "pmu_interrupt.h"
#include "log_reg_dump.h"
#include "watchdog.h"
#include "preserve.h"
#include "hal_reboot.h"
#ifdef TEST_SUITE
#include "test_suite.h"
#include "test_auxiliary.h"
#include "test_suite_uart.h"
#ifdef PLT_TEST_ENABLE
#include "test_keyscan.h"
#include "test_i2s.h"
#include "test_qdec.h"
#include "test_usb.h"
#include "test_pdm.h"
#include "test_pinctrl.h"
#endif
#endif
#ifdef FTRACE
#include "test_ftrace.h"
#endif

#ifdef AT_COMMAND
#include "at_product.h"
#include "at_porting.h"
#endif
#if defined(CONFIG_SAMPLE_ENABLE)
#include "app_init.h"
#endif
#ifdef COREMARK_TEST
#include "core_portme.h"
#endif
#include "los_task_pri.h"
#include "debug_print.h"
#if (ENABLE_LOW_POWER == YES)
#include "pm_veto.h"
#endif
#include "memory_info.h"
#include "chip_io.h"
#include "app_os_init.h"
#include "demo.h"  //lh
/*
 *  优先级范围 0-31
 *  0：     SWT 线程优先级（最高）
 *  1-30：  推荐使用优先级
 *  31：    IDLE 线程优先级（最低）
 */

/* 平台优先级，由高到低 */
#define TASK_PRIORITY_CMD                 23
#define TASK_PRIORITY_APP                 24
#define TASK_PRIORITY_LOG                 28
/* 业务优先级，由高到低 */
/* BTC */
#define TASK_PRIORITY_BT                  17
/* BTH */
#define TASK_PRIORITY_BTH_RECV            18
#define TASK_PRIORITY_BTH_SDK             19
#define TASK_PRIORITY_SDK                 22
#define TASK_PRIORITY_SRV                 27
#define TASK_PRIORITY_TEST                30  //lh
/* 线程栈分配，推荐比水线高30% */
#define APP_STACK_SIZE                  0xA00
#define CMD_STACK_SIZE                  0x800
#define LOG_STACK_SIZE                  0x600
#define AT_STACK_SIZE                   0xc00
#define TASK_STAKDEPTH_BTH_SDK          0x200
#define TASK_STAKDEPTH_BTH_RECV         0x400
#define STACK_SIZE_BASELINE             0x200
#define BTH_SERVICE_STACK_SIZE          (STACK_SIZE_BASELINE * 6)
#define BTH_SDK_STACK_SIZE              (0x800)
#define BT_STACK_SIZE                   (0xA00)

#define TEST_TASK_SIZE                  0X300  //lh

#define TASK_COMMON_APP_DELAY_MS        5000

typedef struct {
    char *task_name;
    void *task_arg;
    uint32_t task_stack;
    uint32_t task_pri;
    osal_task *task_handle;
    osal_kthread_handler task_func;
} app_task_attr_t;

/* plt init thread. */
void app_main(void *unused);
void cmd_main_add_functions(void);
/* rf init thread. */
void bt_thread_handle(void *para);
void add_nfc_t2t_test_case(void);
#ifndef DEVICE_ONLY
void bt_acore_task_main(const void *pvParams);
void bt_tran_task_queue_init(void);
void recv_data_task(void);
void btsrv_task_body(const void *data);
void sdk_msg_thread(void);
/* AT register */
#ifdef CONFIG_AT_GLE
void bth_gle_dft_command_register(void);
#endif
#ifdef CONFIG_AT_BLE
void bth_ble_dft_command_register(void);
#endif
#endif

static app_task_attr_t g_app_tasks[] = {
    {"app", NULL, APP_STACK_SIZE, TASK_PRIORITY_APP, NULL, (osal_kthread_handler)app_main},
#if (USE_COMPRESS_LOG_INSTEAD_OF_SDT_LOG == NO)
    {"log", NULL, LOG_STACK_SIZE, TASK_PRIORITY_LOG, NULL, (osal_kthread_handler)log_main},
#endif
#ifdef AT_COMMAND
    {"at", NULL, AT_STACK_SIZE, TASK_PRIORITY_CMD, NULL, (osal_kthread_handler)uapi_at_msg_main},
#endif
#ifdef BGLE_TASK_EXIST
    {"bt", NULL, BT_STACK_SIZE, TASK_PRIORITY_BT, NULL, (osal_kthread_handler)bt_thread_handle},
#ifndef DEVICE_ONLY
#ifndef BT_GLE_ONLY
    {"bt_sdk", NULL, BTH_SDK_STACK_SIZE, TASK_PRIORITY_SDK, NULL, (osal_kthread_handler)bt_acore_task_main},
#endif
    {"bth_data", NULL, TASK_STAKDEPTH_BTH_RECV, TASK_PRIORITY_BTH_RECV, NULL, (osal_kthread_handler)recv_data_task},
    {"bt_service", NULL, BTH_SERVICE_STACK_SIZE, TASK_PRIORITY_SRV, NULL, (osal_kthread_handler)btsrv_task_body},
#endif
#endif
    {"test_task",NULL,TEST_TASK_SIZE,TASK_PRIORITY_TEST,NULL,(osal_kthread_handler)test_task}, //lh
};

#define M_NUM_TASKS (sizeof(g_app_tasks) / sizeof(app_task_attr_t))

void app_os_init(void)
{
    osal_kthread_lock();
    for (uint8_t i = 0; i < M_NUM_TASKS; i++) {
        g_app_tasks[i].task_handle = osal_kthread_create(g_app_tasks[i].task_func, g_app_tasks[i].task_arg,
                                                         g_app_tasks[i].task_name, g_app_tasks[i].task_stack);
        if (g_app_tasks[i].task_handle == NULL) {
            panic(PANIC_TASK_CREATE_FAILED, i);
        }
        osal_kthread_set_priority(g_app_tasks[i].task_handle, g_app_tasks[i].task_pri);
    }
    osal_kthread_unlock();
#ifdef TEST_SUITE
#ifdef AT_COMMAND
    for (uint8_t i = 0; i < M_NUM_TASKS; i++) {
        if (strcmp(g_app_tasks[i].task_name, "at") == 0) {
            uapi_set_at_task((uint32_t *)g_app_tasks[i].task_handle->task);
            osal_kthread_suspend(g_app_tasks[i].task_handle);
            break;
        }
    }
#endif
    cmd_main_add_functions();
#endif
}
#ifdef TEST_SUITE
#if (ENABLE_LOW_POWER == YES)
static int test_mcu_vote_sleep(int argc, char* argv[])
{
    unused(argc);
    uint8_t vote = (uint8_t)strtol(argv[0], NULL, 0);
    if (vote == 0) {
        uapi_pm_add_sleep_veto(PM_VETO_ID_MCU);
        writel(MEMORY_INFO_CRTL_REG, MEMORY_INFO_CLOSE);
    } else {
        uapi_pm_remove_sleep_veto(PM_VETO_ID_MCU);
    }
    return 0;
}
#endif

void cmd_main_add_functions(void)
{
    add_auxiliary_functions();
#ifdef AT_COMMAND
    uapi_test_suite_add_function("testsuite_sw_at", "<at>", uapi_testsuite_sw_at);
#endif
#ifdef COREMARK_TEST
    uapi_test_suite_add_function("coremark", "Coremark test Function", coremark_test);
#endif
#if (ENABLE_LOW_POWER == YES)
    uapi_test_suite_add_function("mcu_vote_slp", "MCU vote to sleep or not Function", test_mcu_vote_sleep);
#endif
#ifndef DEVICE_ONLY
#ifdef CONFIG_AT_GLE
    bth_gle_dft_command_register();
#endif
#ifdef CONFIG_AT_BLE
    bth_ble_dft_command_register();
#endif
#endif
#ifdef PLT_TEST_ENABLE
#ifndef SUPPORT_BT_UPG
    add_usb_test_case();
#endif
#endif
#ifdef NFC_TASK_EXIST
    add_nfc_t2t_test_case();
#endif
#ifdef FTRACE
    add_ftrace_test_case();
#endif
}
#endif
__attribute__((weak)) void app_main(void *unused)
{
    UNUSED(unused);
    hal_reboot_clear_history();
    system_boot_reason_print();
    system_boot_reason_process();
#if (USE_COMPRESS_LOG_INSTEAD_OF_SDT_LOG == NO)
    log_exception_dump_reg_check();
#endif
    //demo_prinf();
#if defined(CONFIG_SAMPLE_ENABLE)
    app_tasks_init();
#endif
    while (1) {  //lint !e716 Main Loop
        (void)osal_msleep(TASK_COMMON_APP_DELAY_MS);
        oml_pf_log_print0(LOG_BCORE_PLT_DRIVER_REBOOT, LOG_NUM_DEBUG, LOG_LEVEL_INFO, "App main");
#if defined(PM_MCPU_MIPS_STATISTICS_ENABLE) && (PM_MCPU_MIPS_STATISTICS_ENABLE == YES)
        oml_pf_log_print2(LOG_BCORE_PLT, LOG_NUM_DEBUG, LOG_LEVEL_INFO,
            "[Mcpu mips statistics] total work time: %dms, total idle time: %dms.\r\n",
            pm_get_total_work_time(), pm_get_total_idle_time());
        PRINT("[Mcpu mips statistics] total work time: %dms, total idle time: %dms.\r\n",
            pm_get_total_work_time(), pm_get_total_idle_time());
#endif
        uapi_watchdog_kick();
        print_stack_waterline_riscv();
        print_heap_statistics_riscv();
    }
}