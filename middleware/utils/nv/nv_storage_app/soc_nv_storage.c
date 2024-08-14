/*
 * Copyright (c) @CompanyNameMagicTag 2016-2022. All rights reserved.
 * Description: SOC KEY VALUE STORAGE IMPLEMENTATION
 */

#include "nv.h"
#include "nv_storage.h"
#include "nv_store.h"
#include "nv_reset.h"
#include "securec.h"
#include "nv_porting.h"
#include "common_def.h"
#include "nv_config.h"
#include "systick.h"
#ifdef CONFIG_NV_SUPPORT_SINGLE_CORE_SYSTEM
#include "nv_update.h"
#include "nv_notify.h"
#include "nv_task_adapt.h"
#endif

nv_opt_funcs_t g_nv_opt_funcs = {
    .init                = NULL,
    .write_with_attr     = NULL,
    .write_with_force    = NULL,
    .read_with_attr      = NULL,
    .set_key_attr        = NULL,
    .get_key_attr        = NULL,
    .erase_key           = NULL,
    .get_store_status    = NULL,
    .key_cmp_with_stored = NULL,
    .backup_keys         = NULL,
    .set_restore_flag_all = NULL,
    .set_restore_flag_partitial = NULL,
#ifdef CONFIG_NV_SUPPORT_SINGLE_CORE_SYSTEM
    .notify_register     = NULL,
#endif
};

void uapi_nv_init(void)
{
#ifdef CONFIG_NV_SUPPORT_SINGLE_CORE_SYSTEM
    nv_direct_set_opt_func(&g_nv_opt_funcs);
#else
    nv_helper_set_opt_func(&g_nv_opt_funcs);
#endif
    if (g_nv_opt_funcs.init != NULL) {
        g_nv_opt_funcs.init();
    }
}

errcode_t uapi_nv_write_with_attr(uint16_t key, const uint8_t *kvalue, uint16_t kvalue_length, nv_key_attr_t *attr,
                                  nv_storage_completed_callback func)
{
    if (kvalue == NULL || kvalue_length == 0) {
        return ERRCODE_NV_INVALID_PARAMS;
    }
    if (g_nv_opt_funcs.write_with_attr == NULL) {
        return ERRCODE_NV_INIT_FAILED;
    }
    uint64_t start_time = uapi_systick_get_ms();
    errcode_t ret = g_nv_opt_funcs.write_with_attr(key, kvalue, kvalue_length, attr, func);
    uint64_t end_time = uapi_systick_get_ms();
    nv_log_debug("[NV] write key (key_id = 0x%x len = %d) ret = %d. takes %lld ms\r\n",
        key, kvalue_length, ret, (end_time - start_time));
    unused(start_time);
    unused(end_time);
    return ret;
}

errcode_t uapi_nv_write(uint16_t key, const uint8_t *kvalue, uint16_t kvalue_length)
{
    return uapi_nv_write_with_attr(key, kvalue, kvalue_length, NULL, NULL);
}

errcode_t uapi_nv_write_force(uint16_t key, const uint8_t *kvalue, uint16_t kvalue_length)
{
    if (kvalue == NULL || kvalue_length == 0) {
        return ERRCODE_NV_INVALID_PARAMS;
    }

    if (g_nv_opt_funcs.write_with_force == NULL) {
        return ERRCODE_NV_INIT_FAILED;
    }
    return g_nv_opt_funcs.write_with_force(key, kvalue, kvalue_length);
}

errcode_t uapi_nv_update_key_attr(uint16_t key, nv_key_attr_t *attr, nv_storage_completed_callback func)
{
    if (attr == NULL) {
        return ERRCODE_NV_INVALID_PARAMS;
    }

    if (g_nv_opt_funcs.set_key_attr == NULL) {
        return ERRCODE_NV_INIT_FAILED;
    }
    return g_nv_opt_funcs.set_key_attr(key, attr, func);
}

errcode_t uapi_nv_get_key_attr(uint16_t key, uint16_t *length, nv_key_attr_t *attr)
{
    if (attr == NULL) {
        return ERRCODE_NV_INVALID_PARAMS;
    }

    if (g_nv_opt_funcs.get_key_attr == NULL) {
        return ERRCODE_NV_INIT_FAILED;
    }
    return g_nv_opt_funcs.get_key_attr(key, length, attr);
}

