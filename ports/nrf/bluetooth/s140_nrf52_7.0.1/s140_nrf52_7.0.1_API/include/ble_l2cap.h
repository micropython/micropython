/*
 * Copyright (c) 2011 - 2020, Nordic Semiconductor ASA
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
  @addtogroup BLE_L2CAP Logical Link Control and Adaptation Protocol (L2CAP)
  @{
  @brief Definitions and prototypes for the L2CAP interface.
 */

#ifndef BLE_L2CAP_H__
#define BLE_L2CAP_H__

#include <stdint.h>
#include "nrf_svc.h"
#include "nrf_error.h"
#include "ble_ranges.h"
#include "ble_types.h"
#include "ble_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**@addtogroup BLE_L2CAP_TERMINOLOGY Terminology
 * @{
 * @details
 *
 * L2CAP SDU
 * - A data unit that the application can send/receive to/from a peer.
 *
 * L2CAP PDU
 * - A data unit that is exchanged between local and remote L2CAP entities.
 *   It consists of L2CAP protocol control information and payload fields.
 *   The payload field can contain an L2CAP SDU or a part of an L2CAP SDU.
 *
 * L2CAP MTU
 * - The maximum length of an L2CAP SDU.
 *
 * L2CAP MPS
 * - The maximum length of an L2CAP PDU payload field.
 *
 * Credits
 * - A value indicating the number of L2CAP PDUs that the receiver of the credit can send to the peer.
 * @} */

/**@addtogroup BLE_L2CAP_ENUMERATIONS Enumerations
 * @{ */

/**@brief L2CAP API SVC numbers. */
enum BLE_L2CAP_SVCS
{
    SD_BLE_L2CAP_CH_SETUP        = BLE_L2CAP_SVC_BASE + 0,/**< Set up an L2CAP channel. */
    SD_BLE_L2CAP_CH_RELEASE      = BLE_L2CAP_SVC_BASE + 1,/**< Release an L2CAP channel. */
    SD_BLE_L2CAP_CH_RX           = BLE_L2CAP_SVC_BASE + 2,/**< Receive an SDU on an L2CAP channel. */
    SD_BLE_L2CAP_CH_TX           = BLE_L2CAP_SVC_BASE + 3,/**< Transmit an SDU on an L2CAP channel. */
    SD_BLE_L2CAP_CH_FLOW_CONTROL = BLE_L2CAP_SVC_BASE + 4, /**< Advanced SDU reception flow control. */
};

/**@brief L2CAP Event IDs. */
enum BLE_L2CAP_EVTS
{
    BLE_L2CAP_EVT_CH_SETUP_REQUEST    = BLE_L2CAP_EVT_BASE + 0,  /**< L2CAP Channel Setup Request event.
                                                                   \n Reply with @ref sd_ble_l2cap_ch_setup.
                                                                   \n See @ref ble_l2cap_evt_ch_setup_request_t. */
    BLE_L2CAP_EVT_CH_SETUP_REFUSED    = BLE_L2CAP_EVT_BASE + 1,  /**< L2CAP Channel Setup Refused event.
                                                                   \n See @ref ble_l2cap_evt_ch_setup_refused_t. */
    BLE_L2CAP_EVT_CH_SETUP            = BLE_L2CAP_EVT_BASE + 2,  /**< L2CAP Channel Setup Completed event.
                                                                   \n See @ref ble_l2cap_evt_ch_setup_t. */
    BLE_L2CAP_EVT_CH_RELEASED         = BLE_L2CAP_EVT_BASE + 3,  /**< L2CAP Channel Released event.
                                                                   \n No additional event structure applies. */
    BLE_L2CAP_EVT_CH_SDU_BUF_RELEASED = BLE_L2CAP_EVT_BASE + 4,  /**< L2CAP Channel SDU data buffer released event.
                                                                   \n See @ref ble_l2cap_evt_ch_sdu_buf_released_t. */
    BLE_L2CAP_EVT_CH_CREDIT           = BLE_L2CAP_EVT_BASE + 5,  /**< L2CAP Channel Credit received.
                                                                   \n See @ref ble_l2cap_evt_ch_credit_t. */
    BLE_L2CAP_EVT_CH_RX               = BLE_L2CAP_EVT_BASE + 6,  /**< L2CAP Channel SDU received.
                                                                   \n See @ref ble_l2cap_evt_ch_rx_t. */
    BLE_L2CAP_EVT_CH_TX               = BLE_L2CAP_EVT_BASE + 7, /**< L2CAP Channel SDU transmitted.
                                                                   \n See @ref ble_l2cap_evt_ch_tx_t. */
};

