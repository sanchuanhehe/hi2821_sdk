/*
 * Copyright (c) @CompanyNameMagicTag 2018-2023. All rights reserved.
 * Description:   CLOCKS DRIVER.
 * Author: @CompanyNameTag
 * Create:
 */

#include "chip_io.h"
#include "soc_osal.h"
#include "product.h"
#include "tcxo.h"
#include "clocks_switch.h"
#include "pm_clock.h"

static const clocks_clk_cfg_t g_system_clocks_cfg[CLOCKS_CCRG_MODULE_MAX] = {
#if (BS21_DLL2_ENABLE == YES)
    {CLOCKS_CLK_SRC_TCXO_2X, CLOCK_DIV_1},      // CLOCKS_CCRG_MODULE_MCU_CORE
#else
    {CLOCKS_CLK_SRC_TCXO, CLOCK_DIV_1},         // CLOCKS_CCRG_MODULE_MCU_CORE
#endif
    {CLOCKS_CLK_SRC_TCXO, CLOCK_DIV_1},         // CLOCKS_CCRG_MODULE_MCU_PERP_LS
    {CLOCKS_CLK_SRC_TCXO, CLOCK_DIV_1},         // CLOCKS_CCRG_MODULE_MCU_PERP_UART
#if defined(SUPPORT_EXTERN_FLASH)
    {CLOCKS_CLK_SRC_TCXO, CLOCK_DIV_1},         // CLOCKS_CCRG_MODULE_MCU_PERP_SPI
#else
    {CLOCKS_CLK_SRC_NONE, CLOCK_DIV_1},         // CLOCKS_CCRG_MODULE_MCU_PERP_SPI
#endif
#ifdef NFC_TASK_EXIST
    {CLOCKS_CLK_SRC_RC, CLOCK_DIV_2},           // CLOCKS_CCRG_MODULE_NFC_SUB
#else
    {CLOCKS_CLK_SRC_NONE, CLOCK_DIV_2},         // CLOCKS_CCRG_MODULE_NFC_SUB
#endif
    {CLOCKS_CLK_SRC_NONE, CLOCK_DIV_4},         // CLOCKS_CCRG_MODULE_I2S
    {CLOCKS_CLK_SRC_TCXO, CLOCK_DIV_1},         // CLOCKS_CCRG_MODULE_XIP_QSPI
};

static void clocks_32k_sel_config(void)
{
#if (RC_CLOCK_ON == YES)
    writew(0x5702C4C0, 0x1); // rc_32k enable
    writew(0x5702C4B0, 0x0); // xo_32k disable
#else
    writew(0x5702C4B0, 0x1); // xo_32k enable
    writew(0x5702C4C0, 0x0); // rc_32k disable
#endif
}

static void clocks_system_init(void)
{
    uint32_t irq_sts = osal_irq_lock();
    for (uint8_t module = CLOCKS_CCRG_MODULE_MCU_CORE; module < CLOCKS_CCRG_MODULE_MAX; module++) {
        system_ccrg_clock_config(module, g_system_clocks_cfg[module].clk_src, g_system_clocks_cfg[module].clk_div);
    }
    osal_irq_restore(irq_sts);
}

void clocks_hardware_sub_init(void)
{
    writew(0x520003E0, 0x3);    // MSUB_NOR_CFG
    uapi_clock_control(CLOCK_CONTROL_GLB_CLKEN_DISABLE, CLOCK_GLB_CLKEN_USB_PHY);
    uapi_clock_control(CLOCK_CONTROL_GLB_CLKEN_DISABLE, CLOCK_GLB_CLKEN_USB_BUS);
    uapi_clock_control(CLOCK_CONTROL_GLB_CLKEN_DISABLE, CLOCK_GLB_CLKEN_CAN_BUS);
    uapi_clock_control(CLOCK_CONTROL_GLB_CLKEN_DISABLE, CLOCK_GLB_CLKEN_PDM);
}

#if defined(BUILD_APPLICATION_STANDARD)
static void clocks_mclken_config_init(void)
{
    uapi_clock_control(CLOCK_CONTROL_MCLKEN_DISABLE, CLOCK_APERP_MTOP_GLUE_TRIGGER_CLKEN);
    uapi_clock_control(CLOCK_CONTROL_MCLKEN_DISABLE, CLOCK_APERP_PULSE_CAPTURE_CLKEN);
    uapi_clock_control(CLOCK_CONTROL_MCLKEN_DISABLE, CLOCK_APERP_BT_TGTWS_CLKEN);

    uapi_clock_control(CLOCK_CONTROL_MCLKEN_DISABLE, CLOCK_APERP_TRNG_CLKEN);
    uapi_clock_control(CLOCK_CONTROL_MCLKEN_DISABLE, CLOCK_APERP_I2S_CLKEN);
    // SPI.
#if defined(SUPPORT_EXTERN_FLASH)
    uapi_clock_control(CLOCK_CONTROL_MCLKEN_ENABLE, CLOCK_APERP_SPI0_M_CLKEN);
#else
    uapi_clock_control(CLOCK_CONTROL_MCLKEN_DISABLE, CLOCK_APERP_SPI0_M_CLKEN);
#endif
    uapi_clock_control(CLOCK_CONTROL_MCLKEN_DISABLE, CLOCK_APERP_SPI1_M_CLKEN);
    uapi_clock_control(CLOCK_CONTROL_MCLKEN_DISABLE, CLOCK_APERP_SPI2_M_CLKEN);
    // I2C.
    uapi_clock_control(CLOCK_CONTROL_MCLKEN_DISABLE, CLOCK_APERP_I2C0_CLKEN);
    uapi_clock_control(CLOCK_CONTROL_MCLKEN_DISABLE, CLOCK_APERP_I2C1_CLKEN);
}

void clocks_init(void)
{
#if (BS21_DLL2_ENABLE == YES)
    writew(0x52000540, 0x7);
#else
    writew(0x52000540, 0x6);
#endif
    clocks_32k_sel_config();
    clocks_system_init();
    clocks_hardware_sub_init();
    clocks_mclken_config_init();
}
#endif