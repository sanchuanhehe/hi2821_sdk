/*
 * Copyright (c) @CompanyNameMagicTag 2016-2022. All rights reserved.
 * Description: SOC KEY VALUE STORAGE IMPLEMENTATION
 */

#include "nv.h"
#include "nv_storage.h"
#include "nv_store.h"
#include "nv_upg.h"
#include "securec.h"
#include "nv_porting.h"
#include "common_def.h"
#include "nv_reset.h"
#ifdef CONFIG_NV_SUPPORT_SINGLE_CORE_SYSTEM
#include "osal_semaphore.h"
#include "nv_update.h"
#include "nv_notify.h"
#include "nv_task_adapt.h"
#else
#include "soc_partition.h"
#include "rpc_auto_generated.h"
#include "flash_task.h"
#include "soc_flash_task.h"
#ifdef CONFIG_SUPPORT_NV_REMOTE_CORE
#include "soc_mem.h"
#include "soc_osal.h"
#include "soc_riscv_cache.h"
#else
#include "flash_task_adapt.h"
#endif
#endif

osal_semaphore nv_sem;
#define MAX_BINARY_VAL 1

STATIC nv_attributes_t nv_helper_convert_key_attr(const nv_key_attr_t *attr)
{
    nv_attributes_t attribute = NV_ATTRIBUTE_NORMAL;
    bool is_support_crypt = false;
    if (attr == NULL) {
        return attribute;
    }

#if (CONFIG_NV_SUPPORT_ENCRYPT == NV_YES)
    is_support_crypt = attr->encrypted;
#endif

    if (attr->permanent && is_support_crypt) {
        attribute = NV_ATTRIBUTE_ENCRYPTED | NV_ATTRIBUTE_PERMANENT | NV_ATTRIBUTE_NON_UPGRADE;
    } else if (attr->permanent) {
        attribute = NV_ATTRIBUTE_PERMANENT | NV_ATTRIBUTE_NON_UPGRADE;
    } else if (is_support_crypt) {
        attribute = NV_ATTRIBUTE_ENCRYPTED | NV_ATTRIBUTE_NON_UPGRADE;
    } else if (attr->non_upgrade) {
        attribute = NV_ATTRIBUTE_NON_UPGRADE;
    }
    return attribute;
}

static uint16_t nv_storage_max_key_space(const nv_key_attr_t *attr)
{
    if (attr == NULL) {
        return NV_NORMAL_KVALUE_MAX_LEN;
    }
    return (attr->encrypted ? NV_ENCRYPTED_KVALUE_MAX_LEN : NV_NORMAL_KVALUE_MAX_LEN);
}

#ifndef CONFIG_NV_SUPPORT_SINGLE_CORE_SYSTEM
typedef struct {
    /** Callback function to call if not NULL */
    owner_callback                callback;
    /* Include any owner data to be available to the callback function upon execution */
    uint8_t                         *kvalue;
    nv_storage_completed_callback user_func;
} nv_task_callback;

static uint8_t g_nv_task_count = 0;
static errcode_t nv_task_completed_callback(errcode_t result, owner_callback *callback)
{
    volatile nv_task_callback *nv_callback = (nv_task_callback *)callback;

    if (nv_callback->kvalue != NULL) {
        kv_free((void *)nv_callback->kvalue);
        nv_callback->kvalue = NULL;
    }

    if (nv_callback->user_func != NULL) {
        if (result == ERRCODE_FLASH_TASK_COMPLETED) {
            result = ERRCODE_SUCC;
        }
        nv_callback->user_func(result);
    }

    if (g_nv_task_count > 0) {
        g_nv_task_count--;
    }
    return ERRCODE_SUCC;
}

static errcode_t nv_storage_search_write_queue_tasks(kv_key_id key_id, nv_attributes_t *queued_attributes,
                                                     uint8_t **queued_kvalue, uint16_t *queued_kvalue_length)
{
    errcode_t res = ERRCODE_NV_KEY_NOT_IN_WRITE_QUEUE;
    flash_task_node *task_found = NULL;

    /* Walk Flash Task queue looking for the most recent update to the key requested */
    while (flash_task_find_next_uncompleted(task_found, &task_found) == ERRCODE_SUCC) {
        switch (task_found->task) {
            case FLASH_TASK_KV_DATA:
                if ((task_found->data.kv.key == key_id) &&
                    ((*queued_attributes & NV_ATTRIBUTE_PERMANENT) == 0)) {
                    *queued_kvalue = (uint8_t *)task_found->data.kv.kvalue;
                    *queued_kvalue_length = task_found->data.kv.kvalue_length;
                    *queued_attributes |= task_found->data.kv.attribute;
                    res = ERRCODE_SUCC;
                }
                break;
            case FLASH_TASK_KV_ATTRIBUTE:
                if ((task_found->data.kv_attribute.key == key_id) &&
                    ((*queued_attributes & NV_ATTRIBUTE_PERMANENT) == 0)) {
                    *queued_attributes |= task_found->data.kv_attribute.attribute;
                    /* Don't update return value for attribute changes */
                }
                break;
            case FLASH_TASK_KV_ERASE:
                if ((task_found->data.kv_erase.key == key_id) &&
                    ((*queued_attributes & NV_ATTRIBUTE_PERMANENT) == 0)) {
                    *queued_kvalue = NULL;
                    *queued_kvalue_length = 0;
                    *queued_attributes = 0;
                    res = ERRCODE_NV_KEY_NOT_FOUND;
                }
                break;
            default:
                break;
        }
    }
    return res;
}

