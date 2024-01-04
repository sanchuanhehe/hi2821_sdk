/*
 * Copyright (c) @CompanyNameMagicTag 2019-2020. All rights reserved.
 * Description:   BT LPC CORE SPECIFIC FUNCTIONS
 * Author: @CompanyNameTag
 * Create: 2019-12-19
 */

#include "ulp_aon_control.h"
#include "core.h"
#include "hal_reg_config.h"
#include "tcxo.h"

#define ULP_AON_CLK_32K_SEL_CFG_REG             (ULP_AON_CTL_RB_ADDR + 0x28)

#define ULP_AON_VDD1P9_SW_OFF_CFG_REG           (ULP_AON_CTL_RB_ADDR + 0x100)
#define VDD1P9_SW_OFF_MANUAL_BIT                0
#define VDD1P9_SW_OFF_SELECT_BIT                4
#define VDD1P9_SW_OFF_STATUS_BIT                8

#define ULP_AON_VDD0P7_TO_SYS_ISO_EN_CFG_REG    (ULP_AON_CTL_RB_ADDR + 0x108)
#define VDD0P7_TO_SYS_ISO_EN_MANUAL_BIT         0
#define VDD0P7_TO_SYS_ISO_EN_SELECT_BIT         4
#define VDD0P7_TO_SYS_ISO_EN_STATUS_BIT         8

#define ULP_SLEEP_N_CFG             (ULP_AON_CTL_RB_ADDR + 0x104)
#define ULP_SLEEP_N_MAN_BIT         0
#define ULP_SLEEP_N_SEL_BIT         4
#define ULP_SLEEP_N_STS_BIT         8

#define ULP_AON_EN_REF_BG1_CFG_REG  (ULP_AON_CTL_RB_ADDR + 0x10C)
#define EN_REF_BG1_MANUAL_BIT       0
#define EN_REF_BG1_SELECT_BIT       4
#define EN_REF_BG1_STATUS_BIT       8

#define ULP_RST_BOOT                (ULP_AON_CTL_RB_ADDR + 0x110)
#define ULP_SLP_EVT_STS             (ULP_AON_CTL_RB_ADDR + 0x1c8)
#define ULP_SLP_EVT_CLR             (ULP_AON_CTL_RB_ADDR + 0x1cc)
#define ULP_WKUP_EVT_STS            (ULP_AON_CTL_RB_ADDR + 0x1e8)
#define ULP_WKUP_EVT_CLR            (ULP_AON_CTL_RB_ADDR + 0x1ec)

#define ULP_SLP_EN                  (ULP_AON_CTL_RB_ADDR + 0x1d0)
#define ULP_WKUP_EN                 (ULP_AON_CTL_RB_ADDR + 0x1f0)
#define ULP_CAPSENS_WKUP_EN_BIT     2
#define ULP_GPIO_WKUP_EN_BIT        1
#define ULP_AON_WKUP_WKUP_EN_BIT    0
#define ULP_AON_RST_BOOT_SEL_BIT    4
#define ULP_AON_RST_BOOT_MANUAL_BIT 0

#define ULP_AON_PAD_ULPON_PE_CFG    (ULP_AON_CTL_RB_ADDR + 0x2a0)
#define ULP_AON_PAD_ULPON_PS_CFG    (ULP_AON_CTL_RB_ADDR + 0x2a4)
#define ULP_AON_PAD_RTC_CLK_BIT     0
#define ULP_AON_PAD_PMUIC_IRQ       1
#define ULP_AON_PAD_SYS_RSTN        2
#define ULP_AON_PAD_PWR_HOLD        3
#define ULP_AON_PAD_HRESET          4
#define ULP_AON_PAD_SLEEP_N         5
#define ULP_AON_PAD_ULP_GPIO0       8
#define ULP_AON_PAD_ULP_GPIO1       9
#define ULP_AON_PAD_ULP_GPIO2       10
#define ULP_AON_PAD_ULP_GPIO3       11

#define ULP_AON_PMU_ISO_MAN_REG     (ULP_AON_CTL_RB_ADDR + 0x410)
#define ULP_AON_PMU_ISO_SEL_REG     (ULP_AON_CTL_RB_ADDR + 0x414)
#define ULP_AON_PMU_ISO_STS_REG     (ULP_AON_CTL_RB_ADDR + 0x418)
#define PMU_CODEC_ISO_N_BIT         0
#define PMU_CMU_IS_N_BIT            1
#define PMU2_ISO_N_BIT              2
#define PMU1_ISO_N_BIT              3

