/*
 * Copyright (c) 2015 - 2018, Nordic Semiconductor ASA
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
 * @defgroup nrf_soc_api SoC Library API
 * @{
 *
 * @brief APIs for the SoC library.
 *
 */

#ifndef NRF_SOC_H__
#define NRF_SOC_H__

#include <stdint.h>
#include "nrf.h"
#include "nrf_svc.h"
#include "nrf_error.h"
#include "nrf_error_soc.h"

#ifdef __cplusplus
extern "C" {
#endif

/**@addtogroup NRF_SOC_DEFINES Defines
 * @{ */

/**@brief The number of the lowest SVC number reserved for the SoC library. */
#define SOC_SVC_BASE               (0x20)                   /**< Base value for SVCs that are available when the SoftDevice is disabled. */
#define SOC_SVC_BASE_NOT_AVAILABLE (0x2C)                   /**< Base value for SVCs that are not available when the SoftDevice is disabled. */

/**@brief Guaranteed time for application to process radio inactive notification. */
#define NRF_RADIO_NOTIFICATION_INACTIVE_GUARANTEED_TIME_US  (62)

/**@brief The minimum allowed timeslot extension time. */
#define NRF_RADIO_MINIMUM_TIMESLOT_LENGTH_EXTENSION_TIME_US (200)

/**@brief The maximum processing time to handle a timeslot extension. */
#define NRF_RADIO_MAX_EXTENSION_PROCESSING_TIME_US           (17)

/**@brief The latest time before the end of a timeslot the timeslot can be extended. */
#define NRF_RADIO_MIN_EXTENSION_MARGIN_US                    (79)

#define SOC_ECB_KEY_LENGTH                (16)                       /**< ECB key length. */
#define SOC_ECB_CLEARTEXT_LENGTH          (16)                       /**< ECB cleartext length. */
#define SOC_ECB_CIPHERTEXT_LENGTH         (SOC_ECB_CLEARTEXT_LENGTH) /**< ECB ciphertext length. */

#define SD_EVT_IRQn                       (SWI2_IRQn)        /**< SoftDevice Event IRQ number. Used for both protocol events and SoC events. */
#define SD_EVT_IRQHandler                 (SWI2_IRQHandler)  /**< SoftDevice Event IRQ handler. Used for both protocol events and SoC events.
                                                                       The default interrupt priority for this handler is set to 6 */
#define RADIO_NOTIFICATION_IRQn           (SWI1_IRQn)        /**< The radio notification IRQ number. */
#define RADIO_NOTIFICATION_IRQHandler     (SWI1_IRQHandler)  /**< The radio notification IRQ handler.
                                                                       The default interrupt priority for this handler is set to 6 */
#define NRF_RADIO_LENGTH_MIN_US           (100)               /**< The shortest allowed radio timeslot, in microseconds. */
#define NRF_RADIO_LENGTH_MAX_US           (100000)            /**< The longest allowed radio timeslot, in microseconds. */

#define NRF_RADIO_DISTANCE_MAX_US         (128000000UL - 1UL) /**< The longest timeslot distance, in microseconds, allowed for the distance parameter (see @ref nrf_radio_request_normal_t) in the request. */

#define NRF_RADIO_EARLIEST_TIMEOUT_MAX_US (128000000UL - 1UL) /**< The longest timeout, in microseconds, allowed when requesting the earliest possible timeslot. */

#define NRF_RADIO_START_JITTER_US         (2)                 /**< The maximum jitter in @ref NRF_RADIO_CALLBACK_SIGNAL_TYPE_START relative to the requested start time. */

/**@brief Mask of PPI channels reserved by the SoftDevice when the SoftDevice is disabled. */
#define NRF_SOC_SD_PPI_CHANNELS_SD_DISABLED_MSK ((uint32_t)(0))

/**@brief Mask of PPI channels reserved by the SoftDevice when the SoftDevice is enabled. */
#define NRF_SOC_SD_PPI_CHANNELS_SD_ENABLED_MSK  ((uint32_t)( \
    (1U << 17) \
    | (1U << 18) \
    | (1U << 19) \
    | (1U << 20) \
    | (1U << 21) \
    | (1U << 22) \
    | (1U << 23) \
    | (1U << 24) \
    | (1U << 25) \
    | (1U << 26) \
    | (1U << 27) \
    | (1U << 28) \
    | (1U << 29) \
    | (1U << 30) \
    | (1U << 31) \
    ))

/**@brief Mask of PPI channels available to the application when the SoftDevice is disabled. */
#define NRF_SOC_APP_PPI_CHANNELS_SD_DISABLED_MSK (~NRF_SOC_SD_PPI_CHANNELS_SD_DISABLED_MSK)

/**@brief Mask of PPI channels available to the application when the SoftDevice is enabled. */
#define NRF_SOC_APP_PPI_CHANNELS_SD_ENABLED_MSK  (~NRF_SOC_SD_PPI_CHANNELS_SD_ENABLED_MSK)

/**@brief Mask of PPI groups reserved by the SoftDevice when the SoftDevice is disabled. */
#define NRF_SOC_SD_PPI_GROUPS_SD_DISABLED_MSK    ((uint32_t)(0))

/**@brief Mask of PPI groups reserved by the SoftDevice when the SoftDevice is enabled. */
#define NRF_SOC_SD_PPI_GROUPS_SD_ENABLED_MSK     ((uint32_t)( \
    (1U << 4) \
    | (1U << 5) \
    ))

/**@brief Mask of PPI groups available to the application when the SoftDevice is disabled. */
#define NRF_SOC_APP_PPI_GROUPS_SD_DISABLED_MSK   (~NRF_SOC_SD_PPI_GROUPS_SD_DISABLED_MSK)

/**@brief Mask of PPI groups available to the application when the SoftDevice is enabled. */
#define NRF_SOC_APP_PPI_GROUPS_SD_ENABLED_MSK    (~NRF_SOC_SD_PPI_GROUPS_SD_ENABLED_MSK)

/**@} */

/**@addtogroup NRF_SOC_ENUMS Enumerations
 * @{ */

