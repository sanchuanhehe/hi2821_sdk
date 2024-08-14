/*
 * Copyright (c) @CompanyNameMagicTag 2020-2020. All rights reserved.
 * Description: SEC COMMON
 * Author: @CompanyNameTag
 * Create: 2020-01-07
 */

#ifndef SEC_COMMON_H
#define SEC_COMMON_H

#include "hal_sec_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup connectivity_drivers_non_os_sec SEC
 * @ingroup  connectivity_drivers_non_os
 * @{
 */
/**
 * @brief  Enable interrupt of sec module.
 * @param  sec_type Select sec module.
 */
void sec_comm_enable_irq(sec_type_t sec_type);

/**
 * @brief  Disable interrupt of sec module.
 * @param  sec_type Select sec module.
 */
void sec_comm_disable_irq(sec_type_t sec_type);

/**
 * @brief  Get ip of sec module initialised or not.
 * @param  sec_type Select sec module.
 * @return Return true if sec_type has been initialised, otherwise return false.
 */
bool sec_common_driver_initialised_get(sec_type_t sec_type);

/**
 * @brief  Set the value of sec module initialise flag.
 * @param  sec_type Select sec module.
 * @param  value The value to set the bit to.
 */
void sec_common_driver_initalised_set(sec_type_t sec_type, bool value);
/**
 * @}
 */
#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif