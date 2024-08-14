/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2023. All rights reserved.
 * Description : LiteOS PMP Module Headfile.
 * Author: Huawei LiteOS Team
 * Create : 2022-12-20
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ---------------------------------------------------------------------------- */

/**
 * @defgroup pmp
 * @ingroup kernel
 */

#ifndef _ARCH_PMP_H
#define _ARCH_PMP_H

#ifndef __ASSEMBLER__
#include "arch/regs.h"
#include "los_base.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Locking PMP entry. */
#define PMP_RGN_LOCK                0x80

#ifdef LOSCFG_ARCH_LINX_M
#define LOSCFG_PMPCFG_MAX_SUPPORT   8
#define LOSCFG_PMP_MAX_SUPPORT      32
#else
#define LOSCFG_PMPCFG_MAX_SUPPORT   4
#define LOSCFG_PMP_MAX_SUPPORT      16
#endif
#define LOSCFG_PMP_MIN_ADDRESS      0x00000000UL
/*
 * The physical address space is 32-bits.
 * To follow the RISC-V spec the address registers allow up to bit 33 to be programmed,
 * representing a 16GB address space.
 * TOR : the last entry baseaddr can be set 4G value.  Pmpaddr_i-1 <= addr < Pmpaddr_i
 * NAPOT : Pmpaddr_i <= addr < Pmpaddr_i + size
 */
#define LOSCFG_PMP_MAX_ADDRESS      0xFFFFFFFFUL

/* Address Matching. */
#define PMP_RGN_ADDR_MATCH_OFF      0    /* Null region (disabled) */
#define PMP_RGN_ADDR_MATCH_TOR      0x8  /* Top of range */
#define PMP_RGN_ADDR_MATCH_NA4      0x10 /* Naturally aligned four-byte region */
#define PMP_RGN_ADDR_MATCH_NAPOT    0x18 /* Naturally aligned power-of-two region, >= 8 bytes */
#define PMP_RGN_ADDR_MATCH_MASK     0x18

/* Access permission. */
#define PMPCFG_R_BIT    0
#define PMPCFG_W_BIT    1
#define PMPCFG_X_BIT    2

/* risc-v pmp attr register config */
#ifdef LOSCFG_ARCH_LINX_M
#define MEMATTR0                    0xBC0
#define MEMATTR1                    0xBC1
#define MEMATTR2                    0xBC2
#define MEMATTR3                    0xBC3
#define MEMATTR4                    0xBC4
#define MEMATTR5                    0xBC5
#define MEMATTR6                    0xBC6
#define MEMATTR7                    0xBC7
#define MEMATTR8                    0xBC8
#define MEMATTR9                    0xBC9
#define MEMATTR10                   0xBCA
#define MEMATTR11                   0xBCB
#define MEMATTR12                   0xBCC
#define MEMATTR13                   0xBCD
#define MEMATTR14                   0xBCE
#define MEMATTR15                   0xBCF
#define MEMATTR16                   0xBD0
#define MEMATTR17                   0xBD1
#define MEMATTR18                   0xBD2
#define MEMATTR19                   0xBD3
#define MEMATTR20                   0xBD4
#define MEMATTR21                   0xBD5
#define MEMATTR22                   0xBD6
#define MEMATTR23                   0xBD7
#define MEMATTR24                   0xBD8
#define MEMATTR25                   0xBD9
#define MEMATTR26                   0xBDA
#define MEMATTR27                   0xBDB
#define MEMATTR28                   0xBDC
#define MEMATTR29                   0xBDD
#define MEMATTR30                   0xBDE
#define MEMATTR31                   0xBDF

#define MEMATTR_NS_BIT              9U
#define MEMATTR_ME_BIT              10U
#define MEMATTR_OATTR_BIT           4U
#else
#define MEMATTRL                    0x7D8
#define MEMATTRH                    0x7D9
#define SECTL                       0x7DA
#endif

#define PMPCFG0                     0x3A0
#define PMPCFG1                     0x3A1
#define PMPCFG2                     0x3A2
#define PMPCFG3                     0x3A3
#define PMPCFG4                     0x3A4
#define PMPCFG5                     0x3A5
#define PMPCFG6                     0x3A6
#define PMPCFG7                     0x3A7

