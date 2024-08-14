/*
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: UPG common functions for different chip
 */

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "errcode.h"
#include "common_def.h"
#if defined(SUPPORT_EXTERN_FLASH)
#include "flash.h"
#endif
#include "upg_debug.h"
#include "partition.h"
#include "securec.h"
#include "sfc.h"
#include "upg_porting.h"
#include "hal_reboot.h"
#include "tcxo.h"
#include "non_os_reboot.h"
#include "non_os.h"
#include "cpu_utils.h"
#if (UPG_CFG_VERIFICATION_SUPPORT == YES)
#include "otp.h"
#include "bootloader_configuration.h"
#endif
#include "upg_alloc.h"
#include "upg_config.h"
#include "watchdog.h"
#include "osal_interrupt.h"
#include "upg_definitions_porting.h"
#include "soc_osal.h"
#include "upg_common_porting.h"

#define MS_ID_ADDR   0x30
#define CHIP_ID_ADDR 0x108
#define TCXO_REBOOT_DELAY 500ULL
#define FLASH_PAGE_SIZE_BIT_LENGTH 12
#define ADDR_4K_MASK              0xFFF
#define SIZE_4K                   0x1000
#define SECTOR_ERASE_DELAY_MS     50

#define EXTERN_FLASH_UPG_START_ADDRESS 0x80000

#define uapi_array_size(_array)  (sizeof(_array) / sizeof((_array)[0]))

static upg_image_partition_ids_map_t g_img_partition_map[] = {
    {FLASHBOOT_IMAGE_ID, PARTITION_FLASHBOOT_IMAGE},
    {APPLICATION_IMAGE_ID, PARTITION_ACPU_IMAGE},
};

/* 获取镜像ID和分区ID的map表
 * map: 镜像ID和分区ID的映射表
 * 返回map表中映射数量
 */
uint32_t upg_get_ids_map(upg_image_partition_ids_map_t **map)
{
    *map = g_img_partition_map;
    return uapi_array_size(g_img_partition_map);
}

/*
 * recovery/APP支持升级的镜像ID
 * 注意增删ID时同步更新升级ID的数量
 */
#define UPDATE_IMAGE_SET \
    { FLASHBOOT_IMAGE_ID, \
      APPLICATION_IMAGE_ID, \
      UPG_IMAGE_ID_NV }
#define UPDATE_IMAGE_SET_CNT 3 /* 镜像ID列表长度 */

static uint32_t g_bs21_support_upg_id[] = UPDATE_IMAGE_SET;
static upg_image_collections_t g_bs21_upg_collect = { g_bs21_support_upg_id, UPDATE_IMAGE_SET_CNT };
/* 获取当前程序支持的升级镜像 */
upg_image_collections_t *uapi_upg_get_image_id_collection(void)
{
    return &g_bs21_upg_collect;
}

/* 获取升级包路径 */
char *upg_get_pkg_file_path(void)
{
    return UPG_FILE_NAME;
}

/* 获取升级包所在目录 */
char *upg_get_pkg_file_dir(void)
{
    return UPG_FILE_PATH;
}

/* 本次支持升级的镜像ID集合，若为空则无镜像限制 */
bool upg_img_in_set(uint32_t img_id)
{
    upg_image_collections_t *collect = uapi_upg_get_image_id_collection();
    if (collect == NULL || collect->img_ids_cnt == 0) {
        return true;
    }

    for (uint32_t i = 0; i < collect->img_ids_cnt; i++) {
        if (collect->img_ids[i] == img_id) {
            return true;
        }
    }
    return false;
}

/*
 * 获取FOTA升级标记区的Flash起始地址，该地址为在flash上的相对地址，是相对flash基地址的偏移
 * start_address 返回升级标记区的起始地址
 */
errcode_t upg_get_upgrade_flag_flash_start_addr(uint32_t *start_address)
{
    errcode_t ret_val;
    partition_information_t info;

    ret_val = uapi_partition_get_info(PARTITION_FOTA_DATA, &info);
    if (ret_val != ERRCODE_SUCC) {
        return ret_val;
    }

    /* FOTA升级标记区在FOTA分区的最后 */
    *start_address = info.part_info.addr_info.addr + info.part_info.addr_info.size - FOTA_DATA_FLAG_AREA_LEN;
    return ERRCODE_SUCC;
}

