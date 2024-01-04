/*
 * Copyright (c) @CompanyNameMagicTag 2018-2020. All rights reserved.
 * Description:  HAL CPU TRACE DRIVER.
 * Author: @CompanyNameTag
 * Create: 2018-10-15
 */

#include "hal_cpu_trace.h"
#include "chip_io.h"
#include "core.h"
#include "chip_definitions.h"
#include "arch_barrier.h"

/* for mcpu pc/lr/sp lock */
#define HAL_MCPU_TRACE_CTL_RB_BASE_ADDR                  0x57000000  // MCU_DIAG_CTL_RB
#define HAL_MCPU_LOAD_DIAG_REG_OFFSET                    0x3A0       // MCPU_LOAD_DIAG
#define HAL_LOAD_DIAG_REG_LOCK_ENABLE_BIT                0
#define HAL_LOAD_DIAG_REG_LOCK_STATUS_BIT                8

#define HAL_MCPU_PC_L_DIAG_REG_OFFSET                    0x3A8       // MCPU_PC_L_DIAG
#define HAL_MCPU_PC_H_DIAG_REG_OFFSET                    0x3AC
#define HAL_MCPU_LR_L_DIAG_REG_OFFSET                    0x3B0
#define HAL_MCPU_LR_H_DIAG_REG_OFFSET                    0x3B4
#define HAL_MCPU_SP_L_DIAG_REG_OFFSET                    0x3B8
#define HAL_MCPU_SP_H_DIAG_REG_OFFSET                    0x3BC

/* for bcpu pc/lr lock */
#define HAL_BCPU_TRACE_CTL_RB_BASE_ADDR                  0x57000400  // BCPU_TRACE_CTL_RB
#define HAL_BCPU_LOAD_DIAG_REG_OFFSET                    0x3A0       // BCPU_LOAD_DIAG

#define HAL_BCPU_PC_L_DIAG_REG_OFFSET                    0x3A8       // BCPU_PC_L_DIAG
#define HAL_BCPU_PC_H_DIAG_REG_OFFSET                    0x3AC
#define HAL_BCPU_LR_L_DIAG_REG_OFFSET                    0x3B0
#define HAL_BCPU_LR_H_DIAG_REG_OFFSET                    0x3B4

#define HAL_CRG_CLKEN_REG_OFFSET                         0x800
#define HAL_BTOP_GLUE_TRIGGER_CLKEN_BIT                  0x8
#define HAL_CRG_SOFT_RST_N_REG_OFFSET                    0x80C
#define HAL_SOFT_RST_BTOP_GLUE_TRIGGER_N_BIT             0x4

#if CHIP_LIBRA
/* for gnss pc/lr/sp lock */
#define HAL_GCPU_TRACE_CTL_RB_BASE_ADDR                  0x57000000  // GNSS_TRACE_CTL_RB
#define HAL_GCPU_LOAD_DIAG_REG_OFFSET                    0x380       // GNSS_LOAD_DIAG

#define HAL_GCPU_PC_L_DIAG_REG_OFFSET                    0x388       // GCPU_PC_L_DIAG
#define HAL_GCPU_PC_H_DIAG_REG_OFFSET                    0x38C
#define HAL_GCPU_LR_L_DIAG_REG_OFFSET                    0x390
#define HAL_GCPU_LR_H_DIAG_REG_OFFSET                    0x394
#define HAL_GCPU_SP_L_DIAG_REG_OFFSET                    0x398
#define HAL_GCPU_SP_H_DIAG_REG_OFFSET                    0x39C

/* for security pc/lr lock */
#define HAL_SCPU_TRACE_CTL_RB_BASE_ADDR                  0x52000000  // SEC_TRACE_CTL_RB
#define HAL_SCPU_LOAD_DIAG_REG_OFFSET                    0x814       // SECURITY_LOAD_DIAG

#define HAL_SCPU_PC_L_DIAG_REG_OFFSET                    0x81C
#define HAL_SCPU_PC_H_DIAG_REG_OFFSET                    0x820
#define HAL_SCPU_LR_L_DIAG_REG_OFFSET                    0x824
#define HAL_SCPU_LR_H_DIAG_REG_OFFSET                    0x828
#endif
#define HAL_CFG_MCPU_LOCK_EN_REG                        0x3A4       // MCPU_PC_LR_ENABLE
#define HAL_CFG_BCPU_LOCK_EN_REG                        0x3A4       // BCPU_PC_LR_ENABLE
#define HAL_CFG_GCPU_LOCK_EN_REG                        0x384       // GCPU_PC_LR_ENABLE
#define HAL_CFG_SCPU_LOCK_EN_REG                        0x818       // SCPU_PC_LR_ENABLE
#define HAL_CFG_WCPU_LOCK_EN_REG                        0x818       // WCPU_PC_LR_ENABLE
#if CORE == APPS
/* for mcpu trace */
#define HAL_PCLR_EN_BIT                                 1

#define HAL_CPU_TRACE_CTL_RB_BASE_ADDR                  0x52004000  // MCPU_DIAG_CTL_RB
#define HAL_CPU_TRACE_ENABLE                            0x52004108
#define HAL_TRACE_EN_BIT                                1

#define HAL_CFG_MONITOR_SEL_REG_OFFSET                  0x0100      // CFG_FUNCTION_SEL
#define HAL_CFG_MONITOR_SEL_BIT                         0

#define HAL_CFG_CPU_TRACE_SAVE_SEL_REG_OFFSET           0x0104      // CFG_MCU_DIAG_TRACE_SAVE_SEL
#define HAL_CFG_CPU_TRACE_SAVE_SEL_BIT                  0