static errcode_t nv_storage_search_write_queue(kv_key_id key_id, uint16_t kvalue_max_length, uint8_t *kvalue,
                                               uint16_t *kvalue_actual_length, nv_attributes_t *attributes)
{
    errcode_t res;
    uint8_t *queued_kvalue = NULL;
    uint16_t queued_kvalue_length = 0;
    nv_attributes_t queued_attributes = 0;

    if (attributes != NULL) {
        queued_attributes = *attributes;
    }

    /* Walk Flash Task queue looking for the most recent update to the key requested */
    res = nv_storage_search_write_queue_tasks(key_id, &queued_attributes, &queued_kvalue, &queued_kvalue_length);
    /* Pass back attributes, if required */
    if (attributes != NULL) {
        *attributes = queued_attributes;
    }

    /* Pass back key data, if required */
    if ((kvalue != NULL) && (kvalue_actual_length != NULL) && (res == ERRCODE_SUCC)) {
        *kvalue_actual_length = queued_kvalue_length;
        if (kvalue_max_length < queued_kvalue_length) {
            return ERRCODE_NV_GET_BUFFER_TOO_SMALL;
        }

        if ((memcpy_s(kvalue, kvalue_max_length, queued_kvalue, queued_kvalue_length)) != EOK) {
            return ERRCODE_FAIL;
        }
    }
    return res;
}

static errcode_t nv_helper_get_key_attr(uint16_t key_id, uint16_t *len, nv_key_attr_t *attr)
{
    uint8_t attribute;
    errcode_t ret_val = ERRCODE_FAIL;

    if (len == NULL || attr == NULL) {
        return ERRCODE_NV_INVALID_PARAMS;
    }

    memset_s(attr, sizeof(nv_key_attr_t), 0, sizeof(nv_key_attr_t));
#ifdef CONFIG_SUPPORT_NV_REMOTE_CORE
    unused(nv_storage_search_write_queue);
    if (uapi_rpc_cmd_kv_store_get_key_attr(key_id, (uint32_t *)&ret_val, len, (uint8_t *)&attribute) != ERRCODE_SUCC) {
        return ERRCODE_NV_RPC_ERROR;
    }
#else
    kv_store_t store = kv_store_from_core(CORE);
    errcode_t store_res = kv_store_get_key_attr(store, key_id, len, (kv_attributes_t *)&attribute);
    errcode_t queue_res = nv_storage_search_write_queue(key_id, 0, NULL, NULL, (nv_attributes_t *)&attribute);
    if ((((uint32_t)(uintptr_t)attr) & (uint32_t)NV_ATTRIBUTE_PERMANENT) != 0) {
        return ERRCODE_NV_TRYING_TO_MODIFY_A_PERMANENT_KEY;
    } else if (queue_res != ERRCODE_NV_KEY_NOT_IN_WRITE_QUEUE) {
        ret_val = queue_res;
    } else {
        ret_val = store_res;
    }
#endif
    if (ret_val == ERRCODE_SUCC) {
        if (attribute & NV_ATTRIBUTE_PERMANENT) {
            attr->permanent = true;
            attr->non_upgrade = true;
        }
        if (attribute & NV_ATTRIBUTE_ENCRYPTED) {
            attr->encrypted = true;
            attr->non_upgrade = true;
        }
        if (attribute & NV_ATTRIBUTE_NON_UPGRADE) {
            attr->non_upgrade = true;
        }
    }
    return ret_val;
}

