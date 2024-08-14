/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides patch driver source \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-09-17, Create file. \n
 */
#include "soc_osal.h"
#include "patch.h"

#define PATCH_FLPCTRL      0xE0000000

#define PATCH_FLPCTRL_OFFSET  0x0
#define PATCH_FLPRMP      (PATCH_FLPCTRL + 0x4)

#define PATCH_FLPLACMP0   (PATCH_FLPCTRL + 0x8)
#define PATCH_FLPLACMP1   (PATCH_FLPCTRL + 0xC)

#define PATCH_FLPIACMP0   (PATCH_FLPCTRL + 0x10)

#define PATCH_FLPCTRL_ENABLE  0x1

#define PATCH_FLPCTRL_PCO_WITHIN_1M  0x0
#define PATCH_FLPCTRL_PCO_LARGER_1M  0x1

#define PATCH_FLPCTRL_WPT_WRITABLE    0x0
#define PATCH_FLPCTRL_WPT_UNWRITABLE  0x1

#define PATCH_FLPCTRL_FLPLACMP0_DISABLE  0x0
#define PATCH_FLPCTRL_FLPLACMP1_DISABLE  0x0

#define PATCH_FP_CMP_CTRL_INDEX  0
#define PATCH_FP_CMP_REMAP_INDEX 1
#define PATCH_FP_CMP_COUNT_INDEX 2
#define PATCH_FP_CMP_MATCH_INDEX 3

typedef enum {
    GEN_BIT0 = 0,
    WPT_BIT1 = 1,
    PCO_BIT2 = 2,
    LACEN0_BIT3 = 3,
    LACEN1_BIT4 = 4,
} sema_enum;

void patch_init(riscv_cfg_t patch_cfg)
{
    reg_setbit(PATCH_FLPCTRL, PATCH_FLPCTRL_OFFSET, GEN_BIT0);
    reg_clrbit(PATCH_FLPCTRL, PATCH_FLPCTRL_OFFSET, WPT_BIT1);

    if (patch_cfg.off_region == true) {
        reg_setbit(PATCH_FLPCTRL, PATCH_FLPCTRL_OFFSET, PCO_BIT2);
    } else {
        reg_clrbit(PATCH_FLPCTRL, PATCH_FLPCTRL_OFFSET, PCO_BIT2);
    }

    for (uint32_t loop = 0; loop < PATCH_CMP_REG_NUM; loop++) {
        *((uint32_t *)PATCH_FLPIACMP0 + loop) =
            *((uint32_t *)(uintptr_t)((void *)patch_cfg.cmp_start_addr) +
            PATCH_FP_CMP_MATCH_INDEX + loop);
    }

    writel(PATCH_FLPRMP, (uint32_t)(uintptr_t)((void *)patch_cfg.remap_addr));
    reg_setbit(PATCH_FLPCTRL, PATCH_FLPCTRL_OFFSET, WPT_BIT1);
    osal_dsb();
}