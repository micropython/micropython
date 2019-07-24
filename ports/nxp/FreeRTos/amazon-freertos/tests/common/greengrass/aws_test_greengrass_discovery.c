/*
 * Amazon FreeRTOS Greegrass Discovery Test V1.1.4
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
#include <stdio.h>
#include <string.h>

#include "aws_greengrass_discovery.h"
#include "aws_helper_secure_connect.h"
#include "jsmn.h"
#include "unity_fixture.h"
#include "unity.h"
#include "aws_greengrass_discovery_test_access_declare.h"
#include "aws_test_runner.h"

#define ggdLOOP_BACK_IP                    "127.0.0.1"
#define ggdHTTP_CONTENT_LENGTH_STRING      "content-length:"
#define cJSON_FILE_SIZE                    " 2193"
#define END_OF_HTTP_RESPONSE               "\r\n\r\n"
#define ggdJSON_FILE                       "{\"GGGroups\":[{\"GGGroupId\":\"myGroupID\",\"Cores\":[{\"thingArn\":\"myGreenGrassCoreArn\",\"Connectivity\":[{\"Id\":\"AUTOIP_10.60.212.138_0\",\"HostAddress\":\"44.44.44.44\",\"PortNumber\":1234,\"Metadata\":\"\"},{\"Id\":\"AUTOIP_127.0.0.1_1\",\"HostAddress\":\"127.0.0.1\",\"PortNumber\":8883,\"Metadata\":\"\"},{\"Id\":\"AUTOIP_192.168.2.2_2\",\"HostAddress\":\"01.23.456.789\",\"PortNumber\":4321,\"Metadata\":\"\"},{\"Id\":\"AUTOIP_::1_3\",\"HostAddress\":\"::1\",\"PortNumber\":8883,\"Metadata\":\"\"},{\"Id\":\"AUTOIP_fe80::bfda:8f62:7b4b:f358_4\",\"HostAddress\":\"fe80::bfda:8f62:7b4b:f358\",\"PortNumber\":8883,\"Metadata\":\"\"},{\"Id\":\"AUTOIP_fe80::e234:cff9:f53f:6216_5\",\"HostAddress\":\"fe80::e234:cff9:f53f:6216\",\"PortNumber\":8883,\"Metadata\":\"\"}]}],\"CAs\":[\"-----BEGIN CERTIFICATE-----\\nMIIEFTCCAv2gAwIBAgIVAPRru+NqCDr0r6oD6PnTG05rWuY+MA0GCSqGSIb3DQEB\\nCwUAMIGoMQswCQYDVQQGEwJVUzEYMBYGA1UECgwPQW1hem9uLmNvbSBJbmMuMRww\\nGgYDVQQLDBNBbWF6b24gV2ViIFNlcnZpY2VzMRMwEQYDVQQIDApXYXNoaW5ndG9u\\nMRAwDgYDVQQHDAdTZWF0dGxlMTowOAYDVQQDDDE5NDI5MjczNzY5NjU6ZDk3ZmZl\\nZmUtNTI4MS00ZWM5LTk4NDYtYjNlZTQxMDRjMjAxMCAXDTE3MDcwNjIwMDczOFoY\\nDzIwOTcwNzA2MjAwNzM3WjCBqDELMAkGA1UEBhMCVVMxGDAWBgNVBAoMD0FtYXpv\\nbi5jb20gSW5jLjEcMBoGA1UECwwTQW1hem9uIFdlYiBTZXJ2aWNlczETMBEGA1UE\\nCAwKV2FzaGluZ3RvbjEQMA4GA1UEBwwHU2VhdHRsZTE6MDgGA1UEAwwxOTQyOTI3\\nMzc2OTY1OmQ5N2ZmZWZlLTUyODEtNGVjOS05ODQ2LWIzZWU0MTA0YzIwMTCCASIw\\nDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKxzJpXU2DZDEglh/FT01epAWby6\\np4Ymw76icyMzBUJzafibABJ3cTyjDQE6ZqbSl1ryBxGwQBsveIgj8SVVtv927wk7\\nlncgD+EghfTZgSfscND653AJeVFQlCeHipZI32wzXyPmwglFrWp9vsrY/8BO1Kjk\\nSAs4o8fDVVMAaZCJDMuc5csc3CQ2OJYLOl+SZisGNM1h0xHpWieM38KDDrp99x8Q\\nTwDmgaMjtdIJR7Y9Nzm0N78gTf3gTazEO9iUKojVCNubxK/lQ6KjJ0JcvsljPpVp\\nuzjOmn91xmNoHEQCboa7YoYNNbdAbftGeUl16wFdTgbuUS9vakk5idVoC2ECAwEA\\nAaMyMDAwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUmcz4OlH9+mlpnTKG3taI\\nw+6FSk0wDQYJKoZIhvcNAQELBQADggEBACeiQ6MxiktsU0sLNmP1cNbiuBuutjoq\\nymk476Bhr4E2WSE0B9W1TFOSLIYx9oN63T3lXzsGHP/MznueIbqbwFf/o5aXI7th\\n+J+i9LgBrViNvzkze7G0GiPuEQ7ox4XnPBJAFtTZxa8gXL95QfcypERpQs28lg7W\\nQpdNhiBN+c4o1aSOzJ474sjXnjtI1G2jRTKucm0buYYeAeVT7kpBq9YL7gGfOcyj\\nsPxQEgyQV2Mk+b1q7lYDS4tnzoRkUfNLgAtDKSh8S8iVhAR6wRR2G3aMySKrOxbg\\nalghO3OqfeuTwIj9w17JTAyYAME22RJQ6oxEJ8rHp/9PaYnOmiSkP7M=\\n-----END CERTIFICATE-----\\n\"]}]}"
#define ggdTestJSON_MAX_TOKENS             128       /* Size of the array used by jsmn to store the tokens. */
#define ggdTestJSON_GROUPID_TOKEN_INDEX    4
#define ggdTestJSON_CORE_TOKEN_INDEX       9
#define ggdTestJSON_PORT_ADRESS_1          1234
#define ggdTestJSON_PORT_ADRESS_3          4321
#define ggdTestLOOP_NUMBER                 10

