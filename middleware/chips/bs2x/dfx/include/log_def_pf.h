/*
 * Copyright (c) @CompanyNameMagicTag 2018-2020. All rights reserved.
 * Description:  common logging producer interface - need to change name of log.h in all the protocol core files
 * Author: @CompanyNameTag
 * Create:
 */

#ifndef _LOG_DEF_PF_H_
#define _LOG_DEF_PF_H_

typedef enum {
    // Platform 0-1024
    SECURITY_MAIN_C = 0,
    APP_OS_INIT_C,
    THREAD_INIT_C,
    SEC_OS_INIT_C,
    LOG_OAM_MSG_C,
    LOG_OML_EXCEPTION_C,
    BACKTRACE_C_NOT_USED_C,
    PANIC_C,
    RUNTIME_MONITOR_C,
    LOS_TASK_C,
    LOG_C,
    LOW_POWER_CONTROL_C,
    CPU_UTILS_C,
    HAL_PVSENSOR_C,
    PMU_CMU_C,
    GPIO_C,
    IR_C,
    CPU_TRACE_C,
    XIP_C,
    HAL_LPC_CORE_C,
    MPU_C,
    HAL_CPU_HIFI_C,
    IPC_C,
    FIRMWARE_VERSION_C,
    HAL_ADC_C,
    PATCH_C,
    PMU_INTERRUPT_C,
    HAL_CPU_CORE_C,
    COMMU_INTERFACE_C,
    EPMU_COUL_DRV_C,
    EPMU_COUL_CORE_C,
    EPMU_C,
    GNSS_OS_INIT_C,
    HAL_IPC_IRQ_C,
    PAL_LPM_C,
    PRESERVE_C,
    LITEOS_INFO_C,
    CLOCKS_CORE_C,
    SYSTEM_INFO_C,
    CPU_LOAD_C,
    PMU_C,
    LOS_STATUS_C,
    GNSS_PMU_C,
    PATCH_RISCV_C,
    PM_CONTROL_C,
    BLE_HID_SERVER_C,
    MAIN_C,
    DIAG_DFX_C,
    LOG_OAM_REG_QUERY_C,
    APP_MAIN_C,
    MEMORY_INFO_C,
    PF_FILE_ID_MAX = 1024,
} log_file_list_enum_pf_app_t;
#endif
