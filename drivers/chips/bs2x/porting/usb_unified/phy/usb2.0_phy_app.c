/*
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: USB port for project
 * Author: @CompanyNameTag
 * Create:  2023-01-10
 */

#include "chip_io.h"
#include "clocks_switch.h"
#include "debug_print.h"
#include "usb_phy.h"
#include "tcxo.h"
#include "pm_clock.h"

#define PMU_CTL_SOFT_USB_GLB_RST_ADDR           0x5700478C
#define PMU_CTL_SOFT_USB_GLB_RST_CLDO_N_MSK     2
#define PMU_CTL_SOFT_USB_GLB_RST_AON_LGC_N      1
#define PMU_CTL_SOFT_USB_GLB_RST_AON_CRG_N      0

#define GLB_CTL_D_USB_SOFT_RST_ADDR             0x57000828
#define GLB_CTL_D_USB_SOFT_RST_AHB_N            1
#define GLB_CTL_D_USB_SOFT_RST_UTMI_N           0
#define DELAY_US200                             200

void usb_pd_pwr_up(void)
{
    reg32_setbit(PMU_CTL_SOFT_USB_GLB_RST_ADDR, PMU_CTL_SOFT_USB_GLB_RST_CLDO_N_MSK);
    reg32_setbit(PMU_CTL_SOFT_USB_GLB_RST_ADDR, PMU_CTL_SOFT_USB_GLB_RST_AON_CRG_N);
    reg32_setbit(PMU_CTL_SOFT_USB_GLB_RST_ADDR, PMU_CTL_SOFT_USB_GLB_RST_AON_LGC_N);

    reg32_setbit(GLB_CTL_D_USB_SOFT_RST_ADDR, GLB_CTL_D_USB_SOFT_RST_AHB_N);
    reg32_setbit(GLB_CTL_D_USB_SOFT_RST_ADDR, GLB_CTL_D_USB_SOFT_RST_UTMI_N);
}

#define PMU_CTL_GLB_CLKEN_ADDR                  0x52000548
#define PMU_CTL_GLB_CLKEN_USB_PHY_MSK           8
#define PMU_CTL_USB_PHY_CR_ADDR                 0x52000604
#define CR_DIV_NUM_MSK                          5
#define CR_DIV_NUM_LEN                          5
#define CR_DIV_EN_MSK                           4
#define CR_CLK_SEL_MSK                          1
#define CR_CLK_SEL_LEN                          3
#define CR_CH_ENABLE_MSK                        0
#define USB_PHY_CLK                             2

void clk_usb_phy_cfg(uint32_t clk_src, uint32_t div_num)
{
    /* Close CG. */
    reg32_clrbit(PMU_CTL_GLB_CLKEN_ADDR, PMU_CTL_GLB_CLKEN_USB_PHY_MSK);
    /* Close div_en. */
    reg32_clrbit(PMU_CTL_USB_PHY_CR_ADDR, CR_DIV_EN_MSK);
    /* Close channel enable. */
    reg32_clrbit(PMU_CTL_USB_PHY_CR_ADDR, CR_CH_ENABLE_MSK);
    /* Select clk_source. */
    reg32_setbits(PMU_CTL_USB_PHY_CR_ADDR, CR_CLK_SEL_MSK, CR_CLK_SEL_LEN, clk_src);
    /* Select div_num. */
    if (div_num != 1) {
        reg32_setbits(PMU_CTL_USB_PHY_CR_ADDR, CR_DIV_NUM_MSK, CR_DIV_NUM_LEN, div_num);
        reg32_setbit(PMU_CTL_USB_PHY_CR_ADDR, CR_DIV_EN_MSK);
    }
    /* Open enable and CG. */
    reg32_setbit(PMU_CTL_USB_PHY_CR_ADDR, CR_CH_ENABLE_MSK);
    reg32_setbit(PMU_CTL_GLB_CLKEN_ADDR, PMU_CTL_GLB_CLKEN_USB_PHY_MSK);
}

#define GLB_CTL_D_POR_RESET_ADDR                0x57000900
#define GLB_CTL_D_POR_RESET_SYS_RST_COM_N_MSK   1
#define GLB_CTL_D_POR_RESET_POR_RESET_MSK       0
#define GLB_CTL_D_UTMI_RESET_ADDR               0x57000904
#define GLB_CTL_D_UTMI_RESET_UTMI_RESET_0_MSK   0

