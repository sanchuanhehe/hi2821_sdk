#!/usr/bin/env python3
# encoding=utf-8
# =========================================================================
# @brief    Target Definitions File
# Copyright CompanyNameMagicTag 2022-2022. All rights reserved.
# =========================================================================
import os
import copy

target_template = {
    'target_bs21_application_template': {
        'chip': 'bs21',
        'core': 'acore',
        'board': 'evb',
        'tool_chain': 'riscv32_musl_b090_fp',
        'build_type': 'COMPILE',
        'os': 'liteos',
        'std_libs': [],
        'CONFIG_TIMER_USING_V150': 'y',
        'CONFIG_I2C_USING_V151': 'y',
        'defines': ['-:CHIP_BS21=1', 'LIBCPU_UTILS', 'LIBLIB_UTILS', 'PRE_ASIC', 'LIBPANIC', 'LIBAPP_VERSION',
                    'VERSION_STANDARD', 'LIBBUILD_VERSION', "CONFIG_UART_FIFO_DEPTH=64",
                    'LIBTEST_COMMON', '_ALL_SOURCE', 'LOSCFG_MEM_TASK_STAT', 'SUPPORT_CXX', 'BS21_PRODUCT_EVB',
                    'LIBLOG', 'LIBLOG_READER', 'USE_LITEOS', 'USE_VECTORS', 'BUILD_APPLICATION_STANDARD', 'CMD_ENABLE',
                    '__LITEOS__', 'LIBCPU_LOAD', 'LOSCFG_DRIVERS_EMMC', 'UART_DRIVER_CONFIG_USE_VETOS_IN_STEAD_OF_TIMERS',
                    'USE_CMSIS_OS', 'LIBCMD', 'LITEOS_ONETRACK', 'LIBCONNECTIVITY', 'LOSCFG_FS_FAT_CACHE', 'DUMP_MEM_SUPPORT',
                    "CONFIG_NV_SUPPORT_SINGLE_CORE_SYSTEM", "CONFIG_ZDIAG_NV_SUPPORT", 'CONFIG_NV_FEATURE_SUPPORT',
                    'LIBUTIL_COMPAT', "LITEOS_208", "BTH_LOG_BUG_FIX", 'OSALLOG_DISABLE', 'SUPPORT_DIAG_V2_PROTOCOL'],
        'defines_set': ['libsec_defines', 'chip_defines', 'version_defines'],
        'ram_component': ['arch_port', 'pmu_porting', 'libboundscheck', 'non_os', 'board_config', 'ulp_aon', 'osal',
                          'clocks_porting', 'error_code', 'hal_mips', 'mem_config_porting',
                          'dfx_exception', 'lpm', 'chip_porting', 'cmn_header', 'driver_header',
                          'hal_uart', 'uart', 'uart_port', '-:connectivity',
                          'gmssl_hmac_sm3', 'systick', 'tcxo', 'hal_systick', 'hal_tcxo', '-:lib_utils', 'porting_inc',
                          'segger_b090_fp', 'usb_class', 'usb_class_open', 'usb_class_header', 'test_usb_unified',
                          'sec_trng', 'hal_sec_trng2', 'sec_trng', 'sec_common', 'hal_sec_common', 'sec_port',
                          'hal_trng', 'trng', 'trng_port', 'nv', 'nv_porting', 'partition', 'partition_porting',
                          'at', 'at_cmd_port', 'at_plt_cmd', 'at_btc_cmd',],
        'ram_component_set': ['cpu', 'i2c', 'lpc', 'cpu_trace', 'qspi', 'gpio_v150', 'time_set', 'otp', 'mem',
                              'watchdog', 'pinctrl', 'pm_set', 'usb_unified', 'pmp_set', 'pm_clock_set', 'pm_pmu_set'],
        'rom_component': [
        ],
        'bin_name': 'application',
        'ccflags': [
            '-:-mabi=ilp32', '-:-march=rv32imc', '-mabi=ilp32f', '-march=rv32imfc', '-madjust-regorder', '-madjust-const-cost', '-freorder-commu-args', '-fimm-compare-expand',
            '-frmv-str-zero', '-mfp-const-opt', '-mswitch-jump-table', '-frtl-sequence-abstract',
            '-frtl-hoist-sink', '-fsafe-alias-multipointer', '-finline-optimize-size', '--short-enums',
            '-fmuliadd-expand', '-mlli-expand', '-Wa,-mcjal-expand', '-foptimize-reg-alloc', '-fsplit-multi-zero-assignments',
            '-floop-optimize-size',  '-mpattern-abstract', '-foptimize-pro-and-epilogue', '-:-Wno-type-limits'
        ],
        'rom_ccflags': [
            '-:-mabi=ilp32', '-:-march=rv32imc', '-mabi=ilp32f', '-march=rv32imfc', '-madjust-regorder', '-madjust-const-cost', '-freorder-commu-args', '-fimm-compare-expand',
            '-frmv-str-zero', '-mfp-const-opt', '-frtl-sequence-abstract',
            '-frtl-hoist-sink', '-fsafe-alias-multipointer', '-finline-optimize-size',
            '-fmuliadd-expand', '-mlli-expand', '-Wa,-mcjal-expand', '-foptimize-reg-alloc', '-fsplit-multi-zero-assignments',
            '-floop-optimize-size', '-foptimize-pro-and-epilogue', '--short-enums',
            "-fno-inline-functions-called-once", "-fno-inline-small-functions", '-:-Wno-type-limits'
        ],
        'linkflags': [
            # '-Wl,--jal-transfer',
            '-Wl,--cjal-relax',
            '-Wl,--dslf',
        ],
        'application': 'standard',
        'hso_enable_bt': True,
        'hso_enable': True,
        'gen_parse_tool': False,
        'build_rom_callback': True,
        'fixed_rom': True,
        'fixed_rom_path': '<root>/interim_binary/bs21/bin/rom_bin/application_rom.bin',
        'rom_sym_path': '<root>/drivers/chips/bs21/rom_info/acore/acore.sym',
        'rom_ram_check': True,
        'rom_ram_compare': True,
        'packet': True,
        'patch': True,
        'arch': 'riscv31',
        'sector_cfg': 'bs21-standard-512k',
        'nv_cfg': 'bs21_nv_default'
    },
}