#define ggdJSON_FILE_GROUPID               "GGGroupId"
#define ggdJSON_FILE_THING_ARN             "thingArn"
#define ggdJSON_FILE_HOST_ADDRESS          "HostAddress"
#define ggdJSON_FILE_CERTIFICATE           "CAs"
#define ggdJSON_FILE_PORT_NUMBER           "PortNumber"

static const char cJSON_FILE[] = ggdJSON_FILE;
static const char cCERTIFICATE[] = "-----BEGIN CERTIFICATE-----\nMIIEFTCCAv2gAwIBAgIVAPRru+NqCDr0r6oD6PnTG05rWuY+MA0GCSqGSIb3DQEB\nCwUAMIGoMQswCQYDVQQGEwJVUzEYMBYGA1UECgwPQW1hem9uLmNvbSBJbmMuMRww\nGgYDVQQLDBNBbWF6b24gV2ViIFNlcnZpY2VzMRMwEQYDVQQIDApXYXNoaW5ndG9u\nMRAwDgYDVQQHDAdTZWF0dGxlMTowOAYDVQQDDDE5NDI5MjczNzY5NjU6ZDk3ZmZl\nZmUtNTI4MS00ZWM5LTk4NDYtYjNlZTQxMDRjMjAxMCAXDTE3MDcwNjIwMDczOFoY\nDzIwOTcwNzA2MjAwNzM3WjCBqDELMAkGA1UEBhMCVVMxGDAWBgNVBAoMD0FtYXpv\nbi5jb20gSW5jLjEcMBoGA1UECwwTQW1hem9uIFdlYiBTZXJ2aWNlczETMBEGA1UE\nCAwKV2FzaGluZ3RvbjEQMA4GA1UEBwwHU2VhdHRsZTE6MDgGA1UEAwwxOTQyOTI3\nMzc2OTY1OmQ5N2ZmZWZlLTUyODEtNGVjOS05ODQ2LWIzZWU0MTA0YzIwMTCCASIw\nDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKxzJpXU2DZDEglh/FT01epAWby6\np4Ymw76icyMzBUJzafibABJ3cTyjDQE6ZqbSl1ryBxGwQBsveIgj8SVVtv927wk7\nlncgD+EghfTZgSfscND653AJeVFQlCeHipZI32wzXyPmwglFrWp9vsrY/8BO1Kjk\nSAs4o8fDVVMAaZCJDMuc5csc3CQ2OJYLOl+SZisGNM1h0xHpWieM38KDDrp99x8Q\nTwDmgaMjtdIJR7Y9Nzm0N78gTf3gTazEO9iUKojVCNubxK/lQ6KjJ0JcvsljPpVp\nuzjOmn91xmNoHEQCboa7YoYNNbdAbftGeUl16wFdTgbuUS9vakk5idVoC2ECAwEA\nAaMyMDAwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUmcz4OlH9+mlpnTKG3taI\nw+6FSk0wDQYJKoZIhvcNAQELBQADggEBACeiQ6MxiktsU0sLNmP1cNbiuBuutjoq\nymk476Bhr4E2WSE0B9W1TFOSLIYx9oN63T3lXzsGHP/MznueIbqbwFf/o5aXI7th\n+J+i9LgBrViNvzkze7G0GiPuEQ7ox4XnPBJAFtTZxa8gXL95QfcypERpQs28lg7W\nQpdNhiBN+c4o1aSOzJ474sjXnjtI1G2jRTKucm0buYYeAeVT7kpBq9YL7gGfOcyj\nsPxQEgyQV2Mk+b1q7lYDS4tnzoRkUfNLgAtDKSh8S8iVhAR6wRR2G3aMySKrOxbg\nalghO3OqfeuTwIj9w17JTAyYAME22RJQ6oxEJ8rHp/9PaYnOmiSkP7M=\n-----END CERTIFICATE-----\n";
static const char cMyGroupID[] = "myGroupID";
static const char cIP_ADDRESS_3[] = "01.23.456.789";
static const char cIP_ADDRESS_1[] = "44.44.44.44";
static const char cMY_CORE_ARN[] = "myGreenGrassCoreArn";

static jsmntok_t pxTok[ ggdTestJSON_MAX_TOKENS ];
static Socket_t xSocket;

TEST_GROUP( Full_GGD );

TEST_SETUP( Full_GGD )
{
}

TEST_TEAR_DOWN( Full_GGD )
{
    if( xSocket != SOCKETS_INVALID_SOCKET )
    {
        /* Close the socket. */
        ( void ) SOCKETS_Close( xSocket );
    }
}


TEST_GROUP_RUNNER( Full_GGD )
{
    RUN_TEST_CASE( Full_GGD, JSONRequestStart );
    RUN_TEST_CASE( Full_GGD, JSONRequestAbort );
    RUN_TEST_CASE( Full_GGD, GetIPandCertificateFromJSON );
    RUN_TEST_CASE( Full_GGD, GetIPOnInterface );
    RUN_TEST_CASE( Full_GGD, JSONRequestGetSize );
    RUN_TEST_CASE( Full_GGD, JSONRequestGetFile );
    RUN_TEST_CASE( Full_GGD, CheckForContentLengthString );
    RUN_TEST_CASE( Full_GGD, Jsoneq );
    RUN_TEST_CASE( Full_GGD, CheckMatch );
    RUN_TEST_CASE( Full_GGD, GetCertificate );
    RUN_TEST_CASE( Full_GGD, GetCore );
    RUN_TEST_CASE( Full_GGD, prvIsIPvalid );
    RUN_TEST_CASE( Full_GGD, GetGGCIPandCertificate );
}

