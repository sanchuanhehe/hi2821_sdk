/*
 * Copyright (c) @CompanyNameMagicTag 2018-2020. All rights reserved.
 * Description:   OTP Driver.
 * Author: @CompanyNameTag
 * Create:  2018-10-15
 */
#include "otp.h"
#include "non_os.h"
#include "soc_osal.h"

#if (OTP_HAS_READ_PERMISSION == YES)
#include "hal_otp.h"
#include "otp_map.h"
#include "soc_osal.h"
#include "common_def.h"
#endif

#define OTP_PMU_CLKLDO_VSET_MASK       0x78
#define OTP_PMU_CLKLDO_OFFSET          3
#define OTP_TRIM_VERSION_DEFAULT_VALUE 1
#define OTP_CHAR_BIT_WIDE              8
#define OTP_CALC_CRC_MAX_LEN          32

#if (defined(VIRTUAL_OTP) && (CHIP_ASIC))
#error "ASIC version does not need to define VIRTUAL_OTP"
#endif

#if (OTP_HAS_READ_PERMISSION == YES)
static bool g_otp_inited = false;
#endif

void otp_init(void)
{
#if (OTP_HAS_WRITE_PERMISSION == YES)
    uint32_t irq = osal_irq_lock();
    if (unlikely(g_otp_inited)) {
        osal_irq_restore(irq);
        return;
    }

    hal_otp_init();
    g_otp_inited = true;
    osal_irq_restore(irq);
#endif
}

/*
 * Removes power from the otp system
 */
void otp_deinit(void)
{
#if (OTP_HAS_WRITE_PERMISSION == YES)
    uint32_t irq = osal_irq_lock();
    if (unlikely(!g_otp_inited)) {
        osal_irq_restore(irq);
        return;
    }

    hal_otp_deinit();

    g_otp_inited = false;
    osal_irq_restore(irq);
#endif
}

/*
 * Reads a bit from OTP memory
 */
otp_ret_t otp_read_bit(uint8_t *value, uint32_t address, uint8_t bit_pos)
{
#if (OTP_HAS_READ_PERMISSION == YES)
    uint8_t read_value = 0;

    if ((address >= OTP_MAX_BYTES) || (value == NULL) || (bit_pos >= OTP_MAX_BIT_POS)) {
        return OTP_RET_FAIL;
    }

    uint32_t irq_sts = osal_irq_lock();
    if (hal_otp_read_byte(address, &read_value)) {
        *value = (read_value >> bit_pos) & 1U;
        osal_irq_restore(irq_sts);
        return OTP_RET_SUCC;
    }
    osal_irq_restore(irq_sts);

    return OTP_RET_FAIL;
#else
    UNUSED(value);
    UNUSED(address);
    UNUSED(bit_pos);
    return OTP_RET_SUCC;
#endif
}

/*
 * Reads a byte from OTP memory
 */
otp_ret_t otp_read_byte(uint8_t *value, uint32_t address)
{
#if (OTP_HAS_READ_PERMISSION == YES)
    if ((address >= OTP_MAX_BYTES) || (value == NULL)) {
        return OTP_RET_FAIL;
    }

    uint32_t irq_sts = osal_irq_lock();
    if (hal_otp_read_byte(address, value)) {
        osal_irq_restore(irq_sts);
        return OTP_RET_SUCC;
    }
    osal_irq_restore(irq_sts);

    return OTP_RET_FAIL;
#else
    UNUSED(value);
    UNUSED(address);
    return OTP_RET_SUCC;
#endif
}

/*
 * Reads multiple bytes from OTP memory, into the provided buffer
 */
otp_ret_t otp_read_buffer(uint8_t *buffer, uint32_t address, uint16_t length)
{
#if (OTP_HAS_READ_PERMISSION == YES)
    if ((length == 0) || (address >= OTP_MAX_BYTES) || ((address + length) > OTP_MAX_BYTES) || (buffer == NULL)) {
        return OTP_RET_FAIL;
    }

    uint32_t irq_sts = osal_irq_lock();
    for (uint32_t i = 0; i < length; i++) {
        if (!hal_otp_read_byte(address + i, &buffer[i])) {
            osal_irq_restore(irq_sts);
            return OTP_RET_FAIL;
        }
    }
    osal_irq_restore(irq_sts);

    return OTP_RET_SUCC;
#else
    UNUSED(buffer);
    UNUSED(address);
    UNUSED(length);
    return OTP_RET_SUCC;
#endif
}

/*
 * Writes a bit to OTP memory
 */
otp_ret_t otp_write_bit(uint32_t bit_address, uint8_t value)
{
#if (OTP_HAS_WRITE_PERMISSION == YES)
    if (unlikely(!g_otp_inited)) {
        return OTP_RET_UNINIT;
    }
    if (bit_address >= OTP_MAX_BYTES || value >= OTP_MAX_BIT_POS) {
        return OTP_RET_FAIL;
    }

    uint8_t write_value = 0;

    uint32_t irq_sts = osal_irq_lock();
    if (!hal_otp_read_byte(bit_address, &write_value)) {
        osal_irq_restore(irq_sts);
        return OTP_RET_FAIL;
    }

    if ((write_value & BIT(value)) != 0) {
        osal_irq_restore(irq_sts);
        return OTP_RET_INVALID;
    }

    hal_otp_region_e region = hal_otp_get_region(bit_address);
    if (hal_otp_write_operation(bit_address, (uint8_t)BIT(value), region)) {
        osal_irq_restore(irq_sts);
        return OTP_RET_SUCC;
    }

    osal_irq_restore(irq_sts);
    return OTP_RET_FAIL;
#else
    UNUSED(bit_address);
    UNUSED(value);
    return OTP_RET_SUCC;
#endif
}

/*
 * Writes a byte to OTP memory
 */
otp_ret_t otp_write_byte(uint32_t address, uint8_t value)
{
#if (OTP_HAS_WRITE_PERMISSION == YES)
    if (unlikely(!g_otp_inited)) {
        return OTP_RET_UNINIT;
    }
    if (address >= OTP_MAX_BYTES) {
        return OTP_RET_FAIL;
    }

    uint8_t write_value = 0;

    uint32_t irq_sts = osal_irq_lock();
    if (!hal_otp_read_byte(address, &write_value)) {
        osal_irq_restore(irq_sts);
        return OTP_RET_FAIL;
    }

    if ((write_value & value) != 0) {
        osal_irq_restore(irq_sts);
        return OTP_RET_INVALID;
    }

    hal_otp_region_e region = hal_otp_get_region(address);
    if (hal_otp_write_operation(address, value, region)) {
        osal_irq_restore(irq_sts);
        return OTP_RET_SUCC;
    }

    osal_irq_restore(irq_sts);

    return OTP_RET_FAIL;
#else
    UNUSED(address);
    UNUSED(value);
    return OTP_RET_SUCC;
#endif
}

static otp_ret_t otp_write_param_check(uint32_t address, const uint8_t *buffer, uint16_t length)
{
#if (OTP_HAS_WRITE_PERMISSION == YES)
    if (unlikely(!g_otp_inited)) {
        return OTP_RET_UNINIT;
    }

    if ((length == 0) || (address >= OTP_MAX_BYTES) || ((address + length) > OTP_MAX_BYTES) || (buffer == NULL)) {
        return OTP_RET_FAIL;
    }

    return OTP_RET_SUCC;
#else
    UNUSED(buffer);
    UNUSED(address);
    UNUSED(length);
    return OTP_RET_SUCC;
#endif
}

/*
 * Writes multiple bytes to OTP memory, from the provided buffer
 */
otp_ret_t otp_write_buffer(uint32_t address, const uint8_t *buffer, uint16_t length)
{
#if (OTP_HAS_WRITE_PERMISSION == YES)
    uint8_t write_value = 0;
    otp_ret_t ret;

    uint32_t irq_sts = osal_irq_lock();
    ret = otp_write_param_check(address, buffer, length);
    if (ret != OTP_RET_SUCC) {
        osal_irq_restore(irq_sts);
        return ret;
    }

    for (uint32_t i = 0; i < length; i++) {
        if ((!hal_otp_read_byte(address + i, &write_value)) || ((write_value & buffer[i]) != 0)) {
            osal_irq_restore(irq_sts);
            return OTP_RET_FAIL;
        }
    }

    if (!hal_otp_write_buffer_operation(address, buffer, length)) {
        osal_irq_restore(irq_sts);
        return OTP_RET_FAIL;
    }

    osal_irq_restore(irq_sts);

    return OTP_RET_SUCC;
#else
    UNUSED(otp_write_param_check);
    UNUSED(buffer);
    UNUSED(address);
    UNUSED(length);
    return OTP_RET_SUCC;
#endif
}

static uint8_t otp_count_zero_in_byte(uint8_t value)
{
    uint8_t num_zeroes = 0;
    uint8_t tmp_value = value;
    for (uint8_t i = 0; i < OTP_CHAR_BIT_WIDE; i++, tmp_value = tmp_value >> 1) {
        if ((tmp_value & 1) == 0) {
            num_zeroes++;
        }
    }
    return num_zeroes;
}

/*
 * Calculate OPT zero-counting CRC for a block upto 32 bytes (256 bits)
 */
otp_ret_t otp_calc_crc(const uint8_t *buffer, uint8_t length, uint8_t *crc)
{
    int i;
    otp_ret_t ret_value = OTP_RET_SUCC;

    if ((buffer == NULL) || (crc == NULL) || (length > OTP_CALC_CRC_MAX_LEN)) {
        ret_value = OTP_RET_INVALID;
    } else {
        *crc = 0;
        for (i = 0; i < length; i++) {
            *crc += otp_count_zero_in_byte(buffer[i]);
        }
    }

    return ret_value;
}

bool otp_is_chip_trimed(void)
{
#if (OTP_HAS_READ_PERMISSION == YES)
    uint8_t trimed = 0;

#ifdef OTP_ATE_TRIM_FLAG_BYTE
    if (otp_read_bit(&trimed, OTP_ATE_TRIM_FLAG_BYTE, OTP_ATE_TRIM_FLAG_BIT) != OTP_RET_SUCC) {
        return false;
    }
#else
    if (otp_read_bit(&trimed, OTP_TRIM_FLAG, OTP_IS_CHIP_TRIMED) != OTP_RET_SUCC) {
        return false;
    }
#endif
    if (trimed != 0) {
        return true;
    }

    return false;
#else
    return true;
#endif
}

uint8_t otp_get_trim_version(void)
{
#if (OTP_HAS_READ_PERMISSION == YES)
    uint8_t trim_version = OTP_TRIM_VERSION_DEFAULT_VALUE;

#ifdef OTP_ATE_TRIM_FLAG_BYTE
    uint8_t trim_flag = 0;
    if ((otp_read_bit(&trim_flag, OTP_ATE_TRIM_FLAG_BYTE, OTP_ATE_TRIM_FLAG_BIT) != OTP_RET_SUCC) ||
        (trim_flag == 0)) {
        return OTP_TRIM_VERSION_DEFAULT_VALUE;
    }
#endif

    return trim_version;
#else
    return OTP_TRIM_VERSION_DEFAULT_VALUE;
#endif
}

uint8_t otp_get_clkldo_vset(void)
{
#if (OTP_HAS_CLKLDO_VSET == YES)
    uint8_t vset = 0;

    if (otp_read_byte(&vset, OTP_PMU_TRIM_VSET) != OTP_RET_SUCC) {
        return 0;
    }

    vset = (vset & OTP_PMU_CLKLDO_VSET_MASK) >> OTP_PMU_CLKLDO_OFFSET;

    return vset;
#else
    return 0;
#endif
}

otp_ret_t get_dieid(uint8_t *dieid, uint16_t dieid_length)
{
    if ((dieid == NULL) || (dieid_length < OTP_ATE_DIEID_LENGTH)) { return OTP_RET_INVALID; }

    otp_ret_t ret = otp_read_buffer(dieid, OTP_ATE_DIEID_START, OTP_ATE_DIEID_LENGTH);
    return ret;
}