/**@brief The SVC numbers used by the SVC functions in the SoC library. */
enum NRF_SOC_SVCS
{
    SD_PPI_CHANNEL_ENABLE_GET   = SOC_SVC_BASE,
    SD_PPI_CHANNEL_ENABLE_SET   = SOC_SVC_BASE + 1,
    SD_PPI_CHANNEL_ENABLE_CLR   = SOC_SVC_BASE + 2,
    SD_PPI_CHANNEL_ASSIGN       = SOC_SVC_BASE + 3,
    SD_PPI_GROUP_TASK_ENABLE    = SOC_SVC_BASE + 4,
    SD_PPI_GROUP_TASK_DISABLE   = SOC_SVC_BASE + 5,
    SD_PPI_GROUP_ASSIGN         = SOC_SVC_BASE + 6,
    SD_PPI_GROUP_GET            = SOC_SVC_BASE + 7,
    SD_FLASH_PAGE_ERASE         = SOC_SVC_BASE + 8,
    SD_FLASH_WRITE              = SOC_SVC_BASE + 9,
    SD_PROTECTED_REGISTER_WRITE = SOC_SVC_BASE + 11,
    SD_MUTEX_NEW                            = SOC_SVC_BASE_NOT_AVAILABLE,
    SD_MUTEX_ACQUIRE                        = SOC_SVC_BASE_NOT_AVAILABLE + 1,
    SD_MUTEX_RELEASE                        = SOC_SVC_BASE_NOT_AVAILABLE + 2,
    SD_RAND_APPLICATION_POOL_CAPACITY_GET   = SOC_SVC_BASE_NOT_AVAILABLE + 3,
    SD_RAND_APPLICATION_BYTES_AVAILABLE_GET = SOC_SVC_BASE_NOT_AVAILABLE + 4,
    SD_RAND_APPLICATION_VECTOR_GET          = SOC_SVC_BASE_NOT_AVAILABLE + 5,
    SD_POWER_MODE_SET                       = SOC_SVC_BASE_NOT_AVAILABLE + 6,
    SD_POWER_SYSTEM_OFF                     = SOC_SVC_BASE_NOT_AVAILABLE + 7,
    SD_POWER_RESET_REASON_GET               = SOC_SVC_BASE_NOT_AVAILABLE + 8,
    SD_POWER_RESET_REASON_CLR               = SOC_SVC_BASE_NOT_AVAILABLE + 9,
    SD_POWER_POF_ENABLE                     = SOC_SVC_BASE_NOT_AVAILABLE + 10,
    SD_POWER_POF_THRESHOLD_SET              = SOC_SVC_BASE_NOT_AVAILABLE + 11,
    SD_POWER_POF_THRESHOLDVDDH_SET          = SOC_SVC_BASE_NOT_AVAILABLE + 12,
    SD_POWER_RAM_POWER_SET                  = SOC_SVC_BASE_NOT_AVAILABLE + 13,
    SD_POWER_RAM_POWER_CLR                  = SOC_SVC_BASE_NOT_AVAILABLE + 14,
    SD_POWER_RAM_POWER_GET                  = SOC_SVC_BASE_NOT_AVAILABLE + 15,
    SD_POWER_GPREGRET_SET                   = SOC_SVC_BASE_NOT_AVAILABLE + 16,
    SD_POWER_GPREGRET_CLR                   = SOC_SVC_BASE_NOT_AVAILABLE + 17,
    SD_POWER_GPREGRET_GET                   = SOC_SVC_BASE_NOT_AVAILABLE + 18,
    SD_POWER_DCDC_MODE_SET                  = SOC_SVC_BASE_NOT_AVAILABLE + 19,
    SD_POWER_DCDC0_MODE_SET                 = SOC_SVC_BASE_NOT_AVAILABLE + 20,
    SD_APP_EVT_WAIT                         = SOC_SVC_BASE_NOT_AVAILABLE + 21,
    SD_CLOCK_HFCLK_REQUEST                  = SOC_SVC_BASE_NOT_AVAILABLE + 22,
    SD_CLOCK_HFCLK_RELEASE                  = SOC_SVC_BASE_NOT_AVAILABLE + 23,
    SD_CLOCK_HFCLK_IS_RUNNING               = SOC_SVC_BASE_NOT_AVAILABLE + 24,
    SD_RADIO_NOTIFICATION_CFG_SET           = SOC_SVC_BASE_NOT_AVAILABLE + 25,
    SD_ECB_BLOCK_ENCRYPT                    = SOC_SVC_BASE_NOT_AVAILABLE + 26,
    SD_ECB_BLOCKS_ENCRYPT                   = SOC_SVC_BASE_NOT_AVAILABLE + 27,
    SD_RADIO_SESSION_OPEN                   = SOC_SVC_BASE_NOT_AVAILABLE + 28,
    SD_RADIO_SESSION_CLOSE                  = SOC_SVC_BASE_NOT_AVAILABLE + 29,
    SD_RADIO_REQUEST                        = SOC_SVC_BASE_NOT_AVAILABLE + 30,
    SD_EVT_GET                              = SOC_SVC_BASE_NOT_AVAILABLE + 31,
    SD_TEMP_GET                             = SOC_SVC_BASE_NOT_AVAILABLE + 32,
    SD_POWER_USBPWRRDY_ENABLE               = SOC_SVC_BASE_NOT_AVAILABLE + 33,
    SD_POWER_USBDETECTED_ENABLE             = SOC_SVC_BASE_NOT_AVAILABLE + 34,
    SD_POWER_USBREMOVED_ENABLE              = SOC_SVC_BASE_NOT_AVAILABLE + 35,
    SD_POWER_USBREGSTATUS_GET               = SOC_SVC_BASE_NOT_AVAILABLE + 36,
    SVC_SOC_LAST                            = SOC_SVC_BASE_NOT_AVAILABLE + 37
};

/**@brief Possible values of a ::nrf_mutex_t. */
enum NRF_MUTEX_VALUES
{
    NRF_MUTEX_FREE,
    NRF_MUTEX_TAKEN
};

/**@brief Power modes. */
enum NRF_POWER_MODES
{
    NRF_POWER_MODE_CONSTLAT, /**< Constant latency mode. See power management in the reference manual. */
    NRF_POWER_MODE_LOWPWR   /**< Low power mode. See power management in the reference manual. */
};


/**@brief Power failure thresholds */
enum NRF_POWER_THRESHOLDS
{
    NRF_POWER_THRESHOLD_V17 = 4UL, /**< 1.7 Volts power failure threshold. */
    NRF_POWER_THRESHOLD_V18,     /**< 1.8 Volts power failure threshold. */
    NRF_POWER_THRESHOLD_V19,     /**< 1.9 Volts power failure threshold. */
    NRF_POWER_THRESHOLD_V20,     /**< 2.0 Volts power failure threshold. */
    NRF_POWER_THRESHOLD_V21,     /**< 2.1 Volts power failure threshold. */
    NRF_POWER_THRESHOLD_V22,     /**< 2.2 Volts power failure threshold. */
    NRF_POWER_THRESHOLD_V23,     /**< 2.3 Volts power failure threshold. */
    NRF_POWER_THRESHOLD_V24,     /**< 2.4 Volts power failure threshold. */
    NRF_POWER_THRESHOLD_V25,     /**< 2.5 Volts power failure threshold. */
    NRF_POWER_THRESHOLD_V26,     /**< 2.6 Volts power failure threshold. */
    NRF_POWER_THRESHOLD_V27,     /**< 2.7 Volts power failure threshold. */
    NRF_POWER_THRESHOLD_V28      /**< 2.8 Volts power failure threshold. */
};

/**@brief Power failure thresholds for high voltage */
enum NRF_POWER_THRESHOLDVDDHS
{
    NRF_POWER_THRESHOLDVDDH_V27,     /**< 2.7 Volts power failure threshold. */
    NRF_POWER_THRESHOLDVDDH_V28,     /**< 2.8 Volts power failure threshold. */
    NRF_POWER_THRESHOLDVDDH_V29,     /**< 2.9 Volts power failure threshold. */
    NRF_POWER_THRESHOLDVDDH_V30,     /**< 3.0 Volts power failure threshold. */
    NRF_POWER_THRESHOLDVDDH_V31,     /**< 3.1 Volts power failure threshold. */
    NRF_POWER_THRESHOLDVDDH_V32,     /**< 3.2 Volts power failure threshold. */
    NRF_POWER_THRESHOLDVDDH_V33,     /**< 3.3 Volts power failure threshold. */
    NRF_POWER_THRESHOLDVDDH_V34,     /**< 3.4 Volts power failure threshold. */
    NRF_POWER_THRESHOLDVDDH_V35,     /**< 3.5 Volts power failure threshold. */
    NRF_POWER_THRESHOLDVDDH_V36,     /**< 3.6 Volts power failure threshold. */
    NRF_POWER_THRESHOLDVDDH_V37,     /**< 3.7 Volts power failure threshold. */
    NRF_POWER_THRESHOLDVDDH_V38,     /**< 3.8 Volts power failure threshold. */
    NRF_POWER_THRESHOLDVDDH_V39,     /**< 3.9 Volts power failure threshold. */
    NRF_POWER_THRESHOLDVDDH_V40,     /**< 4.0 Volts power failure threshold. */
    NRF_POWER_THRESHOLDVDDH_V41,     /**< 4.1 Volts power failure threshold. */
    NRF_POWER_THRESHOLDVDDH_V42      /**< 4.2 Volts power failure threshold. */
};