static errcode_t nv_helper_get_key_data(uint16_t key_id, uint16_t kvalue_max_length, uint16_t *kvalue_length,
                                        uint8_t *kvalue, nv_key_attr_t *attr)
{
    uint16_t actual_len = 0;

    if (kvalue_length == NULL || kvalue == NULL || attr == NULL) {
        return ERRCODE_NV_INVALID_PARAMS;
    }

    errcode_t ret_val = nv_helper_get_key_attr(key_id, &actual_len, attr);
    if (ret_val != ERRCODE_SUCC) {
        return ret_val;
    }

    if (kvalue_max_length < actual_len) {
        return ERRCODE_NV_BUFFER_TOO_SMALL;
    }

#ifdef CONFIG_SUPPORT_NV_REMOTE_CORE
#ifdef CONFIG_SUPPORT_NV_READ_BY_RPC
    if (uapi_rpc_cmd_kv_store_get_key(key_id, (uint32_t *)&ret_val,
        kvalue_max_length, kvalue_length, (uint32_t *)(uintptr_t)kvalue) != ERRCODE_SUCC) {
        return ERRCODE_NV_RPC_ERROR;
    }
#else
    uint8_t *actual_buf = (uint8_t *)kv_malloc(actual_len);
    if (uapi_rpc_cmd_kv_store_get_key_by_addr(key_id, (uint32_t)(uintptr_t)actual_buf, actual_len,
        (uint32_t *)&ret_val) != ERRCODE_SUCC) {
        return ERRCODE_NV_RPC_ERROR;
    }

    if (memcpy_s((void *)(uintptr_t)kvalue, kvalue_max_length, (void *)(uintptr_t)actual_buf, actual_len) !=
        EOK) {
        kv_free(actual_buf);
        return ERRCODE_MEMCPY;
    }

    kv_free(actual_buf);
    *kvalue_length = actual_len;
#endif
#else
    kv_store_t store = kv_store_from_core(CORE);
    kv_store_key_data_t key_data = {kvalue_max_length, 0, kvalue};

    /* Check if Flash Task queue contains unwritten key data before reading from flash */
    errcode_t store_res = kv_store_get_key(store, key_id, &key_data, NULL);
    *kvalue_length = key_data.kvalue_actual_length;
    errcode_t queue_res = nv_storage_search_write_queue(key_id, kvalue_max_length, kvalue, kvalue_length, NULL);
    if (queue_res != ERRCODE_NV_KEY_NOT_IN_WRITE_QUEUE) {
        ret_val = queue_res;
    } else {
        ret_val = store_res;
    }
#endif
    return ret_val;
}

static errcode_t nv_helper_get_store_status(nv_store_status_t *status)
{
    errcode_t ret_val = ERRCODE_FAIL;

    if (status == NULL) {
        return ERRCODE_MALLOC;
    }

    memset_s(status, sizeof(nv_key_attr_t), 0, sizeof(nv_key_attr_t));
#ifdef CONFIG_SUPPORT_NV_REMOTE_CORE
    uint16_t len = 0;
    if (uapi_rpc_cmd_kv_store_get_status((uint32_t *)&ret_val, sizeof(nv_store_status_t), (uint16_t *)&len,
        (uint8_t *)status) != ERRCODE_SUCC) {
        return ERRCODE_NV_RPC_ERROR;
    }
#else
    kv_store_t store = kv_store_from_core(CORE);
    ret_val = kv_store_get_status(store, status);
#endif
    return ret_val;
}

static void nv_helper_request_process_task()
{
#ifdef CONFIG_SUPPORT_NV_REMOTE_CORE
    (void)uapi_flash_task_request_processing();
#else
    flash_task_thread_signal();
#endif
}

static errcode_t nv_helper_add_write_key_task(task_node_data_t *data, nv_storage_completed_callback func)
{
    flash_task_node *created_task = (flash_task_node *)NULL;
    nv_task_callback *nv_callback = (nv_task_callback *)(uintptr_t)kv_malloc(sizeof(nv_task_callback));
    if (nv_callback == NULL) {
        return ERRCODE_MALLOC;
    }

    nv_callback->callback.func = nv_task_completed_callback;
    nv_callback->callback.free_func = (flash_task_data_free)kv_free;
    nv_callback->kvalue = (uint8_t *)data->kv.kvalue;
    nv_callback->user_func = func;
    if (uapi_flash_task_add_task((owner_callback *)nv_callback, FLASH_TASK_KV_DATA, data, &created_task)
        != ERRCODE_SUCC) {
        kv_free(nv_callback);
        return ERRCODE_NV_RPC_ERROR;
    }
    return ERRCODE_SUCC;
}

static void nv_helper_init(void)
{
    nv_log_info("[NV] init ...!\r\n");

    errcode_t ret;
    task_node_data_t data;
    flash_task_node *created_task = NULL;
    partition_information_t info;

    ret = uapi_partition_get_info(EXT_PARTITION_NV_DATA, &info);
    if (ret != ERRCODE_SUCC) {
        nv_log_err("[NV] Read NV region fail!\r\n");
    }

    data.kv_region.kv_addr = info.part_info.addr_info.addr;
    data.kv_region.kv_size = info.part_info.addr_info.size;
    ret = flash_task_add_task(NULL, FLASH_TASK_KV_PAGE_INIT, &data, &created_task);
    if (ret != ERRCODE_SUCC) {
        nv_log_err("[NV] Add NV task fail!\r\n");
    }

    nv_helper_request_process_task();
    nv_log_info("[NV] init success!\r\n");
}

static errcode_t nv_helper_write_force(uint16_t key, const uint8_t *kvalue, uint16_t kvalue_length)
{
    if ((kvalue == NULL) || (kvalue_length == 0)) {
        return ERRCODE_NV_INVALID_PARAMS;
    }
    errcode_t ret;
    task_node_data_t data;
    data.kv.kvalue = (uint8_t *)(uintptr_t)kv_malloc(kvalue_length);
    if (data.kv.kvalue == NULL) {
        return ERRCODE_MALLOC;
    }

    if ((memcpy_s((void *)data.kv.kvalue, kvalue_length, kvalue, kvalue_length)) != EOK) {
        kv_free((void *)data.kv.kvalue);
        return ERRCODE_FAIL;
    }

    data.kv.key = key;
    data.kv.kvalue_length = kvalue_length;
    data.kv.force_write = true;
    data.kv.attribute = (uint8_t)NV_ATTRIBUTE_NORMAL;
    ret = nv_helper_add_write_key_task(&data, NULL);
    if (ret != ERRCODE_SUCC) {
        kv_free((void *)data.kv.kvalue);
        return ret;
    }

    nv_helper_request_process_task();
    return ERRCODE_SUCC;
}