#define HAL_CFG_CPU_TRACE_REG_OFFSET                    0x0108      // CFG_MCU_DIAG_CPU_TRACE
#define HAL_CFG_CPU_TRACE_EN_BIT                        0
#define HAL_MCPU_TRACE_EN_BIT                           1
#define HAL_CFG_CPU_TRACE_REG_DEFAULT                   0

#define HAL_CFG_CPU_MONITOR_CLOCK_REG_OFFSET            0x010C      // CFG_MCU_DIAG_MONITOR_CLOCK
#define HAL_CFG_CPU_TRACE_CLK_EN_CFG                    0x1FF
#define HAL_CFG_MONITOR_CLK_EN_CFG                      0x18F

#define HAL_CFG_CPU_SAMPLE_MODE_REG_OFFSET              0x0204      // CFG_MCU_DIAG_SAMPLE_MODE
#define HAL_CFG_CPU_SAMPLE_SYNC_BIT                     9
#define HAL_CFG_CPU_SAMPLE_EN_BIT                       8
#define HAL_CFG_CPU_SAMPLE_MODE_BITS_OFFSET             0
#define HAL_CFG_CPU_SAMPLE_MODE_BITS_LEN                2
#define HAL_CFG_CPU_SAMPLE_DEFAULT                      0

#define HAL_CFG_CPU_SAMPLE_LENGTH_L_REG_OFFSET          0x0208      // CFG_MCU_DIAG_SAMPLE_LENGTH_L
#define HAL_CFG_CPU_SAMPLE_LENGTH_H_REG_OFFSET          0x020C
#define HAL_CFG_CPU_SAMPLE_START_ADDR_L_REG_OFFSET      0x0210
#define HAL_CFG_CPU_SAMPLE_START_ADDR_H_REG_OFFSET      0x0214
#define HAL_CFG_CPU_SAMPLE_END_ADDR_L_REG_OFFSET        0x0218
#define HAL_CFG_CPU_SAMPLE_END_ADDR_H_REG_OFFSET        0x021C

#define CPU_TRACE_INTRENAL_MEM_REGION_START             0x52006000  // for mcpu internal 8k ram
#define CPU_TRACE_INTRENAL_MEM_REGION_END               0x52008000

#elif (CORE == BT) || (CORE == CONTROL_CORE)
/* for bcpu trace */
#define HAL_PCLR_EN_BIT                                 1

#define HAL_CPU_TRACE_CTL_RB_BASE_ADDR                  0x5900C000  // BCPU_TRACE_CTL_RB
#define HAL_CPU_TRACE_ENABLE                            0x59008500
#define HAL_TRACE_EN_BIT                                0

#define HAL_CFG_CPU_TRACE_SAVE_SEL_REG_OFFSET           0x0100      // CFG_TRACE_SAVE_SEL
#define HAL_CFG_CPU_TRACE_SAVE_SEL_BIT                  0

#define HAL_CFG_CPU_TRACE_REG_OFFSET                    0x0104      // CFG_CPU_TRACE
#define HAL_CFG_CPU_TRACE_EN_BIT                        0
#define HAL_CFG_CPU_TRACE_REG_DEFAULT                   0

#define HAL_CFG_CPU_MONITOR_CLOCK_REG_OFFSET            0x0108      // CFG_MONITOR_CLOCK
#define HAL_CFG_CPU_TRACE_CLK_EN_CFG                    0x1F
#define HAL_CFG_MONITOR_CLK_EN_CFG                      0x1B

#define HAL_CFG_CPU_SAMPLE_MODE_REG_OFFSET              0x0200      // CFG_SAMPLE_MODE
#define HAL_CFG_CPU_SAMPLE_SYNC_BIT                     9
#define HAL_CFG_CPU_SAMPLE_EN_BIT                       8
#define HAL_CFG_CPU_SAMPLE_MODE_BITS_OFFSET             0
#define HAL_CFG_CPU_SAMPLE_MODE_BITS_LEN                2
#define HAL_CFG_CPU_SAMPLE_DEFAULT                      0

#define HAL_CFG_CPU_SAMPLE_LENGTH_L_REG_OFFSET          0x0204      // CFG_SAMPLE_LENGTH_L
#define HAL_CFG_CPU_SAMPLE_LENGTH_H_REG_OFFSET          0x0208
#define HAL_CFG_CPU_SAMPLE_START_ADDR_L_REG_OFFSET      0x020C
#define HAL_CFG_CPU_SAMPLE_START_ADDR_H_REG_OFFSET      0x0210
#define HAL_CFG_CPU_SAMPLE_END_ADDR_L_REG_OFFSET        0x0214
#define HAL_CFG_CPU_SAMPLE_END_ADDR_H_REG_OFFSET        0x0218

#define CPU_TRACE_INTRENAL_MEM_REGION_START             0x5900E000  // for bcpu internal 8k ram
#define CPU_TRACE_INTRENAL_MEM_REGION_END               0x59010000

#define HAL_CPU_BT_DIAG_CTL_RB_BASE_ADDR                0x59008000
#define HAL_CFG_MONITOR_SAVE_SEL_REG_OFFSET             0x0100
#define HAL_CFG_MONITOR_SAVE_SEL_BIT                    0

#define HAL_CFG_CPU_TRACE_BT_DIAG_SEL_REG_OFFSET        0x0104      // CFG_MCU_DIAG_TRACE_SAVE_SEL
#define HAL_CFG_CPU_TRACE_DIAG_SEL_BIT                  0

#define HAL_CFG_CPU_DIAG_MONITOR_CLOCK_REG_OFFSET       0x0108      // CFG_MCU_DIAG_MONITOR_CLOCK
#define HAL_CFG_CPU_MONITOR_CLK_EN_CFG                  0x7F

