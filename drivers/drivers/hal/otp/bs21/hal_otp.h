/*
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved.
 * Description: BS25 OTP HAL interface.
 * Author: @CompanyNameTag
 * Create: 2022-03-29
 */

#ifndef HAL_OTP_H
#define HAL_OTP_H

#include "core.h"
#include "product.h"

/**
 * @defgroup connectivity_drivers_hal_otp OTP
 * @ingroup  connectivity_drivers_hal
 * @{
 */
#define OTP_FIRST_REGION_BYTES  (OTP_FIRST_REGION_BITS >> 3)  // MAX_BIT / 8
#define OTP_MAX_BITS         (OTP_FIRST_REGION_BITS)
#define OTP_MAX_BYTES        (OTP_FIRST_REGION_BYTES)  // MAX_BIT / 8

#define OTP_MAX_BIT_POS      8U

#define FUSE_CTL_RB_BASE        0x57028000

#define HAL_OTP0_BOOT_DONE_REG  (FUSE_CTL_RB_BASE + 0x30)
#define HAL_OTP0_CTRL           (FUSE_CTL_RB_BASE + 0x40)
#define HAL_OTP0_CLK_PERIOD     (FUSE_CTL_RB_BASE + 0x44)
#define HAL_OTP0_BASE_ADDR      (FUSE_CTL_RB_BASE + 0x400)
#define HAL_OTP_EN_SWITCH_ADDR  (ULP_AON_CTL_RB_ADDR + 0x254)
#define HAL_OTP_EN              0xa5a5
#define HAL_OTP_OFF             0x5a5a

#define HAL_OTP_WRITE_MODE 0xa5a5
#define HAL_OTP_READ_MODE  0x5a5a

typedef enum {
    HAL_OTP_REGION_0,
    HAL_OTP_REGION_MAX,
} hal_otp_region_e;

/**
 * @brief  Enables power to the otp system, blocks until the otp is ready to be used
 */
void hal_otp_init(void);

/**
 * @brief  Removes power from the otp system
 */
void hal_otp_deinit(void);

/**
 * @brief  Flush select otp region, write the value in register to efuse.
 * @param  region Select OTP region
 * @return true on OK, false otherwise.
 */
bool hal_otp_flush_write(hal_otp_region_e region);

/**
 * @brief  Refresh read region to read mode after write.
 * @param  region Select OTP region
 * @return true on OK, false otherwise.
 */
bool hal_otp_refresh_read(hal_otp_region_e region);

/**
 * @brief  Reads the byte at the given OTP memory location
 * @param  byte_address The OTP byte address of the byte to read
 * @param  value The pointer of read value to store
 * @return true on OK, or false on error
 */
bool hal_otp_read_byte(uint32_t byte_address, uint8_t *value);

/**
 * @brief  Writes the value to the given OTP memory location
 * @param  byte_address The OTP byte address of the byte to write
 * @param  value The OTP byte value of the byte to write
 * @return true on OK, or false on error
 */
bool hal_otp_write_byte(uint32_t byte_address, uint8_t value);

/**
 * @brief  Clear all write register to protect next efuse write.
 */
void hal_otp_clear_all_write_regs(void);

/**
 * @brief  Get the region of a otp byte address
 * @param  byte_addr the addr of the byte to get register
 * @return region The region of otp
 */
hal_otp_region_e hal_otp_get_region(uint32_t byte_addr);

/**
 * @brief  Writes the value to the given OTP memory location
 * @param  address The byte address of OTP to write to
 * @param  value  The value to write to OTP memory
 * @param  region  The region of OTP to write
 * @return The OTP write operation result, true mean SUCCESS, false mean FAILED
 */
bool hal_otp_write_operation(uint32_t address, uint8_t value, hal_otp_region_e region);

/**
 * @brief  Writes the buffer to the given OTP memory location
 * @param  address The byte address of OTP to write to
 * @param  buffer  The buffer to write to OTP memory
 * @param  length  The length of buffer to write to OTP memory
 * @return The OTP write operation result, true mean SUCCESS, false mean FAILED
 */
bool hal_otp_write_buffer_operation(uint32_t address, const uint8_t *buffer, uint16_t length);

/**
 * @}
 */
#endif
