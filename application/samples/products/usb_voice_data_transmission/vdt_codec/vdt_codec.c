/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Microphone Codec Source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-08-21, Create file. \n
 */
#include "common_def.h"
#include "osal_debug.h"
#include "audio_enc_codec.h"
#include "audio_dec_codec.h"
#include "audio_profile_calc.h"
#include "vdt_codec.h"

#define PDM_SAMPLE_RATE_8K 8000
#define PDM_SAMPLE_RATE_16K 16000
#define PDM_SAMPLE_RATE_32K 32000
#define PDM_SAMPLE_RATE_48K 48000
#define PDM_SAMPLE_RATE_96K 96000

#define ENC_FREAM_16K_SBC_SIZE 256
#define ENC_FREAM_16K_MSBC_SIZE 240
#define ENC_FREAM_16K_OPUS_SIZE 320
#define ENC_FREAM_16K_L2HC_SIZE 320

#define ENC_FREAM_48K_SBC_SIZE 256
#define ENC_FREAM_48K_MSBC_SIZE 240
#define ENC_FREAM_48K_OPUS_SIZE 960
#define ENC_FREAM_48K_L2HC_SIZE 960

#define DEC_FREAM_16K_SBC_SIZE 68
#define DEC_FREAM_16K_MSBC_SIZE 60
#define DEC_FREAM_16K_OPUS_SIZE 100
#define DEC_FREAM_16K_L2HC_SIZE 84

#define DEC_FREAM_48K_SBC_SIZE 68
#define DEC_FREAM_48K_MSBC_SIZE 60
#define DEC_FREAM_48K_OPUS_SIZE 120
#define DEC_FREAM_48K_L2HC_SIZE 84

typedef struct vdt_codec_inst {
    uint32_t codec_id;
    uint32_t sample_rate;
    uint32_t bit_depth;
    uint32_t channels;
} vdt_codec_inst_t;

static vdt_codec_inst_t g_vdt_codec_inst = {
#if defined(CONFIG_CODEC_ENABLE_SBC)
    .codec_id = 0,
#elif defined(CONFIG_CODEC_ENABLE_MSBC)
    .codec_id = 1,
#elif defined(CONFIG_CODEC_ENABLE_OPUS)
    .codec_id = 2,
#elif defined(CONFIG_CODEC_ENABLE_L2HC)
    .codec_id = 3,
#endif

#if defined(CONFIG_USB_PDM_SAMPLE_RATE_8K)
    .sample_rate = 8000,
#elif defined(CONFIG_USB_PDM_SAMPLE_RATE_16K)
    .sample_rate = 16000,
#elif defined(CONFIG_USB_PDM_SAMPLE_RATE_32K)
    .sample_rate = 32000,
#elif defined(CONFIG_USB_PDM_SAMPLE_RATE_48K)
    .sample_rate = 48000,
#elif defined(CONFIG_USB_PDM_SAMPLE_RATE_96K)
    .sample_rate = 96000,
#endif

    .bit_depth = (uint32_t)0x10,
    .channels = (uint32_t)1
};

static uint32_t g_encoder_indata_size[2][4] = {
    {ENC_FREAM_16K_SBC_SIZE, ENC_FREAM_16K_MSBC_SIZE, ENC_FREAM_16K_OPUS_SIZE, ENC_FREAM_16K_L2HC_SIZE},
    {ENC_FREAM_48K_SBC_SIZE, ENC_FREAM_48K_MSBC_SIZE, ENC_FREAM_48K_OPUS_SIZE, ENC_FREAM_48K_L2HC_SIZE} /* msbc不支持 */
}; /* 与codec_id_iput一一对应 */

static uint32_t g_decoder_indata_size[2][4] = {
    {DEC_FREAM_16K_SBC_SIZE, DEC_FREAM_16K_MSBC_SIZE, DEC_FREAM_16K_OPUS_SIZE, DEC_FREAM_16K_L2HC_SIZE},
    {DEC_FREAM_48K_SBC_SIZE, DEC_FREAM_48K_MSBC_SIZE, DEC_FREAM_48K_OPUS_SIZE, DEC_FREAM_48K_L2HC_SIZE} /* msbc不支持 */
}; /* 与codec_id_iput一一对应 */