#define HAL_BT_DIAG_MEM_BUS_CLKEN_REG_OFFSET            0x98
#define HAL_CFG_DIAG_MEM_BUS_CLKEN_BIT                  0

#elif CORE == GNSS
/* for gcpu trace */
#define HAL_PCLR_EN_BIT                                 1

#define HAL_CPU_TRACE_CTL_RB_BASE_ADDR                  0x5000C000  // GCPU_TRACE_CTL_RB

#define HAL_CFG_CPU_TRACE_SAVE_SEL_REG_OFFSET           0x0100      // CFG_GNSS_DIAG_TRACE_SAVE_SEL
#define HAL_CFG_CPU_TRACE_SAVE_SEL_BIT                  0

#define HAL_CFG_CPU_MONITOR_CLOCK_REG_OFFSET            0x0108      // CFG_GNSS_DIAG_MONITOR_CLOCK
#define HAL_CFG_CPU_TRACE_CLK_EN_CFG                    0x3
#define HAL_CFG_MONITOR_CLK_EN_CFG                      0x1

#define HAL_CFG_CPU_TRACE_MEM_CLK_REG_OFFSET            0x010C      // CFG_GNSS_DIAG_CPU_TRACE_MEM_CLK
#define HAL_CFG_CPU_TRACE_CLOCK_EN_BIT                  1
#define HAL_CFG_CPU_TRACE_RAM_SEL                       1

#define HAL_CFG_CPU_SAMPLE_MODE_REG_OFFSET              0x0200      // CFG_GNSS_DIAG_SAMPLE_MODE
#define HAL_CFG_CPU_SAMPLE_EN_BIT                       1
#define HAL_CFG_CPU_SAMPLE_MODE_BITS_OFFSET             0
#define HAL_CFG_CPU_SAMPLE_MODE_BITS_LEN                1
#define HAL_CFG_CPU_SAMPLE_DEFAULT                      1

#define CPU_TRACE_INTRENAL_MEM_REGION_START             0x5000E000  // for gnss internal 8k ram
#define CPU_TRACE_INTRENAL_MEM_REGION_END               0x50010000

#elif CORE == SECURITY
/* for scpu trace */
#define HAL_PCLR_EN_BIT                                 1

#define HAL_CPU_TRACE_CTL_RB_BASE_ADDR                  0x51004000  // SCPU_TRACE_CTL_RB
#define HAL_CPU_TRACE_ENABLE                            0x51000068
#define HAL_TRACE_EN_BIT                                0

#define HAL_CFG_CPU_TRACE_SAVE_SEL_REG_OFFSET           0x0100      // CFG_SECURITY_DIAG_TRACE_SAVE_SEL
#define HAL_CFG_CPU_TRACE_SAVE_SEL_BIT                  0

#define HAL_CFG_CPU_MONITOR_CLOCK_REG_OFFSET            0x0108      // CFG_SCPU_DIAG_MONITOR_CLOCK
#define HAL_CFG_CPU_TRACE_CLK_EN_CFG                    0x3
#define HAL_CFG_MONITOR_CLK_EN_CFG                      0x1

#define HAL_CFG_CPU_TRACE_MEM_CLK_REG_OFFSET            0x010C      // CFG_CPU_TRACE_MEM_CLK
#define HAL_CFG_CPU_TRACE_CLOCK_EN_BIT                  1
#define HAL_CFG_CPU_TRACE_RAM_SEL                       1

#define HAL_CFG_CPU_SAMPLE_MODE_REG_OFFSET              0x0200      // CFG_SAMPLE_MODE
#define HAL_CFG_CPU_SAMPLE_EN_BIT                       1
#define HAL_CFG_CPU_SAMPLE_MODE_BITS_OFFSET             0
#define HAL_CFG_CPU_SAMPLE_MODE_BITS_LEN                1
#define HAL_CFG_CPU_SAMPLE_DEFAULT                      1

#define CPU_TRACE_INTRENAL_MEM_REGION_START             0x51006000  // for scpu internal 8k ram
#define CPU_TRACE_INTRENAL_MEM_REGION_END               0x51008000
#elif CORE == WIFI
/* for wcpu trace */
#define HAL_PCLR_EN_BIT                                 1

#define HAL_CPU_TRACE_CTL_RB_BASE_ADDR                  0x58010000  // WCPU_TRACE_CTL_RB
#define HAL_CPU_TRACE_ENABLE                            0x51000068
#define HAL_TRACE_EN_BIT                                0

#define HAL_CFG_CPU_TRACE_SAVE_SEL_REG_OFFSET           0x0100      // CFG_WCPU_DIAG_TRACE_SAVE_SEL
#define HAL_CFG_CPU_TRACE_SAVE_SEL_BIT                  0

#define HAL_CFG_CPU_TRACE_DIAG_EN_REG_OFFSET            0x0104      // CFG_WCPU_DIAG_TRACE_SAVE_SEL
#define HAL_CFG_CPU_TRACE_DIAG_EN_BIT                   0

#define HAL_CFG_CPU_MONITOR_CLOCK_REG_OFFSET            0x0108      // CFG_WCPU_DIAG_MONITOR_CLOCK
#define HAL_CFG_CPU_TRACE_CLK_EN_CFG                    0x3
#define HAL_CFG_MONITOR_CLK_EN_CFG                      0x1

#define HAL_CFG_CPU_TRACE_MEM_CLK_REG_OFFSET            0x010C      // CFG_CPU_TRACE_MEM_CLK
#define HAL_CFG_CPU_TRACE_RAM_SEL_BIT                   0
#define HAL_CFG_CPU_TRACE_CLOCK_EN_BIT                  1
#define HAL_CFG_CPU_TRACE_RAM_SEL                       1