static errcode_t nv_helper_write_with_attr(uint16_t key, const uint8_t *kvalue, uint16_t kvalue_length,
    nv_key_attr_t *attr, nv_storage_completed_callback func)
{
    errcode_t ret;
    task_node_data_t data;

    if ((kvalue == NULL) || (kvalue_length == 0)) {
        return ERRCODE_NV_INVALID_PARAMS;
    }
    if (kvalue_length > nv_storage_max_key_space(attr)) {
        return ERRCODE_NV_NO_ENOUGH_SPACE;
    }
    if (g_nv_task_count > NV_TASKS_MAX_NUM) {
        return ERRCODE_NV_ILLEGAL_OPERATION;
    }
    data.kv.kvalue = (uint8_t *)(uintptr_t)kv_malloc(kvalue_length);
    if (data.kv.kvalue == NULL) {
        return ERRCODE_MALLOC;
    }

    data.kv.key = key;
    data.kv.kvalue_length = kvalue_length;
    data.kv.force_write = false;
    data.kv.attribute = (uint8_t)nv_helper_convert_key_attr(attr);

    if ((memcpy_s((void *)data.kv.kvalue, kvalue_length, kvalue, kvalue_length)) != EOK) {
        kv_free((void *)data.kv.kvalue);
        return ERRCODE_FAIL;
    }
    ret = nv_helper_add_write_key_task(&data, func);
    if (ret != ERRCODE_SUCC) {
        kv_free((void *)data.kv.kvalue);
        return ret;
    }

    g_nv_task_count++;
    nv_helper_request_process_task();
    return ERRCODE_SUCC;
}

static errcode_t nv_helper_erase(uint16_t key)
{
    errcode_t ret;
    uint8_t attributes = 0;
    uint16_t len = 0;
    task_node_data_t data;
    flash_task_node *created_task = NULL;
    nv_key_attr_t attr;
    unused(attributes);
    ret = nv_helper_get_key_attr(key, &len, &attr);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    data.kv_erase.key = key;
    if (uapi_flash_task_add_task(NULL, FLASH_TASK_KV_ERASE, &data, &created_task) != ERRCODE_SUCC) {
        return ERRCODE_NV_RPC_ERROR;
    }

    nv_helper_request_process_task();
    return ERRCODE_SUCC;
}

static errcode_t nv_helper_stored(uint16_t key, uint16_t kvalue_length, const uint8_t *kvalue)
{
    errcode_t ret_val = ERRCODE_SUCC;
    uint8_t *stored_kvalue = NULL;
    uint16_t stored_kvalue_length = 0;
    nv_key_attr_t attr;

    if ((kvalue_length == 0) || (kvalue == NULL)) {
        return ERRCODE_NV_INVALID_PARAMS;
    }

    stored_kvalue = (uint8_t *)(uintptr_t)kv_malloc(kvalue_length);
    if (stored_kvalue == NULL) {
        return ERRCODE_MALLOC;
    }

    if (nv_helper_get_key_data(key, kvalue_length, &stored_kvalue_length, stored_kvalue, &attr) != ERRCODE_SUCC) {
        ret_val = ERRCODE_FAIL;
    } else if ((stored_kvalue_length != kvalue_length) ||
               (memcmp((void *)stored_kvalue, (void *)kvalue, kvalue_length) != 0)) {
        ret_val = ERRCODE_FAIL;
    }

    kv_free(stored_kvalue);
    return ret_val;
}

static errcode_t nv_helper_update_key_attr(uint16_t key, nv_key_attr_t *attr, nv_storage_completed_callback func)
{
    task_node_data_t data;
    flash_task_node *created_task = NULL;
    if (attr == NULL) {
        return ERRCODE_NV_INVALID_PARAMS;
    }

    data.kv_attribute.key = key;
    data.kv_attribute.attribute = (uint8_t)nv_helper_convert_key_attr(attr);
    nv_task_callback *nv_callback = (nv_task_callback *)(uintptr_t)kv_malloc(sizeof(nv_task_callback));
    if (nv_callback == NULL) {
        kv_free((void *)data.kv.kvalue);
        return ERRCODE_MALLOC;
    }

    nv_callback->callback.func = nv_task_completed_callback;
    nv_callback->callback.free_func = (flash_task_data_free)kv_free;
    nv_callback->kvalue = NULL;
    nv_callback->user_func = func;
    if (uapi_flash_task_add_task((owner_callback *)nv_callback, FLASH_TASK_KV_ATTRIBUTE, &data, &created_task)
        != ERRCODE_SUCC) {
        return ERRCODE_NV_RPC_ERROR;
    }
    nv_helper_request_process_task();

    return ERRCODE_SUCC;
}

