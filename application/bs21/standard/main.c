/**
 * Copyright (c) @CompanyNameMagicTag 2022-2023. All rights reserved. \n
 *
 * Description: Application core main function for standard \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-07-27, Create file. \n
 */

#include <stdint.h>
#include "irmalloc.h"
#include "msg_chl.h"
#include "nv.h"
#include "nv_upg.h"
#include "string.h"
#include "rtc.h"
#include "timer.h"
#include "timer_porting.h"
#include "app_os_init.h"
#include "log_common.h"
#include "gpio.h"
#include "chip_io.h"
#include "lpc.h"
#include "memory_core.h"
#include "lpc_core.h"
#ifdef XO_32M_CALI
#include "efuse.h"
#endif
#include "i2c.h"
#include "clocks.h"
#include "uart.h"
#include "log_uart.h"
#include "log_memory_region.h"
#include "securec.h"
#include "pinctrl_porting.h"
#include "pinctrl.h"
#include "tcxo.h"
#include "cpu_trace.h"
#include "application_version.h"
#include "pmu.h"
#include "systick.h"
#include "watchdog.h"
#if CHIP_ASIC
#include "clock_calibration.h"
#endif
#include "otp.h"
#ifdef CONFIG_SYSTEM_VIEW
#include "SEGGER_SYSVIEW_Conf.h"
#include "SEGGER_SYSVIEW.h"
#endif
#include "cpu_utils.h"
#include "exception.h"
#include "log_oml_exception.h"
#include "los_task_pri.h"
#include "los_init_pri.h"
#include "los_hw.h"
#include "oam_trace.h"
#ifdef TEST_SUITE
#include "test_suite.h"
#include "test_suite_uart.h"
#endif
#ifdef AT_COMMAND
#include "at_product.h"
#include "at_porting.h"
#endif
#if (ENABLE_LOW_POWER == YES)
#include "pmu_interrupt.h"
#include "pm.h"
#include "pm_veto.h"
#endif
#include "sfc.h"
#include "arch_barrier.h"
#include "watchdog_porting.h"
#include "dfx_system_init.h"
#if defined(DEVICE_ONLY) && !defined(TEST_SUITE)
#include "wvt_uart.h"
#endif

#include "debug_print.h"
#include "pmp_porting.h"
#include "drv_pmp.h"
#include "patch.h"
#include "preserve.h"
#ifdef SUPPORT_BT_UPG
#include "upg.h"
#include "partition.h"
#include "ota_upgrade.h"
#endif
#ifdef OS_DFX_SUPPORT
#include "os_dfx.h"
#endif
#if defined(CONFIG_SECURE_STORAGE_SUPPORT)
#include "security_init.h"
#endif
#if defined(SUPPORT_EXTERN_FLASH)
#include "flash.h"
#include "flash_porting.h"
#endif

#define WDT_TIMEOUT_S 30
#define IMG_NUM_MAX   2
#define IMG_SEC       0
#define IMG_HIFI      1
#define NFC_SOFT_RST              (GLB_CTL_D_RB_BASE + 0x0200)
#define DTCM_SHARE_MODE           0xF90
#define SHARE_MODE_GT             12
#define SHARE_MODE_CFG            8

#define CPU_TRACE_CLK_REG        0x5200410c
#define CPU_TRACE_CLK_VAL        0x1ff

#define PMP_REGION1_END         0x40000
#define PMP_REGION2_END         0x58000
#define PMP_REGION3_END         0x20000000
#define PMP_REGION4_END         0x20010000
#define PMP_REGION5_END         0x90100000
#if defined (FLASH_1M)
#define PMP_REGION6_END         0x901FE000
#define PMP_REGION7_END         0x90200000
#else
#define PMP_REGION6_END         0x9017E000
#define PMP_REGION7_END         0x90180000
#endif

#define PATCH_NUM 128
#define PATCH_CMP_HEADINFO_NUM 3
static uint32_t patch_remap[PATCH_NUM] __attribute__((section(".patch_remap"))) = { 0 };
static uint32_t patch_cmp[PATCH_NUM + PATCH_CMP_HEADINFO_NUM] __attribute__((section(".patch_cmp"))) = { 0 };

