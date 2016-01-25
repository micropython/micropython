/*
 * @brief Common PHY functions
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2012
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

#ifndef __LPC_PHY_H_
#define __LPC_PHY_H_

#include "board.h"
#include "chip.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup BOARD_PHY BOARD: Board specific PHY drivers
 * @ingroup BOARD_Common
 * The simple PHY function API provides simple non-blocking PHY status
 * monitoring and initialization support for various Ethernet PHYs.
 * To initialize the PHY, call lpc_phy_init() once. lpc_phy_init() requires
 * several standard functions from the MAC driver for interfacing to the
 * PHY via a MII link (Chip_ENET_StartMIIWrite(), Chip_ENET_IsMIIBusy(),
 * Chip_ENET_StartMIIRead(), and Chip_ENET_ReadMIIData()).
 *
 * Once initialized, just preiodically call the lpcPHYStsPoll() function
 * from the background loop or a thread and monitor the returned status
 * to determine if the PHY state has changed and the current PHY state.
 * @{
 */
#define PHY_LINK_ERROR     (1 << 0)	/*!< PHY status bit for link error */
#define PHY_LINK_BUSY      (1 << 1)	/*!< PHY status bit for MII link busy */
#define PHY_LINK_CHANGED   (1 << 2)	/*!< PHY status bit for changed state (not persistent) */
#define PHY_LINK_CONNECTED (1 << 3)	/*!< PHY status bit for connected state */
#define PHY_LINK_SPEED100  (1 << 4)	/*!< PHY status bit for 100Mbps mode */
#define PHY_LINK_FULLDUPLX (1 << 5)	/*!< PHY status bit for full duplex mode */

/**
 * @brief	Phy status update state machine
 * @return	An Or'ed value of PHY_LINK_* statuses
 * @note	This function can be called at any rate and will poll the the PHY status. Multiple
 * calls may be needed to determine PHY status.
 */
uint32_t lpcPHYStsPoll(void);

/**
 * @brief	Initialize the PHY
 * @param	rmii			: Initializes PHY for RMII mode if true, MII if false
 * @param	pDelayMsFunc	: Delay function (in mS) used for this driver
 * @return	PHY_LINK_ERROR or 0 on success
 * @note	This function initializes the PHY. It will block until complete. It will not
 * wait for the PHY to detect a connected cable and remain busy. Use lpcPHYStsPoll to
 * detect cable insertion.
 */
uint32_t lpc_phy_init(bool rmii, p_msDelay_func_t pDelayMsFunc);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __LPC_PHY_H_ */