errcode_t uapi_nv_read_with_attr(uint16_t key, uint16_t kvalue_max_length, uint16_t *kvalue_length, uint8_t *kvalue,
                                 nv_key_attr_t *attr)
{
    if (kvalue_length == NULL || kvalue == NULL || attr == NULL) {
        return ERRCODE_NV_INVALID_PARAMS;
    }

    if (g_nv_opt_funcs.read_with_attr == NULL) {
        return ERRCODE_NV_INIT_FAILED;
    }
    uint64_t start_time = uapi_systick_get_ms();
    errcode_t ret = g_nv_opt_funcs.read_with_attr(key, kvalue_max_length, kvalue_length, kvalue, attr);
    uint64_t end_time = uapi_systick_get_ms();
    nv_log_debug("[NV] read key (key_id = 0x%x len = %d) ret = 0x%x. takes %lld ms\r\n",
        key, *kvalue_length, ret, (end_time - start_time));
    unused(start_time);
    unused(end_time);
    return ret;
}

errcode_t uapi_nv_read(uint16_t key, uint16_t kvalue_max_length, uint16_t *kvalue_length, uint8_t *kvalue)
{
    if (kvalue_length == NULL || kvalue == NULL) {
        return ERRCODE_NV_INVALID_PARAMS;
    }
    nv_key_attr_t attr;
    return uapi_nv_read_with_attr(key, kvalue_max_length, kvalue_length, kvalue, &attr);
}

errcode_t uapi_nv_delete_key(uint16_t key)
{
    if (g_nv_opt_funcs.erase_key == NULL) {
        return ERRCODE_NV_INIT_FAILED;
    }
    return g_nv_opt_funcs.erase_key(key);
}

bool uapi_nv_is_stored(uint16_t key, uint16_t kvalue_length, const uint8_t *kvalue)
{
    if (kvalue_length == 0 || kvalue == NULL) {
        return false;
    }

    if (g_nv_opt_funcs.key_cmp_with_stored == NULL) {
        return false;
    }

    errcode_t ret = g_nv_opt_funcs.key_cmp_with_stored(key, kvalue_length, kvalue);
    if (ret == ERRCODE_SUCC) {
        return true;
    } else {
        return false;
    }
}

errcode_t uapi_nv_get_store_status(nv_store_status_t *status)
{
    if (status == NULL) {
        return ERRCODE_NV_INVALID_PARAMS;
    }

    if (g_nv_opt_funcs.get_store_status == NULL) {
        return ERRCODE_NV_INIT_FAILED;
    }

    uint64_t start_time = uapi_systick_get_ms();
    errcode_t ret = g_nv_opt_funcs.get_store_status(status);
    uint64_t end_time = uapi_systick_get_ms();
    nv_log_debug("[NV] get store status take %lld ms\r\n", (end_time - start_time));
    unused(start_time);
    unused(end_time);
    return ret;
}

errcode_t uapi_nv_backup(const nv_backup_mode_t *backup_mode)
{
#if (CONFIG_NV_SUPPORT_BACKUP_RESTORE == NV_YES)
    if (backup_mode == NULL) {
        return ERRCODE_NV_INVALID_PARAMS;
    }

    if (g_nv_opt_funcs.backup_keys == NULL) {
        return ERRCODE_NV_INIT_FAILED;
    }
    return g_nv_opt_funcs.backup_keys(backup_mode);
#else
    unused(backup_mode);
    return ERRCODE_NOT_SUPPORT;
#endif
}

errcode_t uapi_nv_set_restore_mode_all(void)
{
#if (CONFIG_NV_SUPPORT_BACKUP_RESTORE == NV_YES)
    if (g_nv_opt_funcs.set_restore_flag_all == NULL) {
        return ERRCODE_NV_INIT_FAILED;
    }

    return g_nv_opt_funcs.set_restore_flag_all();
#else
    return ERRCODE_NOT_SUPPORT;
#endif
}

errcode_t uapi_nv_set_restore_mode_partitial(const nv_restore_mode_t *restore_mode)
{
#if (CONFIG_NV_SUPPORT_BACKUP_RESTORE == NV_YES)
    if (g_nv_opt_funcs.set_restore_flag_partitial == NULL) {
        return ERRCODE_NV_INIT_FAILED;
    }
    return g_nv_opt_funcs.set_restore_flag_partitial(restore_mode);
#else
    unused(restore_mode);
    return ERRCODE_NOT_SUPPORT;
#endif
}

#ifdef CONFIG_NV_SUPPORT_SINGLE_CORE_SYSTEM
errcode_t uapi_nv_register_change_notify_proc(uint16_t min_key, uint16_t max_key, nv_changed_notify_func func)
{
    if ((min_key > max_key) || (func == NULL)) {
        return ERRCODE_NV_INVALID_PARAMS;
    }

    if (nv_direct_get_nv_ctrl()->notify_regitser_max_nums == 0) {
        return ERRCODE_NV_INVALID_PARAMS;
    }

    if (g_nv_opt_funcs.notify_register == NULL) {
        return ERRCODE_NV_INIT_FAILED;
    }
    return g_nv_opt_funcs.notify_register(min_key, max_key, func);
}
#endif