typedef enum {
    PMP_ROM_ID1,
    PMP_ITCM_ID2,
    PMP_UNDEF_MEM_ID3,
    PMP_DTCM_ID4,
    PMP_UNDEF_MEM_ID5,
    PMP_SFC_IDX6,
    PMP_BT_SEC_IDX7,
    MAX_REGION_NUM
} region_index_t;


#ifdef NFC_TASK_EXIST
void NFC_Init(void);
#endif

/*
 *  bs25 support pmp_attr：
 *  PMP_ATTR_DEVICE_NO_BUFFERABLE
 *  PMP_ATTR_NO_CACHEABLE_AND_BUFFERABLE
 *  PMP_ATTR_WRITEBACK_RALLOCATE
 *  PMP_ATTR_WRITEBACK_RWALLOCATE
 */
static void app_mpu_enable(void)
{
    uapi_pmp_reset_cfg();
    pmp_conf_t region_attr[MAX_REGION_NUM] = {
        /* rom pmp */
        {
            PMP_ROM_ID1, PMP_REGION1_END, 0,
            { PMPCFG_READ_ONLY_EXECUTE, PMPCFG_ADDR_MATCH_TOR, true, PMP_ATTR_NO_CACHEABLE_AND_BUFFERABLE },
        },
        /* itcm: .ramtext .rodata */
        {
            PMP_ITCM_ID2, PMP_REGION2_END, 0,
            { PMPCFG_RW_EXECUTE, PMPCFG_ADDR_MATCH_TOR, true, PMP_ATTR_NO_CACHEABLE_AND_BUFFERABLE },
        },
        /* undef mem region */
        {
            PMP_UNDEF_MEM_ID3, PMP_REGION3_END, 0,
            { PMPCFG_RW_NEXECUTE, PMPCFG_ADDR_MATCH_TOR, true, PMP_ATTR_DEVICE_NO_BUFFERABLE },
        },
        /* dtcm: .data .bss .stack .heap */
        {
            PMP_DTCM_ID4, PMP_REGION4_END, 0,
            { PMPCFG_RW_NEXECUTE, PMPCFG_ADDR_MATCH_TOR, true, PMP_ATTR_NO_CACHEABLE_AND_BUFFERABLE },
        },
        /* undef mem region */
        {
            PMP_UNDEF_MEM_ID5, PMP_REGION5_END, 0,
            { PMPCFG_RW_NEXECUTE, PMPCFG_ADDR_MATCH_TOR, true, PMP_ATTR_DEVICE_NO_BUFFERABLE },
        },
        /* sfc code region */
        {
            PMP_SFC_IDX6, PMP_REGION6_END, 0,
            { PMPCFG_RW_EXECUTE, PMPCFG_ADDR_MATCH_TOR, true, PMP_ATTR_WRITEBACK_RWALLOCATE },
        },
        /* nv data */
        {
            PMP_BT_SEC_IDX7, PMP_REGION7_END, 0,
            { PMPCFG_RW_NEXECUTE, PMPCFG_ADDR_MATCH_TOR, true, PMP_ATTR_NO_CACHEABLE_AND_BUFFERABLE },
        },
    };
    uapi_pmp_config((pmp_conf_t *)region_attr, MAX_REGION_NUM);
    ArchICacheFlush();
    ArchDCacheFlush();
    ArchICacheEnable(CACHE_8KB);
    ArchICachePrefetchEnable(CACHE_PREF_1_LINES);
    ArchDCacheEnable(CACHE_4KB);
}

static void func_patch_init(void)
{
    riscv_cfg_t patch_cfg;
    patch_cfg.cmp_start_addr = (uint32_t)(uintptr_t)((void*)patch_cmp);
    patch_cfg.remap_addr = (uint32_t)(uintptr_t)((void*)patch_remap);
    patch_cfg.off_region = false;
    patch_init(patch_cfg);
}