/**@brief DC/DC converter modes. */
enum NRF_POWER_DCDC_MODES
{
    NRF_POWER_DCDC_DISABLE,        /**< The DCDC is disabled. */
    NRF_POWER_DCDC_ENABLE          /**< The DCDC is enabled.  */
};

/**@brief Radio notification distances. */
enum NRF_RADIO_NOTIFICATION_DISTANCES
{
    NRF_RADIO_NOTIFICATION_DISTANCE_NONE = 0, /**< The event does not have a notification. */
    NRF_RADIO_NOTIFICATION_DISTANCE_800US,  /**< The distance from the active notification to start of radio activity. */
    NRF_RADIO_NOTIFICATION_DISTANCE_1740US, /**< The distance from the active notification to start of radio activity. */
    NRF_RADIO_NOTIFICATION_DISTANCE_2680US, /**< The distance from the active notification to start of radio activity. */
    NRF_RADIO_NOTIFICATION_DISTANCE_3620US, /**< The distance from the active notification to start of radio activity. */
    NRF_RADIO_NOTIFICATION_DISTANCE_4560US, /**< The distance from the active notification to start of radio activity. */
    NRF_RADIO_NOTIFICATION_DISTANCE_5500US  /**< The distance from the active notification to start of radio activity. */
};


/**@brief Radio notification types. */
enum NRF_RADIO_NOTIFICATION_TYPES
{
    NRF_RADIO_NOTIFICATION_TYPE_NONE = 0,      /**< The event does not have a radio notification signal. */
    NRF_RADIO_NOTIFICATION_TYPE_INT_ON_ACTIVE, /**< Using interrupt for notification when the radio will be enabled. */
    NRF_RADIO_NOTIFICATION_TYPE_INT_ON_INACTIVE, /**< Using interrupt for notification when the radio has been disabled. */
    NRF_RADIO_NOTIFICATION_TYPE_INT_ON_BOTH,   /**< Using interrupt for notification both when the radio will be enabled and disabled. */
};

/**@brief The Radio signal callback types. */
enum NRF_RADIO_CALLBACK_SIGNAL_TYPE
{
    NRF_RADIO_CALLBACK_SIGNAL_TYPE_START,           /**< This signal indicates the start of the radio timeslot. */
    NRF_RADIO_CALLBACK_SIGNAL_TYPE_TIMER0,          /**< This signal indicates the NRF_TIMER0 interrupt. */
    NRF_RADIO_CALLBACK_SIGNAL_TYPE_RADIO,           /**< This signal indicates the NRF_RADIO interrupt. */
    NRF_RADIO_CALLBACK_SIGNAL_TYPE_EXTEND_FAILED,   /**< This signal indicates extend action failed. */
    NRF_RADIO_CALLBACK_SIGNAL_TYPE_EXTEND_SUCCEEDED /**< This signal indicates extend action succeeded. */
};

/**@brief The actions requested by the signal callback.
 *
 *  This code gives the SOC instructions about what action to take when the signal callback has
 *  returned.
 */
enum NRF_RADIO_SIGNAL_CALLBACK_ACTION
{
    NRF_RADIO_SIGNAL_CALLBACK_ACTION_NONE,          /**< Return without action. */
    NRF_RADIO_SIGNAL_CALLBACK_ACTION_EXTEND,        /**< Request an extension of the current
                                                         timeslot. Maximum execution time for this action:
                                                         @ref NRF_RADIO_MAX_EXTENSION_PROCESSING_TIME_US.
                                                         This action must be started at least
                                                         @ref NRF_RADIO_MIN_EXTENSION_MARGIN_US before
                                                         the end of the timeslot. */
    NRF_RADIO_SIGNAL_CALLBACK_ACTION_END,           /**< End the current radio timeslot. */
    NRF_RADIO_SIGNAL_CALLBACK_ACTION_REQUEST_AND_END /**< Request a new radio timeslot and end the current timeslot. */
};

/**@brief Radio timeslot high frequency clock source configuration. */
enum NRF_RADIO_HFCLK_CFG
{
    NRF_RADIO_HFCLK_CFG_XTAL_GUARANTEED, /**< The SoftDevice will guarantee that the high frequency clock source is the
                                           external crystal for the whole duration of the timeslot. This should be the
                                           preferred option for events that use the radio or require high timing accuracy.
                                           @note The SoftDevice will automatically turn on and off the external crystal,
                                           at the beginning and end of the timeslot, respectively. The crystal may also
                                           intentionally be left running after the timeslot, in cases where it is needed
                                           by the SoftDevice shortly after the end of the timeslot. */
    NRF_RADIO_HFCLK_CFG_NO_GUARANTEE  /**< This configuration allows for earlier and tighter scheduling of timeslots.
                                           The RC oscillator may be the clock source in part or for the whole duration of the timeslot.
                                           The RC oscillator's accuracy must therefore be taken into consideration.
                                           @note If the application will use the radio peripheral in timeslots with this configuration,
                                           it must make sure that the crystal is running and stable before starting the radio. */
};

/**@brief Radio timeslot priorities. */
enum NRF_RADIO_PRIORITY
{
    NRF_RADIO_PRIORITY_HIGH,                        /**< High (equal priority as the normal connection priority of the SoftDevice stack(s)). */
    NRF_RADIO_PRIORITY_NORMAL,                      /**< Normal (equal priority as the priority of secondary activities of the SoftDevice stack(s)). */
};

/**@brief Radio timeslot request type. */
enum NRF_RADIO_REQUEST_TYPE
{
    NRF_RADIO_REQ_TYPE_EARLIEST,                    /**< Request radio timeslot as early as possible. This should always be used for the first request in a session. */
    NRF_RADIO_REQ_TYPE_NORMAL                       /**< Normal radio timeslot request. */
};

/**@brief SoC Events. */
enum NRF_SOC_EVTS
{
    NRF_EVT_HFCLKSTARTED,                       /**< Event indicating that the HFCLK has started. */
    NRF_EVT_POWER_FAILURE_WARNING,              /**< Event indicating that a power failure warning has occurred. */
    NRF_EVT_FLASH_OPERATION_SUCCESS,            /**< Event indicating that the ongoing flash operation has completed successfully. */
    NRF_EVT_FLASH_OPERATION_ERROR,              /**< Event indicating that the ongoing flash operation has timed out with an error. */
    NRF_EVT_RADIO_BLOCKED,                      /**< Event indicating that a radio timeslot was blocked. */
    NRF_EVT_RADIO_CANCELED,                     /**< Event indicating that a radio timeslot was canceled by SoftDevice. */
    NRF_EVT_RADIO_SIGNAL_CALLBACK_INVALID_RETURN, /**< Event indicating that a radio timeslot signal callback handler return was invalid. */
    NRF_EVT_RADIO_SESSION_IDLE,                 /**< Event indicating that a radio timeslot session is idle. */
    NRF_EVT_RADIO_SESSION_CLOSED,               /**< Event indicating that a radio timeslot session is closed. */
    NRF_EVT_POWER_USB_POWER_READY,              /**< Event indicating that a USB 3.3 V supply is ready. */
    NRF_EVT_POWER_USB_DETECTED,                 /**< Event indicating that voltage supply is detected on VBUS. */
    NRF_EVT_POWER_USB_REMOVED,                  /**< Event indicating that voltage supply is removed from VBUS. */
    NRF_EVT_NUMBER_OF_EVTS
};

