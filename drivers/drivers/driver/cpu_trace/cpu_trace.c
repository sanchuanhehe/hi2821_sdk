/*
 * Copyright (c) @CompanyNameMagicTag 2018-2020. All rights reserved.
 * Description:  CPU Trace Driver
 * Author: @CompanyNameTag
 * Create:
 */
#include "cpu_trace.h"
#include "arch_barrier.h"

#if MCU_ONLY
#include "preserve.h"
#endif

#if (ENABLE_CPU_TRACE != NONE_CPU_TRACE)
#include "securec.h"
#include "memory_config.h"

#if defined(__GNUC__)
CPUTRACE_MEM cpu_trace_item_t cpu_trace_arry[CPU_TRACE_MEM_REGION_LENGTH / sizeof(cpu_trace_item_t)];
#elif defined(__ICCARM__)
cpu_trace_item_t cpu_trace_arry[CPU_TRACE_MEM_REGION_LENGTH / sizeof(cpu_trace_item_t)] @ ".cpu_trace";
#endif
#endif
#if defined(BUILD_APPLICATION_STANDARD)
#include "log_oam_logger.h"
#include "log_printf.h"
#include "log_def.h"
#endif

/* Store for mem analyze */
uint32_t g_cpu_trace_start_addr = 0x0;
uint32_t g_cpu_trace_end_addr = 0x0;

static uint32_t g_cpu_trace_pc = 0x0;
static uint32_t g_cpu_trace_lr = 0x0;
static uint32_t g_cpu_trace_sp = 0x0;
uint16_t g_cpu_trace_result = 0x0;
uint16_t g_cpu_trace_exception =  CPU_TRACE_TRACED_EXCEPTION_MAX;

#if defined(CONFIG_CPU_TRACE_SUPPORT_LPM)
static bool g_cpu_enable_flag = false;
static bool g_cpu_suspend_flag = false;
static cpu_trace_sample_mode_t g_cpu_sample_mode = 0;
static uint32_t g_cpu_start_addr = 0;
static uint32_t g_cpu_end_addr = 0;
#endif

bool cpu_trace_enable(cpu_trace_traced_cpu_t cpu, cpu_trace_sample_mode_t sample_mode,
                      uint32_t start_addr, uint32_t end_addr)
{
    g_cpu_trace_start_addr = start_addr;
    g_cpu_trace_end_addr = end_addr;
#if MCU_ONLY
    if (get_cpu_utils_reset_cause() != REBOOT_CAUSE_APPLICATION_STD_CHIP_WDT_FRST) {
#if defined(CONFIG_CPU_TRACE_SUPPORT_LPM)
        g_cpu_enable_flag = true;
        if (g_cpu_suspend_flag != true) {
            g_cpu_sample_mode = sample_mode;
            g_cpu_start_addr = start_addr;
            g_cpu_end_addr = end_addr;
        }
#endif
        return hal_cpu_trace_enable((hal_cpu_trace_traced_cpu_t)cpu,
                                    (hal_cpu_trace_sample_mode_t)sample_mode, start_addr, end_addr);
    }
    return false;
#else
#if defined(CONFIG_CPU_TRACE_SUPPORT_LPM)
    g_cpu_enable_flag = true;
    if (g_cpu_suspend_flag != true) {
        g_cpu_start_addr = start_addr;
        g_cpu_end_addr = end_addr;
        g_cpu_sample_mode = sample_mode;
    }
#endif
    return hal_cpu_trace_enable((hal_cpu_trace_traced_cpu_t)cpu,
                                (hal_cpu_trace_sample_mode_t)sample_mode, start_addr, end_addr);
#endif
}

void cpu_trace_disable(void)
{
    dsb(); /* flush cpu before disable trace */
    hal_cpu_trace_disable();
}

void cpu_trace_enable_bcpu_repeat(void)
{
#if ((ENABLE_CPU_TRACE & BCPU_TRACE_MASK) == BCPU_TRACE_MASK)
    uint32_t trace_start, trace_end;

    trace_start = (uint32_t)(uintptr_t)((void *)cpu_trace_arry);
    trace_end = trace_start + (uint32_t)sizeof(cpu_trace_arry);

    memset_s((void *)cpu_trace_arry, sizeof(cpu_trace_arry), 0x0, sizeof(cpu_trace_arry));
    cpu_trace_enable(CPU_TRACE_TRACED_BCPU, CPU_TRACE_SAMPLE_MODE_REPEAT, trace_start, trace_end);
#endif
}

void cpu_trace_enable_mcpu_repeat(void)
{
#if ((ENABLE_CPU_TRACE & MCPU_TRACE_MASK) == MCPU_TRACE_MASK)
    uint32_t trace_start, trace_end;

    trace_start = (uint32_t)(uintptr_t)((void *)cpu_trace_arry);
    trace_end = trace_start + (uint32_t)sizeof(cpu_trace_arry);

    memset_s((void *)cpu_trace_arry, sizeof(cpu_trace_arry), 0x0, sizeof(cpu_trace_arry));
    cpu_trace_enable(CPU_TRACE_TRACED_MCPU, CPU_TRACE_SAMPLE_MODE_REPEAT, trace_start, trace_end);
#endif
}

