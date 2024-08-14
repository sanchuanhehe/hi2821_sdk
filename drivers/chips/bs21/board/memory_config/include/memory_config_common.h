/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description:  Default memory configurations
 * Author: @CompanyNameTag
 * Create:  2021-06-16
 */

#ifndef MEMORY_CONFIG_COMMON_H
#define MEMORY_CONFIG_COMMON_H

#include "product.h"
#include "chip_definitions.h"
#include "chip_core_definition.h"

/**
 * @defgroup connectivity_config_memory MEMORY
 * @ingroup  connectivity_config
 * @{
 */
/* Standard lengths */
#define BT_VECTORS_LENGTH     0x204
#define APP_VECTORS_LENGTH    360
#define VERSION_LENGTH        88
#define CHANGE_ID_LENGTH      16

#ifdef MASTER_LOAD_SLAVE
#define BUILD_INFO_LENGRH    408
#else
#define BUILD_INFO_LENGRH    0
#endif

#define FILL_4_BYTE           4

#define BCPU_RAM_START        0x0
/* ----------------------------------------------------------------------------------------------------------------- */
/* Share MEM defines, 64K in FPGA, 160K in ASIC
 * SHARE MEM                IPC Mailbox
 *                          SYSTEM CONFIG
 *                          SYSTEM STATUS
 *                          BT LOG
 *                          APP LOG
 *                          DSP LOG
 */
#define SHARED_MEM_START 0x87000000
#define SHARED_MEM_LENGTH 0x4000
#define SHARED_MEM_END    (SHARED_MEM_START + SHARED_MEM_LENGTH)

/*
 * ********************* ROM ALLOCATION ***********************
 *
 * Used solely by the BT Core ROM image built into the chip.
 */
/* 32K ROM */
#define BOOTROM_START  0x10000
#ifdef ROM_EC
#define BOOTROM_LENGTH 0x5c00
#else
#define BOOTROM_LENGTH 0x5400
#endif


#define ROM_START      (BOOTROM_START + BOOTROM_LENGTH)
#if defined(BUILD_APPLICATION_STANDARD) && !defined(_PRE_FEATURE_VENEER_ROM)
#define ROM_LENGTH     0x2cc00
#else
#define ROM_LENGTH     0x2ac00
#endif


/* APP ITCM config */
#if defined(BUILD_APPLICATION_STANDARD) && !defined(_PRE_FEATURE_VENEER_ROM)
#define APP_ITCM_ORIGIN 0x42000
#else
#define APP_ITCM_ORIGIN 0x40000
#endif

#if defined(HADM_CARKEY)
#define APP_ITCM_LENGTH 0x10000
#else
#define APP_ITCM_LENGTH 0x14000
#endif

/* APP DTCM config */
#define APP_DTCM_ORIGIN 0x20000000
#define APP_DTCM_LENGTH 0x10000

/*
 * ********************* FLASH ALLOCATION *********************
 * Flash is shared between the cores, and also handles a limited number of
 * non-volatile storage areas.
 *
 * Flash is split into 2 main areas
 * IMAGE Area
 *  Managed by the BT core
 *  This contains, in order from the bottom of Flash:
 *      SSB - SSB image starting at the bottom of flash. Variable size depending on the SSB on the chip that
 *            the package is loaded on. The values in this file only apply to the SSB built with this file.
 *      RECOVERY_Image   - starts in the page after the SSB.
 *      BT_Image         - starts in the page after the RECOVERY_Image
 *      DSP_Image        - starts in the page after the BT_Image
 *      APP_Image        - starts in the page after the DSP_Image
 *      System On-demand region - dynamically allocated flash for FOTA etc
 * NV Area
 *  Ends at the last page of flash, is configurable in size above a minimum allocation.
 *  NV Area, of 12 pages, is allocated, from the lowest address:
 *      8 pages of general use (pages are not fixed and self-identifying), consisting of
 *          1 Page for Asset Store defragmentation
 *          3 pages for Asset Store
 *          2 Pages for App and DSP Core KV defragmentation
 *          1 page for DSP Core KV
 *          1 page for App Core KV
 *      2 pages for BT core use (last 2 pages in Flash)
 *          1 page for BT Core KV
 *          1 page for BT Core KV defragmentation
 */