/**@} */


/**@addtogroup NRF_SOC_STRUCTURES Structures
 * @{ */

/**@brief Represents a mutex for use with the nrf_mutex functions.
 * @note Accessing the value directly is not safe, use the mutex functions!
 */
typedef volatile uint8_t nrf_mutex_t;

/**@brief Parameters for a request for a timeslot as early as possible. */
typedef struct
{
    uint8_t hfclk;                                  /**< High frequency clock source, see @ref NRF_RADIO_HFCLK_CFG. */
    uint8_t priority;                               /**< The radio timeslot priority, see @ref NRF_RADIO_PRIORITY. */
    uint32_t length_us;                             /**< The radio timeslot length (in the range 100 to 100,000] microseconds). */
    uint32_t timeout_us;                            /**< Longest acceptable delay until the start of the requested timeslot (up to @ref NRF_RADIO_EARLIEST_TIMEOUT_MAX_US microseconds). */
} nrf_radio_request_earliest_t;

/**@brief Parameters for a normal radio timeslot request. */
typedef struct
{
    uint8_t hfclk;                                  /**< High frequency clock source, see @ref NRF_RADIO_HFCLK_CFG. */
    uint8_t priority;                               /**< The radio timeslot priority, see @ref NRF_RADIO_PRIORITY. */
    uint32_t distance_us;                           /**< Distance from the start of the previous radio timeslot (up to @ref NRF_RADIO_DISTANCE_MAX_US microseconds). */
    uint32_t length_us;                             /**< The radio timeslot length (in the range [100..100,000] microseconds). */
} nrf_radio_request_normal_t;

/**@brief Radio timeslot request parameters. */
typedef struct
{
    uint8_t request_type;                           /**< Type of request, see @ref NRF_RADIO_REQUEST_TYPE. */
    union
    {
        nrf_radio_request_earliest_t earliest;      /**< Parameters for requesting a radio timeslot as early as possible. */
        nrf_radio_request_normal_t normal;          /**< Parameters for requesting a normal radio timeslot. */
    } params;                                       /**< Parameter union. */
} nrf_radio_request_t;

/**@brief Return parameters of the radio timeslot signal callback. */
typedef struct
{
    uint8_t callback_action;                        /**< The action requested by the application when returning from the signal callback, see @ref NRF_RADIO_SIGNAL_CALLBACK_ACTION. */
    union
    {
        struct
        {
            nrf_radio_request_t *p_next;            /**< The request parameters for the next radio timeslot. */
        } request;                                  /**< Additional parameters for return_code @ref NRF_RADIO_SIGNAL_CALLBACK_ACTION_REQUEST_AND_END. */
        struct
        {
            uint32_t length_us;                     /**< Requested extension of the radio timeslot duration (microseconds) (for minimum time see @ref NRF_RADIO_MINIMUM_TIMESLOT_LENGTH_EXTENSION_TIME_US). */
        } extend;                                   /**< Additional parameters for return_code @ref NRF_RADIO_SIGNAL_CALLBACK_ACTION_EXTEND. */
    } params;                                       /**< Parameter union. */
} nrf_radio_signal_callback_return_param_t;

/**@brief The radio timeslot signal callback type.
 *
 * @note In case of invalid return parameters, the radio timeslot will automatically end
 *       immediately after returning from the signal callback and the
 *       @ref NRF_EVT_RADIO_SIGNAL_CALLBACK_INVALID_RETURN event will be sent.
 * @note The returned struct pointer must remain valid after the signal callback
 *       function returns. For instance, this means that it must not point to a stack variable.
 *
 * @param[in] signal_type Type of signal, see @ref NRF_RADIO_CALLBACK_SIGNAL_TYPE.
 *
 * @return Pointer to structure containing action requested by the application.
 */
typedef nrf_radio_signal_callback_return_param_t * (*nrf_radio_signal_callback_t)(uint8_t signal_type);

/**@brief AES ECB parameter typedefs */
typedef uint8_t soc_ecb_key_t[SOC_ECB_KEY_LENGTH];                /**< Encryption key type. */
typedef uint8_t soc_ecb_cleartext_t[SOC_ECB_CLEARTEXT_LENGTH];    /**< Cleartext data type. */
typedef uint8_t soc_ecb_ciphertext_t[SOC_ECB_CIPHERTEXT_LENGTH];  /**< Ciphertext data type. */

/**@brief AES ECB data structure */
typedef struct
{
    soc_ecb_key_t key;                 /**< Encryption key. */
    soc_ecb_cleartext_t cleartext;     /**< Cleartext data. */
    soc_ecb_ciphertext_t ciphertext;   /**< Ciphertext data. */
} nrf_ecb_hal_data_t;

/**@brief AES ECB block. Used to provide multiple blocks in a single call
          to @ref sd_ecb_blocks_encrypt.*/
typedef struct
{
    soc_ecb_key_t const *p_key;                /**< Pointer to the Encryption key. */
    soc_ecb_cleartext_t const *p_cleartext;    /**< Pointer to the Cleartext data. */
    soc_ecb_ciphertext_t *p_ciphertext;        /**< Pointer to the Ciphertext data. */
} nrf_ecb_hal_data_block_t;

/**@} */

/**@addtogroup NRF_SOC_FUNCTIONS Functions
 * @{ */

/**@brief Initialize a mutex.
 *
 * @param[in] p_mutex Pointer to the mutex to initialize.
 *
 * @retval ::NRF_SUCCESS
 */
SVCALL(SD_MUTEX_NEW, uint32_t, sd_mutex_new(nrf_mutex_t * p_mutex));

/**@brief Attempt to acquire a mutex.
 *
 * @param[in] p_mutex Pointer to the mutex to acquire.
 *
 * @retval ::NRF_SUCCESS The mutex was successfully acquired.
 * @retval ::NRF_ERROR_SOC_MUTEX_ALREADY_TAKEN The mutex could not be acquired.
 */
SVCALL(SD_MUTEX_ACQUIRE, uint32_t, sd_mutex_acquire(nrf_mutex_t * p_mutex));

/**@brief Release a mutex.
 *
 * @param[in] p_mutex Pointer to the mutex to release.
 *
 * @retval ::NRF_SUCCESS
 */
SVCALL(SD_MUTEX_RELEASE, uint32_t, sd_mutex_release(nrf_mutex_t * p_mutex));

/**@brief Query the capacity of the application random pool.
 *
 * @param[out] p_pool_capacity The capacity of the pool.
 *
 * @retval ::NRF_SUCCESS
 */
SVCALL(SD_RAND_APPLICATION_POOL_CAPACITY_GET, uint32_t, sd_rand_application_pool_capacity_get(uint8_t * p_pool_capacity));

/**@brief Get number of random bytes available to the application.
 *
 * @param[out] p_bytes_available The number of bytes currently available in the pool.
 *
 * @retval ::NRF_SUCCESS
 */
SVCALL(SD_RAND_APPLICATION_BYTES_AVAILABLE_GET, uint32_t, sd_rand_application_bytes_available_get(uint8_t * p_bytes_available));

/**@brief Get random bytes from the application pool.
 *
 * @param[out]  p_buff  Pointer to unit8_t buffer for storing the bytes.
 * @param[in]   length  Number of bytes to take from pool and place in p_buff.
 *
 * @retval ::NRF_SUCCESS The requested bytes were written to p_buff.
 * @retval ::NRF_ERROR_SOC_RAND_NOT_ENOUGH_VALUES No bytes were written to the buffer, because there were not enough bytes available.
*/
SVCALL(SD_RAND_APPLICATION_VECTOR_GET, uint32_t, sd_rand_application_vector_get(uint8_t * p_buff, uint8_t length));

