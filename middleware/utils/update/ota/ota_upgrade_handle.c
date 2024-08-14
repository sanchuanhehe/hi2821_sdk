/*
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: ota upgrade handle.
 * This file should be changed only infrequently and with great care.
 */

#include "build_version.h"
#include "securec.h"
#include "osal_addr.h"
#include "diag_service.h"
#include "partition.h"
#include "transmit_st.h"
#include "chip_io.h"
#include "upg.h"
#include "upg_common_porting.h"
#include "upg_porting.h"
#include "upg_definitions.h"
#include "tcxo.h"
#include "dfx_adapt_layer.h"
#include "ota_upgrade_handle.h"

#define upgrade_build_tlv_type(type, id)       (((type) << (0x7)) | (id))
#define upgrade_build_tlv_len(size)               (((1) << (0x7)) | (size))
#define OTA_MSG_LEN 8
#define OTA_ACK_MSG_INDEX 4

#if defined(CONFIG_DFX_SUPPORT_CONTINUOUSLY_TRANSMIT) && (CONFIG_DFX_SUPPORT_CONTINUOUSLY_TRANSMIT == DFX_YES)
extern errcode_t transmit_get_progress(uint16_t transmit_type, uint32_t *offset);

static uint32_t upgrade_get_hash(uint8_t *hash, uint32_t len)
{
    return upg_flash_read(OTA_HASH_START, len, hash);
}
#endif

static void *upgrade_get_tlv_payload(uint8_t *cmd_param)
{
    tlv_t *tlv = (tlv_t *)cmd_param;
    uint8_t offset = 0;

    if ((tlv->len & 0x80) != 0) {
        offset = (uint8_t)sizeof(tlv_t);
    } else {
        offset = (uint8_t)(sizeof(tlv_t) + (tlv->len & 0x7F));
    }

    return ((uint8_t *)cmd_param + offset);
}

static errcode_t upgrade_set_enable_flag(void)
{
    return ERRCODE_SUCC;
}

static errcode_t upgrade_send_ack_pkt(uint8_t cmd_id, const uint8_t *pkt, uint32_t pkt_size, uint8_t dst)
{
    errcode_t ret;
    uint16_t buf_len = (uint16_t)(sizeof(diag_ser_header_t) + sizeof(diag_ser_frame_t) +
                       sizeof(tlv_t) + (uint16_t)pkt_size);
    uint8_t *buf = osal_vmalloc(buf_len);
    if (buf == NULL) {
        return ERRCODE_MALLOC;
    }

    diag_ser_header_t *header = (diag_ser_header_t *)buf;
    header->ser_id = DIAG_SER_OTA;
    header->cmd_id = cmd_id;
    header->src = 0x02;
    header->dst = dst;
    header->crc_en = true;
    header->ack_en = false;
    header->length = (uint16_t)(buf_len - sizeof(diag_ser_header_t));

    diag_ser_frame_t *frame = (diag_ser_frame_t *)(buf + sizeof(diag_ser_header_t));
    frame->module_id = DIAG_SER_OTA;
    frame->cmd_id = cmd_id;

    tlv_t *tlv = (tlv_t *)((uintptr_t)frame + sizeof(diag_ser_frame_t));
    tlv->type = upgrade_build_tlv_type(1, cmd_id);
    tlv->len = (uint8_t)upgrade_build_tlv_len((uint16_t)pkt_size);

    if (memcpy_s(((uint8_t *)tlv + sizeof(tlv_t)), pkt_size, pkt, pkt_size) != EOK) {
        osal_vfree(buf);
        return ERRCODE_FAIL;
    }

    ret = uapi_diag_service_send_data((diag_ser_data_t *)(uintptr_t)header);

    osal_vfree(buf);
    return ret;
}

errcode_t ota_upgrade_prepare(uint8_t cmd_id, uint8_t *cmd_param, uint16_t cmd_param_size, uint8_t dst)
{
    UNUSED(cmd_param_size);
    uint32_t file_length;
    uint8_t *value = (uint8_t *)upgrade_get_tlv_payload(cmd_param);
    errcode_t ret = ERRCODE_FAIL;
    if (memcpy_s(&file_length, sizeof(uint32_t), (uint8_t *)value, sizeof(uint32_t)) != EOK) {
        return ERRCODE_FAIL;
    }
    upg_prepare_info_t info;
    info.package_len = file_length;
    ret = uapi_upg_prepare(&info);
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
    return upgrade_send_ack_pkt(cmd_id, (uint8_t *)&ret, sizeof(errcode_t), dst);
}

errcode_t ota_upgrade_request(uint8_t cmd_id, uint8_t *cmd_param, uint16_t cmd_param_size, uint8_t dst)
{
    UNUSED(cmd_param_size);
    UNUSED(cmd_param);
    uint32_t ret = ERRCODE_SUCC;
    return upgrade_send_ack_pkt(cmd_id, (uint8_t *)&ret, sizeof(errcode_t), dst);
}