void nv_helper_set_opt_func(nv_opt_funcs_t *opts)
{
    opts->init                = nv_helper_init;
    opts->write_with_attr     = nv_helper_write_with_attr;
    opts->write_with_force    = nv_helper_write_force;
    opts->read_with_attr      = nv_helper_get_key_data;
    opts->set_key_attr        = nv_helper_update_key_attr;
    opts->get_key_attr        = nv_helper_get_key_attr;
    opts->erase_key           = nv_helper_erase;
    opts->get_store_status    = nv_helper_get_store_status;
    opts->key_cmp_with_stored = nv_helper_stored;
}

#else /* #ifndef CONFIG_NV_SUPPORT_SINGLE_CORE_SYSTEM */

nv_direct_ctrl_t g_nv_direct_ctrl;
nv_direct_ctrl_t *nv_direct_get_nv_ctrl(void)
{
    return &g_nv_direct_ctrl;
}

static errcode_t nv_direct_get_key_attr(uint16_t key_id, uint16_t *len, nv_key_attr_t *attr)
{
    kv_attributes_t get_attribute = 0;
    errcode_t ret = ERRCODE_SUCC;

    if (len == NULL || attr == NULL) {
        return ERRCODE_NV_INVALID_PARAMS;
    }

    if (osal_sem_down_timeout(&nv_sem, 0xFFFFFFFF) != ERRCODE_SUCC) {
        return ERRCODE_NV_SEM_WAIT_ERR;
    }

    *len = 0;
    memset_s(attr, sizeof(nv_key_attr_t), 0, sizeof(nv_key_attr_t));
    ret = kv_store_get_key_attr(KV_STORE_APPLICATION, key_id, len, &get_attribute);
    if (ret == ERRCODE_SUCC) {
        if (((uint32_t)get_attribute & NV_ATTRIBUTE_PERMANENT) != 0) {
            attr->permanent = true;
            attr->non_upgrade = true;
        }
        if (((uint32_t)get_attribute & NV_ATTRIBUTE_ENCRYPTED) != 0) {
            attr->encrypted = true;
            attr->non_upgrade = true;
        }
        if (((uint32_t)get_attribute & NV_ATTRIBUTE_NON_UPGRADE) != 0) {
            attr->non_upgrade = true;
        }
    }
    osal_sem_up(&nv_sem);
    return ret;
}

static errcode_t nv_direct_write_with_attr(uint16_t key_id, const uint8_t *kvalue, uint16_t kvalue_length,
    nv_key_attr_t *attr, nv_storage_completed_callback func)
{
    errcode_t ret;
    flash_task_node task_node = {0};
    uint16_t len;
    kv_attributes_t attribute = (kv_attributes_t)0;
    unused(func);

    if (kvalue_length > nv_storage_max_key_space(attr)) {
        nv_log_err("[NV] nv_direct_write_with_attr: len exceeds the max size. key_id = 0x%x\r\n", key_id);
        return ERRCODE_NV_NO_ENOUGH_SPACE;
    }

#if (CONFIG_NV_SUPPORT_ENCRYPT != NV_YES)
    if ((attr != NULL && attr->encrypted == true)) {
        nv_log_err("[NV] nv_direct_write_with_attr: encryption not support!\r\n");
        return ERRCODE_NV_ILLEGAL_OPERATION;
    }
#endif

    if (osal_sem_down_timeout(&nv_sem, 0xFFFFFFFF) != ERRCODE_SUCC) {
        nv_log_err("[NV] nv_direct_write_with_attr: semaphore error!\r\n");
        return ERRCODE_NV_SEM_WAIT_ERR;
    }

    ret = kv_store_get_key_attr(KV_STORE_APPLICATION, key_id, &len, &attribute);
    if (ret == ERRCODE_SUCC) {
        if ((((uint32_t)attribute & NV_ATTRIBUTE_PERMANENT) != 0) ||
            ((((uint32_t)attribute & NV_ATTRIBUTE_ENCRYPTED) != 0) && (attr != NULL && attr->encrypted == false))) {
            /* When old key is permanent, not permit to write */
            /* When old key is encrypted, new key is non-encrypted, not permit to write */
            nv_log_err("[NV] nv_direct_write_with_attr: operation not allowed! key_id = 0x%x\r\n", key_id);
            osal_sem_up(&nv_sem);
            return ERRCODE_NV_ILLEGAL_OPERATION;
        }
    }
    if (attr != NULL) {
        attribute = (uint8_t)nv_helper_convert_key_attr(attr);
    }
    task_node.state_code = FLASH_TASK_READY;
    task_node.data.kv.key = key_id;
    task_node.data.kv.kvalue = kvalue;
    task_node.data.kv.kvalue_length = kvalue_length;
    task_node.data.kv.force_write = false;
    task_node.data.kv.attribute = attribute;
    ret = kv_update_write_key(KV_STORE_APPLICATION, &task_node);
    osal_sem_up(&nv_sem);
    if (ret != ERRCODE_SUCC) {
        nv_log_err("[NV] nv_direct_write_with_attr failed. key_id = 0x%x, ret = 0x%x\r\n", key_id, ret);
        return ret;
    }
#if (CONFIG_NV_SUPPORT_CHANGE_NOTIFY == NV_YES)
    nv_change_notify(key_id);
#endif
    return ERRCODE_SUCC;
}

