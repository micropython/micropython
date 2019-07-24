/*
 * Amazon FreeRTOS Demo Bootloader V1.4.6
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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


#ifndef _AWS_BOOT_PAL_H_
#define _AWS_BOOT_PAL_H_

/**
 * @brief Launches application into execution.
 * This function launches the application into execution from application
 * address passed.
 * @param[in] pvLaunchAddress Application execution address.
 * @return It should never return.
 */
void BOOT_PAL_LaunchApplication( const void * const pvLaunchAddress );

/**
 * @brief Launches application into execution from descriptor.
 * This function launches application into execution from the information in
 * application descriptor received.
 * @param[in] pvLaunchDescriptor - Application launch descriptor.
 * @return It should never return.
 */
void BOOT_PAL_LaunchApplicationDesc( const BOOTImageDescriptor_t * const pvLaunchDescriptor );

/**
 * @brief Watchdog Init.
 * Initialize watchdog timer to support test run of new OTA image
 * received and rollback feature if this new image fails. The timeout is
 * selected based on time required for few retries for new OTA image to
 * communicate with the cloud service.
 * @param[in] None.
 * @return pdTRUE if watchdog is enabled, pdFALSE otherwise.
 */
BaseType_t BOOT_PAL_WatchdogInit( void );

/**
 * @brief Watchdog Enable.
 * Enable the watchdog timer.
 * @param[in] None.
 * @return pdTRUE if watchdog is enabled, pdFALSE otherwise.
 */
BaseType_t BOOT_PAL_WatchdogEnable( void );

/**
 * @brief Watchdog Disable.
 * Disable the watchdog timer.
 * @param[in] None.
 * @return pdTRUE if watchdog is disabled, pdFALSE otherwise.
 */
BaseType_t BOOT_PAL_WatchdogDisable( void );

/**
 * @brief Boot error state notification.
 * This function is used for notifying error state in bootloader.
 * @param[in] None.
 * @return It should never return.
 */
void BOOT_PAL_NotifyBootError( void );

#endif /* ifndef _AWS_BOOT_PAL_H_ */