#define HAL_CFG_CPU_SAMPLE_MODE_REG_OFFSET              0x0200      // CFG_SAMPLE_MODE
#define HAL_CFG_CPU_SAMPLE_EN_BIT                       1
#define HAL_CFG_CPU_SAMPLE_MODE_BITS_OFFSET             0
#define HAL_CFG_CPU_SAMPLE_MODE_BITS_LEN                1
#define HAL_CFG_CPU_SAMPLE_DEFAULT                      1
#else
#error "Should NOT build here!"
#endif

#define HAL_CPU_LOAD_LOCK_TIMEOUT 100
#define HAL_CPU_LOAD_RETRY_COUNT  2

#define get_low_16bits(val)          ((uint16_t)((val) & 0xFFFF))
#define get_high_16bits(val)         ((uint16_t)((val) >> 16))
#define comb_to_32bits(val_h, val_l) ((uint32_t)((((uint32_t)((val_h) & 0xFFFF)) << 16) | ((val_l) & 0xFFFF)))

typedef struct {
    uint32_t cpu_info_base_addr;
    uint32_t pc_low_offset;
    uint32_t pc_high_offset;
    uint32_t lr_low_offset;
    uint32_t lr_high_offset;
    uint32_t cpu_diag_offset;
    uint32_t cpu_lock_enable;
} hal_cpu_info_t;

static hal_cpu_info_t g_trace_value[] = {
    {HAL_BCPU_TRACE_CTL_RB_BASE_ADDR, HAL_BCPU_PC_L_DIAG_REG_OFFSET, HAL_BCPU_PC_H_DIAG_REG_OFFSET,
     HAL_BCPU_LR_L_DIAG_REG_OFFSET, HAL_BCPU_LR_H_DIAG_REG_OFFSET, HAL_BCPU_LOAD_DIAG_REG_OFFSET,
     HAL_CFG_BCPU_LOCK_EN_REG},
    {HAL_MCPU_TRACE_CTL_RB_BASE_ADDR, HAL_MCPU_PC_L_DIAG_REG_OFFSET, HAL_MCPU_PC_H_DIAG_REG_OFFSET,
     HAL_MCPU_LR_L_DIAG_REG_OFFSET, HAL_MCPU_LR_H_DIAG_REG_OFFSET, HAL_MCPU_LOAD_DIAG_REG_OFFSET,
     HAL_CFG_MCPU_LOCK_EN_REG},
#if CHIP_LIBRA
    {HAL_GCPU_TRACE_CTL_RB_BASE_ADDR, HAL_GCPU_PC_L_DIAG_REG_OFFSET, HAL_GCPU_PC_H_DIAG_REG_OFFSET,
     HAL_GCPU_LR_L_DIAG_REG_OFFSET, HAL_GCPU_LR_H_DIAG_REG_OFFSET, HAL_GCPU_LOAD_DIAG_REG_OFFSET,
     HAL_CFG_GCPU_LOCK_EN_REG},
    {HAL_SCPU_TRACE_CTL_RB_BASE_ADDR, HAL_SCPU_PC_L_DIAG_REG_OFFSET, HAL_SCPU_PC_H_DIAG_REG_OFFSET,
     HAL_SCPU_LR_L_DIAG_REG_OFFSET, HAL_SCPU_LR_H_DIAG_REG_OFFSET, HAL_SCPU_LOAD_DIAG_REG_OFFSET,
     HAL_CFG_SCPU_LOCK_EN_REG}
#endif
};

bool g_cputrace_running = false;
bool hal_cpu_trace_is_running(void)
{
    return g_cputrace_running;
}

#if (CORE == BT) || (CORE == CONTROL_CORE)
static void hal_cpu_trace_set_external_ram_clk(void)
{
    regw_setbit((void *)(uintptr_t)HAL_CPU_BT_DIAG_CTL_RB_BASE_ADDR, HAL_CFG_MONITOR_SAVE_SEL_REG_OFFSET,
                HAL_CFG_MONITOR_SAVE_SEL_BIT);
    regw_clrbit((void *)(uintptr_t)HAL_CPU_BT_DIAG_CTL_RB_BASE_ADDR, HAL_CFG_CPU_TRACE_BT_DIAG_SEL_REG_OFFSET,
                HAL_CFG_CPU_TRACE_DIAG_SEL_BIT);
    reg_writew(HAL_CPU_BT_DIAG_CTL_RB_BASE_ADDR, HAL_CFG_CPU_DIAG_MONITOR_CLOCK_REG_OFFSET,
               HAL_CFG_CPU_MONITOR_CLK_EN_CFG);
    regw_setbit((void *)(uintptr_t)M_CTL_RB_BASE, HAL_BT_DIAG_MEM_BUS_CLKEN_REG_OFFSET,
                HAL_CFG_DIAG_MEM_BUS_CLKEN_BIT);
}
#endif

static void hal_cpu_trace_set_top_glue_trigger(hal_cpu_trace_traced_cpu_t cpu, bool en)
{
#ifdef CONFIG_CPUTRACE_EXIST_BTOP_GLUE
    if (cpu != HAL_CPU_TRACE_TRACED_BCPU) { return; }
    if (en) {
        regw_setbit((void *)(uintptr_t)B_CTL_RB_BASE, HAL_CRG_CLKEN_REG_OFFSET,
                    HAL_BTOP_GLUE_TRIGGER_CLKEN_BIT);
        regw_setbit((void *)(uintptr_t)B_CTL_RB_BASE, HAL_CRG_SOFT_RST_N_REG_OFFSET,
                    HAL_SOFT_RST_BTOP_GLUE_TRIGGER_N_BIT);
    } else {
        regw_clrbit((void *)(uintptr_t)B_CTL_RB_BASE, HAL_CRG_CLKEN_REG_OFFSET,
                    HAL_BTOP_GLUE_TRIGGER_CLKEN_BIT);
        regw_clrbit((void *)(uintptr_t)B_CTL_RB_BASE, HAL_CRG_SOFT_RST_N_REG_OFFSET,
                    HAL_SOFT_RST_BTOP_GLUE_TRIGGER_N_BIT);
    }
#else
    UNUSED(cpu);
    UNUSED(en);
#endif
}