/** @} */

/**@addtogroup BLE_L2CAP_DEFINES Defines
 * @{ */

/**@brief Maximum number of L2CAP channels per connection. */
#define BLE_L2CAP_CH_COUNT_MAX    (64)

/**@brief Minimum L2CAP MTU, in bytes. */
#define BLE_L2CAP_MTU_MIN         (23)

/**@brief Minimum L2CAP MPS, in bytes. */
#define BLE_L2CAP_MPS_MIN         (23)

/**@brief Invalid CID. */
#define BLE_L2CAP_CID_INVALID     (0x0000)

/**@brief Default number of credits for @ref sd_ble_l2cap_ch_flow_control. */
#define BLE_L2CAP_CREDITS_DEFAULT (1)

/**@defgroup BLE_L2CAP_CH_SETUP_REFUSED_SRCS L2CAP channel setup refused sources
 * @{ */
#define BLE_L2CAP_CH_SETUP_REFUSED_SRC_LOCAL            (0x01)    /**< Local. */
#define BLE_L2CAP_CH_SETUP_REFUSED_SRC_REMOTE           (0x02)    /**< Remote. */
/** @}  */

/** @defgroup BLE_L2CAP_CH_STATUS_CODES L2CAP channel status codes
 * @{ */
#define BLE_L2CAP_CH_STATUS_CODE_SUCCESS                (0x0000)  /**< Success. */
#define BLE_L2CAP_CH_STATUS_CODE_LE_PSM_NOT_SUPPORTED   (0x0002)  /**< LE_PSM not supported. */
#define BLE_L2CAP_CH_STATUS_CODE_NO_RESOURCES           (0x0004)  /**< No resources available. */
#define BLE_L2CAP_CH_STATUS_CODE_INSUFF_AUTHENTICATION  (0x0005)  /**< Insufficient authentication. */
#define BLE_L2CAP_CH_STATUS_CODE_INSUFF_AUTHORIZATION   (0x0006)  /**< Insufficient authorization. */
#define BLE_L2CAP_CH_STATUS_CODE_INSUFF_ENC_KEY_SIZE    (0x0007)  /**< Insufficient encryption key size. */
#define BLE_L2CAP_CH_STATUS_CODE_INSUFF_ENC             (0x0008)  /**< Insufficient encryption. */
#define BLE_L2CAP_CH_STATUS_CODE_INVALID_SCID           (0x0009)  /**< Invalid Source CID. */
#define BLE_L2CAP_CH_STATUS_CODE_SCID_ALLOCATED         (0x000A)  /**< Source CID already allocated. */
#define BLE_L2CAP_CH_STATUS_CODE_UNACCEPTABLE_PARAMS    (0x000B)  /**< Unacceptable parameters. */
#define BLE_L2CAP_CH_STATUS_CODE_NOT_UNDERSTOOD         (0x8000)  /**< Command Reject received instead of LE Credit Based Connection Response. */
#define BLE_L2CAP_CH_STATUS_CODE_TIMEOUT                (0xC000)  /**< Operation timed out. */
/** @} */

/** @} */

/**@addtogroup BLE_L2CAP_STRUCTURES Structures
 * @{ */

