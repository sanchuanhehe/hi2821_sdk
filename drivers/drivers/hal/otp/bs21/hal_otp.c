/*
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved.
 * Description:  BS25 OTP HAL.
 * Author: @CompanyNameTag
 * Create: 2022-03-29
 */

#include "hal_otp.h"
#include "non_os.h"
#include "chip_io.h"

#ifndef VIRTUAL_OTP
#include "tcxo.h"
#endif

#define OTP_FIRST_REGION        0
#define OTP_SECOND_REGION       1

#define OTP_REGION_NUM          2

#define HAL_OTP_CTL_REG_OFFSET 0
#define HAL_OTP_CLEAR_RESULT   0x0

#define HAL_OTP_REG_SHIFT       2U
#define HAL_OTP_REG_LENGTH      4U
#define HAL_OTP_REG_DATA_SHIFT  8U
#define HAL_OTP_REG_DATA_LENGTH 16
#define HAL_OTP_REG_VALID_DATA  0x1
#define HAL_OTP_REG_BYTE_OFFSET 1
#define HAL_OTP_BYTES_PRE_REG   2
#define HAL_OTP_BYTE_MASK       0xFF
#define HAL_OTP_REG_H_MASK      0xFF00
#define HAL_OTP_REG_L_MASK      0x00FF

#define HAL_OTP_READ_CLK_PERIOD   0x15
#define HAL_OTP_WRITE_CLK_PERIOD  0x1F

#define HAL_OTP_POWER_ON_DELAY_US 120ULL
#define HAL_OTP_AVDD_DELAY_US     150ULL

#define HAL_EFUSE_RST_REG         0x570000B0
#define HAL_EFUSE_RST_OFFSET      8
#define EFUSE_DELAY_BEFORE_RST    1ULL
#define EFUSE_DELAY_AFTER_RST     12ULL

#if defined VIRTUAL_OTP
#define VIRTUAL_OTP_SEC var_segment("virtual_otp")
uint8_t OTP_status[OTP_MAX_BYTES] VIRTUAL_OTP_SEC;
#endif

uint32_t g_otp_region_read_address[] = { HAL_OTP0_BASE_ADDR };

uint32_t g_otp_region_write_address[] = { HAL_OTP0_CTRL };


void hal_otp_init(void)
{
    if (!hal_otp_refresh_read(HAL_OTP_REGION_0)) {
        return;
    }
}

void hal_otp_deinit(void)
{
}

#ifndef VIRTUAL_OTP
/**
 * Get the offset addr of a otp byte address
 * @param byte_addr the addr of the byte to get register
 * @return address
 */
static uint16_t hal_otp_get_byte_offset(uint32_t byte_addr)
{
    return byte_addr % OTP_FIRST_REGION_BYTES;
}

/**
 * Get the read register of a otp byte address
 * @param byte_addr the addr of the byte to get register
 * @return address
 */
static uint32_t hal_otp_get_read_addr(uint32_t byte_addr)
{
    uint16_t byte_offset = hal_otp_get_byte_offset(byte_addr);
    hal_otp_region_e region = hal_otp_get_region(byte_addr);

    hal_otp_refresh_read(region);
    // Each register has 16 bits, register address is (byte_offset / 2 * 4)
    return g_otp_region_read_address[region] + ((byte_offset >> 1U) << HAL_OTP_REG_SHIFT);
}


/**
 * Get the write register of a otp byte address
 * @param byte_addr the addr of the byte to get register
 * @return address
 */
static uint32_t hal_otp_get_write_addr(uint32_t byte_addr)
{
    uint16_t byte_offset = hal_otp_get_byte_offset(byte_addr);

    hal_otp_region_e region = hal_otp_get_region(byte_addr);
    // Each register has 16 bits, register address is (byte_offset / 2 * 4)
    return g_otp_region_read_address[region] + ((byte_offset >> 1U) << HAL_OTP_REG_SHIFT);
}
#endif

#ifndef VIRTUAL_OTP
static void hal_otp_set_clk_period(hal_otp_region_e region, uint32_t clk_period)
{
#if OTP_SET_CLK_PERIOD == YES
    uint32_t clk_period_addr[HAL_OTP_REGION_MAX] = { HAL_OTP0_CLK_PERIOD };
    writew((void *)(uintptr_t)clk_period_addr[region], clk_period);
#else
    UNUSED(region);
    UNUSED(clk_period);
#endif
}
#endif

/**
 * Write the value in write register to efuse and clear the write register
 * To write the value need:
 * 1. Power on the efuse
 * 2. Set write mode and wait unti OTP_CTL clear
 * 3. Power off the efuse
 * 4. Clear all of the write register
 *
 * @param region which efuse region that need flash
 * return true if OK, or false otherwise
 */
bool hal_otp_flush_write(hal_otp_region_e region)
{
#ifndef VIRTUAL_OTP
    if (region >= HAL_OTP_REGION_MAX) {
        return false;
    }

    writel((void *)((uintptr_t)HAL_OTP_EN_SWITCH_ADDR), HAL_OTP_EN);
    uapi_tcxo_delay_us(HAL_OTP_AVDD_DELAY_US);
    hal_otp_set_clk_period(region, HAL_OTP_WRITE_CLK_PERIOD);
    // Set efuse to write mode
    writel((void *)(uintptr_t)g_otp_region_write_address[region], HAL_OTP_WRITE_MODE);

#else
    UNUSED(region);
#endif
    return true;
}

