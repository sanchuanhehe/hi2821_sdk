/*
 * Copyright (c) @CompanyNameMagicTag 2020-2023. All rights reserved.
 * Description: SYSTEM CLOCKS SWITCH DRIVER
 * Author: @CompanyNameTag
 * Create: 2020-7-28
 */

#include "chip_io.h"
#include "soc_osal.h"
#include "tcxo.h"
#include "clocks_switch.h"

#define GLB_CTL_M_MCPU_CLK_CH_SEL_REG   0x57000100
#define MCU_CORE_CH_SEL_NOR_BIT         0x0
#define MCU_CORE_CH_SEL_SLP_BIT         0x0

#define GLB_CTL_M_MCU_CORE_CR_CH0_REG   0x570000F8
#define GLB_CTL_M_MCU_CORE_CR_CH1_REG   0x570000FC
#define M_CTL_MCU_PERP_LS_CR_REG        0x52000554
#define M_CTL_MCU_PERP_UART_CR_REG      0x52000558
#define M_CTL_MCU_PERP_SPI_CR_REG       0x5200055C
#define M_CTL_XIP_QSPI_CR_REG           0x52000574
#define M_CTL_NFC_SUB_CR_REG            0x52000588
#define M_CTL_I2S_CR_REG                0x52000608
#define CLOCKS_COM_CR_CH_EN_BIT         0
#define CLOCKS_COM_CR_CLK_SEL_BIT       1
#define CLOCKS_COM_CR_CLK_SEL_LEN       3
#define CLOCKS_COM_CR_CLK_DIV_EN_BIT    4
#define CLOCKS_COM_CR_CLK_DIV_NUM_BIT   5
#define CLOCKS_COM_CR_CLK_DIV_NUM_LEN   4
#define CLOCKS_GLB_CLKEN_REG            0x52000548

#define M_CTL_MCU_PERP_BUS_DIV_REG      0x5200007C
#define CLOCKS_MCU_BUS_DIV_NUM_BIT      0
#define CLOCKS_MCU_BUS_DIV_NUM_LEN      4
#define CLOCKS_MCU_BUS_LOAD_DIV_EN_BIT  4

#define M_CTL_M2B_H2H_SEL_REG           0x52000A40

#define PMU1_CTL_CMU_SEL_REG            0x570040F4
#define PMU1_CTL_CMU_MAN_REG            0x570040E0
#define CLOCK_CMU_RC2_CLKEN_BIT         15
#define CLOCK_CMU_RCCLK2_EN_BIT         14
#define CLOCK_CMU_RC2_DBB_PD_BIT        13
#define PMU2_CMU_CTL_RC_CLK_EN_REG      0x5700840C
#define CLOCK_CMU_RCCLK1_EN_BIT         1

typedef enum clocks_glb_clken {
    GLB_CLKEN_MCU_PERP_LS   = 1,
    GLB_CLKEN_CAN_BUS       = 4,
    GLB_CLKEN_NFC_SUB       = 5,
    GLB_CLKEN_XO_CALI       = 6,
    GLB_CLKEN_XIP_QSPI      = 7,
    GLB_CLKEN_USB_PHY       = 8,
    GLB_CLKEN_USB_BUS       = 9,
    GLB_CLKEN_PDM           = 10,
    GLB_CLKEN_I2S           = 11,
} clocks_glb_clken_t;

static uint32_t g_ccrg_modules_reg_map[CLOCKS_CCRG_MODULE_MAX] = {
    GLB_CTL_M_MCU_CORE_CR_CH1_REG,
    M_CTL_MCU_PERP_LS_CR_REG,
    M_CTL_MCU_PERP_UART_CR_REG,
    M_CTL_MCU_PERP_SPI_CR_REG,
    M_CTL_NFC_SUB_CR_REG,
    M_CTL_I2S_CR_REG,
    M_CTL_XIP_QSPI_CR_REG,
};

clocks_clk_cfg_t g_current_clocks[CLOCKS_CCRG_MODULE_MAX] = { 0 };

