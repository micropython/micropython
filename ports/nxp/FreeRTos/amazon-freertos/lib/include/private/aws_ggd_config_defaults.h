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
 * @file aws_ggd_config_defaults.h
 * @brief Greengrass Discovery default config options.
 *
 * Ensures that the config options for Greegrass discovert are set to
 * sensible default values if the user does not provide one.
 */
#ifndef _AWS_GREENGRASS_CONFIG_DEFAULT_H_
#define _AWS_GREENGRASS_CONFIG_DEFAULT_H_

/**
 * @brief The number of your network interface here.. Starting from 1.
 */
#ifndef ggdconfigCORE_NETWORK_INTERFACE
    #define ggdconfigCORE_NETWORK_INTERFACE    ( 1 )
#endif

/**
 * @brief Time in milliseconds after which the TCP receive operation should timeout.
 */
#ifndef ggdconfigTCP_RECEIVE_TIMEOUT_MS
    #define ggdconfigTCP_RECEIVE_TIMEOUT_MS    ( 5000 )
#endif

/**
 * @brief Number of times to retry before giving up while receiving on a connected TCP socket.
 */
#ifndef ggdconfigTCP_RECEIVE_RETRY
    #define ggdconfigTCP_RECEIVE_RETRY    ( 3 )
#endif

/**
 * @brief Time in milliseconds after which the TCP send operation should timeout.
 */
#ifndef ggdconfigTCP_SEND_TIMEOUT_MS
    #define ggdconfigTCP_SEND_TIMEOUT_MS    ( 5000 )
#endif

/**
 * @brief Number of times to retry before giving up while sending on a connected
 * TCP socket. (Not used for now).
 */
#ifndef ggdconfigTCP_SEND_RETRY
    #define ggdconfigTCP_SEND_RETRY    ( 1 )
#endif

/**
 * @brief Size of the array used by jsmn to store the tokens.
 */
#ifndef ggdconfigJSON_MAX_TOKENS
    #define ggdconfigJSON_MAX_TOKENS    ( 128 )        /* Size of the array used by jsmn to store the tokens. */
#endif

#ifndef ggdconfigPRINT
    #define ggdconfigPRINT    vLoggingPrintf
#endif

#endif /* ifndef _AWS_GREENGRASS_CONFIG_DEFAULT_H_ */