#define ULP_GLB_MEM_PWR_FORCE_ON    (ULP_AON_CTL_RB_ADDR + 0x500)
#define ULP_GLB_MEM_PWR_BIT         0

void ulp_aon_sleep_n_pad_manual_control(switch_type_t sleep_n_pull)
{
    if (sleep_n_pull == TURN_ON) {
        reg16_setbit(ULP_SLEEP_N_CFG, ULP_SLEEP_N_MAN_BIT); // Pull up sleep_n
    } else {
        reg16_clrbit(ULP_SLEEP_N_CFG, ULP_SLEEP_N_MAN_BIT); // Pull down sleep_n
    }
    reg16_setbit(ULP_SLEEP_N_CFG, ULP_SLEEP_N_SEL_BIT); // Set sleep_N manual config
}

void ulp_en_ref_bg1_manual_config(switch_type_t ref_bg1_manual)
{
    if (ref_bg1_manual == TURN_ON) {
        reg16_setbit(ULP_AON_EN_REF_BG1_CFG_REG, EN_REF_BG1_MANUAL_BIT);
    } else {
        reg16_clrbit(ULP_AON_EN_REF_BG1_CFG_REG, EN_REF_BG1_MANUAL_BIT);
    }
}

void ulp_en_ref_bg1_select_config(switch_type_t ref_bg1_manual)
{
    if (ref_bg1_manual == TURN_ON) {
        reg16_setbit(ULP_AON_EN_REF_BG1_CFG_REG, EN_REF_BG1_SELECT_BIT);
    } else {
        reg16_clrbit(ULP_AON_EN_REF_BG1_CFG_REG, EN_REF_BG1_SELECT_BIT);
    }
}

void ulp_vdd1p9_sw_off_manual_config(switch_type_t man_switch)
{
    hal_reg_config_bit(ULP_AON_VDD1P9_SW_OFF_CFG_REG, man_switch, (REG16_POS)VDD1P9_SW_OFF_MANUAL_BIT);
}

void ulp_vdd1p9_sw_off_select_config(switch_type_t sel_switch)
{
    hal_reg_config_bit(ULP_AON_VDD1P9_SW_OFF_CFG_REG, sel_switch, (REG16_POS)VDD1P9_SW_OFF_SELECT_BIT);
}

void ulp_vdd0p7_to_sys_iso_en_manual_config(switch_type_t man_switch)
{
    hal_reg_config_bit(ULP_AON_VDD0P7_TO_SYS_ISO_EN_CFG_REG, man_switch, (REG16_POS)VDD0P7_TO_SYS_ISO_EN_MANUAL_BIT);
}

void ulp_vdd0p7_to_sys_iso_en_select_config(switch_type_t sel_switch)
{
    hal_reg_config_bit(ULP_AON_VDD0P7_TO_SYS_ISO_EN_CFG_REG, sel_switch, (REG16_POS)VDD0P7_TO_SYS_ISO_EN_SELECT_BIT);
}

void pmu_iso_manual_config(pmu_iso_man_sel_t pmu_iso_man, switch_type_t manual_switch)
{
    if (manual_switch == TURN_ON) {
        reg16_setbit(ULP_AON_PMU_ISO_MAN_REG, pmu_iso_man);
    } else {
        reg16_clrbit(ULP_AON_PMU_ISO_MAN_REG, pmu_iso_man);
    }
}

void pmu_iso_select_config(pmu_iso_man_sel_t pmu_iso_sel, switch_type_t select_switch)
{
    if (select_switch == TURN_ON) {
        reg16_setbit(ULP_AON_PMU_ISO_SEL_REG, pmu_iso_sel);
    } else {
        reg16_clrbit(ULP_AON_PMU_ISO_SEL_REG, pmu_iso_sel);
    }
}

static void ulp_sleep_event_clear(void)
{
    writew(ULP_SLP_EVT_CLR, 0xFFFF); // Clear ULP sleep event status
}

static void ulp_wakeup_event_clear(void)
{
    writew(ULP_WKUP_EVT_CLR, 0xFFFF); // Clear ULP wakeup event status
}

void ulp_aon_before_enter_wfi_handle(void)
{
    ulp_sleep_event_clear();
    ulp_wakeup_event_clear();
}

