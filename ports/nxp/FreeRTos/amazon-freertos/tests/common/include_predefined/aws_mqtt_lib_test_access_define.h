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
 * @file aws_mqtt_lib_test_access_define.h
 * @brief Function wrappers to access private methods in aws_mqtt_lib.c.
 *
 * Needed for testing private functions.
 */

#ifndef _AWS_MQTT_LIB_TEST_ACCESS_DECLARE_H_
#define _AWS_MQTT_LIB_TEST_ACCESS_DECLARE_H_

#if ( mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT == 1 )

/*-----------------------------------------------------------*/

    MQTTTopicFilterType_t Test_prvGetTopicFilterType( const uint8_t * const pucTopicFilter,
                                                      uint16_t usTopicFilterLength )
    {
        return prvGetTopicFilterType( pucTopicFilter, usTopicFilterLength );
    }

#endif /* mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT */
/*-----------------------------------------------------------*/

#if ( mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT == 1 )

    MQTTBool_t Test_prvDoesTopicMatchTopicFilter( const uint8_t * const pucTopic,
                                                  uint16_t usTopicLength,
                                                  const uint8_t * const pucTopicFilter,
                                                  uint16_t usTopicFilterLength )
    {
        return prvDoesTopicMatchTopicFilter( pucTopic, usTopicLength, pucTopicFilter, usTopicFilterLength );
    }

#endif /* mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT */
/*-----------------------------------------------------------*/

void Test_prvResetMQTTContext( MQTTContext_t * pxMQTTContext )
{
    prvResetMQTTContext( pxMQTTContext );
}
/*-----------------------------------------------------------*/

#endif /* _AWS_MQTT_LIB_TEST_ACCESS_DECLARE_H_ */
