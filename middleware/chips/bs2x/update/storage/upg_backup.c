/*
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: UPG backup image functions for different chip
 */

#include <stddef.h>
#include <stdint.h>
#include "errcode.h"
#include "common_def.h"
#include "partition.h"
#include "upg_definitions.h"
#include "upg_porting.h"
#include "upg_alloc.h"
#include "upg_common_porting.h"

typedef struct upg_partition_id_ref {
    uint32_t primary_id;
    uint32_t backup_id;
} upg_partition_id_ref_t;

STATIC upg_partition_id_ref_t g_image_refs[] = {
    { PARTITION_FLASHBOOT_IMAGE, PARTITION_FLASHBOOT_BACKUP},
};

errcode_t upg_backup_update(upg_partition_id_ref_t *ids)
{
    partition_information_t primary;
    partition_information_t backup;
    // 获取主区和备份区的地址
    errcode_t ret = uapi_partition_get_info(ids->primary_id, &primary);
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }

    ret = uapi_partition_get_info(ids->backup_id, &backup);
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }

    uint8_t *buffer = upg_malloc(UPG_FLASH_PAGE_SIZE);
    if (buffer == NULL) {
        return ERRCODE_MALLOC;
    }

    // 将主区拷贝到备份区
    uint32_t read_len = 0;
    while (read_len < primary.part_info.addr_info.size) {
        ret = upg_flash_read(primary.part_info.addr_info.addr + read_len, UPG_FLASH_PAGE_SIZE, buffer);
        if (ret != ERRCODE_SUCC) {
            break;
        }

        /* 写之前先擦除 */
        ret = upg_flash_write(backup.part_info.addr_info.addr + read_len, UPG_FLASH_PAGE_SIZE, buffer, true);
        if (ret != ERRCODE_SUCC) {
            break;
        }
        read_len += UPG_FLASH_PAGE_SIZE;
    }

    upg_free(buffer);
    buffer = NULL;
    return ret;
}

errcode_t upg_image_backups_update(void)
{
    errcode_t ret;
    uint32_t backup_num = sizeof(g_image_refs) / sizeof(g_image_refs[0]);
    for (uint32_t i = 0; i < backup_num; i++) {
        ret = upg_backup_update(&g_image_refs[i]);
        if (ret != ERRCODE_SUCC) {
            return ERRCODE_UPG_BACKUP_UPDATE_ERROR;
        }
    }
    return ERRCODE_SUCC;
}