bool hal_cpu_trace_enable(hal_cpu_trace_traced_cpu_t cpu, hal_cpu_trace_sample_mode_t sample_mode,
                          uint32_t start_addr, uint32_t end_addr)
{
    hal_cpu_trace_sample_sync_bit_config();
    hal_cpu_trace_clock_enable();
    hal_cpu_trace_set_sample_mode(sample_mode);

#if (CORE == APPS) || (CORE == BT) || (CORE == CONTROL_CORE)
    if (!hal_cpu_trace_set_sample_addr(start_addr, end_addr)) {
        return false;
    }
#endif

    /* enable trace and pc/lr lock */
#if (CHIP_LIBRA && (CORE != GNSS)) || CHIP_SOCMN1 || CHIP_BS25
    regw_setbit(HAL_CPU_TRACE_ENABLE, 0, HAL_TRACE_EN_BIT);
#endif

#if (CORE == APPS) || (CORE == BT) || (CORE == CONTROL_CORE)
    regw_setbit((void *)(uintptr_t)HAL_CPU_TRACE_CTL_RB_BASE_ADDR, HAL_CFG_CPU_SAMPLE_MODE_REG_OFFSET,
                HAL_CFG_CPU_SAMPLE_SYNC_BIT);
    regw_setbit((void *)(uintptr_t)HAL_CPU_TRACE_CTL_RB_BASE_ADDR, HAL_CFG_CPU_SAMPLE_MODE_REG_OFFSET,
                HAL_CFG_CPU_SAMPLE_EN_BIT);
    regw_setbit((void *)(uintptr_t)HAL_CPU_TRACE_CTL_RB_BASE_ADDR, HAL_CFG_CPU_TRACE_REG_OFFSET,
                HAL_CFG_CPU_TRACE_EN_BIT);
#if (CORE == APPS)
    regw_setbit((void *)(uintptr_t)HAL_CPU_TRACE_CTL_RB_BASE_ADDR, HAL_CFG_CPU_TRACE_REG_OFFSET,
                HAL_MCPU_TRACE_EN_BIT);
#endif
#endif

#if (CORE == GNSS) || (CORE == SECURITY) || (CORE == WIFI)
    regw_setbit((void *)(uintptr_t)HAL_CPU_TRACE_CTL_RB_BASE_ADDR, HAL_CFG_CPU_MONITOR_CLOCK_REG_OFFSET,
                HAL_CFG_CPU_TRACE_CLOCK_EN_BIT);
    regw_setbit((void *)(uintptr_t)HAL_CPU_TRACE_CTL_RB_BASE_ADDR, HAL_CFG_CPU_SAMPLE_MODE_REG_OFFSET,
                HAL_CFG_CPU_SAMPLE_EN_BIT);
#endif
    UNUSED(cpu);
    UNUSED(start_addr);
    UNUSED(end_addr);
    g_cputrace_running = true;
    return true;
}

void hal_cpu_trace_disable(void)
{
#if (CORE == APPS) || (CORE == BT) || (CORE == CONTROL_CORE)
    reg_writew(HAL_CPU_TRACE_CTL_RB_BASE_ADDR, HAL_CFG_CPU_SAMPLE_MODE_REG_OFFSET, HAL_CFG_CPU_SAMPLE_DEFAULT);
    reg_writew(HAL_CPU_TRACE_CTL_RB_BASE_ADDR, HAL_CFG_CPU_TRACE_REG_OFFSET, HAL_CFG_CPU_TRACE_REG_DEFAULT);
#elif (CORE == WIFI)
    regw_clrbit((void *)(uintptr_t)HAL_CPU_TRACE_CTL_RB_BASE_ADDR, HAL_CFG_CPU_SAMPLE_MODE_REG_OFFSET,
                HAL_CFG_CPU_SAMPLE_EN_BIT);
    regw_clrbit((void *)(uintptr_t)HAL_CPU_TRACE_CTL_RB_BASE_ADDR, HAL_CFG_CPU_MONITOR_CLOCK_REG_OFFSET,
                HAL_CFG_CPU_TRACE_CLOCK_EN_BIT);
    regw_setbit((void *)(uintptr_t)HAL_CPU_TRACE_CTL_RB_BASE_ADDR, HAL_CFG_CPU_TRACE_MEM_CLK_REG_OFFSET,
                HAL_CFG_CPU_TRACE_RAM_SEL_BIT);
#else
    reg_writew(HAL_CPU_TRACE_CTL_RB_BASE_ADDR, HAL_CFG_CPU_SAMPLE_MODE_REG_OFFSET, HAL_CFG_CPU_SAMPLE_DEFAULT);
    reg_writew(HAL_CPU_TRACE_CTL_RB_BASE_ADDR, HAL_CFG_CPU_TRACE_MEM_CLK_REG_OFFSET, HAL_CFG_CPU_TRACE_RAM_SEL);
#endif
    g_cputrace_running = false;
}

