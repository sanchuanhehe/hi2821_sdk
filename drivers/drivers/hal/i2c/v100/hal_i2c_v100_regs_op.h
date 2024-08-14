/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides V100 i2c register operation api \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-08-15, Create file. \n
 */
#ifndef HAL_I2C_V100_REGS_OP_H
#define HAL_I2C_V100_REGS_OP_H

#include <stdint.h>
#include <stdbool.h>
#include "soc_osal.h"
#include "common_def.h"
#include "hal_i2c_v100_regs_def.h"
#include "i2c_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_i2c_v100_regs_op I2C V100 Regs Operation
 * @ingroup  drivers_hal_i2c
 * @{
 */

/**
 * @brief  Definition of the I2C interrupts.
 */
typedef enum i2c_int {
    I2C_INT_RX_UNDER,           /*!< Attempts to read the receive buffer when it is empty. */
    I2C_INT_RX_OVER,            /*!< Additional byte is received from an external I2C device when
                                     receive buffer is completely full */
    I2C_INT_RX_FULL,            /*!< Receive buffer reaches or goes above the RX_TL threshold in the
                                     IC_RX_TL register. */
    I2C_INT_TX_OVER,            /*!< Additional byte is writing when
                                     transmit buffer is filled to IC_TX_BUFFER_DEPTH. */
    I2C_INT_TX_EMPTY,           /*!< Transmit buffer is at or below the threshold value set
                                     in the IC_TX_TL register. */
    I2C_INT_RD_REQ,             /*!< Dw_apb_i2c is acting as a slave and another I2C master
                                     is attempting to read data from DW_apb_i2c. */
    I2C_INT_TX_ABRT,            /*!< I2c transmitter is unable to complete the
                                     intended actions on the contents of the transmit FIFO. */
    I2C_INT_RX_DONE,            /*!< Slave-transmitter set this bit to 1 if the
                                     master does not acknowledge a transmitted byte. */
    I2C_INT_ACTIVITY,           /*!< This bit captures DW_apb_i2c activity. */
    I2C_INT_STOP_DET,           /*!< Stop condition has occurred on the I2C interface regardless
                                     of whether DW_apb_i2c is operating in slave or master mode. */
    I2C_INT_START_DET,          /*!< Start or restart condition has occurred on the I2C interface regardless
                                     of whether DW_apb_i2c is operating in slave or master mode. */
    I2C_INT_GEN_CALL,           /*!< General Call address is received and it is acknowledged. */
    I2C_INT_INVALID_TYPE
} i2c_int_t;

/**
 * @brief  The interrupt registers type.
 */
typedef enum i2c_int_reg {
    INTR_STAT,                  /*!< Interrupt status register, mask valid. */
    INTR_MASK,                  /*!< Masked interrupt status register. */
    RAW_INTR_STAT,              /*!< Raw interrupt status register. */
} i2c_int_reg_t;

/**
 * @brief  The ic_con reg bit configuration type.
 */
typedef enum i2c_con_reg_config {
    IC_CON_DISABLED_BIT,                  /*!< Config ic_con reg bit as disabled. */
    IC_CON_ENABLED_BIT,                   /*!< Config ic_con reg bit as enabled. */
} i2c_con_reg_config_t;

extern i2c_regs_t *g_i2c_regs[I2C_BUS_MAX_NUM];

/**
 * @brief  Init the I2C which will set the base address of registers.
 * @param  [in]  bus The index of I2C. see @ref i2c_bus_t
 */
void hal_i2c_v100_regs_init(i2c_bus_t bus);

/**
 * @brief  Deinit the hal_i2c which will clear the base address of registers has been \n
 *         set by @ref hal_i2c_v100_regs_init.
 * @param  [in]  bus The index of I2C. see @ref i2c_bus_t
 */
void hal_i2c_v100_regs_deinit(i2c_bus_t bus);

/**
 * @brief  Set the I2C gc_mode.
 * @param  [in]  bus The index of I2C. see @ref i2c_bus_t
 */
void hal_i2c_v100_set_gc_mode(i2c_bus_t bus);

/**
 * @brief  Get the hal_i2c read command.
 * @param  [in]  first_flag First byte flag.
 * @param  [in]  restart_flag Restart flag.
 * @param  [in]  last_flag Last byte flag.
 * @param  [in]  stop_flag Stop flag.
 * @return The value of @ref ic_data_cmd_data.d32.
 */
uint32_t hal_i2c_v100_get_data_read_cmd(uint32_t first_flag, uint32_t restart_flag,
                                        uint32_t last_flag, uint32_t stop_flag);

/**
 * @brief  Get the hal_i2c write command.
 * @param  [in]  first_flag First byte flag.
 * @param  [in]  restart_flag Restart flag.
 * @param  [in]  last_flag Last byte flag.
 * @param  [in]  stop_flag Stop flag.
 * @return The value of @ref ic_data_cmd_data.d32.
 */
uint32_t hal_i2c_v100_get_data_write_cmd(uint32_t first_flag, uint32_t restart_flag,
                                         uint32_t last_flag, uint32_t stop_flag);

/**
 * @brief  Unmask hal_i2c interrupt.
 * @param  [in]  bus The index of I2C. see @ref i2c_bus_t
 * @param  [in]  int_id Interrupt id.
 */
void hal_i2c_v100_unmask_int(i2c_bus_t bus, i2c_int_t int_id);

/**
 * @brief  Mask hal_i2c interrupt.
 * @param  [in]  bus The index of I2C. see @ref i2c_bus_t
 * @param  [in]  int_id interrupt id
 */
void hal_i2c_v100_mask_int(i2c_bus_t bus, i2c_int_t int_id);

/**
 * @if Eng
 * @brief  Mask all I2C interrupt.
 * @param  [in]  bus The I2C bus. see @ref i2c_bus_t
 * @endif
 */
void hal_i2c_v100_mask_all_int(i2c_bus_t bus);

/**
 * @brief  Get hal_i2c interrupt.
 * @param  [in]  bus The index of I2C. see @ref i2c_bus_t
 * @param  [in]  reg Interrupt reg type. see @ref i2c_int_reg_t
 * @param  [in]  int_id Interrupt id. see @ref i2c_int_t
 * @return Interrupt status.
 */
uint32_t hal_i2c_v100_get_int(i2c_bus_t bus, i2c_int_reg_t reg, i2c_int_t int_id);

/**
 * @brief  Get hal_i2c interrupt.
 * @param  [in]  bus The index of I2C. see @ref i2c_bus_t
 * @param  [in]  cmd_type Command type(restart, stop, write, read).
 * @param  [in]  val The data to transmit.
 */
void hal_i2c_v100_set_transmit_data(i2c_bus_t bus, uint32_t cmd_type, uint32_t val);

/**
 * @brief  Set the hal_i2c gc_mode.
 * @param  [in]  bus The index of I2C. see @ref i2c_bus_t
 */
void hal_i2c_v100_set_gc_mode(i2c_bus_t bus);

/**
 * @brief  Set the hal_i2c normal_mode.
 * @param  [in]  bus The index of I2C. see @ref i2c_bus_t
 */
void hal_i2c_v100_set_normal_mode(i2c_bus_t bus);

/**
 * @brief  Set the value of @ref ic_con_data.master_mode.
 * @param  [in]  bus The index of I2C. see @ref i2c_bus_t
 */
static inline void hal_i2c_set_con_master_mode(i2c_bus_t bus)
{
    ic_con_data_t con_data;
    con_data.d32 = g_i2c_regs[bus]->con;
    con_data.b.master_mode = true;
    con_data.b.ic_slave_disable = true;
    g_i2c_regs[bus]->con = con_data.d32;
}

/**
 * @brief  Set the value of @ref ic_con_data.ic_slave_disable.
 * @param  [in]  bus The index of I2C. see @ref i2c_bus_t
 */
static inline void hal_i2c_set_con_slave_mode(i2c_bus_t bus)
{
    ic_con_data_t con_data;
    con_data.d32 = g_i2c_regs[bus]->con;
    con_data.b.master_mode = false;
    con_data.b.ic_slave_disable = false;
    g_i2c_regs[bus]->con = con_data.d32;
}

/**
 * @brief  Set the value of @ref ic_con_data.stop_det_ifaddressed.
 * @param  [in]  bus The index of I2C. see @ref i2c_bus_t
 * @param  [in]  val The value of @ref ic_con_data.stop_det_ifaddressed.
 */
static inline void hal_i2c_set_con_stop_det_ifaddressed(i2c_bus_t bus, uint32_t val)
{
    ic_con_data_t con_data;
    con_data.d32 = g_i2c_regs[bus]->con;
    con_data.b.stop_det_ifaddressed = val;
    g_i2c_regs[bus]->con = con_data.d32;
}

/**
 * @brief  Set the value of @ref ic_con_data.speed.
 * @param  [in]  bus The index of I2C. see @ref i2c_bus_t
 * @param  [in]  val The value of @ref ic_con_data.speed.
 */
static inline void hal_i2c_set_speed_mode(i2c_bus_t bus, uint32_t val)
{
    ic_con_data_t con_data;
    con_data.d32 = g_i2c_regs[bus]->con;
    con_data.b.speed = val;
    g_i2c_regs[bus]->con = con_data.d32;
}

/**
 * @brief  Set the value of @ref ic_con_data.ic_10bitaddr_slave.
 * @param  [in]  bus The index of I2C. see @ref i2c_bus_t
 * @param  [in]  val The value of @ref ic_con_data.ic_10bitaddr_slave.
 */
static inline void hal_i2c_set_slave_address_mode(i2c_bus_t bus, uint32_t val)
{
    ic_con_data_t con_data;
    con_data.d32 = g_i2c_regs[bus]->con;
    con_data.b.ic_10bitaddr_slave = val;
    g_i2c_regs[bus]->con = con_data.d32;
}

/**
 * @brief  Set the value of @ref ic_con_data.ic_10bitaddr_master.
 * @param  [in]  bus The index of I2C. see @ref i2c_bus_t
 * @param  [in]  val The value of @ref ic_con_data.ic_10bitaddr_master.
 */
static inline void hal_i2c_set_con_master_address_mode(i2c_bus_t bus, uint32_t val)
{
    ic_con_data_t con_data;
    con_data.d32 = g_i2c_regs[bus]->con;
    con_data.b.ic_10bitaddr_master = val;
    g_i2c_regs[bus]->con = con_data.d32;
}

/**
 * @brief  Set the value of @ref ic_con_data.rx_fifo_full_hld_ctrl.
 * @param  [in]  bus The index of I2C. see @ref i2c_bus_t
 * @param  [in]  val The value of @ref ic_con_data.rx_fifo_full_hld_ctrl.
 */
static inline void hal_i2c_set_con_rx_full_hold(i2c_bus_t bus, uint32_t val)
{
    ic_con_data_t con_data;
    con_data.d32 = g_i2c_regs[bus]->con;
    con_data.b.rx_fifo_full_hld_ctrl = val;
    g_i2c_regs[bus]->con = con_data.d32;
}

/**
 * @brief  Set the value of @ref ic_tar_data.master_10bitaddr.
 * @param  [in]  bus The index of I2C. see @ref i2c_bus_t
 * @param  [in]  val The value of @ref ic_tar_data.master_10bitaddr.
 */
static inline void hal_i2c_set_master_address_mode(i2c_bus_t bus, uint32_t val)
{
    ic_tar_data_t tar_data;
    tar_data.d32 = g_i2c_regs[bus]->tar;
    tar_data.b.master_10bitaddr = val;
    g_i2c_regs[bus]->tar = tar_data.d32;
}

/**
 * @brief  Set the value of @ref ic_tar_data.ic_tar.
 * @param  [in]  bus The index of I2C. see @ref i2c_bus_t
 * @param  [in]  val The value of @ref ic_tar_data.ic_tar.
 */
static inline void hal_i2c_set_master_target_address(i2c_bus_t bus, uint32_t val)
{
    ic_tar_data_t tar_data;
    tar_data.d32 = g_i2c_regs[bus]->tar;
    tar_data.b.ic_tar = val;
    g_i2c_regs[bus]->tar = tar_data.d32;
}

/**
 * @brief  Set the value of @ref ic_sar_data.ic_sar.
 * @param  [in]  bus The index of I2C. see @ref i2c_bus_t
 * @param  [in]  val The value of @ref ic_sar_data.ic_sar.
 */
static inline void hal_i2c_set_slave_address(i2c_bus_t bus, uint32_t val)
{
    ic_sar_data_t sar_data;
    sar_data.d32 = g_i2c_regs[bus]->sar;
    sar_data.b.ic_sar = val;
    g_i2c_regs[bus]->sar = sar_data.d32;
}

/**
 * @brief  Set the value of @ref ic_hs_maddr_data.ic_hs_mar.
 * @param  [in]  bus The index of I2C. see @ref i2c_bus_t
 * @param  [in]  val The value of @ref ic_hs_maddr_data.ic_hs_mar.
 */
static inline void hal_i2c_set_hs_address_code(i2c_bus_t bus, uint32_t val)
{
    ic_hs_maddr_data_t hs_maddr_data;
    hs_maddr_data.d32 = g_i2c_regs[bus]->hs_maddr;
    hs_maddr_data.b.ic_hs_mar = val;
    g_i2c_regs[bus]->hs_maddr = hs_maddr_data.d32;
}

/**
 * @brief  Get the value of @ref ic_data_cmd_data.dat.
 * @param  [in]  bus The index of I2C. see @ref i2c_bus_t
 * @return The value of @ref ic_data_cmd_data.dat.
 */
static inline uint8_t hal_i2c_get_receive_data(i2c_bus_t bus)
{
    ic_data_cmd_data_t data_cmd;
    data_cmd.d32 = g_i2c_regs[bus]->data_cmd;
    return (uint8_t)data_cmd.b.dat;
}

/**
 * @brief  Set the value of @ref ic_data_cmd_data.cmd.
 * @param  [in]  bus The index of I2C. see @ref i2c_bus_t
 * @param  [in]  val The value of @ref ic_data_cmd_data.
 */
static inline void hal_i2c_set_read_direction(i2c_bus_t bus, uint32_t val)
{
    ic_data_cmd_data_t data_cmd;
    data_cmd.d32 = val;
    g_i2c_regs[bus]->data_cmd = data_cmd.d32;
}

/**
 * @brief  Set the value of @ref ic_data_cmd_data.stop.
 * @param  [in]  bus The index of I2C. see @ref i2c_bus_t
 */
static inline void hal_i2c_set_force_stop(i2c_bus_t bus)
{
    ic_data_cmd_data_t data_cmd;
    data_cmd.d32 = g_i2c_regs[bus]->data_cmd;
    data_cmd.b.stop = 1;
    g_i2c_regs[bus]->data_cmd = data_cmd.d32;
}

/**
 * @brief  Set the value of @ref ic_data_cmd_data.restart.
 * @param  [in]  bus The index of I2C. see @ref i2c_bus_t
 * @param  [in]  val The value of @ref ic_data_cmd_data.restart.
 */
static inline void hal_i2c_set_force_restart(i2c_bus_t bus, uint32_t val)
{
    ic_data_cmd_data_t data_cmd;
    data_cmd.d32 = g_i2c_regs[bus]->data_cmd;
    data_cmd.b.restart = val;
    g_i2c_regs[bus]->data_cmd = data_cmd.d32;
}

/**
 * @brief  Set the value of @ref ic_ss_scl_hcnt_ic_ufm_scl_hcnt_data.ic_ss_scl_hcnt.
 * @param  [in]  bus The index of I2C. see @ref i2c_bus_t
 * @param  [in]  val The value of @ref ic_ss_scl_hcnt_ic_ufm_scl_hcnt_data.ic_ss_scl_hcnt.
 */
static inline void hal_i2c_set_ss_scl_hcnt(i2c_bus_t bus, uint32_t val)
{
    ic_ss_scl_hcnt_ic_ufm_scl_hcnt_data_t ss_scl_hcnt;
    ss_scl_hcnt.d32 = g_i2c_regs[bus]->ss_scl_hcnt;
    ss_scl_hcnt.ss_b.ic_ss_scl_hcnt = val;
    g_i2c_regs[bus]->ss_scl_hcnt = ss_scl_hcnt.d32;
}

/**
 * @brief  Set the value of @ref ic_ss_scl_lcnt_ic_ufm_scl_lcnt_data_t.ic_ss_scl_lcnt.
 * @param  [in]  bus The index of I2C. see @ref i2c_bus_t
 * @param  [in]  val The value of @ref ic_ss_scl_lcnt_ic_ufm_scl_lcnt_data_t.ic_ss_scl_lcnt.
 */
static inline void hal_i2c_set_ss_scl_lcnt(i2c_bus_t bus, uint32_t val)
{
    ic_ss_scl_lcnt_ic_ufm_scl_lcnt_data_t ss_scl_lcnt;
    ss_scl_lcnt.d32 = g_i2c_regs[bus]->ss_scl_lcnt;
    ss_scl_lcnt.ss_b.ic_ss_scl_lcnt = val;
    g_i2c_regs[bus]->ss_scl_lcnt = ss_scl_lcnt.d32;
}

/**
 * @brief  Set the value of @ref ic_fs_scl_hcnt_ic_ufm_tbuf_cnt_data.ic_fs_scl_hcnt.
 * @param  [in]  bus The index of I2C. see @ref i2c_bus_t
 * @param  [in]  val The value of @ref ic_fs_scl_hcnt_ic_ufm_tbuf_cnt_data.ic_fs_scl_hcnt.
 */
static inline void hal_i2c_set_fs_scl_hcnt(i2c_bus_t bus, uint32_t val)
{
    ic_fs_scl_hcnt_ic_ufm_tbuf_cnt_data_t fs_scl_hcnt;
    fs_scl_hcnt.d32 = g_i2c_regs[bus]->fs_scl_hcnt;
    fs_scl_hcnt.fs_b.ic_fs_scl_hcnt = val;
    g_i2c_regs[bus]->fs_scl_hcnt = fs_scl_hcnt.d32;
}

/**
 * @brief  Set the value of @ref ic_ss_scl_hcnt_ic_ufm_scl_hcnt_data.ic_ufm_scl_hcnt.
 * @param  [in]  bus The index of I2C. see @ref i2c_bus_t
 * @param  [in]  val The value of @ref ic_ss_scl_hcnt_ic_ufm_scl_hcnt_data.ic_ufm_scl_hcnt.
 */
static inline void hal_i2c_set_ufm_scl_hcnt(i2c_bus_t bus, uint32_t val)
{
    ic_ss_scl_hcnt_ic_ufm_scl_hcnt_data_t ufm_scl_hcnt;
    ufm_scl_hcnt.d32 = g_i2c_regs[bus]->ss_scl_hcnt;
    ufm_scl_hcnt.ufm_b.ic_ufm_scl_hcnt = val;
    g_i2c_regs[bus]->ss_scl_hcnt = ufm_scl_hcnt.d32;
}

/**
 * @brief  Set the value of @ref ic_ss_scl_lcnt_ic_ufm_scl_lcnt_data_t.ic_ufm_scl_lcnt.
 * @param  [in]  bus The index of I2C. see @ref i2c_bus_t
 * @param  [in]  val The value of @ref ic_ss_scl_lcnt_ic_ufm_scl_lcnt_data_t.ic_ufm_scl_lcnt.
 */
static inline void hal_i2c_set_ufm_scl_lcnt(i2c_bus_t bus, uint32_t val)
{
    ic_ss_scl_lcnt_ic_ufm_scl_lcnt_data_t ufm_scl_lcnt;
    ufm_scl_lcnt.d32 = g_i2c_regs[bus]->ss_scl_lcnt;
    ufm_scl_lcnt.ufm_b.ic_ufm_scl_lcnt = val;
    g_i2c_regs[bus]->ss_scl_lcnt = ufm_scl_lcnt.d32;
}

/**
 * @brief  Set the value of @ref ic_fs_scl_hcnt_ic_ufm_tbuf_cnt_data.ic_ufm_tbuf_cnt.
 * @param  [in]  bus The index of I2C. see @ref i2c_bus_t
 * @param  [in]  val The value of @ref ic_fs_scl_hcnt_ic_ufm_tbuf_cnt_data.ic_ufm_tbuf_cnt.
 */
static inline void hal_i2c_set_ufm_tbuf_cnt(i2c_bus_t bus, uint32_t val)
{
    ic_fs_scl_hcnt_ic_ufm_tbuf_cnt_data_t ufm_tbuf_cnt;
    ufm_tbuf_cnt.d32 = g_i2c_regs[bus]->fs_scl_hcnt;
    ufm_tbuf_cnt.fs_b.ic_fs_scl_hcnt = val;
    g_i2c_regs[bus]->fs_scl_hcnt = ufm_tbuf_cnt.d32;
}

/**
 * @brief  Set the value of @ref ic_fs_scl_lcnt_data.ic_fs_scl_lcnt.
 * @param  [in]  bus The index of I2C. see @ref i2c_bus_t
 * @param  [in]  val The value of @ref ic_fs_scl_lcnt_data.ic_fs_scl_lcnt.
 */
static inline void hal_i2c_set_fs_scl_lcnt(i2c_bus_t bus, uint32_t val)
{
    ic_fs_scl_lcnt_data_t fs_scl_lcnt;
    fs_scl_lcnt.d32 = g_i2c_regs[bus]->fs_scl_lcnt;
    fs_scl_lcnt.b.ic_fs_scl_lcnt = val;
    g_i2c_regs[bus]->fs_scl_lcnt = fs_scl_lcnt.d32;
}

/**
 * @brief  Set the value of @ref ic_hs_scl_hcnt_data.ic_hs_scl_hcnt.
 * @param  [in]  bus The index of I2C. see @ref i2c_bus_t
 * @param  [in]  val The value of @ref ic_hs_scl_hcnt_data.ic_hs_scl_hcnt.
 */
static inline void hal_i2c_set_hs_scl_hcnt(i2c_bus_t bus, uint32_t val)
{
    ic_hs_scl_hcnt_data_t hs_scl_hcnt;
    hs_scl_hcnt.d32 = g_i2c_regs[bus]->hs_scl_hcnt;
    hs_scl_hcnt.b.ic_hs_scl_hcnt = val;
    g_i2c_regs[bus]->hs_scl_hcnt = hs_scl_hcnt.d32;
}

/**
 * @brief  Set the value of @ref ic_hs_scl_lcnt_data.ic_hs_scl_lcnt.
 * @param  [in]  bus The index of I2C. see @ref i2c_bus_t
 * @param  [in]  val The value of @ref ic_hs_scl_lcnt_data.ic_hs_scl_lcnt.
 */
static inline void hal_i2c_set_hs_scl_lcnt(i2c_bus_t bus, uint32_t val)
{
    ic_hs_scl_lcnt_data_t hs_scl_lcnt;
    hs_scl_lcnt.d32 = g_i2c_regs[bus]->hs_scl_lcnt;
    hs_scl_lcnt.b.ic_hs_scl_lcnt = val;
    g_i2c_regs[bus]->hs_scl_lcnt = hs_scl_lcnt.d32;
}

/**
 * @brief  Get the value of @ref ic_intr_stat_data.d32.
 * @param  [in]  bus The index of I2C. see @ref i2c_bus_t
 * @return The value of @ref ic_intr_stat_data.b.
 */
static inline uint32_t hal_i2c_get_intr_stat(i2c_bus_t bus)
{
    return g_i2c_regs[bus]->intr_stat;
}

/**
 * @brief  Get the value of @ref ic_intr_mask_data.d32.
 * @param  [in]  bus The index of I2C. see @ref i2c_bus_t
 * @return The value of @ref ic_intr_mask_data.b.
 */
static inline uint32_t hal_i2c_get_intr_mask(i2c_bus_t bus)
{
    return g_i2c_regs[bus]->intr_mask;
}


/**
 * @brief  Get the value of @ref ic_raw_intr_stat_data.d32.
 * @param  [in]  bus The index of I2C. see @ref i2c_bus_t
 * @return The value of @ref ic_raw_intr_stat_data.b.
 */
static inline uint32_t hal_i2c_get_raw_intr_stat(i2c_bus_t bus)
{
    return g_i2c_regs[bus]->raw_intr_stat;
}

/**
 * @brief  Set the value of @ref ic_rx_tl_data.rx_tl.
 * @param  [in]  bus The index of I2C. see @ref i2c_bus_t
 * @param  [in]  val The value of @ref ic_rx_tl_data.rx_tl.
 */
static inline void hal_i2c_set_receive_threshold_level(i2c_bus_t bus, uint32_t val)
{
    ic_rx_tl_data_t rx_tl;
    rx_tl.d32 = g_i2c_regs[bus]->rx_tl;
    rx_tl.b.rx_tl = val;
    g_i2c_regs[bus]->rx_tl = rx_tl.d32;
}

/**
 * @brief  Set the value of @ref ic_tx_tl_data.tx_tl.
 * @param  [in]  bus The index of I2C. see @ref i2c_bus_t
 * @param  [in]  val The value of @ref ic_tx_tl_data.tx_tl.
 */
static inline void hal_i2c_set_transmit_threshold_level(i2c_bus_t bus, uint32_t val)
{
    ic_tx_tl_data_t tx_tl;
    tx_tl.d32 = g_i2c_regs[bus]->tx_tl;
    tx_tl.b.tx_tl = val;
    g_i2c_regs[bus]->tx_tl = tx_tl.d32;
}

/**
 * @brief  Clear intr by read the value of @ref ic_clr_intr_data.clr_intr.
 * @param  [in]  bus The index of I2C. see @ref i2c_bus_t
 */
static inline void hal_i2c_clr_all_int(i2c_bus_t bus)
{
    uapi_reg_read_val32(&g_i2c_regs[bus]->clr_intr);
}

/**
 * @brief  Clear rx_under intr by read the value of @ref ic_clr_rx_under_data.clr_rx_under.
 * @param  [in]  bus The index of I2C. see @ref i2c_bus_t
 */
static inline void hal_i2c_clr_rx_under_intr(i2c_bus_t bus)
{
    uapi_reg_read_val32(&g_i2c_regs[bus]->clr_rx_under);
}

/**
 * @brief  Clear rx_under intr by read the value of @ref ic_clr_rx_over_data.clr_rx_over.
 * @param  [in]  bus The index of I2C. see @ref i2c_bus_t
 */
static inline void hal_i2c_clr_rx_over_intr(i2c_bus_t bus)
{
    uapi_reg_read_val32(&g_i2c_regs[bus]->clr_rx_over);
}

/**
 * @brief  Clear tx_under intr by read the value of @ref ic_clr_tx_over_data.clr_tx_over.
 * @param  [in]  bus The index of I2C. see @ref i2c_bus_t
 */
static inline void hal_i2c_clr_tx_over_intr(i2c_bus_t bus)
{
    uapi_reg_read_val32(&g_i2c_regs[bus]->clr_tx_over);
}

/**
 * @brief  Clear rd_req intr by read the value of @ref ic_clr_rd_req_data.clr_rd_req.
 * @param  [in]  bus The index of I2C. see @ref i2c_bus_t
 */
static inline void hal_i2c_clr_rd_req_intr(i2c_bus_t bus)
{
    uapi_reg_read_val32(&g_i2c_regs[bus]->clr_rd_req);
}

/**
 * @brief  Clear tx_abrt intr by read the value of @ref ic_clr_tx_abrt_data.clr_tx_abrt.
 * @param  [in]  bus The index of I2C. see @ref i2c_bus_t
 */
static inline void hal_i2c_clr_tx_abrt_intr(i2c_bus_t bus)
{
    uapi_reg_read_val32(&g_i2c_regs[bus]->clr_tx_abrt);
}

/**
 * @brief  Clear rx_done intr by read the value of @ref ic_clr_rx_done_data.clr_rx_done.
 * @param  [in]  bus The index of I2C. see @ref i2c_bus_t
 */
static inline void hal_i2c_clr_rx_done_intr(i2c_bus_t bus)
{
    uapi_reg_read_val32(&g_i2c_regs[bus]->clr_rx_done);
}

/**
 * @brief  Clear activity intr by read the value of @ref ic_clr_activity_data.clr_activity.
 * @param  [in]  bus The index of I2C. see @ref i2c_bus_t
 */
static inline void hal_i2c_clr_activity_intr(i2c_bus_t bus)
{
    uapi_reg_read_val32(&g_i2c_regs[bus]->clr_activity);
}

/**
 * @brief  Clear stop_det intr by read the value of @ref ic_clr_stop_det_data.clr_stop_det.
 * @param  [in]  bus The index of I2C. see @ref i2c_bus_t
 */
static inline void hal_i2c_clr_stop_det_intr(i2c_bus_t bus)
{
    uapi_reg_read_val32(&g_i2c_regs[bus]->clr_stop_det);
}

/**
 * @brief  Clear start_det intr by read the value of @ref ic_clr_start_det_data.clr_start_det.
 * @param  [in]  bus The index of I2C. see @ref i2c_bus_t
 */
static inline void hal_i2c_clr_start_det_intr(i2c_bus_t bus)
{
    uapi_reg_read_val32(&g_i2c_regs[bus]->clr_start_det);
}

/**
 * @brief  Clear gen_call intr by read the value of @ref ic_clr_gen_call_data.clr_gen_call.
 * @param  [in]  bus The index of I2C. see @ref i2c_bus_t
 */
static inline void hal_i2c_clr_gen_call_intr(i2c_bus_t bus)
{
    uapi_reg_read_val32(&g_i2c_regs[bus]->clr_gen_call);
}

/**
 * @brief  Set true to the value of @ref ic_enable_data.enable.
 * @param  [in]  bus The index of I2C. see @ref i2c_bus_t
 */
static inline void hal_i2c_set_ic_enable(i2c_bus_t bus)
{
    ic_enable_data_t enable;
    enable.d32 = g_i2c_regs[bus]->enable;
    enable.b.enable = true;
    g_i2c_regs[bus]->enable = enable.d32;
}

/**
 * @brief  Set false to the value of @ref ic_enable_data.enable.
 * @param  [in]  bus The index of I2C. see @ref i2c_bus_t
 */
static inline void hal_i2c_set_ic_disable(i2c_bus_t bus)
{
    ic_enable_data_t enable;
    enable.d32 = g_i2c_regs[bus]->enable;
    enable.b.enable = false;
    g_i2c_regs[bus]->enable = enable.d32;
}

/**
 * @brief  Get the value of @ref ic_status_data.d32.
 * @param  [in]  bus The index of I2C. see @ref i2c_bus_t
 * @return The value of @ref ic_status_data.b.
 */
static inline uint32_t hal_i2c_get_status(i2c_bus_t bus)
{
    return g_i2c_regs[bus]->status;
}

/**
 * @brief  Get the value of @ref ic_txflr_data.d32.
 * @param  [in]  bus The index of I2C. see @ref i2c_bus_t
 * @return The value of @ref ic_txflr_data.b
 */
static inline uint32_t hal_i2c_get_transmit_fifo_num(i2c_bus_t bus)
{
    return g_i2c_regs[bus]->txflr;
}

/**
 * @brief  Get the value of @ref ic_rxflr_data.d32.
 * @param  [in]  bus The index of I2C. see @ref i2c_bus_t
 * @return The value of @ref ic_rxflr_data.b
 */
static inline uint32_t hal_i2c_get_receive_fifo_num(i2c_bus_t bus)
{
    return g_i2c_regs[bus]->rxflr;
}

/**
 * @brief  Set the value of @ref ic_sda_hold_data.sda_tx_hold.
 * @param  [in]  bus The index of I2C. see @ref i2c_bus_t
 * @param  [in]  val The value of @ref ic_sda_hold_data.sda_tx_hold.
 */
static inline void hal_i2c_set_sda_tx_hold_value(i2c_bus_t bus, uint32_t val)
{
    ic_sda_hold_data_t sda_hold_value;
    sda_hold_value.d32 = g_i2c_regs[bus]->sda_hold;
    sda_hold_value.b.sda_tx_hold = val;
    g_i2c_regs[bus]->sda_hold = sda_hold_value.d32;
}

/**
 * @brief  Set the value of @ref ic_sda_hold_data.sda_rx_hold.
 * @param  [in]  bus The index of I2C. see @ref i2c_bus_t
 * @param  [in]  val The value of @ref ic_sda_hold_data.sda_rx_hold.
 */
static inline void hal_i2c_set_sda_rx_hold_value(i2c_bus_t bus, uint32_t val)
{
    ic_sda_hold_data_t sda_hold_value;
    sda_hold_value.d32 = g_i2c_regs[bus]->sda_hold;
    sda_hold_value.b.sda_rx_hold = val;
    g_i2c_regs[bus]->sda_hold = sda_hold_value.d32;
}

/**
 * @brief  Get the value of @ref ic_tx_abrt_source_data.d32.
 * @param  [in]  bus The index of I2C. see @ref i2c_bus_t
 * @return The value of @ref ic_tx_abrt_source_data.b.
 */
static inline uint32_t hal_i2c_get_tx_abrt_source(i2c_bus_t bus)
{
    return g_i2c_regs[bus]->tx_abrt_source;
}

/**
 * @brief  Set the value of @ref ic_slv_data_nack_only_data.nack.
 * @param  [in]  bus The index of I2C. see @ref i2c_bus_t
 * @param  [in]  val The value of @ref ic_slv_data_nack_only_data.nack.
 */
static inline void hal_i2c_set_slv_data_nack(i2c_bus_t bus, uint32_t val)
{
    ic_slv_data_nack_only_data_t slv_data_nack;
    slv_data_nack.d32 = g_i2c_regs[bus]->slave_data_nack_only;
    slv_data_nack.b.nack = val;
    g_i2c_regs[bus]->slave_data_nack_only = slv_data_nack.d32;
}

/**
 * @brief  Set the value of @ref ic_dma_cr_data.rdmae.
 * @param  [in]  bus The index of I2C. see @ref i2c_bus_t
 * @param  [in]  val The value of @ref ic_dma_cr_data.rdmae.
 */
static inline void hal_i2c_set_receive_dma_en(i2c_bus_t bus, uint32_t val)
{
    ic_dma_cr_data_t dma_cr;
    dma_cr.d32 = g_i2c_regs[bus]->dma_cr;
    dma_cr.b.rdmae = val;
    g_i2c_regs[bus]->dma_cr = dma_cr.d32;
}

/**
 * @brief  Set the value of @ref ic_dma_cr_data.tdmae.
 * @param  [in]  bus The index of I2C. see @ref i2c_bus_t
 * @param  [in]  val The value of @ref ic_dma_cr_data.tdmae.
 */
static inline void hal_i2c_set_transmit_dma_en(i2c_bus_t bus, uint32_t val)
{
    ic_dma_cr_data_t dma_cr;
    dma_cr.d32 = g_i2c_regs[bus]->dma_cr;
    dma_cr.b.tdmae = val;
    g_i2c_regs[bus]->dma_cr = dma_cr.d32;
}

/**
 * @brief  Set the value of @ref ic_dma_tdlr_data.dmatdl.
 * @param  [in]  bus The index of I2C. see @ref i2c_bus_t
 * @param  [in]  val The value of @ref ic_dma_tdlr_data.dmatdl.
 */
static inline void hal_i2c_set_transmit_dma_level(i2c_bus_t bus, uint32_t val)
{
    ic_dma_tdlr_data_t dma_tdlr;
    dma_tdlr.d32 = g_i2c_regs[bus]->dma_tdlr;
    dma_tdlr.b.dmatdl = val;
    g_i2c_regs[bus]->dma_tdlr = dma_tdlr.d32;
}

/**
 * @brief  Set the value of @ref ic_dma_rdlr_data.dmardl.
 * @param  [in]  bus The index of I2C. see @ref i2c_bus_t
 * @param  [in]  val The value of @ref ic_dma_rdlr_data.dmardl.
 */
static inline void hal_i2c_set_receive_dma_level(i2c_bus_t bus, uint32_t val)
{
    ic_dma_rdlr_data_t dma_rdlr;
    dma_rdlr.d32 = g_i2c_regs[bus]->dma_rdlr;
    dma_rdlr.b.dmardl = val;
    g_i2c_regs[bus]->dma_rdlr = dma_rdlr.d32;
}

/**
 * @brief  Set the value of @ref ic_ack_general_data.ack_gen_call.
 * @param  [in]  bus The index of I2C. see @ref i2c_bus_t
 * @param  [in]  val The value of @ref ic_ack_general_data.ack_gen_call.
 */
static inline void hal_i2c_set_ack_gc_en(i2c_bus_t bus, uint32_t val)
{
    ic_ack_general_data_t ack_gc_en;
    ack_gc_en.d32 = g_i2c_regs[bus]->ack_general_call;
    ack_gc_en.b.ack_gen_call = val;
    g_i2c_regs[bus]->ack_general_call = ack_gc_en.d32;
}

/**
 * @brief  Get the value of @ref ic_enable_status_data.ic_en.
 * @param  [in]  bus The index of I2C. see @ref i2c_bus_t
 * @return The value of @ref ic_enable_status_data.ic_en.
 */
static inline uint32_t hal_i2c_get_ic_en_status(i2c_bus_t bus)
{
    ic_enable_status_data_t ic_en_status;
    ic_en_status.d32 = g_i2c_regs[bus]->enable_status;
    return ic_en_status.b.ic_en;
}

/**
 * @brief  Get the value of @ref ic_comp_param_data_t.d32.
 * @param  [in]  bus The index of I2C. see @ref i2c_bus_t
 * @return The value of @ref ic_comp_param_data_t.b.
 */
static inline uint32_t hal_i2c_get_component_parameter(i2c_bus_t bus)
{
    return g_i2c_regs[bus]->comp_param;
}


/**
 * @brief  Get the value of @ref ic_comp_version_data.d32.
 * @param  [in]  bus The index of I2C. see @ref i2c_bus_t
 * @return The value of @ref ic_comp_version_data.b.
 */
static inline uint32_t hal_i2c_get_component_version(i2c_bus_t bus)
{
    return g_i2c_regs[bus]->comp_version;
}

/**
 * @brief  Get the value of @ref ic_comp_type_data.d32.
 * @param  [in]  bus The index of I2C. see @ref i2c_bus_t
 * @return The value of @ref ic_comp_type_data.b.
 */
static inline uint32_t hal_i2c_get_component_type(i2c_bus_t bus)
{
    return g_i2c_regs[bus]->comp_type;
}

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif