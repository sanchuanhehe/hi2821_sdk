/*
 * Copyright (c) @CompanyNameMagicTag 2020-2020. All rights reserved.
 * Description: HAL SEC TRNG2
 * Author: @CompanyNameTag
 * Create: 2020-01-20
 */

#ifndef HAL_SEC_TRNG2_H
#define HAL_SEC_TRNG2_H

#include <stdint.h>
#include <stdbool.h>

/** @defgroup connectivity_drivers_hal_sec_trng2 Sec Trng2
  * @ingroup  connectivity_drivers_hal
  * @{
  */
typedef union {
    struct {
        uint32_t alarm_threshold : 8;
        uint32_t reserved0 : 7;
        uint32_t stall_run_poker : 1;
        uint32_t shutdown_threshold : 5;
        uint32_t reserved1 : 2;
        uint32_t shutdown_fatal : 1;
        uint32_t shutdown_count : 6;
        uint32_t reserved2 : 2;
    } trng2_cfg_para;
    uint32_t trng2_cfg;
} trng2_cfg_reg_t;

typedef struct {
    uint32_t trng2_output_0;
    uint32_t trng2_output_1;
    uint32_t trng2_output_2;
    uint32_t trng2_output_3;
} trng2_output_data_t;

/**
 * @brief  Enable trng2 module.
 */
void hal_sec_trng2_enable(void);

/**
 * @brief  Disable trng2 module.
 */
void hal_sec_trng2_disable(void);

/**
 * @brief  Start trng2 calculate.
 * @param  trng2_cfg_para Choose threshold.
 * @param  data_blocks    Data blocks is no more than 4095.
 */
void hal_sec_trng2_start(trng2_cfg_reg_t trng2_cfg_para, uint32_t data_blocks);

/**
 * @brief  The result has generated or not.
 * @return True if random number has generated, otherwise false.
 */
bool hal_sec_trng2_is_ready(void);

/**
 * @brief  Clear trng2 ready ack.
 */
void hal_sec_trng2_clear_ready_ack(void);

/**
 * @brief  Get the result of trng2.
 * @return Output data.
 */
trng2_output_data_t hal_sec_trng2_output(void);
/**
  * @}
  */
#endif