#ifdef PRE_FPGA
#define FPGA_CTL_RB_BASE                        0x59004000

#define USB_PHY_RST_REG                         (FPGA_CTL_RB_BASE + 0x300)
#define USB_PHY_RST_PHY_RESETN                  0
#define USB_PHY_RST_RESETN                      4

#define USB_PHY_MODE_REG                        (FPGA_CTL_RB_BASE + 0x304)

#define USB_PHY_OUT_EN_REG                      (FPGA_CTL_RB_BASE + 0x308)
#define USB_PHY_OUT_EN_OUT_ENABLE               0
#endif

void usb_phy_reset(void)
{
#ifdef PRE_FPGA
    writew(USB_PHY_RST_REG, 0);

    writew(USB_PHY_MODE_REG, 0x10);
    writew(USB_PHY_OUT_EN_REG, 0x1);

    reg32_setbit(USB_PHY_RST_REG, USB_PHY_RST_RESETN);
    reg32_setbit(USB_PHY_RST_REG, USB_PHY_RST_PHY_RESETN);
#endif
}

static void usb_phy_power_off(void)
{
    uapi_clock_control(CLOCK_CONTROL_GLB_CLKEN_DISABLE, CLOCK_GLB_CLKEN_USB_PHY);
    uapi_clock_control(CLOCK_CONTROL_GLB_CLKEN_DISABLE, CLOCK_GLB_CLKEN_USB_BUS);
    uapi_clock_control(CLOCK_CONTROL_XO_OUT_DISABLE, CLOCK_XO2USB);
    PRINT("usb_phy_power_off\n");
}

static void usb_phy_power_on(void)
{
    uapi_clock_control(CLOCK_CONTROL_XO_OUT_ENABLE, CLOCK_XO2USB);
    uapi_clock_control(CLOCK_CONTROL_GLB_CLKEN_ENABLE, CLOCK_GLB_CLKEN_USB_BUS);
    uapi_clock_control(CLOCK_CONTROL_GLB_CLKEN_ENABLE, CLOCK_GLB_CLKEN_USB_PHY);
#ifdef PRE_FPGA
    usb_phy_reset();
    usb_pd_pwr_up();
    clk_usb_phy_cfg(CLOCKS_CLK_SRC_TCXO, 2); /* usb phy clk div is 2 */
    uapi_tcxo_delay_us(600); /* usb phy need wait 600 us */
    writel(GLB_CTL_D_POR_RESET_ADDR, 0);
    writel(GLB_CTL_D_UTMI_RESET_ADDR, 0);
    uapi_tcxo_delay_us(600); /* usb phy need wait 600 us */
#else
    writel(0x52000304, 0x0); // usb_can_sel：USB与CAN只有一个能工作
    writel(0x5700478C, 0x0); // USB复位
    writel(0x57004780, 0x3); // USB上电
    uapi_tcxo_delay_us(30); // delay 30us
    writel(0x57004788, 0x0); // USB上电
    while (readl(0x57004784) != 0x1) { }
    writel(0x5700478C, 0x7); // USB解复位
    writel(0x57000910, 0x4); // clk_usb_gt = 0 保持62配置
    writel(0x57000828, 0x2); // soft_usb_ahb_rst_n = 1
    writel(0x57000828, 0x3); // soft_usb_utmi_rst_n = 1
    writel(0x57000900, 0x2); // POR解复位
    writel(0x57000904, 0x0); // utmi解复位
    writel(0x57000824, 0x2); // soft_rst_usb_phy_test_n = 1
    writel(0x57000824, 0x3); // usb_phy_test_clken = 1
    writel(0x57018004, 0x85000000); // usb_phy内部寄存器
    writel(0x57018100, 0x402); // ref_clk选择
#endif
    PRINT("usb_phy_power_on\n");
}

void usb_start_hcd(void)
{
    usb_phy_power_on();
}

void usb_stop_hcd(void)
{
    usb_phy_power_off();
}

static const usb_phy_ops_t g_usb_phy_ops = {
    .usbStartHcd            = usb_start_hcd,
    .usbStopHcd             = usb_stop_hcd,
    .usbHost2Device         = NULL,
    .usbDevice2Host         = NULL,
    .usbSuspend             = NULL,
    .usbResume              = NULL,
};

void usb_phy_init(void)
{
    usb_phy_reg(&g_usb_phy_ops);
}
