/*
 * Copyright (c) @CompanyNameMagicTag 2020-2020. All rights reserved.
 * Description: HAL SEC COMMON
 * Author: @CompanyNameTag
 * Create: 2020-01-20
 */

#ifndef HAL_SEC_COMMON_H
#define HAL_SEC_COMMON_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @defgroup connectivity_drivers_hal_sec SEC
 * @ingroup  connectivity_drivers_hal
 * @{
 */
/**
 * @brief  Definition of sec sub module.
 */
typedef enum {
    SEC_DERIV = 0,
    SEC_TRNG1 = 1,
    SEC_SHA = 2,
    SEC_AES = 3,
    SEC_RSAV2 = 4,
    SEC_TRNG2 = 5,
    SEC_MAX,
} sec_type_t;

/**
 * @brief definition of bytes order.
 */
typedef enum {
    LITTLE_ENDIAN_32BIT = 0,
    LITTLE_ENDIAN_64BIT = 1,
    BIG_ENDIAN_32BIT = 2,
    BIG_ENDIAN_64BIT = 3,
} endian_mode_t;

/** SEC_CALLBACK TYPE */
typedef void (*SEC_CALLBACK)(sec_type_t sec_type);

/**
 * @brief  Enable security module.
 * @param  sec_type Select security module.
 */
void hal_sec_comm_enable(sec_type_t sec_type);

/**
 * @brief  Disable security module.
 * @param  sec_type Select security module.
 */
void hal_sec_comm_disable(sec_type_t sec_type);

/**
 * @brief  Select big-endian or little-endian.
 * @param  sec_type    Select security module.
 * @param  endian_mode Select byte mode of security module.
 */
void hal_sec_comm_set_endian(sec_type_t sec_type, endian_mode_t endian_mode);

/**
 * @brief  Get endian mode.
 * @return Endian mode.
 */
uint32_t hal_sec_comm_get_endian(void);

/**
 * @brief  Register callback of security module.
 * @param  callback The function to register.
 * @param  sec_type Select security module.
 * @return True if register success, otherwise false
 */
bool hal_sec_comm_register_callback(SEC_CALLBACK callback, sec_type_t sec_type);

/**
 * @brief  Unregister callback of security module.
 * @param  sec_type Select security module.
 */
void hal_sec_comm_unregister_callback(sec_type_t sec_type);

/**
 * @brief  Clear interrupt of security module.
 * @param  sec_type Select security module.
 */
void hal_sec_comm_intr_clear(sec_type_t sec_type);

/**
 * @brief  Enable interrupt of security module.
 * @param  sec_type Select security module.
 */
void hal_sec_comm_enable_intr(sec_type_t sec_type);

/**
 * @brief  Disable interrupt of security module.
 * @param  sec_type Select security module.
 */
void hal_sec_comm_disable_intr(sec_type_t sec_type);

/**
 * @brief  Get interrupt status of security module.
 * @param  sec_type Select security module.
 * @return True if has been initialised, otherwise false.
 */
bool hal_sec_comm_get_intr_status(sec_type_t sec_type);

/**
 * @brief  Sec core interrupt handler.
 */
void sec_int_handler(void);

/**
 * @}
 */
#endif
