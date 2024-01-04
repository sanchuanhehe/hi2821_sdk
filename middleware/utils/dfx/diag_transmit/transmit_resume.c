/*
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: dfx transmit resume transfer
 * This file should be changed only infrequently and with great care.
 */

#include "transmit_resume.h"
#include "transmit_st.h"
#include "dfx_feature_config.h"
#include "dfx_adapt_layer.h"

#if (CONFIG_DFX_SUPPORT_CONTINUOUSLY_TRANSMIT == DFX_YES)

#ifndef TRANSMIT_OTA_INFO_START
#define TRANSMIT_OTA_INFO_START 0
#endif
#ifndef TRANSMIT_OTA_INFO_END
#define TRANSMIT_OTA_INFO_END 0
#endif
#ifndef TRANSMIT_OTA_INFO_SIZE
#define TRANSMIT_OTA_INFO_SIZE 0
#endif
#ifndef TRANSMIT_OTA_DATA_START
#define TRANSMIT_OTA_DATA_START 0
#endif
#define TRANSMIT_DATA_MAGIC_NUM   0xF05A

typedef struct {
    uint16_t magic_num;
    uint16_t type;
    uint32_t offset;
} transmit_process_info;


#if (CONFIG_DFX_SUPPORT_FILE_SYSTEM == DFX_NO)
static errcode_t transmit_info_loc_find(uint32_t *addr)
{
    uint32_t value = 0;
    uint32_t j;
    for (j = TRANSMIT_OTA_INFO_START; j < TRANSMIT_OTA_INFO_END; j += (uint32_t)sizeof(transmit_process_info)) {
        dfx_flash_read(0, j, (uint8_t *)&value, sizeof(uint32_t));
        if (value == 0xFFFFFFFF) {
            *addr = j;
            return ERRCODE_SUCC;
        }
    }
    return ERRCODE_FAIL;
}

errcode_t transmit_record_progress(uint16_t transmit_type, uint32_t offset)
{
    errcode_t ret = ERRCODE_FAIL;
    int32_t write_size;
    uint32_t record_addr = 0;
    transmit_process_info info;
    info.magic_num = TRANSMIT_DATA_MAGIC_NUM;
    info.type = transmit_type;
    info.offset = offset;

    if (transmit_type == TRANSMIT_TYPE_SAVE_OTA_IMG) {
        ret = transmit_info_loc_find(&record_addr);
        if (ret == ERRCODE_SUCC) {
#ifdef USE_EMBED_FLADH
            write_size = dfx_flash_write(0, record_addr, (uint8_t *)&info, sizeof(transmit_process_info), false);
#else
            write_size = dfx_flash_info_write(0, record_addr, (uint8_t *)&info, sizeof(transmit_process_info), false);
#endif
            if (write_size == (int32_t)sizeof(transmit_process_info)) {
                return ERRCODE_SUCC;
            }
        }
    }
    return ERRCODE_FAIL;
}

errcode_t transmit_get_progress(uint16_t transmit_type, uint32_t *offset)
{
    errcode_t ret = ERRCODE_FAIL;
    uint32_t record_addr = 0;
    transmit_process_info info = {0};

    if (offset == NULL) {
        return ERRCODE_INVALID_PARAM;
    }

    if (transmit_type == TRANSMIT_TYPE_SAVE_OTA_IMG) {
        ret = transmit_info_loc_find(&record_addr);
        if (ret == ERRCODE_SUCC) {
            record_addr -= (uint32_t)sizeof(transmit_process_info);
            dfx_flash_read(0, record_addr, (uint8_t *)&info, sizeof(transmit_process_info));
            if (info.magic_num != TRANSMIT_DATA_MAGIC_NUM || info.type != TRANSMIT_TYPE_SAVE_OTA_IMG) {
                *offset = 0;
                return ERRCODE_FAIL;
            }
            *offset = info.offset;
            return ERRCODE_SUCC;
        }
    }
    return ERRCODE_FAIL;
}

errcode_t transmit_erase_progress(uint16_t transmit_type)
{
    if (transmit_type == TRANSMIT_TYPE_SAVE_OTA_IMG) {
        return dfx_flash_erase(0, TRANSMIT_OTA_INFO_START, TRANSMIT_OTA_INFO_SIZE);
    }
    return ERRCODE_FAIL;
}
#endif /* CONFIG_DFX_SUPPORT_FILE_SYSTEM */
#endif /* CONFIG_DFX_SUPPORT_CONTINUOUSLY_TRANSMIT */