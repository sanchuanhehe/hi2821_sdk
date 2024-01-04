/*
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: zdiag service
 */

#include "diag_service.h"
#include "diag_pkt_router.h"
#include "errcode.h"
#include "stdbool.h"
#include "securec.h"
#include "dfx_adapt_layer.h"
#ifdef CONFIG_DIAG_WITH_SECURE
#include "diag_secure.h"
#endif

#define DIAG_SER_DATA_MERGE_MAX    0x800     // fix 2048
#define DIAG_SER_DATA_MFS_DEFAULT  0x400

typedef enum {
    DIAG_DATA_NO_MERGE,
    DIAG_DATA_MERGED,
    DIAG_DATA_MERGING,
} diag_merge_data_status;

typedef struct {
    uint8_t module_id;
    uint8_t cmd_id;
    uint16_t offset;
    uint8_t *buff;
} diag_merge_data_t;

static diag_merge_data_t g_merge_data = {0};
static diag_merge_data_status g_merge_status = DIAG_DATA_NO_MERGE;

static uint16_t g_diag_ser_mfs = DIAG_SER_DATA_MFS_DEFAULT;

diag_notify_f g_diag_ser_func[DIAG_SER_MAX] = {NULL};

static errcode_t diag_service_data_merge(diag_router_frame_t *data, uint16_t size, uint8_t extra_len)
{
    uint8_t *src_buf = NULL;
    uint8_t *dst_buf = NULL;
    uint8_t sn = get_frame_ctrl_sn(data->ctrl);
    if (sn == DIAG_FRAME_SN_START) {
        dfx_log_debug("diag_service_data_merge: SN start\r\n");
        diag_ser_frame_t *frame = (diag_ser_frame_t *)((uint8_t *)data + DIAG_ROUTER_HEADER_LEN + extra_len);
        if (g_merge_data.buff == NULL) {
            g_merge_data.buff = dfx_malloc(0, DIAG_SER_DATA_MERGE_MAX);
            if (g_merge_data.buff == NULL) {
                return ERRCODE_MALLOC;
            }
        }

        src_buf = (uint8_t *)data + DIAG_ROUTER_HEADER_LEN + extra_len;
        dst_buf = g_merge_data.buff + sizeof(diag_ser_data_t);
        if (memcpy_s(dst_buf, size - extra_len, src_buf, size - extra_len) != EOK) {
        }

        g_merge_data.module_id = frame->module_id;
        g_merge_data.cmd_id = frame->cmd_id;
        g_merge_data.offset = (uint16_t)sizeof(diag_ser_data_t) + size - extra_len;
        g_merge_status = DIAG_DATA_MERGING;
    } else if (sn == DIAG_FRAME_SN_INSIDE) {
        dfx_log_debug("diag_service_data_merge: SN inside\r\n");
        if (g_merge_data.buff == NULL) {
            return ERRCODE_MALLOC;
        }
        src_buf = (uint8_t *)data + DIAG_ROUTER_HEADER_LEN + extra_len;
        dst_buf = g_merge_data.buff + g_merge_data.offset;
        if (memcpy_s(dst_buf, size - extra_len, src_buf, size - extra_len) != EOK) {
        }
        g_merge_data.offset += size - extra_len;
        g_merge_status = DIAG_DATA_MERGING;
    } else if (sn == DIAG_FRAME_SN_END) {
        dfx_log_debug("diag_service_data_merge: SN end\r\n");
        if (g_merge_data.buff == NULL) {
            return ERRCODE_MALLOC;
        }
        src_buf = (uint8_t *)data + DIAG_ROUTER_HEADER_LEN + extra_len;
        dst_buf = g_merge_data.buff + g_merge_data.offset;
        if (memcpy_s(dst_buf, size - extra_len, src_buf, size - extra_len) != EOK) {
        }
        g_merge_data.offset += size - extra_len;
        g_merge_status = DIAG_DATA_MERGED;
    }
    return ERRCODE_SUCC;
}

#ifdef CONFIG_DIAG_WITH_SECURE
static errcode_t diag_make_frame_secure(diag_ser_frame_t **frame, diag_router_frame_t *data, uint16_t size,
                                        uint8_t *extra_len)
{
    if (get_frame_ctrl_secure_flag(data->ctrl) != 0) {
        diag_secure_ctx_t *secure_ctx = diag_get_secure_ctx();
        uint8_t key_check[AES128_KEY_LEN];
        memset_s(key_check, AES128_KEY_LEN, 0, AES128_KEY_LEN);
        if (memcmp(secure_ctx->srp_info.key, key_check, AES128_KEY_LEN) != 0) {
            return ERRCODE_FAIL;
        }
        uint8_t *iv = (uint8_t *)((uint8_t *)data + DIAG_ROUTER_HEADER_LEN + *extra_len);
        uint8_t *tag = (uint8_t *)((uint8_t *)data + DIAG_ROUTER_HEADER_LEN + *extra_len + AES_IV_LEN);
        *extra_len += DIAG_ROUTER_SECURE_LEN;
        *frame = (diag_ser_frame_t *)((uint8_t *)data + DIAG_ROUTER_HEADER_LEN + *extra_len);
        uint16_t decrypt_size = size - *extra_len;
        errcode_t ret = diag_aes_gcm_decrypt_inplace(*frame, decrypt_size, iv, tag);
        if (ret != ERRCODE_SUCC) {
            return ret;
        }
    } else {
        *frame = (diag_ser_frame_t *)((uint8_t *)data + DIAG_ROUTER_HEADER_LEN + *extra_len);
        if (diag_need_secure(makeu16((*frame)->cmd_id, (*frame)->module_id))) {
            return ERRCODE_FAIL;
        }
    }
    return ERRCODE_SUCC;
}
#endif