#define PMPADDR0                    0x3B0
#define PMPADDR1                    0x3B1
#define PMPADDR2                    0x3B2
#define PMPADDR3                    0x3B3
#define PMPADDR4                    0x3B4
#define PMPADDR5                    0x3B5
#define PMPADDR6                    0x3B6
#define PMPADDR7                    0x3B7
#define PMPADDR8                    0x3B8
#define PMPADDR9                    0x3B9
#define PMPADDR10                   0x3BA
#define PMPADDR11                   0x3BB
#define PMPADDR12                   0x3BC
#define PMPADDR13                   0x3BD
#define PMPADDR14                   0x3BE
#define PMPADDR15                   0x3BF
#define PMPADDR16                   0x3C0
#define PMPADDR17                   0x3C1
#define PMPADDR18                   0x3C2
#define PMPADDR19                   0x3C3
#define PMPADDR20                   0x3C4
#define PMPADDR21                   0x3C5
#define PMPADDR22                   0x3C6
#define PMPADDR23                   0x3C7
#define PMPADDR24                   0x3C8
#define PMPADDR25                   0x3C9
#define PMPADDR26                   0x3CA
#define PMPADDR27                   0x3CB
#define PMPADDR28                   0x3CC
#define PMPADDR29                   0x3CD
#define PMPADDR30                   0x3CE
#define PMPADDR31                   0x3CF

/* risc-v PMP enhancements extension */
#ifdef LOSCFG_ARCH_RISCV_EPMP
/* When mseccfg.MML is set,
 * it now marks a rule as M-mode-only when set and U-mode-only when unset.
 * The formerly reserved encoding of pmpcfg.RW=01, pmpcfg.LRWX=1111, now encode a Shared-Region. */
#define MSECCFG_MML_ENABLE      0x1U

/* When set, it changes the default PMP policy for M-mode
 * when accessing memory regions that don't have a matching PMP rule,
 * to denied instead of ignored. */
#define MSECCFG_MMWP_ENABLE     0x2U

/* It's intended to be used as a debug mechanism,
 * or as a temporary workaround during the boot process for simplifying software,
 * and optimizing the allocation of memory and PMP rules. */
#define MSECCFG_RLB_ENABLE      0x4U

#define MSECFG_MASK             0x7U

#define MSECCFG                 0x390

/* An M-mode-only rule is enforced on Machine mode and denied on User modes */
#define M_MODE_ONLY_RULE             0U

/* A U-mode-only rule is enforced on User mode and denied on Machine mode */
#define U_MODE_ONLY_RULE             1U

/* The formerly reserved encoding of RW=01, and the encoding LRWX=1111, now encode a Shared-Region. */
#define SHARE_DATA_REGION_RULE       2U
#define SHARE_CODE_REGION_RULE       3U

#define UNKNOWN_MARK_RULE            4U
#endif

#ifndef __ASSEMBLER__
enum MemReadAcc {
    E_MEM_RD_ACC_NON_RD = 0,
    E_MEM_RD_ACC_RD = 1,
    E_MEM_U_NR_P_NR = 0,
    E_MEM_U_NR_P_R = 1,
    E_MEM_U_R_P_NR = 2,
    E_MEM_U_R_P_R = 3,
};

enum MemWriteAcc {
    E_MEM_WR_ACC_NON_WR = 0,
    E_MEM_WR_ACC_WR = 1,
    E_MEM_U_NW_P_NW = 0,
    E_MEM_U_NW_P_W = 1,
    E_MEM_U_W_P_NW = 2,
    E_MEM_U_W_P_W = 3,
};

enum MemExecuteAcc {
    E_MEM_EX_ACC_NON_EX = 0,
    E_MEM_EX_ACC_EX = 1,
    E_MEM_U_NX_P_NX = 0,
    E_MEM_U_NX_P_X = 1,
    E_MEM_U_X_P_NX = 2,
    E_MEM_U_X_P_X = 3
};

typedef struct {
    enum MemReadAcc       readAcc;
    enum MemWriteAcc      writeAcc;
    enum MemExecuteAcc    excuteAcc;
} PmpAccInfo;

/* memory attribute config. */
#define MEM_ATTRTYPE_DEV        0
#define MEM_ATTRTYPE_NORMAL     1
#define MEM_ATTRTYPE_UNKNOWN    2

#define MEM_OCHE_ENABLE         1

#define MEM_ACC_U_NA_P_NA       (PmpAccInfo){E_MEM_U_NR_P_NR, E_MEM_U_NW_P_NW,  E_MEM_U_NX_P_NX}

#define MEM_ACC_U_X_P_NA        (PmpAccInfo){E_MEM_U_NR_P_NR, E_MEM_U_NW_P_NW,  E_MEM_U_X_P_NX}
#define MEM_ACC_U_R_P_RW        (PmpAccInfo){E_MEM_U_R_P_R,   E_MEM_U_NW_P_W,   E_MEM_U_NX_P_NX}
#define MEM_ACC_U_RW_P_RW       (PmpAccInfo){E_MEM_U_R_P_R,   E_MEM_U_W_P_W,    E_MEM_U_NX_P_NX}
#define MEM_ACC_U_R_P_NA        (PmpAccInfo){E_MEM_U_R_P_NR,  E_MEM_U_NW_P_NW,  E_MEM_U_NX_P_NX}
#define MEM_ACC_U_RX_P_NA       (PmpAccInfo){E_MEM_U_R_P_NR,  E_MEM_U_NW_P_NW,  E_MEM_U_X_P_NX}
#define MEM_ACC_U_RW_P_NA       (PmpAccInfo){E_MEM_U_R_P_NR,  E_MEM_U_W_P_NW,   E_MEM_U_NX_P_NX}
#define MEM_ACC_U_RWX_P_NA      (PmpAccInfo){E_MEM_U_R_P_NR,  E_MEM_U_W_P_NW,   E_MEM_U_X_P_NX}

