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


#ifndef __AWS_CLIENTCREDENTIAL__H__
#define __AWS_CLIENTCREDENTIAL__H__

/*
 * Include for device certificate and private key
 */
#include "aws_clientcredential_keys.h"

/*
 * MQTT Broker endpoint.
 */
static const char clientcredentialMQTT_BROKER_ENDPOINT[] = "Paste AWS IoT Broker endpoint here.";


/* Use of a "define" and not a "static const" here to be able to
 * use pre-compile concatenation on the string. */
#define clientcredentialIOT_THING_NAME               "Paste AWS IoT Thing name here."

/*
 * Port number the MQTT broker is using.
 */
#define clientcredentialMQTT_BROKER_PORT             8883

/*
 * Port number the Green Grass Discovery use for JSON retrieval from cloud is using.
 */
#define clientcredentialGREENGRASS_DISCOVERY_PORT    8443

/*
 * Wi-Fi network to join.
 */
#define clientcredentialWIFI_SSID                    "Paste Wi-Fi SSID here."

/*
 * Password needed to join Wi-Fi network.
 */
#define clientcredentialWIFI_PASSWORD                "Paste Wi-Fi password here."

/**
 * @brief Security type
 * WPA2 Security, @see WIFISecurity_t
 * Possible values are - eWiFiSecurityOpen, eWiFiSecurityWEP, eWiFiSecurityWPA,
 * eWiFiSecurityWPA2
 */
#define clientcredentialWIFI_SECURITY                eWiFiSecurityWPA2

#endif /* ifndef __AWS_CLIENTCREDENTIAL__H__ */
