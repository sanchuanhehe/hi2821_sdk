/*
 * Copyright (c) @CompanyNameMagicTag 2022-2023. All rights reserved.
 * Description: I2S port for project
 * Author: @CompanyNameTag
 * Create: 2023-03-10
 */

#include "common_def.h"
#include "hal_sio_v151.h"
#include "oal_interface.h"
#include "soc_osal.h"
#include "interrupt.h"
#include "pinctrl_porting.h"
#include "pinctrl.h"
#include "chip_core_irq.h"
#include "sio_porting.h"

static uintptr_t g_sio_base_addrs[I2S_MAX_NUMBER] = {
    (uintptr_t)I2S_BUS_0_BASE_ADDR,
};

typedef struct sio_interrupt {
    core_irq_t irq_num;
    osal_irq_handler irq_func;
}sio_interrupt_t;

static const sio_interrupt_t g_sio_interrupt_lines[I2S_MAX_NUMBER] = {
    { I2S_IRQN, (osal_irq_handler)irq_sio0_handler },
};

uintptr_t sio_porting_base_addr_get(sio_bus_t bus)
{
    return g_sio_base_addrs[bus];
}

void sio_porting_register_hal_funcs(sio_bus_t bus)
{
    hal_sio_register_funcs(bus, hal_sio_v151_funcs_get());
}

void sio_porting_unregister_hal_funcs(sio_bus_t bus)
{
    hal_sio_unregister_funcs(bus);
}

void sio_porting_register_irq(sio_bus_t bus)
{
    osal_irq_request(g_sio_interrupt_lines[bus].irq_num, g_sio_interrupt_lines[bus].irq_func, NULL, NULL, NULL);
    osal_irq_enable(g_sio_interrupt_lines[bus].irq_num);
}

void sio_porting_unregister_irq(sio_bus_t bus)
{
    osal_irq_disable(g_sio_interrupt_lines[bus].irq_num);
}

void irq_sio0_handler(void)
{
    hal_sio_v151_irq_handler(SIO_BUS_0);
}

void sio_porting_clock_enable(bool enable)
{
    if (enable) {
        uapi_reg_setbit(M_CTL_RB_BASE + M_CLKEN0, I2S_CLKEN_BIT);
    } else {
        uapi_reg_clrbit(M_CTL_RB_BASE + M_CLKEN0, I2S_CLKEN_BIT);
    }
}

uint32_t sio_porting_get_bclk_div_num(uint8_t data_width, uint32_t ch)
{
    uint32_t mclk_div_num, s_clk, bclk_div_num, freq_of_need;
    float middle_div;
    s_clk = I2S_S_CLK;
    freq_of_need = FREQ_OF_NEED;
    mclk_div_num = uapi_reg_getbits(M_CTL_RB_BASE + I2S_CR, I2S_CR_DIV_NUM_BIT, I2S_CR_DIV_NUM_LEN);
    if ((mclk_div_num == 0) || (mclk_div_num == 1)) {
        mclk_div_num = I2S_MCLK_DIV;
    }
    middle_div = (s_clk / mclk_div_num) / (freq_of_need * data_width * ch);
    if ((uint32_t)(middle_div * I2S_PARAM + 1) == ((uint32_t)middle_div * I2S_PARAM + 1)) {
        bclk_div_num = (uint32_t)middle_div;
    } else {
        bclk_div_num = (uint32_t)middle_div + 1;
    }
    return bclk_div_num;
}

void sio_porting_i2s_pinmux(void)
{
    uapi_pin_set_mode(S_MGPIO10, HAL_PIO_I2S_SCLK);
    uapi_pin_set_mode(S_MGPIO11, HAL_PIO_I2S_WS);
    uapi_pin_set_mode(S_MGPIO12, HAL_PIO_I2S_DOUT);
    uapi_pin_set_mode(S_MGPIO13, HAL_PIO_I2S_DIN);
}