TEST( Full_GGD, JSONRequestAbort )
{
    /** @brief check for stability for all meaningfull values of socket.
     * 1. Invalid
     * 2. Correct
     * 3. NULL
     *  @{
     */
    if( TEST_PROTECT() )
    {
        xSocket = SOCKETS_INVALID_SOCKET;
        GGD_JSONRequestAbort( &xSocket );

        GGD_JSONRequestStart( &xSocket );
        GGD_JSONRequestAbort( &xSocket );
    }
    else
    {
        TEST_FAIL();
    }

    if( TEST_PROTECT() )
    {
        GGD_JSONRequestAbort( NULL );
        TEST_FAIL();
    }

    /** @}*/
}

TEST( Full_GGD, prvIsIPvalid )
{
    BaseType_t xStatus;

    if( TEST_PROTECT() )
    {
        /** @brief Check IP is correctly filtered if a loopback address is given.
         *  @{
         */
        xStatus = test_prvIsIPvalid( ggdLOOP_BACK_IP, sizeof( ggdLOOP_BACK_IP ) );
        TEST_ASSERT_TRUE( xStatus == pdFALSE );
        /** @}*/

        /** @brief Check IP is not filtered if correct IP is given.
         *  @{
         */
        xStatus = test_prvIsIPvalid( clientcredentialMQTT_BROKER_ENDPOINT, sizeof( clientcredentialMQTT_BROKER_ENDPOINT ) );
        TEST_ASSERT_TRUE( xStatus == pdTRUE );
        /** @}*/
    }
    else
    {
        TEST_FAIL();
    }
}


TEST( Full_GGD, GetGGCIPandCertificate )
{
    BaseType_t i;
    BaseType_t xStatus;
    char cMsgBuffer[ 128 ];
    GGD_HostAddressData_t xHostAddressData;
    int16_t nBufferLength = 128;
    uint32_t ulBufferSize = testrunnerBUFFER_SIZE;

    if( TEST_PROTECT() )
    {
        /** @brief Check function works in ideal scenario.
         * Provided with known JSON file and checking output.
         * Check with auto-search flag set to false and true
         *  @{
         */
        for( i = 0; i < ggdTestLOOP_NUMBER; i++ )
        {
            xStatus = GGD_GetGGCIPandCertificate( cBuffer, /*lint !e971 can use char without signed/unsigned. */
                                                  ulBufferSize,
                                                  &xHostAddressData );

            snprintf( cMsgBuffer, nBufferLength,
                      "GGD_GetGGCIPandCertificate returned %d on iteration %d",
                      ( int )xStatus, ( int )i );
            TEST_ASSERT_EQUAL_INT32_MESSAGE( pdPASS, xStatus, cBuffer );
        }

        /** @}*/

        /** @brief Check fail is returned if provided buffer is too small
         *  @{
         */
        xStatus = GGD_GetGGCIPandCertificate( cBuffer, /*lint !e971 can use char without signed/unsigned. */
                                              xHostAddressData.ulCertificateSize - 1,
                                              &xHostAddressData );
        TEST_ASSERT_EQUAL_INT32_MESSAGE( pdFAIL, xStatus, "GGD_GetGGCIPandCertificate() passed when the input buffer was too small." );
        /** @}*/
    }
    else
    {
        TEST_FAIL();
    }

    /** @brief Check stability.
     *
     *  @{
     */
    if( TEST_PROTECT() )
    {
        xStatus = GGD_GetGGCIPandCertificate( NULL,
                                              ulBufferSize,
                                              &xHostAddressData );
        TEST_FAIL();
    }

    if( TEST_PROTECT() )
    {
        xStatus = GGD_GetGGCIPandCertificate( cBuffer,
                                              ulBufferSize,
                                              NULL );
        TEST_FAIL();
    }

    /** @}*/
}


