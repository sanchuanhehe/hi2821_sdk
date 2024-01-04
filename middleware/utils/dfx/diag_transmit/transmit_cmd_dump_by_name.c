/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: dump by name
 * This file should be changed only infrequently and with great care.
 */
#include "transmit_cmd_dump_by_name.h"
#include "diag.h"
#include "transmit_cmd_dump_by_name_st.h"
#include "transmit_file_operation.h"
#include "transmit_debug.h"
#include "securec.h"
#include "soc_log.h"
#include "errcode.h"
#if CONFIG_DFX_SUPPORT_TRANSMIT_FILE == DFX_YES
#define DUMP_MAX_SIZE_PER_TIME 0x400

STATIC errcode_t zdiag_cmd_dump_one_item(uint16_t cmd_id, diag_option_t *option, const char *name,
    diag_dump_item_t *item, diag_dump_by_name_ind_t *ind)
{
    uint32_t read_size;
    int32_t readed_size;
    int32_t fd;
    bool burst = false;
    fd = transmit_file_open_for_read(name);
    if (fd < 0) {
        dfx_log_err("[ERR]open file failed\r\n");
        return ERRCODE_FAIL;
    }

    while (item->size != 0) {
        read_size = (item->size > DUMP_MAX_SIZE_PER_TIME) ? DUMP_MAX_SIZE_PER_TIME : item->size;
        readed_size = transmit_file_read_fd(fd, item->offset, ind->data, read_size, burst);
        burst = true;
        if (readed_size <= 0) {
            dfx_log_err("[ERR]read file failed, readed_size = %d\r\n", readed_size);
            transmit_file_close(fd);
            return ERRCODE_FAIL;
        }

        ind->ret = ERRCODE_SUCC;
        ind->offset = item->offset;
        ind->size = (uint32_t)readed_size;

        item->offset += (uint32_t)readed_size;
        item->size -= (uint32_t)readed_size;

        uapi_diag_report_packet(cmd_id, option, (uint8_t *)ind, sizeof(diag_dump_by_name_ind_t) + readed_size,
            true);
        if (readed_size != (int32_t)read_size) {
            dfx_log_err("[ERR]readed != read, readed_size = %d, read_size = %u\r\n", readed_size, read_size);
            break;
        }
    }
    transmit_file_close(fd);
    return ERRCODE_SUCC;
}

errcode_t transmit_cmd_dump_by_file_name(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size,
                                         diag_option_t *option)
{
    unused(cmd_param_size);

    uint32_t i;
    diag_dump_by_name_cmd_t *req = cmd_param;
    diag_dump_by_name_ind_t *ind = dfx_malloc(0, sizeof(diag_dump_by_name_ind_t) + DUMP_MAX_SIZE_PER_TIME);

    if (ind == NULL) {
        dfx_log_err("[ERR]memory application failed\r\n");
        return ERRCODE_FAIL;
    }

    for (i = 0; i < req->cnt; i++) {
        zdiag_cmd_dump_one_item(cmd_id, option, req->name, &req->item[i], ind);
    }

    dfx_free(0, ind);
    return ERRCODE_SUCC;
}
#endif