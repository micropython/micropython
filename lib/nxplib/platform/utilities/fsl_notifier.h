/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_NOTIFIER_H_
#define _FSL_NOTIFIER_H_

#include "fsl_common.h"
/*!
 * @addtogroup notifier
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*!
 * @brief Notifier error codes.
 *
 * Used as return value of Notifier functions.
 */
enum _notifier_status
{
    kStatus_NOTIFIER_ErrorNotificationBefore =
        MAKE_STATUS(kStatusGroup_NOTIFIER, 0), /*!< An error occurs during send "BEFORE" notification. */
    kStatus_NOTIFIER_ErrorNotificationAfter =
        MAKE_STATUS(kStatusGroup_NOTIFIER, 1), /*!< An error occurs during send "AFTER" notification. */
};

/*!
 * @brief Notifier policies.
 *
 * Defines whether the user function execution is forced or not.
 * For kNOTIFIER_PolicyForcible, the user function is executed regardless of the callback results,
 * while kNOTIFIER_PolicyAgreement policy is used to exit NOTIFIER_SwitchConfig()
 * when any of the callbacks returns error code.
 * See also NOTIFIER_SwitchConfig() description.
 */
typedef enum _notifier_policy
{
    kNOTIFIER_PolicyAgreement, /*!< NOTIFIER_SwitchConfig() method is exited when any of the callbacks returns error
                                      code. */
    kNOTIFIER_PolicyForcible,  /*!< The user function is executed regardless of the results. */
} notifier_policy_t;

/*! @brief Notification type. Used to notify registered callbacks */
typedef enum _notifier_notification_type
{
    kNOTIFIER_NotifyRecover = 0x00U, /*!< Notify IP to recover to previous work state. */
    kNOTIFIER_NotifyBefore  = 0x01U, /*!< Notify IP that configuration setting is going to change. */
    kNOTIFIER_NotifyAfter   = 0x02U, /*!< Notify IP that configuration setting has been changed. */
} notifier_notification_type_t;

/*!
 * @brief The callback type, which indicates kinds of notification the callback handles.
 *
 * Used in the callback configuration structure (notifier_callback_config_t)
 * to specify when the registered callback is called during configuration switch initiated by the
 * NOTIFIER_SwitchConfig().
 * Callback can be invoked in following situations.
 *  - Before the configuration switch (Callback return value can affect NOTIFIER_SwitchConfig()
 *    execution. See the NOTIFIER_SwitchConfig() and notifier_policy_t documentation).
 *  - After an unsuccessful attempt to switch configuration
 *  - After a successful configuration switch
 */
typedef enum _notifier_callback_type
{
    kNOTIFIER_CallbackBefore      = 0x01U, /*!< Callback handles BEFORE notification. */
    kNOTIFIER_CallbackAfter       = 0x02U, /*!< Callback handles AFTER notification. */
    kNOTIFIER_CallbackBeforeAfter = 0x03U, /*!< Callback handles BEFORE and AFTER notification. */
} notifier_callback_type_t;

/*! @brief Notifier user configuration type.
 *
 * Reference of the user defined configuration is stored in an array; the notifier switches between these configurations
 * based on this array.
 */
typedef void notifier_user_config_t;

/*! @brief Notifier user function prototype
 * Use this function to execute specific operations in configuration switch.
 * Before and after this function execution, different notification is sent to registered callbacks.
 * If this function returns any error code, NOTIFIER_SwitchConfig() exits.
 *
 * @param targetConfig target Configuration.
 * @param userData Refers to other specific data passed to user function.
 * @return An error code or kStatus_Success.
 */
typedef status_t (*notifier_user_function_t)(notifier_user_config_t *targetConfig, void *userData);

/*! @brief notification block passed to the registered callback function. */
typedef struct _notifier_notification_block
{
    notifier_user_config_t *targetConfig;    /*!< Pointer to target configuration. */
    notifier_policy_t policy;                /*!< Configure transition policy. */
    notifier_notification_type_t notifyType; /*!< Configure notification type. */
} notifier_notification_block_t;

/*!
 * @brief Callback prototype.
 *
 * Declaration of a callback. It is common for registered callbacks.
 * Reference to function of this type is part of the notifier_callback_config_t callback configuration structure.
 * Depending on callback type, function of this prototype is called (see NOTIFIER_SwitchConfig())
 * before configuration switch, after it or in both use cases to notify about
 * the switch progress (see notifier_callback_type_t). When called, the type of the notification
 * is passed as a parameter along with the reference to the target configuration structure (see
 * notifier_notification_block_t) and any data passed during the callback registration. When notified before the
 * configuration switch, depending on the configuration switch policy (see notifier_policy_t), the callback may deny the
 * execution of the user function by returning an error code different than kStatus_Success (see
 * NOTIFIER_SwitchConfig()).
 *
 * @param notify Notification block.
 * @param data Callback data. Refers to the data passed during callback registration. Intended to
 *  pass any driver or application data such as internal state information.
 * @return An error code or kStatus_Success.
 */
