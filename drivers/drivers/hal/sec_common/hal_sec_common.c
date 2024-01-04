/*
 * Copyright (c) @CompanyNameMagicTag 2020-2020. All rights reserved.
 * Description: HAL SEC COMMON
 * Author: @CompanyNameTag
 * Create: 2020-01-20
 */

#include "hal_sec_common.h"
#include "pal_sec_config_port.h"
#include "chip_io.h"

#define RST_SOFT_N_REG  (PAL_SEC_CTRL_REG_BASE + 0x20)
#define SOFT_CLKEN_REG  (PAL_SEC_CTRL_REG_BASE + 0x24)
#define CFG_ENDIAN_REG  (PAL_SEC_CTRL_REG_BASE + 0x28)
#define SEC_INT_EN_REG  (PAL_SEC_CTRL_REG_BASE + 0x30)
#define SEC_INT_CLR_REG (PAL_SEC_CTRL_REG_BASE + 0x34)
#define SEC_INT_STS_REG (PAL_SEC_CTRL_REG_BASE + 0x38)

#define RSAV2_INTMSK_REG    (PAL_RSAV2_REG_BASE + 0x2800)
#define RSAV2_RINT_REG      (PAL_RSAV2_REG_BASE + 0x2804)
#define RSAV2_INTSTS_REG    (PAL_RSAV2_REG_BASE + 0x2808)
#define TRNG_CONTROL_REG    (PAL_TRNG_REG_BASE + 0x14)
#define TRNG_STATUS_REG     (PAL_TRNG_REG_BASE + 0x10)
#define TRNG_INTACK_REG     (PAL_TRNG_REG_BASE + 0x10)

#define M_SOFT_RST_N_REG_REG    (PAL_M_CTRL_REG_BASE + 0x54)
#define M_SOFT_RST_N1_REG_REG   (PAL_M_CTRL_REG_BASE + 0x58)

#if SEC_SUB_RST_BY_SECURITY_CORE == YES
#define SEC_SUB_STOP0_L_REG     (PAL_SEC_SUB_REG_BASE + 0x60)
#define SOFT_RST_SEC_ALG_BIT 7
#endif

#define SOFT_RST_SEC_N_OFFSET 3
#define SOFT_RST_MEM_BUS_OFFSET 8
#define SOFT_RST_SHA_N_OFFSET 2
#define RSAV2_NC_CIPHER_INT_OFFSET 4
#define RSAV2_NC_FAIL_RINT_OFFSET  8
#define RSAV2_AHBM_TIMEOUT_RINT_OFFSET  14
#define TRNG_READY_OFFSET 0
#define TRNG_CONTROL_DEFAULT_VAL 0

typedef struct {
    bool flag;
    SEC_CALLBACK callback;
} hal_sec_interrupt_flag_t;

static hal_sec_interrupt_flag_t g_hal_sec_comm_callbacks[SEC_MAX];

static void hal_sec_comm_soft_rst(sec_type_t sec_type)
{
    reg32_setbit(RST_SOFT_N_REG, (uint32_t)(sec_type));
}

static void hal_sec_comm_clken(sec_type_t sec_type)
{
    reg32_setbit(SOFT_CLKEN_REG, (uint32_t)(sec_type));
}

static void hal_sec_comm_trigger_irq_callback(sec_type_t sec_type)
{
    if (sec_type < SEC_MAX) {
        if ((g_hal_sec_comm_callbacks[sec_type].flag) && \
            (g_hal_sec_comm_callbacks[sec_type].callback != NULL)) {
                g_hal_sec_comm_callbacks[sec_type].callback(sec_type);
                hal_sec_comm_intr_clear(sec_type);
            }
    }
}

static bool hal_sec_comm_get_intr_clk(sec_type_t sec_type)
{
    if (sec_type >= SEC_MAX) {
        return false;
    }
    return reg32_getbit(SOFT_CLKEN_REG, (uint32_t)sec_type);
}

void hal_sec_comm_enable(sec_type_t sec_type)
{
    if (sec_type >= SEC_MAX) {
        return;
    }
#if SEC_SUB_RST_BY_SECURITY_CORE == YES
    reg32_setbit(SEC_SUB_STOP0_L_REG, SOFT_RST_SEC_ALG_BIT);
#else
    reg32_setbit(M_SOFT_RST_N_REG_REG, SOFT_RST_SEC_N_OFFSET);
    reg32_setbit(M_SOFT_RST_N_REG_REG, SOFT_RST_MEM_BUS_OFFSET);
#endif
    hal_sec_comm_soft_rst(sec_type);
    hal_sec_comm_clken(sec_type);
}

void hal_sec_comm_disable(sec_type_t sec_type)
{
    if (sec_type >= SEC_MAX) {
        return;
    }
    reg32_clrbit(SOFT_CLKEN_REG, (uint32_t)(sec_type));
}