/**@brief Gets the reset reason register.
 *
 * @param[out]  p_reset_reason  Contents of the NRF_POWER->RESETREAS register.
 *
 * @retval ::NRF_SUCCESS
 */
SVCALL(SD_POWER_RESET_REASON_GET, uint32_t, sd_power_reset_reason_get(uint32_t * p_reset_reason));

/**@brief Clears the bits of the reset reason register.
 *
 * @param[in] reset_reason_clr_msk Contains the bits to clear from the reset reason register.
 *
 * @retval ::NRF_SUCCESS
 */
SVCALL(SD_POWER_RESET_REASON_CLR, uint32_t, sd_power_reset_reason_clr(uint32_t reset_reason_clr_msk));

/**@brief Sets the power mode when in CPU sleep.
 *
 * @param[in] power_mode The power mode to use when in CPU sleep, see @ref NRF_POWER_MODES. @sa sd_app_evt_wait
 *
 * @retval ::NRF_SUCCESS The power mode was set.
 * @retval ::NRF_ERROR_SOC_POWER_MODE_UNKNOWN The power mode was unknown.
 */
SVCALL(SD_POWER_MODE_SET, uint32_t, sd_power_mode_set(uint8_t power_mode));

/**@brief Puts the chip in System OFF mode.
 *
 * @retval ::NRF_ERROR_SOC_POWER_OFF_SHOULD_NOT_RETURN
 */
SVCALL(SD_POWER_SYSTEM_OFF, uint32_t, sd_power_system_off(void));

/**@brief Enables or disables the power-fail comparator.
 *
 * Enabling this will give a SoftDevice event (NRF_EVT_POWER_FAILURE_WARNING) when the power failure warning occurs.
 * The event can be retrieved with sd_evt_get();
 *
 * @param[in] pof_enable    True if the power-fail comparator should be enabled, false if it should be disabled.
 *
 * @retval ::NRF_SUCCESS
 */
SVCALL(SD_POWER_POF_ENABLE, uint32_t, sd_power_pof_enable(uint8_t pof_enable));

/**@brief Enables or disables the USB power ready event.
 *
 * Enabling this will give a SoftDevice event (NRF_EVT_POWER_USB_POWER_READY) when a USB 3.3 V supply is ready.
 * The event can be retrieved with sd_evt_get();
 *
 * @param[in] usbpwrrdy_enable    True if the power ready event should be enabled, false if it should be disabled.
 *
 * @retval ::NRF_SUCCESS
 */
SVCALL(SD_POWER_USBPWRRDY_ENABLE, uint32_t, sd_power_usbpwrrdy_enable(uint8_t usbpwrrdy_enable));

/**@brief Enables or disables the power USB-detected event.
 *
 * Enabling this will give a SoftDevice event (NRF_EVT_POWER_USB_DETECTED) when a voltage supply is detected on VBUS.
 * The event can be retrieved with sd_evt_get();
 *
 * @param[in] usbdetected_enable    True if the power ready event should be enabled, false if it should be disabled.
 *
 * @retval ::NRF_SUCCESS
 */
SVCALL(SD_POWER_USBDETECTED_ENABLE, uint32_t, sd_power_usbdetected_enable(uint8_t usbdetected_enable));

/**@brief Enables or disables the power USB-removed event.
 *
 * Enabling this will give a SoftDevice event (NRF_EVT_POWER_USB_REMOVED) when a voltage supply is removed from VBUS.
 * The event can be retrieved with sd_evt_get();
 *
 * @param[in] usbremoved_enable    True if the power ready event should be enabled, false if it should be disabled.
 *
 * @retval ::NRF_SUCCESS
 */
SVCALL(SD_POWER_USBREMOVED_ENABLE, uint32_t, sd_power_usbremoved_enable(uint8_t usbremoved_enable));

/**@brief Get USB supply status register content.
 *
 * @param[out] usbregstatus    The content of USBREGSTATUS register.
 *
 * @retval ::NRF_SUCCESS
 */
SVCALL(SD_POWER_USBREGSTATUS_GET, uint32_t, sd_power_usbregstatus_get(uint32_t * usbregstatus));

/**@brief Sets the power failure comparator threshold value.
 *
 * @note: Power failure comparator threshold setting. This setting applies both for normal voltage
 *        mode (supply connected to both VDD and VDDH) and high voltage mode (supply connected to
 *        VDDH only).
 *
 * @param[in] threshold The power-fail threshold value to use, see @ref NRF_POWER_THRESHOLDS.
 *
 * @retval ::NRF_SUCCESS The power failure threshold was set.
 * @retval ::NRF_ERROR_SOC_POWER_POF_THRESHOLD_UNKNOWN The power failure threshold is unknown.
 */
SVCALL(SD_POWER_POF_THRESHOLD_SET, uint32_t, sd_power_pof_threshold_set(uint8_t threshold));

/**@brief Sets the power failure comparator threshold value for high voltage.
 *
 * @note: Power failure comparator threshold setting for high voltage mode (supply connected to
 *        VDDH only). This setting does not apply for normal voltage mode (supply connected to both
 *        VDD and VDDH).
 *
 * @param[in] threshold The power-fail threshold value to use, see @ref NRF_POWER_THRESHOLDVDDHS.
 *
 * @retval ::NRF_SUCCESS The power failure threshold was set.
 * @retval ::NRF_ERROR_SOC_POWER_POF_THRESHOLD_UNKNOWN The power failure threshold is unknown.
 */
SVCALL(SD_POWER_POF_THRESHOLDVDDH_SET, uint32_t, sd_power_pof_thresholdvddh_set(uint8_t threshold));

/**@brief Writes the NRF_POWER->RAM[index].POWERSET register.
 *
 * @param[in] index Contains the index in the NRF_POWER->RAM[index].POWERSET register to write to.
 * @param[in] ram_powerset Contains the word to write to the NRF_POWER->RAM[index].POWERSET register.
 *
 * @retval ::NRF_SUCCESS
 */
SVCALL(SD_POWER_RAM_POWER_SET, uint32_t, sd_power_ram_power_set(uint8_t index, uint32_t ram_powerset));

/**@brief Writes the NRF_POWER->RAM[index].POWERCLR register.
 *
 * @param[in] index Contains the index in the NRF_POWER->RAM[index].POWERCLR register to write to.
 * @param[in] ram_powerclr Contains the word to write to the NRF_POWER->RAM[index].POWERCLR register.
 *
 * @retval ::NRF_SUCCESS
 */
SVCALL(SD_POWER_RAM_POWER_CLR, uint32_t, sd_power_ram_power_clr(uint8_t index, uint32_t ram_powerclr));

/**@brief Get contents of NRF_POWER->RAM[index].POWER register, indicates power status of RAM[index] blocks.
 *
 * @param[in] index Contains the index in the NRF_POWER->RAM[index].POWER register to read from.
 * @param[out] p_ram_power Content of NRF_POWER->RAM[index].POWER register.
 *
 * @retval ::NRF_SUCCESS
 */
SVCALL(SD_POWER_RAM_POWER_GET, uint32_t, sd_power_ram_power_get(uint8_t index, uint32_t * p_ram_power));

/**@brief Set bits in the general purpose retention registers (NRF_POWER->GPREGRET*).
 *
 * @param[in] gpregret_id 0 for GPREGRET, 1 for GPREGRET2.
 * @param[in] gpregret_msk Bits to be set in the GPREGRET register.
 *
 * @retval ::NRF_SUCCESS
 */
