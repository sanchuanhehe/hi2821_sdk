/*
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved.
 * Description: diag mem adapt
 * This file should be changed only infrequently and with great care.
 */

#include "diag_cmd_mem_read_write.h"

typedef struct diag_mem_config_t {
    uintptr_t start_addr;
    uintptr_t end_addr;
} diag_mem_config_t;

static const diag_mem_config_t g_mem_config[] = {
    { 0x00010000, 0x0003FFFF }, // MCPU_TCM ROM
    { 0x00040000, 0x00057FFF }, // MCPU_TCM ITCM_MEM
    { 0x20000000, 0x2000FFFF }, // MCPU_TCM DTCM_MEM

    { 0x52000000, 0x5208EFFF }, // MAIN_CFG_APB
    { 0x57000000, 0x57037FFF }, // MCPU AON_AHB

    { 0x58000000, 0x58043FFF }, // USB/CAN/NFC

    { 0x59000000, 0x59003FFF }, // B_CTL

    { 0x59008000, 0x59008FFF }, // B_DIAG

    { 0x59400000, 0x59417FFF }, // BT_SUB

    { 0x90000000, 0x907FFFFF }, // SFC
};

static bool diag_permit_check(uintptr_t start_addr, uintptr_t end_addr)
{
    bool ret = false;
    uint32_t loop;

    for (loop = 0; loop < sizeof(g_mem_config) / sizeof(diag_mem_config_t); loop++) {
        if ((g_mem_config[loop].start_addr <= start_addr) && (g_mem_config[loop].end_addr >= end_addr)) {
            ret = true;
            break;
        }
    }
    return ret;
}

bool diag_cmd_permit_read(uintptr_t start_addr, uintptr_t end_addr)
{
    return diag_permit_check(start_addr, end_addr);
}

bool diag_cmd_permit_write(uintptr_t start_addr, uintptr_t end_addr)
{
    return diag_permit_check(start_addr, end_addr);
}