static void hardware_config_init(void)
{
    uapi_gpio_init();
    size_t pin_num;
    hal_pio_config_t *pio_func = NULL;
    bool result = false;

    get_pio_func_config(&pin_num, &pio_func);
    for (pin_t i = S_MGPIO0; i < pin_num; i++) {
        if (pio_func[i].func != HAL_PIO_FUNC_INVALID) {
            if (pio_func[i].func == HAL_PIO_FUNC_DEFAULT_HIGH_Z) {
                result = uapi_pin_set_mode(i, (pin_mode_t)HAL_PIO_FUNC_GPIO);
                uapi_gpio_set_dir(i, GPIO_DIRECTION_INPUT);
            } else {
                result = uapi_pin_set_mode(i, (pin_mode_t)pio_func[i].func);
                result = uapi_pin_set_ds(i, (pin_drive_strength_t)pio_func[i].drive);
            }
            if (pio_func[i].pull != HAL_PIO_PULL_MAX) {
                uapi_pin_set_pull(i, (pin_pull_t)pio_func[i].pull);
            }
#if defined(CONFIG_PINCTRL_SUPPORT_IE)
            if (pio_func[i].ie != PIN_IE_MAX) {
                uapi_pin_set_ie(i, 0x1);
            }
#endif /* CONFIG_PINCTRL_SUPPORT_IE */
            UNUSED(result);
        } else {
#if defined(CONFIG_PINCTRL_SUPPORT_IE)
            uapi_pin_set_ie(i, PIN_IE_0);
#endif /* CONFIG_PINCTRL_SUPPORT_IE */
        }
    }
}

static void watchdog_init(void)
{
    watchdog_turnon_clk();
    watchdog_func_adapt(CHIP_WDT_TIMEOUT_32S);
    uapi_watchdog_init(CHIP_WDT_TIMEOUT_32S);
    uapi_watchdog_enable(CHIP_WDT_MODE_INTERRUPT);
}

static void app_upg_init(void)
{
#ifdef SUPPORT_BT_UPG
    uapi_partition_init();
    upg_func_t upg_func = {0};
    upg_func.malloc = malloc;
    upg_func.free = free;
    upg_func.serial_putc = NULL;
    (void)uapi_upg_init(&upg_func);
    uapi_upgrade_init();
#endif
}

static void bt_em_mem_enanble(void)
{
    regw_setbits(M_CTL_RB_BASE, DTCM_SHARE_MODE, SHARE_MODE_GT, 0x2, 0x0);
#ifdef EM_32K_SUPPORT
    regw_setbits(M_CTL_RB_BASE, DTCM_SHARE_MODE, SHARE_MODE_CFG, 0x2, 0x3);
#else
    regw_setbits(M_CTL_RB_BASE, DTCM_SHARE_MODE, SHARE_MODE_CFG, 0x2, 0x1);
#endif
    regw_setbits(M_CTL_RB_BASE, DTCM_SHARE_MODE, SHARE_MODE_GT, 0x2, 0x3);
}

#ifdef NFC_TASK_EXIST
static void nfc_soft_reset(void)
{
    reg_setbit(NFC_SOFT_RST, 0, 0);
    reg_setbit(NFC_SOFT_RST, 0, 1);
}
#endif

static void ota_extern_flash_init(void)
{
#if defined(SUPPORT_EXTERN_FLASH)
    flash_porting_pinmux_cfg(0);
    uint32_t manufacture_id = 0;
    uapi_flash_init(0);
    uapi_flash_read_id(0, &manufacture_id);
    flash_save_manufacturer(0, manufacture_id);
#endif
}