#define MEM_ACC_U_NA_P_X        (PmpAccInfo){E_MEM_U_NR_P_NR, E_MEM_U_NW_P_NW,  E_MEM_U_NX_P_X}
#define MEM_ACC_U_X_P_X         (PmpAccInfo){E_MEM_U_NR_P_NR, E_MEM_U_NW_P_NW,  E_MEM_U_X_P_X}
#define MEM_ACC_U_X_P_RX        (PmpAccInfo){E_MEM_U_NR_P_R,  E_MEM_U_NW_P_NW,  E_MEM_U_X_P_X}
#define MEM_ACC_U_NA_P_R        (PmpAccInfo){E_MEM_U_NR_P_R,  E_MEM_U_NW_P_NW,  E_MEM_U_NX_P_NX}
#define MEM_ACC_U_NA_P_RX       (PmpAccInfo){E_MEM_U_NR_P_R,  E_MEM_U_NW_P_NW,  E_MEM_U_NX_P_X}
#define MEM_ACC_U_NA_P_RW       (PmpAccInfo){E_MEM_U_NR_P_R,  E_MEM_U_NW_P_W,   E_MEM_U_NX_P_NX}
#define MEM_ACC_U_R_P_R         (PmpAccInfo){E_MEM_U_R_P_R,   E_MEM_U_NW_P_NW,  E_MEM_U_NX_P_NX}

#ifdef LOSCFG_ARCH_LINX_M
/*
 * In the current IATTR/OATTR configuration, the two memory attribute types:
 * Write-Through Write-Allocate and Write-Back Write-Allocate are not supported compared to the AXI4 protocol definition
*/
typedef enum {
    MEM_ATTR_DEV_NON_BUF = 0,               /* 4b0000, device non-bufferable */
    MEM_ATTR_DEV_BUF = 1,                   /* 4b0001, device bufferable */
    MEM_ATTR_NORM_NON_CA_NON_BUF = 2,       /* 4b0010, normal non-cacheable non-bufferable */
    MEM_ATTR_NORM_NON_CA_BUF = 3,           /* 4b0011, normal non-cacheable bufferable */
    MEM_ATTR_WT_NA = 4,                     /* 4b0100, write-through no-allocate */
    MEM_ATTR_WT_RA = 6,                     /* 4b0110, Write-through read-allocate */
    MEM_ATTR_WB_RD_ALLOC = 7,               /* 4b0111, Write-back read-allocate */
    MEM_ATTR_WB_NA = 8,                     /* 4b1000, Write-back no-allocate */
    MEM_ATTR_WT_RWA = 14,                   /* 4b1110, write-through read and write-allocate */
    MEM_ATTR_WB_RW_ALLOC = 15,              /* 4b1111, Write-back read- and write-allocate */
} PmpMemAttrInfo;
#else
/**
 * memory attribute.
 */
typedef enum {
    MEM_ATTR_DEV_NON_BUF = 0,         /* 4b0000, device non-bufferable */
    MEM_ATTR_NORM_NON_CA_NON_BUF = 2, /* 4b0010, normal non-cacheable non-bufferable */
    MEM_ATTR_NORM_NON_CA_BUF = 3,     /* 4b0011, normal non-cacheable bufferable */
    MEM_ATTR_WB_RD_ALLOC = 7,         /* 4b0111, write-back read-only allocate */
    MEM_ATTR_WB_WR_ALLOC = 11,        /* 4b1011, write-back write-only allocate */
    MEM_ATTR_WB_RW_ALLOC = 15,        /* 4b1111, write-back write and read allocate */
} PmpMemAttrInfo;
#endif

typedef enum {
    SEC_CONTRLO_SECURE_NOMMU = 0,   /* secure and disable SoC IOMMU */
    SEC_CONTRLO_NOSECURE_NOMMU = 1, /* no-secure and disable SoC IOMMU */
    SEC_CONTRLO_SECURE_MMU = 2,     /* secure and enable SoC IOMMU */
    SEC_CONTRLO_NOSECURE_MMU = 3,   /* no-secure and enable SoC IOMMU */
} PmpSeControlInfo;

