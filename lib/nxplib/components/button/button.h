/*
 * Copyright 2018-2019 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __BUTTON_H__
#define __BUTTON_H__

/*!
 * @addtogroup Button
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief Definition of button handle size as HAL_GPIO_HANDLE_SIZE + button dedicated size. */
#define BUTTON_HANDLE_SIZE (16U + 24U)

/*! @brief Definition of button timer interval,unit is ms. */
#define BUTTON_TIMER_INTERVAL (25U)

/*! @brief Definition of button short press threshold,unit is ms. */
#define BUTTON_SHORT_PRESS_THRESHOLD (200U)

/*! @brief Definition of button long press threshold,unit is ms. */
#define BUTTON_LONG_PRESS_THRESHOLD (500U)

/*! @brief Definition of button double click threshold,unit is ms. */
#define BUTTON_DOUBLE_CLICK_THRESHOLD (200U)

/*! @brief Definition to determine whether use common task. */
#define BUTTON_USE_COMMON_TASK (1U)

/*! @brief Definition of button task priority. */
#define BUTTON_TASK_PRIORITY (2U)

/*! @brief Definition of button task stack size. */
#define BUTTON_TASK_STACK_SIZE (1000U)

/*! @brief Definition of button event. */
#define BUTTON_EVENT_BUTTON (1U)

/*! @brief The handle of button */
typedef void *button_handle_t;

/*! @brief The status type of button */
typedef enum _button_status
{
    kStatus_BUTTON_Success    = kStatus_Success,                     /*!< Success */
    kStatus_BUTTON_Error      = MAKE_STATUS(kStatusGroup_BUTTON, 1), /*!< Failed */
    kStatus_BUTTON_LackSource = MAKE_STATUS(kStatusGroup_BUTTON, 2), /*!< Lack of sources */
} button_status_t;

/*! @brief The event type of button */
typedef enum _button_event
{
    kBUTTON_EventOneClick = 0x01U, /*!< One click with short time, the duration of key down and key up is less than
                                      #BUTTON_SHORT_PRESS_THRESHOLD. */
    kBUTTON_EventDoubleClick,      /*!< Double click with short time, the duration of key down and key up is less than
                                      #BUTTON_SHORT_PRESS_THRESHOLD.      And the duration of the two button actions does not
                                      exceed #BUTTON_DOUBLE_CLICK_THRESHOLD. */
    kBUTTON_EventShortPress,       /*!< Press with short time, the duration of key down and key up is no less than
                                      #BUTTON_SHORT_PRESS_THRESHOLD       and less than #BUTTON_LONG_PRESS_THRESHOLD. */
    kBUTTON_EventLongPress,        /*!< Press with long time, the duration of key down and key up is no less than
                                      #BUTTON_LONG_PRESS_THRESHOLD. */
    kBUTTON_EventError,            /*!< Error event if the button actions cannot be identified. */
} button_event_t;

/*! @brief The callback message struct of button */
typedef struct _button_callback_message_struct
{
    button_event_t event;
} button_callback_message_t;

/*! @brief The callback function of button */
typedef button_status_t (*button_callback_t)(void *buttonHandle,
                                             button_callback_message_t *message,
                                             void *callbackParam);

/*! @brief The button gpio config structure */
typedef struct _button_gpio_config
{
    uint8_t port;            /*!< GPIO Port */
    uint8_t pin;             /*!< GPIO Pin */
    uint8_t pinStateDefault; /*!< GPIO Pin voltage when button is not pressed (0 - low level, 1 - high level)*/
} button_gpio_config_t;

/*! @brief The button config structure */
typedef struct _button_config
{
    button_gpio_config_t gpio;
} button_config_t;

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*!
 * @name Initialization
 * @{
 */

/*!
 * @brief Initializes a button with the button handle and the user configuration structure.
 *
 * This function configures the button with user-defined settings. The user can configure the configuration
 * structure. The parameter buttonHandle is a pointer to point to a memory space of size #BUTTON_HANDLE_SIZE allocated
 * by the caller.
 *
 * Example below shows how to use this API to configure the button.
 *  @code
 *   uint8_t s_buttonHandleBuffer[BUTTON_HANDLE_SIZE];
 *   button_handle_t s_buttonHandle = &s_buttonHandleBuffer[0];
 *   button_config_t buttonConfig;
 *   buttonConfig.gpio.port = 0;
 *   buttonConfig.gpio.pin = 1;
 *   buttonConfig.gpio.pinStateDefault = 0;
 *   BUTTON_Init(s_buttonHandle, &buttonConfig);
 *  @endcode
 *
 * @param buttonHandle Pointer to point to a memory space of size #BUTTON_HANDLE_SIZE allocated by the caller.
 * @param buttonConfig Pointer to user-defined configuration structure.
 * @return Indicates whether initialization was successful or not.
 * @retval kStatus_BUTTON_Error An error occurred.
 * @retval kStatus_BUTTON_Success Button initialization succeed.
 */
button_status_t BUTTON_Init(button_handle_t buttonHandle, button_config_t *buttonConfig);

/*! @}*/

/*!
 * @name Install callback
 * @{
 */

/*!
 * @brief Installs a callback and callback parameter.
 *
 * This function is used to install the callback and callback parameter for button module.
 * Once the button is pressed, the button driver will identify the behavior and notify the
 * upper layer with the button event by the installed callback function. Currently, the Button
 * supports the three types of event, click, double click and long press. Detail information refer
 * to #button_event_t.
 *
 * @param buttonHandle Button handle pointer.
 * @param callback The callback function.
 * @param callbackParam The parameter of the callback function.
 * @return Indicates whether callback install was successful or not.
 * @retval kStatus_BUTTON_Success Successfully install the callback.
 */
button_status_t BUTTON_InstallCallback(button_handle_t buttonHandle, button_callback_t callback, void *callbackParam);

/*! @}*/

/*!
 * @brief Deinitializes a button instance.
 *
 * This function deinitializes the button instance.
 *
 * @param buttonHandle button handle pointer.
 * @retval kStatus_BUTTON_Success button de-initialization succeed.
 */
button_status_t BUTTON_Deinit(button_handle_t buttonHandle);

/*!
 * @brief Enables or disables the button wake-up feature.
 *
 * This function enables or disables the button wake-up feature.
 *
 * @param buttonHandle button handle pointer.
 * @param enable enable or disable (0 - disable, 1 - enable).
 * @retval kStatus_BUTTON_Error An error occurred.
 * @retval kStatus_BUTTON_Success Set successfully.
 */
button_status_t BUTTON_WakeUpSetting(button_handle_t buttonHandle, uint8_t enable);

/*!
 * @brief Prepares to enter low power consumption.
 *
 * This function is used to prepare to enter low power consumption.
 *
 * @param buttonHandle button handle pointer.
 * @retval kStatus_BUTTON_Success Successful operation.
 */
button_status_t BUTTON_EnterLowpower(button_handle_t buttonHandle);

/*!
 * @brief Restores from low power consumption.
 *
 * This function is used to restore from low power consumption.
 *
 * @param buttonHandle button handle pointer.
 * @retval kStatus_BUTTON_Success Successful operation.
 */
button_status_t BUTTON_ExitLowpower(button_handle_t buttonHandle);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

/*! @}*/

#endif /* __BUTTON_H__ */