/**
 * @brief BLE L2CAP connection configuration parameters, set with @ref sd_ble_cfg_set.
 *
 * @note  These parameters are set per connection, so all L2CAP channels created on this connection
 *        will have the same parameters.
 *
 * @retval ::NRF_ERROR_INVALID_PARAM  One or more of the following is true:
 *                                    - rx_mps is smaller than @ref BLE_L2CAP_MPS_MIN.
 *                                    - tx_mps is smaller than @ref BLE_L2CAP_MPS_MIN.
 *                                    - ch_count is greater than @ref BLE_L2CAP_CH_COUNT_MAX.
 * @retval ::NRF_ERROR_NO_MEM         rx_mps or tx_mps is set too high.
 */
typedef struct
{
    uint16_t rx_mps;         /**< The maximum L2CAP PDU payload size, in bytes, that L2CAP shall
                                  be able to receive on L2CAP channels on connections with this
                                  configuration. The minimum value is @ref BLE_L2CAP_MPS_MIN. */
    uint16_t tx_mps;         /**< The maximum L2CAP PDU payload size, in bytes, that L2CAP shall
                                  be able to transmit on L2CAP channels on connections with this
                                  configuration. The minimum value is @ref BLE_L2CAP_MPS_MIN. */
    uint8_t rx_queue_size;   /**< Number of SDU data buffers that can be queued for reception per
                                  L2CAP channel. The minimum value is one. */
    uint8_t tx_queue_size;   /**< Number of SDU data buffers that can be queued for transmission
                                  per L2CAP channel. The minimum value is one. */
    uint8_t ch_count;        /**< Number of L2CAP channels the application can create per connection
                                  with this configuration. The default value is zero, the maximum
                                  value is @ref BLE_L2CAP_CH_COUNT_MAX.
                                  @note if this parameter is set to zero, all other parameters in
                                  @ref ble_l2cap_conn_cfg_t are ignored. */
} ble_l2cap_conn_cfg_t;

/**@brief L2CAP channel RX parameters. */
typedef struct
{
    uint16_t rx_mtu;         /**< The maximum L2CAP SDU size, in bytes, that L2CAP shall be able to
                                  receive on this L2CAP channel.
                                  - Must be equal to or greater than @ref BLE_L2CAP_MTU_MIN. */
    uint16_t rx_mps;         /**< The maximum L2CAP PDU payload size, in bytes, that L2CAP shall be
                                  able to receive on this L2CAP channel.
                                  - Must be equal to or greater than @ref BLE_L2CAP_MPS_MIN.
                                  - Must be equal to or less than @ref ble_l2cap_conn_cfg_t::rx_mps. */
    ble_data_t sdu_buf;      /**< SDU data buffer for reception.
                                  - If @ref ble_data_t::p_data is non-NULL, initial credits are
                                    issued to the peer.
                                  - If @ref ble_data_t::p_data is NULL, no initial credits are
                                    issued to the peer. */
} ble_l2cap_ch_rx_params_t;

/**@brief L2CAP channel setup parameters. */
typedef struct
{
    ble_l2cap_ch_rx_params_t rx_params;     /**< L2CAP channel RX parameters. */
    uint16_t le_psm;                        /**< LE Protocol/Service Multiplexer. Used when requesting
                                                 setup of an L2CAP channel, ignored otherwise. */
    uint16_t status;                        /**< Status code, see @ref BLE_L2CAP_CH_STATUS_CODES.
                                                 Used when replying to a setup request of an L2CAP
                                                 channel, ignored otherwise. */
} ble_l2cap_ch_setup_params_t;

/**@brief L2CAP channel TX parameters. */
typedef struct
{
    uint16_t tx_mtu;         /**< The maximum L2CAP SDU size, in bytes, that L2CAP is able to
                                  transmit on this L2CAP channel. */
    uint16_t peer_mps;       /**< The maximum L2CAP PDU payload size, in bytes, that the peer is
                                  able to receive on this L2CAP channel. */
    uint16_t tx_mps;         /**< The maximum L2CAP PDU payload size, in bytes, that L2CAP is able
                                  to transmit on this L2CAP channel. This is effective tx_mps,
                                  selected by the SoftDevice as
                                  MIN( @ref ble_l2cap_ch_tx_params_t::peer_mps, @ref ble_l2cap_conn_cfg_t::tx_mps ) */
    uint16_t credits;        /**< Initial credits given by the peer. */
} ble_l2cap_ch_tx_params_t;

