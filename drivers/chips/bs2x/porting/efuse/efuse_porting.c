/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Provides efuse port template \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-3-4， Create file. \n
 */
#include "hal_efuse_v151.h"
#include "efuse_porting.h"

#define EFUSE0_BASE_ADDR             0x57028000
#define HAL_EFUSE0_WRITE_BASE_ADDR   (EFUSE0_BASE_ADDR + 0x800)
#define HAL_EFUSE0_READ_BASE_ADDR    (EFUSE0_BASE_ADDR + 0x800)

uint32_t g_efuse_boot_done_addr = EFUSE0_BASE_ADDR + 0x2c;
#if defined(CONFIG_EFUSE_SWITCH_EN)
uint32_t g_efuse_switch_en_addr = 0x5702C258;
#endif
uint32_t g_efuse_base_addr[CONFIG_EFUSE_REGION_NUM] = {EFUSE0_BASE_ADDR + 0x30};
uint32_t g_efuse_region_read_address[CONFIG_EFUSE_REGION_NUM] = {HAL_EFUSE0_READ_BASE_ADDR};
uint32_t g_efuse_region_write_address[CONFIG_EFUSE_REGION_NUM] = {HAL_EFUSE0_WRITE_BASE_ADDR};

void efuse_port_register_hal_funcs(void)
{
    hal_efuse_register_funcs(hal_efuse_funcs_get());
}

void efuse_port_unregister_hal_funcs(void)
{
    hal_efuse_unregister_funcs();
}

hal_efuse_region_t hal_efuse_get_region(uint32_t byte_addr)
{
    return (hal_efuse_region_t)(byte_addr / EFUSE_REGION_MAX_BYTES);
}

uint16_t hal_efuse_get_byte_offset(uint32_t byte_addr)
{
    return byte_addr % EFUSE_REGION_MAX_BYTES;
}