typedef status_t (*notifier_callback_t)(notifier_notification_block_t *notify, void *data);

/*!
 * @brief Callback configuration structure.
 *
 * This structure holds the configuration of callbacks.
 * Callbacks of this type are expected to be statically allocated.
 * This structure contains the following application-defined data.
 *  callback - pointer to the callback function
 *  callbackType - specifies when the callback is called
 *  callbackData - pointer to the data passed to the callback.
 */
typedef struct _notifier_callback_config
{
    notifier_callback_t callback;          /*!< Pointer to the callback function. */
    notifier_callback_type_t callbackType; /*!< Callback type. */
    void *callbackData;                    /*!< Pointer to the data passed to the callback. */
} notifier_callback_config_t;

/*!
 * @brief Notifier handle structure.
 *
 * Notifier handle structure. Contains data necessary for the Notifier proper function.
 * Stores references to registered configurations, callbacks, information about their numbers,
 * user function, user data, and other internal data.
 * NOTIFIER_CreateHandle() must be called to initialize this handle.
 */
typedef struct _notifier_handle
{
    notifier_user_config_t **configsTable;      /*!< Pointer to configure table. */
    uint8_t configsNumber;                      /*!< Number of configurations. */
    notifier_callback_config_t *callbacksTable; /*!< Pointer to callback table. */
    uint8_t callbacksNumber;                    /*!< Maximum number of callback configurations. */
    uint8_t errorCallbackIndex;                 /*!< Index of callback returns error. */
    uint8_t currentConfigIndex;                 /*!< Index of current configuration.  */
    notifier_user_function_t userFunction;      /*!< User function. */
    void *userData;                             /*!< User data passed to user function. */
} notifier_handle_t;

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @brief Creates a Notifier handle.
 *
 * @param notifierHandle A pointer to the notifier handle.
 * @param configs A pointer to an array with references to all configurations which is handled by the Notifier.
 * @param configsNumber Number of configurations. Size of the configuration array.
 * @param callbacks A pointer to an array of callback configurations.
 *  If there are no callbacks to register during Notifier initialization, use NULL value.
 * @param callbacksNumber Number of registered callbacks. Size of the callbacks array.
 * @param userFunction User function.
 * @param userData User data passed to user function.
 * @return An error Code or kStatus_Success.
 */
status_t NOTIFIER_CreateHandle(notifier_handle_t *notifierHandle,
                               notifier_user_config_t **configs,
                               uint8_t configsNumber,
                               notifier_callback_config_t *callbacks,
                               uint8_t callbacksNumber,
                               notifier_user_function_t userFunction,
                               void *userData);

/*!
 * @brief Switches the configuration according to a pre-defined structure.
 *
 * This function sets the system to the target configuration. Before transition,
 * the Notifier sends notifications to all callbacks registered to the callback table.
 * Callbacks are invoked in the following order: All registered callbacks are notified
 * ordered by index in the callbacks array. The same order is used for before and after switch notifications.
 * The notifications before the configuration switch can be used to obtain confirmation about
 * the change from registered callbacks. If any registered callback denies the
 * configuration change, further execution of this function depends on the notifier policy: the
 * configuration change is either forced (kNOTIFIER_PolicyForcible) or exited (kNOTIFIER_PolicyAgreement).
 * When configuration change is forced, the result of the before switch notifications are ignored. If an
 * agreement is required, if any callback returns an error code, further notifications
 * before switch notifications are cancelled and all already notified callbacks are re-invoked.
 * The index of the callback which returned error code during pre-switch notifications is stored
 * (any error codes during callbacks re-invocation are ignored) and NOTIFIER_GetErrorCallback() can be used to get it.
 * Regardless of the policies, if any callback returns an error code, an error code indicating in which phase
 * the error occurred is returned when NOTIFIER_SwitchConfig() exits.
 * @param notifierHandle pointer to notifier handle
 * @param configIndex Index of the target configuration.
 * @param policy            Transaction policy, kNOTIFIER_PolicyAgreement or kNOTIFIER_PolicyForcible.
 *
 * @return An error code or kStatus_Success.
 *
 */
status_t NOTIFIER_SwitchConfig(notifier_handle_t *notifierHandle, uint8_t configIndex, notifier_policy_t policy);

/*!
 * @brief This function returns the last failed notification callback.
 *
 * This function returns an index of the last callback that failed during the configuration switch while
 * the last NOTIFIER_SwitchConfig() was called. If the last NOTIFIER_SwitchConfig() call ended successfully
 * value equal to callbacks number is returned. The returned value represents an index in the array of
 * static call-backs.
 *
 * @param notifierHandle Pointer to the notifier handle
 * @return Callback Index of the last failed callback or value equal to callbacks count.
 */
uint8_t NOTIFIER_GetErrorCallbackIndex(notifier_handle_t *notifierHandle);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

/*! @}*/

#endif /* _FSL_NOTIFIER_H_ */