#define FLASH_START     0x90100000
#ifdef FLASH_1M
#define FLASH_LEN       0x100000
#else
#define FLASH_LEN       0x80000
#endif
#define FLASH_MAX_END   (FLASH_START + FLASH_LEN)
#define EMBED_FLASH_START     0x8C400000
#define EMBED_FLASH_LEN       0x200000
#define EMBED_FLASH_MAX_END   (EMBED_FLASH_START + EMBED_FLASH_LEN)
#define SFC_FLASH_START     0x90100000
#ifdef FLASH_1M
#define SFC_FLASH_LEN       0x100000
#else
#define SFC_FLASH_LEN       0x80000
#endif
#define SFC_FLASH_MAX_END   (SFC_FLASH_START + SFC_FLASH_LEN)
#define FLASH_PAGE_SIZE 4096
/* APP ITCM config */
#define MCU_ITCM_END    (APP_ITCM_ORIGIN + APP_ITCM_LENGTH)

/* MCU Flashboot Region */
#define FLASHBOOT_REGION_START  (SFC_FLASH_START)
#ifdef SUPPORT_EXTERN_FLASH
#define FLASHBOOTA_REGION_LENGTH  (10 * FLASH_PAGE_SIZE)
#define FLASHBOOTB_REGION_LENGTH  (10 * FLASH_PAGE_SIZE)
#else
#define FLASHBOOTA_REGION_LENGTH  (8 * FLASH_PAGE_SIZE)
#define FLASHBOOTB_REGION_LENGTH  (8 * FLASH_PAGE_SIZE)
#endif
#define PARTITION_REGION_LENGTH  FLASH_PAGE_SIZE
#define CODE_INFO_OFFSET        0x300

#define NV_PAGES                2
#if defined(FLASH_1M)
#ifdef SUPPORT_BT_UPG
#define FOTA_DATA_PAGES         94
#else
#define FOTA_DATA_PAGES         0
#endif
#else
#define FOTA_DATA_PAGES         0
#endif

#define APP_PROGRAM_ORIGIN      (FLASHBOOT_REGION_START + FLASHBOOTA_REGION_LENGTH + \
                                PARTITION_REGION_LENGTH + CODE_INFO_OFFSET + FLASHBOOTB_REGION_LENGTH)
#define APP_PROGRAM_LENGTH      (SFC_FLASH_MAX_END - APP_PROGRAM_ORIGIN - NV_PAGES * FLASH_PAGE_SIZE - \
                                FOTA_DATA_PAGES * FLASH_PAGE_SIZE)

/* ----------------------------------------------------------------------------------------------------------------- */
/* KV Region Defines */
#define NV_STATR_ADDR           (SFC_FLASH_START + SFC_FLASH_LEN - NV_PAGES * FLASH_PAGE_SIZE)
#define NV_LENGTH         (NV_PAGES * FLASH_PAGE_SIZE)
/* Minimum System Configuration pages in FLASH reserved */
#if USE_KV_MODE == YES
#define SYSTEM_RESERVED_FLASH_PAGES 4
#else
#define SYSTEM_RESERVED_FLASH_PAGES 0
#endif
/* Minimum reserved Non Volatile storage + Config Data. */
#define FLASH_RESERVED_LENGTH (SYSTEM_RESERVED_FLASH_PAGES * FLASH_PAGE_SIZE)

/*
 * ********************* SEC BOOT REGION *********************
 */
#define MCU_ROOT_PUBKEY_START           SEC_BOOT_FLASH_REGION_START
#define MCU_SUB_PUBKEY_START            (MCU_ROOT_PUBKEY_START + FLASH_PAGE_SIZE)
#define MCU_SSB_IMAGE_SIGN_START        (MCU_SUB_PUBKEY_START + FLASH_PAGE_SIZE)
#define MCU_SUB_PUBKEY_BACKUP_START     (MCU_SSB_IMAGE_SIGN_START + FLASH_PAGE_SIZE)
#define MCU_SSB_IMAGE_SIGN_BACKUP_START (MCU_SUB_PUBKEY_BACKUP_START + FLASH_PAGE_SIZE)