void hal_cpu_trace_clock_enable(void)
{
#if CORE == APPS
    /* select cpu trace function */
    regw_clrbit((void *)(uintptr_t)HAL_CPU_TRACE_CTL_RB_BASE_ADDR, HAL_CFG_MONITOR_SEL_REG_OFFSET,
                HAL_CFG_MONITOR_SEL_BIT);
#endif
#if CORE == WIFI
    regw_setbit((void *)(uintptr_t)HAL_CPU_TRACE_CTL_RB_BASE_ADDR, HAL_CFG_CPU_TRACE_DIAG_EN_REG_OFFSET,
                HAL_CFG_CPU_TRACE_DIAG_EN_BIT);
    regw_clrbit((void *)(uintptr_t)HAL_CPU_TRACE_CTL_RB_BASE_ADDR, HAL_CFG_CPU_TRACE_MEM_CLK_REG_OFFSET,
                HAL_CFG_CPU_TRACE_RAM_SEL_BIT);
#endif
    /* enable cpu trace clock */
    reg_writew(HAL_CPU_TRACE_CTL_RB_BASE_ADDR, HAL_CFG_CPU_MONITOR_CLOCK_REG_OFFSET, HAL_CFG_CPU_TRACE_CLK_EN_CFG);
}

void hal_cpu_trace_clock_disable(void)
{
    reg_writew(HAL_CPU_TRACE_CTL_RB_BASE_ADDR, HAL_CFG_CPU_MONITOR_CLOCK_REG_OFFSET, 0x0);
}

void hal_cpu_trace_set_sample_mode(hal_cpu_trace_sample_mode_t sample_mode)
{
    regw_setbits((void *)(uintptr_t)HAL_CPU_TRACE_CTL_RB_BASE_ADDR, HAL_CFG_CPU_SAMPLE_MODE_REG_OFFSET,
                 HAL_CFG_CPU_SAMPLE_MODE_BITS_OFFSET, HAL_CFG_CPU_SAMPLE_MODE_BITS_LEN, (uint16_t)sample_mode);
}

#if (CORE == APPS) || (CORE == BT) || (CORE == CONTROL_CORE)
bool hal_cpu_trace_set_sample_addr(uint32_t start_addr, uint32_t end_addr)
{
    if (start_addr > end_addr) {
        return false;
    }

    // address must be divisibled by 4,length align to 12 bytes
    if (((start_addr & 0x3) != 0) || ((end_addr & 0x3) != 0) || (((end_addr - start_addr) % 12) != 0)) {
        return false;
    }
    uint32_t length = (end_addr - start_addr) / sizeof(uint32_t);
    uint16_t addr_start_low_16 = get_low_16bits(start_addr);    // addr_start  16~31 bits
    uint16_t addr_start_high_16 = get_high_16bits(start_addr);   // addr_start  0~15 bits
    uint16_t addr_end_low_16 = get_low_16bits(end_addr);        // addr_end 0~15 bits
    uint16_t addr_end_high_16 = get_high_16bits(end_addr);      // addr_end 16~31 bits
    uint16_t length_low_16 = get_low_16bits(length);            // length 0~15 bits
    uint16_t length_high_16 = get_high_16bits(length);          // length 16~31 bits

    /* set the address and length */
    reg_writew((void *)(uintptr_t)HAL_CPU_TRACE_CTL_RB_BASE_ADDR, HAL_CFG_CPU_SAMPLE_START_ADDR_L_REG_OFFSET,
               addr_start_low_16);
    reg_writew((void *)(uintptr_t)HAL_CPU_TRACE_CTL_RB_BASE_ADDR, HAL_CFG_CPU_SAMPLE_START_ADDR_H_REG_OFFSET,
               addr_start_high_16);
    reg_writew((void *)(uintptr_t)HAL_CPU_TRACE_CTL_RB_BASE_ADDR, HAL_CFG_CPU_SAMPLE_END_ADDR_L_REG_OFFSET,
               addr_end_low_16);
    reg_writew((void *)(uintptr_t)HAL_CPU_TRACE_CTL_RB_BASE_ADDR, HAL_CFG_CPU_SAMPLE_END_ADDR_H_REG_OFFSET,
               addr_end_high_16);
    reg_writew((void *)(uintptr_t)HAL_CPU_TRACE_CTL_RB_BASE_ADDR, HAL_CFG_CPU_SAMPLE_LENGTH_L_REG_OFFSET,
               length_low_16);
    reg_writew((void *)(uintptr_t)HAL_CPU_TRACE_CTL_RB_BASE_ADDR, HAL_CFG_CPU_SAMPLE_LENGTH_H_REG_OFFSET,
               length_high_16);

    /* check if using the internal ram */
    if ((start_addr >= CPU_TRACE_INTRENAL_MEM_REGION_START) && (end_addr <= CPU_TRACE_INTRENAL_MEM_REGION_END)) {
        regw_clrbit((void *)(uintptr_t)HAL_CPU_TRACE_CTL_RB_BASE_ADDR, HAL_CFG_CPU_TRACE_SAVE_SEL_REG_OFFSET,
                    HAL_CFG_CPU_TRACE_SAVE_SEL_BIT);
    } else {
        regw_setbit((void *)(uintptr_t)HAL_CPU_TRACE_CTL_RB_BASE_ADDR, HAL_CFG_CPU_TRACE_SAVE_SEL_REG_OFFSET,
                    HAL_CFG_CPU_TRACE_SAVE_SEL_BIT);
#if (CORE == BT) || (CORE == CONTROL_CORE)
        hal_cpu_trace_set_external_ram_clk();
#endif
    }
    return true;
}
#endif

