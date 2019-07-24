/*
 * Amazon FreeRTOS V1.1.4
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
#ifndef AWS_TEST_RUNNER_CONFIG_H
#define AWS_TEST_RUNNER_CONFIG_H

/* Uncomment this line if you want to run AFQP tests only. */
/* #define testrunnerAFQP_ENABLED */

#define testrunnerUNSUPPORTED                      0

/* Unsupported tests. */
#define testrunnerFULL_OTA_CBOR_ENABLED            testrunnerUNSUPPORTED
#define testrunnerFULL_OTA_AGENT_ENABLED           testrunnerUNSUPPORTED
#define testrunnerFULL_OTA_PAL_ENABLED             testrunnerUNSUPPORTED
#define testrunnerFULL_MQTT_ALPN_ENABLED           testrunnerUNSUPPORTED

/* Supported tests. 0 = Disabled, 1 = Enabled */
#define testrunnerFULL_TCP_ENABLED                 1
#define testrunnerFULL_GGD_ENABLED                 0
#define testrunnerFULL_GGD_HELPER_ENABLED          0
#define testrunnerFULL_SHADOW_ENABLED              0
#define testrunnerFULL_MQTT_ENABLED                0
#define testrunnerFULL_MQTT_STRESS_TEST_ENABLED    0
#define testrunnerFULL_MQTT_AGENT_ENABLED          0
#define testrunnerFULL_WIFI_ENABLED                0
#define testrunnerFULL_PKCS11_ENABLED              0
#define testrunnerFULL_CRYPTO_ENABLED              0
#define testrunnerFULL_MEMORYLEAK_ENABLED          0
#define testrunnerFULL_TLS_ENABLED                 0
#define testrunnerFULL_POSIX_ENABLED               0


#endif /* AWS_TEST_RUNNER_CONFIG_H */