#define MCU_SEC_BOOT_SIGN_BACKUP_START  MCU_SUB_PUBKEY_BACKUP_START
#define MCU_SEC_BOOT_SIGN_BACKUP_LEN    (2 * FLASH_PAGE_SIZE)

#define MCU_RECOVERY_IMAGE_SIGN_START   (MCU_SEC_BOOT_SIGN_BACKUP_START + FLASH_PAGE_SIZE)
#define MCU_BT_IMAGE_SIGN_START         (MCU_RECOVERY_IMAGE_SIGN_START + FLASH_PAGE_SIZE)
#define MCU_APP_IMAGE_SIGN_START        (MCU_BT_IMAGE_SIGN_START + FLASH_PAGE_SIZE)

/*
 * ********************* RAM ALLOCATION *********************
 *
 * Three main RAM areas, the 'BCPU' RAM, the 'APP' RAM, and 'share' RAM.
 *      'APP' RAM include 'ITCM' for code and 'DTCM' for DATA.
 * As the name implies, every core can access it's RAM and they all allowed
 * to access 'share' RAM
 *
 * BT core can access all RAM include APP RAM and DSP RAM.
 *
 * The 'shared' RAM is used by all cores, this area visible to them all
 * to exchange larger amounts of data.
 *
 * SHARED RAM               IPC mail box
 *                          LOG Area
 */
/* ----------------------------------------------------------------------------------------------------------------- */
/* ----------------------------------------------------------------------------------------------------------------- */
/* APP RAM defines
 * APP has base 80K ITCM (Instruction TCM) for code
 *              64K DTCM (Data TCM) for DATA.
 * APP ITCM                 VECTORS TABLE
 *                          RAM TEXT
 *
 * APP DTCM                 STACK
 *                          RAM
 */
/* 80K ITCM for APP core code, start at end of vectors table */
#define APP_RAMTEXT_ORIGIN (APP_ITCM_ORIGIN)
#define APP_RAMTEXT_LENGTH (APP_ITCM_LENGTH)

/* 64K DTCM for APP core data */
/* stack for normal 1k */
#define APP_USER_STACK_BASEADDR APP_DTCM_ORIGIN
#define APP_USER_STACK_LEN      0x400
#define APP_USER_STACK_LIMIT    (APP_USER_STACK_BASEADDR + APP_USER_STACK_LEN)

/* stack for irq 2k */
#define APP_IRQ_STACK_BASEADDR APP_USER_STACK_LIMIT
#define APP_IRQ_STACK_LEN      0x800
#define APP_IRQ_STACK_LIMIT    (APP_IRQ_STACK_BASEADDR + APP_IRQ_STACK_LEN)

/* stack for exception 0.5k */
#define APP_EXCP_STACK_BASEADDR APP_IRQ_STACK_LIMIT
#define APP_EXCP_STACK_LEN      0x200
#define APP_EXCP_STACK_LIMIT    (APP_EXCP_STACK_BASEADDR + APP_EXCP_STACK_LEN)

/* stack for nmi 0.5k */
#define APP_NMI_STACK_BASEADDR APP_EXCP_STACK_LIMIT
#define APP_NMI_STACK_LEN      0x200
#define APP_NMI_STACK_LIMIT    (APP_NMI_STACK_BASEADDR + APP_NMI_STACK_LEN)

#define APP_RAM_ORIGIN         (APP_EXCP_STACK_LIMIT)
#define APP_RAM_END            (APP_DTCM_ORIGIN + APP_DTCM_LENGTH - PRESERVED_REGION_LENGTH)
#define APP_RAM_LENGTH         (APP_RAM_END - APP_RAM_ORIGIN)

/* 12*N bytes for cpu trace, trace line is 12 bytes, 8184 bytes */
#define MCPU_TRACE_MEM_REGION_START 0x52006000
#define CPU_TRACE_MEM_REGION_LENGTH 0x3FC

/* 176 bytes for BT core preserve region */
#define PRESERVED_REGION_ORIGIN (APP_DTCM_ORIGIN + APP_DTCM_LENGTH - PRESERVED_REGION_LENGTH)
#define PRESERVED_REGION_LENGTH 0x100