/**@brief L2CAP Channel Setup Request event. */
typedef struct
{
    ble_l2cap_ch_tx_params_t tx_params; /**< L2CAP channel TX parameters. */
    uint16_t le_psm;                    /**< LE Protocol/Service Multiplexer. */
} ble_l2cap_evt_ch_setup_request_t;

/**@brief L2CAP Channel Setup Refused event. */
typedef struct
{
    uint8_t source;          /**< Source, see @ref BLE_L2CAP_CH_SETUP_REFUSED_SRCS */
    uint16_t status;         /**< Status code, see @ref BLE_L2CAP_CH_STATUS_CODES */
} ble_l2cap_evt_ch_setup_refused_t;

/**@brief L2CAP Channel Setup Completed event. */
typedef struct
{
    ble_l2cap_ch_tx_params_t tx_params; /**< L2CAP channel TX parameters. */
} ble_l2cap_evt_ch_setup_t;

/**@brief L2CAP Channel SDU Data Buffer Released event. */
typedef struct
{
    ble_data_t sdu_buf;      /**< Returned reception or transmission SDU data buffer. The SoftDevice
                                  returns SDU data buffers supplied by the application, which have
                                  not yet been returned previously via a @ref BLE_L2CAP_EVT_CH_RX or
                                  @ref BLE_L2CAP_EVT_CH_TX event. */
} ble_l2cap_evt_ch_sdu_buf_released_t;

/**@brief L2CAP Channel Credit received event. */
typedef struct
{
    uint16_t credits;        /**< Additional credits given by the peer. */
} ble_l2cap_evt_ch_credit_t;

/**@brief L2CAP Channel received SDU event. */
typedef struct
{
    uint16_t sdu_len;        /**< Total SDU length, in bytes. */
    ble_data_t sdu_buf;      /**< SDU data buffer.
                                  @note If there is not enough space in the buffer
                                        (sdu_buf.len < sdu_len) then the rest of the SDU will be
                                        silently discarded by the SoftDevice. */
} ble_l2cap_evt_ch_rx_t;

/**@brief L2CAP Channel transmitted SDU event. */
typedef struct
{
    ble_data_t sdu_buf;      /**< SDU data buffer. */
} ble_l2cap_evt_ch_tx_t;

/**@brief L2CAP event structure. */
typedef struct
{
    uint16_t conn_handle;                                   /**< Connection Handle on which the event occured. */
    uint16_t local_cid;                                     /**< Local Channel ID of the L2CAP channel, or
                                                                 @ref BLE_L2CAP_CID_INVALID if not present. */
    union
    {
        ble_l2cap_evt_ch_setup_request_t ch_setup_request;  /**< L2CAP Channel Setup Request Event Parameters. */
        ble_l2cap_evt_ch_setup_refused_t ch_setup_refused;  /**< L2CAP Channel Setup Refused Event Parameters. */
        ble_l2cap_evt_ch_setup_t ch_setup;                  /**< L2CAP Channel Setup Completed Event Parameters. */
        ble_l2cap_evt_ch_sdu_buf_released_t ch_sdu_buf_released;/**< L2CAP Channel SDU Data Buffer Released Event Parameters. */
        ble_l2cap_evt_ch_credit_t credit;                   /**< L2CAP Channel Credit Received Event Parameters. */
        ble_l2cap_evt_ch_rx_t rx;                           /**< L2CAP Channel SDU Received Event Parameters. */
        ble_l2cap_evt_ch_tx_t tx;                           /**< L2CAP Channel SDU Transmitted Event Parameters. */
    } params;                                               /**< Event Parameters. */
} ble_l2cap_evt_t;

/** @} */

/**@addtogroup BLE_L2CAP_FUNCTIONS Functions
 * @{ */

