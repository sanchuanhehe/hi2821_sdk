/*
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: diag usr config
 * This file should be changed only infrequently and with great care.
 */

#ifndef ZDIAG_CONFIG_H
#define ZDIAG_CONFIG_H

#include "dfx_resource_id.h"
#include "memory_config_common.h"

#define DFX_OFFLINE_LOG_MUTI_FILE_NUM   1                       /* 离线日志最大文件个数 */
#define DFX_OFFLINE_LOG_PATH            ""                      /* 离线日志存放路径 */
#define DFX_OFFLINE_LOG_NAME            ""                      /* 离线日志文件名 */
#define DFX_OFFLINE_LOG_FILE_SIZE       0x32000 /* 离线日志每个文件最大字节数 */
#define DFX_OFFLINE_LOG_CACHE_SIZE      2048                    /* 离线日志缓存大小 */
#define DFX_OFFLINE_LOG_CACHE_ENABLE    1                       /* 离线日志缓存开关 */

#endif
