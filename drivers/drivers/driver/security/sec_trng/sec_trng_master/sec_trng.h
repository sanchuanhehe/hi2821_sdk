/*
 * Copyright (c) @CompanyNameMagicTag 2020-2020. All rights reserved.
 * Description: SEC TRNG
 * Author: @CompanyNameTag
 * Create: 2020-01-20
 */

#ifndef NON_OS_SEC_TRNG_H
#define NON_OS_SEC_TRNG_H

#include "sec_common.h"
#include "hal_sec_trng2.h"

/**
 * @addtogroup connectivity_drivers_non_os_sec_trng
 * @{
 */
typedef enum {
    SEC_TRNG2_RAND_BUF_UPDATA_ALL,
    SEC_TRNG2_RAND_BUF_UPDATA_UP_SIDE,
    SEC_TRNG2_RAND_BUF_UPDATA_DOWN_SIDE,
} sec_trng2_rand_buf_updata_t;

typedef enum {
    SEC_TRNG_RET_OK = 0,
    SEC_TRNG_RET_ERROR = 1,
    SEC_TRNG_RET_TOO_LONG = 2,
} sec_trng_ret_t;

/**
 * @brief Defination of trng get handler.
 */
typedef sec_trng_ret_t (*sec_trng_random_get_handler)(uint8_t *trng_buffer, uint32_t trng_buffer_length);

/**
 * @brief  Initialise the trng2 driver module. eg: TRNG is "true random number generator".
 */
void sec_trng2_init(void);

/**
 * @brief  De-initialise the trng2 driver module.
 */
void sec_trng2_deinit(void);

/**
 * @brief  Start trng2.
 * @param  trng2_cfg_para Choose threshold (ref value is : 0x10010).
 * @param  data_blocks Data blocks is no more than 4095 (ref value is : 0xfff).
 */
void sec_trng2_start(trng2_cfg_reg_t trng2_cfg_para, uint32_t data_blocks);

/**
 * @brief  Get trng2 result.
 * @return Trng2 result.
 */
trng2_output_data_t sec_trng2_get_result(void);

/**
 * @brief  Register callback of trng2 and enable interrupt.
 * @param  callback The function to register.
 * @return True register success, false failed.
 */
bool sec_trng2_register_callback(SEC_CALLBACK callback);

/**
 * @brief  Unregister callback of trng2, disable and clear interrupt.
 */
void sec_trng2_unregister_callback(void);

/**
 * @brief  Apps core to get random.
 * @param  trng_buffer        Addresses requiring random numbers.
 * @param  trng_buffer_length The length of the random number required.
 * @return return get random success or not, see defination of sec_trng2_ret_t.
 */
sec_trng_ret_t sec_trng_random_get(uint8_t *trng_buffer, uint32_t trng_buffer_length);

/**
 * @brief  Update trng_data shared memory.
 * @param  updata Refreshed buffer.
 */
void sec_trng2_random_buffer_init(sec_trng2_rand_buf_updata_t updata);

/**
 * @brief  Register callback function for refreshing random numbers.
 */
void sec_trng2_updata_reigister(void);

/**
 * @brief  Invoke get random number callback.
 * @param  trng_buffer        The trng buffer to store random numbers.
 * @param  trng_buffer_length The length of the random number required.
 * @return SEC_TRNG_RET_OK if success, otherwise false.
 */
sec_trng_ret_t sec_trng_invoke_random_get_callback(uint8_t *trng_buffer, uint32_t trng_buffer_length);
/**
 * @}
 */
#endif
