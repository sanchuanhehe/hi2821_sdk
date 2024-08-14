/*
 * Copyright (c) CompanyNameMagicTag 2018-2020. All rights reserved.
 * Description:  Security core version table.
 * Author:
 * Create:
 */

#include <stdbool.h>
#include <stddef.h>
#include "build_version_info.h"
#include "application_version.h"
/*lint -e528 */
static startup_table g_application_to_ssb_table;
static bool g_ssb_table_valid = false;

/*
 * The updater core needs to be able to read and modify this table,
 * the SSB only need to write, the security core only needs to read
 */
#if (defined BUILD_APPLICATION_SSB) && (CORE != SECURITY)
#include "build_version.h"
#include "securec.h"
#include "cpu_utils.h"
#ifndef APP_SUPPORT_READ_FLASH_ID
#include "flash.h"
#endif
#if (EMBED_FLASH_EXIST == YES)
#include "bootloader_configuration.h"
#endif

/**
 * For the checksum to work we need to set both the length and the checksum
 */
static void set_application_table_checksum(void)
{
    uint32_t checksum = 0;
    uint32_t *table = (uint32_t *)&g_application_to_ssb_table;
    uint32_t i;

    g_application_to_ssb_table.ssb_to_application_length = sizeof(g_application_to_ssb_table) / sizeof(uint32_t);
    for (i = SSB_TO_APPLICATION_DATA_START; i < g_application_to_ssb_table.ssb_to_application_length; i++) {
        checksum ^= table[i];
    }
    checksum ^= table[SSB_TO_APPLICATION_LEN];
    checksum ^= table[SSB_TO_APPLICATION_SSB_VSN];
    g_application_to_ssb_table.ssb_to_application_checksum = checksum;
}

void create_ssb_to_application_table(uint32_t reboot_cause_var)
{
    const build_version_info *info = build_version_get_info();
#ifndef APP_SUPPORT_READ_FLASH_ID
    flash_info_t flash_info;
#endif

    memset_s((uint32_t *)&g_application_to_ssb_table, sizeof(g_application_to_ssb_table),
             0, sizeof(g_application_to_ssb_table));  //lint !e740 unusual cast // ensure it is init'd to all 0

    g_application_to_ssb_table.ssb_to_application_ssb_version = info->version;
    g_application_to_ssb_table.ssb_to_application_reboot_cause = reboot_cause_var;

#ifndef APP_SUPPORT_READ_FLASH_ID
    uapi_flash_get_info(CHIP_FLASH_ID, &flash_info);
    g_application_to_ssb_table.ssb_to_application_flash_id = flash_info.flash_id;
    g_application_to_ssb_table.ssb_to_application_flash_unique_id = flash_info.flash_unique_id;
#endif
    g_ssb_table_valid = true;
}

#ifdef EPMU_EVENT_SSB_TO_APP
void set_ssb_to_application_epmu_event(epmu_event_t *event)
{
    if (g_ssb_table_valid) {
        if (memcpy_s(&(g_application_to_ssb_table.ssb_to_application_epmu_event), sizeof(epmu_event_t),
            event, sizeof(epmu_event_t)) != EOK) {
            return;
        }
    }
}
#endif

void set_ssb_to_application_image_vaild(uint32_t security_valid, uint32_t protocol_valid)
{
    if (g_ssb_table_valid) {
        g_application_to_ssb_table.ssb_to_application_security_valid = security_valid;
        g_application_to_ssb_table.ssb_to_application_protocol_valid = protocol_valid;
    }
}

void set_ssb_to_application_mcu_freq(uint32_t ssb_freq)
{
    if (g_ssb_table_valid) {
        g_application_to_ssb_table.ssb_to_application_mcu_freq = ssb_freq;
    }
}

void get_ssb_application_table(uint32_t **table_addr, uint32_t *table_length)
{
#if ((defined EMBED_FLASH_EXIST) && (EMBED_FLASH_EXIST == YES))
    g_application_to_ssb_table.ssb_to_application_embed_flash_exist = bootloader_configuration_get_flash_mode();;
#endif
    set_application_table_checksum();  // Ensure that the checksum has been set before sending it to the next core
    *table_addr = (uint32_t *)&g_application_to_ssb_table;
    *table_length = g_application_to_ssb_table.ssb_to_application_length;
}

#endif  // (defined BUILD_APPLICATION_SSB) || (defined BUILD_APPLICATION_UPDATER)

startup_table *application_version_get_table(void)
{
    if (g_ssb_table_valid) {
        return (startup_table *)&g_application_to_ssb_table;
    } else {
        return (startup_table *)NULL;
    }
}