TEST( Full_GGD, GetIPandCertificateFromJSON )
{
    uint32_t ulJSONFileSize = strlen( cJSON_FILE );
    BaseType_t xStatus;
    HostParameters_t xHostParameters;
    GGD_HostAddressData_t xHostAddressData;
    BaseType_t xAutoSearchFlag;
    char cBadGroupId[] = "myBadGroupID";
    char cBadCoreARN[] = "myBadCoreARN";
    uint32_t ulBufferSize = testrunnerBUFFER_SIZE;
    uint32_t ulByteRead;
    BaseType_t xJSONFileRetrieveCompleted;

    if( TEST_PROTECT() )
    {
        /** @brief Prepare test.
         *  @{
         */
        memcpy( cBuffer, cJSON_FILE, strlen( cJSON_FILE ) );
        /** @}*/

        /** @brief Check function works in ideal scenario.
         * Provided with known JSON file and checking output.
         * Check with auto-search flag set to false and true
         *  @{
         */
        xHostParameters.pcCoreAddress = ( char * ) cMY_CORE_ARN;
        xHostParameters.pcGroupName = ( char * ) cMyGroupID;
        xHostParameters.ucInterface = 3;
        xAutoSearchFlag = pdFALSE;
        xStatus = GGD_GetIPandCertificateFromJSON( cBuffer, /*lint !e971 can use char without signed/unsigned. */
                                                   ulJSONFileSize,
                                                   &xHostParameters,
                                                   &xHostAddressData,
                                                   xAutoSearchFlag );
        TEST_ASSERT_EQUAL_INT32( pdPASS, xStatus );
        TEST_ASSERT_EQUAL_STRING( cIP_ADDRESS_3, xHostAddressData.pcHostAddress );
        TEST_ASSERT_EQUAL_STRING( cCERTIFICATE, xHostAddressData.pcCertificate );
        TEST_ASSERT_EQUAL_INT32( strlen( cCERTIFICATE ) + 1, xHostAddressData.ulCertificateSize );

        xAutoSearchFlag = pdTRUE;
        xStatus = GGD_JSONRequestStart( &xSocket );

        if( xStatus == pdPASS )
        {
            xStatus = GGD_JSONRequestGetSize( &xSocket, &ulJSONFileSize );
        }

        if( xStatus == pdPASS )
        {
            ulByteRead = 0;
            xStatus = GGD_JSONRequestGetFile( &xSocket,
                                              cBuffer,
                                              ulBufferSize,
                                              &ulByteRead,
                                              &xJSONFileRetrieveCompleted,
                                              ulJSONFileSize );
        }

        TEST_ASSERT_EQUAL_INT32( pdPASS, xStatus );

        xStatus = GGD_GetIPandCertificateFromJSON( cBuffer, /*lint !e971 can use char without signed/unsigned. */
                                                   ulJSONFileSize,
                                                   &xHostParameters,
                                                   &xHostAddressData,
                                                   xAutoSearchFlag );
        TEST_ASSERT_EQUAL_INT32( pdPASS, xStatus );
        /** @}*/

        /** @brief Check manual connect fails if host parameters not found in JSON
         *  @{
         */
        xAutoSearchFlag = pdFALSE;
        memcpy( cBuffer, cJSON_FILE, strlen( cJSON_FILE ) );
        xHostParameters.pcCoreAddress = ( char * ) cMY_CORE_ARN;
        xHostParameters.pcGroupName = ( char * ) cBadGroupId;
        xHostParameters.ucInterface = 3;
        xStatus = GGD_GetIPandCertificateFromJSON( cBuffer, /*lint !e971 can use char without signed/unsigned. */
                                                   ulJSONFileSize,
                                                   &xHostParameters,
                                                   &xHostAddressData,
                                                   xAutoSearchFlag );
        TEST_ASSERT_EQUAL_INT32( pdFAIL, xStatus );

        xAutoSearchFlag = pdFALSE;
        memcpy( cBuffer, cJSON_FILE, strlen( cJSON_FILE ) );
        xHostParameters.pcCoreAddress = ( char * ) cBadCoreARN;
        xHostParameters.pcGroupName = ( char * ) cMyGroupID;
        xHostParameters.ucInterface = 3;
        xStatus = GGD_GetIPandCertificateFromJSON( cBuffer, /*lint !e971 can use char without signed/unsigned. */
                                                   ulJSONFileSize,
                                                   &xHostParameters,
                                                   &xHostAddressData,
                                                   xAutoSearchFlag );
        TEST_ASSERT_EQUAL_INT32( pdFAIL, xStatus );

        xAutoSearchFlag = pdFALSE;
        memcpy( cBuffer, cJSON_FILE, strlen( cJSON_FILE ) );
        xHostParameters.pcCoreAddress = ( char * ) cMY_CORE_ARN;
        xHostParameters.pcGroupName = ( char * ) cMyGroupID;
        xHostParameters.ucInterface = 100;
        xStatus = GGD_GetIPandCertificateFromJSON( cBuffer, /*lint !e971 can use char without signed/unsigned. */
                                                   ulJSONFileSize,
                                                   &xHostParameters,
                                                   &xHostAddressData,
                                                   xAutoSearchFlag );
        TEST_ASSERT_EQUAL_INT32( pdFAIL, xStatus );
        /** @}*/
    }
    else
    {
        TEST_FAIL();
    }

    /** @brief Check stability if NULL pointers are passed.
     *
     *  @{
     */
    xAutoSearchFlag = pdFALSE;

    if( TEST_PROTECT() )
    {
        xHostParameters.pcCoreAddress = ( char * ) cMY_CORE_ARN;
        xHostParameters.pcGroupName = ( char * ) cMyGroupID;
        xHostParameters.ucInterface = 3;
        xAutoSearchFlag = pdFALSE;
        xStatus = GGD_GetIPandCertificateFromJSON( NULL, /*lint !e971 can use char without signed/unsigned. */
                                                   ulJSONFileSize,
                                                   &xHostParameters,
                                                   &xHostAddressData,
                                                   xAutoSearchFlag );
        TEST_FAIL();
    }

    if( TEST_PROTECT() )
    {
        memcpy( cBuffer, cJSON_FILE, strlen( cJSON_FILE ) );
        xStatus = GGD_GetIPandCertificateFromJSON( cBuffer, /*lint !e971 can use char without signed/unsigned. */
                                                   ulJSONFileSize,
                                                   NULL,
                                                   &xHostAddressData,
                                                   xAutoSearchFlag );
        TEST_FAIL();
    }

    if( TEST_PROTECT() )
    {
        memcpy( cBuffer, cJSON_FILE, strlen( cJSON_FILE ) );
        xStatus = GGD_GetIPandCertificateFromJSON( cBuffer, /*lint !e971 can use char without signed/unsigned. */
                                                   ulJSONFileSize,
                                                   &xHostParameters,
                                                   NULL,
                                                   xAutoSearchFlag );
        TEST_FAIL();
    }

    /** @}*/
}

