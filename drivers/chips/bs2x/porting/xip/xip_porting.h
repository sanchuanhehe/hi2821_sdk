/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides xip port template \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-09-13， Create file. \n
 */
#ifndef XIP_PORTING_H
#define XIP_PORTING_H

#include <stdint.h>
#include <stdbool.h>
#include "platform_core.h"
#include "non_os.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/* XIP_CACHE_CTL_RB XIP_CACHE_CTL_RB_BASE address of Module's Register */
#define XIP_CACHE_CTL_RB_BASE 0xA3006000

/******************************************************************************/
/*                      XIP_CACHE_CTL_RB Registers' Definitions        */
/******************************************************************************/
#define XIP_CACHE_CTL_ID_REG                       (XIP_CACHE_CTL_RB_BASE + 0x0)
#define XIP_GP_REG0_REG                            (XIP_CACHE_CTL_RB_BASE + 0x10)
#define XIP_GP_REG1_REG                            (XIP_CACHE_CTL_RB_BASE + 0x14)
#define XIP_GP_REG2_REG                            (XIP_CACHE_CTL_RB_BASE + 0x18)
#define XIP_GP_REG3_REG                            (XIP_CACHE_CTL_RB_BASE + 0x1C)
#define MEM_CLKEN0_REG                             (XIP_CACHE_CTL_RB_BASE + 0x20)
#define MEM_SOFT_RST_N_REG                         (XIP_CACHE_CTL_RB_BASE + 0x30)
#define MEM_WDT_DIS0_REG                           (XIP_CACHE_CTL_RB_BASE + 0x38)
#define MEM_DIV4_REG                               (XIP_CACHE_CTL_RB_BASE + 0x50)
#define MEM_DIV_EN_REG                             (XIP_CACHE_CTL_RB_BASE + 0x90)
#define XIP_CACHE_EN_REG                           (XIP_CACHE_CTL_RB_BASE + 0x100)
#define XIP_MONITOR_SEL_REG                        (XIP_CACHE_CTL_RB_BASE + 0x104)
#define XIP_ICU_EN_REG                             (XIP_CACHE_CTL_RB_BASE + 0x108)
#define CFG_CACHE2HABM_OVER_TIME_L_REG             (XIP_CACHE_CTL_RB_BASE + 0x10C)
#define CFG_CACHE2HABM_OVER_TIME_H_REG             (XIP_CACHE_CTL_RB_BASE + 0x110)
#define XIP_CACHE_ERROR_RESP_MASK_REG              (XIP_CACHE_CTL_RB_BASE + 0x114)
#define XIP_CACHE_INTR_STS_REG                     (XIP_CACHE_CTL_RB_BASE + 0x120)
#define XIP_CACHE_INTR_MASK_STS_REG                (XIP_CACHE_CTL_RB_BASE + 0x124)
#define XIP_CACHE_INTR_MASK_REG                    (XIP_CACHE_CTL_RB_BASE + 0x128)
#define XIP_CACHE_INTR_CLR_REG                     (XIP_CACHE_CTL_RB_BASE + 0x12C)
#define CFG_CALCULATE_EN_REG                       (XIP_CACHE_CTL_RB_BASE + 0x140)
#define CACHE_MISS_LOAD_REG                        (XIP_CACHE_CTL_RB_BASE + 0x144)
#define CACHE_TOTAL_H_REG                          (XIP_CACHE_CTL_RB_BASE + 0x148)
#define CACHE_TOTAL_M_REG                          (XIP_CACHE_CTL_RB_BASE + 0x14C)
#define CACHE_TOTAL_L_REG                          (XIP_CACHE_CTL_RB_BASE + 0x150)
#define CACHE_HIT_H_REG                            (XIP_CACHE_CTL_RB_BASE + 0x154)
#define CACHE_HIT_M_REG                            (XIP_CACHE_CTL_RB_BASE + 0x158)
#define CACHE_HIT_L_REG                            (XIP_CACHE_CTL_RB_BASE + 0x15C)
#define CACHE_MISS_H_REG                           (XIP_CACHE_CTL_RB_BASE + 0x160)
#define CACHE_MISS_M_REG                           (XIP_CACHE_CTL_RB_BASE + 0x164)
#define CACHE_MISS_L_REG                           (XIP_CACHE_CTL_RB_BASE + 0x168)
#define MAN_SINGLE_REG                             (XIP_CACHE_CTL_RB_BASE + 0x170)
#define MAN_SINGLE_ADDR_H_REG                      (XIP_CACHE_CTL_RB_BASE + 0x174)
#define MAN_SINGLE_ADDR_L_REG                      (XIP_CACHE_CTL_RB_BASE + 0x178)
#define MAN_ALL_REG                                (XIP_CACHE_CTL_RB_BASE + 0x17C)
#define DIAG_READ_ADDR_L_REG                       (XIP_CACHE_CTL_RB_BASE + 0x190)
#define DIAG_READ_ADDR_H_REG                       (XIP_CACHE_CTL_RB_BASE + 0x194)
#define DIAG_WRITE_ADDR_L_REG                      (XIP_CACHE_CTL_RB_BASE + 0x198)
#define DIAG_WRITE_ADDR_H_REG                      (XIP_CACHE_CTL_RB_BASE + 0x19C)
#define DIAG_WRITE_DATA_L_REG                      (XIP_CACHE_CTL_RB_BASE + 0x1A0)
#define DIAG_WRITE_DATA_H_REG                      (XIP_CACHE_CTL_RB_BASE + 0x1A4)
#define CACHE2AHBM_CUR_STS_REG                     (XIP_CACHE_CTL_RB_BASE + 0x1A8)
#define XIP_WRITE_READ_ENABLE_REG                  (XIP_CACHE_CTL_RB_BASE + 0x200)
#define XIP_WRITE_READ_SYNC_REG                    (XIP_CACHE_CTL_RB_BASE + 0x204)
#define WRITE_TCPH_PERIOD_REG                      (XIP_CACHE_CTL_RB_BASE + 0x208)
#define WRITE_REDUNDANT_CNT_REG                    (XIP_CACHE_CTL_RB_BASE + 0x20C)
#define WRITE_FIFO_THRESHOLD_REG                   (XIP_CACHE_CTL_RB_BASE + 0x210)
#define WRITE_FIFO_SOFT_RESET_REG                  (XIP_CACHE_CTL_RB_BASE + 0x214)
#define WRITE_FIFO_STS_CLR_REG                     (XIP_CACHE_CTL_RB_BASE + 0x218)
#define CFG_XIP_OPI_READ_OVER_TIME_L_REG           (XIP_CACHE_CTL_RB_BASE + 0x21C)
#define CFG_XIP_OPI_READ_OVER_TIME_H_REG           (XIP_CACHE_CTL_RB_BASE + 0x220)
#define CFG_XIP_WRITE_OVER_TIME_L_REG              (XIP_CACHE_CTL_RB_BASE + 0x224)
#define CFG_XIP_WRITE_OVER_TIME_H_REG              (XIP_CACHE_CTL_RB_BASE + 0x228)
#define XIP_WRITE_READ_ERROR_RESP_MASK_REG         (XIP_CACHE_CTL_RB_BASE + 0x22C)
#define CFG_XIP_WRITE_PSRAM_CMD_L_REG              (XIP_CACHE_CTL_RB_BASE + 0x230)
#define CFG_XIP_WRITE_PSRAM_CMD_H_REG              (XIP_CACHE_CTL_RB_BASE + 0x234)
#define CFG_XIP_READ_PSRAM_CMD_L_REG               (XIP_CACHE_CTL_RB_BASE + 0x238)
#define CFG_XIP_READ_PSRAM_CMD_H_REG               (XIP_CACHE_CTL_RB_BASE + 0x23C)
#define CFG_CLK_BUS_LOW_FREQ_REG                   (XIP_CACHE_CTL_RB_BASE + 0x240)
#define WRITE_FIFO_STS_REG                         (XIP_CACHE_CTL_RB_BASE + 0x280)
#define XIP_WRITE_READ_DISABLE_AHB_ADD_L_REG       (XIP_CACHE_CTL_RB_BASE + 0x290)
#define XIP_WRITE_READ_DISABLE_AHB_ADD_H_REG       (XIP_CACHE_CTL_RB_BASE + 0x294)
#define XIP_WRITE_READ_WRONG_BURST_SIZE_ADDR_L_REG (XIP_CACHE_CTL_RB_BASE + 0x298)
#define XIP_WRITE_READ_WRONG_BURST_SIZE_ADDR_H_REG (XIP_CACHE_CTL_RB_BASE + 0x29C)
#define XIP_WRITE_READ_WRITE_WRAP_ADDR_L_REG       (XIP_CACHE_CTL_RB_BASE + 0x2A0)
#define XIP_WRITE_READ_WRITE_WRAP_ADDR_H_REG       (XIP_CACHE_CTL_RB_BASE + 0x2A4)
#define XIP_WRITE_READ_OPI_APB_ADDR_L_REG          (XIP_CACHE_CTL_RB_BASE + 0x2A8)
#define XIP_WRITE_READ_OPI_APB_ADDR_H_REG          (XIP_CACHE_CTL_RB_BASE + 0x2AC)
#define XIP_WRITE_READ_CUR_STS_WRITE_REG           (XIP_CACHE_CTL_RB_BASE + 0x2B0)
#define XIP_WRITE_READ_CUR_STS_READ_REG            (XIP_CACHE_CTL_RB_BASE + 0x2B4)
#define XIP_READ_QSPI_ENABLE_0_REG                 (XIP_CACHE_CTL_RB_BASE + 0x300)
#define XIP_READ_QSPI_SYNC_0_REG                   (XIP_CACHE_CTL_RB_BASE + 0x304)
#define CFG_WRAP_OPERATION_0_REG                   (XIP_CACHE_CTL_RB_BASE + 0x308)
#define CFG_ADDR_24_32_0_REG                       (XIP_CACHE_CTL_RB_BASE + 0x30C)
#define CFG_FLASH_SEL_0_REG                        (XIP_CACHE_CTL_RB_BASE + 0x310)
#define XIP_MODE_CODE_0_REG                        (XIP_CACHE_CTL_RB_BASE + 0x314)
#define FLASH_READ_CMD_0_REG                       (XIP_CACHE_CTL_RB_BASE + 0x318)
#define CFG_XIP_READ_OVER_TIME_L_0_REG             (XIP_CACHE_CTL_RB_BASE + 0x31C)
#define CFG_XIP_READ_OVER_TIME_H_0_REG             (XIP_CACHE_CTL_RB_BASE + 0x320)
#define XIP_READ_ERROR_RESP_MASK_0_REG             (XIP_CACHE_CTL_RB_BASE + 0x324)
#define XIP_READ_DISABLE_AHB_ADDR_L_0_REG          (XIP_CACHE_CTL_RB_BASE + 0x340)
#define XIP_READ_DISABLE_AHB_ADDR_H_0_REG          (XIP_CACHE_CTL_RB_BASE + 0x344)
#define XIP_READ_WRITE_ADDR_L_0_REG                (XIP_CACHE_CTL_RB_BASE + 0x348)
#define XIP_READ_WRITE_ADDR_H_0_REG                (XIP_CACHE_CTL_RB_BASE + 0x34C)
#define XIP_READ_WRITE_DATA_L_0_REG                (XIP_CACHE_CTL_RB_BASE + 0x350)
#define XIP_READ_WRITE_DATA_H_0_REG                (XIP_CACHE_CTL_RB_BASE + 0x354)
#define XIP_READ_WRONG_BURST_SIZE_ADDR_L_0_REG     (XIP_CACHE_CTL_RB_BASE + 0x358)
#define XIP_READ_WRONG_BURST_SIZE_ADDR_H_0_REG     (XIP_CACHE_CTL_RB_BASE + 0x35C)
#define XIP_READ_QSPI_APB_ADDR_L_0_REG             (XIP_CACHE_CTL_RB_BASE + 0x360)
#define XIP_READ_QSPI_APB_ADDR_H_0_REG             (XIP_CACHE_CTL_RB_BASE + 0x364)
#define XIP_READ_CUR_STS_0_REG                     (XIP_CACHE_CTL_RB_BASE + 0x368)
#define XIP_READ_QSPI_ENABLE_1_REG                 (XIP_CACHE_CTL_RB_BASE + 0x400)
#define XIP_READ_QSPI_SYNC_1_REG                   (XIP_CACHE_CTL_RB_BASE + 0x404)
#define CFG_WRAP_OPERATION_1_REG                   (XIP_CACHE_CTL_RB_BASE + 0x408)
#define CFG_ADDR_24_32_1_REG                       (XIP_CACHE_CTL_RB_BASE + 0x40C)
#define CFG_FLASH_SEL_1_REG                        (XIP_CACHE_CTL_RB_BASE + 0x410)
#define XIP_MODE_CODE_1_REG                        (XIP_CACHE_CTL_RB_BASE + 0x414)
#define FLASH_READ_CMD_1_REG                       (XIP_CACHE_CTL_RB_BASE + 0x418)
#define CFG_XIP_READ_OVER_TIME_L_1_REG             (XIP_CACHE_CTL_RB_BASE + 0x41C)
#define CFG_XIP_READ_OVER_TIME_H_1_REG             (XIP_CACHE_CTL_RB_BASE + 0x420)
#define XIP_READ_ERROR_RESP_MASK_1_REG             (XIP_CACHE_CTL_RB_BASE + 0x424)
#define XIP_READ_DISABLE_AHB_ADDR_L_1_REG          (XIP_CACHE_CTL_RB_BASE + 0x440)
#define XIP_READ_DISABLE_AHB_ADDR_H_1_REG          (XIP_CACHE_CTL_RB_BASE + 0x444)
#define XIP_READ_WRITE_ADDR_L_1_REG                (XIP_CACHE_CTL_RB_BASE + 0x448)
#define XIP_READ_WRITE_ADDR_H_1_REG                (XIP_CACHE_CTL_RB_BASE + 0x44C)
#define XIP_READ_WRITE_DATA_L_1_REG                (XIP_CACHE_CTL_RB_BASE + 0x450)
#define XIP_READ_WRITE_DATA_H_1_REG                (XIP_CACHE_CTL_RB_BASE + 0x454)
#define XIP_READ_WRONG_BURST_SIZE_ADDR_L_1_REG     (XIP_CACHE_CTL_RB_BASE + 0x458)
#define XIP_READ_WRONG_BURST_SIZE_ADDR_H_1_REG     (XIP_CACHE_CTL_RB_BASE + 0x45C)
#define XIP_READ_QSPI_APB_ADDR_L_1_REG             (XIP_CACHE_CTL_RB_BASE + 0x460)
#define XIP_READ_QSPI_APB_ADDR_H_1_REG             (XIP_CACHE_CTL_RB_BASE + 0x464)
#define XIP_READ_CUR_STS_1_REG                     (XIP_CACHE_CTL_RB_BASE + 0x468)
#define XIP_CTL_INTR_STS_REG                       (XIP_CACHE_CTL_RB_BASE + 0x500)
#define XIP_CTL_INTR_MASK_STS_REG                  (XIP_CACHE_CTL_RB_BASE + 0x504)
#define XIP_CTL_INTR_MASK_REG                      (XIP_CACHE_CTL_RB_BASE + 0x508)
#define XIP_CTL_INTR_CLR_REG                       (XIP_CACHE_CTL_RB_BASE + 0x50C)
#define CACHE_AHB_S1_ICM_PRIORITY_REG              (XIP_CACHE_CTL_RB_BASE + 0x600)
#define CACHE_AHB_S2_ICM_PRIORITY_REG              (XIP_CACHE_CTL_RB_BASE + 0x604)
#define CACHE_AHB_S3_ICM_PRIORITY_REG              (XIP_CACHE_CTL_RB_BASE + 0x608)
#define MEM_SUB_GATING_CONFIGURE_REG               (XIP_CACHE_CTL_RB_BASE + 0x700)
#define MEM_SUB_GATING_STS_REG                     (XIP_CACHE_CTL_RB_BASE + 0x704)
#define XIP_SUB_DIAG_EN_REG                        (XIP_CACHE_CTL_RB_BASE + 0x710)
#define XIP_SUB_DIAG_INFO_REG                      (XIP_CACHE_CTL_RB_BASE + 0x714)
#define RXDS_SEL_REG                               (XIP_CACHE_CTL_RB_BASE + 0x720)
#define RXDS_HIGH_SEL_REG                          (XIP_CACHE_CTL_RB_BASE + 0x724)
#define XIP_CFG_WAIT_CNT_0                         (XIP_CACHE_CTL_RB_BASE + 0xB10)

/**
 * @defgroup drivers_port_xip XIP
 * @ingroup  drivers_port
 * @{
 */

/**
 * @brief  Definition of the bus ID of hal xip.
 * @else
 * @brief  XIP bus定义
 * @endif
 */
typedef enum xip_bus {
    BUS_QSPI0 = 0,
    BUS_QSPI1 = 1,
    BUS_QSPI3 = 3,
    BUS_MAX,
} xip_bus_t;

/**
 * @brief  Base address for xip.
 * @return 指定xip的基地址。
 */
uintptr_t xip_porting_base_addr_get(void);

/**
 * @brief  Register xip index objects into xip module.
 */
uint32_t xip_porting_get_index(void);

/**
 * @brief  Enable xip error interrupt.
 */
void xip_error_interrupt_enable(void);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif