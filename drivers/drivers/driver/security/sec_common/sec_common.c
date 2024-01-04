/*
 * Copyright (c) @CompanyNameMagicTag 2020-2020. All rights reserved.
 * Description: SEC COMMON
 * Author: @CompanyNameTag
 * Create: 2020-01-07
 */

#include "sec_common.h"
#include "stddef.h"
#include "osal_interrupt.h"
#include "pal_sec_config_port.h"

static uint32_t g_sec_driver_init_bit = 0;

void sec_comm_enable_irq(sec_type_t sec_type)
{
    if (sec_type >= SEC_MAX) {
        return;
    }
    osal_irq_request(SEC_INT_NUMBER, (osal_irq_handler)sec_int_handler, NULL, NULL, NULL);
    osal_irq_set_priority(SEC_INT_NUMBER,  SEC_INT_PRI);
    osal_irq_enable(SEC_INT_NUMBER);
}

void sec_comm_disable_irq(sec_type_t sec_type)
{
    if (sec_type >= SEC_MAX) {
        return;
    }
    osal_irq_free(SEC_INT_NUMBER, NULL);
}

bool sec_common_driver_initialised_get(sec_type_t sec_type)
{
    return ((g_sec_driver_init_bit & (uint32_t)((uint32_t)0x01UL << (uint32_t)sec_type)) != 0);
}

void sec_common_driver_initalised_set(sec_type_t sec_type, bool value)
{
    uint32_t irq_sts = osal_irq_lock();
    uint32_t mask = (uint32_t)0x01UL << (uint32_t)sec_type;
    if (value) {
        g_sec_driver_init_bit |= mask;
    } else {
        g_sec_driver_init_bit &= (~mask);
    }
    osal_irq_restore(irq_sts);
}
