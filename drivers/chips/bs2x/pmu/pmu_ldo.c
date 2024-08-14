/*
 * Copyright (c) @CompanyNameMagicTag 2018-2023. All rights reserved.
 * Description:    APPLICATION PMU DRIVER.
 * Author: @CompanyNameTag
 * Create: 2019-12-19
 */
#include "non_os.h"
#include "panic.h"
#include "tcxo.h"
#include "pmu_ldo.h"

#define PMU_BUCK_MIC_LDO_1P1_VSET_REG   0x5702C080
#define PMU_BUCK_LDO_1P1_VSET_BIT       0
#define PMU_BUCK_LDO_1P1_VSET_LEN       5
#define PMU_MICLDO_VSET_BIT             8
#define PMU_MICLDO_VSET_LEN             5
#define PMU_BUCK_MIC_LDO_1P1_VGET_REG   0x5702C060
#define PMU_BUCK_LDO_1P1_VGET_BIT       0
#define PMU_BUCK_LDO_1P1_VGET_LEN       5
#define PMU_MICLDO_VGET_BIT             8
#define PMU_MICLDO_VGET_LEN             5
#define PMU_CLDO_SYSLDO_VSET_REG        0x5702C088
#define PMU_SYSLDO_VSET_BIT             0
#define PMU_SYSLDO_VSET_LEN             4
#define PMU_CLDO_VSET_BIT               4
#define PMU_CLDO_VSET_LEN               4
#define PMU_CLDO_SYSLDO_VGET_REG        0x5702C068
#define PMU_SYSLDO_VGET_BIT             0
#define PMU_SYSLDO_VGET_LEN             4
#define PMU_CLDO_VGET_BIT               4
#define PMU_CLDO_VGET_LEN               4
#define PMU_NFCLDO_VSET_REG             0x5702C230
#define PMU_NFCLDO_VSET_BIT             0
#define PMU_NFCLDO_VSET_LEN             4
#define PMU_NFCLDO_ISO_BIT              8
#define PMU_RF_ISO_BIT                  9
#define PMU_AFE_ISO_BIT                 10
#define PMU_NFCLDO_EN_REG               0x5702C234
#define PMU_NFCLDO_EN_DELAY_BIT         1
#define PMU_NFCLDO_EN_BIT               0
#define PMU_FLASHLDO_VSET_REG           0x5702C220
#define PMU_FLASHLDO_VSET_BIT           0
#define PMU_FLASHLDO_VSET_LEN           4

#define PMU_LDO_EN_CLDO2_DELAY_US       120ULL
#define CHECK_FREQUENCY                 1000
#define PMU_LDO_TIMEOUT                 1000
#define PMU_LDO_UNLOCK_CLEAR_PANIC      1
#define PMU_LDO_DELAY_30_US             30ULL
#define PMU_LDO_DELAY_150_US            150ULL
#define PMU_LDO_DELAY_250_US            250ULL

#define PMU_EN_XLDO_DELAY_TIME              2

#define PMU_AUX_ADC_TRIM_REG                (M_CTL_RB_BASE + 0xB10)
#define PMU_AUXLDO_TRIM_BIT                 12
#define PMU_AUXLDO_TRIM_LEN                 4

#define PMU_XLDO_ICTR_CONFIG                0x2

#define ULP_AON_CTL_MICLDO_MAN_REG          0x5702C204
#define PMU_MICLDO_EN_SEL_BIT               0
#define PMU_MICLDO_EN_MAN_BIT               4
#define PMU_MICLDO_EN_DELAY_MAN_BIT         8

#define ULP_AON_CTL_FLASHLDO_MAN_REG        0x5702C208
#define PMU_FLASHLDO_EN_SEL_BIT             0
#define PMU_FLASHLDO_EN_MAN_BIT             4
#define PMU_FLASHLDO_EN_DELAY_MAN_BIT       8

void pmu_buck_ldo_vset_cfg(uint8_t vset)
{
    reg32_setbits(PMU_BUCK_MIC_LDO_1P1_VSET_REG, PMU_BUCK_LDO_1P1_VSET_BIT, PMU_BUCK_LDO_1P1_VSET_LEN, vset);
}

uint8_t pmu_buck_ldo_get_vset(void)
{
    return reg32_getbits(PMU_BUCK_MIC_LDO_1P1_VGET_REG, PMU_BUCK_LDO_1P1_VGET_BIT, PMU_BUCK_LDO_1P1_VGET_LEN);
}

void pmu_sysldo_vset_cfg(uint8_t vset)
{
    reg32_setbits(PMU_CLDO_SYSLDO_VSET_REG, PMU_SYSLDO_VSET_BIT, PMU_SYSLDO_VSET_LEN, vset);
}

uint8_t pmu_sysldo_get_vset(void)
{
    return reg32_getbits(PMU_CLDO_SYSLDO_VGET_REG, PMU_SYSLDO_VGET_BIT, PMU_SYSLDO_VGET_LEN);
}

void pmu_cldo_vset_cfg(uint8_t vset)
{
    reg32_setbits(PMU_CLDO_SYSLDO_VSET_REG, PMU_CLDO_VSET_BIT, PMU_CLDO_VSET_LEN, vset);
}

uint8_t pmu_cldo_get_vset(void)
{
    return reg32_getbits(PMU_CLDO_SYSLDO_VGET_REG, PMU_CLDO_VGET_BIT, PMU_CLDO_VGET_LEN);
}

