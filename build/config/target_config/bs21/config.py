#!/usr/bin/env python3
# encoding=utf-8
# ============================================================================
# @brief    Target Definitions File
# Copyright CompanyNameMagicTag 2022-2022. All rights reserved.
# ============================================================================

target = {
    'bs21-sle-ble-central-peripheral': {
        'base_target_name': 'target_bs21_application_template',
        'defines': ['SUPPORT_EXTERN_FLASH', 'SUPPORT_BT_UPG', 'PLT_TEST_ENABLE', 'BGLE_TASK_EXIST', 'PRE_ASIC', 'BS21_PRODUCT_EVB',
                    'AT_ONLY', '-:SW_UART_DEBUG', 'AT_COMMAND', 'XO_32M_CALI', 'TEST_SUITE','SAMPLE_SUPPORT_CMD'],
        'ram_component': [
            'test_adc', 'demo','test_i2s', 'test_qdec', 'test_keyscan', 'std_rom_lds_porting', 'i2s', 'standard_porting', 'liteos_208_6_0_b017', 'dfx_porting', 'algorithm', 'test_pinctrl', 'sfc_porting',
            'systick_port', 'tcxo_port', 'bg_common', 'bt_host', 'bth_sdk', 'samples', 'bts_header', 'bth_gle', 'bt_app', 'bgtp', 'mips', 'drv_timer', 'hal_timer', 'rtc_unified', 'hal_rtc_unified',
            'rtc_unified_port', 'timer_port', "test_pdm", '-:libboundscheck', 'app_init', 'update_common', 'update_local', 'update_storage',  'lzma_21.07', 'update_common_porting',
                          'update_storage_porting', 'ota_upgrade', 'pm_sys'
        ],
        'rom_component': ['bgtp_rom', 'libboundscheck', 'bt_host_rom', 'bg_common_rom'],
        'ram_component_set' : [
            'efuse_v151', 'spi', 'qdec', 'pdm', 'sio_v151', 'xip_os', 'dmav151', 'keyscan', 'std_common_lib',
            '-:connectivity', '-:time_set', 'pm_set', 'dfx_set', 'sfc_flash', 'adc', 'flash', 'pm_clock_set'
        ],
        'board': 'evb',
        'sector_cfg': 'bs21-extern-flash',
        'upg_pkg': ['application']
    },
}

# custom copy rules, put it in target_group below and it takes effect.
# <root> means root path
# <out_root> means output_root path
# <pack_target> means target_group key_name (like pack_bs21_standard)
target_copy = {

}

target_group = {

}