TEST( Full_GGD, GetCore )
{
    BaseType_t xStatus;
    BaseType_t xAutoConnectFlag = pdTRUE;
    int32_t lNbTokens;
    uint32_t ulJSONFileSize = strlen( cJSON_FILE );
    HostParameters_t xHostParameters;
    uint32_t ulTokenIndex;
    jsmn_parser xParser;
    char cBadGroupId[] = "myBadGroupID";
    char cBadCoreARN[] = "myBadCoreARN";

    if( TEST_PROTECT() )
    {
        /** @brief Prepare test.
         *  @{
         */
        jsmn_init( &xParser );

        /* From jsmn, parse the JSON file. */
        lNbTokens = ( int32_t ) jsmn_parse( &xParser,
                                            cJSON_FILE, /*lint !e971 can use char without signed/unsigned. */
                                            ( size_t ) ulJSONFileSize,
                                            pxTok,
                                            ( unsigned int ) ggdTestJSON_MAX_TOKENS ); /*lint !e961 redundant casting only when int = int32_t. */
/** @}*/

/** @brief Check core is found and returned index is correct
 * when given a know JSON file.
 *  @{
 */
        xAutoConnectFlag = pdFALSE;
        xHostParameters.pcCoreAddress = ( char * ) cMY_CORE_ARN;
        xHostParameters.pcGroupName = ( char * ) cMyGroupID;
        xStatus = test_prvGGDGetCore( cJSON_FILE,
                                      &xHostParameters,
                                      xAutoConnectFlag,
                                      pxTok,
                                      lNbTokens,
                                      &ulTokenIndex );
        TEST_ASSERT_EQUAL_INT32( pdPASS, xStatus );
        TEST_ASSERT_EQUAL_INT32( ggdTestJSON_CORE_TOKEN_INDEX, ulTokenIndex );
        /** @}*/

        /** @brief Check core search is failed when provided wrong groupID or ARN
         * when given a know JSON file.
         *  @{
         */
        xAutoConnectFlag = pdFALSE;
        xHostParameters.pcCoreAddress = ( char * ) cMY_CORE_ARN;
        xHostParameters.pcGroupName = ( char * ) cBadGroupId;
        xStatus = test_prvGGDGetCore( cJSON_FILE,
                                      &xHostParameters,
                                      xAutoConnectFlag,
                                      pxTok,
                                      lNbTokens,
                                      &ulTokenIndex );
        TEST_ASSERT_EQUAL_INT32( pdFAIL, xStatus );

        xAutoConnectFlag = pdFALSE;
        xHostParameters.pcCoreAddress = ( char * ) cBadCoreARN;
        xHostParameters.pcGroupName = ( char * ) cMyGroupID;
        xStatus = test_prvGGDGetCore( cJSON_FILE,
                                      &xHostParameters,
                                      xAutoConnectFlag,
                                      pxTok,
                                      lNbTokens,
                                      &ulTokenIndex );
        TEST_ASSERT_EQUAL_INT32( pdFAIL, xStatus );
        /** @}*/

        /** @brief Check core search succeeds when auto-search is enabled
         * irrelevant of Host parameters.
         * Check code doesn't crash if NULL is passed
         *  @{
         */
        xAutoConnectFlag = pdTRUE;
        xStatus = test_prvGGDGetCore( cJSON_FILE,
                                      NULL,
                                      xAutoConnectFlag,
                                      pxTok,
                                      lNbTokens,
                                      &ulTokenIndex );
        TEST_ASSERT_EQUAL_INT32( pdTRUE, xStatus );
        /** @}*/
    }
    else
    {
        TEST_FAIL();
    }
}

TEST( Full_GGD, GetIPOnInterface )
{
    BaseType_t xStatus;
    int32_t lNbTokens;
    uint32_t ulJSONFileSize = strlen( cJSON_FILE );
    GGD_HostAddressData_t xHostAddressData;
    uint8_t ucTargetInterface;
    uint8_t usCurrentInterface;
    uint32_t ulTokenIndex;
    jsmn_parser xParser;

    if( TEST_PROTECT() )
    {
        /** @brief Prepare test.
         *  @{
         */
        jsmn_init( &xParser );

        /* From jsmn, parse the JSON file. */
        lNbTokens = ( int32_t ) jsmn_parse( &xParser,
                                            cJSON_FILE, /*lint !e971 can use char without signed/unsigned. */
                                            ( size_t ) ulJSONFileSize,
                                            pxTok,
                                            ( unsigned int ) ggdTestJSON_MAX_TOKENS ); /*lint !e961 redundant casting only when int = int32_t. */

        memcpy( cBuffer, cJSON_FILE, strlen( cJSON_FILE ) );
        /** @}*/

        /** @brief Check that IP is found,
         * the function can be re-used and still get a match without reseting current interface.
         *  @{
         */
        usCurrentInterface = 0;
        ucTargetInterface = 1;
        ulTokenIndex = ggdTestJSON_CORE_TOKEN_INDEX;
        xStatus = test_prvGGDGetIPOnInterface( cBuffer,
                                               ucTargetInterface,
                                               pxTok,
                                               lNbTokens,
                                               &xHostAddressData,
                                               &ulTokenIndex,
                                               &usCurrentInterface );
        TEST_ASSERT_EQUAL_INT32( pdPASS, xStatus );
        TEST_ASSERT_EQUAL_STRING( cIP_ADDRESS_1, xHostAddressData.pcHostAddress );
        TEST_ASSERT_EQUAL_INT32( ggdTestJSON_PORT_ADRESS_1, xHostAddressData.usPort );

        /* Do not reset current interface and token index here. */
        ucTargetInterface = 3;
        xStatus = test_prvGGDGetIPOnInterface( cBuffer,
                                               ucTargetInterface,
                                               pxTok,
                                               lNbTokens,
                                               &xHostAddressData,
                                               &ulTokenIndex,
                                               &usCurrentInterface );
        TEST_ASSERT_EQUAL_INT32( pdPASS, xStatus );
        TEST_ASSERT_EQUAL_STRING( cIP_ADDRESS_3, xHostAddressData.pcHostAddress );
        TEST_ASSERT_EQUAL_INT32( ggdTestJSON_PORT_ADRESS_3, xHostAddressData.usPort );
        /** @}*/

        /** @brief Check status returns failed if not host Address is found
         *  @{
         */
        usCurrentInterface = 0;
        ucTargetInterface = 1;
        ulTokenIndex = lNbTokens - 2; /* Set to the end of JSON file*/
        xStatus = test_prvGGDGetIPOnInterface( cBuffer,
                                               ucTargetInterface,
                                               pxTok,
                                               lNbTokens,
                                               &xHostAddressData,
                                               &ulTokenIndex,
                                               &usCurrentInterface );
        TEST_ASSERT_EQUAL_INT32( pdFAIL, xStatus );
        /** @}*/

        /** @brief Check status returns failed if interface is not found
         *  @{
         */
        usCurrentInterface = 0;
        ucTargetInterface = 100;
        ulTokenIndex = ggdTestJSON_CORE_TOKEN_INDEX; /* Set to the end of JSON file*/
        xStatus = test_prvGGDGetIPOnInterface( cBuffer,
                                               ucTargetInterface,
                                               pxTok,
                                               lNbTokens,
                                               &xHostAddressData,
                                               &ulTokenIndex,
                                               &usCurrentInterface );
        TEST_ASSERT_EQUAL_INT32( pdFAIL, xStatus );
        /** @}*/
    }
    else
    {
        TEST_FAIL();
    }
}

