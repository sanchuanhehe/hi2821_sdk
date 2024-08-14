/*
 * Copyright (c) @CompanyNameMagicTag 2018-2020. All rights reserved.
 * Description:  OTP Driver interface.
 * Author: @CompanyNameTag
 * Create:  2018-10-15
 */

#ifndef NON_OS_OTP_H
#define NON_OS_OTP_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @defgroup connectivity_drivers_non_os_otp OTP
 * @ingroup  connectivity_drivers_non_os
 * @{
 */
/**
 * @brief  OTP Driver return codes.
 */
typedef enum {
    OTP_RET_SUCC,
    OTP_RET_FAIL,
    OTP_RET_INVALID,
    OTP_RET_UNINIT,
} otp_ret_t;

/**
 * @brief  Enables power to the otp system, blocks until the otp is ready to be used.
 */
void otp_init(void);

/**
 * @brief  Removes power from the otp system.
 */
void otp_deinit(void);

/**
 * @brief  Reads a bit from OTP memory.
 * @param  value The value of the bit read.
 * @param  address The source OTP bit address of the bit to be read.
 * @param  bit_pos Position of the bit.
 * @return OTP_RET_SUCC on a successful read, or a return code indicating why a failure occurred.
 */
otp_ret_t otp_read_bit(uint8_t *value, uint32_t address, uint8_t bit_pos);

/**
 * @brief  Reads a byte from OTP memory.
 * @param  value The value of the byte read.
 * @param  address The source OTP byte address of the byte to be read.
 * @return OTP_RET_SUCC on a successful read, or a return code indicating why a failure occurred.
 */
otp_ret_t otp_read_byte(uint8_t *value, uint32_t address);

/**
 * @brief  Reads multiple bytes from OTP memory, into the provided buffer.
 * @param  buffer The buffer to hold the read data.
 * @param  address The initial source OTP byte address of the data to read.
 * @param  length The length of the data, in bytes.
 * @return OTP_RET_SUCC on a successful read, or a return code indicating why a failure occurred.
 */
otp_ret_t otp_read_buffer(uint8_t *buffer, uint32_t address, uint16_t length);

/**
 * @brief  Writes a bit to OTP memory.
 * @param  bit_address The destination OTP bit address of the bit to be written.
 * @param  value A non-zero value indicates the bit will be set,
           a zero value indicates a write should not actually occur.
 * @return OTP_RET_SUCC on a successful write, or a return code indicating why a failure occurred.
 */
otp_ret_t otp_write_bit(uint32_t bit_address, uint8_t value);

/**
 * @brief  Writes a byte to OTP memory.
 * @param  address The destination OTP byte address of the byte to be written.
 * @param  value The 8-bit pattern to write.
 * @return OTP_RET_SUCC on a successful write, or a return code indicating why a failure occurred.
 */
otp_ret_t otp_write_byte(uint32_t address, uint8_t value);

/**
 * @brief  Writes multiple bytes to OTP memory, from the provided buffer.
 * @param  address The initial destination OTP byte address of the data to be written.
 * @param  buffer A buffer containing the data to write.
 * @param  length The length of the data, in bytes.
 * @return OTP_RET_SUCC on a successful write, or a return code indicating why a failure occurred.
 */
otp_ret_t otp_write_buffer(uint32_t address, const uint8_t *buffer, uint16_t length);

/**
 * @brief  Calculate OTP 0-counting CRC.
 * @param  buffer start address of buffer to CRC.
 * @param  length The length of the data, in bytes. Max length is 256 bits.
 * @param  crc CRC values to get.
 * @return OTP_RET_SUCC on a successful write, or a return code indicating why a failure occurred.
 */
otp_ret_t otp_calc_crc(const uint8_t *buffer, uint8_t length, uint8_t *crc);

/**
 * @brief  Check the chip is trimed or not.
 * @return true: trimed or false: not trimed.
 */
bool otp_is_chip_trimed(void);

/**
 * @brief  Get the chip trim version.
 * @return the trim verison.
 */
uint8_t otp_get_trim_version(void);

/**
 * @brief  Get clock ldo voltage from otp.
 * @return the voltage value.
 */
uint8_t otp_get_clkldo_vset(void);

/**
 * @brief  Get dieid
 * @param  dieid_length length of dieid data items returned
 * @param  dieid a pointer to the pointer of a buffer of dieid
 * @return OTP_RET_SUCC means success, or a return code indicating why a failure occurred.
 */
otp_ret_t get_dieid(uint8_t *dieid, uint16_t dieid_length);

/**
 * @}
 */
#endif