void hal_sec_comm_set_endian(sec_type_t sec_type, endian_mode_t endian_mode)
{
    if (sec_type <= SEC_RSAV2) {
        writel(CFG_ENDIAN_REG, (uint32_t)(endian_mode));
    }
}

uint32_t hal_sec_comm_get_endian(void)
{
    return reg32(CFG_ENDIAN_REG);
}

bool hal_sec_comm_register_callback(SEC_CALLBACK callback, sec_type_t sec_type)
{
    if ((sec_type >= SEC_MAX) || (callback == NULL)) {  //lint !e774 if always evaluates to False
        return false;
    }

    if (g_hal_sec_comm_callbacks[sec_type].flag == false) {
        g_hal_sec_comm_callbacks[sec_type].flag = true;
        g_hal_sec_comm_callbacks[sec_type].callback = callback;
        return true;
    }
    return false;
}

void hal_sec_comm_unregister_callback(sec_type_t sec_type)
{
    if (sec_type >= SEC_MAX) {
        return;
    }

    if (g_hal_sec_comm_callbacks[sec_type].callback != NULL) {
        g_hal_sec_comm_callbacks[sec_type].flag = false;
        g_hal_sec_comm_callbacks[sec_type].callback = NULL;
    }
}

void hal_sec_comm_intr_clear(sec_type_t sec_type)
{
    if (sec_type >= SEC_MAX) {
        return;
    }

    if (sec_type < SEC_RSAV2) {
        reg32_setbit(SEC_INT_CLR_REG, (uint32_t)(sec_type));
    } else if (sec_type == SEC_RSAV2) {
        reg32_setbit(RSAV2_RINT_REG, RSAV2_NC_CIPHER_INT_OFFSET);
        reg32_setbit(RSAV2_RINT_REG, RSAV2_NC_FAIL_RINT_OFFSET);
        reg32_setbit(RSAV2_RINT_REG, RSAV2_AHBM_TIMEOUT_RINT_OFFSET);
    } else {
        writel(TRNG_CONTROL_REG, TRNG_CONTROL_DEFAULT_VAL);
        reg32_setbit(TRNG_INTACK_REG, TRNG_READY_OFFSET);
        reg32_clrbit(TRNG_INTACK_REG, TRNG_READY_OFFSET);
    }
}

bool hal_sec_comm_get_intr_status(sec_type_t sec_type)
{
    if (sec_type >= SEC_MAX) {
        return false;
    }

    if (sec_type < SEC_RSAV2) {
        return reg32_getbit(SEC_INT_STS_REG, (uint32_t)sec_type);
    } else if (sec_type == SEC_RSAV2) {
        return reg32(RSAV2_RINT_REG);
    }
    return reg32_getbit(TRNG_STATUS_REG, TRNG_READY_OFFSET);
}

void hal_sec_comm_enable_intr(sec_type_t sec_type)
{
    if (sec_type >= SEC_MAX) {
        return;
    }

    if (sec_type < SEC_RSAV2) {
        reg32_setbit(SEC_INT_EN_REG, (uint32_t)(sec_type));
    } else if (sec_type == SEC_RSAV2) {
        reg32_clrbit(RSAV2_INTMSK_REG, RSAV2_NC_CIPHER_INT_OFFSET);
        reg32_clrbit(RSAV2_INTMSK_REG, RSAV2_NC_FAIL_RINT_OFFSET);
        reg32_clrbit(RSAV2_INTMSK_REG, RSAV2_AHBM_TIMEOUT_RINT_OFFSET);
    } else {
        reg32_setbit(TRNG_CONTROL_REG, TRNG_READY_OFFSET);
    }
}

void hal_sec_comm_disable_intr(sec_type_t sec_type)
{
    if (sec_type >= SEC_MAX) {
        return;
    }

    if (sec_type < SEC_RSAV2) {
        reg32_clrbit(SEC_INT_EN_REG, (uint32_t)(sec_type));
    } else if (sec_type == SEC_RSAV2) {
        reg32_setbit(RSAV2_INTMSK_REG, RSAV2_NC_CIPHER_INT_OFFSET);
        reg32_setbit(RSAV2_INTMSK_REG, RSAV2_NC_FAIL_RINT_OFFSET);
        reg32_setbit(RSAV2_INTMSK_REG, RSAV2_AHBM_TIMEOUT_RINT_OFFSET);
    } else {
        writel(TRNG_CONTROL_REG, 0x0);
        reg32_setbit(TRNG_INTACK_REG, TRNG_READY_OFFSET);
        reg32_clrbit(TRNG_INTACK_REG, TRNG_READY_OFFSET);
    }
}

void sec_int_handler(void)
{
    for (int32_t index = 0; index < SEC_MAX; index++) {
        if (hal_sec_comm_get_intr_clk((sec_type_t)index) && hal_sec_comm_get_intr_status((sec_type_t)index)) {
            hal_sec_comm_trigger_irq_callback((sec_type_t)index);
        }
    }
}