void ulp_aon_clk_32k_sel(ulp_32k_sel_t sel)
{
    writew(ULP_AON_CLK_32K_SEL_CFG_REG, sel); // ulp clk 32k set from aon sub
}

#if CORE == MASTER_BY_ALL
static void ulp_aon_sleep_en(switch_type_t sleep_en)
{
    if (sleep_en == TURN_ON) {
        // ULP AON sleep enable
        reg16_setbit(ULP_SLP_EN, 0);
    } else {
        // ULP AON sleep mask
        reg16_clrbit(ULP_SLP_EN, 0);
    }
}

static void ulp_aon_wakeup_en(switch_type_t wakeup_en)
{
    uint16_t ulp_aon_wkup_en_config = BIT(ULP_CAPSENS_WKUP_EN_BIT) | \
                                    BIT(ULP_GPIO_WKUP_EN_BIT) | \
                                    BIT(ULP_AON_WKUP_WKUP_EN_BIT);
    if (wakeup_en == TURN_ON) {
        // ULP AON wakeup enable
        writew(ULP_WKUP_EN, ulp_aon_wkup_en_config);
    } else {
        // ULP AON wakeup mask
        writew(ULP_WKUP_EN, 0);
    }
}

static void ulp_aon_rst_boot_sel(switch_type_t on)
{
    if (on == TURN_ON) {
        reg16_setbit(ULP_RST_BOOT, ULP_AON_RST_BOOT_SEL_BIT);
    } else {
        reg16_clrbit(ULP_RST_BOOT, ULP_AON_RST_BOOT_SEL_BIT);
    }
}

static void ulp_aon_rst_boot_manual(switch_type_t on)
{
    if (on == TURN_ON) {
        reg16_setbit(ULP_RST_BOOT, ULP_AON_RST_BOOT_MANUAL_BIT);
    } else {
        reg16_clrbit(ULP_RST_BOOT, ULP_AON_RST_BOOT_MANUAL_BIT);
    }
}

void ulp_aon_init(void)
{
    // ULP AON sleep en
    ulp_aon_sleep_en(TURN_ON);
    // ULP AON wakeup en
    ulp_aon_wakeup_en(TURN_ON);

    ulp_vdd1p9_sw_off_manual_config(TURN_OFF);
    ulp_vdd1p9_sw_off_select_config(TURN_ON);
    ulp_vdd0p7_to_sys_iso_en_manual_config(TURN_OFF);
    ulp_vdd0p7_to_sys_iso_en_select_config(TURN_ON);
    ulp_en_ref_bg1_manual_config(TURN_ON);
    ulp_en_ref_bg1_select_config(TURN_ON);

    // Clamp digital circuit reset
    ulp_aon_rst_boot_select();
}

void ulp_aon_ship_mode_cfg(bool en_ship_mode)
{
    if (en_ship_mode) {
        ulp_vdd1p9_sw_off_select_config(TURN_OFF);
        ulp_en_ref_bg1_select_config(TURN_OFF);
        ulp_vdd0p7_to_sys_iso_en_manual_config(TURN_OFF);
        ulp_vdd0p7_to_sys_iso_en_select_config(TURN_ON);
        // Config digital circuit auto reset
        ulp_aon_rst_boot_sel(TURN_OFF);
    } else {
        ulp_vdd1p9_sw_off_manual_config(TURN_OFF);
        ulp_vdd1p9_sw_off_select_config(TURN_ON);
        ulp_vdd0p7_to_sys_iso_en_manual_config(TURN_OFF);
        ulp_vdd0p7_to_sys_iso_en_select_config(TURN_ON);
        ulp_en_ref_bg1_manual_config(TURN_ON);
        ulp_en_ref_bg1_select_config(TURN_ON);

        // Clamp digital circuit reset
        ulp_aon_rst_boot_select();
    }
}

void ulp_aon_rst_boot_select(void)
{
    ulp_aon_rst_boot_manual(TURN_ON); // Digital circuit dereset
    ulp_aon_rst_boot_sel(TURN_ON); // Config digital circuit manual reset
}

void ulp_glb_mem_pwr_force_on_control(switch_type_t mem_pwr_on)
{
    hal_reg_config_bit(ULP_GLB_MEM_PWR_FORCE_ON, mem_pwr_on, (REG16_POS)ULP_GLB_MEM_PWR_BIT);
}
#endif
