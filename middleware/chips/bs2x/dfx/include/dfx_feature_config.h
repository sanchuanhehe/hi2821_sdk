/*
 * Copyright (c) @CompanyNameMagicTag 2022-2023. All rights reserved.
 * Description: dfx feature config file
 * This file should be changed only infrequently and with great care.
 */

#ifndef DFX_FEATURE_CONFIG_H
#define DFX_FEATURE_CONFIG_H

#define DFX_YES     1
#define DFX_NO      0

#ifndef CONFIG_DFX_SUPPORT_IDLE_TASK
#if defined(__LITEOS__)
#define CONFIG_DFX_SUPPORT_IDLE_TASK                DFX_NO
#else
#define CONFIG_DFX_SUPPORT_IDLE_TASK                DFX_NO
#endif /* __LITEOS__ */
#endif

/* 特性:是否支持文件传输 */
#ifndef CONFIG_DFX_SUPPORT_TRANSMIT_FILE
#define CONFIG_DFX_SUPPORT_TRANSMIT_FILE            DFX_YES
#endif

/* 特性:是否支持断点续传 */
#ifndef CONFIG_DFX_SUPPORT_CONTINUOUSLY_TRANSMIT
#define CONFIG_DFX_SUPPORT_CONTINUOUSLY_TRANSMIT    DFX_NO
#endif

/* 特性:是否支持DIAG */
#ifndef CONFIG_DFX_SUPPORT_DIAG
#define CONFIG_DFX_SUPPORT_DIAG                     DFX_YES
#endif

/* 特性:是否支持diag cmd的源 */
#ifndef CONFIG_DFX_SUPOORT_DIAG_CMD_SRC
#define CONFIG_DFX_SUPOORT_DIAG_CMD_SRC             DFX_NO
#endif


/* 特性:是否支持diag cmd的目标 */
#ifndef CONFIG_DFX_SUPPORT_DIAG_CMD_DST
#define CONFIG_DFX_SUPPORT_DIAG_CMD_DST             DFX_NO
#endif

/* 特性:是否支持diag ind的源 */
#ifndef CONFIG_DFX_SUPPORT_DIAG_IND_SRC
#define CONFIG_DFX_SUPPORT_DIAG_IND_SRC             DFX_NO
#endif

/* 特性:是否支持diag ind的目标 */
#ifndef CONFIG_DFX_SUPPORT_DIAG_IND_DST
#define CONFIG_DFX_SUPPORT_DIAG_IND_DST             DFX_NO
#endif

/* 特性:是否支持diag 接收数据包 */
#ifndef CONFIG_DFX_SUPPORT_DIAG_RX_PKT
#define CONFIG_DFX_SUPPORT_DIAG_RX_PKT              DFX_NO
#endif

/* 特性:是否支持串口连接UART */
#ifndef CONFIG_DFX_SUPPORT_DIAG_UART_CHANNEL
#define CONFIG_DFX_SUPPORT_DIAG_UART_CHANNEL        DFX_YES
#endif

/* 特性:是否作为集合日志的目标 */
#ifndef CONFIG_DFX_SUPPORT_DIAG_LOG_GATHER_DST
#define CONFIG_DFX_SUPPORT_DIAG_LOG_GATHER_DST      DFX_NO
#endif

/* 特性:当前核支持作为直连核 */
#ifndef CONFIG_DFX_SUPPORT_DIAG_CONNECT_DIRECTLY_ROLE
#define CONFIG_DFX_SUPPORT_DIAG_CONNECT_DIRECTLY_ROLE DFX_YES
#endif

/* 特性:DIAG作为上位机处理IND */
#ifndef CONFIG_DFX_SUPPORT_DIAG_UP_MACHINE
#define CONFIG_DFX_SUPPORT_DIAG_UP_MACHINE          DFX_NO
#endif

/* 特性:DIAG连接支持心跳机制 */
#ifndef CONFIG_DFX_SUPPORT_DIAG_BEAT_HEART
#define CONFIG_DFX_SUPPORT_DIAG_BEAT_HEART          DFX_YES
#endif

/* 特性: 支持密码连接 */
#ifndef CONFIG_DFX_SUPPORT_DIAG_CONNECT_PASSWORD
#define CONFIG_DFX_SUPPORT_DIAG_CONNECT_PASSWORD    DFX_NO
#endif