SVCALL(SD_POWER_GPREGRET_SET, uint32_t, sd_power_gpregret_set(uint32_t gpregret_id, uint32_t gpregret_msk));

/**@brief Clear bits in the general purpose retention registers (NRF_POWER->GPREGRET*).
 *
 * @param[in] gpregret_id 0 for GPREGRET, 1 for GPREGRET2.
 * @param[in] gpregret_msk Bits to be clear in the GPREGRET register.
 *
 * @retval ::NRF_SUCCESS
 */
SVCALL(SD_POWER_GPREGRET_CLR, uint32_t, sd_power_gpregret_clr(uint32_t gpregret_id, uint32_t gpregret_msk));

/**@brief Get contents of the general purpose retention registers (NRF_POWER->GPREGRET*).
 *
 * @param[in] gpregret_id 0 for GPREGRET, 1 for GPREGRET2.
 * @param[out] p_gpregret Contents of the GPREGRET register.
 *
 * @retval ::NRF_SUCCESS
 */
SVCALL(SD_POWER_GPREGRET_GET, uint32_t, sd_power_gpregret_get(uint32_t gpregret_id, uint32_t * p_gpregret));

/**@brief Enable or disable the DC/DC regulator for the regulator stage 1 (REG1).
 *
 * @param[in] dcdc_mode The mode of the DCDC, see @ref NRF_POWER_DCDC_MODES.
 *
 * @retval ::NRF_SUCCESS
 * @retval ::NRF_ERROR_INVALID_PARAM The DCDC mode is invalid.
 */
SVCALL(SD_POWER_DCDC_MODE_SET, uint32_t, sd_power_dcdc_mode_set(uint8_t dcdc_mode));

/**@brief Enable or disable the DC/DC regulator for the regulator stage 0 (REG0).
 *
 * For more details on the REG0 stage, please see product specification.
 *
 * @param[in] dcdc_mode The mode of the DCDC0, see @ref NRF_POWER_DCDC_MODES.
 *
 * @retval ::NRF_SUCCESS
 * @retval ::NRF_ERROR_INVALID_PARAM The dcdc_mode is invalid.
 */
SVCALL(SD_POWER_DCDC0_MODE_SET, uint32_t, sd_power_dcdc0_mode_set(uint8_t dcdc_mode));

/**@brief Request the high frequency crystal oscillator.
 *
 * Will start the high frequency crystal oscillator, the startup time of the crystal varies
 * and the ::sd_clock_hfclk_is_running function can be polled to check if it has started.
 *
 * @see sd_clock_hfclk_is_running
 * @see sd_clock_hfclk_release
 *
 * @retval ::NRF_SUCCESS
 */
SVCALL(SD_CLOCK_HFCLK_REQUEST, uint32_t, sd_clock_hfclk_request(void));

/**@brief Releases the high frequency crystal oscillator.
 *
 * Will stop the high frequency crystal oscillator, this happens immediately.
 *
 * @see sd_clock_hfclk_is_running
 * @see sd_clock_hfclk_request
 *
 * @retval ::NRF_SUCCESS
 */
SVCALL(SD_CLOCK_HFCLK_RELEASE, uint32_t, sd_clock_hfclk_release(void));

/**@brief Checks if the high frequency crystal oscillator is running.
 *
 * @see sd_clock_hfclk_request
 * @see sd_clock_hfclk_release
 *
 * @param[out] p_is_running 1 if the external crystal oscillator is running, 0 if not.
 *
 * @retval ::NRF_SUCCESS
 */
SVCALL(SD_CLOCK_HFCLK_IS_RUNNING, uint32_t, sd_clock_hfclk_is_running(uint32_t * p_is_running));

/**@brief Waits for an application event.
 *
 * An application event is either an application interrupt or a pended interrupt when the interrupt
 * is disabled.
 *
 * When the application waits for an application event by calling this function, an interrupt that
 * is enabled will be taken immediately on pending since this function will wait in thread mode,
 * then the execution will return in the application's main thread.
 *
 * In order to wake up from disabled interrupts, the SEVONPEND flag has to be set in the Cortex-M
 * MCU's System Control Register (SCR), CMSIS_SCB. In that case, when a disabled interrupt gets
 * pended, this function will return to the application's main thread.
 *
 * @note The application must ensure that the pended flag is cleared using ::sd_nvic_ClearPendingIRQ
 *       in order to sleep using this function. This is only necessary for disabled interrupts, as
 *       the interrupt handler will clear the pending flag automatically for enabled interrupts.
 *
 * @note If an application interrupt has happened since the last time sd_app_evt_wait was
 *       called this function will return immediately and not go to sleep. This is to avoid race
 *       conditions that can occur when a flag is updated in the interrupt handler and processed
 *       in the main loop.
 *
 * @post An application interrupt has happened or a interrupt pending flag is set.
 *
 * @retval ::NRF_SUCCESS
 */
SVCALL(SD_APP_EVT_WAIT, uint32_t, sd_app_evt_wait(void));

/**@brief Get PPI channel enable register contents.
 *
 * @param[out] p_channel_enable The contents of the PPI CHEN register.
 *
 * @retval ::NRF_SUCCESS
 */
SVCALL(SD_PPI_CHANNEL_ENABLE_GET, uint32_t, sd_ppi_channel_enable_get(uint32_t * p_channel_enable));

/**@brief Set PPI channel enable register.
 *
 * @param[in] channel_enable_set_msk Mask containing the bits to set in the PPI CHEN register.
 *
 * @retval ::NRF_SUCCESS
 */
SVCALL(SD_PPI_CHANNEL_ENABLE_SET, uint32_t, sd_ppi_channel_enable_set(uint32_t channel_enable_set_msk));

/**@brief Clear PPI channel enable register.
 *
 * @param[in] channel_enable_clr_msk Mask containing the bits to clear in the PPI CHEN register.
 *
 * @retval ::NRF_SUCCESS
 */
SVCALL(SD_PPI_CHANNEL_ENABLE_CLR, uint32_t, sd_ppi_channel_enable_clr(uint32_t channel_enable_clr_msk));

/**@brief Assign endpoints to a PPI channel.
 *
 * @param[in] channel_num Number of the PPI channel to assign.
 * @param[in] evt_endpoint Event endpoint of the PPI channel.
 * @param[in] task_endpoint Task endpoint of the PPI channel.
 *
 * @retval ::NRF_ERROR_SOC_PPI_INVALID_CHANNEL The channel number is invalid.
 * @retval ::NRF_SUCCESS
 */
SVCALL(SD_PPI_CHANNEL_ASSIGN, uint32_t, sd_ppi_channel_assign(uint8_t channel_num, const volatile void *evt_endpoint, const volatile void *task_endpoint));

/**@brief Task to enable a channel group.
 *
 * @param[in] group_num Number of the channel group.
 *
 * @retval ::NRF_ERROR_SOC_PPI_INVALID_GROUP The group number is invalid
 * @retval ::NRF_SUCCESS
 */
SVCALL(SD_PPI_GROUP_TASK_ENABLE, uint32_t, sd_ppi_group_task_enable(uint8_t group_num));

/**@brief Task to disable a channel group.
 *
 * @param[in] group_num Number of the PPI group.
 *
 * @retval ::NRF_ERROR_SOC_PPI_INVALID_GROUP The group number is invalid.
 * @retval ::NRF_SUCCESS
 */
SVCALL(SD_PPI_GROUP_TASK_DISABLE, uint32_t, sd_ppi_group_task_disable(uint8_t group_num));