/**
 * Read the value in efuse to otp read register
 * @param region witch efuse region that need flash
 * return true if OK, or false otherwise
 */
bool hal_otp_refresh_read(hal_otp_region_e region)
{
#ifndef VIRTUAL_OTP
    if (region >= HAL_OTP_REGION_MAX) {
        return false;
    }

    hal_otp_set_clk_period(region, HAL_OTP_READ_CLK_PERIOD);
    // Set efuse to read mode
    writel((void *)(uintptr_t)g_otp_region_write_address[region], HAL_OTP_READ_MODE);

#else
    UNUSED(region);
#endif
    return true;
}

/**
 * Reads the byte at the given OTP memory location
 * @param byte_address The OTP byte address of the byte to read
 * @param value The pointer of read value to store
 * @return true on OK, or false on error
 */
bool hal_otp_read_byte(uint32_t byte_address, uint8_t *value)
{
#if defined VIRTUAL_OTP
    // Accessing locations past the end of the valid address range appears to make the chip crash badly
    *value = OTP_status[byte_address];
#else
    // check address valid
    if ((byte_address >= OTP_MAX_BYTES) || (value == NULL)) {
        return false;
    }

    if (byte_address < OTP_FIRST_REGION_BYTES) {
        while (reg16_getbit((void *)((uintptr_t)HAL_OTP0_BOOT_DONE_REG), 0) != 1) { } // wait efuse0 boot done
    }

    volatile uint16_t *otp_byte = (volatile uint16_t *)((uintptr_t)hal_otp_get_read_addr(byte_address));

    if ((byte_address % HAL_OTP_BYTES_PRE_REG) != 0) {
        *value = (uint8_t)((*otp_byte >> HAL_OTP_REG_DATA_SHIFT) & HAL_OTP_BYTE_MASK);
    } else {
        *value = (uint8_t)(*otp_byte & HAL_OTP_BYTE_MASK);
    }
#endif

    return true;
}

/**
 * Writes the value to the given OTP memory location
 * @param byte_address The OTP byte address of the byte to write
 * @param value The OTP byte value of the byte to write
 * @return true on OK, or false on error
 */
bool hal_otp_write_byte(uint32_t byte_address, uint8_t value)
{
#if defined VIRTUAL_OTP
    OTP_status[byte_address] = value;
#else
    if (byte_address >= OTP_MAX_BYTES) {
        return false;
    }

    uint16_t write_value;
    volatile uint16_t *otp_byte = (volatile uint16_t *)((uintptr_t)hal_otp_get_write_addr(byte_address));

    // Each write register has 16bits, odd address is low 8-bits, even address is high 8-bits.
    write_value = ((byte_address % HAL_OTP_BYTES_PRE_REG) != 0) ?
                   (uint16_t)(((uint16_t)value << HAL_OTP_REG_DATA_SHIFT) & HAL_OTP_REG_H_MASK) :
                   ((uint16_t)value & HAL_OTP_REG_L_MASK);

    *otp_byte = write_value;
#endif
    return true;
}

/**
 * Clear all write register to protect next efuse write.
 */
void hal_otp_clear_all_write_regs(void)
{
    uint32_t write_addr[HAL_OTP_REGION_MAX] = { HAL_OTP0_BASE_ADDR };
    for (uint32_t i = 0; i < OTP_FIRST_REGION_BYTES; i += HAL_OTP_REG_SHIFT) {
        *((volatile uint16_t *)((uintptr_t)(write_addr[HAL_OTP_REGION_0]))) = 0;
        write_addr[HAL_OTP_REGION_0] += HAL_OTP_REG_LENGTH;
    }
}

hal_otp_region_e hal_otp_get_region(uint32_t byte_addr)
{
    UNUSED(byte_addr);
    return (hal_otp_region_e)OTP_FIRST_REGION;
}

bool hal_otp_write_operation(uint32_t address, uint8_t value, hal_otp_region_e region)
{
    if (hal_otp_flush_write(region)) {
        if (hal_otp_write_byte(address, value)) {
            if (hal_otp_refresh_read(region)) {
                return true;
            }
        }
    }
    return false;
}

bool hal_otp_write_buffer_operation(uint32_t address, const uint8_t *buffer, uint16_t length)
{
    bool write_flag[HAL_OTP_REGION_MAX] = { false };

    for (uint32_t i = 0; i < length; i++) {
        hal_otp_region_e region = hal_otp_get_region(address + i);
        if (!write_flag[region]) {
            (void)hal_otp_flush_write(region);
            write_flag[region] = true;
        }
        if (!hal_otp_write_byte(address + i, buffer[i])) {
            return false;
        }
    }

    for (uint32_t i = 0; i < length; i++) {
        hal_otp_region_e region = hal_otp_get_region(address + i);
        if (write_flag[region]) {
            (void)hal_otp_refresh_read(region);
            write_flag[region] = false;
        }
    }
    return true;
}