#ifdef CONFIG_NV_SUPPORT_WRITE_FORCE
static errcode_t nv_direct_write_force(uint16_t key_id, const uint8_t *kvalue, uint16_t kvalue_length)
{
    errcode_t ret;
    flash_task_node task_node = {0};
    uint16_t len;
    kv_attributes_t attribute = 0;

    if (kvalue_length > NV_NORMAL_KVALUE_MAX_LEN) {
        return ERRCODE_NV_NO_ENOUGH_SPACE;
    }

    if (osal_sem_down_timeout(&nv_sem, 0xFFFFFFFF) != ERRCODE_SUCC) {
        return ERRCODE_NV_SEM_WAIT_ERR;
    }

    ret = kv_store_get_key_attr(KV_STORE_APPLICATION, key_id, &len, &attribute);
    if (ret == ERRCODE_SUCC) {
        if (((uint32_t)attribute & NV_ATTRIBUTE_ENCRYPTED) != 0) {
            /* When old key is encrypted, not permit to write */
            osal_sem_up(&nv_sem);
            return ERRCODE_NV_ILLEGAL_OPERATION;
        }
    }

    task_node.state_code = FLASH_TASK_READY;
    task_node.data.kv.key = key_id;
    task_node.data.kv.kvalue = kvalue;
    task_node.data.kv.kvalue_length = kvalue_length;
    task_node.data.kv.force_write = true;
    task_node.data.kv.attribute = (uint8_t)NV_ATTRIBUTE_NORMAL;
    ret = kv_update_write_key(KV_STORE_APPLICATION, &task_node);
    osal_sem_up(&nv_sem);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
#if (CONFIG_NV_SUPPORT_CHANGE_NOTIFY == NV_YES)
    nv_change_notify(key_id);
#endif
    return ERRCODE_SUCC;
}
#endif

static errcode_t nv_direct_get_key_data(uint16_t key_id, uint16_t kvalue_max_length, uint16_t *kvalue_length,
                                        uint8_t *kvalue, nv_key_attr_t *attr)
{
    kv_attributes_t data_attribute = 0;

    if (osal_sem_down_timeout(&nv_sem, 0xFFFFFFFF) != ERRCODE_SUCC) {
        return ERRCODE_NV_SEM_WAIT_ERR;
    }

    kv_store_key_data_t key_data = {kvalue_max_length, 0, kvalue};
    errcode_t ret_val = kv_store_get_key(KV_STORE_APPLICATION, key_id, &key_data, &data_attribute);
    /* 如果在工作区没有找到该数据，可能存在数据被破坏情况，去备份区去找该数据 */
    if (ret_val != ERRCODE_SUCC) {
        ret_val = kv_store_read_backup_key(key_id, &key_data, &data_attribute);
    }

    if (ret_val == ERRCODE_SUCC) {
        if (((uint32_t)data_attribute & NV_ATTRIBUTE_PERMANENT) != 0) {
            attr->permanent = true;
            attr->non_upgrade = true;
        }
        if (((uint32_t)data_attribute & NV_ATTRIBUTE_ENCRYPTED) != 0) {
            attr->encrypted = true;
            attr->non_upgrade = true;
        }
        if (((uint32_t)data_attribute & NV_ATTRIBUTE_NON_UPGRADE) != 0) {
            attr->non_upgrade = true;
        }
    }

    *kvalue_length = key_data.kvalue_actual_length;
    osal_sem_up(&nv_sem);
    return ret_val;
}

#ifdef CONFIG_NV_SUPPORT_UPDATE_ATTR
static errcode_t nv_direct_update_key_attr(uint16_t key_id, nv_key_attr_t *attr, nv_storage_completed_callback func)
{
    errcode_t ret_val;
    flash_task_node task_node = {0};
    uint16_t len;
    kv_attributes_t attribute = 0;
    nv_key_attr_t new_attr = {0};
    unused(func);

    memcpy_s(&new_attr, sizeof(nv_key_attr_t), attr, sizeof(nv_key_attr_t));
    if (osal_sem_down_timeout(&nv_sem, 0xFFFFFFFF) != ERRCODE_SUCC) {
        return ERRCODE_NV_SEM_WAIT_ERR;
    }

    ret_val = kv_store_get_key_attr(KV_STORE_APPLICATION, key_id, &len, &attribute);
    if (ret_val != ERRCODE_SUCC) {
        osal_sem_up(&nv_sem);
        return ret_val;
    }

    if (((uint32_t)attribute & NV_ATTRIBUTE_PERMANENT) != 0) {
        osal_sem_up(&nv_sem);
        return ERRCODE_SUCC;
    }

    if (((uint32_t)attribute & NV_ATTRIBUTE_ENCRYPTED) != 0) {
        /* if old key is encrypted, the new key must be encrypted too */
        new_attr.encrypted = true;
    }

    task_node.state_code = FLASH_TASK_READY;
    task_node.data.kv_attribute.key = key_id;
    task_node.data.kv_attribute.attribute = (uint8_t)nv_helper_convert_key_attr(&new_attr);
    ret_val = kv_update_modify_attribute(KV_STORE_APPLICATION, &task_node);
    osal_sem_up(&nv_sem);
    return ret_val;
}
#endif

