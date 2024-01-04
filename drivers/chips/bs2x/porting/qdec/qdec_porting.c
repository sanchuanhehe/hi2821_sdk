/**
 * Copyright (c) @CompanyNameMagicTag 2022-2023. All rights reserved. \n
 *
 * Description: Provides qdec port template \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-09-13， Create file. \n
 */
#include "chip_io.h"
#include "gpio.h"
#include "osal_interrupt.h"
#include "pinctrl_porting.h"
#include "pinctrl.h"
#include "arch_port.h"
#include "qdec.h"
#include "hal_qdec_v150.h"
#include "hal_qdec_v150_regs_op.h"
#include "qdec_porting.h"

#define QDEC_BASE_ADDR       0x52000200
#define QDEC_CLK_DIV_ADDR    0x520004A0

static uintptr_t g_qdec_base_addr = (uintptr_t)QDEC_BASE_ADDR;
static uintptr_t g_qdec_clk_div_addr = (uintptr_t)QDEC_CLK_DIV_ADDR;
static uint8_t g_qdec_acc_per_roll = 1;

uintptr_t qdec_porting_base_addr_get(void)
{
    return g_qdec_base_addr;
}

uintptr_t qdec_clk_div_porting_base_addr_get(void)
{
    return g_qdec_clk_div_addr;
}

void qdec_port_register_hal_funcs(void)
{
    hal_qdec_register_funcs(hal_qdec_v150_get_funcs());
}

void qdec_port_unregister_hal_funcs(void)
{
    hal_qdec_unregister_funcs();
}

void qdec_port_regs_init(void)
{
    hal_qdec_v150_regs_init();
}

void qdec_port_regs_deinit(void)
{
    hal_qdec_v150_regs_deinit();
}

void qdec_irq_handler(void)
{
    hal_qdec_irq_handler();
}

void qdec_port_register_irq(void)
{
    osal_irq_request(IRQ_QDEC, (osal_irq_handler)qdec_irq_handler, NULL, NULL, NULL);
    osal_irq_set_priority(IRQ_QDEC,  irq_prio(IRQ_QDEC));
    osal_irq_enable(IRQ_QDEC);
}

void qdec_port_unregister_irq(void)
{
    osal_irq_free(IRQ_QDEC, NULL);
}

void qdec_port_pinmux_init(pin_t qdec_a, pin_t qdec_b)
{
    uapi_gpio_set_dir(qdec_a, GPIO_DIRECTION_INPUT);
    uapi_pin_set_ds(qdec_a, PIN_DS_3);
    uapi_pin_set_pull(qdec_a, PIN_PULL_UP);
    uapi_pin_set_mode(qdec_a, (pin_mode_t)HAL_PIO_QDEC_A);
    uapi_gpio_set_dir(qdec_b, GPIO_DIRECTION_INPUT);
    uapi_pin_set_ds(qdec_b, PIN_DS_3);
    uapi_pin_set_pull(qdec_b, PIN_PULL_UP);
    uapi_pin_set_mode(qdec_b, (pin_mode_t)HAL_PIO_QDEC_B);
}

void qdec_clk_sel_set(qdec_clk_type_t clk_type)
{
    if ((clk_type != QDEC_SLEEP_CLK) && (clk_type != QDEC_NOR_CLK)) {
        return;
    }
    if (clk_type == QDEC_SLEEP_CLK) {
        reg16_clrbit(M_CTL_QDEC_CLK_CTL_REG, QDEC_CLK_EN_BIT); // Disable the clock sel en.
        reg16_setbit(M_CTL_QDEC_CLK_CTL_REG, QDEC_CLK_SEL_BIT); // Clock source conversion to 32k.
        reg16_clrbit(M_CTL_QDEC_SAMPLEPER_REG, QDEC_CLK_FREQ_BIT); // Frequency conversion to 32k.
        reg16_setbit(M_CTL_QDEC_CLK_CTL_REG, QDEC_CLK_EN_BIT); // Enable the clock sel en.
    } else {
        reg16_clrbit(M_CTL_QDEC_CLK_CTL_REG, QDEC_CLK_EN_BIT); // Disable the clock sel en.
        reg16_clrbit(M_CTL_QDEC_CLK_CTL_REG, QDEC_CLK_SEL_BIT); // Clock source conversion to 1M.
        reg16_setbit(M_CTL_QDEC_SAMPLEPER_REG, QDEC_CLK_FREQ_BIT); // Frequency conversion to 1M.
        reg16_setbit(M_CTL_QDEC_CLK_CTL_REG, QDEC_CLK_EN_BIT); // Enable the clock sel en.
    }
}
void qdec_port_set_acc_per_roll(uint8_t acc_per_roll)
{
    g_qdec_acc_per_roll = acc_per_roll;
}

uint8_t qdec_port_get_acc_per_roll(void)
{
    return g_qdec_acc_per_roll;
}