/**@brief Assign PPI channels to a channel group.
 *
 * @param[in] group_num Number of the channel group.
 * @param[in] channel_msk Mask of the channels to assign to the group.
 *
 * @retval ::NRF_ERROR_SOC_PPI_INVALID_GROUP The group number is invalid.
 * @retval ::NRF_SUCCESS
 */
SVCALL(SD_PPI_GROUP_ASSIGN, uint32_t, sd_ppi_group_assign(uint8_t group_num, uint32_t channel_msk));

/**@brief Gets the PPI channels of a channel group.
 *
 * @param[in]   group_num Number of the channel group.
 * @param[out]  p_channel_msk Mask of the channels assigned to the group.
 *
 * @retval ::NRF_ERROR_SOC_PPI_INVALID_GROUP The group number is invalid.
 * @retval ::NRF_SUCCESS
 */
SVCALL(SD_PPI_GROUP_GET, uint32_t, sd_ppi_group_get(uint8_t group_num, uint32_t * p_channel_msk));

/**@brief Configures the Radio Notification signal.
 *
 * @note
 *      - The notification signal latency depends on the interrupt priority settings of SWI used
 *        for notification signal.
 *      - To ensure that the radio notification signal behaves in a consistent way, the radio
 *        notifications must be configured when there is no protocol stack or other SoftDevice
 *        activity in progress. It is recommended that the radio notification signal is
 *        configured directly after the SoftDevice has been enabled.
 *      - In the period between the ACTIVE signal and the start of the Radio Event, the SoftDevice
 *        will interrupt the application to do Radio Event preparation.
 *      - Using the Radio Notification feature may limit the bandwidth, as the SoftDevice may have
 *        to shorten the connection events to have time for the Radio Notification signals.
 *
 * @param[in]  type      Type of notification signal, see @ref NRF_RADIO_NOTIFICATION_TYPES.
 *                       @ref NRF_RADIO_NOTIFICATION_TYPE_NONE shall be used to turn off radio
 *                       notification. Using @ref NRF_RADIO_NOTIFICATION_DISTANCE_NONE is
 *                       recommended (but not required) to be used with
 *                       @ref NRF_RADIO_NOTIFICATION_TYPE_NONE.
 *
 * @param[in]  distance  Distance between the notification signal and start of radio activity, see @ref NRF_RADIO_NOTIFICATION_DISTANCES.
 *                       This parameter is ignored when @ref NRF_RADIO_NOTIFICATION_TYPE_NONE or
 *                       @ref NRF_RADIO_NOTIFICATION_TYPE_INT_ON_INACTIVE is used.
 *
 * @retval ::NRF_ERROR_INVALID_PARAM The group number is invalid.
 * @retval ::NRF_ERROR_INVALID_STATE A protocol stack or other SoftDevice is running. Stop all
 *                                   running activities and retry.
 * @retval ::NRF_SUCCESS
 */
SVCALL(SD_RADIO_NOTIFICATION_CFG_SET, uint32_t, sd_radio_notification_cfg_set(uint8_t type, uint8_t distance));

/**@brief Encrypts a block according to the specified parameters.
 *
 * 128-bit AES encryption.
 *
 * @note:
 *    - The application may set the SEVONPEND bit in the SCR to 1 to make the SoftDevice sleep while
 *      the ECB is running. The SEVONPEND bit should only be cleared (set to 0) from application
 *      main or low interrupt level.
 *
 * @param[in, out] p_ecb_data Pointer to the ECB parameters' struct (two input
 *                            parameters and one output parameter).
 *
 * @retval ::NRF_SUCCESS
 */
SVCALL(SD_ECB_BLOCK_ENCRYPT, uint32_t, sd_ecb_block_encrypt(nrf_ecb_hal_data_t * p_ecb_data));

/**@brief Encrypts multiple data blocks provided as an array of data block structures.
 *
 * @details: Performs 128-bit AES encryption on multiple data blocks
 *
 * @note:
 *    - The application may set the SEVONPEND bit in the SCR to 1 to make the SoftDevice sleep while
 *      the ECB is running. The SEVONPEND bit should only be cleared (set to 0) from application
 *      main or low interrupt level.
 *
 * @param[in]     block_count     Count of blocks in the p_data_blocks array.
 * @param[in,out] p_data_blocks   Pointer to the first entry in a contiguous array of
 *                                @ref nrf_ecb_hal_data_block_t structures.
 *
 * @retval ::NRF_SUCCESS
 */
SVCALL(SD_ECB_BLOCKS_ENCRYPT, uint32_t, sd_ecb_blocks_encrypt(uint8_t block_count, nrf_ecb_hal_data_block_t * p_data_blocks));

/**@brief Gets any pending events generated by the SoC API.
 *
 * The application should keep calling this function to get events, until ::NRF_ERROR_NOT_FOUND is returned.
 *
 * @param[out] p_evt_id Set to one of the values in @ref NRF_SOC_EVTS, if any events are pending.
 *
 * @retval ::NRF_SUCCESS An event was pending. The event id is written in the p_evt_id parameter.
 * @retval ::NRF_ERROR_NOT_FOUND No pending events.
 */
SVCALL(SD_EVT_GET, uint32_t, sd_evt_get(uint32_t * p_evt_id));

/**@brief Get the temperature measured on the chip
 *
 * This function will block until the temperature measurement is done.
 * It takes around 50 us from call to return.
 *
 * @param[out] p_temp Result of temperature measurement. Die temperature in 0.25 degrees Celsius.
 *
 * @retval ::NRF_SUCCESS A temperature measurement was done, and the temperature was written to temp
 */
SVCALL(SD_TEMP_GET, uint32_t, sd_temp_get(int32_t * p_temp));

/**@brief Flash Write
*
* Commands to write a buffer to flash
*
* If the SoftDevice is enabled:
*  This call initiates the flash access command, and its completion will be communicated to the
*  application with exactly one of the following events:
*      - @ref NRF_EVT_FLASH_OPERATION_SUCCESS - The command was successfully completed.
*      - @ref NRF_EVT_FLASH_OPERATION_ERROR   - The command could not be started.
*
* If the SoftDevice is not enabled no event will be generated, and this call will return @ref NRF_SUCCESS when the
 * write has been completed
*
* @note
*      - This call takes control over the radio and the CPU during flash erase and write to make sure that
*        they will not interfere with the flash access. This means that all interrupts will be blocked
*        for a predictable time (depending on the NVMC specification in the device's Product Specification
*        and the command parameters).
*      - The data in the p_src buffer should not be modified before the @ref NRF_EVT_FLASH_OPERATION_SUCCESS
*        or the @ref NRF_EVT_FLASH_OPERATION_ERROR have been received if the SoftDevice is enabled.
*      - This call will make the SoftDevice trigger a hardfault when the page is written, if it is
*        protected.
*
*
* @param[in]  p_dst Pointer to start of flash location to be written.
* @param[in]  p_src Pointer to buffer with data to be written.
* @param[in]  size  Number of 32-bit words to write. Maximum size is the number of words in one
*                   flash page. See the device's Product Specification for details.
*
* @retval ::NRF_ERROR_INVALID_ADDR   Tried to write to a non existing flash address, or p_dst or p_src was unaligned.
* @retval ::NRF_ERROR_BUSY           The previous command has not yet completed.
* @retval ::NRF_ERROR_INVALID_LENGTH Size was 0, or higher than the maximum allowed size.
* @retval ::NRF_ERROR_FORBIDDEN      Tried to write to an address outside the application flash area.
* @retval ::NRF_SUCCESS              The command was accepted.
*/
SVCALL(SD_FLASH_WRITE, uint32_t, sd_flash_write(uint32_t * p_dst, uint32_t const *p_src, uint32_t size));


