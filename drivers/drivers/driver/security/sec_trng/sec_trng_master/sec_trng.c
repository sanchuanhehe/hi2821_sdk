/*
 * Copyright (c) @CompanyNameMagicTag 2020-2022. All rights reserved.
 * Description: SEC TRNG2
 * Author: @CompanyNameTag
 * Create: 2020-01-20
 */

#include "sec_trng.h"
#include "soc_osal.h"
#include "securec.h"
#include "core.h"
#if (CORE_NUMS > 1) && (!defined BUILD_APPLICATION_SSB)
#include "ipc.h"
#endif
#include "oal_interface.h"
#include "systick.h"
#include "pal_sec_config_port.h"

#define SEC_TRNG2_DEFAULT_CFG          0x10010     // trng2_cfg_para choose threshold ,ref value is : 0x10010
#define SEC_TRNG2_DATA_BLACKS          0xfff       // data blocks is no more than 4095, ref value is : 0xfff
#define SEC_TRNG2_DATA_HALF_LENGTH     0x100       // median of trng shared memory
#define SEC_TRNG2_TIMEOUT_MS           3ULL

#ifdef USE_CMSIS_OS
#define SEC_TRNG2_DATA_HALF_START      (TRNG_DATA_REGION_START + SEC_TRNG2_DATA_HALF_LENGTH)
#endif

static uint32_t g_sec_trng_err_cnt = 0;

void sec_trng2_init(void)
{
    if (sec_common_driver_initialised_get(SEC_TRNG2)) {
        return;
    }

    hal_sec_trng2_enable();
    sec_common_driver_initalised_set(SEC_TRNG2, true);
}

void sec_trng2_deinit(void)
{
    if (!sec_common_driver_initialised_get(SEC_TRNG2)) {
        return;
    }
    sec_trng2_unregister_callback();
    hal_sec_trng2_disable();
    sec_common_driver_initalised_set(SEC_TRNG2, false);
}

void sec_trng2_start(trng2_cfg_reg_t trng2_cfg_para, uint32_t data_blocks)
{
    hal_sec_trng2_start(trng2_cfg_para, data_blocks);
}

trng2_output_data_t sec_trng2_get_result(void)
{
    uint64_t start_time = uapi_systick_get_ms();
    while (!hal_sec_trng2_is_ready()) {
        if ((start_time + SEC_TRNG2_TIMEOUT_MS) < uapi_systick_get_ms()) {
            g_sec_trng_err_cnt++;
            break;
        }
    }
    return hal_sec_trng2_output();
}

bool sec_trng2_register_callback(SEC_CALLBACK callback)
{
    uint32_t irq_sts = osal_irq_lock();
    hal_sec_comm_intr_clear(SEC_TRNG2);
#if TRNG_WITH_SEC_COMMON == YES
    if (hal_sec_comm_register_callback(callback, SEC_TRNG2) == false) {
        osal_irq_restore(irq_sts);
        return false;
    }
    sec_comm_enable_irq(SEC_TRNG2);
#else
    oal_int_create(SEC_INT_NUMBER, SEC_INT_PRI, (oal_int_func)callback, 0);
#endif
    hal_sec_comm_enable_intr(SEC_TRNG2);
    osal_irq_restore(irq_sts);
    return true;
}

void sec_trng2_unregister_callback(void)
{
    uint32_t irq_sts = osal_irq_lock();
    sec_comm_disable_irq(SEC_TRNG2);
    hal_sec_comm_disable_intr(SEC_TRNG2);
    hal_sec_comm_intr_clear(SEC_TRNG2);
    hal_sec_comm_unregister_callback(SEC_TRNG2);
    osal_irq_restore(irq_sts);
}

static trng2_output_data_t sec_trng2_get_trng(void)
{
    trng2_output_data_t trng2_output;
    trng2_cfg_reg_t trng2_cfg_values;
    trng2_cfg_values.trng2_cfg = SEC_TRNG2_DEFAULT_CFG;
    uint32_t data_blockes = SEC_TRNG2_DATA_BLACKS;
    sec_trng2_start(trng2_cfg_values, data_blockes);
    trng2_output = sec_trng2_get_result();
    hal_sec_trng2_clear_ready_ack();
    return trng2_output;
}

sec_trng_ret_t sec_trng_random_get(uint8_t *trng_buffer, uint32_t trng_buffer_length)
{
    uint8_t* buffer = trng_buffer;
    size_t len = (size_t)trng_buffer_length;
    trng2_output_data_t trng2_output;
    if ((buffer == NULL) || (len == 0)) {
        return SEC_TRNG_RET_ERROR;
    }
    sec_trng2_init();
    while (len > sizeof(trng2_output_data_t)) {
        trng2_output = sec_trng2_get_trng();
        if (memcpy_s(buffer, len, &trng2_output, sizeof(trng2_output_data_t)) != EOK) {
            sec_trng2_deinit();
            return SEC_TRNG_RET_ERROR;
        }
        buffer += sizeof(trng2_output_data_t);
        len -= sizeof(trng2_output_data_t);
    }
    trng2_output = sec_trng2_get_trng();
    sec_trng2_deinit();
    if (memcpy_s(buffer, len, &trng2_output, len) != EOK) {
        return SEC_TRNG_RET_ERROR;
    }
    return SEC_TRNG_RET_OK;
}

void sec_trng2_random_buffer_init(sec_trng2_rand_buf_updata_t updata)
{
#ifdef USE_CMSIS_OS
    uint32_t trng_data_start;
    uint32_t trng_data_end;
    uint32_t buffer_length;
    trng2_output_data_t trng2_output = {0};
    switch (updata) {
        case SEC_TRNG2_RAND_BUF_UPDATA_ALL:
            trng_data_start = TRNG_DATA_REGION_START;
            trng_data_end = TRNG_DATA_REGION_START + TRNG_DATA_REGION_LENGTH;
            buffer_length = TRNG_DATA_REGION_LENGTH;
            break;
        case SEC_TRNG2_RAND_BUF_UPDATA_UP_SIDE:
            trng_data_start = TRNG_DATA_REGION_START;
            trng_data_end = SEC_TRNG2_DATA_HALF_START;
            buffer_length = SEC_TRNG2_DATA_HALF_LENGTH;
            break;
        case SEC_TRNG2_RAND_BUF_UPDATA_DOWN_SIDE:
            trng_data_start = SEC_TRNG2_DATA_HALF_START;
            trng_data_end = TRNG_DATA_REGION_START + TRNG_DATA_REGION_LENGTH;
            buffer_length = SEC_TRNG2_DATA_HALF_LENGTH;
            break;
        default:
            return;
    }
    memset_s((void *)(uintptr_t)trng_data_start, buffer_length, 0, buffer_length);
    while ((trng_data_end - trng_data_start) >= sizeof(trng2_output)) {
        trng2_output = sec_trng2_get_trng();
        if (memcpy_s((void *)(uintptr_t)trng_data_start, trng_data_end - trng_data_start, &trng2_output,
                     sizeof(trng2_output)) != EOK) {
            return;
        }
        trng_data_start += sizeof(trng2_output);
    }
    if (memcpy_s((void *)(uintptr_t)trng_data_start, trng_data_end - trng_data_start, &trng2_output,
                 trng_data_end - trng_data_start) != EOK) {
        return;
    }
#else
    UNUSED(updata);
#endif
}

#if (CORE_NUMS > 1) && defined IPC_NEW
#elif (CORE_NUMS > 1) && (!defined BUILD_APPLICATION_SSB)
static bool sec_trng2_updata_handler(ipc_action_t message,
                                     const volatile ipc_payload *payload_p, cores_t src, uint32_t id)
{
    if (message != IPC_ACTION_UPDATA_TRNG) {
        return true;
    }
    UNUSED(message);
    UNUSED(id);
    UNUSED(src);
    sec_trng2_init();
    switch (payload_p->updata_trng_signal.updata) {
        case SEC_TRNG_RAND_BUF_UPDATA_ALL:
            sec_trng2_random_buffer_init(SEC_TRNG2_RAND_BUF_UPDATA_ALL);
            break;
        case SEC_TRNG_RAND_BUF_UPDATA_UP_SIDE:
            sec_trng2_random_buffer_init(SEC_TRNG2_RAND_BUF_UPDATA_UP_SIDE);
            break;
        case SEC_TRNG_RAND_BUF_UPDATA_DOWN_SIDE:
            sec_trng2_random_buffer_init(SEC_TRNG2_RAND_BUF_UPDATA_DOWN_SIDE);
            break;
        default:
            sec_trng2_deinit();
            return false;
    }
    sec_trng2_deinit();
    return true;
}

void sec_trng2_updata_reigister(void)
{
    (void)ipc_register_handler(IPC_ACTION_UPDATA_TRNG, sec_trng2_updata_handler);
}
#endif
