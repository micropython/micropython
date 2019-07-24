/*
 * Amazon FreeRTOS V1.1.4
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
 * @file aws_shadow_config.h
 * @brief Configuration constants used by the Shadow library.
 */

#ifndef _AWS_SHADOW_CONFIG_H_
#define _AWS_SHADOW_CONFIG_H_

/**
 * @brief Number of jsmn tokens to use in parsing.  Each jsmn token contains 4 ints.
 * Ensure that the number of tokens does not overflow the calling task's stack,
 * but is also sufficient to parse the largest expected JSON documents. */
#define shadowconfigJSON_JSMN_TOKENS             ( 64 )

/**
 * @brief Maximum number of Shadow Clients.
 *
 * Up to this number of Shadow Clients may be successfully created with
 * #SHADOW_ClientCreate. Shadow clients are allocated in the global data
 * segment. Ensure that there is enough memory to accommodate the Shadow
 * Clients.
 *
 * @note Should be less than 256.
 */
#define shadowconfigMAX_CLIENTS                  ( 2 )

/**
 * @brief Shadow debug message setting.
 *
 * Set this value to @c 0 to disable Shadow Client debug messages; or set
 * it to @c 1 to enable debug messages. Ensure that the macro @c configPRINTF
 * is available if debugging is enabled.
 */
#define shadowconfigENABLE_DEBUG_LOGS            ( 0 )

/**
 * @brief Number of unique Things for which user notify callbacks can be
 * registered in each Shadow Client.
 *
 * Each Shadow Client stores the Things with user notify callbacks registered.
 * Define how many unique Things require user notify callbacks here.
 *
 * @note Should be less than 256.
 */
#define shadowconfigMAX_THINGS_WITH_CALLBACKS    ( 4 )

/**
 * @brief Time (in milliseconds) a Shadow Client may block during cleanup @b IF
 * a timeout occurs.
 *
 * Should a Shadow API call time out, the Shadow Client will stop its current
 * operation and cleanup before returning. The time below (in milliseconds) is
 * the amount of additional time that the Shadow Client may block to cleanup @b
 * IF the user's given timeout is inadequate. In general, 5000 ms is sufficient
 * for cleanup on a good connection; more time should be given if the connection
 * is unreliable.
 *
 * @note If a user gives a Shadow API call @a x milliseconds of block time but
 * @a x is insufficient time to complete the API call, then function may block
 * for up to (@a x + #shadowCLEANUP_TIME_MS) milliseconds. However, if @a x is
 * sufficient time for the API call, then block time will be at most @a x
 * milliseconds.
 * @warning If cleanup doesn't fully complete, users may be billed for MQTT
 * messages on topics that weren't properly cleaned up!
 */
#define shadowconfigCLEANUP_TIME_MS              ( 5000UL )

#endif /* _AWS_SHADOW_CONFIG_H_ */