TEST( Full_GGD, GetCertificate )
{
    BaseType_t xStatus;
    BaseType_t xAutoConnectFlag = pdTRUE;
    int32_t lNbTokens;
    uint32_t ulJSONFileSize = strlen( cJSON_FILE );
    HostParameters_t xHostParameters;
    GGD_HostAddressData_t xHostAddressData;
    char cBadGroupId[] = "myBadGroupID";
    jsmn_parser xParser;

    if( TEST_PROTECT() )
    {
        /** @brief Prepare test.
         *  @{
         */
        jsmn_init( &xParser );

        /* From jsmn, parse the JSON file. */
        lNbTokens = ( int32_t ) jsmn_parse( &xParser,
                                            cJSON_FILE, /*lint !e971 can use char without signed/unsigned. */
                                            ( size_t ) ulJSONFileSize,
                                            pxTok,
                                            ( unsigned int ) ggdTestJSON_MAX_TOKENS ); /*lint !e961 redundant casting only when int = int32_t. */

        xHostParameters.pcGroupName = ( char * ) cMyGroupID;
        memcpy( cBuffer, cJSON_FILE, strlen( cJSON_FILE ) );
        /** @}*/

        /** @brief Check Certificate is found provided correct data input
         *  @{
         */
        xAutoConnectFlag = pdFALSE;
        xStatus = test_prvGGDGetCertificate( cBuffer,
                                             &xHostParameters,
                                             xAutoConnectFlag,
                                             pxTok,
                                             lNbTokens,
                                             &xHostAddressData );
        TEST_ASSERT_EQUAL_INT32( pdPASS, xStatus );
        TEST_ASSERT_EQUAL_MEMORY( cCERTIFICATE, xHostAddressData.pcCertificate, strlen( cCERTIFICATE ) );
        TEST_ASSERT_EQUAL_INT32( strlen( cCERTIFICATE ) + 1, xHostAddressData.ulCertificateSize );
        /** @}*/

        /** @brief Check Certificate is not found provided it can't find group id in JSON file
         *  @{
         */
        xHostParameters.pcGroupName = ( char * ) cBadGroupId;
        xAutoConnectFlag = pdFALSE;
        memcpy( cBuffer, cJSON_FILE, strlen( cJSON_FILE ) );
        xStatus = test_prvGGDGetCertificate( cBuffer,
                                             &xHostParameters,
                                             xAutoConnectFlag,
                                             pxTok,
                                             lNbTokens,
                                             &xHostAddressData );
        TEST_ASSERT_EQUAL_INT32( pdFALSE, xStatus );
        /** @}*/

        /** @brief Check Certificate is found provided independant of xHostParameters
         * if xAutoConnectFlag is set to true.
         * Should not crash if a NULL is provided when doing auto connect
         *  @{
         */
        xAutoConnectFlag = pdTRUE;
        memcpy( cBuffer, cJSON_FILE, strlen( cJSON_FILE ) );
        xStatus = test_prvGGDGetCertificate( cBuffer,
                                             NULL, /* HostParameters set to NULL */
                                             xAutoConnectFlag,
                                             pxTok,
                                             lNbTokens,
                                             &xHostAddressData );
        TEST_ASSERT_EQUAL_INT32( pdTRUE, xStatus );
        /** @}*/
    }
    else
    {
        TEST_FAIL();
    }
}

TEST( Full_GGD, CheckMatch )
{
    BaseType_t xMatch;
    BaseType_t xAutoConnectFlag = pdTRUE;
    int32_t lNbTokens;
    uint32_t ulTokenIndex;
    uint32_t ulJSONFileSize = strlen( cJSON_FILE );
    jsmn_parser xParser;

    if( TEST_PROTECT() )
    {
        /** @brief Prepare test.
         *  @{
         */
        jsmn_init( &xParser );

        /* From jsmn, parse the JSON file. */
        lNbTokens = ( int32_t ) jsmn_parse( &xParser,
                                            cJSON_FILE, /*lint !e971 can use char without signed/unsigned. */
                                            ( size_t ) ulJSONFileSize,
                                            pxTok,
                                            ( unsigned int ) ggdTestJSON_MAX_TOKENS ); /*lint !e961 redundant casting only when int = int32_t. */
        TEST_ASSERT_GREATER_THAN( 0, lNbTokens );
/** @}*/

/** @brief Check we have a match finding the group ID.
 *  @{
 */
        xAutoConnectFlag = pdFALSE;
        ulTokenIndex = ggdTestJSON_GROUPID_TOKEN_INDEX;
        test_prvCheckMatch( cJSON_FILE,
                            pxTok,
                            ulTokenIndex,
                            &xMatch,
                            ggdJSON_FILE_GROUPID,
                            cMyGroupID,
                            xAutoConnectFlag );
        TEST_ASSERT_EQUAL_INT32( pdTRUE, xMatch );
        /** @}*/

        /** @brief Check we don't have a match finding the group ID,
         * if we pass the wrong index
         *  @{
         */
        xAutoConnectFlag = pdFALSE;
        ulTokenIndex = ggdTestJSON_GROUPID_TOKEN_INDEX + 1;
        test_prvCheckMatch( cJSON_FILE,
                            pxTok,
                            ulTokenIndex,
                            &xMatch,
                            ggdJSON_FILE_GROUPID,
                            cMyGroupID,
                            xAutoConnectFlag );
        TEST_ASSERT_EQUAL_INT32( pdFALSE, xMatch );
        /** @}*/

        /** @brief Auto-connect should find the index then conect to any string.
         *  Check that function match return false if the index is not found,
         * but returns true for any random string, as long as index is found.
         *
         *  @{
         */
        xAutoConnectFlag = pdTRUE;
        ulTokenIndex = ggdTestJSON_GROUPID_TOKEN_INDEX + 1;
        test_prvCheckMatch( cJSON_FILE,
                            pxTok,
                            ulTokenIndex,
                            &xMatch,
                            "randomString",
                            cMyGroupID,
                            xAutoConnectFlag );
        TEST_ASSERT_EQUAL_INT32( pdFALSE, xMatch );

        xAutoConnectFlag = pdTRUE;
        ulTokenIndex = ggdTestJSON_GROUPID_TOKEN_INDEX;
        test_prvCheckMatch( cJSON_FILE,
                            pxTok,
                            ulTokenIndex,
                            &xMatch,
                            ggdJSON_FILE_GROUPID,
                            "randomString",
                            xAutoConnectFlag );
        TEST_ASSERT_EQUAL_INT32( pdTRUE, xMatch );
        /** @}*/

        /** @brief Check we don't have a match finding the group ID,
         * if we pass the wrong string
         *  @{
         */
        xAutoConnectFlag = pdFALSE;
        ulTokenIndex = ggdTestJSON_GROUPID_TOKEN_INDEX;
        test_prvCheckMatch( cJSON_FILE,
                            pxTok,
                            ulTokenIndex,
                            &xMatch,
                            "randomString",
                            cMyGroupID,
                            xAutoConnectFlag );
        TEST_ASSERT_EQUAL_INT32( pdFALSE, xMatch );
        /** @}*/
    }
    else
    {
        TEST_FAIL();
    }
}

