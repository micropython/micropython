/*
 * Copyright (c) 2011 - 2018, Nordic Semiconductor ASA
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
  @addtogroup BLE_GATTS Generic Attribute Profile (GATT) Server
  @{
  @brief  Definitions and prototypes for the GATTS interface.
 */

#ifndef BLE_GATTS_H__
#define BLE_GATTS_H__

#include <stdint.h>
#include "nrf_svc.h"
#include "nrf_error.h"
#include "ble_hci.h"
#include "ble_ranges.h"
#include "ble_types.h"
#include "ble_err.h"
#include "ble_gatt.h"
#include "ble_gap.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup BLE_GATTS_ENUMERATIONS Enumerations
 * @{ */

/**
 * @brief GATTS API SVC numbers.
 */
enum BLE_GATTS_SVCS
{
    SD_BLE_GATTS_SERVICE_ADD = BLE_GATTS_SVC_BASE, /**< Add a service. */
    SD_BLE_GATTS_INCLUDE_ADD,                    /**< Add an included service. */
    SD_BLE_GATTS_CHARACTERISTIC_ADD,             /**< Add a characteristic. */
    SD_BLE_GATTS_DESCRIPTOR_ADD,                 /**< Add a generic attribute. */
    SD_BLE_GATTS_VALUE_SET,                      /**< Set an attribute value. */
    SD_BLE_GATTS_VALUE_GET,                      /**< Get an attribute value. */
    SD_BLE_GATTS_HVX,                            /**< Handle Value Notification or Indication. */
    SD_BLE_GATTS_SERVICE_CHANGED,                /**< Perform a Service Changed Indication to one or more peers. */
    SD_BLE_GATTS_RW_AUTHORIZE_REPLY,             /**< Reply to an authorization request for a read or write operation on one or more attributes. */
    SD_BLE_GATTS_SYS_ATTR_SET,                   /**< Set the persistent system attributes for a connection. */
    SD_BLE_GATTS_SYS_ATTR_GET,                   /**< Retrieve the persistent system attributes. */
    SD_BLE_GATTS_INITIAL_USER_HANDLE_GET,        /**< Retrieve the first valid user handle. */
    SD_BLE_GATTS_ATTR_GET,                       /**< Retrieve the UUID and/or metadata of an attribute. */
    SD_BLE_GATTS_EXCHANGE_MTU_REPLY              /**< Reply to Exchange MTU Request. */
};

/**
 * @brief GATT Server Event IDs.
 */
enum BLE_GATTS_EVTS
{
    BLE_GATTS_EVT_WRITE = BLE_GATTS_EVT_BASE,     /**< Write operation performed.                                           \n See @ref ble_gatts_evt_write_t.                 */
    BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST,           /**< Read/Write Authorization request.                                    \n Reply with @ref sd_ble_gatts_rw_authorize_reply. \n See @ref ble_gatts_evt_rw_authorize_request_t. */
    BLE_GATTS_EVT_SYS_ATTR_MISSING,               /**< A persistent system attribute access is pending.                     \n Respond with @ref sd_ble_gatts_sys_attr_set.     \n See @ref ble_gatts_evt_sys_attr_missing_t.     */
    BLE_GATTS_EVT_HVC,                            /**< Handle Value Confirmation.                                           \n See @ref ble_gatts_evt_hvc_t.                   */
    BLE_GATTS_EVT_SC_CONFIRM,                     /**< Service Changed Confirmation.                                        \n No additional event structure applies.          */
    BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST,           /**< Exchange MTU Request.                                                \n Reply with @ref sd_ble_gatts_exchange_mtu_reply. \n See @ref ble_gatts_evt_exchange_mtu_request_t. */
    BLE_GATTS_EVT_TIMEOUT,                        /**< Peer failed to respond to an ATT request in time.                    \n See @ref ble_gatts_evt_timeout_t.               */
    BLE_GATTS_EVT_HVN_TX_COMPLETE                 /**< Handle Value Notification transmission complete.                     \n See @ref ble_gatts_evt_hvn_tx_complete_t.       */
};

/**@brief GATTS Configuration IDs.
 *
 * IDs that uniquely identify a GATTS configuration.
 */
enum BLE_GATTS_CFGS
{
    BLE_GATTS_CFG_SERVICE_CHANGED = BLE_GATTS_CFG_BASE, /**< Service changed configuration. */
    BLE_GATTS_CFG_ATTR_TAB_SIZE,                      /**< Attribute table size configuration. */
};

/** @} */

/** @addtogroup BLE_GATTS_DEFINES Defines
 * @{ */

/** @defgroup BLE_ERRORS_GATTS SVC return values specific to GATTS
 * @{ */
#define BLE_ERROR_GATTS_INVALID_ATTR_TYPE   (NRF_GATTS_ERR_BASE + 0x000) /**< Invalid attribute type. */
#define BLE_ERROR_GATTS_SYS_ATTR_MISSING    (NRF_GATTS_ERR_BASE + 0x001) /**< System Attributes missing. */
/** @} */

/** @defgroup BLE_GATTS_ATTR_LENS_MAX Maximum attribute lengths
 * @{ */
#define BLE_GATTS_FIX_ATTR_LEN_MAX (510)  /**< Maximum length for fixed length Attribute Values. */
#define BLE_GATTS_VAR_ATTR_LEN_MAX (512)  /**< Maximum length for variable length Attribute Values. */
/** @} */

/** @defgroup BLE_GATTS_SRVC_TYPES GATT Server Service Types
 * @{ */
#define BLE_GATTS_SRVC_TYPE_INVALID          0x00  /**< Invalid Service Type. */
#define BLE_GATTS_SRVC_TYPE_PRIMARY          0x01  /**< Primary Service. */
#define BLE_GATTS_SRVC_TYPE_SECONDARY        0x02  /**< Secondary Type. */
/** @} */


/** @defgroup BLE_GATTS_ATTR_TYPES GATT Server Attribute Types
 * @{ */
#define BLE_GATTS_ATTR_TYPE_INVALID         0x00  /**< Invalid Attribute Type. */
#define BLE_GATTS_ATTR_TYPE_PRIM_SRVC_DECL  0x01  /**< Primary Service Declaration. */
#define BLE_GATTS_ATTR_TYPE_SEC_SRVC_DECL   0x02  /**< Secondary Service Declaration. */
#define BLE_GATTS_ATTR_TYPE_INC_DECL        0x03  /**< Include Declaration. */
#define BLE_GATTS_ATTR_TYPE_CHAR_DECL       0x04  /**< Characteristic Declaration. */
#define BLE_GATTS_ATTR_TYPE_CHAR_VAL        0x05  /**< Characteristic Value. */
#define BLE_GATTS_ATTR_TYPE_DESC            0x06  /**< Descriptor. */
#define BLE_GATTS_ATTR_TYPE_OTHER           0x07  /**< Other, non-GATT specific type. */
/** @} */


/** @defgroup BLE_GATTS_OPS GATT Server Operations
 * @{ */
#define BLE_GATTS_OP_INVALID                0x00  /**< Invalid Operation. */
#define BLE_GATTS_OP_WRITE_REQ              0x01  /**< Write Request. */
#define BLE_GATTS_OP_WRITE_CMD              0x02  /**< Write Command. */
#define BLE_GATTS_OP_SIGN_WRITE_CMD         0x03  /**< Signed Write Command. */
#define BLE_GATTS_OP_PREP_WRITE_REQ         0x04  /**< Prepare Write Request. */
#define BLE_GATTS_OP_EXEC_WRITE_REQ_CANCEL  0x05  /**< Execute Write Request: Cancel all prepared writes. */
#define BLE_GATTS_OP_EXEC_WRITE_REQ_NOW     0x06  /**< Execute Write Request: Immediately execute all prepared writes. */
/** @} */

/** @defgroup BLE_GATTS_VLOCS GATT Value Locations
 * @{ */
#define BLE_GATTS_VLOC_INVALID       0x00  /**< Invalid Location. */
#define BLE_GATTS_VLOC_STACK         0x01  /**< Attribute Value is located in stack memory, no user memory is required. */
#define BLE_GATTS_VLOC_USER          0x02  /**< Attribute Value is located in user memory. This requires the user to maintain a valid buffer through the lifetime of the attribute, since the stack
                                                will read and write directly to the memory using the pointer provided in the APIs. There are no alignment requirements for the buffer. */
/** @} */

/** @defgroup BLE_GATTS_AUTHORIZE_TYPES GATT Server Authorization Types
 * @{ */
#define BLE_GATTS_AUTHORIZE_TYPE_INVALID    0x00  /**< Invalid Type. */
#define BLE_GATTS_AUTHORIZE_TYPE_READ       0x01  /**< Authorize a Read Operation. */
#define BLE_GATTS_AUTHORIZE_TYPE_WRITE      0x02  /**< Authorize a Write Request Operation. */
/** @} */

/** @defgroup BLE_GATTS_SYS_ATTR_FLAGS System Attribute Flags
 * @{ */
#define BLE_GATTS_SYS_ATTR_FLAG_SYS_SRVCS (1 << 0)  /**< Restrict system attributes to system services only. */
#define BLE_GATTS_SYS_ATTR_FLAG_USR_SRVCS (1 << 1)  /**< Restrict system attributes to user services only. */
/** @} */

/** @defgroup BLE_GATTS_SERVICE_CHANGED Service Changed Inclusion Values
 * @{
 */
#define BLE_GATTS_SERVICE_CHANGED_DEFAULT   (1)   /**< Default is to include the Service Changed characteristic in the Attribute Table. */
/** @} */

/** @defgroup BLE_GATTS_ATTR_TAB_SIZE Attribute Table size
 * @{
 */
#define BLE_GATTS_ATTR_TAB_SIZE_MIN         (248)  /**< Minimum Attribute Table size */
#define BLE_GATTS_ATTR_TAB_SIZE_DEFAULT     (1408) /**< Default Attribute Table size. */
/** @} */

/** @defgroup BLE_GATTS_DEFAULTS GATT Server defaults
 * @{
 */
#define BLE_GATTS_HVN_TX_QUEUE_SIZE_DEFAULT  1 /**< Default number of Handle Value Notifications that can be queued for transmission. */
/** @} */

/** @} */

/** @addtogroup BLE_GATTS_STRUCTURES Structures
 * @{ */

/**
 * @brief BLE GATTS connection configuration parameters, set with @ref sd_ble_cfg_set.
 */
typedef struct
{
    uint8_t hvn_tx_queue_size; /**< Minimum guaranteed number of Handle Value Notifications that can be queued for transmission.
                                    The default value is @ref BLE_GATTS_HVN_TX_QUEUE_SIZE_DEFAULT */
} ble_gatts_conn_cfg_t;

/**@brief Attribute metadata. */
typedef struct
{
    ble_gap_conn_sec_mode_t read_perm;     /**< Read permissions. */
    ble_gap_conn_sec_mode_t write_perm;    /**< Write permissions. */
    uint8_t vlen       : 1;                /**< Variable length attribute. */
    uint8_t vloc       : 2;                /**< Value location, see @ref BLE_GATTS_VLOCS.*/
    uint8_t rd_auth    : 1;                /**< Read authorization and value will be requested from the application on every read operation. */
    uint8_t wr_auth    : 1;                /**< Write authorization will be requested from the application on every Write Request operation (but not Write Command). */
} ble_gatts_attr_md_t;


/**@brief GATT Attribute. */
typedef struct
{
    ble_uuid_t const *p_uuid;               /**< Pointer to the attribute UUID. */
    ble_gatts_attr_md_t const *p_attr_md;   /**< Pointer to the attribute metadata structure. */
    uint16_t init_len;                      /**< Initial attribute value length in bytes. */
    uint16_t init_offs;                     /**< Initial attribute value offset in bytes. If different from zero, the first init_offs bytes of the attribute value will be left uninitialized. */
    uint16_t max_len;                       /**< Maximum attribute value length in bytes, see @ref BLE_GATTS_ATTR_LENS_MAX for maximum values. */
    uint8_t *p_value;                       /**< Pointer to the attribute data. Please note that if the @ref BLE_GATTS_VLOC_USER value location is selected in the attribute metadata, this will have to point to a buffer
                                                 that remains valid through the lifetime of the attribute. This excludes usage of automatic variables that may go out of scope or any other temporary location.
                                                 The stack may access that memory directly without the application's knowledge. For writable characteristics, this value must not be a location in flash memory.*/
} ble_gatts_attr_t;

/**@brief GATT Attribute Value. */
typedef struct
{
    uint16_t len;       /**< Length in bytes to be written or read. Length in bytes written or read after successful return.*/
    uint16_t offset;    /**< Attribute value offset. */
    uint8_t *p_value;   /**< Pointer to where value is stored or will be stored.
                             If value is stored in user memory, only the attribute length is updated when p_value == NULL.
                             Set to NULL when reading to obtain the complete length of the attribute value */
} ble_gatts_value_t;


/**@brief GATT Characteristic Presentation Format. */
typedef struct
{
    uint8_t format;             /**< Format of the value, see @ref BLE_GATT_CPF_FORMATS. */
    int8_t exponent;            /**< Exponent for integer data types. */
    uint16_t unit;              /**< Unit from Bluetooth Assigned Numbers. */
    uint8_t name_space;         /**< Namespace from Bluetooth Assigned Numbers, see @ref BLE_GATT_CPF_NAMESPACES. */
    uint16_t desc;              /**< Namespace description from Bluetooth Assigned Numbers, see @ref BLE_GATT_CPF_NAMESPACES. */
} ble_gatts_char_pf_t;


/**@brief GATT Characteristic metadata. */
typedef struct
{
    ble_gatt_char_props_t char_props;                   /**< Characteristic Properties. */
    ble_gatt_char_ext_props_t char_ext_props;           /**< Characteristic Extended Properties. */
    uint8_t const *p_char_user_desc;                    /**< Pointer to a UTF-8 encoded string (non-NULL terminated), NULL if the descriptor is not required. */
    uint16_t char_user_desc_max_size;                   /**< The maximum size in bytes of the user description descriptor. */
    uint16_t char_user_desc_size;                       /**< The size of the user description, must be smaller or equal to char_user_desc_max_size. */
    ble_gatts_char_pf_t const *p_char_pf;               /**< Pointer to a presentation format structure or NULL if the CPF descriptor is not required. */
    ble_gatts_attr_md_t const *p_user_desc_md;          /**< Attribute metadata for the User Description descriptor, or NULL for default values. */
    ble_gatts_attr_md_t const *p_cccd_md;               /**< Attribute metadata for the Client Characteristic Configuration Descriptor, or NULL for default values. */
    ble_gatts_attr_md_t const *p_sccd_md;               /**< Attribute metadata for the Server Characteristic Configuration Descriptor, or NULL for default values. */
} ble_gatts_char_md_t;


/**@brief GATT Characteristic Definition Handles. */
typedef struct
{
    uint16_t value_handle;              /**< Handle to the characteristic value. */
    uint16_t user_desc_handle;          /**< Handle to the User Description descriptor, or @ref BLE_GATT_HANDLE_INVALID if not present. */
    uint16_t cccd_handle;               /**< Handle to the Client Characteristic Configuration Descriptor, or @ref BLE_GATT_HANDLE_INVALID if not present. */
    uint16_t sccd_handle;               /**< Handle to the Server Characteristic Configuration Descriptor, or @ref BLE_GATT_HANDLE_INVALID if not present. */
} ble_gatts_char_handles_t;


/**@brief GATT HVx parameters. */
typedef struct
{
    uint16_t handle;                    /**< Characteristic Value Handle. */
    uint8_t type;                       /**< Indication or Notification, see @ref BLE_GATT_HVX_TYPES. */
    uint16_t offset;                    /**< Offset within the attribute value. */
    uint16_t *p_len;                    /**< Length in bytes to be written, length in bytes written after return. */
    uint8_t const *p_data;              /**< Actual data content, use NULL to use the current attribute value. */
} ble_gatts_hvx_params_t;

/**@brief GATT Authorization parameters. */
typedef struct
{
    uint16_t gatt_status;               /**< GATT status code for the operation, see @ref BLE_GATT_STATUS_CODES. */
    uint8_t update : 1;                 /**< If set, data supplied in p_data will be used to update the attribute value.
                                             Please note that for @ref BLE_GATTS_AUTHORIZE_TYPE_WRITE operations this bit must always be set,
                                             as the data to be written needs to be stored and later provided by the application. */
    uint16_t offset;                    /**< Offset of the attribute value being updated. */
    uint16_t len;                       /**< Length in bytes of the value in p_data pointer, see @ref BLE_GATTS_ATTR_LENS_MAX. */
    uint8_t const *p_data;              /**< Pointer to new value used to update the attribute value. */
} ble_gatts_authorize_params_t;

/**@brief GATT Read or Write Authorize Reply parameters. */
typedef struct
{
    uint8_t type;                               /**< Type of authorize operation, see @ref BLE_GATTS_AUTHORIZE_TYPES. */
    union {
        ble_gatts_authorize_params_t read;      /**< Read authorization parameters. */
        ble_gatts_authorize_params_t write;     /**< Write authorization parameters. */
    } params;                                   /**< Reply Parameters. */
} ble_gatts_rw_authorize_reply_params_t;

/**@brief Service Changed Inclusion configuration parameters, set with @ref sd_ble_cfg_set. */
typedef struct
{
    uint8_t service_changed : 1;     /**< If 1, include the Service Changed characteristic in the Attribute Table. Default is @ref BLE_GATTS_SERVICE_CHANGED_DEFAULT. */
} ble_gatts_cfg_service_changed_t;

/**@brief Attribute table size configuration parameters, set with @ref sd_ble_cfg_set.
 *
 * @retval ::NRF_ERROR_INVALID_LENGTH One or more of the following is true:
 *                                    - The specified Attribute Table size is too small.
 *                                      The minimum acceptable size is defined by @ref BLE_GATTS_ATTR_TAB_SIZE_MIN.
 *                                    - The specified Attribute Table size is not a multiple of 4.
 */
typedef struct
{
    uint32_t attr_tab_size; /**< Attribute table size. Default is @ref BLE_GATTS_ATTR_TAB_SIZE_DEFAULT, minimum is @ref BLE_GATTS_ATTR_TAB_SIZE_MIN. */
} ble_gatts_cfg_attr_tab_size_t;

/**@brief Config structure for GATTS configurations. */
typedef union
{
    ble_gatts_cfg_service_changed_t service_changed; /**< Include service changed characteristic, cfg_id is @ref BLE_GATTS_CFG_SERVICE_CHANGED. */
    ble_gatts_cfg_attr_tab_size_t attr_tab_size;    /**< Attribute table size, cfg_id is @ref BLE_GATTS_CFG_ATTR_TAB_SIZE. */
} ble_gatts_cfg_t;


/**@brief Event structure for @ref BLE_GATTS_EVT_WRITE. */
typedef struct
{
    uint16_t handle;                              /**< Attribute Handle. */
    ble_uuid_t uuid;                              /**< Attribute UUID. */
    uint8_t op;                                   /**< Type of write operation, see @ref BLE_GATTS_OPS. */
    uint8_t auth_required;                        /**< Writing operation deferred due to authorization requirement. Application may use @ref sd_ble_gatts_value_set to finalize the writing operation. */
    uint16_t offset;                              /**< Offset for the write operation. */
    uint16_t len;                                 /**< Length of the received data. */
    uint8_t data[1];                              /**< Received data. @note This is a variable length array. The size of 1 indicated is only a placeholder for compilation.
                                                       See @ref sd_ble_evt_get for more information on how to use event structures with variable length array members. */
} ble_gatts_evt_write_t;

/**@brief Event substructure for authorized read requests, see @ref ble_gatts_evt_rw_authorize_request_t. */
typedef struct
{
    uint16_t handle;                              /**< Attribute Handle. */
    ble_uuid_t uuid;                              /**< Attribute UUID. */
    uint16_t offset;                              /**< Offset for the read operation. */
} ble_gatts_evt_read_t;

/**@brief Event structure for @ref BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST. */
typedef struct
{
    uint8_t type;                               /**< Type of authorize operation, see @ref BLE_GATTS_AUTHORIZE_TYPES. */
    union {
        ble_gatts_evt_read_t read;              /**< Attribute Read Parameters. */
        ble_gatts_evt_write_t write;            /**< Attribute Write Parameters. */
    } request;                                  /**< Request Parameters. */
} ble_gatts_evt_rw_authorize_request_t;

/**@brief Event structure for @ref BLE_GATTS_EVT_SYS_ATTR_MISSING. */
typedef struct
{
    uint8_t hint;                               /**< Hint (currently unused). */
} ble_gatts_evt_sys_attr_missing_t;


/**@brief Event structure for @ref BLE_GATTS_EVT_HVC. */
typedef struct
{
    uint16_t handle;                              /**< Attribute Handle. */
} ble_gatts_evt_hvc_t;

/**@brief Event structure for @ref BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST. */
typedef struct
{
    uint16_t client_rx_mtu;                     /**< Client RX MTU size. */
} ble_gatts_evt_exchange_mtu_request_t;

/**@brief Event structure for @ref BLE_GATTS_EVT_TIMEOUT. */
typedef struct
{
    uint8_t src;                              /**< Timeout source, see @ref BLE_GATT_TIMEOUT_SOURCES. */
} ble_gatts_evt_timeout_t;

/**@brief Event structure for @ref BLE_GATTS_EVT_HVN_TX_COMPLETE. */
typedef struct
{
    uint8_t count;                            /**< Number of notification transmissions completed. */
} ble_gatts_evt_hvn_tx_complete_t;

/**@brief GATTS event structure. */
typedef struct
{
    uint16_t conn_handle;                                     /**< Connection Handle on which the event occurred. */
    union
    {
        ble_gatts_evt_write_t write;                             /**< Write Event Parameters. */
        ble_gatts_evt_rw_authorize_request_t authorize_request;  /**< Read or Write Authorize Request Parameters. */
        ble_gatts_evt_sys_attr_missing_t sys_attr_missing;       /**< System attributes missing. */
        ble_gatts_evt_hvc_t hvc;                                 /**< Handle Value Confirmation Event Parameters. */
        ble_gatts_evt_exchange_mtu_request_t exchange_mtu_request; /**< Exchange MTU Request Event Parameters. */
        ble_gatts_evt_timeout_t timeout;                         /**< Timeout Event. */
        ble_gatts_evt_hvn_tx_complete_t hvn_tx_complete;         /**< Handle Value Notification transmission complete Event Parameters. */
    } params;                                                    /**< Event Parameters. */
} ble_gatts_evt_t;

/** @} */

/** @addtogroup BLE_GATTS_FUNCTIONS Functions
 * @{ */

/**@brief Add a service declaration to the Attribute Table.
 *
 * @note Secondary Services are only relevant in the context of the entity that references them, it is therefore forbidden to
 *       add a secondary service declaration that is not referenced by another service later in the Attribute Table.
 *
 * @mscs
 * @mmsc{@ref BLE_GATTS_ATT_TABLE_POP_MSC}
 * @endmscs
 *
 * @param[in] type      Toggles between primary and secondary services, see @ref BLE_GATTS_SRVC_TYPES.
 * @param[in] p_uuid    Pointer to service UUID.
 * @param[out] p_handle Pointer to a 16-bit word where the assigned handle will be stored.
 *
 * @retval ::NRF_SUCCESS Successfully added a service declaration.
 * @retval ::NRF_ERROR_INVALID_ADDR Invalid pointer supplied.
 * @retval ::NRF_ERROR_INVALID_PARAM Invalid parameter(s) supplied, Vendor Specific UUIDs need to be present in the table.
 * @retval ::NRF_ERROR_FORBIDDEN Forbidden value supplied, certain UUIDs are reserved for the stack.
 * @retval ::NRF_ERROR_NO_MEM Not enough memory to complete operation.
 */
SVCALL(SD_BLE_GATTS_SERVICE_ADD, uint32_t, sd_ble_gatts_service_add(uint8_t type, ble_uuid_t const *p_uuid, uint16_t * p_handle));


/**@brief Add an include declaration to the Attribute Table.
 *
 * @note It is currently only possible to add an include declaration to the last added service (i.e. only sequential population is supported at this time).
 *
 * @note The included service must already be present in the Attribute Table prior to this call.
 *
 * @mscs
 * @mmsc{@ref BLE_GATTS_ATT_TABLE_POP_MSC}
 * @endmscs
 *
 * @param[in] service_handle    Handle of the service where the included service is to be placed, if @ref BLE_GATT_HANDLE_INVALID is used, it will be placed sequentially.
 * @param[in] inc_srvc_handle   Handle of the included service.
 * @param[out] p_include_handle Pointer to a 16-bit word where the assigned handle will be stored.
 *
 * @retval ::NRF_SUCCESS Successfully added an include declaration.
 * @retval ::NRF_ERROR_INVALID_ADDR Invalid pointer supplied.
 * @retval ::NRF_ERROR_INVALID_PARAM Invalid parameter(s) supplied, handle values need to match previously added services.
 * @retval ::NRF_ERROR_INVALID_STATE Invalid state to perform operation, a service context is required.
 * @retval ::NRF_ERROR_NOT_SUPPORTED Feature is not supported, service_handle must be that of the last added service.
 * @retval ::NRF_ERROR_FORBIDDEN Forbidden value supplied, self inclusions are not allowed.
 * @retval ::NRF_ERROR_NO_MEM Not enough memory to complete operation.
 * @retval ::NRF_ERROR_NOT_FOUND Attribute not found.
 */
SVCALL(SD_BLE_GATTS_INCLUDE_ADD, uint32_t, sd_ble_gatts_include_add(uint16_t service_handle, uint16_t inc_srvc_handle, uint16_t * p_include_handle));


/**@brief Add a characteristic declaration, a characteristic value declaration and optional characteristic descriptor declarations to the Attribute Table.
 *
 * @note It is currently only possible to add a characteristic to the last added service (i.e. only sequential population is supported at this time).
 *
 * @note Several restrictions apply to the parameters, such as matching permissions between the user description descriptor and the writable auxiliaries bits,
 *       readable (no security) and writable (selectable) CCCDs and SCCDs and valid presentation format values.
 *
 * @note If no metadata is provided for the optional descriptors, their permissions will be derived from the characteristic permissions.
 *
 * @mscs
 * @mmsc{@ref BLE_GATTS_ATT_TABLE_POP_MSC}
 * @endmscs
 *
 * @param[in] service_handle    Handle of the service where the characteristic is to be placed, if @ref BLE_GATT_HANDLE_INVALID is used, it will be placed sequentially.
 * @param[in] p_char_md         Characteristic metadata.
 * @param[in] p_attr_char_value Pointer to the attribute structure corresponding to the characteristic value.
 * @param[out] p_handles        Pointer to the structure where the assigned handles will be stored.
 *
 * @retval ::NRF_SUCCESS Successfully added a characteristic.
 * @retval ::NRF_ERROR_INVALID_ADDR Invalid pointer supplied.
 * @retval ::NRF_ERROR_INVALID_PARAM Invalid parameter(s) supplied, service handle, Vendor Specific UUIDs, lengths, and permissions need to adhere to the constraints.
 * @retval ::NRF_ERROR_INVALID_STATE Invalid state to perform operation, a service context is required.
 * @retval ::NRF_ERROR_FORBIDDEN Forbidden value supplied, certain UUIDs are reserved for the stack.
 * @retval ::NRF_ERROR_NO_MEM Not enough memory to complete operation.
 * @retval ::NRF_ERROR_DATA_SIZE Invalid data size(s) supplied, attribute lengths are restricted by @ref BLE_GATTS_ATTR_LENS_MAX.
 */
SVCALL(SD_BLE_GATTS_CHARACTERISTIC_ADD, uint32_t, sd_ble_gatts_characteristic_add(uint16_t service_handle, ble_gatts_char_md_t const *p_char_md, ble_gatts_attr_t const *p_attr_char_value, ble_gatts_char_handles_t * p_handles));


/**@brief Add a descriptor to the Attribute Table.
 *
 * @note It is currently only possible to add a descriptor to the last added characteristic (i.e. only sequential population is supported at this time).
 *
 * @mscs
 * @mmsc{@ref BLE_GATTS_ATT_TABLE_POP_MSC}
 * @endmscs
 *
 * @param[in] char_handle   Handle of the characteristic where the descriptor is to be placed, if @ref BLE_GATT_HANDLE_INVALID is used, it will be placed sequentially.
 * @param[in] p_attr        Pointer to the attribute structure.
 * @param[out] p_handle     Pointer to a 16-bit word where the assigned handle will be stored.
 *
 * @retval ::NRF_SUCCESS Successfully added a descriptor.
 * @retval ::NRF_ERROR_INVALID_ADDR Invalid pointer supplied.
 * @retval ::NRF_ERROR_INVALID_PARAM Invalid parameter(s) supplied, characteristic handle, Vendor Specific UUIDs, lengths, and permissions need to adhere to the constraints.
 * @retval ::NRF_ERROR_INVALID_STATE Invalid state to perform operation, a characteristic context is required.
 * @retval ::NRF_ERROR_FORBIDDEN Forbidden value supplied, certain UUIDs are reserved for the stack.
 * @retval ::NRF_ERROR_NO_MEM Not enough memory to complete operation.
 * @retval ::NRF_ERROR_DATA_SIZE Invalid data size(s) supplied, attribute lengths are restricted by @ref BLE_GATTS_ATTR_LENS_MAX.
 */
SVCALL(SD_BLE_GATTS_DESCRIPTOR_ADD, uint32_t, sd_ble_gatts_descriptor_add(uint16_t char_handle, ble_gatts_attr_t const *p_attr, uint16_t * p_handle));

/**@brief Set the value of a given attribute.
 *
 * @note Values other than system attributes can be set at any time, regardless of whether any active connections exist.
 *
 * @mscs
 * @mmsc{@ref BLE_GATTS_QUEUED_WRITE_QUEUE_FULL_MSC}
 * @mmsc{@ref BLE_GATTS_QUEUED_WRITE_NOBUF_NOAUTH_MSC}
 * @endmscs
 *
 * @param[in] conn_handle  Connection handle. Ignored if the value does not belong to a system attribute.
 * @param[in] handle       Attribute handle.
 * @param[in,out] p_value  Attribute value information.
 *
 * @retval ::NRF_SUCCESS Successfully set the value of the attribute.
 * @retval ::NRF_ERROR_INVALID_ADDR Invalid pointer supplied.
 * @retval ::NRF_ERROR_INVALID_PARAM Invalid parameter(s) supplied.
 * @retval ::NRF_ERROR_NOT_FOUND Attribute not found.
 * @retval ::NRF_ERROR_FORBIDDEN Forbidden handle supplied, certain attributes are not modifiable by the application.
 * @retval ::NRF_ERROR_DATA_SIZE Invalid data size(s) supplied, attribute lengths are restricted by @ref BLE_GATTS_ATTR_LENS_MAX.
 * @retval ::BLE_ERROR_INVALID_CONN_HANDLE Invalid connection handle supplied on a system attribute.
 */
SVCALL(SD_BLE_GATTS_VALUE_SET, uint32_t, sd_ble_gatts_value_set(uint16_t conn_handle, uint16_t handle, ble_gatts_value_t * p_value));

/**@brief Get the value of a given attribute.
 *
 * @note                 If the attribute value is longer than the size of the supplied buffer,
 *                       @ref ble_gatts_value_t::len will return the total attribute value length (excluding offset),
 *                       and not the number of bytes actually returned in @ref ble_gatts_value_t::p_value.
 *                       The application may use this information to allocate a suitable buffer size.
 *
 * @note                 When retrieving system attribute values with this function, the connection handle
 *                       may refer to an already disconnected connection. Refer to the documentation of
 *                       @ref sd_ble_gatts_sys_attr_get for further information.
 *
 * @param[in] conn_handle  Connection handle. Ignored if the value does not belong to a system attribute.
 * @param[in] handle       Attribute handle.
 * @param[in,out] p_value  Attribute value information.
 *
 * @retval ::NRF_SUCCESS Successfully retrieved the value of the attribute.
 * @retval ::NRF_ERROR_INVALID_ADDR Invalid pointer supplied.
 * @retval ::NRF_ERROR_NOT_FOUND Attribute not found.
 * @retval ::NRF_ERROR_INVALID_PARAM Invalid attribute offset supplied.
 * @retval ::BLE_ERROR_INVALID_CONN_HANDLE Invalid connection handle supplied on a system attribute.
 * @retval ::BLE_ERROR_GATTS_SYS_ATTR_MISSING System attributes missing, use @ref sd_ble_gatts_sys_attr_set to set them to a known value.
 */
SVCALL(SD_BLE_GATTS_VALUE_GET, uint32_t, sd_ble_gatts_value_get(uint16_t conn_handle, uint16_t handle, ble_gatts_value_t * p_value));

/**@brief Notify or Indicate an attribute value.
 *
 * @details This function checks for the relevant Client Characteristic Configuration descriptor value to verify that the relevant operation
 *          (notification or indication) has been enabled by the client. It is also able to update the attribute value before issuing the PDU, so that
 *          the application can atomically perform a value update and a server initiated transaction with a single API call.
 *
 * @note    The local attribute value may be updated even if an outgoing packet is not sent to the peer due to an error during execution.
 *          The Attribute Table has been updated if one of the following error codes is returned: @ref NRF_ERROR_INVALID_STATE, @ref NRF_ERROR_BUSY,
 *          @ref NRF_ERROR_FORBIDDEN, @ref BLE_ERROR_GATTS_SYS_ATTR_MISSING and @ref NRF_ERROR_RESOURCES.
 *          The caller can check whether the value has been updated by looking at the contents of *(@ref ble_gatts_hvx_params_t::p_len).
 *
 * @note    Only one indication procedure can be ongoing per connection at a time.
 *          If the application tries to indicate an attribute value while another indication procedure is ongoing,
 *          the function call will return @ref NRF_ERROR_BUSY.
 *          A @ref BLE_GATTS_EVT_HVC event will be issued as soon as the confirmation arrives from the peer.
 *
 * @note    The number of Handle Value Notifications that can be queued is configured by @ref ble_gatts_conn_cfg_t::hvn_tx_queue_size
 *          When the queue is full, the function call will return @ref NRF_ERROR_RESOURCES.
 *          A @ref BLE_GATTS_EVT_HVN_TX_COMPLETE event will be issued as soon as the transmission of the notification is complete.
 *
 * @note    The application can keep track of the available queue element count for notifications by following the procedure below:
 *          - Store initial queue element count in a variable.
 *          - Decrement the variable, which stores the currently available queue element count, by one when a call to this function returns @ref NRF_SUCCESS.
 *          - Increment the variable, which stores the current available queue element count, by the count variable in @ref BLE_GATTS_EVT_HVN_TX_COMPLETE event.
 *
 * @events
 * @event{@ref BLE_GATTS_EVT_HVN_TX_COMPLETE, Notification transmission complete.}
 * @event{@ref BLE_GATTS_EVT_HVC, Confirmation received from the peer.}
 * @endevents
 *
 * @mscs
 * @mmsc{@ref BLE_GATTS_HVX_SYS_ATTRS_MISSING_MSC}
 * @mmsc{@ref BLE_GATTS_HVN_MSC}
 * @mmsc{@ref BLE_GATTS_HVI_MSC}
 * @mmsc{@ref BLE_GATTS_HVX_DISABLED_MSC}
 * @endmscs
 *
 * @param[in] conn_handle      Connection handle.
 * @param[in,out] p_hvx_params Pointer to an HVx parameters structure. If @ref ble_gatts_hvx_params_t::p_data
 *                             contains a non-NULL pointer the attribute value will be updated with the contents
 *                             pointed by it before sending the notification or indication. If the attribute value
 *                             is updated, @ref ble_gatts_hvx_params_t::p_len is updated by the SoftDevice to
 *                             contain the number of actual bytes written, else it will be set to 0.
 *
 * @retval ::NRF_SUCCESS Successfully queued a notification or indication for transmission, and optionally updated the attribute value.
 * @retval ::BLE_ERROR_INVALID_CONN_HANDLE Invalid Connection Handle.
 * @retval ::NRF_ERROR_INVALID_STATE One or more of the following is true:
 *                                   - Invalid Connection State
 *                                   - Notifications and/or indications not enabled in the CCCD
 *                                   - An ATT_MTU exchange is ongoing
 * @retval ::NRF_ERROR_INVALID_ADDR Invalid pointer supplied.
 * @retval ::NRF_ERROR_INVALID_PARAM Invalid parameter(s) supplied.
 * @retval ::BLE_ERROR_INVALID_ATTR_HANDLE Invalid attribute handle(s) supplied. Only attributes added directly by the application are available to notify and indicate.
 * @retval ::BLE_ERROR_GATTS_INVALID_ATTR_TYPE Invalid attribute type(s) supplied, only characteristic values may be notified and indicated.
 * @retval ::NRF_ERROR_NOT_FOUND Attribute not found.
 * @retval ::NRF_ERROR_FORBIDDEN The connection's current security level is lower than the one required by the write permissions of the CCCD associated with this characteristic.
 * @retval ::NRF_ERROR_DATA_SIZE Invalid data size(s) supplied.
 * @retval ::NRF_ERROR_BUSY For @ref BLE_GATT_HVX_INDICATION Procedure already in progress. Wait for a @ref BLE_GATTS_EVT_HVC event and retry.
 * @retval ::BLE_ERROR_GATTS_SYS_ATTR_MISSING System attributes missing, use @ref sd_ble_gatts_sys_attr_set to set them to a known value.
 * @retval ::NRF_ERROR_RESOURCES Too many notifications queued.
 *                               Wait for a @ref BLE_GATTS_EVT_HVN_TX_COMPLETE event and retry.
 * @retval ::NRF_ERROR_TIMEOUT There has been a GATT procedure timeout. No new GATT procedure can be performed without reestablishing the connection.
 */
SVCALL(SD_BLE_GATTS_HVX, uint32_t, sd_ble_gatts_hvx(uint16_t conn_handle, ble_gatts_hvx_params_t const *p_hvx_params));

/**@brief Indicate the Service Changed attribute value.
 *
 * @details This call will send a Handle Value Indication to one or more peers connected to inform them that the Attribute
 *          Table layout has changed. As soon as the peer has confirmed the indication, a @ref BLE_GATTS_EVT_SC_CONFIRM event will
 *          be issued.
 *
 * @note    Some of the restrictions and limitations that apply to @ref sd_ble_gatts_hvx also apply here.
 *
 * @events
 * @event{@ref BLE_GATTS_EVT_SC_CONFIRM, Confirmation of attribute table change received from peer.}
 * @endevents
 *
 * @mscs
 * @mmsc{@ref BLE_GATTS_SC_MSC}
 * @endmscs
 *
 * @param[in] conn_handle  Connection handle.
 * @param[in] start_handle Start of affected attribute handle range.
 * @param[in] end_handle   End of affected attribute handle range.
 *
 * @retval ::NRF_SUCCESS Successfully queued the Service Changed indication for transmission.
 * @retval ::BLE_ERROR_INVALID_CONN_HANDLE Invalid Connection Handle.
 * @retval ::NRF_ERROR_NOT_SUPPORTED Service Changed not enabled at initialization. See @ref
 *                                   sd_ble_cfg_set and @ref ble_gatts_cfg_service_changed_t.
 * @retval ::NRF_ERROR_INVALID_STATE One or more of the following is true:
 *                                   - Invalid Connection State
 *                                   - Notifications and/or indications not enabled in the CCCD
 *                                   - An ATT_MTU exchange is ongoing
 * @retval ::NRF_ERROR_INVALID_PARAM Invalid parameter(s) supplied.
 * @retval ::BLE_ERROR_INVALID_ATTR_HANDLE Invalid attribute handle(s) supplied, handles must be in the range populated by the application.
 * @retval ::NRF_ERROR_BUSY Procedure already in progress.
 * @retval ::BLE_ERROR_GATTS_SYS_ATTR_MISSING System attributes missing, use @ref sd_ble_gatts_sys_attr_set to set them to a known value.
 * @retval ::NRF_ERROR_TIMEOUT There has been a GATT procedure timeout. No new GATT procedure can be performed without reestablishing the connection.
 */
SVCALL(SD_BLE_GATTS_SERVICE_CHANGED, uint32_t, sd_ble_gatts_service_changed(uint16_t conn_handle, uint16_t start_handle, uint16_t end_handle));

/**@brief Respond to a Read/Write authorization request.
 *
 * @note This call should only be used as a response to a @ref BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST event issued to the application.
 *
 * @mscs
 * @mmsc{@ref BLE_GATTS_QUEUED_WRITE_NOBUF_AUTH_MSC}
 * @mmsc{@ref BLE_GATTS_QUEUED_WRITE_BUF_AUTH_MSC}
 * @mmsc{@ref BLE_GATTS_QUEUED_WRITE_NOBUF_NOAUTH_MSC}
 * @mmsc{@ref BLE_GATTS_READ_REQ_AUTH_MSC}
 * @mmsc{@ref BLE_GATTS_WRITE_REQ_AUTH_MSC}
 * @mmsc{@ref BLE_GATTS_QUEUED_WRITE_QUEUE_FULL_MSC}
 * @mmsc{@ref BLE_GATTS_QUEUED_WRITE_PEER_CANCEL_MSC}
 * @endmscs
 *
 * @param[in] conn_handle                 Connection handle.
 * @param[in] p_rw_authorize_reply_params Pointer to a structure with the attribute provided by the application.
 *
 * @note @ref ble_gatts_authorize_params_t::p_data is ignored when this function is used to respond
 *       to a @ref BLE_GATTS_AUTHORIZE_TYPE_READ event if @ref ble_gatts_authorize_params_t::update
 *       is set to 0.
 *
 * @retval ::NRF_SUCCESS               Successfully queued a response to the peer, and in the case of a write operation, Attribute Table updated.
 * @retval ::BLE_ERROR_INVALID_CONN_HANDLE Invalid Connection Handle.
 * @retval ::NRF_ERROR_BUSY            The stack is busy, process pending events and retry.
 * @retval ::NRF_ERROR_INVALID_ADDR    Invalid pointer supplied.
 * @retval ::NRF_ERROR_INVALID_STATE   Invalid Connection State or no authorization request pending.
 * @retval ::NRF_ERROR_INVALID_PARAM   Authorization op invalid,
 *                                         handle supplied does not match requested handle,
 *                                         or invalid data to be written provided by the application.
 * @retval ::NRF_ERROR_TIMEOUT There has been a GATT procedure timeout. No new GATT procedure can be performed without reestablishing the connection.
 */
SVCALL(SD_BLE_GATTS_RW_AUTHORIZE_REPLY, uint32_t, sd_ble_gatts_rw_authorize_reply(uint16_t conn_handle, ble_gatts_rw_authorize_reply_params_t const *p_rw_authorize_reply_params));


/**@brief Update persistent system attribute information.
 *
 * @details Supply information about persistent system attributes to the stack,
 *          previously obtained using @ref sd_ble_gatts_sys_attr_get.
 *          This call is only allowed for active connections, and is usually
 *          made immediately after a connection is established with an known bonded device,
 *          often as a response to a @ref BLE_GATTS_EVT_SYS_ATTR_MISSING.
 *
 *          p_sysattrs may point directly to the application's stored copy of the system attributes
 *          obtained using @ref sd_ble_gatts_sys_attr_get.
 *          If the pointer is NULL, the system attribute info is initialized, assuming that
 *          the application does not have any previously saved system attribute data for this device.
 *
 * @note The state of persistent system attributes is reset upon connection establishment and then remembered for its duration.
 *
 * @note If this call returns with an error code different from @ref NRF_SUCCESS, the storage of persistent system attributes may have been completed only partially.
 *       This means that the state of the attribute table is undefined, and the application should either provide a new set of attributes using this same call or
 *       reset the SoftDevice to return to a known state.
 *
 * @note When the @ref BLE_GATTS_SYS_ATTR_FLAG_SYS_SRVCS is used with this function, only the system attributes included in system services will be modified.
 * @note When the @ref BLE_GATTS_SYS_ATTR_FLAG_USR_SRVCS is used with this function, only the system attributes included in user services will be modified.
 *
 * @mscs
 * @mmsc{@ref BLE_GATTS_HVX_SYS_ATTRS_MISSING_MSC}
 * @mmsc{@ref BLE_GATTS_SYS_ATTRS_UNK_PEER_MSC}
 * @mmsc{@ref BLE_GATTS_SYS_ATTRS_BONDED_PEER_MSC}
 * @endmscs
 *
 * @param[in]  conn_handle        Connection handle.
 * @param[in]  p_sys_attr_data    Pointer to a saved copy of system attributes supplied to the stack, or NULL.
 * @param[in]  len                Size of data pointed by p_sys_attr_data, in octets.
 * @param[in]  flags              Optional additional flags, see @ref BLE_GATTS_SYS_ATTR_FLAGS
 *
 * @retval ::NRF_SUCCESS Successfully set the system attribute information.
 * @retval ::BLE_ERROR_INVALID_CONN_HANDLE Invalid Connection Handle.
 * @retval ::NRF_ERROR_INVALID_ADDR Invalid pointer supplied.
 * @retval ::NRF_ERROR_INVALID_STATE Invalid Connection State.
 * @retval ::NRF_ERROR_INVALID_PARAM Invalid flags supplied.
 * @retval ::NRF_ERROR_INVALID_DATA Invalid data supplied, the data should be exactly the same as retrieved with @ref sd_ble_gatts_sys_attr_get.
 * @retval ::NRF_ERROR_NO_MEM Not enough memory to complete operation.
 */
SVCALL(SD_BLE_GATTS_SYS_ATTR_SET, uint32_t, sd_ble_gatts_sys_attr_set(uint16_t conn_handle, uint8_t const *p_sys_attr_data, uint16_t len, uint32_t flags));


/**@brief Retrieve persistent system attribute information from the stack.
 *
 * @details This call is used to retrieve information about values to be stored persistently by the application
 *          during the lifetime of a connection or after it has been terminated. When a new connection is established with the same bonded device,
 *          the system attribute information retrieved with this function should be restored using using @ref sd_ble_gatts_sys_attr_set.
 *          If retrieved after disconnection, the data should be read before a new connection established. The connection handle for
 *          the previous, now disconnected, connection will remain valid until a new one is created to allow this API call to refer to it.
 *          Connection handles belonging to active connections can be used as well, but care should be taken since the system attributes
 *          may be written to at any time by the peer during a connection's lifetime.
 *
 * @note When the @ref BLE_GATTS_SYS_ATTR_FLAG_SYS_SRVCS is used with this function, only the system attributes included in system services will be returned.
 * @note When the @ref BLE_GATTS_SYS_ATTR_FLAG_USR_SRVCS is used with this function, only the system attributes included in user services will be returned.
 *
 * @mscs
 * @mmsc{@ref BLE_GATTS_SYS_ATTRS_BONDED_PEER_MSC}
 * @endmscs
 *
 * @param[in]     conn_handle       Connection handle of the recently terminated connection.
 * @param[out]    p_sys_attr_data   Pointer to a buffer where updated information about system attributes will be filled in. The format of the data is described
 *                                  in @ref BLE_GATTS_SYS_ATTRS_FORMAT. NULL can be provided to obtain the length of the data.
 * @param[in,out] p_len             Size of application buffer if p_sys_attr_data is not NULL. Unconditionally updated to actual length of system attribute data.
 * @param[in]     flags             Optional additional flags, see @ref BLE_GATTS_SYS_ATTR_FLAGS
 *
 * @retval ::NRF_SUCCESS Successfully retrieved the system attribute information.
 * @retval ::BLE_ERROR_INVALID_CONN_HANDLE Invalid Connection Handle.
 * @retval ::NRF_ERROR_INVALID_ADDR Invalid pointer supplied.
 * @retval ::NRF_ERROR_INVALID_PARAM Invalid flags supplied.
 * @retval ::NRF_ERROR_DATA_SIZE The system attribute information did not fit into the provided buffer.
 * @retval ::NRF_ERROR_NOT_FOUND No system attributes found.
 */
SVCALL(SD_BLE_GATTS_SYS_ATTR_GET, uint32_t, sd_ble_gatts_sys_attr_get(uint16_t conn_handle, uint8_t * p_sys_attr_data, uint16_t * p_len, uint32_t flags));


/**@brief Retrieve the first valid user attribute handle.
 *
 * @param[out] p_handle   Pointer to an integer where the handle will be stored.
 *
 * @retval ::NRF_SUCCESS Successfully retrieved the handle.
 * @retval ::NRF_ERROR_INVALID_ADDR Invalid pointer supplied.
 */
SVCALL(SD_BLE_GATTS_INITIAL_USER_HANDLE_GET, uint32_t, sd_ble_gatts_initial_user_handle_get(uint16_t * p_handle));

/**@brief Retrieve the attribute UUID and/or metadata.
 *
 * @param[in]  handle Attribute handle
 * @param[out] p_uuid UUID of the attribute. Use NULL to omit this field.
 * @param[out] p_md Metadata of the attribute. Use NULL to omit this field.
 *
 * @retval ::NRF_SUCCESS Successfully retrieved the attribute metadata,
 * @retval ::NRF_ERROR_INVALID_ADDR Invalid pointer supplied.
 * @retval ::NRF_ERROR_INVALID_PARAM Invalid parameters supplied. Returned when both @c p_uuid and @c p_md are NULL.
 * @retval ::NRF_ERROR_NOT_FOUND Attribute was not found.
 */
SVCALL(SD_BLE_GATTS_ATTR_GET, uint32_t, sd_ble_gatts_attr_get(uint16_t handle, ble_uuid_t * p_uuid, ble_gatts_attr_md_t * p_md));

/**@brief Reply to an ATT_MTU exchange request by sending an Exchange MTU Response to the client.
 *
 * @details This function is only used to reply to a @ref BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST event.
 *
 * @details The SoftDevice sets ATT_MTU to the minimum of:
 *          - The Client RX MTU value from @ref BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST, and
 *          - The Server RX MTU value.
 *
 *          However, the SoftDevice never sets ATT_MTU lower than @ref BLE_GATT_ATT_MTU_DEFAULT.
 *
 * @mscs
 * @mmsc{@ref BLE_GATTS_MTU_EXCHANGE}
 * @endmscs
 *
 * @param[in] conn_handle    The connection handle identifying the connection to perform this procedure on.
 * @param[in] server_rx_mtu  Server RX MTU size.
 *                           - The minimum value is @ref BLE_GATT_ATT_MTU_DEFAULT.
 *                           - The maximum value is @ref ble_gatt_conn_cfg_t::att_mtu in the connection configuration
 *                             used for this connection.
 *                           - The value must be equal to Client RX MTU size given in @ref sd_ble_gattc_exchange_mtu_request
 *                             if an ATT_MTU exchange has already been performed in the other direction.
 *
 * @retval ::NRF_SUCCESS Successfully sent response to the client.
 * @retval ::BLE_ERROR_INVALID_CONN_HANDLE Invalid Connection Handle.
 * @retval ::NRF_ERROR_INVALID_STATE Invalid Connection State or no ATT_MTU exchange request pending.
 * @retval ::NRF_ERROR_INVALID_PARAM Invalid Server RX MTU size supplied.
 * @retval ::NRF_ERROR_TIMEOUT There has been a GATT procedure timeout. No new GATT procedure can be performed without reestablishing the connection.
 */
SVCALL(SD_BLE_GATTS_EXCHANGE_MTU_REPLY, uint32_t, sd_ble_gatts_exchange_mtu_reply(uint16_t conn_handle, uint16_t server_rx_mtu));
/** @} */

#ifdef __cplusplus
}
#endif
#endif // BLE_GATTS_H__

/**
  @}
*/