/**@brief Set up an L2CAP channel.
 *
 * @details This function is used to:
 *          - Request setup of an L2CAP channel: sends an LE Credit Based Connection Request packet to a peer.
 *          - Reply to a setup request of an L2CAP channel (if called in response to a
 *            @ref BLE_L2CAP_EVT_CH_SETUP_REQUEST event): sends an LE Credit Based Connection
 *            Response packet to a peer.
 *
 * @note    A call to this function will require the application to keep the SDU data buffer alive
 *          until the SDU data buffer is returned in @ref BLE_L2CAP_EVT_CH_RX or
 *          @ref BLE_L2CAP_EVT_CH_SDU_BUF_RELEASED event.
 *
 * @events
 * @event{@ref BLE_L2CAP_EVT_CH_SETUP, Setup successful.}
 * @event{@ref BLE_L2CAP_EVT_CH_SETUP_REFUSED, Setup failed.}
 * @endevents
 *
 * @mscs
 * @mmsc{@ref BLE_L2CAP_CH_SETUP_MSC}
 * @endmscs
 *
 * @param[in] conn_handle      Connection Handle.
 * @param[in,out] p_local_cid  Pointer to a uint16_t containing Local Channel ID of the L2CAP channel:
 *                             - As input: @ref BLE_L2CAP_CID_INVALID when requesting setup of an L2CAP
 *                               channel or local_cid provided in the @ref BLE_L2CAP_EVT_CH_SETUP_REQUEST
 *                               event when replying to a setup request of an L2CAP channel.
 *                             - As output: local_cid for this channel.
 * @param[in] p_params         L2CAP channel parameters.
 *
 * @retval ::NRF_SUCCESS                    Successfully queued request or response for transmission.
 * @retval ::NRF_ERROR_BUSY                 The stack is busy, process pending events and retry.
 * @retval ::NRF_ERROR_INVALID_ADDR         Invalid pointer supplied.
 * @retval ::BLE_ERROR_INVALID_CONN_HANDLE  Invalid Connection Handle.
 * @retval ::NRF_ERROR_INVALID_PARAM        Invalid parameter(s) supplied.
 * @retval ::NRF_ERROR_INVALID_LENGTH       Supplied higher rx_mps than has been configured on this link.
 * @retval ::NRF_ERROR_INVALID_STATE        Invalid State to perform operation (L2CAP channel already set up).
 * @retval ::NRF_ERROR_NOT_FOUND            CID not found.
 * @retval ::NRF_ERROR_RESOURCES            The limit has been reached for available L2CAP channels,
 *                                          see @ref ble_l2cap_conn_cfg_t::ch_count.
 */
SVCALL(SD_BLE_L2CAP_CH_SETUP, uint32_t, sd_ble_l2cap_ch_setup(uint16_t conn_handle, uint16_t * p_local_cid, ble_l2cap_ch_setup_params_t const *p_params));

/**@brief Release an L2CAP channel.
 *
 * @details This sends a Disconnection Request packet to a peer.
 *
 * @events
 * @event{@ref BLE_L2CAP_EVT_CH_RELEASED, Release complete.}
 * @endevents
 *
 * @mscs
 * @mmsc{@ref BLE_L2CAP_CH_RELEASE_MSC}
 * @endmscs
 *
 * @param[in] conn_handle   Connection Handle.
 * @param[in] local_cid     Local Channel ID of the L2CAP channel.
 *
 * @retval ::NRF_SUCCESS                    Successfully queued request for transmission.
 * @retval ::BLE_ERROR_INVALID_CONN_HANDLE  Invalid Connection Handle.
 * @retval ::NRF_ERROR_INVALID_STATE        Invalid State to perform operation (Setup or release is
 *                                          in progress for the L2CAP channel).
 * @retval ::NRF_ERROR_NOT_FOUND            CID not found.
 */
SVCALL(SD_BLE_L2CAP_CH_RELEASE, uint32_t, sd_ble_l2cap_ch_release(uint16_t conn_handle, uint16_t local_cid));

