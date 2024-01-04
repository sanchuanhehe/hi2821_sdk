/*
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: NV on different flash
 */

#include "nv_porting.h"
#include "sfc.h"
#include "soc_osal.h"
#include "std_def.h"
#include "uapi_crc.h"
#include "memory_config_common.h"

#define FLASH_WORD_ALIGN 4

errcode_t kv_flash_read(const uint32_t flash_offset, const uint32_t size, uint8_t *ram_data)
{
    errcode_t ret = ERRCODE_SUCC;
    uint32_t mstatus = osal_irq_lock();
    if (uapi_sfc_reg_read(flash_offset, ram_data, size) != ERRCODE_SUCC) {
        ret = ERRCODE_FAIL;
    }
    osal_irq_restore(mstatus);
    return ret;
}

errcode_t kv_flash_write(const uint32_t flash_offset, uint32_t size, const uint8_t *ram_data, bool do_erase)
{
    (void)do_erase;
    errcode_t ret = ERRCODE_SUCC;
    uint32_t mstatus = osal_irq_lock();
    if (uapi_sfc_reg_write(flash_offset, (uint8_t *)ram_data, size) != ERRCODE_SUCC) {
        ret = ERRCODE_FAIL;
    }
    osal_irq_restore(mstatus);
    return ret;
}

errcode_t kv_flash_erase(const uint32_t flash_offset, uint32_t size)
{
    UNUSED(size);
    errcode_t ret = ERRCODE_SUCC;
    uint32_t mstatus = osal_irq_lock();
    if (uapi_sfc_reg_erase(flash_offset, FLASH_PAGE_SIZE) != ERRCODE_SUCC) {
        ret = ERRCODE_FAIL;
    }
    osal_irq_restore(mstatus);
    return ret;
}