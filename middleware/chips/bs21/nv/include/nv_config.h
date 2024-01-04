/*
 * Copyright (c) @CompanyNameMagicTag 2021-2023. All rights reserved.
 * Description: KV Storage Library definitions
 */

#ifndef NV_CONFIG_H
#define NV_CONFIG_H

#define NV_TASKS_MAX_NUM             64             /* 可支持的NV保存任务的最大数（多核系统使用） */
#define NV_NORMAL_KVALUE_MAX_LEN     4060           /* 普通NV的最大数据长度 */
#define NV_ENCRYPTED_KVALUE_MAX_LEN  4048           /* 加密NV的最大数据长度 */

#define NV_CACHE_LINE_SIZE           32

#define MALLOC_ALIGN_BOUNDARY    4

/* Flash 物理地址起始和结束 */
#define FLASH_PHYSICAL_ADDR_START     SFC_FLASH_START
#define FLASH_PHYSICAL_ADDR_END       (FLASH_PHYSICAL_ADDR_START + SFC_FLASH_LEN)

#define KV_PAGE_SIZE                  4096

#define FLASH_MAPPED_ADDR_START       SFC_FLASH_START
#define FLASH_MAPPED_END              (SFC_FLASH_START + SFC_FLASH_LEN)

/* NV运行区默认起始地址(物理地址) */
#define KV_STORE_START_ADDR           (NV_STATR_ADDR)

#define FLASH_PHYSICAL_ADDR_REAL_START FLASH_PHYSICAL_ADDR_START

/* NV运行区page数 */
#define KV_STORE_DATA_PAGE_NUM        (NV_LENGTH / KV_PAGE_SIZE)

/* NV运行区默认长度（不包含备份区） */
#define KV_STORE_DATA_SIZE            NV_LENGTH


/* NV备份区默认起始地址(物理地址) */
#define KV_BACKUP_START_ADDR          (KV_STORE_START_ADDR + KV_STORE_DATA_SIZE)
/* NV备份区page数 */
#define KV_BACKUP_PAGE_NUM            0
/* NV备份区默认长度 */
#define KV_BACKUP_DATA_SIZE           (KV_BACKUP_PAGE_NUM * KV_PAGE_SIZE)

/**
 * Fixed number of pages for each KV store
 */
/* Consider using NV region contents to determine actual number of pages and stores */
#define KV_STORE_PAGES_SCPU    0
#define KV_STORE_PAGES_ACPU    2

#define MCORE_REGISTER_NV_NOTIFY_MAX_NUM 10

/*
 * NV处理数据的块大小（取值范围[128 ~ 4096]，必须为16字节倍数）
 * 此长度影响NV内部处理数据所需的内存大小，如内存紧张，则不宜太大。
 * 在需要加密NV但又不支持分段加解密的情况下，最好设置为最大值4096，以避免数据被分段处理。
 */
#define NV_KEY_DATA_CHUNK_LEN            128


/* -------------------------------------  特性宏定义 ------------------------------------- */

#define NV_YES     1
#define NV_NO      0

/* 特性： 支持NV升级 */
#ifndef CONFIG_NV_SUPPORT_OTA_UPDATE
#define CONFIG_NV_SUPPORT_OTA_UPDATE            NV_NO
#endif

/* 特性： 支持NV备份恢复 */
#ifndef CONFIG_NV_SUPPORT_BACKUP_RESTORE
#define CONFIG_NV_SUPPORT_BACKUP_RESTORE        NV_NO
#endif

/* 特性： 支持NV加密 */
#ifndef CONFIG_NV_SUPPORT_ENCRYPT
#define CONFIG_NV_SUPPORT_ENCRYPT               NV_NO
#endif

/* 特性： 支持NV跳过被破坏的NV项 */
#ifndef CONFIG_NV_SUPPORT_SKIP_CORRUPT_KEY
#define CONFIG_NV_SUPPORT_SKIP_CORRUPT_KEY      NV_NO
#endif

/* 特性： 加密NV使用HASH校验 */
#ifndef CONFIG_NV_SUPPORT_HASH_FOR_CRYPT
#define CONFIG_NV_SUPPORT_HASH_FOR_CRYPT        NV_NO
#endif

/* 特性：KEY变更通知 */
#ifndef CONFIG_NV_SUPPORT_CHANGE_NOTIFY
#define CONFIG_NV_SUPPORT_CHANGE_NOTIFY         NV_NO
#endif

#endif /* NV_CONFIG_H */

