/*
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: NV Storage Library Restore factory settings interface
 */
#include "nv.h"
#include "nv_store.h"
#include "nv_page.h"
#include "nv_storage.h"
#include "nv_nvregion.h"
#include "nv_porting.h"
#include "nv_notify.h"
#include "nv_task_adapt.h"
#include "nv_update.h"
#include "common_def.h"
#include "nv_reset.h"

#if (CONFIG_NV_SUPPORT_BACKUP_RESTORE == NV_YES)

bool g_reset_region_flag[KEY_ID_REGION_MAX_NUM] = {0};
STATIC retain_key_list_handle_t* g_retain_key_list = NULL;

#define RESET_MAX_BINARY_VAL 1
#define assert_r(x)        ((void)0)
#define NV_RESTORE_MAX_KEY 4060

STATIC errcode_t kv_region_is_enable(uint16_t key_id)
{
    uint16_t region_id = key_id / REGION_KEY_NUMS;
    if (region_id < KEY_ID_REGION_MAX_NUM && g_reset_region_flag[region_id]) {
        return ERRCODE_SUCC;
    }
    return ERRCODE_FAIL;
}

STATIC void retain_key_list_free(void)
{
    retain_key_t *current_key = g_retain_key_list->next;
    retain_key_t *temp_key = NULL;
    while (current_key != NULL) {
        temp_key = current_key;
        current_key = current_key->next;
        if (temp_key->kvalue != NULL) {
            kv_free(temp_key->kvalue);
        }
        kv_free(temp_key);
        g_retain_key_list->list_length--;
    }
    kv_free(g_retain_key_list);
    g_retain_key_list = NULL;
}

STATIC errcode_t retain_key_list_init(void)
{
    if (g_retain_key_list != NULL) {
        return ERRCODE_SUCC;
    }
    g_retain_key_list = (retain_key_list_handle_t *)(uintptr_t)kv_malloc(sizeof(retain_key_list_handle_t));
    if (g_retain_key_list == NULL) {
        return ERRCODE_MALLOC;
    }

    g_retain_key_list->list_length = 0;
    g_retain_key_list->tail = NULL;
    g_retain_key_list->next = NULL;
    return ERRCODE_SUCC;
}

STATIC void retain_key_list_add(retain_key_t* cur_key)
{
    if (g_retain_key_list->list_length == 0) {
        g_retain_key_list->next = cur_key;
        g_retain_key_list->tail = cur_key;
    } else {
        g_retain_key_list->tail->next = cur_key;
        g_retain_key_list->tail = cur_key;
    }
    g_retain_key_list->list_length++;
}

STATIC errcode_t kv_reset_backup_retain_keys_helper(kv_page_handle_t *page)
{
    errcode_t res;
    kv_key_handle_t key;
    retain_key_t *cur_key = NULL;
    res = retain_key_list_init();
    if (res != ERRCODE_SUCC) {
        return res;
    }

    errcode_t key_found = kv_page_find_first_key(page, NULL, &key);
    while (key_found == ERRCODE_SUCC) {
        if ((kv_key_is_valid(&key)) && (kv_region_is_enable(key.header.key_id) == ERRCODE_FAIL)) {
            cur_key = (retain_key_t *)kv_malloc(sizeof(retain_key_t));
            if (cur_key == NULL) {
                return ERRCODE_MALLOC;
            }
            (void)memset_s(cur_key, sizeof(retain_key_t), 0, sizeof(retain_key_t));
            cur_key->key = key.header.key_id;
            cur_key->kvalue_length = key.header.length;
            cur_key->kvalue = (uint8_t *)kv_malloc(cur_key->kvalue_length);
            if (cur_key->kvalue == NULL) {
                return ERRCODE_MALLOC;
            }
            cur_key->next = NULL;
            res = kv_key_read_data(&key, cur_key->kvalue);
            if (res != ERRCODE_SUCC) {
                return ERRCODE_FAIL;
            }
            nv_log_debug("[NV] backup_retain_keys_helper key_id: %d\r\n", cur_key->key);
            (void)retain_key_list_add(cur_key);
        }
        key_found = kv_page_find_next_key(page, NULL, &key);
    }
    return ERRCODE_SUCC;
}

STATIC errcode_t kv_reset_backup_retain_keys(kv_store_t store, uint8_t num_pages)
{
    errcode_t ret;
    kv_page_handle_t kv_page;
    for (uint8_t page_index = 0; page_index < num_pages; page_index++) {
        ret = kv_store_get_page_handle(store, page_index, &kv_page);
        if (ret != ERRCODE_SUCC) {
            return ret;
        }
        ret = kv_reset_backup_retain_keys_helper(&kv_page);
        if (ret != ERRCODE_SUCC) {
            retain_key_list_free();
            return ret;
        }
    }
    return ERRCODE_SUCC;
}