/* 176 bytes for App core preserve region */
#define APP_PRESERVED_REGION_ORIGIN (PRESERVED_REGION_ORIGIN + PRESERVED_REGION_LENGTH)
#define APP_PRESERVED_REGION_LENGTH 0x0


/* System config region. */
#define SYSTEM_CFG_REGION_START  (APP_PRESERVED_REGION_ORIGIN + APP_PRESERVED_REGION_LENGTH)
#define SYSTEM_CFG_REGION_LENGTH 0x40

/* Reboot magic region. */
#define REBOOT_MAGIC_START  (SYSTEM_CFG_REGION_START + SYSTEM_CFG_REGION_LENGTH)
#define REBOOT_MAGIC_LENGTH 0x80

#define SYSTEM_CLK_REGION_START (REBOOT_MAGIC_START + REBOOT_MAGIC_LENGTH)
#define SYSTEM_CLK_REGION_LENGTH 0xC0

/* System status region. */
#define SYSTEM_STATUS_ORIGIN (SYSTEM_CLK_REGION_START + SYSTEM_CLK_REGION_LENGTH)
#define SYSTEM_STATUS_LENGTH 0xC0

/* Mass data region */
/* wear(BT - 1K, APP - 1K)  tws(BT - 1K, APP - 1K) */
#define MASSDATA_REGION_START   (SYSTEM_STATUS_ORIGIN + SYSTEM_STATUS_LENGTH)
#define MASSDATA_REGION_LENGTH  (BT_MASSDATA_LENGTH + APP_MASSDATA_LENGTH)

/* LOG Region */
/* BT - 5K, APP - 5K */

#define LOGGING_REGION_START  APP_NMI_STACK_LIMIT
#define LOGGING_REGION_LENGTH APP_LOGGING_LENGTH

/* Trng data shared memory 512byte */
#define TRNG_DATA_REGION_START  (MASSDATA_REGION_START + MASSDATA_REGION_LENGTH)
#define TRNG_DATA_REGION_LENGTH 0

/* Prompt tone region 10K for tws and 5K for wear */
#define PROMPT_TONE_REGION_START  (TRNG_DATA_REGION_START + TRNG_DATA_REGION_LENGTH)

/* Audio data stream region 8K for tws and 20K for wear */
#define AUDIO_DATA_STREAM_REGION_START  (PROMPT_TONE_REGION_START + PROMPT_TONE_REGION_LENGTH)

/* RESERVED share mem */
#define RESERVED_SHARE_MEM_ORIGIN (AUDIO_DATA_STREAM_REGION_START + AUDIO_DATA_STREAM_REGION_LENGTH)
#define RESERVED_SHARE_MEM_LENGTH (SHARED_MEM_START + SHARED_MEM_LENGTH - RESERVED_SHARE_MEM_ORIGIN)

#if (RESERVED_SHARE_MEM_LENGTH < 0)
#error Share memory overflow
#endif

#ifndef BUILD_APPLICATION_ATE
/* BT DIAG region, Use L2RAM  */
#define BT_DIAG_REGION_MEMORY_START  0x20008000
#define BT_DIAG_REGION_MEMORY_LENGTH 0xFFF8
#define BT_DIAG_REGION_MEMORY_END    (BT_DIAG_REGION_MEMORY_START + BT_DIAG_REGION_MEMORY_LENGTH)
#endif

#define TRANSMIT_OTA_INFO_SIZE    (0x1000 - 0x20)
#define TRANSMIT_OTA_INFO_START   0xF0000
#define TRANSMIT_OTA_INFO_END     (0xF1000 - 0x20) // 1M flash
#define TRANSMIT_OTA_DATA_START   0x90000 // 1M flash

#define OTA_FOTA_DATA_START        0x90000
#define OTA_TRANSMIT_INFO_START    0xF0000
#define OTA_HASH_START             (0xF1000 - 0x20)
#define OTA_TRANSMIT_INFO_LEN      0x1000
#define OTA_HASH_INFO_SIZE         0x20

/*
 * ********************* ADDITIONAL MEMORY CONFIGURATION DEFINITIONS *********************
 * The offset in the BCPU (either flash or ROM) to where the version information starts.
 */
#define VERSION_INFORMATION_OFFSET (BT_VECTORS_LENGTH)

/**
 * @}
 */
#endif
