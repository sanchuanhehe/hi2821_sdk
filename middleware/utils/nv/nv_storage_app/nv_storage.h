/*
 * Copyright (c) @CompanyNameMagicTag 2021-2022. All rights reserved.
 * Description: KEY VALUE STORAGE INTERNAL INTERFACE
 */

#ifndef NV_STORAGE_H
#define NV_STORAGE_H

#ifdef CONFIG_NV_SUPPORT_SINGLE_CORE_SYSTEM
#include "nv_notify.h"
#include "nv.h"
#endif

#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * KV key attribute flags
 * A key with a normal or encrypted attribute can be set to a permanent attribute again.
 * A key with a permanent attribute cannot be set to another attribute.
 * Attribute 3 is considered to be both encrypted and permanent.
 */
typedef enum {
    NV_ATTRIBUTE_NORMAL       = 0x0,    /* Default type, no special attribute */
    NV_ATTRIBUTE_PERMANENT    = 0x1,    /* Key is permanent and can't be deleted or modified */
    NV_ATTRIBUTE_ENCRYPTED    = 0x2,    /* Key is encrypted in flash, The key can be encrypted permanently */
    NV_ATTRIBUTE_NON_UPGRADE  = 0x4
} nv_attributes_t;

typedef void  (* nv_opt_init)(void);
typedef errcode_t (* nv_opt_write_with_attr)(uint16_t key, const uint8_t *kvalue, uint16_t kvalue_length,\
    nv_key_attr_t *attr, nv_storage_completed_callback func);
typedef errcode_t (* nv_opt_write_with_force)(uint16_t key, const uint8_t *kvalue, uint16_t kvalue_length);
typedef errcode_t (* nv_opt_read_with_attr)(uint16_t key, uint16_t kvalue_max_length,uint16_t *kvalue_length,\
    uint8_t *kvalue, nv_key_attr_t *attr);
typedef errcode_t (* nv_opt_set_key_attr)(uint16_t key, nv_key_attr_t *attr, nv_storage_completed_callback func);
typedef errcode_t (* nv_opt_get_key_attr)(uint16_t key, uint16_t *length, nv_key_attr_t *attr);
typedef errcode_t (* nv_opt_erase_key)(uint16_t key);
typedef errcode_t (* nv_opt_get_store_status)(nv_store_status_t *status);
typedef errcode_t (* nv_opt_key_cmp_with_stored)(uint16_t key, uint16_t kvalue_length, const uint8_t *kvalue);
typedef errcode_t (* nv_opt_backup)(const nv_backup_mode_t *backup_mode);
typedef errcode_t (* nv_opt_set_restore_flag_all)(void);
typedef errcode_t (* nv_opt_set_restore_flag_partitial)(const nv_restore_mode_t *nv_restore_mode);

#ifdef CONFIG_NV_SUPPORT_SINGLE_CORE_SYSTEM
typedef errcode_t (* nv_opt_register_change_notify)(uint16_t min_key, uint16_t max_key, nv_changed_notify_func func);
#endif
typedef struct {
    nv_opt_init                   init;
    nv_opt_write_with_attr        write_with_attr;
    nv_opt_write_with_force       write_with_force;
    nv_opt_read_with_attr         read_with_attr;
    nv_opt_set_key_attr           set_key_attr;
    nv_opt_get_key_attr           get_key_attr;
    nv_opt_erase_key              erase_key;
    nv_opt_get_store_status       get_store_status;
    nv_opt_key_cmp_with_stored    key_cmp_with_stored;
    nv_opt_backup                 backup_keys;
    nv_opt_set_restore_flag_all   set_restore_flag_all;
    nv_opt_set_restore_flag_partitial set_restore_flag_partitial;

#ifdef CONFIG_NV_SUPPORT_SINGLE_CORE_SYSTEM
    nv_opt_register_change_notify notify_register;
#endif
} nv_opt_funcs_t;

#ifdef CONFIG_NV_SUPPORT_SINGLE_CORE_SYSTEM
nv_direct_ctrl_t *nv_direct_get_nv_ctrl(void);
void nv_direct_set_opt_func(nv_opt_funcs_t *opts);
#endif
void nv_helper_set_opt_func(nv_opt_funcs_t *opts);

/**
 * Force store a key value pair, even if the key has been set to permanent.
 * @param key key to associate kvalue to
 * @param kvalue value to store
 * @param kvalue_length length in bytes of kvalue
 * @return SOC_ERR_SUCCESS or an error code
 */
errcode_t uapi_nv_write_force(uint16_t key, const uint8_t *kvalue, uint16_t kvalue_length);

/**
 * Erase an stored value given its key
 * @param key key of the key to erase
 * @return SOC_ERR_SUCCESS or an error code
 */
errcode_t uapi_nv_delete_key(uint16_t key);

/**
 * Update the attributes of an existing key.
 * @param key key to associate make permanent
 * @param attr Sets the storage attributes of a key.
 * @return SOC_ERR_SUCCESS or an error code
 * @note This key can not then be deleted or overwitten, and there is no mechanism to undo this
 */
errcode_t uapi_nv_update_key_attr(uint16_t key, nv_key_attr_t *attr, nv_storage_completed_callback func);

/**
 * Get key information associated with a specific key
 * @param key key of the value to get
 * @param length length of the current key
 * @param attr The storage attributes of a key.
 * @return SOC_ERR_SUCCESS or an error code
 */
errcode_t uapi_nv_get_key_attr(uint16_t key, uint16_t *length, nv_key_attr_t *attr);

 /**
 * @ingroup iot_nv
 * @brief compare a value associated with a specific key.
 *        CNcomment:检验特定NV的值 CNend
 *
 * @par 描述：
 * Check whether NV data has been stored. If the stored data is the same
 * as the input parameter data,a success message is returned.
 * CNcomment:检查NV数据是否已存储。如果存储的数据与入参数据相同，则返回成功 CNend
 * @attention None.
 *
 * @param key               [IN] The key to which the kvalue is associated.
 *                               CNcomment:要检验的NV ID CNend
 * @param kvalue_length     [IN] Length of kvalue in bytes.
 *                               CNcomment:要检验的NV长度 CNend
 * @param kvalue            [IN] The value to store.CNcomment:待进行对比的NV值 CNend
 *
 * @retval #true                Success
 * @retval #false               Failure
 *
 * @par 依赖：
 * @li nv
 * @li nv.h
 * @see None
 */
bool uapi_nv_is_stored(uint16_t key, uint16_t kvalue_length, const uint8_t *kvalue);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* NV_STORAGE_H */