static void chip_hw_init(void)
{
    sfc_flash_config_t sfc_cfg = {
        .read_type = FAST_READ_QUAD_OUTPUT,
        .write_type = PAGE_PROGRAM,
        .mapping_addr = FLASH_START,
        .mapping_size = FLASH_LEN,
    };
    uapi_sfc_init(&sfc_cfg);
    for (int i = 0; i < 20000; i++) {   // 20000: SFC重复初始化后delay ~937us。
        nop();
    }
    pmu_init();
    clocks_init();
    uapi_tcxo_init();
    uapi_systick_init();
    uapi_timer_init();
    uapi_timer_adapter(TIMER_INDEX_0, TIMER_0_IRQN, OS_HWI_PRIO_LOWEST - 1);
    panic_init();
    uapi_pin_init();
    hardware_config_init();
    uapi_nv_init();
    /* nv init must be before nv_upg_upgrade_task_process */
    (void)nv_upg_upgrade_task_process();
#ifdef XO_32M_CALI
    uapi_efuse_init();
#endif
    panic_init();

#ifdef SW_UART_DEBUG
    sw_debug_uart_init(SW_UART_BAUDRATE);
    PRINT("Debug uart init succ.\r\n");
#endif

#ifdef TEST_SUITE
    test_suite_uart_init();
    uapi_test_suite_init();
#endif

#if defined(AT_COMMAND) && defined(AT_ONLY)
    uapi_at_cmd_init();
#endif

#if defined(DEVICE_ONLY) && !defined(TEST_SUITE)
    wvt_uart_init();
#endif

#if (ENABLE_LOW_POWER == YES)
    /* Init pmu interrupts, sleep and wakeup interrupts are included. */
    pmu_init_interrupts();
    uapi_pm_lpc_init();
#endif
    ota_extern_flash_init();
}

static void chip_sw_init(void)
{
    cpu_utils_init();
    /* Initialise malloc */
    irmalloc_init_default();
#ifdef SUPPORT_DFX_LOG
    log_memory_region_init();
    log_init();
    log_init_after_rtos();
    log_uart_init();
    log_uart_init_after_rtos();
#endif

#ifdef CONFIG_SYSTEM_VIEW
    SEGGER_SYSVIEW_Conf();
#endif

#ifdef HSO_SUPPORT
    dfx_system_init();
    hal_register_exception_dump_callback(log_exception_dump);
    app_upg_init();
#endif

    /* Enable MCPU trace */
    if (get_cpu_utils_reset_cause() != REBOOT_CAUSE_APPLICATION_STD_CHIP_WDT_FRST) {
        cpu_trace_enable_mcpu_repeat();
#ifdef OS_DFX_SUPPORT
        os_dfx_trace_init();
        LOS_TaskSwitchHookReg(os_dfx_task_switch_trace);
        LOS_HwiPreHookReg(os_dfx_hwi_pre);
        LOS_HwiPostHookReg(os_dfx_hwi_post);
#endif
    } else {
        writel(CPU_TRACE_CLK_REG, CPU_TRACE_CLK_VAL);
    }
}

static void chip_rf_init(void)
{
    /* bt msg queue init. */
    LOS_MsgChl_Init();
    bt_em_mem_enanble();

#ifdef NFC_TASK_EXIST
    nfc_soft_reset();
    NFC_Init();
#endif

#ifdef EDA_TEST
    bt_eda_init();
#endif
}

static uint32_t rtos_kernel_init(void)
{
    BoardConfig();
    if (OsMain() == LOS_OK) {
        /* set exception and nmi entry. */
        ArchSetExcHook((EXC_PROC_FUNC)do_fault_handler);
        ArchSetNMIHook((NMI_PROC_FUNC)do_hard_fault_handler);
        return ERRCODE_SUCC;
    } else {
        return ERRCODE_FAIL;
    }
}

static void rtos_kernel_start(void)
{
    OsStart();
}

void main(const void *startup_details_table)
{
    UNUSED(startup_details_table);
    func_patch_init();
    watchdog_init();
    app_mpu_enable();
#if defined(CONFIG_SECURE_STORAGE_SUPPORT)
    uapi_drv_cipher_env_init();
#endif
    /* rtos kernel init. */
    if (rtos_kernel_init() == ERRCODE_SUCC) {
        /* hardware & software init. */
        chip_hw_init();
        chip_sw_init();
        chip_rf_init();
        /* rtos thread init. */
        app_os_init();
        /* rtos kernel start. */
        rtos_kernel_start();
    }

    for (;;) { }
}