/**
 * PMP entry info
 * uwBaseAddress must be in the range of RAM, and
 * uwRegionSize granularity must be the multiple of cache line size
 * if address matching mode  equals to NPAOT, the base address and size should match the type and size
 * uwAccessPermission is not arbitrary input
 */
typedef struct {
    /* Index of PMP entry */
    UINT8  ucNumber;

    /* whether lock, Works in any supported mode */
    BOOL  bLocked;

    /*
     * Address matching mode of PMP configuration,
     * Currently supported modes: PMP_RGN_ADDR_MATCH_OFF,
     * PMP_RGN_ADDR_MATCH_TOR and PMP_RGN_ADDR_MATCH_NAPOT.
     */
    UINT8 ucAddressMatch;

    /*
     * access permission of read, write and instruction execution.
     * The changed member is not valid in PMP_RGN_ADDR_MATCH_OFF mode.
     */
    PmpAccInfo accPermission;

    /*
     * set the base address of the protected region,
     * the base address must be in the range of RAM.
     * It should be cache line aligned under the cache.
     */
    UINT32 uwBaseAddress;

    /*
     * PMP entry region size: It is valid only in PMP_RGN_ADDR_MATCH_NAPOT mode,
     * requiring >= 8byte and aligned power-of-2 byte.
     * It should be cache line aligned under the cache.
     */
    UINT32 uwRegionSize;

    /*
     * PMP memory attribute register information:
     * The changed member is not valid in PMP_RGN_ADDR_MATCH_OFF mode.
     */
    PmpMemAttrInfo memAttr;

    /*
     * Security control register information:
     * The changed member is not valid in PMP_RGN_ADDR_MATCH_OFF mode.
     */
    PmpSeControlInfo  seCtl;
} PMP_ENTRY_INFO;
/**
 * @ingroup pmp
 * PMP error code: The pointer to an input parameter is NULL.
 *
 * Value: 0x02001200
 *
 * Solution: Check whether the pointer to the input parameter is usable.
 */
#define LOS_ERRNO_PMP_PTR_NULL                    LOS_ERRNO_OS_ERROR(LOS_MOD_MPU, 0x00)

/**
 * @ingroup pmp
 * PMP error code: The base address is not aligned to the boundary of the region capacity.
 *
 * Value: 0x02001201
 *
 * Solution: Check base address.
 */
#define LOS_ERRNO_PMP_INVALID_BASE_ADDRESS        LOS_ERRNO_OS_ERROR(LOS_MOD_MPU, 0x01)

/**
 * @ingroup pmp
 * PMP error code: Capacity less than cacheline bytes.
 * The PMP entry size granularity must be the multiple of cache line size.
 *
 * Value: 0x02001202
 *
 * Solution: Guaranteed that the capacity is greater than or equal to cacheline, supporting cacheline granularity.
 */
#define LOS_ERRNO_PMP_INVALID_CAPACITY            LOS_ERRNO_OS_ERROR(LOS_MOD_MPU, 0x02)

/**
 * @ingroup pmp
 * PMP error code: Type extension that given is invalid.
 *
 * Value: 0x02001203
 *
 * Solution: memory attribute configuration only support 0000,0010,0011,0111,,1011,1111
 */
#define LOS_ERRNO_PMP_ATTR_INVALID                LOS_ERRNO_OS_ERROR(LOS_MOD_MPU, 0x03)

/**
 * @ingroup pmp
 * PMP error code: Invalid number.
 *
 * Value: 0x02001204
 *
 * Solution: Enter a valid number.
 */
#define LOS_ERRNO_PMP_INVALID_NUMBER              LOS_ERRNO_OS_ERROR(LOS_MOD_MPU, 0x04)

/**
 * @ingroup pmp
 * PMP error code: The next PMP is TOR mode, current pmpcfg match is error.
 *
 * Value: 0x02001205
 *
 * Solution: The next PMP is TOR mode, and when modifying the current pmpcfg,
 * When modifying the current config, if the current pmpcfg original match is
 * PMP_RGN_ADDR_MATCH_TOR or PMP_RGN_ADDR_MATCH_OFF, the modified match must
 * be PMP_RGN_ADDR_MATCH_TOR or PMP_RGN_ADDR_MATCH_OFF.
 */
#define LOS_ERRNO_PMP_TOR_INVALID_MATCH_MODE     LOS_ERRNO_OS_ERROR(LOS_MOD_MPU, 0x05)

/**
 * @ingroup pmp
 * PMP error code: The next PMP is TOR mode, current pmpcfg match is error.
 *
 * Value: 0x02001206
 *
 * Solution: The next PMP is TOR mode, and when modifying the current pmpcfg,
 * When modifying the current config, if the current pmpcfg original match is
 * PMP_RGN_ADDR_MATCH_NAPOT, the modified match must be PMP_RGN_ADDR_MATCH_NAPOT.
 */
#define LOS_ERRNO_PMP_NAPOT_INVALID_MATCH_MODE   LOS_ERRNO_OS_ERROR(LOS_MOD_MPU, 0x06)

/**
 * @ingroup pmp
 * PMP error code: Invalid access.
 *
 * Value: 0x02001207
 *
 * Solution: Checking whether the access is correct.
 */
#define LOS_ERRNO_PMP_REGION_INVALID_ACCESS      LOS_ERRNO_OS_ERROR(LOS_MOD_MPU, 0x07)

/**
 * @ingroup pmp
 * PMP error code: Base address is not in range.
 *
 * Value: 0x02001208
 *
 * Solution: Checking base address
 */
#define LOS_ERRNO_PMP_BASE_ADDRESS_NOT_IN_RANGE  LOS_ERRNO_OS_ERROR(LOS_MOD_MPU, 0x08)

/**
 * @ingroup pmp
 * PMP error code: According to the current base address, the size of the application is too big.
 *
 * Value: 0x02001209
 *
 * Solution: baseAddress + regionSize must not exceed RAM Max address
 */
#define LOS_ERRNO_PMP_REGION_SIZE_IS_TOO_BIG     LOS_ERRNO_OS_ERROR(LOS_MOD_MPU, 0x09)

/**
 * @ingroup pmp
 * PMP error code: According to the current configuration, the PMP entry is locked.
 *
 * Value: 0x0200120a
 *
 * Solution: if PMP entry is locked, then write configuration and address register will be ignored,
 * you should reset the system.
 */
#define LOS_ERRNO_PMP_ENTRY_IS_LOCKED            LOS_ERRNO_OS_ERROR(LOS_MOD_MPU, 0x0a)

/**
 * @ingroup pmp
 * PMP error code: PMP_RGN_ADDR_MATCH_NA4 is Unsupported patterns.
 *
 * Value: 0x0200120b
 *
 * Solution: PMP_RGN_ADDR_MATCH_NA4 is not support now.
 */
#define LOS_ERRNO_PMP_INVALID_MATCH              LOS_ERRNO_OS_ERROR(LOS_MOD_MPU, 0x0b)

/**
 * @ingroup pmp
 * PMP error code: According to the current configuration,
 * the NAPOT type and size is not matched in NAPOT range encoding.
 *
 * Value: 0x0200120c
 *
 * Solution: baseAddress + regionSize must obey the NAPOT range encoding rules.
 */
#define LOS_ERRNO_PMP_NAPOT_ENCODING_NON_MATCHED LOS_ERRNO_OS_ERROR(LOS_MOD_MPU, 0x0c)

/**
 * @ingroup pmp
 * PMP error code: According to the current configuration, if pmp(i + 1)cfg.A is set to TOR,
 * writes to pmpaddr(i) are ignored.
 *
 * Value: 0x0200120d
 *
 * Solution: check if the lower bound of region is locked of this region when setting address matching mode to TOR.
 */
#define LOS_ERRNO_PMP_TOR_LOWER_BOUND_LOCKED     LOS_ERRNO_OS_ERROR(LOS_MOD_MPU, 0x0d)

/**
 * @ingroup pmp
 * PMP error code: According to the current configuration, check whether the lower bound of TOR region equals to
 * the pre-region end address
 *
 * Value: 0x0200120e
 *
 * Solution: set the lower bound of TOR i-th region to the pre-region, i.e. (i-1)'th region end address
 */
#define LOS_ERRNO_PMP_TOR_LOWER_BOUND_NONMATCHED LOS_ERRNO_OS_ERROR(LOS_MOD_MPU, 0x0e)

/**
 * @ingroup pmp
 * PMP error code: Type extension that given is invalid.
 *
 * Value: 0x0200120f
 *
 * Solution: memory sectl configuration only support 00,01,10,11
 */
#define LOS_ERRNO_PMP_SECTL_INVALID              LOS_ERRNO_OS_ERROR(LOS_MOD_MPU, 0xf)

/**
 * @ingroup pmp
 * @brief pmp region init.
 *
 * @par Description:
 * This API is used to init the pmp Minimum protected address and Maximum protected address.
 * @attention
 * <ul>
 * @param  minAddr [IN] Minimum protected address
 * @param  maxAddr [IN] Maximum protected address
 * @par Dependency
 * <ul><li>pmp.h: the header file that contains the API declaration.</li></ul>
 */
UINT32 ArchProtectionUnitInit(UINT64 minAddr, UINT64 maxAddr);

#ifdef LOSCFG_ARCH_LINX_M
/**
 * @ingroup pmp
 * @brief pmp Sectl region get.
 *
 * @par Description:
 * This API is used to get Sectl region value.
 * @attention
 * <ul>
 * @param  NO
 * @par Dependency
 * <ul><li>pmp.h: the header file that contains the API declaration.</li></ul>
 * @since Huawei LiteOS V200R002C00
 */