static void hal_cpu_trace_clear_lock_trigger(hal_cpu_trace_traced_cpu_t cpu)
{
    uint32_t lock_count = 0;
    uint32_t cpu_trace_base_addr, cpu_load_diag_offset;
    cpu_trace_base_addr = g_trace_value[cpu].cpu_info_base_addr;
    cpu_load_diag_offset = g_trace_value[cpu].cpu_diag_offset;
    regw_setbit(cpu_trace_base_addr, cpu_load_diag_offset, 1);
    do {
        if (reg_getbits((void *)(uintptr_t)cpu_trace_base_addr, cpu_load_diag_offset,
                        HAL_LOAD_DIAG_REG_LOCK_STATUS_BIT, 1) != 0) {
            regw_clrbit((void *)(uintptr_t)cpu_trace_base_addr, cpu_load_diag_offset, 1);
            return;
        }
        lock_count++;
    } while (lock_count < HAL_CPU_LOAD_LOCK_TIMEOUT);
}

static bool hal_cpu_trace_lock_trigger(hal_cpu_trace_traced_cpu_t cpu, bool force_enable)
{
    UNUSED(force_enable);
    uint32_t lock_count = 0;
    uint32_t cpu_trace_base_addr, cpu_load_diag_offset, cpu_load_diag_lock_bit, cpu_load_diag_lock_stat_bit;
    uint32_t cpu_lock_pclr_en;
    if (cpu >= (uint8_t)(sizeof(g_trace_value) / sizeof(g_trace_value[0]))) {
        return false;
    }

    cpu_trace_base_addr = g_trace_value[cpu].cpu_info_base_addr;
    cpu_load_diag_offset = g_trace_value[cpu].cpu_diag_offset;
    cpu_lock_pclr_en = g_trace_value[cpu].cpu_lock_enable;
    cpu_load_diag_lock_bit = HAL_LOAD_DIAG_REG_LOCK_ENABLE_BIT;
    cpu_load_diag_lock_stat_bit = HAL_LOAD_DIAG_REG_LOCK_STATUS_BIT;

    /* enable trace and pc/lr lock */
#if (CHIP_LIBRA && (CORE != GNSS)) || CHIP_SOCMN1
    regw_setbit(HAL_CPU_TRACE_ENABLE, 0, HAL_TRACE_EN_BIT);
#endif
    hal_cpu_trace_set_top_glue_trigger(cpu, true);
    regw_setbit(cpu_trace_base_addr, cpu_lock_pclr_en, HAL_PCLR_EN_BIT);

    /* trigger pc/lr lock */
    regw_setbit(cpu_trace_base_addr, cpu_load_diag_offset, cpu_load_diag_lock_bit);
    dsb();
    /* check the status */
    do {
        if (reg_getbits((void *)(uintptr_t)cpu_trace_base_addr, cpu_load_diag_offset,
                        cpu_load_diag_lock_stat_bit, 1) != 0) {
            regw_clrbit((void *)(uintptr_t)cpu_trace_base_addr, cpu_load_diag_offset, 0);
            regw_setbit((void *)(uintptr_t)cpu_trace_base_addr, cpu_load_diag_offset, 1);
            regw_clrbit((void *)(uintptr_t)cpu_trace_base_addr, cpu_load_diag_offset, 1);
            regw_clrbit(cpu_trace_base_addr, cpu_lock_pclr_en, HAL_PCLR_EN_BIT);  // close lock.
            hal_cpu_trace_set_top_glue_trigger(cpu, false);
            return true;
        }
        lock_count++;
    } while (lock_count < HAL_CPU_LOAD_LOCK_TIMEOUT);
    regw_clrbit(cpu_trace_base_addr, cpu_lock_pclr_en, HAL_PCLR_EN_BIT); // close lock.
    hal_cpu_trace_set_top_glue_trigger(cpu, false);
    return false;
}

static void hal_cpu_trace_get_locked_reg(hal_cpu_trace_traced_cpu_t cpu, uint32_t *pc, uint32_t *lr, uint32_t *sp)
{
    uint16_t pc_l, pc_h;
    uint16_t lr_l, lr_h;
    uint16_t sp_l, sp_h;
    uint32_t cpu_trace_base_addr, cpu_pc_low_offset, cpu_pc_high_offset, cpu_lr_low_offset, cpu_lr_high_offset;
    if (cpu >= (uint8_t)(sizeof(g_trace_value) / sizeof(g_trace_value[0]))) {
        return;
    }
    *sp = 0;
    cpu_trace_base_addr = g_trace_value[cpu].cpu_info_base_addr;
    cpu_pc_low_offset = g_trace_value[cpu].pc_low_offset;
    cpu_pc_high_offset = g_trace_value[cpu].pc_high_offset;
    cpu_lr_low_offset = g_trace_value[cpu].lr_low_offset;
    cpu_lr_high_offset = g_trace_value[cpu].lr_high_offset;

    /* get pc value */
    reg_readw((void *)(uintptr_t)cpu_trace_base_addr, cpu_pc_low_offset, pc_l);
    reg_readw((void *)(uintptr_t)cpu_trace_base_addr, cpu_pc_high_offset, pc_h);
    *pc = comb_to_32bits(pc_h, pc_l);

    /* get lr value */
    reg_readw((void *)(uintptr_t)cpu_trace_base_addr, cpu_lr_low_offset, lr_l);
    reg_readw((void *)(uintptr_t)cpu_trace_base_addr, cpu_lr_high_offset, lr_h);
    *lr = comb_to_32bits(lr_h, lr_l);
#if CHIP_LIBRA
    if (cpu == HAL_CPU_TRACE_TRACED_GCPU) {
        /* get sp value */
        reg_readw((void *)(uintptr_t)HAL_GCPU_TRACE_CTL_RB_BASE_ADDR, HAL_GCPU_SP_L_DIAG_REG_OFFSET, sp_l);
        reg_readw((void *)(uintptr_t)HAL_GCPU_TRACE_CTL_RB_BASE_ADDR, HAL_GCPU_SP_H_DIAG_REG_OFFSET, sp_h);
        *sp = comb_to_32bits(sp_h, sp_l);
    }
#endif
    if (cpu == HAL_CPU_TRACE_TRACED_MCPU) {
        /* get sp value */
        reg_readw((void *)(uintptr_t)HAL_MCPU_TRACE_CTL_RB_BASE_ADDR, HAL_MCPU_SP_L_DIAG_REG_OFFSET, sp_l);
        reg_readw((void *)(uintptr_t)HAL_MCPU_TRACE_CTL_RB_BASE_ADDR, HAL_MCPU_SP_H_DIAG_REG_OFFSET, sp_h);
        *sp = comb_to_32bits(sp_h, sp_l);
    }
    hal_cpu_trace_clear_lock_trigger(cpu);
}

