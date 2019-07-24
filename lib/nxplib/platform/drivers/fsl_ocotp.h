/*
 * Copyright 2019 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _FSL_OCOTP_H_
#define _FSL_OCOTP_H_

#include "fsl_common.h"

/*!
 * @addtogroup ocotp
 * @{
 */

/*******************************************************************************
 * Definitions
 *******************************************************************************/
/*! @name Driver version */
/*@{*/
/*! @brief OCOTP driver version 2.0.0 */
#define FSL_OCOTP_DRIVER_VERSION (MAKE_VERSION(2, 0, 0))
/*@}*/

/*! @brief Error codes for the OCOTP driver. */
enum _ocotp_status
{
    kStatus_OCOTP_AccessError = MAKE_STATUS(kStatusGroup_SDK_OCOTP, 0), /*!< eFuse and shadow register access error. */
    kStatus_OCOTP_CrcFail     = MAKE_STATUS(kStatusGroup_SDK_OCOTP, 1), /*!< CRC check failed. */
};

/*! @brief OCOTP timing structure.
 *  Note that, these value are used for calcalating the read/write timings.
 *  And the values should statisfy below rules:
 *
 *  Tsp_rd=(WAIT+1)/ipg_clk_freq should be >= 150ns;
 *  Tsp_pgm=(RELAX+1)/ipg_clk_freq should be >= 100ns;
 *  Trd = ((STROBE_READ+1)- 2*(RELAX_READ+1)) /ipg_clk_freq,
 *  The Trd is required to be larger than 40 ns.
 *  Tpgm = ((STROBE_PROG+1)- 2*(RELAX_PROG+1)) /ipg_clk_freq;
 *  The Tpgm should be configured within the range of 9000 ns < Tpgm < 11000 ns;
 */
typedef struct _ocotp_timing
{
    uint32_t wait;        /*!< Wait time value to fill in the TIMING register. */
    uint32_t relax;       /*!< Relax time value to fill in the TIMING register. */
    uint32_t strobe_prog; /*!< Storbe program time value to fill in the TIMING register. */
    uint32_t strobe_read; /*!< Storbe read time value to fill in the TIMING register. */
} ocotp_timing_t;

/*******************************************************************************
 * API
 *******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @brief Initializes OCOTP controller.
 *
 * @param base         OCOTP peripheral base address.
 * @param srcClock_Hz  source clock frequency in unit of Hz.
 */
void OCOTP_Init(OCOTP_Type *base, uint32_t srcClock_Hz);

/*!
 * @brief De-initializes OCOTP controller.
 *
 * @retval kStatus_Success upon successful execution, error status otherwise.
 */
void OCOTP_Deinit(OCOTP_Type *base);

/*!
 * @brief Checking the BUSY bit in CTRL register.
 * Checking this BUSY bit will help confirm if the OCOTP controller is ready for access.
 *
 * @param base         OCOTP peripheral base address.
 * @retval       true for bit set and false for cleared.
 */
static inline bool OCOTP_CheckBusyStatus(OCOTP_Type *base)
{
    return ((OCOTP_CTRL_BUSY_MASK == (base->CTRL & OCOTP_CTRL_BUSY_MASK)) ? (true) : (false));
}

/*!
 * @brief Checking the ERROR bit in CTRL register.
 *
 * @param base         OCOTP peripheral base address.
 * @retval       true for bit set and false for cleared.
 */
static inline bool OCOTP_CheckErrorStatus(OCOTP_Type *base)
{
    return ((OCOTP_CTRL_ERROR_MASK == (base->CTRL & OCOTP_CTRL_ERROR_MASK)) ? (true) : (false));
}

/*!
 * @brief Clear the error bit if this bit is set.
 *
 * @param base  OCOTP peripheral base address.
 */
static inline void OCOTP_ClearErrorStatus(OCOTP_Type *base)
{
    base->CTRL_CLR = OCOTP_CTRL_CLR_ERROR_MASK;
}

/*!
 * @brief Reload the shadow register.
 * This function will help reload the shadow register without reseting the OCOTP module.
 * Please make sure the OCOTP has been initialized before calling this API.
 *
 * @param base OCOTP peripheral base addess.
 */
void OCOTP_ReloadShadowRegister(OCOTP_Type *base);

/*!
 * @brief Read the fuse shadow register with the fuse addess.
 *
 * @param base     OCOTP peripheral base address.
 * @param address  the fuse address to be read from.
 */
uint32_t OCOTP_ReadFuseShadowRegister(OCOTP_Type *base, uint32_t address);

/*!
 * @brief Write the fuse shadow register with the fuse addess and data.
 * Please make sure the wrtie address is not locked while calling this API.
 *
 * @param base     OCOTP peripheral base address.
 * @param address  the fuse address to be written.
 * @param data     the value will be writen to fuse address.
 * @retval   write status, kStatus_Success for success and kStatus_Fail for failed.
 */
status_t OCOTP_WriteFuseShadowRegister(OCOTP_Type *base, uint32_t address, uint32_t data);

/*!
 * @brief Get the OCOTP controller version from the register.
 *
 * @param base    OCOTP peripheral base address.
 * @retval  return the version value.
 */
static inline uint32_t OCOTP_GetVersion(OCOTP_Type *base)
{
    return (base->VERSION);
}

#if defined(__cplusplus)
}
#endif

/*! @}*/

#endif /* _FSL_OCOTP_H_ */
