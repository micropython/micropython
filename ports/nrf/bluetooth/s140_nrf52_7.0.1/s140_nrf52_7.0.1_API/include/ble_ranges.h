/*
 * Copyright (c) 2012 - 2018, Nordic Semiconductor ASA
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
  @addtogroup BLE_COMMON
  @{
  @defgroup ble_ranges Module specific SVC, event and option number subranges
  @{

  @brief Definition of SVC, event and option number subranges for each API module.

  @note
  SVCs, event and option numbers are split into subranges for each API module.
  Each module receives its entire allocated range of SVC calls, whether implemented or not,
  but return BLE_ERROR_NOT_SUPPORTED for unimplemented or undefined calls in its range.

  Note that the symbols BLE_<module>_SVC_LAST is the end of the allocated SVC range,
  rather than the last SVC function call actually defined and implemented.

  Specific SVC, event and option values are defined in each module's ble_<module>.h file,
  which defines names of each individual SVC code based on the range start value.
*/

#ifndef BLE_RANGES_H__
#define BLE_RANGES_H__

#ifdef __cplusplus
extern "C" {
#endif

#define BLE_SVC_BASE           0x60       /**< Common BLE SVC base. */
#define BLE_SVC_LAST           0x6B       /**< Common BLE SVC last. */

#define BLE_GAP_SVC_BASE       0x6C       /**< GAP BLE SVC base. */
#define BLE_GAP_SVC_LAST       0x9A       /**< GAP BLE SVC last. */

#define BLE_GATTC_SVC_BASE     0x9B       /**< GATTC BLE SVC base. */
#define BLE_GATTC_SVC_LAST     0xA7       /**< GATTC BLE SVC last. */

#define BLE_GATTS_SVC_BASE     0xA8       /**< GATTS BLE SVC base. */
#define BLE_GATTS_SVC_LAST     0xB7       /**< GATTS BLE SVC last. */

#define BLE_L2CAP_SVC_BASE     0xB8       /**< L2CAP BLE SVC base. */
#define BLE_L2CAP_SVC_LAST     0xBF       /**< L2CAP BLE SVC last. */


#define BLE_EVT_INVALID        0x00       /**< Invalid BLE Event. */

#define BLE_EVT_BASE           0x01       /**< Common BLE Event base. */
#define BLE_EVT_LAST           0x0F       /**< Common BLE Event last. */

#define BLE_GAP_EVT_BASE       0x10       /**< GAP BLE Event base. */
#define BLE_GAP_EVT_LAST       0x2F       /**< GAP BLE Event last. */

#define BLE_GATTC_EVT_BASE     0x30       /**< GATTC BLE Event base. */
#define BLE_GATTC_EVT_LAST     0x4F       /**< GATTC BLE Event last. */

#define BLE_GATTS_EVT_BASE     0x50       /**< GATTS BLE Event base. */
#define BLE_GATTS_EVT_LAST     0x6F       /**< GATTS BLE Event last. */

#define BLE_L2CAP_EVT_BASE     0x70       /**< L2CAP BLE Event base. */
#define BLE_L2CAP_EVT_LAST     0x8F       /**< L2CAP BLE Event last. */


#define BLE_OPT_INVALID        0x00       /**< Invalid BLE Option. */

#define BLE_OPT_BASE           0x01       /**< Common BLE Option base. */
#define BLE_OPT_LAST           0x1F       /**< Common BLE Option last. */

#define BLE_GAP_OPT_BASE       0x20       /**< GAP BLE Option base. */
#define BLE_GAP_OPT_LAST       0x3F       /**< GAP BLE Option last. */

#define BLE_GATT_OPT_BASE      0x40       /**< GATT BLE Option base. */
#define BLE_GATT_OPT_LAST      0x5F       /**< GATT BLE Option last. */

#define BLE_GATTC_OPT_BASE     0x60       /**< GATTC BLE Option base. */
#define BLE_GATTC_OPT_LAST     0x7F       /**< GATTC BLE Option last. */

#define BLE_GATTS_OPT_BASE     0x80       /**< GATTS BLE Option base. */
#define BLE_GATTS_OPT_LAST     0x9F       /**< GATTS BLE Option last. */

#define BLE_L2CAP_OPT_BASE     0xA0       /**< L2CAP BLE Option base. */
#define BLE_L2CAP_OPT_LAST     0xBF       /**< L2CAP BLE Option last. */


#define BLE_CFG_INVALID        0x00       /**< Invalid BLE configuration. */

#define BLE_CFG_BASE           0x01       /**< Common BLE configuration base. */
#define BLE_CFG_LAST           0x1F       /**< Common BLE configuration last. */

#define BLE_CONN_CFG_BASE      0x20       /**< BLE connection configuration base. */
#define BLE_CONN_CFG_LAST      0x3F       /**< BLE connection configuration last. */

#define BLE_GAP_CFG_BASE       0x40       /**< GAP BLE configuration base. */
#define BLE_GAP_CFG_LAST       0x5F       /**< GAP BLE configuration last. */

#define BLE_GATT_CFG_BASE      0x60       /**< GATT BLE configuration base. */
#define BLE_GATT_CFG_LAST      0x7F       /**< GATT BLE configuration last. */

#define BLE_GATTC_CFG_BASE     0x80       /**< GATTC BLE configuration base. */
#define BLE_GATTC_CFG_LAST     0x9F       /**< GATTC BLE configuration last. */

#define BLE_GATTS_CFG_BASE     0xA0       /**< GATTS BLE configuration base. */
#define BLE_GATTS_CFG_LAST     0xBF       /**< GATTS BLE configuration last. */

#define BLE_L2CAP_CFG_BASE     0xC0       /**< L2CAP BLE configuration base. */
#define BLE_L2CAP_CFG_LAST     0xDF       /**< L2CAP BLE configuration last. */





#ifdef __cplusplus
}
#endif
#endif /* BLE_RANGES_H__ */

/**
  @}
  @}
*/