static errcode_t diag_service_frame_proc(diag_ser_frame_t *frame, diag_router_frame_t *data,
                                         uint16_t size, uint8_t extra_len)
{
    errcode_t ret = ERRCODE_FAIL;
    uint8_t *notify_buffer = NULL;
    diag_ser_data_t *notify_data = NULL;
    uint8_t module_id;
    uint8_t cmd_id;
    uint16_t data_len;

    if (g_merge_status == DIAG_DATA_MERGED) {
        notify_data = (diag_ser_data_t *)g_merge_data.buff;
        module_id = g_merge_data.module_id;
        cmd_id = g_merge_data.cmd_id;
        data_len = g_merge_data.offset - (uint16_t)sizeof(diag_ser_data_t);
    } else {
        data_len = size - extra_len;

        notify_buffer = dfx_malloc(0, data_len + (uint16_t)sizeof(diag_ser_header_t));
        if (notify_buffer == NULL) {
            return ERRCODE_MALLOC;
        }

        notify_data = (diag_ser_data_t *)notify_buffer;
        memcpy_s((uint8_t *)notify_data->payload, data_len, frame, data_len);
        module_id = frame->module_id;
        cmd_id = frame->cmd_id;
    }

    notify_data->header.ser_id  = module_id;
    notify_data->header.cmd_id  = cmd_id;
    notify_data->header.src     = data->len_lsb;
    notify_data->header.dst     = data->len_msb;
    notify_data->header.crc_en  = 0;
    notify_data->header.ack_en  = 0;
    notify_data->header.length  = data_len;

    if (module_id < DIAG_SER_MAX) {
        if (g_diag_ser_func[module_id]) {
            ret = g_diag_ser_func[module_id](notify_data);
        }
    }
    if (notify_buffer != NULL) {
        dfx_free(0, notify_buffer);
    }

    if (g_merge_data.buff != NULL) {
        dfx_free(0, g_merge_data.buff);
        g_merge_data.buff = NULL;
    }
    g_merge_status = DIAG_DATA_NO_MERGE;
    return ret;
}

static errcode_t diag_service_data_proc(diag_router_frame_t *data, uint16_t size)
{
    errcode_t ret = ERRCODE_FAIL;
    uint8_t extra_len = 0;

    if (data == NULL || size == 0) {
        return ERRCODE_INVALID_PARAM;
    }
    // fid en
    if (get_frame_ctrl_fid_en(data->ctrl) != 0) {
        extra_len++;
    }

    // echo en
    if (get_frame_ctrl_ack_type(data->ctrl) == FRAME_ACK_TYPE_ECHO) {
        extra_len++;
    }

    // 是否是分包数据
    if (get_frame_ctrl_sn(data->ctrl) != 0) {
        extra_len++;
        ret = diag_service_data_merge(data, size, extra_len);
        if (g_merge_status == DIAG_DATA_MERGING) {
            return ret;
        }
    }

#ifdef CONFIG_DIAG_WITH_SECURE
    diag_ser_frame_t *frame = NULL;
    ret = diag_make_frame_secure(&frame, data, size, &extra_len);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
#else
    diag_ser_frame_t *frame = (diag_ser_frame_t *)((uint8_t *)data + DIAG_ROUTER_HEADER_LEN + extra_len);
#endif

    return diag_service_frame_proc(frame, data, size, extra_len);
}

errcode_t uapi_diag_service_register(diag_ser_id_t module_id, diag_notify_f func)
{
    if (module_id >= DIAG_SER_MAX) {
        return ERRCODE_INVALID_PARAM;
    }
    g_diag_ser_func[module_id] = func;
    return ERRCODE_SUCC;
}

errcode_t uapi_diag_service_send_data(diag_ser_data_t *data)
{
    if (data == NULL) {
        return ERRCODE_INVALID_PARAM;
    }

    diag_router_data_t router_data = {0};
    diag_ser_header_t *header = &data->header;

    bool fid_en = false;
    bool sn_en = false;
    if ((header->dst != 0) || (header->src != 0)) {
        fid_en = true;
    }

    if (header->length > g_diag_ser_mfs) {
        sn_en = true;
    }

    router_data.ctrl.en_crc   = header->crc_en;
    router_data.ctrl.ack_type = header->ack_en;
    router_data.ctrl.en_fid   = fid_en;
    router_data.ctrl.en_sn    = sn_en;
    router_data.ctrl.en_eof   = 0;

    router_data.fid         = (uint8_t)(header->dst << DIAG_ROUTER_FID_DST_BIT | header->src);
    router_data.sn_count    = 0;
    router_data.echo        = 0;
    router_data.mfs         = g_diag_ser_mfs;

    router_data.data        = data->payload;
    router_data.data_len    = header->length;
    return diag_pkt_router_send(&router_data);
}

void uapi_diag_service_init(void)
{
    diag_router_register_notify(diag_service_data_proc);
}

void uapi_diag_service_set_mfs(uint16_t mfs)
{
    g_diag_ser_mfs = mfs;
}