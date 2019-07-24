/*
 * Amazon FreeRTOS V1.2.3
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

#ifndef _AWS_APPLICATION_VERSION_H_
#define _AWS_APPLICATION_VERSION_H_

#define APP_VERSION_MAJOR    0
#define APP_VERSION_MINOR    9
#define APP_VERSION_BUILD    0

/* Application version structure. */
#pragma pack(push,1)
typedef struct
{
    union
    {
        #if ( defined( __BYTE_ORDER__ ) && defined( __ORDER_LITTLE_ENDIAN__ ) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__ ) || ( __little_endian__ == 1 ) || WIN32 || ( __BYTE_ORDER == __LITTLE_ENDIAN )
            struct
            {
                uint16_t usBuild;
                uint8_t ucMinor;
                uint8_t ucMajor;
            } x;
        #elif ( defined( __BYTE_ORDER__ ) && defined( __ORDER_BIG_ENDIAN__ ) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__ ) || ( __big_endian__ == 1 ) || ( __BYTE_ORDER == __BIG_ENDIAN )
            struct version
            {
                uint8_t ucMajor;
                uint8_t ucMinor;
                uint16_t usBuild;
            } x;
        #else /* if ( defined( __BYTE_ORDER__ ) && defined( __ORDER_LITTLE_ENDIAN__ ) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__ ) || ( __little_endian__ == 1 ) || WIN32 || ( __BYTE_ORDER == __LITTLE_ENDIAN ) */
        #error "Unable to determine byte order!"
        #endif /* if ( defined( __BYTE_ORDER__ ) && defined( __ORDER_LITTLE_ENDIAN__ ) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__ ) || ( __little_endian__ == 1 ) || WIN32 || ( __BYTE_ORDER == __LITTLE_ENDIAN ) */
        uint32_t ulVersion32;
    } u;
} AppVersion32_t;
#pragma pack(pop)

extern const AppVersion32_t xAppFirmwareVersion;

#endif /* ifndef _AWS_APPLICATION_VERSION_H_ */
