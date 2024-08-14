/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description:  RISCV31 MPU DRIVER.
 * Author: @CompanyNameTag
 * Create:  2021-10-10
 */

#include <stdint.h>
#include "arch_encoding.h"
#include "arch_barrier.h"
#include "mpu.h"

#define MEMXATTR_VECTOR         2
#define MEMXATTR_NUM_PER_REG    8
#define PMPADDR_RIGHT_SHIFT_BIS 2
#define PMP_REGION_NUM          16

/*lint -e40 -e701 -e718 -e732 -e746*/
static void set_pmpxcfg(uint8_t idx, pmpxcfg_t cfg)
{
    union rv_csr_pmpcfg pmpcfg;

    if (idx >= PMP_REGION_NUM) {
        return;
    }

    /* Because like that pmpaddr0 is RISCV CSR, so cannot be performed using table lookup mode. */
    switch (idx / PMPXCFG_NUM_PER_REG) {
        case PMPCFG0:
            pmpcfg.value = read_csr(pmpcfg0);
            pmpcfg.cfg[idx % PMPXCFG_NUM_PER_REG] = cfg;
            write_csr(pmpcfg0, pmpcfg.value);
            break;
        case PMPCFG1:
            pmpcfg.value = read_csr(pmpcfg1);
            pmpcfg.cfg[idx % PMPXCFG_NUM_PER_REG] = cfg;
            write_csr(pmpcfg1, pmpcfg.value);
            break;
        case PMPCFG2:
            pmpcfg.value = read_csr(pmpcfg2);
            pmpcfg.cfg[idx % PMPXCFG_NUM_PER_REG] = cfg;
            write_csr(pmpcfg2, pmpcfg.value);
            break;
        case PMPCFG3:
            pmpcfg.value = read_csr(pmpcfg3);
            pmpcfg.cfg[idx % PMPXCFG_NUM_PER_REG] = cfg;
            write_csr(pmpcfg3, pmpcfg.value);
            break;
        default:
            break;
    }
}

static void set_memxattr(uint8_t idx, uint8_t attr)
{
    uint32_t memattr;
    uint8_t idx_temp;
    if (idx >= PMP_REGION_NUM) {
        return;
    }

    if (idx < MEMXATTR_NUM_PER_REG) {
        memattr = read_custom_csr(MEMATTRL);
        idx_temp = (uint8_t)(idx << MEMXATTR_VECTOR);
        memattr = (memattr & ~(0xFUL << idx_temp)) | (attr << idx_temp);
        write_custom_csr_val(MEMATTRL, memattr);
    } else {
        memattr = read_custom_csr(MEMATTRH);
        idx_temp = (uint8_t)((idx - MEMXATTR_NUM_PER_REG) << MEMXATTR_VECTOR);
        memattr = (memattr & ~(0xFUL << idx_temp)) | (attr << idx_temp);
        write_custom_csr_val(MEMATTRH, memattr);
    }
}

static void set_pmpxaddr_first_half(uint8_t idx, uint32_t pmpaddr)
{
    /* Because like that pmpaddr0 is RISCV CSR, so cannot be performed using table lookup mode. */
    if (idx == PMPADDR0) {
        write_csr(pmpaddr0, pmpaddr);
    } else if (idx == PMPADDR1) {
        write_csr(pmpaddr1, pmpaddr);
    } else if (idx == PMPADDR2) {
        write_csr(pmpaddr2, pmpaddr);
    } else if (idx == PMPADDR3) {
        write_csr(pmpaddr3, pmpaddr);
    } else if (idx == PMPADDR4) {
        write_csr(pmpaddr4, pmpaddr);
    } else if (idx == PMPADDR5) {
        write_csr(pmpaddr5, pmpaddr);
    } else if (idx == PMPADDR6) {
        write_csr(pmpaddr6, pmpaddr);
    } else if (idx == PMPADDR7) {
        write_csr(pmpaddr7, pmpaddr);
    } else {
        return;
    }
}

static void set_pmpxaddr_second_half(uint8_t idx, uint32_t pmpaddr)
{
    /* Because like that pmpaddr0 is RISCV CSR, so cannot be performed using table lookup mode. */
    if (idx == PMPADDR8) {
        write_csr(pmpaddr8, pmpaddr);
    } else if (idx == PMPADDR9) {
        write_csr(pmpaddr9, pmpaddr);
    } else if (idx == PMPADDR10) {
        write_csr(pmpaddr10, pmpaddr);
    } else if (idx == PMPADDR11) {
        write_csr(pmpaddr11, pmpaddr);
    } else if (idx == PMPADDR12) {
        write_csr(pmpaddr12, pmpaddr);
    } else if (idx == PMPADDR13) {
        write_csr(pmpaddr13, pmpaddr);
    } else if (idx == PMPADDR14) {
        write_csr(pmpaddr14, pmpaddr);
    } else if (idx == PMPADDR15) {
        write_csr(pmpaddr15, pmpaddr);
    } else {
        return;
    }
}

static void set_pmpxaddr(uint8_t idx, uint32_t addr)
{
    uint32_t pmpaddr = addr >> PMPADDR_RIGHT_SHIFT_BIS;

    if (idx <= PMPADDR7) {
        set_pmpxaddr_first_half(idx, pmpaddr);
    } else {
        set_pmpxaddr_second_half(idx, pmpaddr);
    }
}

static void pmp_region_init(mpu_config_t attr)
{
    pmpxcfg_t c;

    set_pmpxaddr(attr.idx, attr.end_addr);
    set_memxattr(attr.idx, attr.memattr);
    c.r = attr.cfg.r;
    c.w = attr.cfg.w;
    c.x = attr.cfg.x;
    c.resv_0 = 0;
    c.a = 0;
    c.l = 0;
    set_pmpxcfg(attr.idx, c);
    dsb();

    /* All other PMP registers are configured before the PMP region is enabled (pmp[n]cfg.A). */
    c.a = attr.cfg.a;
    c.l = attr.cfg.l;
    set_pmpxcfg(attr.idx, c);
}

void mpu_config(mpu_config_t *mpu_cfg, uint16_t mpu_cfg_lenth)
{
    uint8_t i;

    for (i = 0; i < mpu_cfg_lenth; i++) {
        pmp_region_init(mpu_cfg[i]);
    }
    dsb();
}
/*lint +e40 +e701 +e718 +e732 +e746*/