static void clocks_rc2_clk_config(bool turn_on)
{
    // 手动关闭：pd = 1, en = 0
    reg16_setbits(PMU1_CTL_CMU_MAN_REG, CLOCK_CMU_RC2_DBB_PD_BIT, 0x3, 0x1);
    // 手动控制
    reg16_setbits(PMU1_CTL_CMU_SEL_REG, CLOCK_CMU_RC2_DBB_PD_BIT, 0x3, 0x7);
    if (turn_on) {
        uapi_tcxo_delay_us(30);     // delay 30us
        reg16_clrbit(PMU1_CTL_CMU_MAN_REG, CLOCK_CMU_RC2_DBB_PD_BIT);
        uapi_tcxo_delay_us(30);     // delay 30us
        reg16_setbit(PMU1_CTL_CMU_MAN_REG, CLOCK_CMU_RCCLK2_EN_BIT);
        uapi_tcxo_delay_us(30);     // delay 30us
        reg16_setbit(PMU1_CTL_CMU_MAN_REG, CLOCK_CMU_RC2_CLKEN_BIT);
        uapi_tcxo_delay_us(30);     // delay 30us
        // RC振荡器2到 DBB输出时钟1使能: 1：nomal; 0：输出为0。
        reg16_setbit(PMU2_CMU_CTL_RC_CLK_EN_REG, CLOCK_CMU_RCCLK1_EN_BIT);
    } else {
        reg16_clrbit(PMU2_CMU_CTL_RC_CLK_EN_REG, CLOCK_CMU_RCCLK1_EN_BIT);
    }
}

static void clocks_ccrg_common_config_process(clocks_ccrg_module_t module, clocks_clk_src_t clk_src, uint8_t clk_div)
{
    uint32_t reg_addr = g_ccrg_modules_reg_map[module];
    // Disable the clock.
    reg16_clrbit(reg_addr, CLOCKS_COM_CR_CH_EN_BIT);
    if  (clk_src == CLOCKS_CLK_SRC_NONE) {
        if (module == CLOCKS_CCRG_MODULE_NFC_SUB) {
            clocks_rc2_clk_config(false);
        }
        return;
    }
    if (module == CLOCKS_CCRG_MODULE_NFC_SUB) {
        clocks_rc2_clk_config(true);
    }
    // Select the clock source.
    reg16_setbits(reg_addr, CLOCKS_COM_CR_CLK_SEL_BIT, CLOCKS_COM_CR_CLK_SEL_LEN, clk_src);
    // Set the clock divisor.
    reg16_clrbit(reg_addr, CLOCKS_COM_CR_CLK_DIV_EN_BIT);
    if (clk_div > CLOCK_DIV_1) {
        reg16_setbits(reg_addr, CLOCKS_COM_CR_CLK_DIV_NUM_BIT, CLOCKS_COM_CR_CLK_DIV_NUM_LEN, clk_div);
        reg16_setbit(reg_addr, CLOCKS_COM_CR_CLK_DIV_EN_BIT);
    } else {
        reg16_clrbit(reg_addr, CLOCKS_COM_CR_CLK_DIV_EN_BIT);
    }
    // Enable the clock.
    reg16_setbit(reg_addr, CLOCKS_COM_CR_CH_EN_BIT);
}

static void clocks_glb_clken_config(clocks_ccrg_module_t module, bool on)
{
    uint8_t pos;
    switch (module) {
        case CLOCKS_CCRG_MODULE_MCU_PERP_LS:
            pos = GLB_CLKEN_MCU_PERP_LS;
            break;
        case CLOCKS_CCRG_MODULE_NFC_SUB:
            pos = GLB_CLKEN_NFC_SUB;
            break;
        case CLOCKS_CCRG_MODULE_I2S:
            pos = GLB_CLKEN_I2S;
            break;
        case CLOCKS_CCRG_MODULE_XIP_QSPI:
            pos = GLB_CLKEN_XIP_QSPI;
            break;
        default:
            return;
    }

    if (on) {
        reg16_setbit(CLOCKS_GLB_CLKEN_REG, pos);
    } else {
        reg16_clrbit(CLOCKS_GLB_CLKEN_REG, pos);
    }
}