STATIC errcode_t restore_retain_keys(void)
{
    if (g_retain_key_list == NULL) {
        return ERRCODE_FAIL;
    }
    retain_key_t *cur_key = g_retain_key_list->next;
    while (cur_key != NULL && cur_key->kvalue != NULL) {
        nv_log_debug("[NV] restore_retain_keys_b key_id: 0x%x\r\n", cur_key->key);
        (void)uapi_nv_write(cur_key->key, cur_key->kvalue, cur_key->kvalue_length);
        cur_key = cur_key->next;
    }
    retain_key_list_free();
    return ERRCODE_SUCC;
}

errcode_t kv_backup_set_invalid_key(const kv_key_handle_t *key)
{
    /* 计算valid相对于key起始的位置，即要写入数据在flash中相对于key_location的偏移量 */
    uint32_t offset = ((uint32_t)(uintptr_t)&key->header.valid) - ((uint32_t)(uintptr_t)key);
    uint32_t write_position = (uint32_t)(uintptr_t)key->key_location + offset - FLASH_PHYSICAL_ADDR_START;
    uint32_t write_length = sizeof(key->header.valid);
    uint8_t *write_data = (uint8_t *)kv_malloc(write_length);
    if (write_data == NULL) {
        return ERRCODE_MALLOC;
    }
    (void)memset_s(write_data, write_length, 0, write_length);
    errcode_t ret = kv_flash_write(write_position, write_length, write_data, false);
    kv_free(write_data);
    return ret;
}

STATIC errcode_t kv_backup_delete_one_page_repeat_key(kv_page_handle_t *page)
{
    errcode_t ret;
    kv_key_handle_t key;
    errcode_t key_found = kv_page_find_first_key(page, NULL, &key);
    while (key_found == ERRCODE_SUCC) {
        if ((kv_key_is_valid(&key)) && (kv_region_is_enable(key.header.key_id) == ERRCODE_SUCC)) {
            ret = kv_backup_set_invalid_key(&key);
            if (ret != ERRCODE_SUCC) {
                return ret;
            }
        }
        key_found = kv_page_find_next_key(page, NULL, &key);
    }
    return ERRCODE_SUCC;
}

errcode_t kv_backup_delete_repeat_key(void)
{
    errcode_t ret;
    kv_page_handle_t page;
    kv_nvregion_area_t* nvregion_area =  nv_get_region_area();
    if (nvregion_area == NULL) {
        return ERRCODE_FAIL;
    }
    uint32_t page_start = nvregion_area->nv_backup_addr;
    /* 遍历备份区，将每个要被重复备份的key标志位置为无效 */
    for (uint32_t page_index = 0; page_index < KV_BACKUP_PAGE_NUM; page_index++) {
        page.page_location = (kv_page_location)(uintptr_t)(page_start + page_index * KV_PAGE_SIZE);
        ret = kv_backup_delete_one_page_repeat_key(&page);
        if (ret != ERRCODE_SUCC) {
            return ret;
        }
    }

    return ERRCODE_SUCC;
}

STATIC errcode_t kv_backup_dragpage_switch_unused_page(kv_page_handle_t dragpage, kv_page_handle_t unused_page)
{
    /* 将换页页内容转移到unused page */
    errcode_t ret;
    uint32_t page_head_size = (uint32_t)sizeof(kv_page_header_t);

    /* 将换页页的页头转移到unused page */
    kv_page_header_t head_buffer;
    ret = kv_key_helper_copy_flash((uint32_t)(uintptr_t)&head_buffer,
        (uint32_t)(uintptr_t)dragpage.page_location, (uint16_t)page_head_size);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    ret = kv_key_write_flash((uint32_t)(uintptr_t)unused_page.page_location, page_head_size, (uint8_t *)&head_buffer);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    kv_key_handle_t key;
    uint16_t key_size;
    uint32_t write_position = (uint32_t)((uintptr_t)unused_page.page_location + page_head_size);
    /* 将换页页的有效的key转移到unused page */
    errcode_t key_found = kv_page_find_first_key(&dragpage, NULL, &key);
    while (key_found == ERRCODE_SUCC) {
        if (kv_key_is_valid(&key)) {
            key_size = kv_key_flash_size(&key);
            uint8_t *kv_buffer = (uint8_t *)kv_malloc(key_size);
            if (kv_buffer == NULL) {
                return ERRCODE_MALLOC;
            }
            (void)memset_s(kv_buffer, key_size, 0, key_size);
            ret = kv_key_helper_copy_flash((uint32_t)(uintptr_t)kv_buffer, (uint32_t)(uintptr_t)key.key_location,
                key_size);
            if (ret != ERRCODE_SUCC) {
                kv_free(kv_buffer);
                return ret;
            }
            ret = kv_key_write_flash(write_position, key_size, kv_buffer);
            if (ret != ERRCODE_SUCC) {
                kv_free(kv_buffer);
                return ret;
            }
            kv_free(kv_buffer);
            write_position += key_size;
        }
        key_found = kv_page_find_next_key(&dragpage, NULL, &key);
    }

    return ERRCODE_SUCC;
}