static void vdt_codec_set_enc_data_size(vdt_codec_inst_t *codec_inst)
{
    uint32_t base_size;
    uint32_t data_size;
    base_size = g_encoder_indata_size[codec_inst->sample_rate == HA_CODEC_SAMPLE_RATE_48K][codec_inst->codec_id];
    data_size = (base_size * codec_inst->channels);
#ifdef CODEC_5MS
    if (codec_inst->codec_id == HA_CODEC_ID_OPUS) {
        data_size = (base_size * codec_inst->channels) >> 1;
    }
#endif
    audio_set_encoder_consume_data_size(data_size);
}

static void vdt_codec_set_dec_data_size(vdt_codec_inst_t *codec_inst)
{
    uint32_t base_size;
    uint32_t data_size;
    base_size = g_decoder_indata_size[codec_inst->sample_rate == HA_CODEC_SAMPLE_RATE_48K][codec_inst->codec_id];
    data_size = (base_size * codec_inst->channels);
    audio_set_decoder_consume_data_size(data_size);
}

void vdt_codec_init(void)
{
    int32_t ret;

    vdt_codec_set_enc_data_size(&g_vdt_codec_inst);
    vdt_codec_set_dec_data_size(&g_vdt_codec_inst);

    ha_codec_enc_param enc_open_param;
    enc_open_param.channels = g_vdt_codec_inst.channels;
    enc_open_param.bit_depth = g_vdt_codec_inst.bit_depth;
    enc_open_param.sample_rate = g_vdt_codec_inst.sample_rate;
    enc_open_param.private_data = NULL;
    enc_open_param.private_data_size = 0;
    ret = aenc_open_codec(g_vdt_codec_inst.codec_id, &enc_open_param);
    if (ret != AUDIO_SUCCESS) {
        osal_printk("open encoder codec fail %#x\n", ret);
        aenc_close_codec();
        return;
    }
    osal_printk("Open encoder codec success.\r\n");

    ha_codec_dec_param dec_open_param;
    dec_open_param.channels = g_vdt_codec_inst.channels;
    dec_open_param.bit_depth = g_vdt_codec_inst.bit_depth;
    dec_open_param.sample_rate = g_vdt_codec_inst.sample_rate;
    dec_open_param.private_data = NULL;
    dec_open_param.private_data_size = 0;
    ret = adec_open_codec(g_vdt_codec_inst.codec_id, &dec_open_param);
    if (ret != AUDIO_SUCCESS) {
        osal_printk("open decoder codec fail %#x\n", ret);
        aenc_close_codec();
        adec_close_codec();
        return;
    }
    osal_printk("Open decoder codec success.\r\n");
}

void vdt_codec_deinit(void)
{
    aenc_close_codec();
    adec_close_codec();
}

uint32_t vdt_codec_encode(uint8_t *pcm_data, uint8_t **enc_data)
{
    int32_t ret;
    ring_buffer_t *enc_in_fifo = audio_get_in_fifo(HA_CODEC_ENDODER);
    ring_buffer_t *enc_out_fifo = audio_get_out_fifo(HA_CODEC_ENDODER);

    /* audio codec begin */
    ret = write_data_into_fifo(pcm_data, enc_in_fifo, audio_get_encoder_consume_data_size());
    if (ret != AUDIO_SUCCESS) {
        osal_printk("write data err %d\n", ret);
        return 0;
    }

    ret = aenc_encode_frame();
    if (ret != 0) {
        osal_printk("aenc_encode_frame %d\n", ret);
        return 0;
    }

    uint32_t enc_product_data_size = audio_get_encoder_product_data_size();
    ret = read_data_from_fifo(enc_data, enc_out_fifo, enc_product_data_size);
    if (ret != AUDIO_SUCCESS) {
        return 0;
    }

    read_data_from_fifo_finish(enc_out_fifo, enc_product_data_size);

    return enc_product_data_size;
}

uint32_t vdt_codec_decode(uint8_t *enc_data, uint8_t **pcm_data)
{
    int32_t ret;
    ring_buffer_t *dec_in_fifo = audio_get_in_fifo(HA_CODEC_DECODER);
    ring_buffer_t *dec_out_fifo = audio_get_out_fifo(HA_CODEC_DECODER);

    write_data_into_fifo(enc_data, dec_in_fifo, audio_get_encoder_product_data_size());

    ret = adec_decode_frame(0);
    if (ret != 0) {
        osal_printk("adec_decode_frame %d\n", ret);
        return 0;
    }

    uint32_t product_data_size = audio_get_decoder_product_data_size();
    ret = read_data_from_fifo(pcm_data, dec_out_fifo, product_data_size);
    if (ret != AUDIO_SUCCESS) {
        return 0;
    }
    read_data_from_fifo_finish(dec_out_fifo, product_data_size);

    return product_data_size;
}