/**@brief Receive an SDU on an L2CAP channel.
 *
 * @details This may issue additional credits to the peer using an LE Flow Control Credit packet.
 *
 * @note    A call to this function will require the application to keep the memory pointed by
 *          @ref ble_data_t::p_data alive until the SDU data buffer is returned in @ref BLE_L2CAP_EVT_CH_RX
 *          or @ref BLE_L2CAP_EVT_CH_SDU_BUF_RELEASED event.
 *
 * @note    The SoftDevice can queue up to @ref ble_l2cap_conn_cfg_t::rx_queue_size SDU data buffers
 *          for reception per L2CAP channel.
 *
 * @events
 * @event{@ref BLE_L2CAP_EVT_CH_RX, The SDU is received.}
 * @endevents
 *
 * @mscs
 * @mmsc{@ref BLE_L2CAP_CH_RX_MSC}
 * @endmscs
 *
 * @param[in] conn_handle Connection Handle.
 * @param[in] local_cid   Local Channel ID of the L2CAP channel.
 * @param[in] p_sdu_buf   Pointer to the SDU data buffer.
 *
 * @retval ::NRF_SUCCESS                    Buffer accepted.
 * @retval ::NRF_ERROR_INVALID_ADDR         Invalid pointer supplied.
 * @retval ::BLE_ERROR_INVALID_CONN_HANDLE  Invalid Connection Handle.
 * @retval ::NRF_ERROR_INVALID_STATE        Invalid State to perform operation (Setup or release is
 *                                          in progress for an L2CAP channel).
 * @retval ::NRF_ERROR_NOT_FOUND            CID not found.
 * @retval ::NRF_ERROR_RESOURCES            Too many SDU data buffers supplied. Wait for a
 *                                          @ref BLE_L2CAP_EVT_CH_RX event and retry.
 */
SVCALL(SD_BLE_L2CAP_CH_RX, uint32_t, sd_ble_l2cap_ch_rx(uint16_t conn_handle, uint16_t local_cid, ble_data_t const *p_sdu_buf));

/**@brief Transmit an SDU on an L2CAP channel.
 *
 * @note    A call to this function will require the application to keep the memory pointed by
 *          @ref ble_data_t::p_data alive until the SDU data buffer is returned in @ref BLE_L2CAP_EVT_CH_TX
 *          or @ref BLE_L2CAP_EVT_CH_SDU_BUF_RELEASED event.
 *
 * @note    The SoftDevice can queue up to @ref ble_l2cap_conn_cfg_t::tx_queue_size SDUs for
 *          transmission per L2CAP channel.
 *
 * @note    The application can keep track of the available credits for transmission by following
 *          the procedure below:
 *          - Store initial credits given by the peer in a variable.
 *            (Initial credits are provided in a @ref BLE_L2CAP_EVT_CH_SETUP event.)
 *          - Decrement the variable, which stores the currently available credits, by
 *            ceiling((@ref ble_data_t::len + 2) / tx_mps) when a call to this function returns
 *            @ref NRF_SUCCESS. (tx_mps is provided in a @ref BLE_L2CAP_EVT_CH_SETUP event.)
 *          - Increment the variable, which stores the currently available credits, by additional
 *            credits given by the peer in a @ref BLE_L2CAP_EVT_CH_CREDIT event.
 *
 * @events
 * @event{@ref BLE_L2CAP_EVT_CH_TX, The SDU is transmitted.}
 * @endevents
 *
 * @mscs
 * @mmsc{@ref BLE_L2CAP_CH_TX_MSC}
 * @endmscs
 *
 * @param[in] conn_handle Connection Handle.
 * @param[in] local_cid   Local Channel ID of the L2CAP channel.
 * @param[in] p_sdu_buf   Pointer to the SDU data buffer.
 *
 * @retval ::NRF_SUCCESS                    Successfully queued L2CAP SDU for transmission.
 * @retval ::NRF_ERROR_INVALID_ADDR         Invalid pointer supplied.
 * @retval ::BLE_ERROR_INVALID_CONN_HANDLE  Invalid Connection Handle.
 * @retval ::NRF_ERROR_INVALID_STATE        Invalid State to perform operation (Setup or release is
 *                                          in progress for the L2CAP channel).
 * @retval ::NRF_ERROR_NOT_FOUND            CID not found.
 * @retval ::NRF_ERROR_DATA_SIZE            Invalid SDU length supplied, must not be more than
 *                                          @ref ble_l2cap_ch_tx_params_t::tx_mtu provided in
 *                                          @ref BLE_L2CAP_EVT_CH_SETUP event.
 * @retval ::NRF_ERROR_RESOURCES            Too many SDUs queued for transmission. Wait for a
 *                                          @ref BLE_L2CAP_EVT_CH_TX event and retry.
 */