errcode_t kv_backup_copy_unused_page_to_dragpage(uint32_t dragpage_location, uint32_t unused_page_location)
{
    errcode_t ret;

    /* 首先将工作区用来换页的页的key内容全部转移到备份区原来的页上 */
    uint8_t *page_buffer = (uint8_t *)kv_malloc(KV_PAGE_SIZE - sizeof(kv_page_header_t));
    if (page_buffer == NULL) {
        return ERRCODE_MALLOC;
    }
    ret = kv_key_helper_copy_flash((uint32_t)(uintptr_t)page_buffer,
        unused_page_location + sizeof(kv_page_header_t), KV_PAGE_SIZE - sizeof(kv_page_header_t));
    if (ret != ERRCODE_SUCC) {
        kv_free(page_buffer);
        return ret;
    }
    ret = kv_key_write_flash(dragpage_location + sizeof(kv_page_header_t),
        KV_PAGE_SIZE - sizeof(kv_page_header_t), page_buffer);
    if (ret != ERRCODE_SUCC) {
        kv_free(page_buffer);
        return ret;
    }
    kv_free(page_buffer);

    /* 其次拷贝工作区换页页的页头内容:先拷贝页内容，后拷贝页头是为了更好的处理换页时掉电异常场景 */
    kv_page_header_t head_buffer;
    ret = kv_key_helper_copy_flash((uint32_t)(uintptr_t)&head_buffer,
        unused_page_location, sizeof(kv_page_header_t));
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    ret = kv_key_write_flash(dragpage_location, sizeof(kv_page_header_t), (uint8_t *)&head_buffer);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    return ERRCODE_SUCC;
}

/* 备份区换页处理机制 */
STATIC errcode_t kv_backup_dragpage_process(kv_page_handle_t dragpage)
{
    errcode_t ret;
    kv_page_handle_t unused_page;
    kv_page_location unused_page_location = NULL;
    /* 1、去找工作区可以换页的页，然后将可以换页的页地址给传出 */
    ret = kv_nvregion_find_unused_page(&unused_page_location);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    /* 2、将工作区内可以换页的页的内容擦除 */
    ret = kv_nvregion_erase_page(unused_page_location);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    unused_page.page_location = unused_page_location;
    /* 3、将备份区要换页的内容拷贝到工作区可以换页的页上 */
    ret = kv_backup_dragpage_switch_unused_page(dragpage, unused_page);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    /* 4、将备份区要换页的页擦除内容 */
    ret = kv_key_erase_flash((uint32_t)(uintptr_t)dragpage.page_location, KV_PAGE_SIZE);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    /* 5、将换页页的内容拷贝到备份区被擦除的页上 */
    ret =  kv_backup_copy_unused_page_to_dragpage((uint32_t)(uintptr_t)dragpage.page_location,
        (uint32_t)(uintptr_t)unused_page.page_location);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    /* 6、擦除工作区换页页 */
    ret = kv_nvregion_erase_page(unused_page_location);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    return ERRCODE_SUCC;
}

errcode_t kv_backup_find_write_position(uint16_t required_space, uint32_t *write_position)
{
    errcode_t ret;
    kv_page_handle_t page;
    kv_page_status_t page_status;
    kv_page_handle_t dragpage;
    kv_nvregion_area_t* nvregion_area =  nv_get_region_area();
    uint32_t page_start = nvregion_area->nv_backup_addr;
    for (uint32_t page_index = 0; page_index < KV_BACKUP_PAGE_NUM; page_index++) {
        page.page_location = (kv_page_location)(uintptr_t)(page_start + page_index * KV_PAGE_SIZE);
        kv_page_get_status(&page, &page_status);
        if ((page_status.total_space - page_status.used_space) >= required_space) {
            *write_position = page_status.first_writable_location;
            return ERRCODE_SUCC;
        }

        /* 找可换页页 */
        if (page_status.max_key_space >= required_space) {
            dragpage = page;
        }
    }

    /* 进行备份区换页 */
    ret = kv_backup_dragpage_process(dragpage);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    /* 将备份区整理好的页可以写的位置传出 */
    kv_page_get_status(&dragpage, &page_status);
    if ((page_status.total_space - page_status.used_space) >= required_space) {
        *write_position = page_status.first_writable_location;
        return ERRCODE_SUCC;
    }
    return ERRCODE_NV_NO_ENOUGH_SPACE;
}