TEST( Full_GGD, Jsoneq )
{
    BaseType_t xStatus;
    jsmn_parser xParser;
    int32_t lNbTokens;
    uint32_t ulJSONFileSize = strlen( cJSON_FILE );
    jsmntok_t pxTok[ ggdTestJSON_MAX_TOKENS ];


    if( TEST_PROTECT() )
    {
        /** @brief Prepare test.
         *  @{
         */
        jsmn_init( &xParser );

        /* From jsmn, parse the JSON file. */
        lNbTokens = ( int32_t ) jsmn_parse( &xParser,
                                            cJSON_FILE, /*lint !e971 can use char without signed/unsigned. */
                                            ( size_t ) ulJSONFileSize,
                                            pxTok,
                                            ( unsigned int ) ggdTestJSON_MAX_TOKENS ); /*lint !e961 redundant casting only when int = int32_t. */
        TEST_ASSERT_GREATER_THAN( 0, lNbTokens );
/** @}*/

/** @brief Check return status and value in ideal case.
 *  @{
 */
        xStatus = test_prvGGDJsoneq( cJSON_FILE,
                                     &pxTok[ ggdTestJSON_GROUPID_TOKEN_INDEX ],
                                     ggdJSON_FILE_GROUPID );
        TEST_ASSERT_EQUAL_INT32( pdPASS, xStatus );
        /** @}*/

        /** @brief Check return fail if another index is passed.
         *  @{
         */
        xStatus = test_prvGGDJsoneq( cJSON_FILE,
                                     &pxTok[ ggdTestJSON_GROUPID_TOKEN_INDEX + 1 ],
                                     ggdJSON_FILE_GROUPID );
        TEST_ASSERT_EQUAL_INT32( pdFAIL, xStatus );
        /** @}*/

        /* No need to check for stability here.
         * NULL pointers being passed would be a bug from higher levels.
         */
    }
    else
    {
        TEST_FAIL();
    }
}

TEST( Full_GGD, CheckForContentLengthString )
{
    BaseType_t xStatus;
    uint8_t ucIndex = 0;
    uint32_t ulI;
    char cHTTPContentString[] = ggdHTTP_CONTENT_LENGTH_STRING;

    if( TEST_PROTECT() )
    {
        /** @brief Check return status and value in ideal case.
         *  @{
         */
        for( ulI = 0;
             ulI < strlen( ggdHTTP_CONTENT_LENGTH_STRING ) - 1;
             ulI++ )
        {
            xStatus = test_prvCheckForContentLengthString( &ucIndex,
                                                           cHTTPContentString[ ulI ] );
            TEST_ASSERT_EQUAL_INT32( pdFAIL, xStatus );
            TEST_ASSERT_EQUAL_INT32( ulI + 1, ucIndex );
        }

        xStatus = test_prvCheckForContentLengthString( &ucIndex,
                                                       cHTTPContentString[ ulI ] );
        TEST_ASSERT_EQUAL_INT32( pdPASS, xStatus );
        /** @}*/

        /** @brief Check index start back to 0 when a wrong char is given.
         *  @{
         */
        ucIndex = 0;

        for( ulI = 0;
             ulI < strlen( ggdHTTP_CONTENT_LENGTH_STRING ) - 1;
             ulI++ )
        {
            xStatus = test_prvCheckForContentLengthString( &ucIndex,
                                                           cHTTPContentString[ ulI ] );
            TEST_ASSERT_EQUAL_INT32( pdFAIL, xStatus );
            TEST_ASSERT_EQUAL_INT32( ulI + 1, ucIndex );
        }

        xStatus = test_prvCheckForContentLengthString( &ucIndex,
                                                       '0' ); /* Send a random character. */
        TEST_ASSERT_EQUAL_INT32( pdFAIL, xStatus );
        TEST_ASSERT_EQUAL_INT32( 0, ucIndex );
        /** @}*/
    }
}

