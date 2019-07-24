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
 * @file aws_secure_sockets_config_defaults.h
 * @brief Ensures that the required sockets configuration options are supplied
 * and the optional ones are set to sane values if the user does not supply.
 */

#ifndef AWS_INC_SECURE_SOCKETS_CONFIG_DEFAULTS_H_
#define AWS_INC_SECURE_SOCKETS_CONFIG_DEFAULTS_H_

/**
 * @brief Byte order of the target MCU must be defined.
 *
 * Valid values are pdLITTLE_ENDIAN and pdBIG_ENDIAN.
 */
#ifndef socketsconfigBYTE_ORDER
    #error "socketsconfigBYTE_ORDER must be defined."
#endif

/**
 * @brief Default socket send timeout.
 *
 * The user can change the send timeout for a socket using the SOCKETS_SetSockOpt API
 * with the SOCKETS_SO_SNDTIMEO option.
 */
#ifndef socketsconfigDEFAULT_SEND_TIMEOUT
    #define socketsconfigDEFAULT_SEND_TIMEOUT    ( 10000 )
#endif

/**
 * @brief Default socket receive timeout.
 *
 * The user can change the receive timeout for a socket using the SOCKETS_SetSockOpt API
 * with the SOCKETS_SO_RCVTIMEO option.
 */
#ifndef socketsconfigDEFAULT_RECV_TIMEOUT
    #define socketsconfigDEFAULT_RECV_TIMEOUT    ( 10000 )
#endif

#endif /* AWS_INC_SECURE_SOCKETS_CONFIG_DEFAULTS_H_ */