void hal_cpu_trace_lock_pclr(hal_cpu_trace_traced_cpu_t cpu)
{
    hal_cpu_trace_lock_trigger(cpu, false);
}

uint32_t hal_cpu_trace_get_locked_pc(hal_cpu_trace_traced_cpu_t cpu)
{
    uint16_t pc_l, pc_h;
    uint32_t cpu_trace_base_addr, cpu_pc_low_offset, cpu_pc_high_offset;
    if (cpu >= (uint8_t)(sizeof(g_trace_value) / sizeof(g_trace_value[0]))) {
        return 0;
    }

    cpu_trace_base_addr = g_trace_value[cpu].cpu_info_base_addr;
    cpu_pc_low_offset = g_trace_value[cpu].pc_low_offset;
    cpu_pc_high_offset = g_trace_value[cpu].pc_high_offset;

    reg_readw((void *)(uintptr_t)cpu_trace_base_addr, cpu_pc_low_offset, pc_l);
    reg_readw((void *)(uintptr_t)cpu_trace_base_addr, cpu_pc_high_offset, pc_h);
    hal_cpu_trace_clear_lock_trigger(cpu);
    return comb_to_32bits(pc_h, pc_l);
}

uint32_t hal_cpu_trace_get_locked_lr(hal_cpu_trace_traced_cpu_t cpu)
{
    uint16_t lr_l, lr_h;
    uint32_t cpu_trace_base_addr, cpu_lr_low_offset, cpu_lr_high_offset;
    if (cpu >= (uint8_t)(sizeof(g_trace_value) / sizeof(g_trace_value[0]))) {
        return 0;
    }

    cpu_trace_base_addr = g_trace_value[cpu].cpu_info_base_addr;
    cpu_lr_low_offset = g_trace_value[cpu].lr_low_offset;
    cpu_lr_high_offset = g_trace_value[cpu].lr_high_offset;

    reg_readw((void *)(uintptr_t)cpu_trace_base_addr, cpu_lr_low_offset, lr_l);
    reg_readw((void *)(uintptr_t)cpu_trace_base_addr, cpu_lr_high_offset, lr_h);
    hal_cpu_trace_clear_lock_trigger(cpu);
    return comb_to_32bits(lr_h, lr_l);
}

uint32_t hal_cpu_trace_get_locked_sp(hal_cpu_trace_traced_cpu_t cpu)
{
    uint16_t sp_l, sp_h;
#if CHIP_LIBRA
    if (cpu == HAL_CPU_TRACE_TRACED_GCPU) {
        /* get sp value */
        reg_readw((void *)(uintptr_t)HAL_GCPU_TRACE_CTL_RB_BASE_ADDR, HAL_GCPU_SP_L_DIAG_REG_OFFSET, sp_l);
        reg_readw((void *)(uintptr_t)HAL_GCPU_TRACE_CTL_RB_BASE_ADDR, HAL_GCPU_SP_H_DIAG_REG_OFFSET, sp_h);
        return comb_to_32bits(sp_h, sp_l);
    }
#endif
    if (cpu == HAL_CPU_TRACE_TRACED_MCPU) {
        /* get sp value */
        reg_readw((void *)(uintptr_t)HAL_MCPU_TRACE_CTL_RB_BASE_ADDR, HAL_MCPU_SP_L_DIAG_REG_OFFSET, sp_l);
        reg_readw((void *)(uintptr_t)HAL_MCPU_TRACE_CTL_RB_BASE_ADDR, HAL_MCPU_SP_H_DIAG_REG_OFFSET, sp_h);
        return comb_to_32bits(sp_h, sp_l);
    }
    return 0;
}

bool hal_cpu_trace_get_locked_regs(hal_cpu_trace_traced_cpu_t cpu, uint32_t *pc, uint32_t *lr, uint32_t *sp)
{
    uint32_t retry = 0;

    if ((pc == NULL) || (lr == NULL) || (sp == NULL)) {
        return false;
    }

    do {
        /* try to lock registers */
        if (hal_cpu_trace_lock_trigger(cpu, false) != true) {
            return false;
        }

        /* get locked values and check */
        hal_cpu_trace_get_locked_reg(cpu, pc, lr, sp);
        if ((*pc != 0) || (*lr != 0)) {
            return true;
        }
        retry++;
    } while (retry < HAL_CPU_LOAD_RETRY_COUNT);
    return false;
}

void hal_cpu_trace_sample_sync_bit_config(void)
{
#if (CORE == APPS)
    regw_clrbit((void *)(uintptr_t)HAL_CPU_TRACE_CTL_RB_BASE_ADDR, HAL_CFG_CPU_SAMPLE_MODE_REG_OFFSET,
                HAL_CFG_CPU_SAMPLE_SYNC_BIT);
#endif
}