TEST( Full_GGD, JSONRequestGetFile )
{
    BaseType_t xStatus;
    uint32_t ulBufferSize = testrunnerBUFFER_SIZE;
    uint32_t ulByteRead;
    BaseType_t xJSONFileRetrieveCompleted;
    uint32_t ulJSONFileSize;

    if( TEST_PROTECT() )
    {
        /** @brief Check return status and value in ideal case.
         *  @{
         */
        xStatus = GGD_JSONRequestStart( &xSocket );

        if( xStatus == pdPASS )
        {
            xStatus = GGD_JSONRequestGetSize( &xSocket, &ulJSONFileSize );
        }

        if( xStatus == pdPASS )
        {
            ulByteRead = 0;
            xStatus = GGD_JSONRequestGetFile( &xSocket,
                                              cBuffer,
                                              ulBufferSize,
                                              &ulByteRead,
                                              &xJSONFileRetrieveCompleted,
                                              ulJSONFileSize );
        }

        TEST_ASSERT_EQUAL_INT32( SOCKETS_INVALID_SOCKET, xSocket );
        TEST_ASSERT_EQUAL_INT32( pdPASS, xStatus );
        TEST_ASSERT_EQUAL_INT32( pdTRUE, xJSONFileRetrieveCompleted );
        TEST_ASSERT_EQUAL_INT32( ulJSONFileSize, ulByteRead + 1 );

        /** @}*/

        /** @brief Retrieve the JSON file in two separate chunks.
         *  @{
         */
        xStatus = GGD_JSONRequestStart( &xSocket );

        if( xStatus == pdPASS )
        {
            xStatus = GGD_JSONRequestGetSize( &xSocket, &ulJSONFileSize );
        }

        if( xStatus == pdPASS )
        {
            ulByteRead = 0;
            xStatus = GGD_JSONRequestGetFile( &xSocket,
                                              cBuffer,
                                              ulJSONFileSize / 2,
                                              &ulByteRead,
                                              &xJSONFileRetrieveCompleted,
                                              ulJSONFileSize );


            TEST_ASSERT_EQUAL_INT32( pdPASS, xStatus );
            TEST_ASSERT_EQUAL_INT32( pdFALSE, xJSONFileRetrieveCompleted ); /* Not yet retrieved - only half of it. */

            xStatus = GGD_JSONRequestGetFile( &xSocket,
                                              &cBuffer[ ulByteRead ],
                                              ulBufferSize - ( ulJSONFileSize / 2 ),
                                              &ulByteRead,
                                              &xJSONFileRetrieveCompleted,
                                              ulJSONFileSize );
        }

        TEST_ASSERT_EQUAL_INT32_MESSAGE( pdPASS, xStatus, "GGD_JSONRequestGetFile() failed to return pdPASS." );
        TEST_ASSERT_EQUAL_INT32_MESSAGE( pdTRUE, xJSONFileRetrieveCompleted, "GGD_JSONRequestGetFile() return pdFALSE in xJSONFileRetrieveCompleted." );
        TEST_ASSERT_EQUAL_INT32_MESSAGE( ulJSONFileSize, ulByteRead + 1, "GGD_JSONRequestGetFile() returned ulJSONFileSize that is not ulByteRead + 1." );
        /** @}*/

        /** @brief Check fail if we receive more bytes than expected.
         *  @{
         */
        xStatus = GGD_JSONRequestStart( &xSocket );

        if( xStatus == pdPASS )
        {
            xStatus = GGD_JSONRequestGetSize( &xSocket, &ulJSONFileSize );
        }

        if( xStatus == pdPASS )
        {
            ulByteRead = 0;
            xStatus = GGD_JSONRequestGetFile( &xSocket,
                                              cBuffer,
                                              ulBufferSize,
                                              &ulByteRead,
                                              &xJSONFileRetrieveCompleted,
                                              ulJSONFileSize - 1 ); /* Remove one byte to the expected JSON file size. */
        }

        TEST_ASSERT_EQUAL_INT32( pdFAIL, xStatus );
        TEST_ASSERT_EQUAL_INT32( pdFAIL, xJSONFileRetrieveCompleted ); /* Failed. */
        /** @}*/
    }
    else
    {
        TEST_FAIL();
    }

    /** @brief Check Statility by passing in NULL pointers or buffers too small
     *  @{
     */
    if( TEST_PROTECT() )
    {
        ulByteRead = 0;
        xStatus = GGD_JSONRequestGetFile( &xSocket,
                                          cBuffer,
                                          ulBufferSize,
                                          NULL,
                                          &xJSONFileRetrieveCompleted,
                                          ulJSONFileSize );
        TEST_FAIL();
    }

    if( TEST_PROTECT() )
    {
        ulByteRead = 0;
        xStatus = GGD_JSONRequestGetFile( &xSocket,
                                          NULL,
                                          ulBufferSize,
                                          &ulByteRead,
                                          &xJSONFileRetrieveCompleted,
                                          ulJSONFileSize );
        TEST_FAIL();
    }

    if( TEST_PROTECT() )
    {
        ulByteRead = 0;
        xStatus = GGD_JSONRequestGetFile( &xSocket,
                                          cBuffer,
                                          ulBufferSize,
                                          &ulByteRead,
                                          NULL,
                                          ulJSONFileSize );
        TEST_FAIL();
    }

    /** @}*/
}

TEST( Full_GGD, JSONRequestGetSize )
{
    BaseType_t xStatus;
    uint32_t ulJSONFileSize;

    if( TEST_PROTECT() )
    {
        /** @brief Check return status and value in ideal case
         *  @{
         */
        xStatus = GGD_JSONRequestStart( &xSocket );
        TEST_ASSERT_EQUAL_INT32( pdPASS, xStatus );

        xStatus = GGD_JSONRequestGetSize( &xSocket, &ulJSONFileSize );
        TEST_ASSERT_GREATER_THAN( 1000, ulJSONFileSize );
        TEST_ASSERT_EQUAL_INT32( pdPASS, xStatus );
        GGD_SecureConnect_Disconnect( &xSocket );
        /* Check no disconnect after a pass. */
        /** @}*/
    }
    else
    {
        TEST_FAIL();
    }

    /** @brief Check Statility by passing in NULL pointers
     *  @{
     */
    if( TEST_PROTECT() )
    {
        xStatus = GGD_JSONRequestGetSize( &xSocket, NULL );
        TEST_FAIL();
    }

    /** @}*/
}

TEST( Full_GGD, JSONRequestStart )
{
    BaseType_t xStatus;

    if( TEST_PROTECT() )
    {
        /** @brief Check return status and value in ideal case
         *  @{
         */
        xStatus = GGD_JSONRequestStart( &xSocket );
        TEST_ASSERT_EQUAL_INT32( pdPASS, xStatus );
        GGD_SecureConnect_Disconnect( &xSocket );
        /** @}*/
    }
    else
    {
        TEST_FAIL();
    }

    /** @brief Check Statility by passing in NULL pointers
     *  @{
     */
    if( TEST_PROTECT() )
    {
        xStatus = GGD_JSONRequestStart( NULL );
        TEST_FAIL();
    }

    /** @}*/
}
