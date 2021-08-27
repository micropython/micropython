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
  @addtogroup BLE_COMMON BLE SoftDevice Common
  @{
  @defgroup ble_api Events, type definitions and API calls
  @{

  @brief Module independent events, type definitions and API calls for the BLE SoftDevice.

 */

#ifndef BLE_H__
#define BLE_H__

#include <stdint.h>
#include "nrf_svc.h"
#include "nrf_error.h"
#include "ble_err.h"
#include "ble_gap.h"
#include "ble_l2cap.h"
#include "ble_gatt.h"
#include "ble_gattc.h"
#include "ble_gatts.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup BLE_COMMON_ENUMERATIONS Enumerations
 * @{ */

/**
 * @brief Common API SVC numbers.
 */
enum BLE_COMMON_SVCS
{
    SD_BLE_ENABLE = BLE_SVC_BASE,       /**< Enable and initialize the BLE stack */
    SD_BLE_EVT_GET,                     /**< Get an event from the pending events queue. */
    SD_BLE_UUID_VS_ADD,                 /**< Add a Vendor Specific base UUID. */
    SD_BLE_UUID_DECODE,                 /**< Decode UUID bytes. */
    SD_BLE_UUID_ENCODE,                 /**< Encode UUID bytes. */
    SD_BLE_VERSION_GET,                 /**< Get the local version information (company ID, Link Layer Version, Link Layer Subversion). */
    SD_BLE_USER_MEM_REPLY,              /**< User Memory Reply. */
    SD_BLE_OPT_SET,                     /**< Set a BLE option. */
    SD_BLE_OPT_GET,                     /**< Get a BLE option. */
    SD_BLE_CFG_SET,                     /**< Add a configuration to the BLE stack. */
    SD_BLE_UUID_VS_REMOVE,              /**< Remove a Vendor Specific base UUID. */
};

/**
 * @brief BLE Module Independent Event IDs.
 */
enum BLE_COMMON_EVTS
{
    BLE_EVT_USER_MEM_REQUEST = BLE_EVT_BASE + 0, /**< User Memory request. @ref ble_evt_user_mem_request_t */
    BLE_EVT_USER_MEM_RELEASE = BLE_EVT_BASE + 1, /**< User Memory release. @ref ble_evt_user_mem_release_t */
};

/**@brief BLE Connection Configuration IDs.
 *
 * IDs that uniquely identify a connection configuration.
 */
enum BLE_CONN_CFGS
{
    BLE_CONN_CFG_GAP   = BLE_CONN_CFG_BASE + 0, /**< BLE GAP specific connection configuration. */
    BLE_CONN_CFG_GATTC = BLE_CONN_CFG_BASE + 1, /**< BLE GATTC specific connection configuration. */
    BLE_CONN_CFG_GATTS = BLE_CONN_CFG_BASE + 2, /**< BLE GATTS specific connection configuration. */
    BLE_CONN_CFG_GATT  = BLE_CONN_CFG_BASE + 3, /**< BLE GATT specific connection configuration. */
    BLE_CONN_CFG_L2CAP = BLE_CONN_CFG_BASE + 4, /**< BLE L2CAP specific connection configuration. */
};

/**@brief BLE Common Configuration IDs.
 *
 * IDs that uniquely identify a common configuration.
 */
enum BLE_COMMON_CFGS
{
    BLE_COMMON_CFG_VS_UUID = BLE_CFG_BASE, /**< Vendor specific base UUID configuration */
};

/**@brief Common Option IDs.
 * IDs that uniquely identify a common option.
 */
enum BLE_COMMON_OPTS
{
    BLE_COMMON_OPT_PA_LNA          = BLE_OPT_BASE + 0,/**< PA and LNA options */
    BLE_COMMON_OPT_CONN_EVT_EXT    = BLE_OPT_BASE + 1,/**< Extended connection events option */
    BLE_COMMON_OPT_EXTENDED_RC_CAL = BLE_OPT_BASE + 2, /**< Extended RC calibration option */
};

/** @} */

/** @addtogroup BLE_COMMON_DEFINES Defines
 * @{ */

/** @brief  Required pointer alignment for BLE Events.
*/
#define BLE_EVT_PTR_ALIGNMENT    4

/** @brief  Leaves the maximum of the two arguments.
*/
#define BLE_MAX(a, b) ((a) < (b) ? (b) : (a))

/** @brief  Maximum possible length for BLE Events.
 * @note The highest value used for @ref ble_gatt_conn_cfg_t::att_mtu in any connection configuration shall be used as a parameter.
 * If that value has not been configured for any connections then @ref BLE_GATT_ATT_MTU_DEFAULT must be used instead.
*/
#define BLE_EVT_LEN_MAX(ATT_MTU) ( \
    offsetof(ble_evt_t, evt.gattc_evt.params.prim_srvc_disc_rsp.services) + ((ATT_MTU)-1) / 4 * sizeof(ble_gattc_service_t) \
    )

/** @defgroup BLE_USER_MEM_TYPES User Memory Types
 * @{ */
#define BLE_USER_MEM_TYPE_INVALID               0x00  /**< Invalid User Memory Types. */
#define BLE_USER_MEM_TYPE_GATTS_QUEUED_WRITES   0x01  /**< User Memory for GATTS queued writes. */
/** @} */

/** @defgroup BLE_UUID_VS_COUNTS Vendor Specific base UUID counts
 * @{
 */
#define BLE_UUID_VS_COUNT_DEFAULT 10  /**< Default VS UUID count. */
#define BLE_UUID_VS_COUNT_MAX     254 /**< Maximum VS UUID count. */
/** @} */

/** @defgroup BLE_COMMON_CFG_DEFAULTS Configuration defaults.
 * @{
 */
#define BLE_CONN_CFG_TAG_DEFAULT  0    /**< Default configuration tag, SoftDevice default connection configuration. */

/** @} */

/** @} */

/** @addtogroup BLE_COMMON_STRUCTURES Structures
 * @{ */

/**@brief User Memory Block. */
typedef struct
{
    uint8_t *p_mem;             /**< Pointer to the start of the user memory block. */
    uint16_t len;               /**< Length in bytes of the user memory block. */
} ble_user_mem_block_t;

/**@brief Event structure for @ref BLE_EVT_USER_MEM_REQUEST. */
typedef struct
{
    uint8_t type;                       /**< User memory type, see @ref BLE_USER_MEM_TYPES. */
} ble_evt_user_mem_request_t;

/**@brief Event structure for @ref BLE_EVT_USER_MEM_RELEASE. */
typedef struct
{
    uint8_t type;                         /**< User memory type, see @ref BLE_USER_MEM_TYPES. */
    ble_user_mem_block_t mem_block;       /**< User memory block */
} ble_evt_user_mem_release_t;

/**@brief Event structure for events not associated with a specific function module. */
typedef struct
{
    uint16_t conn_handle;                               /**< Connection Handle on which this event occurred. */
    union
    {
        ble_evt_user_mem_request_t user_mem_request;     /**< User Memory Request Event Parameters. */
        ble_evt_user_mem_release_t user_mem_release;     /**< User Memory Release Event Parameters. */
    } params;                                            /**< Event parameter union. */
} ble_common_evt_t;

/**@brief BLE Event header. */
typedef struct
{
    uint16_t evt_id;              /**< Value from a BLE_<module>_EVT series. */
    uint16_t evt_len;             /**< Length in octets including this header. */
} ble_evt_hdr_t;

/**@brief Common BLE Event type, wrapping the module specific event reports. */
typedef struct
{
    ble_evt_hdr_t header;         /**< Event header. */
    union
    {
        ble_common_evt_t common_evt; /**< Common Event, evt_id in BLE_EVT_* series. */
        ble_gap_evt_t gap_evt;    /**< GAP originated event, evt_id in BLE_GAP_EVT_* series. */
        ble_gattc_evt_t gattc_evt; /**< GATT client originated event, evt_id in BLE_GATTC_EVT* series. */
        ble_gatts_evt_t gatts_evt; /**< GATT server originated event, evt_id in BLE_GATTS_EVT* series. */
        ble_l2cap_evt_t l2cap_evt; /**< L2CAP originated event, evt_id in BLE_L2CAP_EVT* series. */
    } evt;                        /**< Event union. */
} ble_evt_t;


/**
 * @brief Version Information.
 */
typedef struct
{
    uint8_t version_number;    /**< Link Layer Version number. See https://www.bluetooth.org/en-us/specification/assigned-numbers/link-layer for assigned values. */
    uint16_t company_id;       /**< Company ID, Nordic Semiconductor's company ID is 89 (0x0059) (https://www.bluetooth.org/apps/content/Default.aspx?doc_id=49708). */
    uint16_t subversion_number; /**< Link Layer Sub Version number, corresponds to the SoftDevice Config ID or Firmware ID (FWID). */
} ble_version_t;

/**
 * @brief Configuration parameters for the PA and LNA.
 */
typedef struct
{
    uint8_t enable : 1;      /**< Enable toggling for this amplifier */
    uint8_t active_high : 1; /**< Set the pin to be active high */
    uint8_t gpio_pin : 6;    /**< The GPIO pin to toggle for this amplifier */
} ble_pa_lna_cfg_t;

/**
 * @brief PA & LNA GPIO toggle configuration
 *
 * This option configures the SoftDevice to toggle pins when the radio is active for use with a power amplifier and/or
 * a low noise amplifier.
 *
 * Toggling the pins is achieved by using two PPI channels and a GPIOTE channel. The hardware channel IDs are provided
 * by the application and should be regarded as reserved as long as any PA/LNA toggling is enabled.
 *
 * @note  @ref sd_ble_opt_get is not supported for this option.
 * @note  Setting this option while the radio is in use (i.e. any of the roles are active) may have undefined consequences
 * and must be avoided by the application.
 */
typedef struct
{
    ble_pa_lna_cfg_t pa_cfg;  /**< Power Amplifier configuration */
    ble_pa_lna_cfg_t lna_cfg; /**< Low Noise Amplifier configuration */

    uint8_t ppi_ch_id_set;    /**< PPI channel used for radio pin setting */
    uint8_t ppi_ch_id_clr;    /**< PPI channel used for radio pin clearing */
    uint8_t gpiote_ch_id;     /**< GPIOTE channel used for radio pin toggling */
} ble_common_opt_pa_lna_t;

/**
 * @brief Configuration of extended BLE connection events.
 *
 * When enabled the SoftDevice will dynamically extend the connection event when possible.
 *
 * The connection event length is controlled by the connection configuration as set by @ref ble_gap_conn_cfg_t::event_length.
 * The connection event can be extended if there is time to send another packet pair before the start of the next connection interval,
 * and if there are no conflicts with other BLE roles requesting radio time.
 *
 * @note @ref sd_ble_opt_get is not supported for this option.
 */
typedef struct
{
    uint8_t enable : 1; /**< Enable extended BLE connection events, disabled by default. */
} ble_common_opt_conn_evt_ext_t;

/**
 * @brief Enable/disable extended RC calibration.
 *
 * If extended RC calibration is enabled and the internal RC oscillator (@ref NRF_CLOCK_LF_SRC_RC) is used as the SoftDevice
 * LFCLK source, the SoftDevice as a peripheral will by default try to increase the receive window if two consecutive packets
 * are not received. If it turns out that the packets were not received due to clock drift, the RC calibration is started.
 * This calibration comes in addition to the periodic calibration that is configured by @ref sd_softdevice_enable(). When
 * using only peripheral connections, the periodic calibration can therefore be configured with a much longer interval as the
 * peripheral will be able to detect and adjust automatically to clock drift, and calibrate on demand.
 *
 * If extended RC calibration is disabled and the internal RC oscillator is used as the SoftDevice LFCLK source, the
 * RC oscillator is calibrated periodically as configured by @ref sd_softdevice_enable().
 *
 * @note @ref sd_ble_opt_get is not supported for this option.
 */
typedef struct
{
    uint8_t enable : 1; /**< Enable extended RC calibration, enabled by default. */
} ble_common_opt_extended_rc_cal_t;

/**@brief Option structure for common options. */
typedef union
{
    ble_common_opt_pa_lna_t pa_lna;                 /**< Parameters for controlling PA and LNA pin toggling. */
    ble_common_opt_conn_evt_ext_t conn_evt_ext;     /**< Parameters for enabling extended connection events. */
    ble_common_opt_extended_rc_cal_t extended_rc_cal; /**< Parameters for enabling extended RC calibration. */
} ble_common_opt_t;

/**@brief Common BLE Option type, wrapping the module specific options. */
typedef union
{
    ble_common_opt_t common_opt;        /**< COMMON options, opt_id in @ref BLE_COMMON_OPTS series. */
    ble_gap_opt_t gap_opt;              /**< GAP option, opt_id in @ref BLE_GAP_OPTS series. */
} ble_opt_t;

/**@brief BLE connection configuration type, wrapping the module specific configurations, set with
 * @ref sd_ble_cfg_set.
 *
 * @note Connection configurations don't have to be set.
 * In the case that no configurations has been set, or fewer connection configurations has been set than enabled connections,
 * the default connection configuration will be automatically added for the remaining connections.
 * When creating connections with the default configuration, @ref BLE_CONN_CFG_TAG_DEFAULT should be used in
 * place of @ref ble_conn_cfg_t::conn_cfg_tag.
 *
 * @sa sd_ble_gap_adv_start()
 * @sa sd_ble_gap_connect()
 *
 * @mscs
 * @mmsc{@ref BLE_CONN_CFG}
 * @endmscs

 */
typedef struct
{
    uint8_t conn_cfg_tag;                   /**< The application chosen tag it can use with the
                                                 @ref sd_ble_gap_adv_start() and @ref sd_ble_gap_connect() calls
                                                 to select this configuration when creating a connection.
                                                 Must be different for all connection configurations added and not @ref BLE_CONN_CFG_TAG_DEFAULT. */
    union {
        ble_gap_conn_cfg_t gap_conn_cfg;    /**< GAP connection configuration, cfg_id is @ref BLE_CONN_CFG_GAP. */
        ble_gattc_conn_cfg_t gattc_conn_cfg; /**< GATTC connection configuration, cfg_id is @ref BLE_CONN_CFG_GATTC. */
        ble_gatts_conn_cfg_t gatts_conn_cfg; /**< GATTS connection configuration, cfg_id is @ref BLE_CONN_CFG_GATTS. */
        ble_gatt_conn_cfg_t gatt_conn_cfg;  /**< GATT connection configuration, cfg_id is @ref BLE_CONN_CFG_GATT. */
        ble_l2cap_conn_cfg_t l2cap_conn_cfg; /**< L2CAP connection configuration, cfg_id is @ref BLE_CONN_CFG_L2CAP. */
    } params;                               /**< Connection configuration union. */
} ble_conn_cfg_t;

/**
 * @brief Configuration of Vendor Specific base UUIDs, set with @ref sd_ble_cfg_set.
 *
 * @retval ::NRF_ERROR_INVALID_PARAM Too many UUIDs configured.
 */
typedef struct
{
    uint8_t vs_uuid_count; /**< Number of 128-bit Vendor Specific base UUID bases to allocate memory for.
                              Default value is @ref BLE_UUID_VS_COUNT_DEFAULT. Maximum value is
                              @ref BLE_UUID_VS_COUNT_MAX. */
} ble_common_cfg_vs_uuid_t;

/**@brief Common BLE Configuration type, wrapping the common configurations. */
typedef union
{
    ble_common_cfg_vs_uuid_t vs_uuid_cfg; /**< Vendor Specific base UUID configuration, cfg_id is @ref BLE_COMMON_CFG_VS_UUID. */
} ble_common_cfg_t;

/**@brief BLE Configuration type, wrapping the module specific configurations. */
typedef union
{
    ble_conn_cfg_t conn_cfg;    /**< Connection specific configurations, cfg_id in @ref BLE_CONN_CFGS series. */
    ble_common_cfg_t common_cfg; /**< Global common configurations, cfg_id in @ref BLE_COMMON_CFGS series. */
    ble_gap_cfg_t gap_cfg;      /**< Global GAP configurations, cfg_id in @ref BLE_GAP_CFGS series. */
    ble_gatts_cfg_t gatts_cfg;  /**< Global GATTS configuration, cfg_id in @ref BLE_GATTS_CFGS series. */
} ble_cfg_t;

/** @} */

/** @addtogroup BLE_COMMON_FUNCTIONS Functions
 * @{ */

/**@brief Enable the BLE stack
 *
 * @param[in, out] p_app_ram_base   Pointer to a variable containing the start address of the
 *                                  application RAM region (APP_RAM_BASE). On return, this will
 *                                  contain the minimum start address of the application RAM region
 *                                  required by the SoftDevice for this configuration.
 *
 * @note The memory requirement for a specific configuration will not increase between SoftDevices
 *       with the same major version number.
 *
 * @note At runtime the IC's RAM is split into 2 regions: The SoftDevice RAM region is located
 *       between 0x20000000 and APP_RAM_BASE-1 and the application's RAM region is located between
 *       APP_RAM_BASE and the start of the call stack.
 *
 * @details This call initializes the BLE stack, no BLE related function other than @ref
 *          sd_ble_cfg_set can be called before this one.
 *
 * @mscs
 * @mmsc{@ref BLE_COMMON_ENABLE}
 * @endmscs
 *
 * @retval ::NRF_SUCCESS              The BLE stack has been initialized successfully.
 * @retval ::NRF_ERROR_INVALID_STATE  The BLE stack had already been initialized and cannot be reinitialized.
 * @retval ::NRF_ERROR_INVALID_ADDR   Invalid or not sufficiently aligned pointer supplied.
 * @retval ::NRF_ERROR_NO_MEM         One or more of the following is true:
 *                                    - The amount of memory assigned to the SoftDevice by *p_app_ram_base is not
 *                                      large enough to fit this configuration's memory requirement. Check *p_app_ram_base
 *                                      and set the start address of the application RAM region accordingly.
 *                                    - Dynamic part of the SoftDevice RAM region is larger then 64 kB which
 *                                      is currently not supported.
 * @retval ::NRF_ERROR_RESOURCES      The total number of L2CAP Channels configured using @ref sd_ble_cfg_set is too large.
 */
SVCALL(SD_BLE_ENABLE, uint32_t, sd_ble_enable(uint32_t * p_app_ram_base));

/**@brief Add configurations for the BLE stack
 *
 * @param[in] cfg_id              Config ID, see @ref BLE_CONN_CFGS, @ref BLE_COMMON_CFGS, @ref
 *                                BLE_GAP_CFGS or @ref BLE_GATTS_CFGS.
 * @param[in] p_cfg               Pointer to a ble_cfg_t structure containing the configuration value.
 * @param[in] app_ram_base        The start address of the application RAM region (APP_RAM_BASE).
 *                                See @ref sd_ble_enable for details about APP_RAM_BASE.
 *
 * @note The memory requirement for a specific configuration will not increase between SoftDevices
 *       with the same major version number.
 *
 * @note If a configuration is set more than once, the last one set is the one that takes effect on
 *       @ref sd_ble_enable.
 *
 * @note Any part of the BLE stack that is NOT configured with @ref sd_ble_cfg_set will have default
 *       configuration.
 *
 * @note @ref sd_ble_cfg_set may be called at any time when the SoftDevice is enabled (see @ref
 *       sd_softdevice_enable) while the BLE part of the SoftDevice is not enabled (see @ref
 *       sd_ble_enable).
 *
 * @note Error codes for the configurations are described in the configuration structs.
 *
 * @mscs
 * @mmsc{@ref BLE_COMMON_ENABLE}
 * @endmscs
 *
 * @retval ::NRF_SUCCESS              The configuration has been added successfully.
 * @retval ::NRF_ERROR_INVALID_STATE  The BLE stack had already been initialized.
 * @retval ::NRF_ERROR_INVALID_ADDR   Invalid or not sufficiently aligned pointer supplied.
 * @retval ::NRF_ERROR_INVALID_PARAM  Invalid cfg_id supplied.
 * @retval ::NRF_ERROR_NO_MEM         The amount of memory assigned to the SoftDevice by app_ram_base is not
 *                                    large enough to fit this configuration's memory requirement.
 */
SVCALL(SD_BLE_CFG_SET, uint32_t, sd_ble_cfg_set(uint32_t cfg_id, ble_cfg_t const *p_cfg, uint32_t app_ram_base));

/**@brief Get an event from the pending events queue.
 *
 * @param[out] p_dest Pointer to buffer to be filled in with an event, or NULL to retrieve the event length.
 *                    This buffer <b>must be aligned to the extend defined by @ref BLE_EVT_PTR_ALIGNMENT</b>.
 *                    The buffer should be interpreted as a @ref ble_evt_t struct.
 * @param[in, out] p_len Pointer the length of the buffer, on return it is filled with the event length.
 *
 * @details This call allows the application to pull a BLE event from the BLE stack. The application is signaled that
 * an event is available from the BLE stack by the triggering of the SD_EVT_IRQn interrupt.
 * The application is free to choose whether to call this function from thread mode (main context) or directly from the
 * Interrupt Service Routine that maps to SD_EVT_IRQn. In any case however, and because the BLE stack runs at a higher
 * priority than the application, this function should be called in a loop (until @ref NRF_ERROR_NOT_FOUND is returned)
 * every time SD_EVT_IRQn is raised to ensure that all available events are pulled from the BLE stack. Failure to do so
 * could potentially leave events in the internal queue without the application being aware of this fact.
 *
 * Sizing the p_dest buffer is equally important, since the application needs to provide all the memory necessary for the event to
 * be copied into application memory. If the buffer provided is not large enough to fit the entire contents of the event,
 * @ref NRF_ERROR_DATA_SIZE will be returned and the application can then call again with a larger buffer size.
 * The maximum possible event length is defined by @ref BLE_EVT_LEN_MAX. The application may also "peek" the event length
 * by providing p_dest as a NULL pointer and inspecting the value of *p_len upon return:
 *
 *     \code
 *     uint16_t len;
 *     errcode = sd_ble_evt_get(NULL, &len);
 *     \endcode
 *
 * @mscs
 * @mmsc{@ref BLE_COMMON_IRQ_EVT_MSC}
 * @mmsc{@ref BLE_COMMON_THREAD_EVT_MSC}
 * @endmscs
 *
 * @retval ::NRF_SUCCESS Event pulled and stored into the supplied buffer.
 * @retval ::NRF_ERROR_INVALID_ADDR Invalid or not sufficiently aligned pointer supplied.
 * @retval ::NRF_ERROR_NOT_FOUND No events ready to be pulled.
 * @retval ::NRF_ERROR_DATA_SIZE Event ready but could not fit into the supplied buffer.
 */
SVCALL(SD_BLE_EVT_GET, uint32_t, sd_ble_evt_get(uint8_t * p_dest, uint16_t * p_len));


/**@brief Add a Vendor Specific base UUID.
 *
 * @details This call enables the application to add a Vendor Specific base UUID to the BLE stack's table, for later
 * use with all other modules and APIs. This then allows the application to use the shorter, 24-bit @ref ble_uuid_t
 * format when dealing with both 16-bit and 128-bit UUIDs without having to check for lengths and having split code
 * paths. This is accomplished by extending the grouping mechanism that the Bluetooth SIG standard base UUID uses
 * for all other 128-bit UUIDs. The type field in the @ref ble_uuid_t structure is an index (relative to
 * @ref BLE_UUID_TYPE_VENDOR_BEGIN) to the table populated by multiple calls to this function, and the UUID field
 * in the same structure contains the 2 bytes at indexes 12 and 13. The number of possible 128-bit UUIDs available to
 * the application is therefore the number of Vendor Specific UUIDs added with the help of this function times 65536,
 * although restricted to modifying bytes 12 and 13 for each of the entries in the supplied array.
 *
 * @note Bytes 12 and 13 of the provided UUID will not be used internally, since those are always replaced by
 * the 16-bit uuid field in @ref ble_uuid_t.
 *
 * @note If a UUID is already present in the BLE stack's internal table, the corresponding index will be returned in
 * p_uuid_type along with an @ref NRF_SUCCESS error code.
 *
 * @param[in]  p_vs_uuid    Pointer to a 16-octet (128-bit) little endian Vendor Specific base UUID disregarding
 *                          bytes 12 and 13.
 * @param[out] p_uuid_type  Pointer to a uint8_t where the type field in @ref ble_uuid_t corresponding to this UUID will be stored.
 *
 * @retval ::NRF_SUCCESS Successfully added the Vendor Specific base UUID.
 * @retval ::NRF_ERROR_INVALID_ADDR If p_vs_uuid or p_uuid_type is NULL or invalid.
 * @retval ::NRF_ERROR_NO_MEM If there are no more free slots for VS UUIDs.
 */
SVCALL(SD_BLE_UUID_VS_ADD, uint32_t, sd_ble_uuid_vs_add(ble_uuid128_t const *p_vs_uuid, uint8_t * p_uuid_type));


/**@brief Remove a Vendor Specific base UUID.
 *
 * @details This call removes a Vendor Specific base UUID that has been added with @ref sd_ble_uuid_vs_add. This function allows
 * the application to reuse memory allocated for Vendor Specific base UUIDs.
 *
 * @note Currently this function can only be called with a p_uuid_type set to @ref BLE_UUID_TYPE_UNKNOWN or the last added UUID type.
 *
 * @param[in]  p_uuid_type  Pointer to a uint8_t where the type field in @ref ble_uuid_t::type corresponds to the UUID type that
 *                          shall be removed. If the type is set to @ref BLE_UUID_TYPE_UNKNOWN, or the pointer is NULL, the last
 *                          Vendor Specific base UUID will be removed.
 * @param[out] p_uuid_type  Pointer to a uint8_t where the type field in @ref ble_uuid_t corresponds to the UUID type that was
 *                          removed. If function returns with a failure, it contains the last type that is in use by the ATT Server.
 *
 * @retval ::NRF_SUCCESS Successfully removed the Vendor Specific base UUID.
 * @retval ::NRF_ERROR_INVALID_ADDR If p_uuid_type is invalid.
 * @retval ::NRF_ERROR_INVALID_PARAM If p_uuid_type points to a non-valid UUID type.
 * @retval ::NRF_ERROR_FORBIDDEN If the Vendor Specific base UUID is in use by the ATT Server.
 */

SVCALL(SD_BLE_UUID_VS_REMOVE, uint32_t, sd_ble_uuid_vs_remove(uint8_t * p_uuid_type));


/** @brief Decode little endian raw UUID bytes (16-bit or 128-bit) into a 24 bit @ref ble_uuid_t structure.
 *
 * @details The raw UUID bytes excluding bytes 12 and 13 (i.e. bytes 0-11 and 14-15) of p_uuid_le are compared
 * to the corresponding ones in each entry of the table of Vendor Specific base UUIDs populated with @ref sd_ble_uuid_vs_add
 * to look for a match. If there is such a match, bytes 12 and 13 are returned as p_uuid->uuid and the index
 * relative to @ref BLE_UUID_TYPE_VENDOR_BEGIN as p_uuid->type.
 *
 * @note If the UUID length supplied is 2, then the type set by this call will always be @ref BLE_UUID_TYPE_BLE.
 *
 * @param[in]   uuid_le_len Length in bytes of the buffer pointed to by p_uuid_le (must be 2 or 16 bytes).
 * @param[in]   p_uuid_le   Pointer pointing to little endian raw UUID bytes.
 * @param[out]  p_uuid      Pointer to a @ref ble_uuid_t structure to be filled in.
 *
 * @retval ::NRF_SUCCESS Successfully decoded into the @ref ble_uuid_t structure.
 * @retval ::NRF_ERROR_INVALID_ADDR Invalid pointer supplied.
 * @retval ::NRF_ERROR_INVALID_LENGTH Invalid UUID length.
 * @retval ::NRF_ERROR_NOT_FOUND For a 128-bit UUID, no match in the populated table of UUIDs.
 */
SVCALL(SD_BLE_UUID_DECODE, uint32_t, sd_ble_uuid_decode(uint8_t uuid_le_len, uint8_t const *p_uuid_le, ble_uuid_t * p_uuid));


/** @brief Encode a @ref ble_uuid_t structure into little endian raw UUID bytes (16-bit or 128-bit).
 *
 * @note The pointer to the destination buffer p_uuid_le may be NULL, in which case only the validity and size of p_uuid is computed.
 *
 * @param[in]   p_uuid        Pointer to a @ref ble_uuid_t structure that will be encoded into bytes.
 * @param[out]  p_uuid_le_len Pointer to a uint8_t that will be filled with the encoded length (2 or 16 bytes).
 * @param[out]  p_uuid_le     Pointer to a buffer where the little endian raw UUID bytes (2 or 16) will be stored.
 *
 * @retval ::NRF_SUCCESS Successfully encoded into the buffer.
 * @retval ::NRF_ERROR_INVALID_ADDR Invalid pointer supplied.
 * @retval ::NRF_ERROR_INVALID_PARAM Invalid UUID type.
 */
SVCALL(SD_BLE_UUID_ENCODE, uint32_t, sd_ble_uuid_encode(ble_uuid_t const *p_uuid, uint8_t * p_uuid_le_len, uint8_t * p_uuid_le));


/**@brief Get Version Information.
 *
 * @details This call allows the application to get the BLE stack version information.
 *
 * @param[out] p_version Pointer to a ble_version_t structure to be filled in.
 *
 * @retval ::NRF_SUCCESS  Version information stored successfully.
 * @retval ::NRF_ERROR_INVALID_ADDR Invalid pointer supplied.
 * @retval ::NRF_ERROR_BUSY The BLE stack is busy (typically doing a locally-initiated disconnection procedure).
 */
SVCALL(SD_BLE_VERSION_GET, uint32_t, sd_ble_version_get(ble_version_t * p_version));


/**@brief Provide a user memory block.
 *
 * @note This call can only be used as a response to a @ref BLE_EVT_USER_MEM_REQUEST event issued to the application.
 *
 * @param[in] conn_handle Connection handle.
 * @param[in] p_block Pointer to a user memory block structure or NULL if memory is managed by the application.
 *
 * @mscs
 * @mmsc{@ref BLE_GATTS_QUEUED_WRITE_PEER_CANCEL_MSC}
 * @mmsc{@ref BLE_GATTS_QUEUED_WRITE_NOBUF_AUTH_MSC}
 * @mmsc{@ref BLE_GATTS_QUEUED_WRITE_NOBUF_NOAUTH_MSC}
 * @mmsc{@ref BLE_GATTS_QUEUED_WRITE_BUF_AUTH_MSC}
 * @mmsc{@ref BLE_GATTS_QUEUED_WRITE_BUF_NOAUTH_MSC}
 * @mmsc{@ref BLE_GATTS_QUEUED_WRITE_QUEUE_FULL_MSC}
 * @endmscs
 *
 * @retval ::NRF_SUCCESS Successfully queued a response to the peer.
 * @retval ::NRF_ERROR_INVALID_ADDR Invalid pointer supplied.
 * @retval ::NRF_ERROR_BUSY The stack is busy, process pending events and retry.
 * @retval ::BLE_ERROR_INVALID_CONN_HANDLE Invalid Connection Handle.
 * @retval ::NRF_ERROR_INVALID_LENGTH Invalid user memory block length supplied.
 * @retval ::NRF_ERROR_INVALID_STATE Invalid Connection state or no user memory request pending.
 */
SVCALL(SD_BLE_USER_MEM_REPLY, uint32_t, sd_ble_user_mem_reply(uint16_t conn_handle, ble_user_mem_block_t const *p_block));

/**@brief Set a BLE option.
 *
 * @details This call allows the application to set the value of an option.
 *
 * @mscs
 * @mmsc{@ref BLE_GAP_PERIPH_BONDING_STATIC_PK_MSC}
 * @endmscs
 *
 * @param[in] opt_id Option ID, see @ref BLE_COMMON_OPTS and @ref BLE_GAP_OPTS.
 * @param[in] p_opt Pointer to a ble_opt_t structure containing the option value.
 *
 * @retval ::NRF_SUCCESS  Option set successfully.
 * @retval ::NRF_ERROR_INVALID_ADDR Invalid pointer supplied.
 * @retval ::BLE_ERROR_INVALID_CONN_HANDLE Invalid Connection Handle.
 * @retval ::NRF_ERROR_INVALID_PARAM Invalid parameter(s) supplied, check parameter limits and constraints.
 * @retval ::NRF_ERROR_INVALID_STATE Unable to set the parameter at this time.
 * @retval ::NRF_ERROR_BUSY The BLE stack is busy or the previous procedure has not completed.
 */
SVCALL(SD_BLE_OPT_SET, uint32_t, sd_ble_opt_set(uint32_t opt_id, ble_opt_t const *p_opt));


/**@brief Get a BLE option.
 *
 * @details This call allows the application to retrieve the value of an option.
 *
 * @param[in] opt_id Option ID, see @ref BLE_COMMON_OPTS and @ref BLE_GAP_OPTS.
 * @param[out] p_opt Pointer to a ble_opt_t structure to be filled in.
 *
 * @retval ::NRF_SUCCESS  Option retrieved successfully.
 * @retval ::NRF_ERROR_INVALID_ADDR Invalid pointer supplied.
 * @retval ::BLE_ERROR_INVALID_CONN_HANDLE Invalid Connection Handle.
 * @retval ::NRF_ERROR_INVALID_PARAM Invalid parameter(s) supplied, check parameter limits and constraints.
 * @retval ::NRF_ERROR_INVALID_STATE Unable to retrieve the parameter at this time.
 * @retval ::NRF_ERROR_BUSY The BLE stack is busy or the previous procedure has not completed.
 * @retval ::NRF_ERROR_NOT_SUPPORTED This option is not supported.
 *
 */
SVCALL(SD_BLE_OPT_GET, uint32_t, sd_ble_opt_get(uint32_t opt_id, ble_opt_t * p_opt));

/** @} */
#ifdef __cplusplus
}
#endif
#endif /* BLE_H__ */

/**
  @}
  @}
*/
