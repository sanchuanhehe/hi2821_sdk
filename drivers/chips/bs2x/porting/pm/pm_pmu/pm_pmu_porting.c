/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Provides pm pmu port \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-07-29， Create file. \n
 */

#include "chip_io.h"
#include "soc_osal.h"
#include "tcxo.h"
#include "pmu_ldo.h"
#include "pm_pmu.h"
#include "pm_pmu_porting.h"

typedef bool (*pmu_control_func_t)(uint8_t param);
static bool pmu_control_nfcldo_power(pmu_control_power_t power);

static uint8_t g_btrf_need_buck_vset = PMU_BUCK_1P1_VSET_1V1;
static uint8_t g_cldo_need_buck_vset = PMU_BUCK_1P1_VSET_1V1;

static bool pmu_control_nfcldo_power(pmu_control_power_t power)
{
    switch (power) {
        case PMU_CONTROL_POWER_OFF:
        case PMU_CONTROL_POWER_ON:
            pmu_nfcldo_power_cfg((uint8_t)power);
            break;

        default:
            return false;
    }
    return true;
}

static bool pmu_control_flashldo_power(pmu_control_power_t power)
{
    switch (power) {
        case PMU_CONTROL_POWER_OFF:
        case PMU_CONTROL_POWER_ON:
        case PMU_CONTROL_POWER_AUTO:
            pmu_flashldo_power_cfg((uint8_t)power);
            break;

        default:
            return false;
    }
    return true;
}

static bool pmu_control_micldo_power(pmu_control_power_t power)
{
    switch (power) {
        case PMU_CONTROL_POWER_OFF:
        case PMU_CONTROL_POWER_ON:
        case PMU_CONTROL_POWER_AUTO:
            pmu_micldo_power_cfg((uint8_t)power);
            break;

        default:
            return false;
    }
    return true;
}

static const pmu_control_func_t g_pmu_control_funcs[] = {
    pmu_control_flashldo_power,
    pmu_control_nfcldo_power,
    pmu_control_micldo_power,
};

errcode_t uapi_pmu_control(pmu_control_type_t type, uint8_t param)
{
    if (type >= PMU_CONTROL_TYPE_MAX) {
        return ERRCODE_INVALID_PARAM;
    }
    bool ret = false;
    ret = g_pmu_control_funcs[(uint8_t)type](param);
    return (ret == true) ? ERRCODE_SUCC : ERRCODE_FAIL;
}

errcode_t uapi_pmu_ldo_set_voltage(pmu_ldo_id_t id, uint8_t vset)
{
    uint32_t status = osal_irq_lock();
    switch (id) {
        case PMU_LDO_ID_BUCK_1P1:
            if (vset > g_cldo_need_buck_vset) {
                pmu_buck_ldo_vset_cfg(vset);
            } else {
                // 需满足cldo电压需求
                pmu_buck_ldo_vset_cfg(g_cldo_need_buck_vset);
            }
            g_btrf_need_buck_vset = vset;
            break;
        case PMU_LDO_ID_SYSLDO:
            pmu_sysldo_vset_cfg(vset);
            break;
        case PMU_LDO_ID_FLASHLDO:
            pmu_flashldo_vset_cfg(vset);
            break;
        case PMU_LDO_ID_CLDO:
            if (vset >= PMU_CLDO_VSET_1V1) {
                g_cldo_need_buck_vset = PMU_BUCK_1P1_VSET_1V2;
                if (g_cldo_need_buck_vset > g_btrf_need_buck_vset) {
                    // 需抬buck电压
                    pmu_buck_ldo_vset_cfg(g_cldo_need_buck_vset);
                    uapi_tcxo_delay_us(120ULL); // delay 120us
                }
                pmu_cldo_vset_cfg(vset);
            } else {
                pmu_cldo_vset_cfg(vset);
                g_cldo_need_buck_vset = PMU_BUCK_1P1_VSET_1V1;
                if (g_cldo_need_buck_vset <= g_btrf_need_buck_vset) {
                    pmu_buck_ldo_vset_cfg(g_btrf_need_buck_vset);
                }
            }
            break;
        case PMU_LDO_ID_MICLDO:
            pmu_micldo_vset_cfg(vset);
            break;
        case PMU_LDO_ID_NFCLDO:
            pmu_nfcldo_vset_cfg(vset);
            break;
        default:
            osal_irq_restore(status);
            return ERRCODE_FAIL;
    }
    osal_irq_restore(status);
    return ERRCODE_SUCC;
}

uint8_t uapi_pmu_ldo_get_voltage(pmu_ldo_id_t id)
{
    switch (id) {
        case PMU_LDO_ID_BUCK_1P1:
            return pmu_buck_ldo_get_vset();
        case PMU_LDO_ID_SYSLDO:
            return pmu_sysldo_get_vset();
        case PMU_LDO_ID_FLASHLDO:
            return pmu_flashldo_get_vset();
        case PMU_LDO_ID_CLDO:
            return pmu_cldo_get_vset();
        case PMU_LDO_ID_MICLDO:
            return pmu_micldo_get_vset();
        case PMU_LDO_ID_NFCLDO:
            return pmu_nfcldo_get_vset();
        default:
            return 0;
    }
}