/* Copyright (c) 2012 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */

#ifndef NRF_TEMP_H__
#define NRF_TEMP_H__

#include "nrf.h"

/**
* @defgroup nrf_temperature TEMP (temperature) abstraction
* @{
* @ingroup nrf_drivers temperature_example
* @brief Temperature module init and read functions.
*
*/

/**@cond NO_DOXYGEN */
#define MASK_SIGN           (0x00000200UL)
#define MASK_SIGN_EXTENSION (0xFFFFFC00UL)

/**
 * @brief Function for preparing the temp module for temperature measurement.
 *
 * This function initializes the TEMP module and writes to the hidden configuration register.
 */
void hal_temp_init(void);

/**
 * @brief Function for reading temperature measurement.
 *
 * The function reads the 10 bit 2's complement value and transforms it to a 32 bit 2's complement value.
 */
int32_t hal_temp_read(void);
/**@endcond */

/** @} */

#endif