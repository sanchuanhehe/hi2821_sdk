/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: UPG upgrade functions source file
 */

#include <stdint.h>
#include <stddef.h>
#include "common_def.h"
#include "upg_definitions.h"
#include "errcode.h"
#include "upg_lzmadec.h"
#include "upg_common.h"
#include "upg_common_porting.h"
#include "upg_alloc.h"
#include "partition.h"
#include "upg_porting.h"
#include "upg_patch.h"
#include "upg_config.h"
#include "upg_debug.h"

/* FOTA压缩升级 */
errcode_t uapi_upg_compress_image_update(const upg_image_header_t *image)
{
    uint32_t read_offset = image->image_offset;
    errcode_t ret;
    upg_lzma_decode2_data_t val_tmp = { 0 };
    /* header: 5 bytes of LZMA properties and 8 bytes of uncompressed size */
    uint8_t header[LZMA_PROPS_SIZE + 8]; /* 8: lzma解压算法固定长度 */
    uint32_t read_len = (uint32_t)sizeof(header);

    ret = upg_copy_pkg_image_data(image, 0, &read_len, header);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    read_offset += read_len;

    uint32_t app_address = 0;
    uint32_t app_size = 0;

    errcode_t ret_val = upg_get_partition_info(image->image_id, &app_address, &app_size);
    if (ret_val != ERRCODE_SUCC) {
        return ret_val;
    }

    CLzmaDec p;
    ret = upg_lzma_init(&p, &val_tmp, header, sizeof(header));
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    val_tmp.image_id = image->image_id;
    val_tmp.in_offset = read_offset;
    val_tmp.out_offset = 0;
    val_tmp.compress_len = image->image_len - (uint32_t)sizeof(header);

    if (app_size < val_tmp.decompress_len) {
        upg_msg2("app size is not enough! [app_size, decompress_len] :", app_size, val_tmp.decompress_len);
        upg_lzma_deinit(&p, &val_tmp);
        return ERRCODE_UPG_NO_ENOUGH_SPACE;
    }

    ret = upg_lzma_decode(&p, &val_tmp);
    if (ret != SZ_OK) {
        upg_msg1("upg_lzma_decode fail ret = ", ret);
        ret = ERRCODE_UPG_DECOMPRESS_FAIL;
    }

    upg_lzma_deinit(&p, &val_tmp);

    return ret;
}

/* FOTA差分升级 */
errcode_t uapi_upg_diff_image_update(const upg_image_header_t *image)
{
#if (UPG_CFG_DIFF_UPGRADE_SUPPORT == YES)
    errcode_t ret = ERRCODE_SUCC;
    ret = fota_pkg_task_apply_code_diff(image);

    return ret;
#else
    unused(image);
    return ERRCODE_UPG_NOT_SUPPORTED;
#endif
}

/* FOTA全镜像升级 */
errcode_t uapi_upg_full_image_update(const upg_image_header_t *image)
{
    uint32_t read_offset = image->image_offset;
    errcode_t ret;
    uint32_t write_offset = 0;
    uint32_t app_address = 0;
    uint32_t app_size = 0;
    errcode_t ret_val = upg_get_partition_info(image->image_id, &app_address, &app_size);
    if (ret_val != ERRCODE_SUCC) {
        return ret_val;
    }

    if (app_size < image->image_len) {
        return ERRCODE_FAIL;
    }

    uint32_t read_len = 0;
    uint32_t tmp_len = 0;

    uint8_t *buffer = upg_malloc(UPG_FLASH_PAGE_SIZE);
    if (buffer == NULL) {
        return ERRCODE_MALLOC;
    }

    while (read_len < image->image_len) {
        tmp_len = ((image->image_len - read_len) > UPG_FLASH_PAGE_SIZE) ?
            UPG_FLASH_PAGE_SIZE : (image->image_len - read_len);
        ret = upg_read_fota_pkg_data(read_offset + read_len, (uint8_t *)buffer, &tmp_len);
        if (ret != ERRCODE_SUCC) {
            upg_free(buffer);
            return ret;
        }

        ret = upg_write_new_image_data(write_offset + read_len, (uint8_t *)buffer, &tmp_len, image->image_id);
        if (ret != ERRCODE_SUCC || tmp_len == 0) {
            upg_free(buffer);
            return ret;
        }

        read_len += tmp_len;

        // 踢狗
        upg_watchdog_kick();

        upg_calculate_and_notify_process(tmp_len);
    }

    upg_free(buffer);
    buffer = NULL;
    return ERRCODE_SUCC;
}