/**@brief Flash Erase page
*
* Commands to erase a flash page
* If the SoftDevice is enabled:
*  This call initiates the flash access command, and its completion will be communicated to the
*  application with exactly one of the following events:
*      - @ref NRF_EVT_FLASH_OPERATION_SUCCESS - The command was successfully completed.
*      - @ref NRF_EVT_FLASH_OPERATION_ERROR   - The command could not be started.
*
* If the SoftDevice is not enabled no event will be generated, and this call will return @ref NRF_SUCCESS when the
* erase has been completed
*
* @note
*      - This call takes control over the radio and the CPU during flash erase and write to make sure that
*        they will not interfere with the flash access. This means that all interrupts will be blocked
*        for a predictable time (depending on the NVMC specification in the device's Product Specification
*        and the command parameters).
*      - This call will make the SoftDevice trigger a hardfault when the page is erased, if it is
*        protected.
*
*
* @param[in]  page_number           Page number of the page to erase
*
* @retval ::NRF_ERROR_INTERNAL      If a new session could not be opened due to an internal error.
* @retval ::NRF_ERROR_INVALID_ADDR  Tried to erase to a non existing flash page.
* @retval ::NRF_ERROR_BUSY          The previous command has not yet completed.
* @retval ::NRF_ERROR_FORBIDDEN     Tried to erase a page outside the application flash area.
* @retval ::NRF_SUCCESS             The command was accepted.
*/
SVCALL(SD_FLASH_PAGE_ERASE, uint32_t, sd_flash_page_erase(uint32_t page_number));



/**@brief Opens a session for radio timeslot requests.
 *
 * @note Only one session can be open at a time.
 * @note p_radio_signal_callback(@ref NRF_RADIO_CALLBACK_SIGNAL_TYPE_START) will be called when the radio timeslot
 *       starts. From this point the NRF_RADIO and NRF_TIMER0 peripherals can be freely accessed
 *       by the application.
 * @note p_radio_signal_callback(@ref NRF_RADIO_CALLBACK_SIGNAL_TYPE_TIMER0) is called whenever the NRF_TIMER0
 *       interrupt occurs.
 * @note p_radio_signal_callback(@ref NRF_RADIO_CALLBACK_SIGNAL_TYPE_RADIO) is called whenever the NRF_RADIO
 *       interrupt occurs.
 * @note p_radio_signal_callback() will be called at ARM interrupt priority level 0. This
 *       implies that none of the sd_* API calls can be used from p_radio_signal_callback().
 *
 * @param[in] p_radio_signal_callback The signal callback.
 *
 * @retval ::NRF_ERROR_INVALID_ADDR p_radio_signal_callback is an invalid function pointer.
 * @retval ::NRF_ERROR_BUSY If session cannot be opened.
 * @retval ::NRF_ERROR_INTERNAL If a new session could not be opened due to an internal error.
 * @retval ::NRF_SUCCESS Otherwise.
 */
SVCALL(SD_RADIO_SESSION_OPEN, uint32_t, sd_radio_session_open(nrf_radio_signal_callback_t p_radio_signal_callback));

/**@brief Closes a session for radio timeslot requests.
 *
 * @note Any current radio timeslot will be finished before the session is closed.
 * @note If a radio timeslot is scheduled when the session is closed, it will be canceled.
 * @note The application cannot consider the session closed until the @ref NRF_EVT_RADIO_SESSION_CLOSED
 *       event is received.
 *
 * @retval ::NRF_ERROR_FORBIDDEN If session not opened.
 * @retval ::NRF_ERROR_BUSY If session is currently being closed.
 * @retval ::NRF_SUCCESS Otherwise.
 */
SVCALL(SD_RADIO_SESSION_CLOSE, uint32_t, sd_radio_session_close(void));

/**@brief Requests a radio timeslot.
 *
 * @note The request type is determined by p_request->request_type, and can be one of @ref NRF_RADIO_REQ_TYPE_EARLIEST
 *       and @ref NRF_RADIO_REQ_TYPE_NORMAL. The first request in a session must always be of type @ref NRF_RADIO_REQ_TYPE_EARLIEST.
 * @note For a normal request (@ref NRF_RADIO_REQ_TYPE_NORMAL), the start time of a radio timeslot is specified by
 *       p_request->distance_us and is given relative to the start of the previous timeslot.
 * @note A too small p_request->distance_us will lead to a @ref NRF_EVT_RADIO_BLOCKED event.
 * @note Timeslots scheduled too close will lead to a @ref NRF_EVT_RADIO_BLOCKED event.
 * @note See the SoftDevice Specification for more on radio timeslot scheduling, distances and lengths.
 * @note If an opportunity for the first radio timeslot is not found before 100 ms after the call to this
 *       function, it is not scheduled, and instead a @ref NRF_EVT_RADIO_BLOCKED event is sent.
 *       The application may then try to schedule the first radio timeslot again.
 * @note Successful requests will result in nrf_radio_signal_callback_t(@ref NRF_RADIO_CALLBACK_SIGNAL_TYPE_START).
 *       Unsuccessful requests will result in a @ref NRF_EVT_RADIO_BLOCKED event, see @ref NRF_SOC_EVTS.
 * @note The jitter in the start time of the radio timeslots is +/- @ref NRF_RADIO_START_JITTER_US us.
 * @note The nrf_radio_signal_callback_t(@ref NRF_RADIO_CALLBACK_SIGNAL_TYPE_START) call has a latency relative to the
 *       specified radio timeslot start, but this does not affect the actual start time of the timeslot.
 * @note NRF_TIMER0 is reset at the start of the radio timeslot, and is clocked at 1MHz from the high frequency
 *       (16 MHz) clock source. If p_request->hfclk_force_xtal is true, the high frequency clock is
 *       guaranteed to be clocked from the external crystal.
 * @note The SoftDevice will neither access the NRF_RADIO peripheral nor the NRF_TIMER0 peripheral
 *       during the radio timeslot.
 *
 * @param[in] p_request Pointer to the request parameters.
 *
 * @retval ::NRF_ERROR_FORBIDDEN If session not opened or the session is not IDLE.
 * @retval ::NRF_ERROR_INVALID_ADDR If the p_request pointer is invalid.
 * @retval ::NRF_ERROR_INVALID_PARAM If the parameters of p_request are not valid.
 * @retval ::NRF_SUCCESS Otherwise.
 */
SVCALL(SD_RADIO_REQUEST, uint32_t, sd_radio_request(nrf_radio_request_t const *p_request));

/**@brief Write register protected by the SoftDevice
 *
 * This function writes to a register that is write-protected by the SoftDevice. Please refer to your
 * SoftDevice Specification for more details about which registers that are protected by SoftDevice.
 * This function can write to the following protected peripheral:
 *  - ACL
 *
 * @note Protected registers may be read directly.
 * @note Register that are write-once will return @ref NRF_SUCCESS on second set, even the value in
 *       the register has not changed. See the Product Specification for more details about register
 *       properties.
 *
 * @param[in]  p_register Pointer to register to be written.
 * @param[in]  value Value to be written to the register.
 *
 * @retval ::NRF_ERROR_INVALID_ADDR This function can not write to the reguested register.
 * @retval ::NRF_SUCCESS Value successfully written to register.
 *
 */
SVCALL(SD_PROTECTED_REGISTER_WRITE, uint32_t, sd_protected_register_write(volatile uint32_t * p_register, uint32_t value));

/**@} */

#ifdef __cplusplus
}
#endif
#endif // NRF_SOC_H__

/**@} */