#ifdef CONFIG_NV_SUPPORT_DELETE_KEY
static errcode_t nv_direct_erase(uint16_t key_id)
{
    errcode_t ret_val;
    uint16_t len = 0;
    kv_attributes_t attribute = 0;
    flash_task_node task_node = {0};
    if (osal_sem_down_timeout(&nv_sem, 0xFFFFFFFF) != ERRCODE_SUCC) {
        return ERRCODE_NV_SEM_WAIT_ERR;
    }

    ret_val = kv_store_get_key_attr(KV_STORE_APPLICATION, key_id, &len, &attribute);
    if (ret_val != ERRCODE_SUCC) {
        osal_sem_up(&nv_sem);
        return ret_val;
    }

    if (((uint32_t)attribute & KV_ATTRIBUTE_PERMANENT) != 0) {
        osal_sem_up(&nv_sem);
        return ERRCODE_NV_ILLEGAL_OPERATION;
    }

    task_node.data.kv_erase.key = key_id;
    ret_val = kv_update_erase_key(KV_STORE_APPLICATION, &task_node);
    osal_sem_up(&nv_sem);
    return ret_val;
}
#endif

static errcode_t nv_direct_get_store_status(nv_store_status_t *status)
{
    errcode_t ret_val = ERRCODE_FAIL;
    if (osal_sem_down_timeout(&nv_sem, 0xFFFFFFFF) != ERRCODE_SUCC) {
        return ERRCODE_NV_SEM_WAIT_ERR;
    }

    memset_s(status, sizeof(nv_store_status_t), 0, sizeof(nv_store_status_t));
    ret_val = kv_store_get_status(KV_STORE_APPLICATION, status);
    osal_sem_up(&nv_sem);
    return ret_val;
}

static errcode_t nv_direct_stored(uint16_t key_id, uint16_t kvalue_length, const uint8_t *kvalue)
{
    errcode_t ret = ERRCODE_SUCC;
    uint8_t *stored_kvalue = NULL;
    kv_attributes_t attribute = 0;

    if (kvalue_length > NV_NORMAL_KVALUE_MAX_LEN) {
        return ERRCODE_NV_NO_ENOUGH_SPACE;
    }

    stored_kvalue = (uint8_t *)(uintptr_t)kv_malloc(kvalue_length);
    if (stored_kvalue == NULL) {
        return ERRCODE_MALLOC;
    }

    if (osal_sem_down_timeout(&nv_sem, 0xFFFFFFFF) != ERRCODE_SUCC) {
        kv_free(stored_kvalue);
        return ERRCODE_NV_SEM_WAIT_ERR;
    }

    kv_store_key_data_t key_data = {kvalue_length, 0, stored_kvalue};
    if (kv_store_get_key(KV_STORE_APPLICATION, key_id, &key_data, &attribute) != ERRCODE_SUCC) {
        ret = ERRCODE_FAIL;
    } else if ((key_data.kvalue_actual_length != kvalue_length) ||
               (memcmp((void *)stored_kvalue, (void *)kvalue, kvalue_length) != 0)) {
        ret = ERRCODE_FAIL;
    }
    osal_sem_up(&nv_sem);
    kv_free(stored_kvalue);
    return ret;
}

static errcode_t nv_direct_backup_keys(const nv_backup_mode_t *backup_mode)
{
#if (CONFIG_NV_SUPPORT_BACKUP_RESTORE == NV_YES)
    errcode_t ret;
    if (osal_sem_down_timeout(&nv_sem, 0xFFFFFFFF) != ERRCODE_SUCC) {
        return ERRCODE_NV_SEM_WAIT_ERR;
    }
    ret = kv_restore_set_region_flag(backup_mode->region_mode);
    if (ret != ERRCODE_SUCC) {
        osal_sem_up(&nv_sem);
        return ret;
    }
    ret = kv_backup_delete_repeat_key();
    if (ret != ERRCODE_SUCC) {
        osal_sem_up(&nv_sem);
        return ret;
    }
    ret = kv_backup_write_key();
    osal_sem_up(&nv_sem);
    return ret;
#else
    unused(backup_mode);
    return ERRCODE_NOT_SUPPORT;
#endif
}

static errcode_t nv_direct_set_restore_flag_all(void)
{
#if (CONFIG_NV_SUPPORT_BACKUP_RESTORE == NV_YES)
    nv_reset_mode_t nv_reset_mode = {0};
    nv_reset_mode.mode = RESET_MODE_A;
    return uapi_nv_write(NV_ID_RESTORE_ENABLE, (uint8_t *)&nv_reset_mode, sizeof(nv_reset_mode_t));
#else
    return ERRCODE_NOT_SUPPORT;
#endif
}