void pmu_micldo_vset_cfg(uint8_t vset)
{
    reg32_setbits(PMU_BUCK_MIC_LDO_1P1_VSET_REG, PMU_MICLDO_VSET_BIT, PMU_MICLDO_VSET_LEN, vset);
}

uint8_t pmu_micldo_get_vset(void)
{
    return reg32_getbits(PMU_BUCK_MIC_LDO_1P1_VGET_REG, PMU_MICLDO_VGET_BIT, PMU_MICLDO_VGET_LEN);
}

void pmu_nfcldo_vset_cfg(uint8_t vset)
{
    uint8_t nfcldo_vset = vset;
    // NFCLDO VSET调整，00/10 默认VSET档位，01默认档位-1,11默认档位+1
    uint16_t vset_adj_flag = reg16_getbits(0x5702882C, 0x14, 0x2);
    if ((vset_adj_flag == 0x1) && (nfcldo_vset > 0x0)) {
        nfcldo_vset -= 1;
    } else if ((vset_adj_flag == 0x3) && (nfcldo_vset < 0xF)) {
        nfcldo_vset += 1;
    }
    reg32_setbits(PMU_NFCLDO_VSET_REG, PMU_NFCLDO_VSET_BIT, PMU_NFCLDO_VSET_LEN, nfcldo_vset);
}

uint8_t pmu_nfcldo_get_vset(void)
{
    return reg32_getbits(PMU_NFCLDO_VSET_REG, PMU_NFCLDO_VSET_BIT, PMU_NFCLDO_VSET_LEN);
}

void pmu_nfcldo_power_cfg(uint8_t pwr_ctl)
{
    if (pwr_ctl == PMU_LDO_POWER_ON) {
        reg32_setbit(PMU_NFCLDO_EN_REG, PMU_NFCLDO_EN_BIT);
        uapi_tcxo_delay_us(120ULL);  // delay 120us.
        reg32_setbit(PMU_NFCLDO_EN_REG, PMU_NFCLDO_EN_DELAY_BIT);
    } else if (pwr_ctl == PMU_LDO_POWER_OFF) {
        reg32_clrbit(PMU_NFCLDO_EN_REG, PMU_NFCLDO_EN_BIT);
        reg32_clrbit(PMU_NFCLDO_EN_REG, PMU_NFCLDO_EN_DELAY_BIT);
    }
}

void pmu_flashldo_vset_cfg(uint8_t vset)
{
    reg32_setbits(PMU_FLASHLDO_VSET_REG, PMU_FLASHLDO_VSET_BIT, PMU_FLASHLDO_VSET_LEN, vset);
}

uint8_t pmu_flashldo_get_vset(void)
{
    return reg32_getbits(PMU_FLASHLDO_VSET_REG, PMU_FLASHLDO_VSET_BIT, PMU_FLASHLDO_VSET_LEN);
}

void pmu_flashldo_power_cfg(uint8_t pwr_ctl)
{
    if (pwr_ctl == PMU_LDO_POWER_AUTO) {
        reg16_clrbit(ULP_AON_CTL_FLASHLDO_MAN_REG, PMU_FLASHLDO_EN_SEL_BIT);
    } else if (pwr_ctl == PMU_LDO_POWER_ON) {
        reg16_setbit(ULP_AON_CTL_FLASHLDO_MAN_REG, PMU_FLASHLDO_EN_SEL_BIT);
        reg16_setbit(ULP_AON_CTL_FLASHLDO_MAN_REG, PMU_FLASHLDO_EN_MAN_BIT);
        uapi_tcxo_delay_us(120ULL);  // delay 120us.
        reg16_setbit(ULP_AON_CTL_FLASHLDO_MAN_REG, PMU_FLASHLDO_EN_DELAY_MAN_BIT);
    } else {
        reg16_setbit(ULP_AON_CTL_FLASHLDO_MAN_REG, PMU_FLASHLDO_EN_SEL_BIT);
        reg16_clrbit(ULP_AON_CTL_FLASHLDO_MAN_REG, PMU_FLASHLDO_EN_MAN_BIT);
        reg16_clrbit(ULP_AON_CTL_FLASHLDO_MAN_REG, PMU_FLASHLDO_EN_DELAY_MAN_BIT);
    }
}

void pmu_micldo_power_cfg(uint8_t pwr_ctl)
{
    if (pwr_ctl == PMU_LDO_POWER_AUTO) {
        reg16_clrbit(ULP_AON_CTL_MICLDO_MAN_REG, PMU_MICLDO_EN_SEL_BIT);
    } else if (pwr_ctl == PMU_LDO_POWER_ON) {
        reg16_setbit(ULP_AON_CTL_MICLDO_MAN_REG, PMU_MICLDO_EN_SEL_BIT);
        reg16_setbit(ULP_AON_CTL_MICLDO_MAN_REG, PMU_MICLDO_EN_MAN_BIT);
        uapi_tcxo_delay_us(120ULL);  // delay 120us.
        reg16_setbit(ULP_AON_CTL_MICLDO_MAN_REG, PMU_MICLDO_EN_DELAY_MAN_BIT);
    } else {
        reg16_setbit(ULP_AON_CTL_MICLDO_MAN_REG, PMU_MICLDO_EN_SEL_BIT);
        reg16_clrbit(ULP_AON_CTL_MICLDO_MAN_REG, PMU_MICLDO_EN_MAN_BIT);
        reg16_clrbit(ULP_AON_CTL_MICLDO_MAN_REG, PMU_MICLDO_EN_DELAY_MAN_BIT);
    }
}