void cpu_trace_enable_gcpu_repeat(void)
{
#if ((ENABLE_CPU_TRACE & GCPU_TRACE_MASK) == GCPU_TRACE_MASK)
    uint32_t trace_start, trace_end;

    trace_start = (uint32_t)(uintptr_t)((void *)cpu_trace_arry);
    trace_end = trace_start + (uint32_t)sizeof(cpu_trace_arry);

    memset_s((void *)cpu_trace_arry, sizeof(cpu_trace_arry), 0x0, sizeof(cpu_trace_arry));
    cpu_trace_enable(CPU_TRACE_TRACED_GCPU, CPU_TRACE_SAMPLE_MODE_REPEAT, trace_start, trace_end);
#endif
}

void cpu_trace_enable_scpu_repeat(void)
{
#if ((ENABLE_CPU_TRACE & SCPU_TRACE_MASK) == SCPU_TRACE_MASK)
    uint32_t trace_start, trace_end;

    trace_start = (uint32_t)(uintptr_t)((void *)cpu_trace_arry);
    trace_end = trace_start + (uint32_t)sizeof(cpu_trace_arry);

    memset_s((void *)cpu_trace_arry, sizeof(cpu_trace_arry), 0x0, sizeof(cpu_trace_arry));
    cpu_trace_enable(CPU_TRACE_TRACED_SCPU, CPU_TRACE_SAMPLE_MODE_REPEAT, trace_start, trace_end);
#endif
}

void cpu_trace_enable_wcpu_repeat(void)
{
#if ((ENABLE_CPU_TRACE & WCPU_TRACE_MASK) == WCPU_TRACE_MASK)
    uint32_t trace_start, trace_end;

    trace_start = (uint32_t)(uintptr_t)((void *)cpu_trace_arry);
    trace_end = trace_start + (uint32_t)sizeof(cpu_trace_arry);

    memset_s((void *)cpu_trace_arry, sizeof(cpu_trace_arry), 0x0, sizeof(cpu_trace_arry));
    cpu_trace_enable(CPU_TRACE_TRACED_WCPU, CPU_TRACE_SAMPLE_MODE_REPEAT, trace_start, trace_end);
#endif
}

void cpu_trace_save_cpu_info(cpu_trace_traced_exception_t exception)
{
    g_cpu_trace_exception = (uint16_t)exception;
    g_cpu_trace_result = cpu_trace_get_locked_regs(HAL_CPU_TRACE_TRACED_MCPU,
                                                   &g_cpu_trace_pc, &g_cpu_trace_lr, &g_cpu_trace_sp);
#if defined(BUILD_APPLICATION_STANDARD) && (USE_COMPRESS_LOG_INSTEAD_OF_SDT_LOG == NO)
    pf_write_fifo_log_alter(LOG_BCORE_PLT_DRIVER_REBOOT, LOG_NUM_DRIVER_REBOOT, LOG_LEVEL_INFO, \
                            "result = 0x%x, exc = 0x%x, pc = 0x%x, lr = 0x%x, sp = 0x%x", FIVE_ARG,\
                            g_cpu_trace_result, g_cpu_trace_exception, g_cpu_trace_pc, g_cpu_trace_lr, g_cpu_trace_sp);
#endif
}

void cpu_trace_lock_pclr(cpu_trace_traced_cpu_t cpu)
{
    hal_cpu_trace_lock_pclr((hal_cpu_trace_traced_cpu_t)cpu);
}

uint32_t cpu_trace_get_locked_pc(cpu_trace_traced_cpu_t cpu)
{
    return hal_cpu_trace_get_locked_pc((hal_cpu_trace_traced_cpu_t)cpu);
}

uint32_t cpu_trace_get_locked_lr(cpu_trace_traced_cpu_t cpu)
{
    return hal_cpu_trace_get_locked_lr((hal_cpu_trace_traced_cpu_t)cpu);
}

uint32_t cpu_trace_get_locked_sp(cpu_trace_traced_cpu_t cpu)
{
    return hal_cpu_trace_get_locked_sp((hal_cpu_trace_traced_cpu_t)cpu);
}

bool cpu_trace_get_locked_regs(hal_cpu_trace_traced_cpu_t cpu, uint32_t *pc, uint32_t *lr, uint32_t *sp)
{
    return hal_cpu_trace_get_locked_regs(cpu, pc, lr, sp);
}

#if defined(CONFIG_CPU_TRACE_SUPPORT_LPM)
errcode_t cpu_trace_suspend(uintptr_t arg)
{
    unused(arg);
    if (g_cpu_enable_flag == true) {
        g_cpu_suspend_flag = true;
    }
    return ERRCODE_SUCC;
}

errcode_t cpu_trace_resume(uintptr_t arg)
{
    if (g_cpu_suspend_flag == true) {
        cpu_trace_enable((cpu_trace_traced_cpu_t)arg, g_cpu_sample_mode, g_cpu_start_addr, g_cpu_end_addr);
        g_cpu_suspend_flag = false;
    }
    return ERRCODE_SUCC;
}
#endif