static void clocks_ccrg_mcu_perp_bus_div_config(uint8_t div)
{
    uint8_t clk_div = ((div == CLOCK_DIV_0) ? CLOCK_DIV_1 : div);
    reg16_clrbit(M_CTL_MCU_PERP_BUS_DIV_REG, CLOCKS_MCU_BUS_LOAD_DIV_EN_BIT);
    reg16_setbits(M_CTL_MCU_PERP_BUS_DIV_REG, CLOCKS_MCU_BUS_DIV_NUM_BIT, CLOCKS_MCU_BUS_DIV_NUM_LEN, clk_div);
    reg16_setbit(M_CTL_MCU_PERP_BUS_DIV_REG, CLOCKS_MCU_BUS_LOAD_DIV_EN_BIT);
}

static void clocks_ccrg_clk_src_config(clocks_ccrg_module_t module, clocks_clk_src_t clk_src, uint8_t clk_div)
{
    if (module == CLOCKS_CCRG_MODULE_MCU_CORE) {
        // Switch ch0.
        reg16_clrbit(GLB_CTL_M_MCPU_CLK_CH_SEL_REG, MCU_CORE_CH_SEL_NOR_BIT);
        // Config the clock.
        clocks_ccrg_common_config_process(module, clk_src, clk_div);
        if (clk_src == CLOCKS_CLK_SRC_TCXO_2X) {
            writew(M_CTL_M2B_H2H_SEL_REG, 0x0); // 0：选择2:1的桥
            // clk_mcu_perp_bus固定32M
            clocks_ccrg_mcu_perp_bus_div_config(0x2);
        } else {
            writew(M_CTL_M2B_H2H_SEL_REG, 0x1); // 1：选择1:1、1:2、1:4的桥
            // clk_mcu_perp_bus固定32M
            clocks_ccrg_mcu_perp_bus_div_config(0x1);
        }
        // Switch ch1.
        reg16_setbit(GLB_CTL_M_MCPU_CLK_CH_SEL_REG, MCU_CORE_CH_SEL_NOR_BIT);
    } else {
        // Disable the clock.
        clocks_glb_clken_config(module, false);
        clocks_ccrg_common_config_process(module, clk_src, clk_div);
        if (clk_src != CLOCKS_CLK_SRC_NONE) {
            // Enable the clock.
            clocks_glb_clken_config(module, true);
        }
    }
}

void system_ccrg_clock_config(clocks_ccrg_module_t module, clocks_clk_src_t clk_src, uint8_t clk_div)
{
    if (module >= CLOCKS_CCRG_MODULE_MAX) {
        return;
    }
    if ((g_current_clocks[module].clk_src == clk_src && g_current_clocks[module].clk_div == clk_div)) {
        return;
    }

    uint32_t status = osal_irq_lock();
    clocks_ccrg_clk_src_config(module, clk_src, clk_div);
    g_current_clocks[module].clk_src = clk_src;
    g_current_clocks[module].clk_div = clk_div;
    osal_irq_restore(status);
}

uint32_t clocks_get_module_frequency(clocks_ccrg_module_t module)
{
    clocks_clk_src_t clk_src = g_current_clocks[module].clk_src;
    uint8_t clk_div = g_current_clocks[module].clk_div;
    uint32_t clk_freq = 0;
    switch (clk_src) {
        case CLOCKS_CLK_SRC_RC:
            clk_freq = RC_CLK;
            break;
        case CLOCKS_CLK_SRC_32K:
            clk_freq = SYSTICK_CLK;
            break;
        case CLOCKS_CLK_SRC_TCXO_2X:
            clk_freq = TCXO_2X_CLK;
            break;
        case CLOCKS_CLK_SRC_TCXO:
            clk_freq = TCXO_1X_CLK;
            break;
        default:
            break;
    }
    if (clk_div > CLOCK_DIV_1) {
        clk_freq = clk_freq / clk_div;
    }
    return clk_freq;
}