/*
 * Amazon FreeRTOS
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */


/**
 * @file aws_mqtt_config_defaults.h
 * @brief MQTT default config options.
 *
 * Ensures that the config options for MQTT are set to sensible default
 * values if the user does not provide one.
 */

#ifndef _AWS_MQTT_CONFIG_DEFAULTS_H_
#define _AWS_MQTT_CONFIG_DEFAULTS_H_

/**
 * @brief Enable subscription management.
 *
 * Subscription management allows the user to register per subscription
 * callback.
 */
#ifndef mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT
    #define mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT            ( 1 )
#endif

/**
 * @brief Maximum length of the topic which can be stored in subscription
 * manager.
 *
 * If the user has enabled subscription management (by defining the macro
 * mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT to 1), then this macro must be defined
 * to accommodate the maximum length topic which the user is going to subscribe.
 * The subscribe operation will fail if the user tries to subscribe to a topic
 * of length more than the maximum specified here.
 */
#ifndef mqttconfigSUBSCRIPTION_MANAGER_MAX_TOPIC_LENGTH
    #define mqttconfigSUBSCRIPTION_MANAGER_MAX_TOPIC_LENGTH     ( 128 )
#endif

/**
 * @brief Maximum number of subscriptions which can be stored in subscription
 * manager.
 *
 * If the user has enabled subscription management (by defining the macro
 * mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT to 1), then this macro must be defined
 * to the maximum number of topics which the user is going to subscribe
 * simultaneously. The subscribe operation will fail is the user tries to
 * subscribe to more topics than the maximum specified here.
 */
#ifndef mqttconfigSUBSCRIPTION_MANAGER_MAX_SUBSCRIPTIONS
    #define mqttconfigSUBSCRIPTION_MANAGER_MAX_SUBSCRIPTIONS    ( 8 )
#endif

/**
 * @brief Define mqttconfigASSERT to enable asserts.
 *
 * mqttconfigASSERT should be defined to match the semantics of standard
 * C assert() macro i.e. an assertion should trigger if the parameter
 * passed is zero. If the standard C assert is available, the user might
 * do the following:
 * @code
 * #define mqttconfigASSERT( x ) assert( x )
 * @endcode
 *
 * Otherwise, a user can choose to implement a function which should be
 * called when an assertion triggers and then define the mqttconfigASSERT
 * to that function:
 * @code
 * extern void vAssertCalled( const char *pcFile, uint32_t ulLine );
 * #define mqttconfigASSERT( x ) if( ( x ) == 0 ) vAssertCalled( __FILE__, __LINE__ )
 * @endcode
 */
#ifndef mqttconfigASSERT
    #define mqttconfigASSERT( x )
#endif

/**
 * @brief Define mqttconfigENABLE_DEBUG_LOGS macro to 1 for enabling debug logs.
 *
 * If you choose to enable debug logs, the following function must be implemented
 * which is called to print logs:
 * @code
 * void vLoggingPrintf( const char *pcFormatString, ... );
 * @endcode
 */
#if ( mqttconfigENABLE_DEBUG_LOGS == 1 )
    extern void vLoggingPrintf( const char * pcFormatString,
                                ... );
    #define mqttconfigDEBUG_LOG( x )    vLoggingPrintf x
#else
    #define mqttconfigDEBUG_LOG( x )
#endif

#endif /* _AWS_MQTT_CONFIG_DEFAULTS_H_ */
