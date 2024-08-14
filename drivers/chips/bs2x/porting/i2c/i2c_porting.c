/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides i2c port template \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-08-15， Create file. \n
 */

#include "i2c_porting.h"
#include "hal_i2c.h"
#include "hal_i2c_v151.h"
#include "hal_i2c_v151_comm.h"
#include "osal_interrupt.h"
#include "chip_core_irq.h"
#include "pm_clock.h"

#define BUS_CLOCK_TIME_32M 32000000UL

typedef void (*i2c_porting_irq_handler)(void);

typedef struct i2c_irq_handler {
    uint32_t irq_id;
    i2c_porting_irq_handler irq_handler;
} i2c_irq_handler_t;


uintptr_t g_i2c_base_addrs[I2C_BUS_MAX_NUM] = {
    (uintptr_t)I2C_BUS_0_BASE_ADDR,
    (uintptr_t)I2C_BUS_1_BASE_ADDR,
};

static void irq_i2c0_handler(void)
{
    hal_i2c_v151_irq_handler(I2C_BUS_0);
}

static void irq_i2c1_handler(void)
{
    hal_i2c_v151_irq_handler(I2C_BUS_1);
}

static i2c_irq_handler_t g_i2c_irq_id[I2C_BUS_MAX_NUM] = {
    {
        I2C_0_IRQN,
        irq_i2c0_handler,
    },
    {
        I2C_1_IRQN,
        irq_i2c1_handler,
    },
};

uint32_t i2c_port_get_clock_value(i2c_bus_t bus)
{
    if (bus >= I2C_BUS_MAX_NUM) {
        return 0;
    }
    return BUS_CLOCK_TIME_32M;
}

void i2c_port_register_hal_funcs(i2c_bus_t bus)
{
    hal_i2c_register_funcs(bus, hal_i2c_v151_funcs_get());
}

void i2c_port_unregister_hal_funcs(i2c_bus_t bus)
{
    hal_i2c_unregister_funcs(bus);
}

void i2c_port_register_irq(i2c_bus_t bus)
{
    i2c_irq_handler_t irq = g_i2c_irq_id[bus];
    osal_irq_request(irq.irq_id, (osal_irq_handler)irq.irq_handler, NULL, NULL, NULL);
    osal_irq_enable(irq.irq_id);
}

void i2c_port_unregister_irq(i2c_bus_t bus)
{
    i2c_irq_handler_t irq = g_i2c_irq_id[bus];
    osal_irq_free(irq.irq_id, NULL);
}

uint32_t i2c_porting_lock(i2c_bus_t bus)
{
    unused(bus);
    return osal_irq_lock();
}

void i2c_porting_unlock(i2c_bus_t bus, uint32_t irq_sts)
{
    unused(bus);
    osal_irq_restore(irq_sts);
}

void i2c_port_clock_enable(i2c_bus_t bus, bool on)
{
    clock_control_type_t control_type;
    clock_mclken_aperp_type_t aperp_type;

    switch (bus) {
        case I2C_BUS_0:
            aperp_type = CLOCK_APERP_I2C0_CLKEN;
            break;

        case I2C_BUS_1:
            aperp_type = CLOCK_APERP_I2C1_CLKEN;
            break;

        default:
            return;
    }

    control_type = (on == true) ? CLOCK_CONTROL_MCLKEN_ENABLE : CLOCK_CONTROL_MCLKEN_DISABLE;
    uapi_clock_control(control_type, aperp_type);
}