/* 特性: DIAG支持的日志类型 */
/* 精简型日志 */
#ifndef CONFIG_DFX_SUPPORT_DIAG_MINIMAL_MSG
#define CONFIG_DFX_SUPPORT_DIAG_MINIMAL_MSG         DFX_NO
#endif
/* 标准型日志 */
#ifndef CONFIG_DFX_SUPPORT_DIAG_NORMAL_MSG
#define CONFIG_DFX_SUPPORT_DIAG_NORMAL_MSG          DFX_NO
#endif
/* 扩展型日志(毫秒精度时间戳) */
#ifndef CONFIG_DFX_SUPPORT_DIAG_EXTEND_MSG
#define CONFIG_DFX_SUPPORT_DIAG_EXTEND_MSG          DFX_NO
#endif
/* 完整型日志 */
#ifndef CONFIG_DFX_SUPPORT_DIAG_FULL_MSG
#define CONFIG_DFX_SUPPORT_DIAG_FULL_MSG            DFX_YES
#endif

#if ((CONFIG_DFX_SUPPORT_DIAG_MINIMAL_MSG + CONFIG_DFX_SUPPORT_DIAG_NORMAL_MSG + \
    CONFIG_DFX_SUPPORT_DIAG_EXTEND_MSG + CONFIG_DFX_SUPPORT_DIAG_FULL_MSG) != 1)
#error "SELLECT DIAG LOG TYPE ERROR"
#endif

/* 特性:支持虚拟AT */
#ifndef CONFIG_DFX_SUPPORT_DIAG_VIRTUAL_AT
#define CONFIG_DFX_SUPPORT_DIAG_VIRTUAL_AT          DFX_NO
#endif

/* 特性:支持虚拟SHELL */
#ifndef CONFIG_DFX_SUPPORT_DIAG_VRTTUAL_SHELL
#define CONFIG_DFX_SUPPORT_DIAG_VRTTUAL_SHELL       DFX_YES
#endif

/* 特性: DIAG适配SOC LOG */
#ifndef CONFIG_DFX_SUPPORT_DIAG_ADAPT_SOC_LOG
#define CONFIG_DFX_SUPPORT_DIAG_ADAPT_SOC_LOG       DFX_NO
#endif

/* 特性: 支持SOC LOG */
#ifndef CONFIG_DFX_SUPPORT_SOC_LOG
#define CONFIG_DFX_SUPPORT_SOC_LOG                  DFX_NO
#endif

/* 特性: 系统状态监控 */
#ifndef CONFIG_DFX_SUPPORT_SYS_MONITOR
#define CONFIG_DFX_SUPPORT_SYS_MONITOR              DFX_NO
#endif

/* 特性: 延时重启 */
#ifndef CONFIG_DFX_SUPPORT_DELAY_REBOOT
#define CONFIG_DFX_SUPPORT_DELAY_REBOOT             DFX_NO
#endif

/* 特性: DIAG支持USB */
#ifndef CONFIG_DFX_SUPPORT_DIAG_CONNECT_USB
#define CONFIG_DFX_SUPPORT_DIAG_CONNECT_USB         DFX_NO
#endif

/* 特性: DIAG支持蓝牙连接 */
#ifndef CONFIG_DFX_SUPPORT_DIAG_CONNECT_BLE
#define CONFIG_DFX_SUPPORT_DIAG_CONNECT_BLE         DFX_YES
#endif

/* 特性: 支持文件系统 */
#ifndef CONFIG_DFX_SUPPORT_FILE_SYSTEM
#define CONFIG_DFX_SUPPORT_FILE_SYSTEM              DFX_NO
#endif

/* 特性: 支持日志离线存储到本地 */
#ifndef CONFIG_DFX_SUPPORT_OFFLINE_LOG_FILE
#define CONFIG_DFX_SUPPORT_OFFLINE_LOG_FILE         DFX_YES
#endif

/* 特性: 处理fault event */
#ifndef CONFIG_DFX_SUPPORT_FAULT_EVENT
#define CONFIG_DFX_SUPPORT_FAULT_EVENT              DFX_NO
#endif

/* 特性: 记录key event */
#ifndef CONFIG_DFX_SUPPORT_KEY_EVENT
#define CONFIG_DFX_SUPPORT_KEY_EVENT                DFX_NO
#endif

/* 特性: 记录统计 event */
#ifndef CONFIG_DFX_SUPPORT_STAT_EVENT
#define CONFIG_DFX_SUPPORT_STAT_EVENT               DFX_NO
#endif

/* 特性: 支持 分区表 */
#ifndef CONFIG_DFX_SUPPORT_PARTITION
#define CONFIG_DFX_SUPPORT_PARTITION                DFX_YES
#endif

#endif /* DFX_FEATURE_CONFIG_H */