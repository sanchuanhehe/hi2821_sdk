/*
 * Copyright (c) @CompanyNameMagicTag 2018-2020. All rights reserved.
 * Description:  CHIP Memory Map configuration
 * Author: @CompanyNameTag
 * Create:  2018-10-15
 */

#include "memory_core.h"
#include "non_os.h"

#if defined BUILD_APPLICATION_SSB
// Data structure used to cache memory map KV contents
typedef struct {
    memory_map bt;  // !< Memory configuration for bt core image.
    memory_map protocol;  // !< Memory configuration for hifi Core core image.
    memory_map apps;      // !< Memory configuration for applications core image.
    memory_map recovery;  // !< Memory configuration for recovery core image.
    memory_map extern0;      // !< Memory configuration gnss image for libra.
#if (CHIP == TARGET_CHIP_LIBRA)
    memory_map extern1;         // !< Memory configuration extern1 core image for libra.
    memory_map extern1_ssb;         // !< Memory configuration extern1 core ssb image for libra.
#endif
} memory_config;

// Memory map KV cache
static memory_config g_memory_maps;
static const memory_map g_memory_config_default_map = { 0, 0, 0, 0, 0, 0, 0 };

/*
 * Read the memory map for a core from KV storage
 */
static bool memory_helper_cache_core_map(core_images_e cimage, memory_map *map)
{
    UNUSED(cimage);
    *map = g_memory_config_default_map;
    return false;
}

/*
 * Cache memory maps, from KV store, for all cores
 */
bool memory_cache_all_core_maps(void)
{
    bool config_valid;
    // Only use proper cores for determining validity of memory configuration
    // These 3 run together, and must have a sec A
    config_valid = memory_helper_cache_core_map(CORE_IMAGES_BT, &g_memory_maps.bt);
    config_valid = (memory_helper_cache_core_map(CORE_IMAGES_PROTOCOL, &g_memory_maps.protocol) && config_valid);
    config_valid = (memory_helper_cache_core_map(CORE_IMAGES_APPS, &g_memory_maps.apps) && config_valid);
    config_valid = (memory_helper_cache_core_map(CORE_IMAGES_RECOVERY, &g_memory_maps.recovery) && config_valid);

#if CHIP_SOCMN1 || CHIP_BS25
    config_valid = (memory_helper_cache_core_map(CORE_IMAGES_EXTERN0,
                                                 &g_memory_maps.extern0) && config_valid);
#elif CHIP_LIBRA
    config_valid = (memory_helper_cache_core_map(CORE_IMAGES_EXTERN0,
                                                 &g_memory_maps.extern0) && config_valid);
    config_valid = (memory_helper_cache_core_map(CORE_IMAGES_EXTERN1,
                                                 &g_memory_maps.extern1) && config_valid);
    config_valid = (memory_helper_cache_core_map(CORE_IMAGES_EXTERN1_SSB,
                                                 &g_memory_maps.extern1_ssb) && config_valid);
#endif
    return config_valid;
}

/*
 * Write a cached memory map back to KV storage
 */
bool memory_write_back_core_map(core_images_e cimage)
{
    memory_map *map = memory_get_cached_core_map(cimage);

    if (map != NULL) {
        return true;
    }
    return false;
}

/*
 * Ensure all cached memory maps are written back to KV storage
 */
bool memory_write_back_all_core_maps(void)
{
    bool ret_val = true;
    ret_val = (memory_write_back_core_map(CORE_IMAGES_BT) && ret_val);
    ret_val = (memory_write_back_core_map(CORE_IMAGES_PROTOCOL) && ret_val);
    ret_val = (memory_write_back_core_map(CORE_IMAGES_APPS) && ret_val);
    ret_val = (memory_write_back_core_map(CORE_IMAGES_RECOVERY) && ret_val);
    ret_val = (memory_write_back_core_map(CORE_IMAGES_EXTERN0) && ret_val);
#if (CHIP == TARGET_CHIP_LIBRA)
    ret_val = (memory_write_back_core_map(CORE_IMAGES_EXTERN1) && ret_val);
    ret_val = (memory_write_back_core_map(CORE_IMAGES_EXTERN1_SSB) && ret_val);
#endif
    return ret_val;
}

/*
 * Return the cached memory map for a given core.
 */
memory_map *memory_get_cached_core_map(core_images_e cimage)
{
    memory_map *map = NULL;

    switch (cimage) {
        case CORE_IMAGES_BT:
            map = &g_memory_maps.bt;
            break;
        case CORE_IMAGES_PROTOCOL:
            map = &g_memory_maps.protocol;
            break;
        case CORE_IMAGES_APPS:
            map = &g_memory_maps.apps;
            break;
        case CORE_IMAGES_RECOVERY:
            map = &g_memory_maps.recovery;
            break;
        case CORE_IMAGES_EXTERN0:
            map = &g_memory_maps.extern0;
            break;
#if (CHIP == TARGET_CHIP_LIBRA)
        case CORE_IMAGES_EXTERN1:
            map = &g_memory_maps.extern1;
            break;
        case CORE_IMAGES_EXTERN1_SSB:
            map = &g_memory_maps.extern1_ssb;
            break;
#endif
        default:
            break;
    }

    return map;
}

/*
 * Update the cached CODE region for a given core.
 */
void memory_update_cached_core_code_region(core_images_e cimage, uint32_t start,
                                           uint32_t length, uint32_t section_length)
{
    memory_map *map = memory_get_cached_core_map(cimage);

    if (map != NULL) {
        map->code_addr = start;
        map->code_length = length;
        map->code_section_length = section_length;
    }
}

/*
 * Set the cached RAM region for a given core.
 */
void mem_update_cached_core_ram_region(core_images_e cimage, uint32_t start, uint32_t length)
{
    memory_map *map = memory_get_cached_core_map(cimage);

    if (map != NULL) {
        map->ram_addr = start;
        map->ram_length = length;
    }
}

/*
 * Update the OTP region for a given core.
 */
void memory_update_cached_core_otp_region(core_images_e cimage, uint16_t start, uint16_t length)
{
    memory_map *map = memory_get_cached_core_map(cimage);

    if (map != NULL) {
        map->otp_start = start;
        map->otp_length = length;
    }
}
#endif
