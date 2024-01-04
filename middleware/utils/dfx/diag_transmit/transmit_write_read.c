/*
 * Copyright (c) @CompanyNameMagicTag 2021-2023. All rights reserved.
 * Description: transmit
 * This file should be changed only infrequently and with great care.
 */
#include "transmit_write_read.h"
#include "securec.h"
#include "transmit_file_operation.h"
#include "transmit_debug.h"
#include "dfx_adapt_layer.h"

int32_t bus_read_data(uintptr_t usr_data, uint32_t offset, uint8_t *buf, uint32_t len)
{
    dfx_assert(buf);
    transmit_item_t *item = (transmit_item_t *)usr_data;
    dfx_log_debug("[WR][bus_read_data][bus_addr=0x%x][offset=0x%x][len=0x%x]\r\n", item->bus_addr, offset, len);
    memcpy_s((void *)(uintptr_t)buf, len, (void *)(uintptr_t)(item->bus_addr + offset), len);
    return (int32_t)len;
}

int32_t buf_write_data(uintptr_t usr_data, uint32_t offset, uint8_t *buf, uint32_t len)
{
    dfx_assert(buf);
    transmit_item_t *item = (transmit_item_t *)usr_data;
    dfx_log_debug("[WR][buf_write_data][bus_addr=0x%x][offset=0x%x][len=0x%x]\r\n", item->bus_addr, offset, len);
    memcpy_s((void *)(uintptr_t)(item->bus_addr + offset), len, (void *)(uintptr_t)buf, len);
    return (int32_t)len;
}

int32_t file_read_data(uintptr_t usr_data, uint32_t offset, uint8_t *buf, uint32_t len)
{
    dfx_assert(buf);
    uint32_t read_offset = offset;
    transmit_item_t *item = (transmit_item_t *)usr_data;
    dfx_log_debug("[WR][file_read_data][file_name=%s][offset=0x%x][len=0x%x]\r\n", item->file_name, offset, len);
#if (CONFIG_DFX_SUPPORT_FILE_SYSTEM == DFX_YES)
    return transmit_file_read_fd(item->file_fd, offset, buf, len, false);
#else
    uint8_t opt_type;
    switch (item->remote_type) {
        case TRANSMIT_TYPE_SAVE_OTA_IMG:
            opt_type = FLASH_OP_TYPE_OTA;
            break;
        case TRANSMIT_TYPE_READ_FLASH:
            opt_type = FLASH_OP_TYPE_FLASH_DATA;
            read_offset += item->bus_addr;
            break;
        default:
            return -1;
    }
    return dfx_flash_read(opt_type, read_offset, buf, len);
#endif
}

int32_t file_write_data(uintptr_t usr_data, uint32_t offset, uint8_t *buf, uint32_t len)
{
    dfx_assert(buf);
    transmit_item_t *item = (transmit_item_t *)usr_data;
    dfx_log_debug("[WR][file_write_data][file_name=%s][offset=0x%x][len=0x%x]\r\n", item->file_name, offset, len);
#if (CONFIG_DFX_SUPPORT_FILE_SYSTEM == DFX_YES)
    return transmit_file_write_fd(item->file_fd, offset, buf, len);
#else
    unused(item);
    uint8_t opt_type;
    switch (item->remote_type) {
        case TRANSMIT_TYPE_SAVE_OTA_IMG:
            opt_type = FLASH_OP_TYPE_OTA;
            break;
        default:
            return -1;
    }
    return dfx_flash_write(opt_type, offset, buf, len, false);
#endif
}