STATIC errcode_t kv_backup_write_one_page_key(kv_page_handle_t *page)
{
    errcode_t ret;
    kv_key_handle_t key;
    uint16_t key_size;
    uint32_t write_position = 0;
    errcode_t key_found = kv_page_find_first_key(page, NULL, &key);
    while (key_found == ERRCODE_SUCC) {
        if ((kv_key_is_valid(&key)) &&
            (kv_region_is_enable(key.header.key_id) == ERRCODE_SUCC)) {
            key_size = kv_key_flash_size(&key);
            uint8_t *kv_buffer = (uint8_t *)kv_malloc(key_size);
            if (kv_buffer == NULL) {
                return ERRCODE_MALLOC;
            }
            ret = kv_key_helper_copy_flash((uint32_t)(uintptr_t)kv_buffer, (uint32_t)(uintptr_t)key.key_location,
                key_size);
            if (ret != ERRCODE_SUCC) {
                kv_free(kv_buffer);
                return ret;
            }
            ret = kv_backup_find_write_position(key_size, &write_position);
            if (ret != ERRCODE_SUCC) {
                kv_free(kv_buffer);
                return ret;
            }
            ret = kv_key_write_flash(write_position, key_size, kv_buffer);
            if (ret != ERRCODE_SUCC) {
                kv_free(kv_buffer);
                return ret;
            }
            kv_free(kv_buffer);
        }
        key_found = kv_page_find_next_key(page, NULL, &key);
    }
    return ERRCODE_SUCC;
}

errcode_t kv_backup_write_key(void)
{
    errcode_t ret;
    kv_page_handle_t page;

    for (uint32_t page_index = 0; page_index < KV_STORE_PAGES_ACPU; page_index++) {
        ret = kv_store_get_page_handle(KV_STORE_APPLICATION, page_index, &page);
        if (ret != ERRCODE_SUCC) {
            return ret;
        }
        ret = kv_backup_write_one_page_key(&page);
        if (ret != ERRCODE_SUCC) {
            return ret;
        }
    }

    return ERRCODE_SUCC;
}

STATIC errcode_t kv_read_backup_key(uint16_t key_id, uint16_t *kvalue_length, uint8_t *kvalue, nv_key_attr_t *attr,
    kv_page_location page_location)
{
    kv_attributes_t backup_attribute = 0;
    if (kvalue_length == NULL || kvalue == NULL || attr == NULL) {
        return ERRCODE_NV_INVALID_PARAMS;
    }

    kv_store_key_data_t key_data = {*kvalue_length, 0, kvalue};
    errcode_t ret_val = kv_store_get_backup_key(key_id, &key_data, &backup_attribute, page_location);
    if (ret_val == ERRCODE_SUCC) {
        memset_s(attr, sizeof(nv_key_attr_t), 0, sizeof(nv_key_attr_t));
        if (((uint32_t)backup_attribute & NV_ATTRIBUTE_PERMANENT) != 0) {
            attr->permanent = true;
            attr->non_upgrade = true;
        }
        if (((uint32_t)backup_attribute & NV_ATTRIBUTE_ENCRYPTED) != 0) {
            attr->encrypted = true;
            attr->non_upgrade = true;
        }
        if (((uint32_t)backup_attribute & NV_ATTRIBUTE_NON_UPGRADE) != 0) {
            attr->non_upgrade = true;
        }
    }
    *kvalue_length = key_data.kvalue_actual_length;
    return ret_val;
}

