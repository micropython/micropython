/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_notifier.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * brief Creates a Notifier handle.
 *
 * param notifierHandle A pointer to the notifier handle.
 * param configs A pointer to an array with references to all configurations which is handled by the Notifier.
 * param configsNumber Number of configurations. Size of the configuration array.
 * param callbacks A pointer to an array of callback configurations.
 *  If there are no callbacks to register during Notifier initialization, use NULL value.
 * param callbacksNumber Number of registered callbacks. Size of the callbacks array.
 * param userFunction User function.
 * param userData User data passed to user function.
 * return An error Code or kStatus_Success.
 */
status_t NOTIFIER_CreateHandle(notifier_handle_t *notifierHandle,
                               notifier_user_config_t **configs,
                               uint8_t configsNumber,
                               notifier_callback_config_t *callbacks,
                               uint8_t callbacksNumber,
                               notifier_user_function_t userFunction,
                               void *userData)
{
    /* Check input parameter - at least one configuration is required and userFunction must exist */
    if ((configs == NULL) || (configsNumber == 0U) || (userFunction == NULL))
    {
        return kStatus_Fail;
    }
    /* Initialize handle structure */
    memset(notifierHandle, 0, sizeof(notifier_handle_t));
    /* Store references to user-defined configurations */
    notifierHandle->configsTable  = configs;
    notifierHandle->configsNumber = configsNumber;
    /* Store references to user-defined callback configurations */
    if (callbacks != NULL)
    {
        notifierHandle->callbacksTable  = callbacks;
        notifierHandle->callbacksNumber = callbacksNumber;
        /* If all callbacks return success, then the errorCallbackIndex is callbacksNumber */
        notifierHandle->errorCallbackIndex = callbacksNumber;
    }
    notifierHandle->userFunction = userFunction;
    notifierHandle->userData     = userData;

    return kStatus_Success;
}

/*!
 * brief Switches the configuration according to a pre-defined structure.
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
 * param notifierHandle pointer to notifier handle
 * param configIndex Index of the target configuration.
 * param policy            Transaction policy, kNOTIFIER_PolicyAgreement or kNOTIFIER_PolicyForcible.
 *
 * return An error code or kStatus_Success.
 *
 */
status_t NOTIFIER_SwitchConfig(notifier_handle_t *notifierHandle, uint8_t configIndex, notifier_policy_t policy)
{
    uint8_t currentStaticCallback = 0U;              /* Index to array of statically registered call-backs */
    status_t returnCode           = kStatus_Success; /* Function return */

    notifier_notification_block_t notifyBlock;  /*  Callback notification block */
    notifier_callback_config_t *callbackConfig; /* Pointer to callback configuration */

    /* Set errorcallbackindex as callbacksNumber, which means no callback error now */
    notifierHandle->errorCallbackIndex = notifierHandle->callbacksNumber;

    /* Requested configuration availability check */
    if (configIndex >= notifierHandle->configsNumber)
    {
        return kStatus_OutOfRange;
    }

    /* Initialization of local variables from the Notifier handle structure */

    notifyBlock.policy       = policy;
    notifyBlock.targetConfig = notifierHandle->configsTable[configIndex];
    notifyBlock.notifyType   = kNOTIFIER_NotifyBefore;

    /* From all statically registered call-backs... */
    for (currentStaticCallback = 0U; currentStaticCallback < notifierHandle->callbacksNumber; currentStaticCallback++)
    {
        callbackConfig = &(notifierHandle->callbacksTable[currentStaticCallback]);
        /* ...notify only those which asked to be called before the configuration switch */
        if (((uint32_t)callbackConfig->callbackType) & kNOTIFIER_CallbackBefore)
        {
            /* In case that call-back returned error code mark it, store the call-back handle and eventually cancel
             * the configuration switch */
            if (callbackConfig->callback(&notifyBlock, callbackConfig->callbackData) != kStatus_Success)
            {
                returnCode                         = kStatus_NOTIFIER_ErrorNotificationBefore;
                notifierHandle->errorCallbackIndex = currentStaticCallback;
                /* If not forcing configuration switch, call all already notified call-backs to revert their state
                 * as the switch is canceled */
                if (policy != kNOTIFIER_PolicyForcible)
                {
                    break;
                }
            }
        }
    }

    /* Set configuration */

    /* In case that any call-back returned error code and  policy doesn't force the configuration set, go to after
     * switch call-backs */
    if ((policy == kNOTIFIER_PolicyForcible) || (returnCode == kStatus_Success))
    {
        returnCode = notifierHandle->userFunction(notifierHandle->configsTable[configIndex], notifierHandle->userData);
        if (returnCode != kStatus_Success)
        {
            return returnCode;
        }
        /* Update current configuration index */
        notifierHandle->currentConfigIndex = configIndex;
        notifyBlock.notifyType             = kNOTIFIER_NotifyAfter;
        /* From all statically registered call-backs... */
        for (currentStaticCallback = 0U; currentStaticCallback < notifierHandle->callbacksNumber;
             currentStaticCallback++)
        {
            callbackConfig = &(notifierHandle->callbacksTable[currentStaticCallback]);
            /* ...notify only those which asked to be called after the configuration switch */
            if (((uint32_t)callbackConfig->callbackType) & kNOTIFIER_CallbackAfter)
            {
                /* In case that call-back returned error code mark it and store the call-back handle */
                if (callbackConfig->callback(&notifyBlock, callbackConfig->callbackData) != kStatus_Success)
                {
                    returnCode                         = kStatus_NOTIFIER_ErrorNotificationAfter;
                    notifierHandle->errorCallbackIndex = currentStaticCallback;
                    if (policy != kNOTIFIER_PolicyForcible)
                    {
                        break;
                    }
                }
            }
        }
    }
    else
    {
        /* End of unsuccessful switch */
        notifyBlock.notifyType = kNOTIFIER_NotifyRecover;
        while (currentStaticCallback--)
        {
            callbackConfig = &(notifierHandle->callbacksTable[currentStaticCallback]);
            if (((uint32_t)callbackConfig->callbackType) & kNOTIFIER_CallbackBefore)
            {
                callbackConfig->callback(&notifyBlock, callbackConfig->callbackData);
            }
        }
    }

    return returnCode;
}

/*!
 * brief This function returns the last failed notification callback.
 *
 * This function returns an index of the last callback that failed during the configuration switch while
 * the last NOTIFIER_SwitchConfig() was called. If the last NOTIFIER_SwitchConfig() call ended successfully
 * value equal to callbacks number is returned. The returned value represents an index in the array of
 * static call-backs.
 *
 * param notifierHandle Pointer to the notifier handle
 * return Callback Index of the last failed callback or value equal to callbacks count.
 */
uint8_t NOTIFIER_GetErrorCallbackIndex(notifier_handle_t *notifierHandle)
{
    return notifierHandle->errorCallbackIndex;
}
