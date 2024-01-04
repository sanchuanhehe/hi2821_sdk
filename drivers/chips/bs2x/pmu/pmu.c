/*
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description:  PMU DRIVER
 * Author: @CompanyNameTag
 * Create:  2023-11-06
 */

#include "non_os.h"
#include "chip_io.h"
#include "tcxo.h"
#if defined(BUILD_APPLICATION_STANDARD)
#include "pm_pmu.h"
#endif
#include "pmu.h"

#ifdef BUILD_FLASHBOOT
static void clock_protocol_rst(void)
{
    writel(0x570000B8, 0x0); // B_SOC_SUB初始化
    writel(0x570000B8, 0x1);
    writel(0x570000B8, 0x3);
}

void pmu_init(void)
{
    if (non_os_is_driver_initialised(DRIVER_INIT_PMU) == true) {
        return;
    }
    // buck效率优化配置
    writew(0x5702C410, 0x10);   // BUCK_CFG_REG_4
    writew(0x5702C414, 0xB0);   // BUCK_CFG_REG_5
    writew(0x5702C404, 0x321);  // BUCK_CFG_REG_1
    writew(0x5702C40C, 0x180);  // BUCK_CFG_REG_3

    writel(0x57008410, 0x40);   // XOCLKOUTDIE(S_MGPIO25): 寄存器控制关闭
    clock_protocol_rst();
}
#else /* IF BUILD_APPLICATION_STANDARD */
void pmu_init(void)
{
    if (non_os_is_driver_initialised(DRIVER_INIT_PMU) == true) {
        return;
    }

    // ADC、NFC、RF的LDO ISO，开启CLDO时打开即可。
    reg16_setbits(0x5702C230, 0x8, 0x3, 0x7);
#if (BS21_DLL2_ENABLE == YES)
    uapi_pmu_ldo_set_voltage(PMU_LDO_ID_BUCK_1P1, PMU_BUCK_1P1_VSET_1V2);
    uapi_tcxo_delay_us(120ULL);
    uapi_pmu_ldo_set_voltage(PMU_LDO_ID_CLDO, PMU_CLDO_VSET_1V1);
#else
    uapi_pmu_ldo_set_voltage(PMU_LDO_ID_BUCK_1P1, PMU_BUCK_1P1_VSET_1V1);
    uapi_tcxo_delay_us(120ULL);
    uapi_pmu_ldo_set_voltage(PMU_LDO_ID_CLDO, PMU_CLDO_VSET_1V0);
#endif

#ifdef NFC_TASK_EXIST
    uapi_pmu_ldo_set_voltage(PMU_LDO_ID_NFCLDO, PMU_NFCLDO_VSET_1V2);
    uapi_pmu_control(PMU_CONTROL_NFCLDO_POWER, PMU_CONTROL_POWER_ON);
#endif
    uapi_pmu_control(PMU_CONTROL_MICLDO_POWER, PMU_CONTROL_POWER_OFF);

    return;
}
#endif /* BUILD_APPLICATION_STANDARD END */