/*
 * 获取FOTA升级进度恢复标记区的Flash起始地址，该地址为在flash上的相对地址，是相对flash基地址的偏移
 * start_address 返回标记区的起始地址
 * size 返回包含标记区和flag区的总长度
 */
errcode_t upg_get_progress_status_start_addr(uint32_t *start_address, uint32_t *size)
{
    partition_information_t info;
    errcode_t ret_val = uapi_partition_get_info(PARTITION_FOTA_DATA, &info);
    if (ret_val != ERRCODE_SUCC) {
        return ret_val;
    }

    *start_address = info.part_info.addr_info.addr + info.part_info.addr_info.size - UPG_UPGRADE_FLAG_LENGTH;
    *size = UPG_UPGRADE_FLAG_LENGTH;
    return ERRCODE_SUCC;
}

/*
 * 获取在Flash上预留的FOTA分区的地址和长度，该地址为在flash上的相对地址，是相对flash基地址的偏移
 * start_address 返回FOTA分区的起始地址
 * size          返回FOTA分区大小（包含升级包存储区、升级标记区和缓存区、状态区）
 */
errcode_t upg_get_fota_partiton_area_addr(uint32_t *start_address, uint32_t *size)
{
    partition_information_t info;
    errcode_t ret_val = uapi_partition_get_info(PARTITION_FOTA_DATA, &info);
    if (ret_val != ERRCODE_SUCC) {
        return ret_val;
    }

    *start_address = info.part_info.addr_info.addr;
    *size = info.part_info.addr_info.size;
    return ERRCODE_SUCC;
}

/*
 * 重启
 */
void upg_reboot(void)
{
    /* 重启前睡眠500ms */
    uapi_tcxo_delay_ms((uint64_t)TCXO_REBOOT_DELAY);
    cpu_utils_set_system_status_by_cause(REBOOT_CAUSE_UPG_COMPLETION);
    hal_reboot_chip();
}

/*
 * 防止看门狗超时，踢狗
 */
void upg_watchdog_kick(void)
{
    uapi_watchdog_kick();
}

uint32_t upg_get_flash_base_addr(void)
{
    return FLASH_START;
}

uint32_t upg_get_flash_size(void)
{
    return UPG_FLASH_SIZE;
}

static errcode_t flash_write(uint32_t addr, uint32_t size, uint8_t *data)
{
    errcode_t ret = ERRCODE_FAIL;
#if defined(SUPPORT_EXTERN_FLASH)
    if (addr >= EXTERN_FLASH_UPG_START_ADDRESS) {
        if (uapi_flash_write_data(0, addr, data, size) == size) {
            ret = ERRCODE_SUCC;
        }
    } else {
#endif
        if (memcpy_s((uint8_t *)(addr + FLASH_START), size, (void *)data, size) == EOK) {
            ret = ERRCODE_SUCC;
        }
#if defined(SUPPORT_EXTERN_FLASH)
    }
#endif
    return ret;
}

static errcode_t flash_read(uint32_t addr, uint32_t size, uint8_t *data)
{
    errcode_t ret = ERRCODE_FAIL;
    uint32_t status = (uint32_t)osal_irq_lock();
#if defined(SUPPORT_EXTERN_FLASH)
    if (addr >= EXTERN_FLASH_UPG_START_ADDRESS) {
        if (uapi_flash_read_data(0, addr, data, size) == size) {
            ret = ERRCODE_SUCC;
        }
    } else {
#endif
        if (memcpy_s((void *)data, size, (uint8_t *)(addr + FLASH_START), size) == EOK) {
            ret = ERRCODE_SUCC;
        }
#if defined(SUPPORT_EXTERN_FLASH)
    }
#endif
    osal_irq_restore(status);
    return ret;
}

#if defined(VERSION_STANDARD)
static errcode_t flash_erase(uint32_t addr, uint32_t size)
{
    errcode_t ret = ERRCODE_FAIL;
#if defined(SUPPORT_EXTERN_FLASH)
    if (addr >= EXTERN_FLASH_UPG_START_ADDRESS) {
        for (uint32_t i = addr; i < (addr + size); i += SIZE_4K) {
            ret = uapi_flash_sector_erase(0, i, false);
            while (uapi_flash_is_processing(0)) {
                osal_msleep(SECTOR_ERASE_DELAY_MS);
            }
        }
    } else {
#endif
        uint32_t status = (uint32_t)osal_irq_lock();
        ret = uapi_sfc_reg_erase(addr, size);
        osal_irq_restore(status);
#if defined(SUPPORT_EXTERN_FLASH)
    }
#endif
    return ret;
}
#else
static errcode_t flash_erase(uint32_t addr, uint32_t size)
{
    errcode_t ret = ERRCODE_FAIL;
#if defined(SUPPORT_EXTERN_FLASH)
    if (addr >= EXTERN_FLASH_UPG_START_ADDRESS) {
        if (uapi_flash_block_erase(0, addr, size, true) == ERRCODE_SUCC) {
            ret = ERRCODE_SUCC;
        }
    } else {
#endif
        uint32_t status = (uint32_t)osal_irq_lock();
        ret = uapi_sfc_reg_erase(addr, size);
        osal_irq_restore(status);
#if defined(SUPPORT_EXTERN_FLASH)
    }
#endif
    return ret;
}
#endif

// 升级读flash接口需支持跨页读取
errcode_t upg_flash_read(const uint32_t flash_offset, const uint32_t size, uint8_t *ram_data)
{
    return flash_read(flash_offset, size, ram_data);
}

STATIC errcode_t upg_erase_before_write(const uint32_t flash_offset, uint32_t size)
{
    errcode_t ret = ERRCODE_FAIL;
    uint32_t end_addr = flash_offset + size;
    uint32_t start_sector_addr = flash_offset & ~ADDR_4K_MASK;
    uint32_t end_sector_addr = (end_addr & ADDR_4K_MASK) == 0 ? end_addr : \
                               (end_addr & ~ADDR_4K_MASK) + UPG_FLASH_PAGE_SIZE;
    uint32_t start_data_size = flash_offset - start_sector_addr;
    uint32_t end_data_size = end_sector_addr - end_addr;

    uint8_t *start_data = upg_malloc(UPG_FLASH_PAGE_SIZE);
    if (start_data == NULL) { return ERRCODE_FAIL; }

    uint8_t *end_data = upg_malloc(UPG_FLASH_PAGE_SIZE);
    if (end_data == NULL) {
        upg_free(start_data);
        return ERRCODE_FAIL;
    }
    uint32_t status = (uint32_t)osal_irq_lock();
    if (start_data_size != 0) {
        ret = flash_read(start_sector_addr,  start_data_size, (uint8_t *)start_data);
        if (ret != ERRCODE_SUCC) {
            goto end;
        }
    }
    if (end_data_size != 0) {
        ret = flash_read(end_addr, end_data_size, (uint8_t *)end_data);
        if (ret != ERRCODE_SUCC) {
            goto end;
        }
    }
    ret = flash_erase(start_sector_addr, (end_sector_addr - start_sector_addr));
    if (ret != ERRCODE_SUCC) {
        goto end;
    }
    if (start_data_size != 0) {
        ret = flash_write(start_sector_addr, start_data_size, (uint8_t *)start_data);
        if (ret != ERRCODE_SUCC) {
            goto end;
        }
    }
    if (end_data_size != 0) {
        ret = flash_write(end_addr, end_data_size, (uint8_t *)end_data);
        if (ret != ERRCODE_SUCC) {
            goto end;
        }
    }
end:
    osal_irq_restore(status);
    upg_free(start_data);
    upg_free(end_data);
    return ret;
}

// 升级写flash接口需支持跨页写入和写前擦功能
errcode_t upg_flash_write(const uint32_t flash_offset, uint32_t size, const uint8_t *ram_data, bool do_erase)
{
    errcode_t ret = ERRCODE_SUCC;
    uint32_t status = 0;
    if (do_erase) {
        if (upg_erase_before_write(flash_offset, size) != ERRCODE_SUCC) {
            return ERRCODE_FAIL;
        }
    }
    uint8_t *cmp_data = upg_malloc(size);
    if (cmp_data == NULL) {
        return ERRCODE_FAIL;
    }
    status = (uint32_t)osal_irq_lock();
#if defined(VERSION_STANDARD) && !defined(SUPPORT_EXTERN_FLASH)
    errcode_t write_ret = uapi_sfc_reg_write(flash_offset, (uint8_t *)ram_data, size);
    if (write_ret != ERRCODE_SUCC) {
        goto end;
    }
    write_ret = uapi_sfc_reg_read(flash_offset, (uint8_t *)cmp_data, size);
    if (write_ret != ERRCODE_SUCC) {
        goto end;
    }
#else
    uint32_t write_ret = flash_write(flash_offset, size, (uint8_t *)ram_data);
    if (write_ret != EOK) {
        goto end;
    }
    write_ret = flash_read(flash_offset, size, (uint8_t *)cmp_data);
    if (write_ret != EOK) {
        goto end;
    }
#endif
end:
    osal_irq_restore(status);
    uint32_t sec_ret = memcmp(cmp_data, ram_data, size);
    if (sec_ret != EOK) {
        ret = ERRCODE_FAIL;
    }
    upg_free(cmp_data);
    return ret;
}

errcode_t upg_flash_erase(const uint32_t flash_offset, const uint32_t size)
{
    errcode_t ret = ERRCODE_FAIL;
    uint32_t end_addr = flash_offset + size;
    uint32_t start_sector = flash_offset & ~ADDR_4K_MASK;
    uint32_t end_sector = (end_addr & ADDR_4K_MASK) == 0 ? end_addr : (end_addr & ~ADDR_4K_MASK) + SIZE_4K;
    uint32_t erase_size = end_sector - start_sector;
    ret = flash_erase(start_sector, erase_size);
    return ret;
}

#if (UPG_CFG_VERIFICATION_SUPPORT == YES)
/*
 * 获取校验用的root_public_key
 */
uint8_t *upg_get_root_public_key(void)
{
#if (UPG_CFG_DIRECT_FLASH_ACCESS == YES)
    /* 使用Upgrader_External_Public_Key校验Key Area的签名  */
    uint32_t upgrader_address = 0;
    partition_information_t info;
    errcode_t ret_val = uapi_partition_get_info(PARTITION_UPGRADER_PRIMARY, &info);
    if (ret_val != ERRCODE_SUCC) {
        return NULL;
    }
    upgrader_address = info.part_info.addr_info.addr;
    image_key_area *upgrader_key = (image_key_area *)(uintptr_t_t)(upgrader_address + upg_get_flash_base_addr());
    return upgrader_key->ext_pulic_key_area;
#else
    upg_package_header_t *pkg_header = NULL;
    errcode_t              ret;
    ret = upg_get_package_header(&pkg_header);
    if (ret != ERRCODE_SUCC || pkg_header == NULL) {
        return ret;
    }
    static uint8_t public_key[PUBLIC_KEY_LEN];
    memcpy_s(public_key, sizeof(public_key), (pkg_header->key_area.fota_external_public_key), PUBLIC_KEY_LEN);
    return public_key;
#endif
}

STATIC errcode_t check_fota_msid(const uint32_t msid_ext, const uint32_t mask_msid_ext)
{
    return ERRCODE_SUCC;
}

/*
 * 检查FOTA升级包中的信息与板端是否匹配
 * pkg_header 升级包包头指针
 * 检查成功，返回 ERRCODE_SUCC
 */
errcode_t upg_check_fota_information(const upg_package_header_t *pkg_header)
{
    upg_key_area_data_t *key_area = (upg_key_area_data_t *)&(pkg_header->key_area);
    upg_fota_info_data_t *fota_info = (upg_fota_info_data_t *)&(pkg_header->info_area);
    errcode_t ret;
    uint8_t chip_id;

    ret = check_fota_msid(key_area->msid_ext, key_area->mask_msid_ext);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    ret = check_fota_msid(fota_info->msid_ext, fota_info->mask_msid_ext);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    return ERRCODE_SUCC;
}
#endif