static errcode_t nv_direct_set_restore_flag_partitial(const nv_restore_mode_t *nv_restore_mode)
{
#if (CONFIG_NV_SUPPORT_BACKUP_RESTORE == NV_YES)
    if (nv_restore_mode == NULL) {
        return ERRCODE_NV_INVALID_PARAMS;
    }
    nv_reset_mode_t nv_reset_mode = {0};
    if (memcpy_s(nv_reset_mode.region_flag, KEY_ID_REGION_MAX_NUM, nv_restore_mode, KEY_ID_REGION_MAX_NUM) != EOK) {
        return ERRCODE_MEMCPY;
    }
    nv_reset_mode.mode = RESET_MODE_B;
    return uapi_nv_write(NV_ID_RESTORE_ENABLE, (uint8_t *)&nv_reset_mode, sizeof(nv_reset_mode_t));
#else
    unused(nv_restore_mode);
    return ERRCODE_NOT_SUPPORT;
#endif
}

#if (CONFIG_NV_SUPPORT_CHANGE_NOTIFY == NV_YES)
static errcode_t nv_direct_add_func_to_notify_list(uint16_t min_key, uint16_t max_key, nv_changed_notify_func func)
{
    nv_direct_ctrl_t *nv_ctrl = nv_direct_get_nv_ctrl();
    if (osal_sem_down_timeout(&nv_sem, 0xFFFFFFFF) != ERRCODE_SUCC) {
        return ERRCODE_NV_SEM_WAIT_ERR;
    }

    nv_changed_proc_t *notify_list = nv_ctrl->nv_change_notify_list;
    if (notify_list == NULL) {
        osal_sem_up(&nv_sem);
        return ERRCODE_NV_NOT_INITIALISED;
    }

    if (nv_ctrl->notify_registered_nums >= nv_ctrl->notify_regitser_max_nums) {
        osal_sem_up(&nv_sem);
        return ERRCODE_NV_NOTIFY_LIST_FULL;
    }

    if (!nv_change_notify_segment_is_valid(nv_ctrl, min_key, max_key)) {
        osal_sem_up(&nv_sem);
        return ERRCODE_NV_NOTIFY_SEGMENT_ERR;
    }

    uint8_t new_index = nv_ctrl->notify_registered_nums;
    notify_list[new_index].min_key = min_key;
    notify_list[new_index].max_key = max_key;
    notify_list[new_index].func    = func;
    nv_ctrl->notify_registered_nums++;
    osal_sem_up(&nv_sem);
    return ERRCODE_SUCC;
}
#endif

static void nv_direct_ctrl_init(void)
{
    (void)kv_update_init((cores_t)KV_STORE_APPLICATION);
    nv_direct_ctrl_t *nv_ctrl = nv_direct_get_nv_ctrl();
    memset_s(nv_ctrl, sizeof(nv_direct_ctrl_t), 0, sizeof(nv_direct_ctrl_t));
    if (osal_sem_binary_sem_init(&nv_sem, MAX_BINARY_VAL) != ERRCODE_SUCC) {
        return;
    }
#if (CONFIG_NV_SUPPORT_CHANGE_NOTIFY == NV_YES)
    (void)nv_direct_notify_list_init();
#endif
#if (CONFIG_NV_SUPPORT_BACKUP_RESTORE == NV_YES)
    (void)kv_update_backup_init();
#endif
#if (CONFIG_NV_SUPPORT_BACKUP_RESTORE == NV_YES)
    (void)kv_restore_all_keys();
#endif
    nv_log_info("nv init success!\r\n");
}

void nv_direct_set_opt_func(nv_opt_funcs_t *opts)
{
    opts->init             = nv_direct_ctrl_init;
    opts->write_with_attr  = nv_direct_write_with_attr;
#ifdef CONFIG_NV_SUPPORT_WRITE_FORCE
    opts->write_with_force = nv_direct_write_force;
#endif
    opts->read_with_attr   = nv_direct_get_key_data;
#ifdef CONFIG_NV_SUPPORT_UPDATE_ATTR
    opts->set_key_attr     = nv_direct_update_key_attr;
#endif
    opts->get_key_attr     = nv_direct_get_key_attr;
#ifdef CONFIG_NV_SUPPORT_DELETE_KEY
    opts->erase_key        = nv_direct_erase;
#endif
    opts->get_store_status =  nv_direct_get_store_status;
    opts->key_cmp_with_stored = nv_direct_stored;
    opts->backup_keys      = nv_direct_backup_keys;
    opts->set_restore_flag_all = nv_direct_set_restore_flag_all;
    opts->set_restore_flag_partitial = nv_direct_set_restore_flag_partitial;
#if (CONFIG_NV_SUPPORT_CHANGE_NOTIFY == NV_YES)
    opts->notify_register  = nv_direct_add_func_to_notify_list;
#endif
}

#endif /* #ifndef CONFIG_NV_SUPPORT_SINGLE_CORE_SYSTEM */