errcode_t ota_upgrade_start(uint8_t cmd_id, uint8_t *cmd_param, uint16_t cmd_param_size, uint8_t dst)
{
    UNUSED(cmd_param_size);
    UNUSED(cmd_param);
    if (upgrade_set_enable_flag() != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
    uint32_t ret = ERRCODE_SUCC;
    ret = uapi_upg_request_upgrade(false);
    upgrade_send_ack_pkt(cmd_id, (uint8_t *)&ret, sizeof(errcode_t), dst);
#if defined(CONFIG_DFX_SUPPORT_CONTINUOUSLY_TRANSMIT) && (CONFIG_DFX_SUPPORT_CONTINUOUSLY_TRANSMIT == DFX_YES)
    ret = upg_flash_erase(OTA_TRANSMIT_INFO_START, OTA_TRANSMIT_INFO_LEN);
#endif
    upg_reboot();
    return ret;
}

static errcode_t upgrade_get_version(uint8_t cmd_id, const uint8_t *cmd_param, uint16_t cmd_param_size, uint8_t dst)
{
    UNUSED(cmd_param);
    UNUSED(cmd_param_size);
    uint8_t return_message[OTA_MSG_LEN] = {0};
    info_ack_t *ret = (info_ack_t *)return_message;
    ret->info_type = UPG_INFO_TYPE_VER;
    ret->info_size = (uint16_t)sizeof(uint32_t);
    uint32_t info = 0;

    if (memcpy_s(((uint8_t *)&return_message[OTA_ACK_MSG_INDEX]), sizeof(info), &info, sizeof(info)) != EOK) {
        return ERRCODE_FAIL;
    }
    return upgrade_send_ack_pkt(cmd_id, (uint8_t *)return_message, OTA_MSG_LEN, dst);
}

static errcode_t upgrade_get_permission(uint8_t cmd_id, const uint8_t *cmd_param, uint16_t cmd_param_size, uint8_t dst)
{
    UNUSED(cmd_param_size);
    UNUSED(cmd_param);
    uint8_t return_message[OTA_MSG_LEN] = {0};
    info_ack_t *ret = (info_ack_t *)return_message;
    ret->info_type = UPG_INFO_TYPE_PERMIT;
    ret->info_size = (uint16_t)sizeof(uint32_t);
    uint32_t info = 0;
    if (memcpy_s(((uint8_t *)&return_message[OTA_ACK_MSG_INDEX]), sizeof(info), &info, sizeof(info)) != EOK) {
        return ERRCODE_FAIL;
    }
    return upgrade_send_ack_pkt(cmd_id, (uint8_t *)return_message, OTA_MSG_LEN, dst);
}

static errcode_t upgrade_ack_version(uint8_t cmd_id, uint8_t *cmd_param, uint16_t cmd_param_size, uint8_t dst)
{
    UNUSED(cmd_param_size);
    info_ack_t *info_ack = (info_ack_t *)upgrade_get_tlv_payload(cmd_param);
    uint8_t *info = (uint8_t *)((uintptr_t)info_ack + sizeof(info_ack_t));
    uint32_t version = 0;
    if (memcpy_s(&version, info_ack->info_size, info, info_ack->info_size) != EOK) {
        return ERRCODE_FAIL;
    }
    const build_version_info *info_version = build_version_get_info();
    uint32_t result = 0;
    if (info_version->version < version) {
        result = 0;
    } else {
        result = 1;
    }
    info_ack_t ret = { 0 };
    ret.info_type = UPG_INFO_TYPE_VER;
    ret.info_size = (uint16_t)sizeof(uint32_t);
    if (memcpy_s((uint8_t *)(((uintptr_t)&ret) + sizeof(info_ack_t)), sizeof(uint32_t), \
                 (uint8_t *)&result, sizeof(uint32_t)) != EOK) {
        return ERRCODE_FAIL;
    }
    return upgrade_send_ack_pkt(cmd_id, (uint8_t *)&ret, (sizeof(info_ack_t) + sizeof(uint32_t)), dst);
}

static errcode_t upgrade_ack_permission(uint8_t cmd_id, const uint8_t *cmd_param, uint16_t cmd_param_size, uint8_t dst)
{
    UNUSED(cmd_param_size);
    UNUSED(cmd_param);
    info_ack_t ret = { 0 };
    ret.info_type = UPG_INFO_TYPE_PERMIT;
    ret.info_size = (uint16_t)sizeof(uint32_t);
    uint32_t result = 0;
    if (memcpy_s((uint8_t *)(((uintptr_t)&ret) + sizeof(info_ack_t)), sizeof(uint32_t), \
                 (uint8_t *)&result, sizeof(uint32_t)) != EOK) {
        return ERRCODE_FAIL;
    }
    return upgrade_send_ack_pkt(cmd_id, (uint8_t *)&ret, (sizeof(info_ack_t) + sizeof(uint32_t)), dst);
}

static errcode_t upgrade_ack_resend(uint8_t cmd_id, uint8_t *cmd_param, uint16_t cmd_param_size, uint8_t dst)
{
    UNUSED(cmd_param_size);
    UNUSED(cmd_param);
    uint32_t offset = 0;
#if defined(CONFIG_DFX_SUPPORT_CONTINUOUSLY_TRANSMIT) && (CONFIG_DFX_SUPPORT_CONTINUOUSLY_TRANSMIT == DFX_YES)
    info_ack_t *info_ack = (info_ack_t *)upgrade_get_tlv_payload(cmd_param);
    uint8_t *info = ((uint8_t *)info_ack + sizeof(info_ack_t));
    uint32_t hash_size = info_ack->info_size;
    uint8_t *ack_hash = (uint8_t *)osal_vmalloc(hash_size);
    if (ack_hash == NULL) {
        return ERRCODE_MALLOC;
    }
    if (memcpy_s(ack_hash, hash_size, info, hash_size) != EOK) {
        osal_vfree(ack_hash);
        return ERRCODE_FAIL;
    }
    uint8_t *hash = (uint8_t *)osal_vmalloc(hash_size);
    if (hash == NULL) {
        return ERRCODE_MALLOC;
    }

    upgrade_get_hash(hash, hash_size);
    if (memcmp((uint8_t *)hash, (uint8_t *)ack_hash, hash_size) == 0) {
        transmit_get_progress(TRANSMIT_TYPE_SAVE_OTA_IMG, (uint32_t *)&offset);
    } else {
        upg_flash_erase(OTA_TRANSMIT_INFO_START, OTA_TRANSMIT_INFO_LEN);
        upg_flash_write(OTA_HASH_START, OTA_HASH_INFO_SIZE, ack_hash, false);
    }
    osal_vfree(hash);
    osal_vfree(ack_hash);
#endif
    uint8_t return_message[OTA_MSG_LEN] = {0};
    info_ack_t *ret = (info_ack_t *)return_message;
    ret->info_type = UPG_INFO_TYPE_RESEND;
    ret->info_size = (uint16_t)sizeof(offset);

    if (memcpy_s(((uint8_t *)&return_message[OTA_ACK_MSG_INDEX]), sizeof(offset), &offset, sizeof(offset)) != EOK) {
        return ERRCODE_FAIL;
    }
    return upgrade_send_ack_pkt(cmd_id, (uint8_t *)return_message, OTA_MSG_LEN, dst);
}

errcode_t ota_upgrade_getinfo(uint8_t cmd_id, uint8_t *cmd_param, uint16_t cmd_param_size, uint8_t dst)
{
    tlv_t *tlv = (tlv_t *)cmd_param;
    if ((tlv->type & ~(1 << 0x7)) == 0x1) {
        uint16_t info = 0;
        uint8_t *info_type = (uint8_t *)upgrade_get_tlv_payload(cmd_param);
        if (memcpy_s(&info, sizeof(uint16_t), info_type, sizeof(uint16_t)) != EOK) {
            return ERRCODE_FAIL;
        }
        switch (info) {
            case UPG_INFO_TYPE_VER:
                upgrade_get_version(cmd_id, (const uint8_t *)cmd_param, cmd_param_size, dst);
                break;
            case UPG_INFO_TYPE_PERMIT:
            case UPG_INFO_TYPE_RESEND:
            case UPG_INFO_TYPE_IMAGE_FLAG:
                upgrade_get_permission(cmd_id, (const uint8_t *)cmd_param, cmd_param_size, dst);
                break;
            default:
                break;
        }
    } else if ((tlv->type & ~(1 << 0x7)) == 0x2) {
        info_ack_t *info_ack = (info_ack_t *)upgrade_get_tlv_payload(cmd_param);
        switch (info_ack->info_type) {
            case UPG_INFO_TYPE_VER:
                upgrade_ack_version(cmd_id, cmd_param, cmd_param_size, dst);
                break;
            case UPG_INFO_TYPE_PERMIT:
                upgrade_ack_permission(cmd_id, cmd_param, cmd_param_size, dst);
                break;
            case UPG_INFO_TYPE_RESEND:
            case UPG_INFO_TYPE_IMAGE_FLAG:
                upgrade_ack_resend(cmd_id, cmd_param, cmd_param_size, dst);
                break;
            default:
                break;
        }
    }
    return ERRCODE_SUCC;
}