/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides V100 SPI register operation api \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-08-08, Create file. \n
 */
#ifndef HAL_SPI_V100_REGS_OP_H
#define HAL_SPI_V100_REGS_OP_H

#include <stdint.h>
#include "common_def.h"
#include "hal_spi_v100_regs_def.h"
#include "spi_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_spi_v100_regs_op SPI V100 Regs Operation
 * @ingroup  drivers_hal_spi
 * @{
 */

/**
 * @brief  The registers list of ssi interrupt.
 */
typedef enum ssi_int_reg {
    INT_MASK_REG,               /*!< Interrupt Mask Register. */
    INT_STATUS_REG,             /*!< Interrupt Status Register. */
    RAW_INT_STATUS_REG          /*!< Raw Interrupt Status Register. */
} ssi_int_reg_t;

extern uintptr_t g_hal_spis_regs[SPI_BUS_MAX_NUM];
#define spis_regs(bus) ((ssi_regs_t *)g_hal_spis_regs[bus])

/**
 * @brief  Get the value of @ref ctrlr0_data.
 * @param  [in]  bus The index of ssi.
 * @return The value of @ref ctrlr0_data.
 */
static inline uint32_t hal_spi_v100_ctrlr0_get(spi_bus_t bus)
{
    ctrlr0_data_t ctrlr0;
    ctrlr0.d32 = spis_regs(bus)->ctrlr0;
    return ctrlr0.d32;
}

/**
 * @brief  Set the value of @ref ctrlr0_data.
 * @param  [in]  bus The index of ssi.
 * @param  [in]  val The value of @ref ctrlr0_data.
 */
static inline void hal_spi_v100_ctrlr0_set(spi_bus_t bus, uint32_t val)
{
    ctrlr0_data_t ctrlr0;
    ctrlr0.d32 = val;
    spis_regs(bus)->ctrlr0 = ctrlr0.d32;
}

/**
 * @brief  Get the value of @ref ctrlr0_data.dfs.
 * @param  [in]  bus The index of ssi.
 * @return The value of @ref ctrlr0_data.dfs.
 */
static inline uint32_t hal_spi_v100_ctrlr0_get_dfs(spi_bus_t bus)
{
    ctrlr0_data_t ctrlr0;
    ctrlr0.d32 = spis_regs(bus)->ctrlr0;
    return ctrlr0.b.dfs;
}

/**
 * @brief  Set the value of @ref ctrlr0_data.dfs.
 * @param  [in]  bus The index of ssi.
 * @param  [in]  val The value of @ref ctrlr0_data.dfs.
 */
static inline void hal_spi_v100_ctrlr0_set_dfs(spi_bus_t bus, uint32_t val)
{
    ctrlr0_data_t ctrlr0;
    ctrlr0.d32 = spis_regs(bus)->ctrlr0;
    ctrlr0.b.dfs = val;
    spis_regs(bus)->ctrlr0 = ctrlr0.d32;
}

/**
 * @brief  Get the value of @ref ctrlr0_data.frf.
 * @param  [in]  bus The index of ssi.
 * @return The value of @ref ctrlr0_data.frf.
 */
static inline uint32_t hal_spi_v100_ctrlr0_get_frf(spi_bus_t bus)
{
    ctrlr0_data_t ctrlr0;
    ctrlr0.d32 = spis_regs(bus)->ctrlr0;
    return ctrlr0.b.frf;
}

/**
 * @brief  Set the value of @ref ctrlr0_data.frf.
 * @param  [in]  bus The index of ssi.
 * @param  [in]  val The value of @ref ctrlr0_data.frf.
 */
static inline void hal_spi_v100_ctrlr0_set_frf(spi_bus_t bus, uint32_t val)
{
    ctrlr0_data_t ctrlr0;
    ctrlr0.d32 = spis_regs(bus)->ctrlr0;
    ctrlr0.b.frf = val;
    spis_regs(bus)->ctrlr0 = ctrlr0.d32;
}

/**
 * @brief  Get the value of @ref ctrlr0_data.scph.
 * @param  [in]  bus The index of ssi.
 * @return The value of @ref ctrlr0_data.scph.
 */
static inline uint32_t hal_spi_v100_ctrlr0_get_scph(spi_bus_t bus)
{
    ctrlr0_data_t ctrlr0;
    ctrlr0.d32 = spis_regs(bus)->ctrlr0;
    return ctrlr0.b.scph;
}

/**
 * @brief  Set the value of @ref ctrlr0_data.scph.
 * @param  [in]  bus The index of ssi.
 * @param  [in]  val The value of @ref ctrlr0_data.scph.
 */
static inline void hal_spi_v100_ctrlr0_set_scph(spi_bus_t bus, uint32_t val)
{
    ctrlr0_data_t ctrlr0;
    ctrlr0.d32 = spis_regs(bus)->ctrlr0;
    ctrlr0.b.scph = val;
    spis_regs(bus)->ctrlr0 = ctrlr0.d32;
}

/**
 * @brief  Get the value of @ref ctrlr0_data.scpol.
 * @param  [in]  bus The index of ssi.
 * @return The value of @ref ctrlr0_data.scpol.
 */
static inline uint32_t hal_spi_v100_ctrlr0_get_scpol(spi_bus_t bus)
{
    ctrlr0_data_t ctrlr0;
    ctrlr0.d32 = spis_regs(bus)->ctrlr0;
    return ctrlr0.b.scpol;
}

/**
 * @brief  Set the value of @ref ctrlr0_data.scpol.
 * @param  [in]  bus The index of ssi.
 * @param  [in]  val The value of @ref ctrlr0_data.scpol.
 */
static inline void hal_spi_v100_ctrlr0_set_scpol(spi_bus_t bus, uint32_t val)
{
    ctrlr0_data_t ctrlr0;
    ctrlr0.d32 = spis_regs(bus)->ctrlr0;
    ctrlr0.b.scpol = val;
    spis_regs(bus)->ctrlr0 = ctrlr0.d32;
}

/**
 * @brief  Get the value of @ref ctrlr0_data.tmod.
 * @param  [in]  bus The index of ssi.
 * @return The value of @ref ctrlr0_data.tmod.
 */
static inline uint32_t hal_spi_v100_ctrlr0_get_tmod(spi_bus_t bus)
{
    ctrlr0_data_t ctrlr0;
    ctrlr0.d32 = spis_regs(bus)->ctrlr0;
    return ctrlr0.b.tmod;
}

/**
 * @brief  Set the value of @ref ctrlr0_data.tmod.
 * @param  [in]  bus The index of ssi.
 * @param  [in]  val The value of @ref ctrlr0_data.tmod.
 */
static inline void hal_spi_v100_ctrlr0_set_tmod(spi_bus_t bus, uint32_t val)
{
    ctrlr0_data_t ctrlr0;
    ctrlr0.d32 = spis_regs(bus)->ctrlr0;
    ctrlr0.b.tmod = val;
    spis_regs(bus)->ctrlr0 = ctrlr0.d32;
}

/**
 * @brief  Get the value of @ref ctrlr0_data.slv_oe.
 * @param  [in]  bus The index of ssi.
 * @return The value of @ref ctrlr0_data.slv_oe.
 */
static inline uint32_t hal_spi_v100_ctrlr0_get_slv_oe(spi_bus_t bus)
{
    ctrlr0_data_t ctrlr0;
    ctrlr0.d32 = spis_regs(bus)->ctrlr0;
    return ctrlr0.b.slv_oe;
}

/**
 * @brief  Set the value of @ref ctrlr0_data.slv_oe.
 * @param  [in]  bus The index of ssi.
 * @param  [in]  val The value of @ref ctrlr0_data.slv_oe.
 */
static inline void hal_spi_v100_ctrlr0_set_slv_oe(spi_bus_t bus, uint32_t val)
{
    ctrlr0_data_t ctrlr0;
    ctrlr0.d32 = spis_regs(bus)->ctrlr0;
    ctrlr0.b.slv_oe = val;
    spis_regs(bus)->ctrlr0 = ctrlr0.d32;
}

/**
 * @brief  Get the value of @ref ctrlr0_data.srl.
 * @param  [in]  bus The index of ssi.
 * @return The value of @ref ctrlr0_data.srl.
 */
static inline uint32_t hal_spi_v100_ctrlr0_get_srl(spi_bus_t bus)
{
    ctrlr0_data_t ctrlr0;
    ctrlr0.d32 = spis_regs(bus)->ctrlr0;
    return ctrlr0.b.srl;
}

/**
 * @brief  Set the value of @ref ctrlr0_data.srl.
 * @param  [in]  bus The index of ssi.
 * @param  [in]  val The value of @ref ctrlr0_data.srl.
 */
static inline void hal_spi_v100_ctrlr0_set_srl(spi_bus_t bus, uint32_t val)
{
    ctrlr0_data_t ctrlr0;
    ctrlr0.d32 = spis_regs(bus)->ctrlr0;
    ctrlr0.b.srl = val;
    spis_regs(bus)->ctrlr0 = ctrlr0.d32;
}

/**
 * @brief  Get the value of @ref ctrlr0_data.cfs.
 * @param  [in]  bus The index of ssi.
 * @return The value of @ref ctrlr0_data.cfs.
 */
static inline uint32_t hal_spi_v100_ctrlr0_get_cfs(spi_bus_t bus)
{
    ctrlr0_data_t ctrlr0;
    ctrlr0.d32 = spis_regs(bus)->ctrlr0;
    return ctrlr0.b.cfs;
}

/**
 * @brief  Set the value of @ref ctrlr0_data.cfs.
 * @param  [in]  bus The index of ssi.
 * @param  [in]  val The value of @ref ctrlr0_data.cfs.
 */
static inline void hal_spi_v100_ctrlr0_set_cfs(spi_bus_t bus, uint32_t val)
{
    ctrlr0_data_t ctrlr0;
    ctrlr0.d32 = spis_regs(bus)->ctrlr0;
    ctrlr0.b.cfs = val;
    spis_regs(bus)->ctrlr0 = ctrlr0.d32;
}

/**
 * @brief  Get the value of @ref ctrlr0_data.dfs_32.
 * @param  [in]  bus The index of ssi.
 * @return The value of @ref ctrlr0_data.dfs_32.
 */
static inline uint32_t hal_spi_v100_ctrlr0_get_dfs_32(spi_bus_t bus)
{
    ctrlr0_data_t ctrlr0;
    ctrlr0.d32 = spis_regs(bus)->ctrlr0;
    return ctrlr0.b.dfs_32;
}

/**
 * @brief  Set the value of @ref ctrlr0_data.dfs_32.
 * @param  [in]  bus The index of ssi.
 * @param  [in]  val The value of @ref ctrlr0_data.dfs_32.
 */
static inline void hal_spi_v100_ctrlr0_set_dfs_32(spi_bus_t bus, uint32_t val)
{
    ctrlr0_data_t ctrlr0;
    ctrlr0.d32 = spis_regs(bus)->ctrlr0;
    ctrlr0.b.dfs_32 = val;
    spis_regs(bus)->ctrlr0 = ctrlr0.d32;
}

/**
 * @brief  Get the value of @ref ctrlr0_data.spi_frf.
 * @param  [in]  bus The index of ssi.
 * @return The value of @ref ctrlr0_data.spi_frf.
 */
static inline uint32_t hal_spi_v100_ctrlr0_get_spi_frf(spi_bus_t bus)
{
    ctrlr0_data_t ctrlr0;
    ctrlr0.d32 = spis_regs(bus)->ctrlr0;
    return ctrlr0.b.spi_frf;
}

/**
 * @brief  Set the value of @ref ctrlr0_data.spi_frf.
 * @param  [in]  bus The index of ssi.
 * @param  [in]  val The value of @ref ctrlr0_data.spi_frf.
 */
static inline void hal_spi_v100_ctrlr0_set_spi_frf(spi_bus_t bus, uint32_t val)
{
    ctrlr0_data_t ctrlr0;
    ctrlr0.d32 = spis_regs(bus)->ctrlr0;
    ctrlr0.b.spi_frf = val;
    spis_regs(bus)->ctrlr0 = ctrlr0.d32;
}

/**
 * @brief  Get the value of @ref ctrlr0_data.sste.
 * @param  [in]  bus The index of ssi.
 * @return The value of @ref ctrlr0_data.sste.
 */
static inline uint32_t hal_spi_v100_ctrlr0_get_sste(spi_bus_t bus)
{
    ctrlr0_data_t ctrlr0;
    ctrlr0.d32 = spis_regs(bus)->ctrlr0;
    return ctrlr0.b.sste;
}

/**
 * @brief  Set the value of @ref ctrlr0_data.sste.
 * @param  [in]  bus The index of ssi.
 * @param  [in]  val The value of @ref ctrlr0_data.sste.
 */
static inline void hal_spi_v100_ctrlr0_set_sste(spi_bus_t bus, uint32_t val)
{
    ctrlr0_data_t ctrlr0;
    ctrlr0.d32 = spis_regs(bus)->ctrlr0;
    ctrlr0.b.sste = val;
    spis_regs(bus)->ctrlr0 = ctrlr0.d32;
}

/**
 * @brief  Get the value of @ref ctrlr1_data.ndf.
 * @param  [in]  bus The index of ssi.
 * @return The value of @ref ctrlr1_data.ndf.
 */
static inline uint32_t hal_spi_v100_ctrlr1_get_ndf(spi_bus_t bus)
{
    ctrlr1_data_t ctrlr1;
    ctrlr1.d32 = spis_regs(bus)->ctrlr1;
    return ctrlr1.b.ndf;
}

/**
 * @brief  Set the value of @ref ctrlr1_data.ndf.
 * @param  [in]  bus The index of ssi.
 * @param  [in]  val The value of @ref ctrlr1_data.ndf.
 */
static inline void hal_spi_v100_ctrlr1_set_ndf(spi_bus_t bus, uint32_t val)
{
    ctrlr1_data_t ctrlr1;
    ctrlr1.d32 = spis_regs(bus)->ctrlr1;
    ctrlr1.b.ndf = val;
    spis_regs(bus)->ctrlr1 = ctrlr1.d32;
}

/**
 * @brief  Get the value of @ref ssienr_data.ssi_en.
 * @param  [in]  bus The index of ssi.
 * @return The value of @ref ssienr_data.ssi_en.
 */
static inline uint32_t hal_spi_v100_ssienr_get_ssi_en(spi_bus_t bus)
{
    ssienr_data_t ssienr;
    ssienr.d32 = spis_regs(bus)->ssienr;
    return ssienr.b.ssi_en;
}

/**
 * @brief  Set the value of @ref ssienr_data.ssi_en.
 * @param  [in]  bus The index of ssi.
 * @param  [in]  val The value of @ref ssienr_data.ssi_en.
 */
static inline void hal_spi_ssienr_set_ssi_en(spi_bus_t bus, uint32_t val)
{
    ssienr_data_t ssienr;
    ssienr.d32 = spis_regs(bus)->ssienr;
    ssienr.b.ssi_en = val;
    spis_regs(bus)->ssienr = ssienr.d32;
}

/**
 * @brief  Get the value of @ref mwcr_data.mwmod.
 * @param  [in]  bus The index of ssi.
 * @return The value of @ref mwcr_data.mwmod.
 */
static inline uint32_t hal_spi_v100_mwcr_get_mwmod(spi_bus_t bus)
{
    mwcr_data_t mwcr;
    mwcr.d32 = spis_regs(bus)->mwcr;
    return mwcr.b.mwmod;
}

/**
 * @brief  Set the value of @ref mwcr_data.mwmod.
 * @param  [in]  bus The index of ssi.
 * @param  [in]  val The value of @ref mwcr_data.mwmod.
 */
static inline void hal_spi_v100_mwcr_set_mwmod(spi_bus_t bus, uint32_t val)
{
    mwcr_data_t mwcr;
    mwcr.d32 = spis_regs(bus)->mwcr;
    mwcr.b.mwmod = val;
    spis_regs(bus)->mwcr = mwcr.d32;
}

/**
 * @brief  Get the value of @ref mwcr_data.mdd.
 * @param  [in]  bus The index of ssi.
 * @return The value of @ref mwcr_data.mdd.
 */
static inline uint32_t hal_spi_v100_mwcr_get_mdd(spi_bus_t bus)
{
    mwcr_data_t mwcr;
    mwcr.d32 = spis_regs(bus)->mwcr;
    return mwcr.b.mdd;
}

/**
 * @brief  Set the value of @ref mwcr_data.mdd.
 * @param  [in]  bus The index of ssi.
 * @param  [in]  val The value of @ref mwcr_data.mdd.
 */
static inline void hal_spi_v100_mwcr_set_mdd(spi_bus_t bus, uint32_t val)
{
    mwcr_data_t mwcr;
    mwcr.d32 = spis_regs(bus)->mwcr;
    mwcr.b.mdd = val;
    spis_regs(bus)->mwcr = mwcr.d32;
}

/**
 * @brief  Get the value of @ref mwcr_data.mhs.
 * @param  [in]  bus The index of ssi.
 * @return The value of @ref mwcr_data.mhs.
 */
static inline uint32_t hal_spi_v100_mwcr_get_mhs(spi_bus_t bus)
{
    mwcr_data_t mwcr;
    mwcr.d32 = spis_regs(bus)->mwcr;
    return mwcr.b.mhs;
}

/**
 * @brief  Set the value of @ref mwcr_data.mhs.
 * @param  [in]  bus The index of ssi.
 * @param  [in]  val The value of @ref mwcr_data.mhs.
 */
static inline void hal_spi_v100_mwcr_set_mhs(spi_bus_t bus, uint32_t val)
{
    mwcr_data_t mwcr;
    mwcr.d32 = spis_regs(bus)->mwcr;
    mwcr.b.mhs = val;
    spis_regs(bus)->mwcr = mwcr.d32;
}

/**
 * @brief  Get the value of @ref ser_data.ser.
 * @param  [in]  bus The index of ssi.
 * @return The value of @ref ser_data.ser.
 */
static inline uint32_t hal_spi_v100_ser_get_ser(spi_bus_t bus)
{
    ser_data_t ser;
    ser.d32 = spis_regs(bus)->ser;
    return ser.b.ser;
}

/**
 * @brief  Set the value of @ref ser_data.ser.
 * @param  [in]  bus The index of ssi.
 * @param  [in]  val The value of @ref ser_data.ser.
 */
static inline void hal_spi_ser_set_ser(spi_bus_t bus, uint32_t val)
{
    ser_data_t ser;
    ser.d32 = spis_regs(bus)->ser;
    ser.b.ser = val;
    spis_regs(bus)->ser = ser.d32;
}

/**
 * @brief  Get the value of @ref baudr_data.sckdv.
 * @param  [in]  bus The index of ssi.
 * @return The value of @ref baudr_data.sckdv.
 */
static inline uint32_t hal_spi_v100_baudr_get_sckdv(spi_bus_t bus)
{
    baudr_data_t baudr;
    baudr.d32 = spis_regs(bus)->baudr;
    return baudr.b.sckdv;
}

/**
 * @brief  Set the value of @ref baudr_data.sckdv.
 * @param  [in]  bus The index of ssi.
 * @param  [in]  val The value of @ref baudr_data.sckdv.
 */
static inline void hal_spi_baudr_set_sckdv(spi_bus_t bus, uint32_t val)
{
    baudr_data_t baudr;
    baudr.d32 = spis_regs(bus)->baudr;
    baudr.b.sckdv = val;
    spis_regs(bus)->baudr = baudr.d32;
}

/**
 * @brief  Get the value of @ref txftlr_data.tft.
 * @param  [in]  bus The index of ssi.
 * @return The value of @ref txftlr_data.tft.
 */
static inline uint32_t hal_spi_v100_txftlr_get_tft(spi_bus_t bus)
{
    txftlr_data_t txftlr;
    txftlr.d32 = spis_regs(bus)->txftlr;
    return txftlr.b.tft;
}

/**
 * @brief  Set the value of @ref txftlr_data.tft.
 * @param  [in]  bus The index of ssi.
 * @param  [in]  val The value of @ref txftlr_data.tft.
 */
static inline void hal_spi_txftlr_set_tft(spi_bus_t bus, uint32_t val)
{
    txftlr_data_t txftlr;
    txftlr.d32 = spis_regs(bus)->txftlr;
    txftlr.b.tft = val;
    spis_regs(bus)->txftlr = txftlr.d32;
}

/**
 * @brief  Get the value of @ref rxftlr_data.rft.
 * @param  [in]  bus The index of ssi.
 * @return The value of @ref rxftlr_data.rft.
 */
static inline uint32_t hal_spi_v100_rxftlr_get_rft(spi_bus_t bus)
{
    rxftlr_data_t rxftlr;
    rxftlr.d32 = spis_regs(bus)->rxftlr;
    return rxftlr.b.rft;
}

/**
 * @brief  Set the value of @ref rxftlr_data.rft.
 * @param  [in]  bus The index of ssi.
 * @param  [in]  val The value of @ref rxftlr_data.rft.
 */
static inline void hal_spi_rxftlr_set_rft(spi_bus_t bus, uint32_t val)
{
    rxftlr_data_t rxftlr;
    rxftlr.d32 = spis_regs(bus)->rxftlr;
    rxftlr.b.rft = val;
    spis_regs(bus)->rxftlr = rxftlr.d32;
}

/**
 * @brief  Get the value of @ref txflr_data.txtfl.
 * @param  [in]  bus The index of ssi.
 * @return The value of @ref txflr_data.txtfl.
 */
static inline uint32_t hal_spi_txflr_get_txtfl(spi_bus_t bus)
{
    txflr_data_t txflr;
    txflr.d32 = spis_regs(bus)->txflr;
    return txflr.b.txtfl;
}

/**
 * @brief  Get the value of @ref rxflr_data.rxtfl.
 * @param  [in]  bus The index of ssi.
 * @return The value of @ref rxflr_data.rxtfl.
 */
static inline uint32_t hal_spi_rxflr_get_rxtfl(spi_bus_t bus)
{
    rxflr_data_t rxflr;
    rxflr.d32 = spis_regs(bus)->rxflr;
    return rxflr.b.rxtfl;
}

/**
 * @brief  Get the value of @ref sr_data.busy.
 * @param  [in]  bus The index of ssi.
 * @return The value of @ref sr_data.busy.
 */
static inline uint32_t hal_spi_sr_get_busy(spi_bus_t bus)
{
    sr_data_t sr;
    sr.d32 = spis_regs(bus)->sr;
    return sr.b.busy;
}

/**
 * @brief  Get the value of @ref sr_data.tfnf.
 * @param  [in]  bus The index of ssi.
 * @return The value of @ref sr_data.tfnf.
 */
static inline uint32_t hal_spi_v100_sr_get_tfnf(spi_bus_t bus)
{
    sr_data_t sr;
    sr.d32 = spis_regs(bus)->sr;
    return sr.b.tfnf;
}

/**
 * @brief  Get the value of @ref sr_data.tfe.
 * @param  [in]  bus The index of ssi.
 * @return The value of @ref sr_data.tfe.
 */
static inline uint32_t hal_spi_v100_sr_get_tfe(spi_bus_t bus)
{
    sr_data_t sr;
    sr.d32 = spis_regs(bus)->sr;
    return sr.b.tfe;
}

/**
 * @brief  Get the value of @ref sr_data.rfne.
 * @param  [in]  bus The index of ssi.
 * @return The value of @ref sr_data.rfne.
 */
static inline uint32_t hal_spi_v100_sr_get_rfne(spi_bus_t bus)
{
    sr_data_t sr;
    sr.d32 = spis_regs(bus)->sr;
    return sr.b.rfne;
}

/**
 * @brief  Get the value of @ref sr_data.rff.
 * @param  [in]  bus The index of ssi.
 * @return The value of @ref sr_data.rff.
 */
static inline uint32_t hal_spi_v100_sr_get_rff(spi_bus_t bus)
{
    sr_data_t sr;
    sr.d32 = spis_regs(bus)->sr;
    return sr.b.rff;
}

/**
 * @brief  Get the value of @ref sr_data.txe.
 * @param  [in]  bus The index of ssi.
 * @return The value of @ref sr_data.txe.
 */
static inline uint32_t hal_spi_v100_sr_get_txe(spi_bus_t bus)
{
    sr_data_t sr;
    sr.d32 = spis_regs(bus)->sr;
    return sr.b.txe;
}

/**
 * @brief  Get the value of @ref sr_data.dcol.
 * @param  [in]  bus The index of ssi.
 * @return The value of @ref sr_data.dcol.
 */
static inline uint32_t hal_spi_v100_sr_get_dcol(spi_bus_t bus)
{
    sr_data_t sr;
    sr.d32 = spis_regs(bus)->sr;
    return sr.b.dcol;
}

/**
 * @brief  Get the register address of ssi interrupt set interface.
 * @param  [in]  bus The index of ssi.
 * @param  [in]  reg The register need to get.
 * @return The register address of ssi interrupt set interface.
 */
volatile uint32_t *hal_spi_v100_int_set_reg(spi_bus_t bus, ssi_int_reg_t reg);

/**
 * @brief  Get the register address of ssi interrupt get interface.
 * @param  [in]  bus The index of ssi.
 * @param  [in]  reg The register need to get.
 * @return The register address of ssi interrupt get interface.
 */
volatile uint32_t *hal_spi_v100_int_get_reg(spi_bus_t bus, ssi_int_reg_t reg);

/**
 * @brief  Set the value of @ref interrupt_data
 * @param  [in]  bus The index of ssi.
 * @param  [in]  reg The register need to set.
 * @param  [in]  val The value of @ref interrupt_data.
 */
static inline void hal_spi_v100_int_set(spi_bus_t bus, ssi_int_reg_t reg, uint32_t val)
{
    union interrupt_data data;
    volatile uint32_t *reg_addr = hal_spi_v100_int_set_reg(bus, reg);

    data.d32 = val;
    if (reg_addr) {
        *reg_addr = data.d32;
    }
}

/**
 * @brief  Set the value of @ref interrupt_data.txei.
 * @param  [in]  bus The index of ssi.
 * @param  [in]  reg The register need to set.
 * @param  [in]  val The value of @ref interrupt_data.txei.
 */
static inline void hal_spi_v100_int_set_txei(spi_bus_t bus, ssi_int_reg_t reg, uint32_t val)
{
    union interrupt_data data;
    volatile uint32_t *reg_addr = hal_spi_v100_int_set_reg(bus, reg);

    data.d32 = *reg_addr;
    data.b.txei = val;
    *reg_addr = data.d32;
}

/**
 * @brief  Get the value of @ref interrupt_data.txei.
 * @param  [in]  bus The index of ssi.
 * @param  [in]  reg The register need to get.
 * @return The value of @ref interrupt_data.txei.
 */
static inline uint32_t hal_spi_v100_int_get_txei(spi_bus_t bus, ssi_int_reg_t reg)
{
    union interrupt_data data;
    volatile uint32_t *reg_addr = hal_spi_v100_int_get_reg(bus, reg);

    data.d32 = *reg_addr;
    return data.b.txei;
}

/**
 * @brief  Set the value of @ref interrupt_data.txoi.
 * @param  [in]  bus The index of ssi.
 * @param  [in]  reg The register need to set.
 * @param  [in]  val The value of @ref interrupt_data.txoi.
 */
static inline void hal_spi_v100_int_set_txoi(spi_bus_t bus, ssi_int_reg_t reg, uint32_t val)
{
    union interrupt_data data;
    volatile uint32_t *reg_addr = hal_spi_v100_int_set_reg(bus, reg);

    data.d32 = *reg_addr;
    data.b.txoi = val;
    *reg_addr = data.d32;
}

/**
 * @brief  Get the value of @ref interrupt_data.txoi.
 * @param  [in]  bus The index of ssi.
 * @param  [in]  reg The register need to get.
 * @return The value of @ref interrupt_data.txoi.
 */
static inline uint32_t hal_spi_v100_int_get_txoi(spi_bus_t bus, ssi_int_reg_t reg)
{
    union interrupt_data data;
    volatile uint32_t *reg_addr = hal_spi_v100_int_get_reg(bus, reg);

    data.d32 = *reg_addr;
    return data.b.txoi;
}

/**
 * @brief  Set the value of @ref interrupt_data.rxui.
 * @param  [in]  bus The index of ssi.
 * @param  [in]  reg The register need to set.
 * @param  [in]  val The value of @ref interrupt_data.rxui.
 */
static inline void hal_spi_v100_int_set_rxui(spi_bus_t bus, ssi_int_reg_t reg, uint32_t val)
{
    union interrupt_data data;
    volatile uint32_t *reg_addr = hal_spi_v100_int_set_reg(bus, reg);

    data.d32 = *reg_addr;
    data.b.rxui = val;
    *reg_addr = data.d32;
}

/**
 * @brief  Get the value of @ref interrupt_data.rxui.
 * @param  [in]  bus The index of ssi.
 * @param  [in]  reg The register need to get.
 * @return The value of @ref interrupt_data.rxui.
 */
static inline uint32_t hal_spi_v100_int_get_rxui(spi_bus_t bus, ssi_int_reg_t reg)
{
    union interrupt_data data;
    volatile uint32_t *reg_addr = hal_spi_v100_int_get_reg(bus, reg);

    data.d32 = *reg_addr;
    return data.b.rxui;
}

/**
 * @brief  Set the value of @ref interrupt_data.rxoi.
 * @param  [in]  bus The index of ssi.
 * @param  [in]  reg The register need to set.
 * @param  [in]  val The value of @ref interrupt_data.rxoi.
 */
static inline void hal_spi_v100_int_set_rxoi(spi_bus_t bus, ssi_int_reg_t reg, uint32_t val)
{
    union interrupt_data data;
    volatile uint32_t *reg_addr = hal_spi_v100_int_set_reg(bus, reg);

    data.d32 = *reg_addr;
    data.b.rxoi = val;
    *reg_addr = data.d32;
}

/**
 * @brief  Get the value of @ref interrupt_data.rxoi.
 * @param  [in]  bus The index of ssi.
 * @param  [in]  reg The register need to get.
 * @return The value of @ref interrupt_data.rxoi.
 */
static inline uint32_t hal_spi_v100_int_get_rxoi(spi_bus_t bus, ssi_int_reg_t reg)
{
    union interrupt_data data;
    volatile uint32_t *reg_addr = hal_spi_v100_int_get_reg(bus, reg);

    data.d32 = *reg_addr;
    return data.b.rxoi;
}

/**
 * @brief  Set the value of @ref interrupt_data.rxfi.
 * @param  [in]  bus The index of ssi.
 * @param  [in]  reg The register need to set.
 * @param  [in]  val The value of @ref interrupt_data.rxfi.
 */
static inline void hal_spi_v100_int_set_rxfi(spi_bus_t bus, ssi_int_reg_t reg, uint32_t val)
{
    union interrupt_data data;
    volatile uint32_t *reg_addr = hal_spi_v100_int_set_reg(bus, reg);

    data.d32 = *reg_addr;
    data.b.rxfi = val;
    *reg_addr = data.d32;
}

/**
 * @brief  Get the value of @ref interrupt_data.rxfi.
 * @param  [in]  bus The index of ssi.
 * @param  [in]  reg The register need to get.
 * @return The value of @ref interrupt_data.rxfi.
 */
static inline uint32_t hal_spi_v100_int_get_rxfi(spi_bus_t bus, ssi_int_reg_t reg)
{
    union interrupt_data data;
    volatile uint32_t *reg_addr = hal_spi_v100_int_get_reg(bus, reg);

    data.d32 = *reg_addr;
    return data.b.rxfi;
}

/**
 * @brief  Set the value of @ref interrupt_data.msti.
 * @param  [in]  bus The index of ssi.
 * @param  [in]  reg The register need to set.
 * @param  [in]  val The value of @ref interrupt_data.msti.
 */
static inline void hal_spi_v100_int_set_msti(spi_bus_t bus, ssi_int_reg_t reg, uint32_t val)
{
    union interrupt_data data;
    volatile uint32_t *reg_addr = hal_spi_v100_int_set_reg(bus, reg);

    data.d32 = *reg_addr;
    data.b.msti = val;
    *reg_addr = data.d32;
}

/**
 * @brief  Get the value of @ref interrupt_data.msti.
 * @param  [in]  bus The index of ssi.
 * @param  [in]  reg The register need to get.
 * @return The value of @ref interrupt_data.msti.
 */
static inline uint32_t hal_spi_v100_int_get_msti(spi_bus_t bus, ssi_int_reg_t reg)
{
    union interrupt_data data;
    volatile uint32_t *reg_addr = hal_spi_v100_int_get_reg(bus, reg);

    data.d32 = *reg_addr;
    return data.b.msti;
}

/**
 * @brief  Get the value of txoicr @ref txoicr_data_t.
 * @param  [in]  bus The index of ssi.
 * @return The value of txoicr @ref txoicr_data_t.
 */
static inline uint32_t hal_spi_v100_txoicr_get_txoicr(spi_bus_t bus)
{
    txoicr_data_t txoicr;
    txoicr.d32 = spis_regs(bus)->txoicr;
    return txoicr.b.icr;
}

/**
 * @brief  Get the value of rxoicr @ref rxoicr_data_t.
 * @param  [in]  bus The index of ssi.
 * @return The value of rxoicr @ref rxoicr_data_t.
 */
static inline uint32_t hal_spi_v100_rxoicr_get_rxoicr(spi_bus_t bus)
{
    rxoicr_data_t rxoicr;
    rxoicr.d32 = spis_regs(bus)->rxoicr;
    return rxoicr.b.icr;
}

/**
 * @brief  Get the value of rxuicr @ref rxuicr_data_t.
 * @param  [in]  bus The index of ssi.
 * @return The value of rxuicr @ref rxuicr_data_t.
 */
static inline uint32_t hal_spi_v100_rxuicr_get_rxuicr(spi_bus_t bus)
{
    rxuicr_data_t rxuicr;
    rxuicr.d32 = spis_regs(bus)->rxuicr;
    return rxuicr.b.icr;
}

/**
 * @brief  Get the value of msticr @ref msticr_data_t.
 * @param  [in]  bus The index of ssi.
 * @return The value of msticr @ref msticr_data_t.
 */
static inline uint32_t hal_spi_v100_msticr_get_msticr(spi_bus_t bus)
{
    msticr_data_t msticr;
    msticr.d32 = spis_regs(bus)->msticr;
    return msticr.b.icr;
}

/**
 * @brief  Get the value of @ref icr_data.icr.
 * @param  [in]  bus The index of ssi.
 * @return The value of @ref icr_data.icr.
 */
static inline uint32_t hal_spi_v100_icr_get_icr(spi_bus_t bus)
{
    icr_data_t icr;
    icr.d32 = spis_regs(bus)->icr;
    return icr.b.icr;
}

/**
 * @brief  Get the value of @ref dmacr_data.rdmae.
 * @param  [in]  bus The index of ssi.
 * @return The value of @ref dmacr_data.rdmae.
 */
static inline uint32_t hal_spi_v100_dmacr_get_rdmae(spi_bus_t bus)
{
    dmacr_data_t dmacr;
    dmacr.d32 = spis_regs(bus)->dmacr;
    return dmacr.b.rdmae;
}

/**
 * @brief  Set the value of @ref dmacr_data.rdmae.
 * @param  [in]  bus The index of uart.
 * @param  [in]  val The value of @ref dmacr_data.rdmae.
 */
static inline void hal_spi_dmacr_set_rdmae(spi_bus_t bus, uint32_t val)
{
    dmacr_data_t dmacr;
    dmacr.d32 = spis_regs(bus)->dmacr;
    dmacr.b.rdmae = val;
    spis_regs(bus)->dmacr = dmacr.d32;
}

/**
 * @brief  Set the value of @ref dmacr_data.rdmae.
 * @param  [in]  bus The index of uart.
 * @param  [in]  val The value of @ref dmacr_data.rdmae.
 */
static inline void hal_spi_v100_dmacr_set_rdmae(spi_bus_t bus, uint32_t val)
{
    dmacr_data_t dmacr;
    dmacr.d32 = spis_regs(bus)->dmacr;
    dmacr.b.rdmae = val;
    spis_regs(bus)->dmacr = dmacr.d32;
}

/**
 * @brief  Get the value of @ref dmacr_data.tdmae.
 * @param  [in]  bus The index of ssi.
 * @return The value of @ref dmacr_data.tdmae.
 */
static inline uint32_t hal_spi_v100_dmacr_get_tdmae(spi_bus_t bus)
{
    dmacr_data_t dmacr;
    dmacr.d32 = spis_regs(bus)->dmacr;
    return dmacr.b.tdmae;
}

/**
 * @brief  Set the value of @ref dmacr_data.tdmae.
 * @param  [in]  bus The index of uart.
 * @param  [in]  val The value of @ref dmacr_data.tdmae.
 */
static inline void hal_spi_v100_dmacr_set_tdmae(spi_bus_t bus, uint32_t val)
{
    dmacr_data_t dmacr;
    dmacr.d32 = spis_regs(bus)->dmacr;
    dmacr.b.tdmae = val;
    spis_regs(bus)->dmacr = dmacr.d32;
}

/**
 * @brief  Get the value of dmatdl @ref dmatdlr_data_t.
 * @param  [in]  bus The index of ssi.
 * @return The value of dmatdl @ref dmatdlr_data_t.
 */
static inline uint32_t hal_spi_v100_dmatdlr_get_dmatdl(spi_bus_t bus)
{
    dmatdlr_data_t dmatdlr;
    dmatdlr.d32 = spis_regs(bus)->dmatdlr;
    return dmatdlr.b.dmadl;
}

/**
 * @brief  Set the value of dmatdl @ref dmatdlr_data_t.
 * @param  [in]  bus The index of uart.
 * @param  [in]  val The value of dmatdl @ref dmatdlr_data_t.
 */
static inline void hal_spi_v100_dmatdlr_set_dmatdl(spi_bus_t bus, uint32_t val)
{
    dmatdlr_data_t dmatdlr;
    dmatdlr.d32 = spis_regs(bus)->dmatdlr;
    dmatdlr.b.dmadl = val;
    spis_regs(bus)->dmatdlr = dmatdlr.d32;
}

/**
 * @brief  Get the value of dmardl @ref dmardlr_data_t.
 * @param  [in]  bus The index of ssi.
 * @return The value of dmardl @ref dmardlr_data_t.
 */
static inline uint32_t hal_spi_v100_dmardlr_get_dmardl(spi_bus_t bus)
{
    dmardlr_data_t dmardlr;
    dmardlr.d32 = spis_regs(bus)->dmardlr;
    return dmardlr.b.dmadl;
}

/**
 * @brief  Set the value of dmardl @ref dmardlr_data_t.
 * @param  [in]  bus The index of uart.
 * @param  [in]  val The value of dmardl @ref dmardlr_data_t.
 */
static inline void hal_spi_dmardlr_set_dmardl(spi_bus_t bus, uint32_t val)
{
    dmardlr_data_t dmardlr;
    dmardlr.d32 = spis_regs(bus)->dmardlr;
    dmardlr.b.dmadl = val;
    spis_regs(bus)->dmardlr = dmardlr.d32;
}

/**
 * @brief  Set the value of dmardl @ref dmardlr_data_t.
 * @param  [in]  bus The index of uart.
 * @param  [in]  val The value of dmardl @ref dmardlr_data_t.
 */
static inline void hal_spi_v100_dmardlr_set_dmardl(spi_bus_t bus, uint32_t val)
{
    dmardlr_data_t dmardlr;
    dmardlr.d32 = spis_regs(bus)->dmardlr;
    dmardlr.b.dmadl = val;
    spis_regs(bus)->dmardlr = dmardlr.d32;
}

/**
 * @brief  Get the value of @ref idr_data.idcode.
 * @param  [in]  bus The index of ssi.
 * @return The value of @ref idr_data.idcode.
 */
static inline uint32_t hal_spi_v100_idr_get_idcode(spi_bus_t bus)
{
    idr_data_t idr;
    idr.d32 = spis_regs(bus)->idr;
    return idr.b.idcode;
}

/**
 * @brief  Get the value of @ref ssi_version_id_data.ssi_comp_version.
 * @param  [in]  bus The index of ssi.
 * @return The value of @ref ssi_version_id_data.ssi_comp_version.
 */
static inline uint32_t hal_spi_v100_ssi_version_id_get_ssi_comp_version(spi_bus_t bus)
{
    ssi_version_id_data_t ssi_version_id;
    ssi_version_id.d32 = spis_regs(bus)->ssi_version_id;
    return ssi_version_id.b.ssi_comp_version;
}

/**
 * @brief  Get data from data register.
 * @param  [in]  bus The index of ssi.
 * @return The data from data register.
 */
static inline uint32_t hal_spi_dr_get_dr(spi_bus_t bus)
{
    return spis_regs(bus)->dr[0];
}

/**
 * @brief  Set data into data register.
 * @param  [in]  bus The index of ssi.
 * @param  [in]  val The data need to set into data register.
 */
static inline void hal_spi_dr_set_dr(spi_bus_t bus, uint32_t val)
{
    spis_regs(bus)->dr[0] = val;
}

/**
 * @brief  Get data from data register.
 * @param  [in]  bus The index of ssi.
 * @return The data from data register.
 */
static inline uint32_t hal_spi_v100_dr_get_dr(spi_bus_t bus)
{
    return spis_regs(bus)->dr[0];
}

/**
 * @brief  Set data into data register.
 * @param  [in]  bus The index of ssi.
 * @param  [in]  val The data need to set into data register.
 */
static inline void hal_spi_v100_dr_set_dr(spi_bus_t bus, uint32_t val)
{
    spis_regs(bus)->dr[0] = val;
}

/**
 * @brief  Get the value of @ref rx_sample_dly_data.rsd.
 * @param  [in]  bus The index of ssi.
 * @return The value of @ref rx_sample_dly_data.rsd.
 */
static inline uint32_t hal_spi_v100_rx_sample_dly_get_rsd(spi_bus_t bus)
{
    rx_sample_dly_data_t rx_sample_dly;
    rx_sample_dly.d32 = spis_regs(bus)->rx_sample_dly;
    return rx_sample_dly.b.rsd;
}

/**
 * @brief  Set the value of @ref rx_sample_dly_data.rsd.
 * @param  [in]  bus The index of uart.
 * @param  [in]  val The value of @ref rx_sample_dly_data.rsd.
 */
static inline void hal_spi_rx_sample_dly_set_rsd(spi_bus_t bus, uint32_t val)
{
    rx_sample_dly_data_t rx_sample_dly;
    rx_sample_dly.d32 = spis_regs(bus)->rx_sample_dly;
    rx_sample_dly.b.rsd = val;
    spis_regs(bus)->rx_sample_dly = rx_sample_dly.d32;
}

/**
 * @brief  Set the value of @ref rx_sample_dly_data.rsd.
 * @param  [in]  bus The index of uart.
 * @param  [in]  val The value of @ref rx_sample_dly_data.rsd.
 */
static inline void hal_spi_v100_rx_sample_dly_set_rsd(spi_bus_t bus, uint32_t val)
{
    rx_sample_dly_data_t rx_sample_dly;
    rx_sample_dly.d32 = spis_regs(bus)->rx_sample_dly;
    rx_sample_dly.b.rsd = val;
    spis_regs(bus)->rx_sample_dly = rx_sample_dly.d32;
}

/**
 * @brief  Get the value of @ref spi_ctrlr0_data.
 * @param  [in]  bus The index of ssi.
 * @return The value of @ref spi_ctrlr0_data.
 */
static inline uint32_t hal_spi_v100_spi_ctrlr0_get(spi_bus_t bus)
{
    spi_ctrlr0_data_t spi_ctrlr0;
    spi_ctrlr0.d32 = spis_regs(bus)->spi_ctrlr0;
    return spi_ctrlr0.d32;
}

/**
 * @brief  Set the value of @ref spi_ctrlr0_data.
 * @param  [in]  bus The index of uart.
 * @param  [in]  val The value of @ref spi_ctrlr0_data.
 */
static inline void hal_spi_v100_spi_ctrlr0_set(spi_bus_t bus, uint32_t val)
{
    spi_ctrlr0_data_t spi_ctrlr0;
    spi_ctrlr0.d32 = val;
    spis_regs(bus)->spi_ctrlr0 = spi_ctrlr0.d32;
}

/**
 * @brief  Get the value of @ref spi_ctrlr0_data.trans_type.
 * @param  [in]  bus The index of ssi.
 * @return The value of @ref spi_ctrlr0_data.trans_type.
 */
static inline uint32_t hal_spi_v100_spi_ctrlr0_get_trans_type(spi_bus_t bus)
{
    spi_ctrlr0_data_t spi_ctrlr0;
    spi_ctrlr0.d32 = spis_regs(bus)->spi_ctrlr0;
    return spi_ctrlr0.b.trans_type;
}

/**
 * @brief  Set the value of @ref spi_ctrlr0_data.trans_type.
 * @param  [in]  bus The index of uart.
 * @param  [in]  val The value of @ref spi_ctrlr0_data.trans_type.
 */
static inline void hal_spi_v100_spi_ctrlr0_set_trans_type(spi_bus_t bus, uint32_t val)
{
    spi_ctrlr0_data_t spi_ctrlr0;
    spi_ctrlr0.d32 = spis_regs(bus)->spi_ctrlr0;
    spi_ctrlr0.b.trans_type = val;
    spis_regs(bus)->spi_ctrlr0 = spi_ctrlr0.d32;
}

/**
 * @brief  Get the value of @ref spi_ctrlr0_data.addr_l.
 * @param  [in]  bus The index of ssi.
 * @return The value of @ref spi_ctrlr0_data.addr_l.
 */
static inline uint32_t hal_spi_v100_spi_ctrlr0_get_addr_l(spi_bus_t bus)
{
    spi_ctrlr0_data_t spi_ctrlr0;
    spi_ctrlr0.d32 = spis_regs(bus)->spi_ctrlr0;
    return spi_ctrlr0.b.addr_l;
}

/**
 * @brief  Set the value of @ref spi_ctrlr0_data.addr_l.
 * @param  [in]  bus The index of uart.
 * @param  [in]  val The value of @ref spi_ctrlr0_data.addr_l.
 */
static inline void hal_spi_v100_spi_ctrlr0_set_addr_l(spi_bus_t bus, uint32_t val)
{
    spi_ctrlr0_data_t spi_ctrlr0;
    spi_ctrlr0.d32 = spis_regs(bus)->spi_ctrlr0;
    spi_ctrlr0.b.addr_l = val;
    spis_regs(bus)->spi_ctrlr0 = spi_ctrlr0.d32;
}

/**
 * @brief  Get the value of @ref spi_ctrlr0_data.inst_l.
 * @param  [in]  bus The index of ssi.
 * @return The value of @ref spi_ctrlr0_data.inst_l.
 */
static inline uint32_t hal_spi_v100_spi_ctrlr0_get_inst_l(spi_bus_t bus)
{
    spi_ctrlr0_data_t spi_ctrlr0;
    spi_ctrlr0.d32 = spis_regs(bus)->spi_ctrlr0;
    return spi_ctrlr0.b.inst_l;
}

/**
 * @brief  Set the value of @ref spi_ctrlr0_data.inst_l.
 * @param  [in]  bus The index of uart.
 * @param  [in]  val The value of @ref spi_ctrlr0_data.inst_l.
 */
static inline void hal_spi_v100_spi_ctrlr0_set_inst_l(spi_bus_t bus, uint32_t val)
{
    spi_ctrlr0_data_t spi_ctrlr0;
    spi_ctrlr0.d32 = spis_regs(bus)->spi_ctrlr0;
    spi_ctrlr0.b.inst_l = val;
    spis_regs(bus)->spi_ctrlr0 = spi_ctrlr0.d32;
}

/**
 * @brief  Get the value of @ref spi_ctrlr0_data.wait_cycles.
 * @param  [in]  bus The index of ssi.
 * @return The value of @ref spi_ctrlr0_data.wait_cycles.
 */
static inline uint32_t hal_spi_v100_spi_ctrlr0_get_wait_cycles(spi_bus_t bus)
{
    spi_ctrlr0_data_t spi_ctrlr0;
    spi_ctrlr0.d32 = spis_regs(bus)->spi_ctrlr0;
    return spi_ctrlr0.b.wait_cycles;
}

/**
 * @brief  Set the value of @ref spi_ctrlr0_data.wait_cycles.
 * @param  [in]  bus The index of uart.
 * @param  [in]  val The value of @ref spi_ctrlr0_data.wait_cycles.
 */
static inline void hal_spi_v100_spi_ctrlr0_set_wait_cycles(spi_bus_t bus, uint32_t val)
{
    spi_ctrlr0_data_t spi_ctrlr0;
    spi_ctrlr0.d32 = spis_regs(bus)->spi_ctrlr0;
    spi_ctrlr0.b.wait_cycles = val;
    spis_regs(bus)->spi_ctrlr0 = spi_ctrlr0.d32;
}

/**
 * @brief  Get the value of @ref spi_ctrlr0_data.spi_ddr_en.
 * @param  [in]  bus The index of ssi.
 * @return The value of @ref spi_ctrlr0_data.spi_ddr_en.
 */
static inline uint32_t hal_spi_v100_spi_ctrlr0_get_spi_ddr_en(spi_bus_t bus)
{
    spi_ctrlr0_data_t spi_ctrlr0;
    spi_ctrlr0.d32 = spis_regs(bus)->spi_ctrlr0;
    return spi_ctrlr0.b.spi_ddr_en;
}

/**
 * @brief  Set the value of @ref spi_ctrlr0_data.spi_ddr_en.
 * @param  [in]  bus The index of uart.
 * @param  [in]  val The value of @ref spi_ctrlr0_data.spi_ddr_en.
 */
static inline void hal_spi_v100_spi_ctrlr0_set_spi_ddr_en(spi_bus_t bus, uint32_t val)
{
    spi_ctrlr0_data_t spi_ctrlr0;
    spi_ctrlr0.d32 = spis_regs(bus)->spi_ctrlr0;
    spi_ctrlr0.b.spi_ddr_en = val;
    spis_regs(bus)->spi_ctrlr0 = spi_ctrlr0.d32;
}

/**
 * @brief  Get the value of @ref spi_ctrlr0_data.spi_rxds_en.
 * @param  [in]  bus The index of ssi.
 * @return The value of @ref spi_ctrlr0_data.spi_rxds_en.
 */
static inline uint32_t hal_spi_v100_spi_ctrlr0_get_spi_rxds_en(spi_bus_t bus)
{
    spi_ctrlr0_data_t spi_ctrlr0;
    spi_ctrlr0.d32 = spis_regs(bus)->spi_ctrlr0;
    return spi_ctrlr0.b.spi_rxds_en;
}

/**
 * @brief  Set the value of @ref spi_ctrlr0_data.spi_rxds_en.
 * @param  [in]  bus The index of uart.
 * @param  [in]  val The value of @ref spi_ctrlr0_data.spi_rxds_en.
 */
static inline void hal_spi_v100_spi_ctrlr0_set_spi_rxds_en(spi_bus_t bus, uint32_t val)
{
    spi_ctrlr0_data_t spi_ctrlr0;
    spi_ctrlr0.d32 = spis_regs(bus)->spi_ctrlr0;
    spi_ctrlr0.b.spi_rxds_en = val;
    spis_regs(bus)->spi_ctrlr0 = spi_ctrlr0.d32;
}

/**
 * @brief  Get the value of @ref txd_drive_edge_data.tde.
 * @param  [in]  bus The index of ssi.
 * @return The value of @ref txd_drive_edge_data.tde.
 */
static inline uint32_t hal_spi_v100_txd_drive_edge_get_tde(spi_bus_t bus)
{
    txd_drive_edge_data_t txd_drive_edge;
    txd_drive_edge.d32 = spis_regs(bus)->txd_drive_edge;
    return txd_drive_edge.b.tde;
}

/**
 * @brief  Set the value of @ref txd_drive_edge_data.tde.
 * @param  [in]  bus The index of uart.
 * @param  [in]  val The value of @ref txd_drive_edge_data.tde.
 */
static inline void hal_spi_v100_txd_drive_edge_set_tde(spi_bus_t bus, uint32_t val)
{
    txd_drive_edge_data_t txd_drive_edge;
    txd_drive_edge.d32 = spis_regs(bus)->txd_drive_edge;
    txd_drive_edge.b.tde = val;
    spis_regs(bus)->txd_drive_edge = txd_drive_edge.d32;
}

/**
 * @brief  Set the value of @ref imr_data_t.rxfi.
 * @param  [in]  bus The index of uart.
 * @param  [in]  val The value of @ref imr_data_t.rxfi.
 */
static inline void hal_spi_v100_imr_set_rxfi(spi_bus_t bus, uint32_t val)
{
    imr_data_t imr;
    imr.d32 = spis_regs(bus)->imr;
    imr.b.rxfi = val;
    spis_regs(bus)->imr = imr.d32;
}

/**
 * @brief  Set the value of @ref imr_data_t.txei.
 * @param  [in]  bus The index of uart.
 * @param  [in]  val The value of @ref imr_data_t.txei.
 */
static inline void hal_spi_v100_imr_set_txei(spi_bus_t bus, uint32_t val)
{
    imr_data_t imr;
    imr.d32 = spis_regs(bus)->imr;
    imr.b.txei = val;
    spis_regs(bus)->imr = imr.d32;
}

/**
 * @brief  Set the value of @ref imr_data_t.msti.
 * @param  [in]  bus The index of uart.
 * @param  [in]  val The value of @ref imr_data_t.msti.
 */
static inline void hal_spi_v100_imr_set_msti(spi_bus_t bus, uint32_t val)
{
    imr_data_t imr;
    imr.d32 = spis_regs(bus)->imr;
    imr.b.msti = val;
    spis_regs(bus)->imr = imr.d32;
}

/**
 * @brief  Get the value of @ref isr_data_t.txei.
 * @param  [in]  bus The index of ssi.
 * @return The value of @ref isr_data_t.txei.
 */
static inline uint32_t hal_spi_v100_isr_get_txei(spi_bus_t bus)
{
    isr_data_t sr;
    sr.d32 = spis_regs(bus)->isr;
    return sr.b.txei;
}

/**
 * @brief  Get the value of @ref isr_data_t.txoi.
 * @param  [in]  bus The index of ssi.
 * @return The value of @ref isr_data_t.txoi.
 */
static inline uint32_t hal_spi_v100_isr_get_txoi(spi_bus_t bus)
{
    isr_data_t sr;
    sr.d32 = spis_regs(bus)->isr;
    return sr.b.txoi;
}

/**
 * @brief  Get the value of @ref isr_data_t.rxfi.
 * @param  [in]  bus The index of ssi.
 * @return The value of @ref isr_data_t.rxfi.
 */
static inline uint32_t hal_spi_v100_isr_get_rxfi(spi_bus_t bus)
{
    isr_data_t sr;
    sr.d32 = spis_regs(bus)->isr;
    return sr.b.rxfi;
}

/**
 * @brief  Get the value of @ref isr_data_t.rxui.
 * @param  [in]  bus The index of ssi.
 * @return The value of @ref isr_data_t.rxui.
 */
static inline uint32_t hal_spi_v100_isr_get_rxui(spi_bus_t bus)
{
    isr_data_t sr;
    sr.d32 = spis_regs(bus)->isr;
    return sr.b.rxui;
}

/**
 * @brief  Get the value of @ref isr_data_t.rxoi.
 * @param  [in]  bus The index of ssi.
 * @return The value of @ref isr_data_t.rxoi.
 */
static inline uint32_t hal_spi_v100_isr_get_rxoi(spi_bus_t bus)
{
    isr_data_t sr;
    sr.d32 = spis_regs(bus)->isr;
    return sr.b.rxoi;
}

/**
 * @brief  Get the value of @ref isr_data_t.msti.
 * @param  [in]  bus The index of ssi.
 * @return The value of @ref isr_data_t.msti.
 */
static inline uint32_t hal_spi_v100_isr_get_msti(spi_bus_t bus)
{
    isr_data_t sr;
    sr.d32 = spis_regs(bus)->isr;
    return sr.b.msti;
}

/**
 * @brief  Set the value of @ref icr_data_t.icr.
 * @param  [in]  bus The index of uart.
 */
static inline void hal_spi_v100_icr_set_any(spi_bus_t bus)
{
    icr_data_t icr_data;
    icr_data.d32 = spis_regs(bus)->icr;
    icr_data.b.icr = 1;
    spis_regs(bus)->icr = icr_data.d32;
}

/**
 * @brief  Reserved.
 */
#define HAL_SPI_RSVD_NONE   0
#define HAL_SPI_RSVD_X8     0
#define HAL_SPI_RSVD_X8_X8  1
#define HAL_SPI_RSVD_X16    0x11

/**
 * @brief  Get the value of rsvd.
 * @param  [in]  bus The index of ssi.
 * @return The value of rsvd.
 */
static inline uint32_t hal_spi_v100_rsvd_get(spi_bus_t bus)
{
    return spis_regs(bus)->rsvd;
}

/**
 * @brief  Set the value of rsvd.
 * @param  [in]  bus The index of ssi.
 * @param  [in]  val The value of rsvd.
 */
static inline void hal_spi_v100_rsvd_set(spi_bus_t bus, uint32_t val)
{
    spis_regs(bus)->rsvd = val;
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