STATIC errcode_t kv_start_restore_all_keys(void)
{
    errcode_t ret = ERRCODE_SUCC;
    uint16_t klength = 0;
    uint8_t *kvalue;
    nv_key_attr_t attr;
    kv_key_handle_t key;
    kv_page_handle_t page;
    errcode_t key_found;
    kv_nvregion_area_t* nvregion_area =  nv_get_region_area();
    if (nvregion_area == NULL) {
        return ERRCODE_FAIL;
    }
    for (uint32_t page_num = 0; page_num < KV_BACKUP_PAGE_NUM; page_num++) {
        page.page_location = (kv_page_location)(uintptr_t)(nvregion_area->nv_backup_addr + page_num * KV_PAGE_SIZE);
        key_found = kv_page_find_first_key(&page, NULL, &key);
        while (key_found == ERRCODE_SUCC) {
            if (kv_region_is_enable(key.header.key_id) == ERRCODE_SUCC) {
                kvalue = (uint8_t *)kv_malloc(key.header.length);
                klength = key.header.length;
                (void)kv_read_backup_key(key.header.key_id, &klength, kvalue, &attr, page.page_location);
                ret = uapi_nv_write_with_attr(key.header.key_id, kvalue, klength, &attr, NULL);
                kv_free(kvalue);
            }
            key_found = kv_page_find_next_key(&page, NULL, &key);
        }
    }
    return ret;
}


errcode_t kv_enable_restore_flag(const nv_reset_mode_t *nv_reset_mode)
{
    return uapi_nv_write(NV_ID_RESTORE_ENABLE, (uint8_t *)(uintptr_t)nv_reset_mode, sizeof(nv_reset_mode_t));
}

static errcode_t kv_disable_restore_flag(void)
{
    nv_reset_mode_t nv_reset_mode = {0};
    return uapi_nv_write(NV_ID_RESTORE_ENABLE, (uint8_t *)(uintptr_t)&nv_reset_mode, sizeof(nv_reset_mode_t));
}

errcode_t kv_restore_set_region_flag(const bool *flag)
{
    if (flag == NULL) {
        if (memset_s(g_reset_region_flag, sizeof(g_reset_region_flag), true, sizeof(g_reset_region_flag)) != EOK) {
            return ERRCODE_MEMSET;
        }
    } else {
        if (memcpy_s(g_reset_region_flag, sizeof(g_reset_region_flag), flag, sizeof(g_reset_region_flag)) != EOK) {
            return ERRCODE_MEMCPY;
        }
    }
    return ERRCODE_SUCC;
}

STATIC errcode_t kv_restore_clear_work_area(void)
{
    errcode_t ret;
    kv_nvregion_area_t* nvregion_area =  nv_get_region_area();
    if (nvregion_area == NULL) {
        return ERRCODE_FAIL;
    }
    ret = kv_key_erase_flash(nvregion_area->nv_data_addr, KV_STORE_DATA_SIZE);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    return kv_update_init((cores_t)KV_STORE_APPLICATION);
}

STATIC errcode_t kv_restore_mode_a(void)
{
    errcode_t ret;
    ret = kv_restore_clear_work_area();
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    ret = kv_restore_set_region_flag(NULL);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    return kv_start_restore_all_keys();
}

STATIC errcode_t kv_restore_mode_b(const bool *flag)
{
    if (flag == NULL) {
        return ERRCODE_NV_ILLEGAL_OPERATION;
    }

    errcode_t ret;
    ret = kv_restore_set_region_flag(flag);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    ret = kv_reset_backup_retain_keys(KV_STORE_APPLICATION, kv_store_get_page_count(KV_STORE_APPLICATION));
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    ret = kv_restore_clear_work_area();
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    ret = kv_restore_set_region_flag(NULL);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    ret = kv_start_restore_all_keys();
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    return restore_retain_keys();
}

errcode_t kv_backup_keys(const nv_backup_mode_t *backup_flag)
{
    errcode_t ret;
    ret = kv_restore_set_region_flag(backup_flag->region_mode);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    ret = kv_backup_delete_repeat_key();
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    return kv_backup_write_key();
}

errcode_t kv_restore_all_keys(void)
{
    errcode_t ret;
    nv_reset_mode_t nv_reset_mode;
    uint16_t flag_length;

    ret = uapi_nv_read(NV_ID_RESTORE_ENABLE, sizeof(nv_reset_mode_t), &flag_length,
        (uint8_t *)(uintptr_t)&nv_reset_mode);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    (void)kv_disable_restore_flag();

    if (nv_reset_mode.mode == RESET_MODE_A) {
        return kv_restore_mode_a();
    } else if (nv_reset_mode.mode == RESET_MODE_B) {
        return kv_restore_mode_b(nv_reset_mode.region_flag);
    } else {
        return ERRCODE_NV_ILLEGAL_OPERATION;
    }
}

#endif /* #if (CONFIG_NV_SUPPORT_BACKUP_RESTORE == NV_YES) */