SVCALL(SD_BLE_L2CAP_CH_TX, uint32_t, sd_ble_l2cap_ch_tx(uint16_t conn_handle, uint16_t local_cid, ble_data_t const *p_sdu_buf));

/**@brief Advanced SDU reception flow control.
 *
 * @details Adjust the way the SoftDevice issues credits to the peer.
 *          This may issue additional credits to the peer using an LE Flow Control Credit packet.
 *
 * @mscs
 * @mmsc{@ref BLE_L2CAP_CH_FLOW_CONTROL_MSC}
 * @endmscs
 *
 * @param[in] conn_handle Connection Handle.
 * @param[in] local_cid   Local Channel ID of the L2CAP channel or @ref BLE_L2CAP_CID_INVALID to set
 *                        the value that will be used for newly created channels.
 * @param[in] credits     Number of credits that the SoftDevice will make sure the peer has every
 *                        time it starts using a new reception buffer.
 *                        - @ref BLE_L2CAP_CREDITS_DEFAULT is the default value the SoftDevice will
 *                          use if this function is not called.
 *                        - If set to zero, the SoftDevice will stop issuing credits for new reception
 *                          buffers the application provides or has provided. SDU reception that is
 *                          currently ongoing will be allowed to complete.
 * @param[out] p_credits  NULL or pointer to a uint16_t. If a valid pointer is provided, it will be
 *                        written by the SoftDevice with the number of credits that is or will be
 *                        available to the peer. If the value written by the SoftDevice is 0 when
 *                        credits parameter was set to 0, the peer will not be able to send more
 *                        data until more credits are provided by calling this function again with
 *                        credits > 0. This parameter is ignored when local_cid is set to
 *                        @ref BLE_L2CAP_CID_INVALID.
 *
 * @note Application should take care when setting number of credits higher than default value. In
 *       this case the application must make sure that the SoftDevice always has reception buffers
 *       available (see @ref sd_ble_l2cap_ch_rx) for that channel. If the SoftDevice does not have
 *       such buffers available, packets may be NACKed on the Link Layer and all Bluetooth traffic
 *       on the connection handle may be stalled until the SoftDevice again has an available
 *       reception buffer. This applies even if the application has used this call to set the
 *       credits back to default, or zero.
 *
 * @retval ::NRF_SUCCESS                    Flow control parameters accepted.
 * @retval ::NRF_ERROR_INVALID_ADDR         Invalid pointer supplied.
 * @retval ::BLE_ERROR_INVALID_CONN_HANDLE  Invalid Connection Handle.
 * @retval ::NRF_ERROR_INVALID_STATE        Invalid State to perform operation (Setup or release is
 *                                          in progress for an L2CAP channel).
 * @retval ::NRF_ERROR_NOT_FOUND            CID not found.
 */
SVCALL(SD_BLE_L2CAP_CH_FLOW_CONTROL, uint32_t, sd_ble_l2cap_ch_flow_control(uint16_t conn_handle, uint16_t local_cid, uint16_t credits, uint16_t * p_credits));

/** @} */

#ifdef __cplusplus
}
#endif
#endif // BLE_L2CAP_H__

/**
  @}
*/