UINT32 ArchGetSectl(VOID);
#endif

/**
 * @ingroup pmp
 * @brief pmp region disable.
 *
 * @par Description:
 * This API is used to disable the pmp entry.
 * @attention
 * <ul>
 * <li>the number must be in the range of [0, LOSCFG_PMP_MAX_SUPPORT).</li>
 * <li>.</li>
 * </ul>
 * @param  number     [IN] The pmp entry number
 * @par Dependency:
 * <ul><li>pmp.h: the header file that contains the API declaration.</li></ul>
 */
UINT32 ArchRegionDisable(UINT8 number);

/**
 * @ingroup pmp
 * @brief Obtain the set protection memory region.
 *
 * @par Description:
 * This API is used to set protection memory region.
 * @attention
 * <ul>
 * <li>the base address must be in the range of [LOSCFG_MPU_MIN_ADDRESS, LOSCFG_MPU_MAX_ADDRESS].</li>
 * <li>the input pstPmpInfo should be filled by using the defined macro in pmp.h, the size of pstMpuInfo
 * is less than or equal to LOSCFG_MPU_MAX_SUPPORT.</li>
 * <li>if the value of the input param pstPmpInfo->bLocked is 0, the access permission of machine mode will passed.</li>
 * <li>you can setup the region when the region size is greater than CACHE_LINE_SIZE, otherwise, it may produce
 * an UNALIGNED exception.</li>
 * <li>you can setup the region when memory attribute is belong to the enum struct of PmpMemAttrInfo, otherwise,
 * it may produce unpredictable results.</li>
 * <li>.</li>
 * <li>.</li>
 * </ul>
 *
 * @param  pmpInfo          [IN] Set the related configuration information for the protected area
 * @param  pmpInfo->bLocked [IN] lock region in machine mode. if 1, lock region, validating access permission in
 * machine mode, otherwise, disable it.
 * @retval UINT32           return LOS_OK, or return the index of pstPmpInfo which set up failed.
 * @par Dependency:
 * <ul><li>pmp.h: the header file that contains the API declaration.</li></ul>
 */
UINT32 ArchProtectionRegionSet(PMP_ENTRY_INFO *pmpInfo);
#endif  /* __ASSEMBLER__ */

#define PMPADDR_ALIGN_BIT       2
#define PMPCFG_MASK             0xFF
#ifdef __ASSEMBLER__
.macro PMP_SET_TOR_ENTRY index, baseaddr, memattr, pmpcfg, tmp
            li      \tmp, \baseaddr >> PMPADDR_ALIGN_BIT
            csrw    pmpaddr\index, \tmp

            li      \tmp, \memattr
            csrw    memattr\index, \tmp

            li      \tmp, PMPCFG_MASK << ((\index & 0x3) << 3)
            csrc    PMPCFG0+(\index >> 2), \tmp

            li      \tmp, \pmpcfg << ((\index & 0x3) << 3)
            csrs    PMPCFG0+(\index >> 2), \tmp
.endm
#else
#define PMP_SET_TOR_ENTRY(index, baseaddr, memAttr, pmpcfg)                                                           \
        do {                                                                                                          \
            asm volatile ("csrw "ASM_STR(pmpaddr##index)", %0" :: "r"(((UINT32)(baseaddr)) >> PMPADDR_ALIGN_BIT));    \
            asm volatile ("csrw "ASM_STR(memattr##index)", %0" :: "r"(memAttr));                                      \
            asm volatile ("csrc "CSR2STR(PMPCFG0 + index >> 2)", %0" : : "r"(PMPCFG_MASK << (((index) & 0x3) << 3))); \
            asm volatile ("csrs "CSR2STR(PMPCFG0 + index >> 2)", %0" : : "r"(pmpcfg << (((index) & 0x3) << 3)));      \
        } while (0)

#endif

#ifdef LOSCFG_KERNEL_DEEPSLEEP
#ifdef __ASSEMBLER__

/* Default save 16 pmp region set,
 * can be changed according to the actual situation.
 */
#if (LOSCFG_PMPCFG_MAX_SUPPORT == 4)
#define PMPCFG_REG_SIZE     16
#define PMPADDR_REG_SIZE    64
#define MEMATTR_REG_SIZE    64
#elif (LOSCFG_PMPCFG_MAX_SUPPORT == 8)
#define PMPCFG_REG_SIZE     32
#define PMPADDR_REG_SIZE    128
#define MEMATTR_REG_SIZE    128
#endif
#define SAVE_EIGHT_REG_SIZE 32

/* Save 8 regs to memory */
.macro SAVE_TO_MEMORY
    SREG a7, 0 * REGBYTES(a0)
    SREG a6, 1 * REGBYTES(a0)
    SREG a5, 2 * REGBYTES(a0)
    SREG a4, 3 * REGBYTES(a0)
    SREG a3, 4 * REGBYTES(a0)
    SREG a2, 5 * REGBYTES(a0)
    SREG a1, 6 * REGBYTES(a0)
    SREG t0, 7 * REGBYTES(a0)
.endm

/* Load 8 regs from memory */
.macro LOAD_FROM_MEMORY
    LREG a7, 0 * REGBYTES(a0)
    LREG a6, 1 * REGBYTES(a0)
    LREG a5, 2 * REGBYTES(a0)
    LREG a4, 3 * REGBYTES(a0)
    LREG a3, 4 * REGBYTES(a0)
    LREG a2, 5 * REGBYTES(a0)
    LREG a1, 6 * REGBYTES(a0)
    LREG t0, 7 * REGBYTES(a0)
.endm

/* Save PMP Reg */
.macro SAVE_PMP_REGS pmpSaveAddr
    la a0, \pmpSaveAddr

    csrr a7, PMPCFG0
    csrr a6, PMPCFG1
    csrr a5, PMPCFG2
    csrr a4, PMPCFG3
#if (LOSCFG_PMPCFG_MAX_SUPPORT == 8)
    csrr a3, PMPCFG4
    csrr a2, PMPCFG5
    csrr a1, PMPCFG6
    csrr t0, PMPCFG7
    SAVE_TO_MEMORY
#else
    SREG a7, 0 * REGBYTES(a0)
    SREG a6, 1 * REGBYTES(a0)
    SREG a5, 2 * REGBYTES(a0)
    SREG a4, 3 * REGBYTES(a0)
#endif
    addi a0, a0, PMPCFG_REG_SIZE

    csrr a7, PMPADDR0
    csrr a6, PMPADDR1
    csrr a5, PMPADDR2
    csrr a4, PMPADDR3
    csrr a3, PMPADDR4
    csrr a2, PMPADDR5
    csrr a1, PMPADDR6
    csrr t0, PMPADDR7
    SAVE_TO_MEMORY
    addi a0, a0, SAVE_EIGHT_REG_SIZE
    csrr a7, PMPADDR8
    csrr a6, PMPADDR9
    csrr a5, PMPADDR10
    csrr a4, PMPADDR11
    csrr a3, PMPADDR12
    csrr a2, PMPADDR13
    csrr a1, PMPADDR14
    csrr t0, PMPADDR15
    SAVE_TO_MEMORY
    addi a0, a0, SAVE_EIGHT_REG_SIZE
#if (LOSCFG_PMPCFG_MAX_SUPPORT == 8)
    csrr a7, PMPADDR16
    csrr a6, PMPADDR17
    csrr a5, PMPADDR18
    csrr a4, PMPADDR19
    csrr a3, PMPADDR20
    csrr a2, PMPADDR21
    csrr a1, PMPADDR22
    csrr t0, PMPADDR23
    SAVE_TO_MEMORY
    addi a0, a0, SAVE_EIGHT_REG_SIZE
    csrr a7, PMPADDR24
    csrr a6, PMPADDR25
    csrr a5, PMPADDR26
    csrr a4, PMPADDR27
    csrr a3, PMPADDR28
    csrr a2, PMPADDR29
    csrr a1, PMPADDR30
    csrr t0, PMPADDR31
    SAVE_TO_MEMORY
    addi a0, a0, SAVE_EIGHT_REG_SIZE
#endif

    csrr a7, MEMATTR0
    csrr a6, MEMATTR1
    csrr a5, MEMATTR2
    csrr a4, MEMATTR3
    csrr a3, MEMATTR4
    csrr a2, MEMATTR5
    csrr a1, MEMATTR6
    csrr t0, MEMATTR7
    SAVE_TO_MEMORY
    addi a0, a0, SAVE_EIGHT_REG_SIZE
    csrr a7, MEMATTR8
    csrr a6, MEMATTR9
    csrr a5, MEMATTR10
    csrr a4, MEMATTR11
    csrr a3, MEMATTR12
    csrr a2, MEMATTR13
    csrr a1, MEMATTR14
    csrr t0, MEMATTR15
    SAVE_TO_MEMORY
    addi a0, a0, SAVE_EIGHT_REG_SIZE
#if (LOSCFG_PMPCFG_MAX_SUPPORT == 8)
    csrr a7, MEMATTR16
    csrr a6, MEMATTR17
    csrr a5, MEMATTR18
    csrr a4, MEMATTR19
    csrr a3, MEMATTR20
    csrr a2, MEMATTR21
    csrr a1, MEMATTR22
    csrr t0, MEMATTR23
    SAVE_TO_MEMORY
    addi a0, a0, SAVE_EIGHT_REG_SIZE
    csrr a7, MEMATTR24
    csrr a6, MEMATTR25
    csrr a5, MEMATTR26
    csrr a4, MEMATTR27
    csrr a3, MEMATTR28
    csrr a2, MEMATTR29
    csrr a1, MEMATTR30
    csrr t0, MEMATTR31
    SAVE_TO_MEMORY
#endif
.endm

/* LOAD PMP Reg */
.macro LOAD_PMP_REGS pmpSaveAddr
    la a0, \pmpSaveAddr

#if (LOSCFG_PMPCFG_MAX_SUPPORT == 8)
    LOAD_FROM_MEMORY
#else
    LREG a7, 0 * REGBYTES(a0)
    LREG a6, 1 * REGBYTES(a0)
    LREG a5, 2 * REGBYTES(a0)
    LREG a4, 3 * REGBYTES(a0)
#endif
    csrw PMPCFG0, a7
    csrw PMPCFG1, a6
    csrw PMPCFG2, a5
    csrw PMPCFG3, a4
#if (LOSCFG_PMPCFG_MAX_SUPPORT == 8)
    csrw PMPCFG4, a3
    csrw PMPCFG5, a2
    csrw PMPCFG6, a1
    csrw PMPCFG7, t0
#endif

    addi a0, a0, PMPCFG_REG_SIZE
    LOAD_FROM_MEMORY
    csrw PMPADDR0, a7
    csrw PMPADDR1, a6
    csrw PMPADDR2, a5
    csrw PMPADDR3, a4
    csrw PMPADDR4, a3
    csrw PMPADDR5, a2
    csrw PMPADDR6, a1
    csrw PMPADDR7, t0
    addi a0, a0, SAVE_EIGHT_REG_SIZE
    LOAD_FROM_MEMORY
    csrw PMPADDR8, a7
    csrw PMPADDR9, a6
    csrw PMPADDR10, a5
    csrw PMPADDR11, a4
    csrw PMPADDR12, a3
    csrw PMPADDR13, a2
    csrw PMPADDR14, a1
    csrw PMPADDR15, t0
#if (LOSCFG_PMPCFG_MAX_SUPPORT == 8)
    addi a0, a0, SAVE_EIGHT_REG_SIZE
    LOAD_FROM_MEMORY
    csrw PMPADDR16, a7
    csrw PMPADDR17, a6
    csrw PMPADDR18, a5
    csrw PMPADDR19, a4
    csrw PMPADDR20, a3
    csrw PMPADDR21, a2
    csrw PMPADDR22, a1
    csrw PMPADDR23, t0
    addi a0, a0, SAVE_EIGHT_REG_SIZE
    LOAD_FROM_MEMORY
    csrw PMPADDR24, a7
    csrw PMPADDR25, a6
    csrw PMPADDR26, a5
    csrw PMPADDR27, a4
    csrw PMPADDR28, a3
    csrw PMPADDR29, a2
    csrw PMPADDR30, a1
    csrw PMPADDR31, t0
#endif

    addi a0, a0, SAVE_EIGHT_REG_SIZE
    LOAD_FROM_MEMORY
    csrw MEMATTR0, a7
    csrw MEMATTR1, a6
    csrw MEMATTR2, a5
    csrw MEMATTR3, a4
    csrw MEMATTR4, a3
    csrw MEMATTR5, a2
    csrw MEMATTR6, a1
    csrw MEMATTR7, t0
    addi a0, a0, SAVE_EIGHT_REG_SIZE
    LOAD_FROM_MEMORY
    csrw MEMATTR8, a7
    csrw MEMATTR9, a6
    csrw MEMATTR10, a5
    csrw MEMATTR11, a4
    csrw MEMATTR12, a3
    csrw MEMATTR13, a2
    csrw MEMATTR14, a1
    csrw MEMATTR15, t0
#if (LOSCFG_PMPCFG_MAX_SUPPORT == 8)
    addi a0, a0, SAVE_EIGHT_REG_SIZE
    LOAD_FROM_MEMORY
    csrw MEMATTR16, a7
    csrw MEMATTR17, a6
    csrw MEMATTR18, a5
    csrw MEMATTR19, a4
    csrw MEMATTR20, a3
    csrw MEMATTR21, a2
    csrw MEMATTR22, a1
    csrw MEMATTR23, t0
    addi a0, a0, SAVE_EIGHT_REG_SIZE
    LOAD_FROM_MEMORY
    csrw MEMATTR24, a7
    csrw MEMATTR25, a6
    csrw MEMATTR26, a5
    csrw MEMATTR27, a4
    csrw MEMATTR28, a3
    csrw MEMATTR29, a2
    csrw MEMATTR30, a1
    csrw MEMATTR31, t0
#endif
.endm
#endif
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ARCH_PMP_H */
