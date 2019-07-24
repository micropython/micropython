/*
 * Amazon FreeRTOS PKCS11 AFQP V1.1.4
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

/* Standard includes. */
#include <stdlib.h>
#include <string.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "FreeRTOSIPConfig.h"
#include "task.h"
#include "event_groups.h"

/* Crypto includes. */
#include "aws_crypto.h"
#include "aws_clientcredential.h"
#include "aws_pkcs11.h"
#include "aws_dev_mode_key_provisioning.h"
#include "aws_test_pkcs11_config.h"
#include "aws_pkcs11_config.h"

/* Test includes. */
#include "unity_fixture.h"
#include "unity.h"

/* mbedTLS includes. */
#include "mbedtls/sha256.h"
#include "mbedtls/x509_crt.h"

extern int convert_pem_to_der( const unsigned char * pucInput,
                               size_t xLen,
                               unsigned char * pucOutput,
                               size_t * pxOlen );

/*-----------------------------------------------------------*/
/*         Multitask sign-verify loop configuration.         */
/*-----------------------------------------------------------*/
/* Stack size of each task. This can be configured in aws_test_pkcs11_config.h. */
#ifndef pkcs11testSIGN_VERIFY_TASK_STACK_SIZE
    #define pkcs11testSIGN_VERIFY_TASK_STACK_SIZE    ( configMINIMAL_STACK_SIZE * 4 )
#endif

/* Priority of each task. This can be configured in aws_test_pkcs11_config.h. */
#ifndef pkcs11testSIGN_VERIFY_TASK_PRIORITY
    #define pkcs11testSIGN_VERIFY_TASK_PRIORITY    ( tskIDLE_PRIORITY )
#endif

/* Specifies bits for all tasks to the event group. */
#define pkcs11testALL_BITS    ( ( 1 << pkcs11testSIGN_VERIFY_TASK_COUNT ) - 1 )

/* Parameters to each task. */
typedef struct SignVerifyTaskParams
{
    BaseType_t xTaskNumber;
    CK_RV xTestResult;
} SignVerifyTaskParams_t;

/* Event group used to synchronize tasks. */
static EventGroupHandle_t xSyncEventGroup;



CK_SESSION_HANDLE xGlobalSession;
CK_FUNCTION_LIST_PTR pxGlobalFunctionList;

#define pkcs11testINVALID_SLOT_ID    -1


extern CK_RV xProvisionDevice( CK_SESSION_HANDLE xSession,
                               ProvisioningParams_t * xParams );

/*-----------------------------------------------------------*/
/*                Certificates used in tests.                */
/*-----------------------------------------------------------*/

/* Valid ECDSA certificate. */
static const char pcValidECDSACertificate[] =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIBVDCB+6ADAgECAgkAoJ9fIf9ayYswCgYIKoZIzj0EAwIwHTEbMBkGA1UEAwwS\n"
    "bm9ib2R5QG5vd2hlcmUuY29tMB4XDTE4MDMwODIyNDIzNFoXDTE5MDMwODIyNDIz\n"
    "NFowHTEbMBkGA1UEAwwSbm9ib2R5QG5vd2hlcmUuY29tMFkwEwYHKoZIzj0CAQYI\n"
    "KoZIzj0DAQcDQgAExYJLN8zYGzFieyF4ONxk2BCY8I4fQTpfOJyWSYCqat83xtNE\n"
    "KoOxB33FP22MJXn7rj24YILu1uRLrFkH2pcRkKMkMCIwCwYDVR0PBAQDAgeAMBMG\n"
    "A1UdJQQMMAoGCCsGAQUFBwMDMAoGCCqGSM49BAMCA0gAMEUCIQDhXUT02TsIlzBe\n"
    "Aw9pLCowZ+6dPY1igspplUqZcuDAKQIgN6j5s7x5AudklULRuFyBQBlkVR35IdWs\n"
    "zu/xp2COg9g=\n"
    "-----END CERTIFICATE-----";

/* This ECDSA certificate is malformed. The first 'M' has been replaced with a 'Z'. */
static const char pcInvalidECDSACertificate[] =
    "-----BEGIN CERTIFICATE-----\n"
    "ZIIBVDCB+6ADAgECAgkAoJ9fIf9ayYswCgYIKoZIzj0EAwIwHTEbMBkGA1UEAwwS\n"
    "bm9ib2R5QG5vd2hlcmUuY29tMB4XDTE4MDMwODIyNDIzNFoXDTE5MDMwODIyNDIz\n"
    "NFowHTEbMBkGA1UEAwwSbm9ib2R5QG5vd2hlcmUuY29tMFkwEwYHKoZIzj0CAQYI\n"
    "KoZIzj0DAQcDQgAExYJLN8zYGzFieyF4ONxk2BCY8I4fQTpfOJyWSYCqat83xtNE\n"
    "KoOxB33FP22MJXn7rj24YILu1uRLrFkH2pcRkKMkMCIwCwYDVR0PBAQDAgeAMBMG\n"
    "A1UdJQQMMAoGCCsGAQUFBwMDMAoGCCqGSM49BAMCA0gAMEUCIQDhXUT02TsIlzBe\n"
    "Aw9pLCowZ+6dPY1igspplUqZcuDAKQIgN6j5s7x5AudklULRuFyBQBlkVR35IdWs\n"
    "zu/xp2COg9g=\n"
    "-----END CERTIFICATE-----";

/* Valid ECDSA private key. */
static const char pcValidECDSAPrivateKey[] =
    "-----BEGIN PRIVATE KEY-----\n"
    "MIGHAgEAMBMGByqGSM49AgEGCCqGSM49AwEHBG0wawIBAQQg8gHhd5ELAooWRQls\n"
    "PfpcQREiLrvEb3oLioicMYdUmrmhRANCAATFgks3zNgbMWJ7IXg43GTYEJjwjh9B\n"
    "Ol84nJZJgKpq3zfG00Qqg7EHfcU/bYwlefuuPbhggu7W5EusWQfalxGQ\n"
    "-----END PRIVATE KEY-----";

/* This ECDSA private key is malformed. The first 'M' has been replaced with a 'Z'. */
static const char pcInvalidECDSAPrivateKey[] =
    "-----BEGIN PRIVATE KEY-----\n"
    "ZIGHAgEAMBMGByqGSM49AgEGCCqGSM49AwEHBG0wawIBAQQg8gHhd5ELAooWRQls\n"
    "PfpcQREiLrvEb3oLioicMYdUmrmhRANCAATFgks3zNgbMWJ7IXg43GTYEJjwjh9B\n"
    "Ol84nJZJgKpq3zfG00Qqg7EHfcU/bYwlefuuPbhggu7W5EusWQfalxGQ\n"
    "-----END PRIVATE KEY-----";

/* This is the public key that pairs with pcValidECDSAPrivateKey. */
static const char pcValidECDSAPublicKey[] =
    "-----BEGIN PUBLIC KEY-----\n"
    "MFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAExYJLN8zYGzFieyF4ONxk2BCY8I4f\n"
    "QTpfOJyWSYCqat83xtNEKoOxB33FP22MJXn7rj24YILu1uRLrFkH2pcRkA==\n"
    "-----END PUBLIC KEY-----";

/* This ECDSA public key malformed. The first 'M' has been replaced with a 'Z'. */
static const char pcInvalidECDSAPublicKey[] =
    "-----BEGIN PUBLIC KEY-----\n"
    "ZFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAExYJLN8zYGzFieyF4ONxk2BCY8I4f\n"
    "QTpfOJyWSYCqat83xtNEKoOxB33FP22MJXn7rj24YILu1uRLrFkH2pcRkA==\n"
    "-----END PUBLIC KEY-----";

/* This is a valid public key, but isn't paired with pcValidECDSAPrivateKey. */
static const char pcWrongECDSAPublicKey[] =
    "-----BEGIN PUBLIC KEY-----\n"
    "MFYwEAYHKoZIzj0CAQYFK4EEAAoDQgAEWp505KT6CprrMbGH/j3POjmbuPPIG6W7\n"
    "Dz/Yy4EF6EkfJ+cdPurX7BkKr9ZZ12u1nbNp/BQNQULv8pcdx+jd7Q==\n"
    "-----END PUBLIC KEY-----";

/* Valid RSA certificate. */
static const char pcValidRSACertificate[] =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIDsTCCApmgAwIBAgIJALg4YJlPspxyMA0GCSqGSIb3DQEBCwUAMG8xCzAJBgNV\n"
    "BAYTAlVTMQswCQYDVQQIDAJXQTEQMA4GA1UEBwwHU2VhdHRsZTENMAsGA1UECgwE\n"
    "QW16bjEMMAoGA1UECwwDSW9UMQ0wCwYDVQQDDARUZXN0MRUwEwYJKoZIhvcNAQkB\n"
    "FgZub2JvZHkwHhcNMTgwNjExMTk0NjM2WhcNMjEwMzMxMTk0NjM2WjBvMQswCQYD\n"
    "VQQGEwJVUzELMAkGA1UECAwCV0ExEDAOBgNVBAcMB1NlYXR0bGUxDTALBgNVBAoM\n"
    "BEFtem4xDDAKBgNVBAsMA0lvVDENMAsGA1UEAwwEVGVzdDEVMBMGCSqGSIb3DQEJ\n"
    "ARYGbm9ib2R5MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAsIqRecRx\n"
    "Lz3PZXzZOHF7jMlB25tfv2LDGR7nGTJiey5zxd7oswihe7+26yx8medpNvX1ym9j\n"
    "phty+9IR053k1WGnQQ4aaDeJonqn7V50Vesw6zFx/x8LMdXFoBAkRXIL8WS5YKaf\n"
    "C87KPnye8A0piVWUFy7+IEEaK3hQEJTzB6LC/N100XL5ykLCa4xJBOqlIvbDvJ+b\n"
    "Kty1EBA3sStlTNuXi3nBWZbXwCB2A+ddjijFf5+gUjinr7h6e2uQeipWyiIw9NKW\n"
    "bvq8AG1Mj4XBoFL9wP2YTf2SQAgAzx0ySPNrIYOzBNl1YZ4lIW5sJLATES9+Z8nH\n"
    "i7yRDLw6x/kcVQIDAQABo1AwTjAdBgNVHQ4EFgQUHc4PjEL0CaxZ+1D/4VdeDjxt\n"
    "JO8wHwYDVR0jBBgwFoAUHc4PjEL0CaxZ+1D/4VdeDjxtJO8wDAYDVR0TBAUwAwEB\n"
    "/zANBgkqhkiG9w0BAQsFAAOCAQEAi1/okTpQuPcaQEBgepccZ/Lt/gEQNdGcbsYQ\n"
    "3aEABNVYL8dYOW9r/8l074zD+vi9iSli/yYmwRFD0baN1FRWUqkVEIQ+3yfivOW9\n"
    "R282NuQvEULgERC2KN7vm0vO+DF7ay58qm4PaAGHdQco1LaHKkljMPLHF841facG\n"
    "M9KVtzFveOQKkWvb4VgOyfn7aCnEogGlWt1S0d12pBRwYjJgKrVQaGs6IiGFVtm8\n"
    "JRLZrLL3sfgsN7L1xu//JUoTOkgxdKuYRmPuUdV2hw/VYDzcnKj7/DMXNDvgl3s7\n"
    "5GC4F+8LFLzRrZJWs18FMLaCE+zJChw/oeSt+RS0JZDFn+uX9Q==\n"
    "-----END CERTIFICATE-----\n";

/* This RSA certificate is malformed. The first 'M' has been replaced with a 'Z'. */
static const char pcInvalidRSACertificate[] =
    "-----BEGIN CERTIFICATE-----\n"
    "ZIIDsTCCApmgAwIBAgIJALg4YJlPspxyMA0GCSqGSIb3DQEBCwUAMG8xCzAJBgNV\n"
    "BAYTAlVTMQswCQYDVQQIDAJXQTEQMA4GA1UEBwwHU2VhdHRsZTENMAsGA1UECgwE\n"
    "QW16bjEMMAoGA1UECwwDSW9UMQ0wCwYDVQQDDARUZXN0MRUwEwYJKoZIhvcNAQkB\n"
    "FgZub2JvZHkwHhcNMTgwNjExMTk0NjM2WhcNMjEwMzMxMTk0NjM2WjBvMQswCQYD\n"
    "VQQGEwJVUzELMAkGA1UECAwCV0ExEDAOBgNVBAcMB1NlYXR0bGUxDTALBgNVBAoM\n"
    "BEFtem4xDDAKBgNVBAsMA0lvVDENMAsGA1UEAwwEVGVzdDEVMBMGCSqGSIb3DQEJ\n"
    "ARYGbm9ib2R5MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAsIqRecRx\n"
    "Lz3PZXzZOHF7jMlB25tfv2LDGR7nGTJiey5zxd7oswihe7+26yx8medpNvX1ym9j\n"
    "phty+9IR053k1WGnQQ4aaDeJonqn7V50Vesw6zFx/x8LMdXFoBAkRXIL8WS5YKaf\n"
    "C87KPnye8A0piVWUFy7+IEEaK3hQEJTzB6LC/N100XL5ykLCa4xJBOqlIvbDvJ+b\n"
    "Kty1EBA3sStlTNuXi3nBWZbXwCB2A+ddjijFf5+gUjinr7h6e2uQeipWyiIw9NKW\n"
    "bvq8AG1Mj4XBoFL9wP2YTf2SQAgAzx0ySPNrIYOzBNl1YZ4lIW5sJLATES9+Z8nH\n"
    "i7yRDLw6x/kcVQIDAQABo1AwTjAdBgNVHQ4EFgQUHc4PjEL0CaxZ+1D/4VdeDjxt\n"
    "JO8wHwYDVR0jBBgwFoAUHc4PjEL0CaxZ+1D/4VdeDjxtJO8wDAYDVR0TBAUwAwEB\n"
    "/zANBgkqhkiG9w0BAQsFAAOCAQEAi1/okTpQuPcaQEBgepccZ/Lt/gEQNdGcbsYQ\n"
    "3aEABNVYL8dYOW9r/8l074zD+vi9iSli/yYmwRFD0baN1FRWUqkVEIQ+3yfivOW9\n"
    "R282NuQvEULgERC2KN7vm0vO+DF7ay58qm4PaAGHdQco1LaHKkljMPLHF841facG\n"
    "M9KVtzFveOQKkWvb4VgOyfn7aCnEogGlWt1S0d12pBRwYjJgKrVQaGs6IiGFVtm8\n"
    "JRLZrLL3sfgsN7L1xu//JUoTOkgxdKuYRmPuUdV2hw/VYDzcnKj7/DMXNDvgl3s7\n"
    "5GC4F+8LFLzRrZJWs18FMLaCE+zJChw/oeSt+RS0JZDFn+uX9Q==\n"
    "-----END CERTIFICATE-----\n";

/* Valid RSA private key. */
static const char pcValidRSAPrivateKey[] =
    "-----BEGIN RSA PRIVATE KEY-----\n"
    "MIIEpAIBAAKCAQEAsIqRecRxLz3PZXzZOHF7jMlB25tfv2LDGR7nGTJiey5zxd7o\n"
    "swihe7+26yx8medpNvX1ym9jphty+9IR053k1WGnQQ4aaDeJonqn7V50Vesw6zFx\n"
    "/x8LMdXFoBAkRXIL8WS5YKafC87KPnye8A0piVWUFy7+IEEaK3hQEJTzB6LC/N10\n"
    "0XL5ykLCa4xJBOqlIvbDvJ+bKty1EBA3sStlTNuXi3nBWZbXwCB2A+ddjijFf5+g\n"
    "Ujinr7h6e2uQeipWyiIw9NKWbvq8AG1Mj4XBoFL9wP2YTf2SQAgAzx0ySPNrIYOz\n"
    "BNl1YZ4lIW5sJLATES9+Z8nHi7yRDLw6x/kcVQIDAQABAoIBADd+h3ZIeu/HtT8I\n"
    "vNuSSK0bwpj+wV1O9VcbMLfp760bEAd+J5XHu8NDo4NPi6dxZ9CABpBo7WEUtdNU\n"
    "2Ie11W4B8WpwvXpPIvOxLMJf85/ie5EjDNuObZ1vvlyvVkeCLyDlcaRhHBPBIC/+\n"
    "SpPY/1qNTSzwd6+55zkM69YajD60tFv8WuCsgkAteCoDjcqwDcowyAy4pILhOYaW\n"
    "1x+0ZPMUqwtld+06ct/yqBPB8C9IH7ZIeJr5e58R9SxytbuTwTN4jceOoeD5MBbG\n"
    "A+A0WxGdQ8N+kwWkz77qDbZfP4G8wNxeUXobnfqfDGqb0O5zeEaU7EI+mlEQH58Z\n"
    "B1edj6ECgYEA3rldciCQc4t2qYKiZSffnl7Pg7L+ojzH/Eam4Dlk27+DqOo70MnX\n"
    "LVWUWkLOMQ27dRSBQsUDUaqVRZLkcFKc6C8k2cIpPBMpA9WdZVd9kFawZ8lJ7jko\n"
    "qTbJxnDxvhdHrZRvLRjEenbdNXdAGy2EuqvThUJgPEybLAWg6sE3LB0CgYEAyurT\n"
    "14h4BGEGBpl2KevoPQ4PPS+IoDXMS29mtfcascVkYcxxW1v8wOQVyD4VrsRMErck\n"
    "ZMpu2evd+aQSPSrAod/sQ20C+wCCA7ipBlhAUeuS/FpqFIZWkHzZnVccp8O3nOFO\n"
    "KNeAmw4udq8PyjVVouey/6F386itJdxWt/d8i5kCgYA3Aru045wqHck6RvzLVVTj\n"
    "LfG9Sqmf8rlGc0DmYuapbB0dzHTnteLC3L9ep997uDOT0HO4xSZztllWLNjlcVI1\n"
    "+ub0LgO3Rdg8jTdp/3kQ/IhnqgzrnQyQ9upRbDYZSHC4y8/F6LcmtFMg0Ipx7AU7\n"
    "ghMld+aDHjy5W86KDR0OdQKBgQCAZoPSONqo+rQTbPwmns6AA+uErhVoO2KgwUdf\n"
    "EZPktaFFeVapltWjQTC/WvnhcvkoRpdS5/2pC+WUWEvqRKlMRSN9rvdZ2QJsVGcw\n"
    "Spu4urZx1MyXXEJef4I8W6kYR3JiZPdORL9uXlTsaO425/Tednr/4y7CEhQuhvSg\n"
    "yIwY0QKBgQC2NtKDOwcgFykKRYqtHuo6VpSeLmgm1DjlcAuaGJsblX7C07ZH8Tjm\n"
    "IHQb01oThNEa4tC0vO3518PkQwvyi/TWGHm9SLYdXvpVnBwkk5yRioKPgPmrs4Xi\n"
    "ERIYrvveGGtQ3vSknLWUJ/0BgmuYj5U6aJBZPv8COM2eKIbTQbtQaQ==\n"
    "-----END RSA PRIVATE KEY-----\n";

/* This RSA private key is malformed. The first 'M' has been replaced with a 'Z'. */
static const char pcInvalidRSAPrivateKey[] =
    "-----BEGIN RSA PRIVATE KEY-----\n"
    "ZIIEpAIBAAKCAQEAsIqRecRxLz3PZXzZOHF7jMlB25tfv2LDGR7nGTJiey5zxd7o\n"
    "swihe7+26yx8medpNvX1ym9jphty+9IR053k1WGnQQ4aaDeJonqn7V50Vesw6zFx\n"
    "/x8LMdXFoBAkRXIL8WS5YKafC87KPnye8A0piVWUFy7+IEEaK3hQEJTzB6LC/N10\n"
    "0XL5ykLCa4xJBOqlIvbDvJ+bKty1EBA3sStlTNuXi3nBWZbXwCB2A+ddjijFf5+g\n"
    "Ujinr7h6e2uQeipWyiIw9NKWbvq8AG1Mj4XBoFL9wP2YTf2SQAgAzx0ySPNrIYOz\n"
    "BNl1YZ4lIW5sJLATES9+Z8nHi7yRDLw6x/kcVQIDAQABAoIBADd+h3ZIeu/HtT8I\n"
    "vNuSSK0bwpj+wV1O9VcbMLfp760bEAd+J5XHu8NDo4NPi6dxZ9CABpBo7WEUtdNU\n"
    "2Ie11W4B8WpwvXpPIvOxLMJf85/ie5EjDNuObZ1vvlyvVkeCLyDlcaRhHBPBIC/+\n"
    "SpPY/1qNTSzwd6+55zkM69YajD60tFv8WuCsgkAteCoDjcqwDcowyAy4pILhOYaW\n"
    "1x+0ZPMUqwtld+06ct/yqBPB8C9IH7ZIeJr5e58R9SxytbuTwTN4jceOoeD5MBbG\n"
    "A+A0WxGdQ8N+kwWkz77qDbZfP4G8wNxeUXobnfqfDGqb0O5zeEaU7EI+mlEQH58Z\n"
    "B1edj6ECgYEA3rldciCQc4t2qYKiZSffnl7Pg7L+ojzH/Eam4Dlk27+DqOo70MnX\n"
    "LVWUWkLOMQ27dRSBQsUDUaqVRZLkcFKc6C8k2cIpPBMpA9WdZVd9kFawZ8lJ7jko\n"
    "qTbJxnDxvhdHrZRvLRjEenbdNXdAGy2EuqvThUJgPEybLAWg6sE3LB0CgYEAyurT\n"
    "14h4BGEGBpl2KevoPQ4PPS+IoDXMS29mtfcascVkYcxxW1v8wOQVyD4VrsRMErck\n"
    "ZMpu2evd+aQSPSrAod/sQ20C+wCCA7ipBlhAUeuS/FpqFIZWkHzZnVccp8O3nOFO\n"
    "KNeAmw4udq8PyjVVouey/6F386itJdxWt/d8i5kCgYA3Aru045wqHck6RvzLVVTj\n"
    "LfG9Sqmf8rlGc0DmYuapbB0dzHTnteLC3L9ep997uDOT0HO4xSZztllWLNjlcVI1\n"
    "+ub0LgO3Rdg8jTdp/3kQ/IhnqgzrnQyQ9upRbDYZSHC4y8/F6LcmtFMg0Ipx7AU7\n"
    "ghMld+aDHjy5W86KDR0OdQKBgQCAZoPSONqo+rQTbPwmns6AA+uErhVoO2KgwUdf\n"
    "EZPktaFFeVapltWjQTC/WvnhcvkoRpdS5/2pC+WUWEvqRKlMRSN9rvdZ2QJsVGcw\n"
    "Spu4urZx1MyXXEJef4I8W6kYR3JiZPdORL9uXlTsaO425/Tednr/4y7CEhQuhvSg\n"
    "yIwY0QKBgQC2NtKDOwcgFykKRYqtHuo6VpSeLmgm1DjlcAuaGJsblX7C07ZH8Tjm\n"
    "IHQb01oThNEa4tC0vO3518PkQwvyi/TWGHm9SLYdXvpVnBwkk5yRioKPgPmrs4Xi\n"
    "ERIYrvveGGtQ3vSknLWUJ/0BgmuYj5U6aJBZPv8COM2eKIbTQbtQaQ==\n"
    "-----END RSA PRIVATE KEY-----\n";

/* This is the public key that pairs with pcValidRSAPrivateKey. */
static const char pcValidRSAPublicKey[] =
    "-----BEGIN PUBLIC KEY-----\n"
    "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAsIqRecRxLz3PZXzZOHF7\n"
    "jMlB25tfv2LDGR7nGTJiey5zxd7oswihe7+26yx8medpNvX1ym9jphty+9IR053k\n"
    "1WGnQQ4aaDeJonqn7V50Vesw6zFx/x8LMdXFoBAkRXIL8WS5YKafC87KPnye8A0p\n"
    "iVWUFy7+IEEaK3hQEJTzB6LC/N100XL5ykLCa4xJBOqlIvbDvJ+bKty1EBA3sStl\n"
    "TNuXi3nBWZbXwCB2A+ddjijFf5+gUjinr7h6e2uQeipWyiIw9NKWbvq8AG1Mj4XB\n"
    "oFL9wP2YTf2SQAgAzx0ySPNrIYOzBNl1YZ4lIW5sJLATES9+Z8nHi7yRDLw6x/kc\n"
    "VQIDAQAB\n"
    "-----END PUBLIC KEY-----\n";

/* This RSA public key malformed. The first 'M' has been replaced with a 'Z'. */
static const char pcInvalidRSAPublicKey[] =
    "-----BEGIN PUBLIC KEY-----\n"
    "ZIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAsIqRecRxLz3PZXzZOHF7\n"
    "jMlB25tfv2LDGR7nGTJiey5zxd7oswihe7+26yx8medpNvX1ym9jphty+9IR053k\n"
    "1WGnQQ4aaDeJonqn7V50Vesw6zFx/x8LMdXFoBAkRXIL8WS5YKafC87KPnye8A0p\n"
    "iVWUFy7+IEEaK3hQEJTzB6LC/N100XL5ykLCa4xJBOqlIvbDvJ+bKty1EBA3sStl\n"
    "TNuXi3nBWZbXwCB2A+ddjijFf5+gUjinr7h6e2uQeipWyiIw9NKWbvq8AG1Mj4XB\n"
    "oFL9wP2YTf2SQAgAzx0ySPNrIYOzBNl1YZ4lIW5sJLATES9+Z8nHi7yRDLw6x/kc\n"
    "VQIDAQAB\n"
    "-----END PUBLIC KEY-----\n";

/* This is a valid public key, but isn't paired with pcValidRSAPrivateKey. */
static const char pcWrongRSAPublicKey[] =
    "-----BEGIN PUBLIC KEY-----\n"
    "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAmpfMnNQkv0yEQM3LPAy5\n"
    "LQwT0OL6hYkxIW05POB5hYifE/RIjeu+e25xFbEpXC7p9rFNrkn2C3gQE2X3Qjqy\n"
    "e8eQFpvO7Y+g64kn3DiHsTwuKf57Cyid/dAAFnHMixXTAArH7OwVnSj3oz18GM9x\n"
    "3Zx8nLA92yuclr0Z1LTZwiUBD7loTsA6X+e+RsYhQBMDMSfxflGGqOLmX4tPnE8+\n"
    "PsU0w9GeuHalW4CsVFQ2Sm7jL2gtrEQpGDSwzDmdqrSgY92NNrbjwFMd1lLSwQwP\n"
    "tlznpxGyGPkuetZWkVbaJ3wbUwxf1y+PXjYx1uwULrVeZR+JSrWeiez3DVHy2lRh\n"
    "WQIDAQAB\n"
    "-----END PUBLIC KEY-----";

/*-----------------------------------------------------------*/

/* The SHA-256 hash of the 0 byte "\0" (data). Calculated with OpenSSL 1.0.2g.
 *
 * Command to generate the same are:
 * - Generate empty file: touch file.txt
 * - Generate hash: openssl dgst -sha256 -binary file.txt > hash
 * - Generate output to paste below: xxd --include hash
 */
static const char pcNullByteHash[] =
{
    0xe3, 0xb0, 0xc4, 0x42, 0x98, 0xfc, 0x1c, 0x14, 0x9a, 0xfb, 0xf4, 0xc8,
    0x99, 0x6f, 0xb9, 0x24, 0x27, 0xae, 0x41, 0xe4, 0x64, 0x9b, 0x93, 0x4c,
    0xa4, 0x95, 0x99, 0x1b, 0x78, 0x52, 0xb8, 0x55
};

/* The above hash signed with pcValidPrivateKey using OpenSSL 1.0.2g.
 *
 * Commands to generate the same are:
 * - Generate the signature:  openssl dgst -sha256 -sign validRSAKey.pem -out signature file.txt
 * [where validRSAKey.pem file contains the same key as pcValidRSAPrivateKey above].
 * - Generate the output to copy below: xxd --include signature
 */
static const char pcSignedData[] =
{
    0x25, 0x99, 0x4b, 0x16, 0xa3, 0x2c, 0x0c, 0x42, 0xad, 0x26, 0x02, 0x75,
    0x34, 0x89, 0xd9, 0x1e, 0xa2, 0x8b, 0x2b, 0xc2, 0xf5, 0xc7, 0x70, 0x28,
    0xed, 0xe3, 0xa4, 0x27, 0x63, 0x31, 0xe5, 0x61, 0x92, 0xf6, 0x88, 0x59,
    0xd8, 0x79, 0x08, 0xc2, 0x3b, 0x12, 0xd7, 0x4f, 0xd4, 0x35, 0x5e, 0x50,
    0x77, 0x1f, 0xca, 0x4e, 0xa2, 0x76, 0xfb, 0x8f, 0xd4, 0xe5, 0xff, 0xcf,
    0xd7, 0x65, 0x94, 0xff, 0x8c, 0xdd, 0x33, 0x23, 0x81, 0xcb, 0x80, 0xb4,
    0xb7, 0x39, 0x83, 0xd9, 0x8e, 0x2c, 0x4c, 0x96, 0x28, 0x4f, 0x3c, 0x85,
    0xc9, 0x34, 0x77, 0x0e, 0xae, 0x5b, 0x6c, 0x44, 0x83, 0xc8, 0xe7, 0x60,
    0x96, 0x82, 0x7e, 0x4d, 0x01, 0xf9, 0xb8, 0x54, 0xd0, 0xb4, 0x8e, 0x6b,
    0x2c, 0x69, 0xd8, 0x1a, 0x56, 0x82, 0x25, 0x3e, 0x88, 0xbe, 0x5f, 0xb2,
    0xa3, 0xf1, 0x14, 0x52, 0xe1, 0x99, 0x67, 0x95, 0xc7, 0xc7, 0xbb, 0x7b,
    0x1a, 0x45, 0xff, 0x62, 0x3f, 0x21, 0x46, 0xcd, 0x79, 0xae, 0x8c, 0xbe,
    0x84, 0x2c, 0x54, 0x60, 0x72, 0x39, 0xd4, 0x28, 0x51, 0xc8, 0xba, 0xd6,
    0x8c, 0x94, 0x75, 0xed, 0xeb, 0xa2, 0x3d, 0xb5, 0x0a, 0x8a, 0xae, 0xd5,
    0x48, 0xcc, 0xc6, 0xbe, 0xf0, 0xdc, 0x86, 0x4a, 0x8c, 0xf2, 0xb2, 0x8a,
    0x14, 0x06, 0xc8, 0x59, 0x9d, 0x4b, 0xb4, 0x7f, 0xbd, 0x0f, 0xe5, 0xdd,
    0x79, 0xa6, 0xad, 0xc5, 0x2d, 0xb0, 0x3b, 0x3b, 0x36, 0xc8, 0x63, 0xcc,
    0xca, 0x3b, 0x0c, 0x4e, 0xde, 0xd2, 0x8a, 0xe3, 0x44, 0xd5, 0x5e, 0xdd,
    0x1a, 0x56, 0xa8, 0x71, 0xf8, 0xed, 0x1f, 0x06, 0x40, 0xbb, 0xab, 0x2a,
    0xaa, 0x87, 0xe5, 0x96, 0x0c, 0x64, 0x6c, 0x76, 0x4b, 0xa3, 0x0a, 0x89,
    0x05, 0x4c, 0x79, 0x71, 0x41, 0x57, 0xad, 0x5e, 0x9c, 0x95, 0xcc, 0x2d,
    0x76, 0x62, 0x7f, 0xdc
};


/*-----------------------------------------------------------*/

static CK_RV prvReprovision( const char * const pcClientCertificate,
                             const char * const pcClientPrivateKey,
                             uint32_t ulClientPrivateKeyType )
{
    ProvisioningParams_t xParams;
    CK_RV xResult = CKR_OK;
    int32_t lConverted = 0;

    /* Since PEM is larger than DER, these buffers should always be sufficient. */
    xParams.pcClientCertificate = ( uint8_t * ) pvPortMalloc( strlen( pcClientCertificate ) );
    xParams.pcClientPrivateKey = ( uint8_t * ) pvPortMalloc( strlen( pcClientPrivateKey ) );

    xParams.ulClientCertificateLength = strlen( pcClientCertificate );
    xParams.ulClientPrivateKeyLength = strlen( pcClientPrivateKey );

    xParams.ulClientPrivateKeyType = ulClientPrivateKeyType;

    if( ( xParams.pcClientCertificate != NULL ) && ( xParams.pcClientPrivateKey != NULL ) )
    {
        /* Convert PEM keys and certs to DER. */
        lConverted = convert_pem_to_der( ( const unsigned char * ) pcClientCertificate,
                                         strlen( pcClientCertificate ),
                                         xParams.pcClientCertificate,
                                         ( size_t * ) &( xParams.ulClientCertificateLength ) );

        if( lConverted != 0 )
        {
            xResult = CKR_ATTRIBUTE_VALUE_INVALID;
            configPRINTF( ( "ERROR: Failed to convert certificate to DER format." ) );
        }

        lConverted = convert_pem_to_der( ( const unsigned char * ) pcClientPrivateKey,
                                         strlen( pcClientPrivateKey ),
                                         ( unsigned char * ) xParams.pcClientPrivateKey,
                                         ( size_t * ) &( xParams.ulClientPrivateKeyLength ) );

        if( lConverted != 0 )
        {
            xResult = CKR_ATTRIBUTE_VALUE_INVALID;
            configPRINTF( ( "ERROR: Failed to convert private key to DER format." ) );
        }
    }

    if( xResult == CKR_OK )
    {
        /* Provision using alternate cert and key. */
        xResult = xProvisionDevice( xGlobalSession, &xParams );
    }

    /* Free the memory malloc'ed for key and cert. */
    if( xParams.pcClientCertificate != NULL )
    {
        vPortFree( xParams.pcClientCertificate );
    }

    if( xParams.pcClientPrivateKey != NULL )
    {
        vPortFree( xParams.pcClientPrivateKey );
    }

    return xResult;
}

/*-----------------------------------------------------------*/

static CK_RV prvInitializeAndStartSession( CK_FUNCTION_LIST_PTR * ppxFunctionList,
                                           CK_SLOT_ID * pxSlotId,
                                           CK_SESSION_HANDLE * pxSession )
{
    CK_RV xResult = 0;
    CK_C_GetFunctionList pxCkGetFunctionList = NULL;
    CK_ULONG ulCount = 1;

    /* Ensure that the PKCS#11 module is initialized. */
    if( 0 == xResult )
    {
        pxCkGetFunctionList = C_GetFunctionList;
        xResult = pxCkGetFunctionList( ppxFunctionList );
    }

    if( 0 == xResult )
    {
        xResult = ( *ppxFunctionList )->C_Initialize( NULL );
    }

    /* Get the default private key storage ID. */
    if( ( 0 == xResult ) || ( CKR_CRYPTOKI_ALREADY_INITIALIZED == xResult ) )
    {
        xResult = ( *ppxFunctionList )->C_GetSlotList( CK_TRUE,
                                                       pxSlotId,
                                                       &ulCount );
    }

    /* Start a private session with the PKCS#11 module. */
    if( 0 == xResult )
    {
        xResult = ( *ppxFunctionList )->C_OpenSession( *pxSlotId,
                                                       CKF_SERIAL_SESSION,
                                                       NULL,
                                                       NULL,
                                                       pxSession );
    }

    return xResult;
}


/*-----------------------------------------------------------*/

static CK_RV prvGetPrivateKeyHandle( CK_FUNCTION_LIST_PTR pxFunctionList,
                                     CK_SESSION_HANDLE xSession,
                                     CK_OBJECT_HANDLE_PTR pxPrivateKey )
{
    CK_ATTRIBUTE xTemplate;
    CK_ULONG ulCount;
    CK_RV xResult = CKR_OK;

    /* Set up the template for the private key handle. */
    xTemplate.type = CKA_LABEL;
    xTemplate.ulValueLen = sizeof( pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS );
    xTemplate.pValue = &pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS;
    xResult = pxFunctionList->C_FindObjectsInit( xSession, &xTemplate, 1 );

    if( CKR_OK == xResult )
    {
        xResult = pxFunctionList->C_FindObjects( xSession,
                                                 pxPrivateKey,
                                                 1,
                                                 &ulCount );
    }

    if( CKR_OK == xResult )
    {
        xResult = pxFunctionList->C_FindObjectsFinal( xSession );
    }

    return xResult;
}

/*-----------------------------------------------------------*/

static CK_RV prvGetCertificateHandle( CK_FUNCTION_LIST_PTR pxFunctionList,
                                      CK_SESSION_HANDLE xSession,
                                      CK_OBJECT_HANDLE_PTR pxCertHandle )
{
    CK_ATTRIBUTE xTemplate;
    CK_RV xResult = CKR_OK;
    CK_ULONG ulCount = 0;

    /* Get the certificate handle. */
    if( 0 == xResult )
    {
        xTemplate.type = CKA_LABEL;
        xTemplate.ulValueLen = sizeof( pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS );
        xTemplate.pValue = &pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS;
        xResult = pxFunctionList->C_FindObjectsInit( xSession, &xTemplate, 1 );
    }

    if( 0 == xResult )
    {
        xResult = pxFunctionList->C_FindObjects( xSession,
                                                 ( CK_OBJECT_HANDLE_PTR ) &pxCertHandle,
                                                 1,
                                                 &ulCount );
    }

    if( 0 == xResult )
    {
        xResult = pxFunctionList->C_FindObjectsFinal( xSession );
    }

    return xResult;
}

/*-----------------------------------------------------------*/

static CK_RV prvImportPublicKey( CK_SESSION_HANDLE xSession,
                                 CK_FUNCTION_LIST_PTR pxFunctionList,
                                 CK_OBJECT_HANDLE_PTR pxPublicKeyHandle,
                                 const char * const pcPublicKey )
{
    CK_RV xResult = CKR_OK;
    CK_OBJECT_CLASS xPublicKeyClass = CKO_PUBLIC_KEY;

    /* It doesn't matter what key type is passed. The key type will be automatically
     * identified. */
    CK_KEY_TYPE xKeyType = CKK_RSA;

    size_t xPublicKeyBufferLength = strlen( pcPublicKey );
    uint8_t * pucPublicKeyDer = pvPortMalloc( xPublicKeyBufferLength );

    if( pucPublicKeyDer == NULL )
    {
        xResult = CKR_HOST_MEMORY;
    }

    if( xResult == CKR_OK )
    {
        /* Convert the key from PEM to DER */
        xResult = convert_pem_to_der( ( const unsigned char * ) pcPublicKey,
                                      strlen( pcPublicKey ),
                                      pucPublicKeyDer,
                                      &xPublicKeyBufferLength );
    }

    /* Public key configuration template. */
    CK_ATTRIBUTE xPublicKeyTemplate[] =
    {
        { CKA_CLASS,    &xPublicKeyClass,                         sizeof( xPublicKeyClass )                         },
        { CKA_KEY_TYPE, &xKeyType,                                sizeof( xKeyType )                                },
        { CKA_LABEL,    &pkcs11configLABEL_CODE_VERIFICATION_KEY, sizeof( pkcs11configLABEL_CODE_VERIFICATION_KEY ) },
        { CKA_VALUE,    ( CK_VOID_PTR ) pucPublicKeyDer,          ( CK_ULONG ) xPublicKeyBufferLength              }
    };

    /* Create an object using the public key. */
    if( CKR_OK == xResult )
    {
        xResult = pxFunctionList->C_CreateObject( xSession,
                                                  xPublicKeyTemplate,
                                                  sizeof( xPublicKeyTemplate ) / sizeof( CK_ATTRIBUTE ),
                                                  pxPublicKeyHandle
                                                  );
    }

    if( pucPublicKeyDer != NULL )
    {
        vPortFree( pucPublicKeyDer );
    }

    return xResult;
}

/*-----------------------------------------------------------*/

static CK_RV prvImportCertificateAndKeys( const char * const pcCertificate,
                                          const char * const pcPrivateKey,
                                          const char * const pcPublicKey,
                                          CK_KEY_TYPE xKeyType )
{
    CK_RV xResult = CKR_OK;
    CK_OBJECT_HANDLE xObject = 0;
    /* Certificate configuration template. */
    CK_OBJECT_CLASS xCertificateClass = CKO_CERTIFICATE;
    PKCS11_CertificateTemplate_t xCertificateTemplate;
    size_t xDerCertificateLength = strlen( pcCertificate );
    size_t xDerKeyLength = strlen( pcPrivateKey );

    uint8_t * pucKeyDer = pvPortMalloc( strlen( pcPrivateKey ) );
    uint8_t * pucCertificateDer = pvPortMalloc( strlen( pcCertificate ) );

    if( ( pucCertificateDer == NULL ) || ( pucKeyDer == NULL ) )
    {
        xResult = CKR_HOST_MEMORY;
    }

    if( xResult == CKR_OK )
    {
        /* Convert the key from PEM to DER */
        xResult = convert_pem_to_der( ( const unsigned char * ) pcCertificate,
                                      strlen( pcCertificate ),
                                      pucCertificateDer,
                                      &xDerCertificateLength );
    }

    xCertificateTemplate.xObjectClass = ( CK_ATTRIBUTE ) {
        CKA_CLASS, &xCertificateClass, sizeof( xCertificateClass )
    };
    xCertificateTemplate.xLabel = ( CK_ATTRIBUTE ) {
        CKA_LABEL, &pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS, sizeof( pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS )
    };
    xCertificateTemplate.xValue = ( CK_ATTRIBUTE ) {
        CKA_VALUE, ( CK_VOID_PTR ) pucCertificateDer, ( CK_ULONG ) xDerCertificateLength
    };

    if( xResult == CKR_OK )
    {
        /* Convert the key from PEM to DER */
        xResult = convert_pem_to_der( ( const unsigned char * ) pcPrivateKey,
                                      strlen( pcPrivateKey ),
                                      pucKeyDer,
                                      &xDerKeyLength );
    }

    /* Private key configuration template. */
    CK_OBJECT_CLASS xPrivateKeyClass = CKO_PRIVATE_KEY;
    CK_ATTRIBUTE xPrivateKeyTemplate[] =
    {
        { CKA_CLASS,    &xPrivateKeyClass,                             sizeof( xPrivateKeyClass )                             },
        { CKA_KEY_TYPE, &xKeyType,                                     sizeof( xKeyType )                                     },
        { CKA_LABEL,    &pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS, sizeof( pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS ) },
        { CKA_VALUE,
          ( CK_VOID_PTR ) pucKeyDer,
          ( CK_ULONG ) xDerKeyLength }
    };

    /** \brief Check that a certificate and private key can be imported into
     * persistent storage.
     *  @{
     */

    /* Create an object using the encoded private key. */
    if( CKR_OK == xResult )
    {
        xResult = pxGlobalFunctionList->C_CreateObject( xGlobalSession,
                                                        xPrivateKeyTemplate,
                                                        sizeof( xPrivateKeyTemplate ) / sizeof( CK_ATTRIBUTE ),
                                                        &xObject );
    }

    /* Free the private key. */
    if( CKR_OK == xResult )
    {
        xResult = pxGlobalFunctionList->C_DestroyObject( xGlobalSession,
                                                         xObject );
        xObject = 0;
    }

    /* Create an object using the encoded certificate. */
    if( CKR_OK == xResult )
    {
        xResult = pxGlobalFunctionList->C_CreateObject( xGlobalSession,
                                                        ( CK_ATTRIBUTE_PTR ) &xCertificateTemplate,
                                                        sizeof( xCertificateTemplate ) / sizeof( CK_ATTRIBUTE ),
                                                        &xObject );
    }

    /* Free the certificate. */
    if( CKR_OK == xResult )
    {
        xResult = pxGlobalFunctionList->C_DestroyObject( xGlobalSession,
                                                         xObject );
        xObject = 0;
    }

    if( pucCertificateDer != NULL )
    {
        vPortFree( pucCertificateDer );
    }

    if( pucKeyDer != NULL )
    {
        vPortFree( pucKeyDer );
    }

    /* Import a public key. */
    if( CKR_OK == xResult )
    {
        xResult = prvImportPublicKey( xGlobalSession,
                                      pxGlobalFunctionList,
                                      &xObject,
                                      pcPublicKey );
    }

    /* Free the public key. */
    if( CKR_OK == xResult )
    {
        xResult = pxGlobalFunctionList->C_DestroyObject( xGlobalSession,
                                                         xObject );
        xObject = 0;
    }

    return xResult;
    /** @}*/
}

/*-----------------------------------------------------------*/

static CK_RV prvExportCertificateAndKeys( const char * const pcCertificate,
                                          const char * const pcPrivateKey,
                                          const char * const pcPublicKey,
                                          CK_KEY_TYPE xKeyType )
{
    CK_RV xResult;
    CK_RV xResult2;
    CK_ATTRIBUTE xTemplate = { 0 };
    CK_ULONG ulCount = 0;
    CK_OBJECT_HANDLE xCertificateHandle = 0;
    CK_OBJECT_HANDLE xPublicKeyHandle = 0;
    CK_OBJECT_HANDLE xPrivateKeyHandle = 0;
    CK_BYTE_PTR pucExportBuffer = NULL;
    mbedtls_x509_crt xCertContext = { 0 };


    /* Reprovision with given certificate and key. */
    xResult = prvReprovision( pcCertificate, pcPrivateKey, xKeyType );

    /* Import a public key. */
    if( 0 == xResult )
    {
        xResult = prvImportPublicKey( xGlobalSession,
                                      pxGlobalFunctionList,
                                      &xPublicKeyHandle,
                                      pcPublicKey );
    }

    /* Get the handle of the certificate. */
    if( 0 == xResult )
    {
        xTemplate.type = CKA_LABEL;
        xTemplate.ulValueLen = sizeof( pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS );
        xTemplate.pValue = &pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS;

        xResult = pxGlobalFunctionList->C_FindObjectsInit( xGlobalSession, &xTemplate, 1 );

        if( 0 == xResult )
        {
            xResult = pxGlobalFunctionList->C_FindObjects( xGlobalSession,
                                                           &xCertificateHandle,
                                                           1,
                                                           &ulCount );
        }

        xResult2 = pxGlobalFunctionList->C_FindObjectsFinal( xGlobalSession );

        /* Cleanup for FindObjects, but if a previous function failed, maintain the error code. */
        if( xResult == CKR_OK )
        {
            xResult = xResult2;
        }
    }

    /* Get the handle of the private key. */
    xResult = prvGetPrivateKeyHandle( pxGlobalFunctionList, xGlobalSession, &xPrivateKeyHandle );

    /* Attempt to export the private key. This should fail. */

    if( 0 == xResult )
    {
        xTemplate.type = CKA_VALUE;
        xTemplate.pValue = NULL;

        if( 0 == pxGlobalFunctionList->C_GetAttributeValue( xGlobalSession,
                                                            xPrivateKeyHandle,
                                                            &xTemplate,
                                                            1 ) )
        {
            xResult = CKR_FUNCTION_FAILED;
        }
        else
        {
            /* Note this test should also probably try with a non null value to extract the private key. */
            xResult = CKR_OK;
        }
    }

    /* Test certificate export. */
    if( 0 == xResult )
    {
        xTemplate.pValue = NULL;
        xResult = pxGlobalFunctionList->C_GetAttributeValue( xGlobalSession,
                                                             xCertificateHandle,
                                                             &xTemplate, 1 );
    }

    if( 0 == xResult )
    {
        pucExportBuffer = ( CK_BYTE_PTR ) pvPortMalloc( xTemplate.ulValueLen );

        if( NULL == pucExportBuffer )
        {
            xResult = CKR_HOST_MEMORY;
        }
    }

    if( 0 == xResult )
    {
        xTemplate.pValue = pucExportBuffer;
        xResult = pxGlobalFunctionList->C_GetAttributeValue( xGlobalSession,
                                                             xCertificateHandle,
                                                             &xTemplate,
                                                             1 );
    }

    /* Make sure that the certificate parses.  TODO: Think of way to compare, regardless of stored format */
    if( 0 == xResult )
    {
        mbedtls_x509_crt_init( &xCertContext );

        if( 0 != mbedtls_x509_crt_parse( &xCertContext,
                                         ( const uint8_t * ) xTemplate.pValue,
                                         xTemplate.ulValueLen ) )
        {
            xResult = CKR_FUNCTION_FAILED;
        }

        mbedtls_x509_crt_free( &xCertContext );
    }

    /* Free the exported certificate. */
    if( NULL != pucExportBuffer )
    {
        vPortFree( pucExportBuffer );
        pucExportBuffer = NULL;
    }

    /* Clear the template values being used for the certificate. */
    memset( &xTemplate, 0x00, sizeof( xTemplate ) );

    /* Test public key export. */
    if( 0 == xResult )
    {
        xTemplate.type = CKA_VALUE;
        xResult = pxGlobalFunctionList->C_GetAttributeValue( xGlobalSession,
                                                             xPublicKeyHandle,
                                                             &xTemplate, 1 );
    }

    if( 0 == xResult )
    {
        pucExportBuffer = ( CK_BYTE_PTR ) pvPortMalloc( xTemplate.ulValueLen );

        if( NULL == pucExportBuffer )
        {
            xResult = CKR_HOST_MEMORY;
        }
    }

    if( 0 == xResult )
    {
        xTemplate.pValue = pucExportBuffer;
        xResult = pxGlobalFunctionList->C_GetAttributeValue( xGlobalSession,
                                                             xPublicKeyHandle,
                                                             &xTemplate, 1 );
    }

    /* Free the exported public key. */
    if( NULL != pucExportBuffer )
    {
        vPortFree( pucExportBuffer );
        pucExportBuffer = NULL;
    }

    /* Free the public key object. */
    if( 0 != xPublicKeyHandle )
    {
        xResult2 = pxGlobalFunctionList->C_DestroyObject( xGlobalSession,
                                                          xPublicKeyHandle );

        if( ( xResult2 != CKR_OK ) && ( xResult == CKR_OK ) )
        {
            xResult = xResult2;
        }

        xPublicKeyHandle = 0;
    }

    return xResult;
}

/*-----------------------------------------------------------*/

/* To use a public key for verification, pass a pointer to it. Otherwise, pass NULL. */
static CK_RV prvSignVerifyRoundTrip( CK_MECHANISM_TYPE xMechanism,
                                     const char * const pcPublicKey )
{
    CK_RV xResult = 0;
    CK_ULONG ulCount = 0;
    CK_MECHANISM xMech = { 0 };
    CK_BYTE pucMessage[ cryptoSHA256_DIGEST_BYTES ] = { 0 };
    CK_BYTE pucHash[ cryptoSHA256_DIGEST_BYTES ] = { 0 };
    CK_BYTE pucSignature[ 256 ] = { 0 };
    CK_OBJECT_HANDLE xPublicKey = 0;
    CK_OBJECT_HANDLE xPrivateKey = 0;

    /* Hash the message (the null input). */
    ( void ) mbedtls_sha256_ret( pucMessage, 0, pucHash, 0 );


    /* Get the (first) private key handle. */
    xResult = prvGetPrivateKeyHandle( pxGlobalFunctionList, xGlobalSession, &xPrivateKey );

    /* Query the key type. */
    /*if( 0 == xResult ) */
    /*{ */
    /*    xTemplate.type = CKA_KEY_TYPE; */
    /*    xTemplate.ulValueLen = sizeof( ulKeyType ); */
    /*    xTemplate.pValue = &ulKeyType; */
    /*    xResult = pxGlobalFunctionList->C_GetAttributeValue( xGlobalSession, */
    /*                                                         xPrivateKey, */
    /*                                                         &xTemplate, */
    /*                                                         1 ); */
    /*} */

    /* Sign a hash. */
    if( 0 == xResult )
    {
        xMech.mechanism = xMechanism;
        xResult = pxGlobalFunctionList->C_SignInit( xGlobalSession,
                                                    &xMech,
                                                    xPrivateKey );
    }

    if( 0 == xResult )
    {
        /* TODO - query the key size instead. */
        ulCount = sizeof( pucSignature );
        xResult = pxGlobalFunctionList->C_Sign( xGlobalSession,
                                                pucHash,
                                                sizeof( pucHash ),
                                                pucSignature,
                                                &ulCount );
    }

    /* Create an object using the public key if provided. */
    if( ( 0 == xResult ) && ( pcPublicKey != NULL ) )
    {
        xResult = prvImportPublicKey( xGlobalSession,
                                      pxGlobalFunctionList,
                                      &xPublicKey,
                                      pcPublicKey );
    }

    /* Verify the signature using the same key handle. */
    if( 0 == xResult )
    {
        xResult = pxGlobalFunctionList->C_VerifyInit( xGlobalSession,
                                                      &xMech,
                                                      xPublicKey );
    }

    if( 0 == xResult )
    {
        xResult = pxGlobalFunctionList->C_Verify( xGlobalSession,
                                                  pucHash,
                                                  sizeof( pucHash ),
                                                  pucSignature,
                                                  ulCount );
    }

    return xResult;
}

/*-----------------------------------------------------------*/

static void prvSignVerifyTask( void * pvParameters )
{
    SignVerifyTaskParams_t * pxTaskParams;
    BaseType_t i;
    CK_RV xTestResult = 0;

    pxTaskParams = ( SignVerifyTaskParams_t * ) pvParameters;

    /* Repeatedly run sign-verify in a loop. */
    for( i = 0; i < pkcs11testSIGN_VERIFY_LOOP_COUNT; i++ )
    {
        xTestResult |= prvSignVerifyRoundTrip( CKM_SHA256_RSA_PKCS, NULL );

        if( xTestResult != 0 )
        {
            configPRINTF( ( "Task %d: SignVerify error %d in loop iteration %d.\r\n",
                            pxTaskParams->xTaskNumber,
                            xTestResult,
                            i ) );
            break;
        }
    }

    /* Report the result of the sign-verify loop. */
    pxTaskParams->xTestResult = xTestResult;

    /* Report that task is finished, then delete task. */
    ( void ) xEventGroupSetBits( xSyncEventGroup,
                                 ( 1 << pxTaskParams->xTaskNumber ) );
    vTaskDelete( NULL );
}

/*-----------------------------------------------------------*/

TEST_GROUP( Full_PKCS11_GeneralPurpose );
TEST_GROUP( Full_PKCS11_CryptoOperation );


TEST_SETUP( Full_PKCS11_GeneralPurpose )
{
    CK_RV xResult;

    pxGlobalFunctionList = NULL;

    xResult = C_GetFunctionList( &pxGlobalFunctionList );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to get function list." );
    TEST_ASSERT_NOT_EQUAL_MESSAGE( NULL, pxGlobalFunctionList->C_Finalize, "C_Finalize Function Pointer NULL" );

    xResult = pxGlobalFunctionList->C_Finalize( NULL );

    if( ( xResult != CKR_OK ) && ( xResult != CKR_CRYPTOKI_NOT_INITIALIZED ) )
    {
        /* We are just asserting on CKR_OK here so that xResult will print. */
        TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to finalize PKCS module." );
    }
}

TEST_SETUP( Full_PKCS11_CryptoOperation )
{
    CK_RV xResult;
    CK_SLOT_ID xSlotId = (CK_ULONG) pkcs11testINVALID_SLOT_ID;
    CK_ULONG ulCount = 1;


    pxGlobalFunctionList = NULL;
    xGlobalSession = 0;

    xResult = C_GetFunctionList( &pxGlobalFunctionList );

    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to get function list." );
    TEST_ASSERT_NOT_EQUAL_MESSAGE( NULL, pxGlobalFunctionList, "Invalid function list." );

    /* Check that the functions used by TEST_SETUP are not NULL. */
    TEST_ASSERT_NOT_EQUAL_MESSAGE( NULL, pxGlobalFunctionList->C_Initialize, "C_Initialize Function Pointer NULL" );
    TEST_ASSERT_NOT_EQUAL_MESSAGE( NULL, pxGlobalFunctionList->C_GetSlotList, "C_GetSlotList Function Pointer NULL" );
    TEST_ASSERT_NOT_EQUAL_MESSAGE( NULL, pxGlobalFunctionList->C_OpenSession, "C_OpenSession Function Pointer NULL" );

    /* Initialize the PKCS#11 module. */
    xResult = pxGlobalFunctionList->C_Initialize( NULL );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "PKCS Module initialization failed." );

    /* Get the slot ID with PKCS#11  token. */
    xResult = pxGlobalFunctionList->C_GetSlotList( CK_TRUE, &xSlotId, &ulCount );
    TEST_ASSERT_NOT_EQUAL_MESSAGE( (CK_ULONG) pkcs11testINVALID_SLOT_ID, xSlotId, "Invalid slot number." );

    /* Open a PKCS#11 Session. */
    xResult = pxGlobalFunctionList->C_OpenSession( xSlotId, CKF_SERIAL_SESSION, NULL, NULL, &xGlobalSession );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to open session." );
}

TEST_TEAR_DOWN( Full_PKCS11_GeneralPurpose )
{
}

TEST_TEAR_DOWN( Full_PKCS11_CryptoOperation )
{
    CK_RV xResult;

    /* Check that the functions used by TEST_TEAR_DOWN are not NULL. */
    TEST_ASSERT_NOT_EQUAL_MESSAGE( NULL, pxGlobalFunctionList->C_CloseSession, "C_CloseSession Function Pointer NULL" );
    TEST_ASSERT_NOT_EQUAL_MESSAGE( NULL, pxGlobalFunctionList->C_Finalize, "C_Finalize Function Pointer NULL" );

    xResult = pxGlobalFunctionList->C_CloseSession( xGlobalSession );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to close session." );

    xResult = pxGlobalFunctionList->C_Finalize( NULL );
    TEST_ASSERT_EQUAL_MESSAGE( CKR_OK, xResult, "Failed to finalize PKCS module." );
}

TEST_GROUP_RUNNER( Full_PKCS11_CryptoOperation )
{
    /* Close the last session if it was not closed already. */
    C_Finalize( NULL );

    /* Digest related tests. */
    RUN_TEST_CASE( Full_PKCS11_CryptoOperation, AFQP_Digest );
    RUN_TEST_CASE( Full_PKCS11_CryptoOperation, AFQP_Digest_ErrorConditions );

    /* Verify related tests. */
    RUN_TEST_CASE( Full_PKCS11_CryptoOperation, AFQP_Verify_HappyPath );
    RUN_TEST_CASE( Full_PKCS11_CryptoOperation, AFQP_Verify_InvalidParams );
    RUN_TEST_CASE( Full_PKCS11_CryptoOperation, AFQP_VerifyInit_InvalidParams );

    /* Sign related tests. */
    RUN_TEST_CASE( Full_PKCS11_CryptoOperation, AFQP_Sign_HappyPath );
    RUN_TEST_CASE( Full_PKCS11_CryptoOperation, AFQP_Sign_InvalidParams );
    RUN_TEST_CASE( Full_PKCS11_CryptoOperation, AFQP_SignInit_InvalidParams );

    /* Object related tests. */
    RUN_TEST_CASE( Full_PKCS11_CryptoOperation, AFQP_Objects_HappyPath );
    RUN_TEST_CASE( Full_PKCS11_CryptoOperation, AFQP_CreateObject_InvalidParams );
    RUN_TEST_CASE( Full_PKCS11_CryptoOperation, AFQP_FindObjectsFinal_InvalidParams );
    RUN_TEST_CASE( Full_PKCS11_CryptoOperation, AFQP_FindObjects_InvalidParams );
    RUN_TEST_CASE( Full_PKCS11_CryptoOperation, AFQP_FindObjectsInit_InvalidParams );
    RUN_TEST_CASE( Full_PKCS11_CryptoOperation, AFQP_GetAttributeValue_InvalidParams );

    /* Generated Random tests. */
    RUN_TEST_CASE( Full_PKCS11_CryptoOperation, AFQP_GenerateRandom_InvalidParams );
    RUN_TEST_CASE( Full_PKCS11_CryptoOperation, AFQP_GenerateRandom_HappyPath );

    /* Test that keys can be parsed. */
    RUN_TEST_CASE( Full_PKCS11_CryptoOperation, AFQP_TestRSAParse );
    RUN_TEST_CASE( Full_PKCS11_CryptoOperation, AFQP_TestECDSAParse );

    /* Test key and cert export. */
    RUN_TEST_CASE( Full_PKCS11_CryptoOperation, AFQP_TestRSAExport );
    RUN_TEST_CASE( Full_PKCS11_CryptoOperation, AFQP_TestECDSAExport );

    /* Basic sign-verify tests using RSA/SHA256 mechanism. */
    RUN_TEST_CASE( Full_PKCS11_CryptoOperation, AFQP_SignVerifyRoundTripWithCorrectRSAPublicKey );
    RUN_TEST_CASE( Full_PKCS11_CryptoOperation, AFQP_SignVerifyRoundTripWithWrongRSAPublicKey );

    /* Basic sign-verify tests using EC/SHA256 mechanism. */
    RUN_TEST_CASE( Full_PKCS11_CryptoOperation, AFQP_SignVerifyRoundTripWithCorrectECPublicKey );
    RUN_TEST_CASE( Full_PKCS11_CryptoOperation, AFQP_SignVerifyRoundTripWithWrongECPublicKey );

    /* Test signature verification with output from OpenSSL. Also attempts to
     * verify an invalid signature. */
    RUN_TEST_CASE( Full_PKCS11_CryptoOperation, AFQP_SignVerifyCryptoApiInteropRSA );

    /* Run sign-verify in a loop with multiple tasks. This test may take a while. */
    /*RUN_TEST_CASE( Full_PKCS11_CryptoOperation, AFQP_SignVerifyRoundTrip_MultitaskLoop ); */

    /* Test key generation. */
    RUN_TEST_CASE( Full_PKCS11_CryptoOperation, AFQP_KeyGenerationEcdsaHappyPath );

    /* Re-provision the device with default RSA certs so that subsequent tests are not changed. */
    vDevModeKeyProvisioning();
}

TEST_GROUP_RUNNER( Full_PKCS11_GeneralPurpose )
{
    /* Test PKCS11 functions with invalid parameters. */
    RUN_TEST_CASE( Full_PKCS11_GeneralPurpose, AFQP_GetFunctionListInvalidParams );
    RUN_TEST_CASE( Full_PKCS11_GeneralPurpose, AFQP_InitializeFinalizeInvalidParams );
    RUN_TEST_CASE( Full_PKCS11_GeneralPurpose, AFQP_GetSlotListInvalidParams );
    RUN_TEST_CASE( Full_PKCS11_GeneralPurpose, AFQP_OpenCloseSessionInvalidParams );

    /* Re-provision the device with default RSA certs so that subsequent tests are not changed. */
    vDevModeKeyProvisioning();
}

TEST( Full_PKCS11_CryptoOperation, AFQP_Verify_InvalidParams )
{
    CK_RV xResult = 0;
    CK_MECHANISM xMech = { 0 };
    CK_OBJECT_HANDLE xPublicKey = 0;

    /* Reprovision with test RSA certificate and private key. */
    prvReprovision( pcValidRSACertificate, pcValidRSAPrivateKey, CKK_RSA );

    if( 0 == xResult )
    {
        xResult = prvImportPublicKey( xGlobalSession,
                                      pxGlobalFunctionList,
                                      &xPublicKey,
                                      pcValidRSAPublicKey );

        if( 0 != xResult )
        {
            configPRINTF( ( "Failed to import public key.\r\n" ) );
        }
    }

    /* Verify the signature using the same key handle. */
    if( 0 == xResult )
    {
        xMech.mechanism = CKM_SHA256_RSA_PKCS;
        xResult = pxGlobalFunctionList->C_VerifyInit( xGlobalSession,
                                                      &xMech,
                                                      xPublicKey );

        if( 0 != xResult )
        {
            configPRINTF( ( "Failed to initialize signature verification.\r\n" ) );
        }
    }

    if( 0 == xResult )
    {
        if( TEST_PROTECT() )
        {
            xResult = pxGlobalFunctionList->C_Verify( xGlobalSession,
                                                      ( CK_BYTE_PTR ) NULL,
                                                      ( CK_ULONG ) sizeof( pcNullByteHash ),
                                                      ( CK_BYTE_PTR ) pcSignedData,
                                                      ( CK_ULONG ) sizeof( pcSignedData ) );
            TEST_ASSERT_EQUAL_INT32_MESSAGE( xResult, CKR_ARGUMENTS_BAD, "Unexpected status from VerifyInit_InvalidParams" );

            xResult = pxGlobalFunctionList->C_Verify( xGlobalSession,
                                                      ( CK_BYTE_PTR ) pcNullByteHash,
                                                      ( CK_ULONG ) sizeof( pcNullByteHash ),
                                                      NULL,
                                                      ( CK_ULONG ) sizeof( pcSignedData ) );
            TEST_ASSERT_EQUAL_INT32_MESSAGE( xResult, CKR_ARGUMENTS_BAD, "Unexpected status from VerifyInit_InvalidParams" );

            xResult = pxGlobalFunctionList->C_Verify( xGlobalSession,
                                                      ( CK_BYTE_PTR ) pcNullByteHash,
                                                      ( CK_ULONG ) sizeof( pcNullByteHash ),
                                                      ( CK_BYTE_PTR ) pcSignedData,
                                                      ( CK_ULONG ) 0 );
            TEST_ASSERT_EQUAL_INT32_MESSAGE( xResult, CKR_SIGNATURE_INVALID, "Unexpected status from VerifyInit_InvalidParams" );

            xResult = 0;
        }
    }

    TEST_ASSERT_EQUAL_INT32( 0, xResult );
}

TEST( Full_PKCS11_CryptoOperation, AFQP_VerifyInit_InvalidParams )
{
    CK_RV xResult = 0;
    CK_OBJECT_HANDLE xPublicKey = 0;


    /* Reprovision with test RSA certificate and private key. */
    prvReprovision( pcValidRSACertificate, pcValidRSAPrivateKey, CKK_RSA );

    if( 0 != xResult )
    {
        configPRINTF( ( "Could not start session.\r\n" ) );
    }

    if( 0 == xResult )
    {
        xResult = prvImportPublicKey( xGlobalSession,
                                      pxGlobalFunctionList,
                                      &xPublicKey,
                                      pcValidRSAPublicKey );

        if( 0 != xResult )
        {
            configPRINTF( ( "Failed to import public key.\r\n" ) );
        }
    }

    /* Verify the signature using the same key handle. */
    if( 0 == xResult )
    {
        if( TEST_PROTECT() )
        {
            xResult = pxGlobalFunctionList->C_VerifyInit(
                xGlobalSession,
                NULL,
                xPublicKey );
            TEST_ASSERT_EQUAL_INT32_MESSAGE( xResult, CKR_ARGUMENTS_BAD, "Unexpected status from VerifyInit_InvalidParams" );

            xResult = 0;
        }
    }

    TEST_ASSERT_EQUAL_INT32( 0, xResult );
}

TEST( Full_PKCS11_CryptoOperation, AFQP_Sign_InvalidParams )
{
    CK_RV xResult = 0;
    CK_ULONG ulCount = 0;
    CK_OBJECT_HANDLE xPrivateKey = 0;
    CK_MECHANISM xMech = { 0 };
    CK_BYTE pucMessage[ cryptoSHA256_DIGEST_BYTES ] = { 0 };
    CK_BYTE pucHash[ cryptoSHA256_DIGEST_BYTES ] = { 0 };
    CK_BYTE pucSignature[ 256 ] = { 0 };

    prvReprovision( pcValidRSACertificate, pcValidRSAPrivateKey, CKK_RSA );

    /* Hash the message (the null input). */
    ( void ) mbedtls_sha256_ret( pucMessage, 0, pucHash, 0 );

    /* Get the private key handle. */
    xResult = prvGetPrivateKeyHandle( pxGlobalFunctionList, xGlobalSession, &xPrivateKey );

    /* Sign a hash. */
    if( 0 == xResult )
    {
        xMech.mechanism = CKM_SHA256_RSA_PKCS;
        xResult = pxGlobalFunctionList->C_SignInit( xGlobalSession,
                                                    &xMech,
                                                    xPrivateKey );
    }

    if( 0 == xResult )
    {
        if( TEST_PROTECT() )
        {
            ulCount = sizeof( pucSignature );
            /* Sign data with an invalid (0) length. */
            xResult = pxGlobalFunctionList->C_Sign( xGlobalSession,
                                                    pucHash,
                                                    0, /* Invalid hash data length. */
                                                    pucSignature,
                                                    &ulCount );
            TEST_ASSERT_EQUAL_INT32_MESSAGE( xResult, CKR_DATA_LEN_RANGE, "Unexpected status from C_Sign" );

            /*  Provide a NULL pointer to variable specifying signature length. */
            xResult = pxGlobalFunctionList->C_Sign( xGlobalSession,
                                                    pucHash,
                                                    sizeof( pucHash ),
                                                    pucSignature,
                                                    NULL );
            TEST_ASSERT_EQUAL_INT32_MESSAGE( xResult, CKR_ARGUMENTS_BAD, "Unexpected status from C_Sign" );

            xResult = 0;
        }
    }

    TEST_ASSERT_EQUAL_INT32( 0, xResult );
}


TEST( Full_PKCS11_CryptoOperation, AFQP_SignInit_InvalidParams )
{
    CK_RV xResult = 0;
    CK_OBJECT_HANDLE xPrivateKey = 0;
    CK_BYTE pucMessage[ cryptoSHA256_DIGEST_BYTES ] = { 0 };
    CK_BYTE pucHash[ cryptoSHA256_DIGEST_BYTES ] = { 0 };

    prvReprovision( pcValidRSACertificate, pcValidRSAPrivateKey, CKK_RSA );

    /* Hash the message (the null input). */
    ( void ) mbedtls_sha256_ret( pucMessage, 0, pucHash, 0 );

    /* Get the private key handle. */
    xResult = prvGetPrivateKeyHandle( pxGlobalFunctionList, xGlobalSession, &xPrivateKey );

    /* Sign a hash. */
    if( 0 == xResult )
    {
        if( TEST_PROTECT() )
        {
            /* Try to initialize a signature without a mechanism. */
            xResult = pxGlobalFunctionList->C_SignInit( xGlobalSession,
                                                        NULL,
                                                        xPrivateKey );
            TEST_ASSERT_EQUAL_INT32_MESSAGE( xResult, CKR_ARGUMENTS_BAD, "Unexpected status from C_SignInit" );

            xResult = 0;
        }
    }

    TEST_ASSERT_EQUAL_INT32( 0, xResult );
}

TEST( Full_PKCS11_CryptoOperation, AFQP_FindObjectsFinal_InvalidParams )
{
    CK_RV xResult = 0;
    CK_OBJECT_CLASS xPublicKeyClass = CKO_PUBLIC_KEY;
    CK_OBJECT_HANDLE xObject = 0;
    CK_BBOOL xObjCreated = CK_FALSE;
    CK_ATTRIBUTE xTemplate = { 0 };
    CK_OBJECT_CLASS xObjClass = 0;
    CK_OBJECT_HANDLE xCertObj = 0;
    CK_ULONG ulCount = 0;
    PKCS11_KeyTemplate_t xPublicKeyTemplate;

    /* It doesn't matter what key type is passed. The key type will be automatically
     * identified. */
    CK_KEY_TYPE xKeyType = CKK_RSA;

    /* Public key configuration template. */
    xPublicKeyTemplate.xObjectClass = ( CK_ATTRIBUTE ) {
        CKA_CLASS, &xPublicKeyClass, sizeof( xPublicKeyClass )
    };
    xPublicKeyTemplate.xKeyType = ( CK_ATTRIBUTE ) {
        CKA_KEY_TYPE, &xKeyType, sizeof( xKeyType )
    };
    xPublicKeyTemplate.xLabel = ( CK_ATTRIBUTE ) {
        CKA_LABEL, &pkcs11configLABEL_CODE_VERIFICATION_KEY, sizeof( pkcs11configLABEL_CODE_VERIFICATION_KEY )
    };
    xPublicKeyTemplate.xValue = ( CK_ATTRIBUTE ) {
        CKA_VALUE, ( CK_VOID_PTR ) pcValidRSAPublicKey, ( CK_ULONG ) strlen( pcValidRSAPublicKey ) + 1
    };

    /* Reprovision with test RSA certificate and private key. */
    prvReprovision( pcValidRSACertificate, pcValidRSAPrivateKey, CKK_RSA );

    if( 0 != xResult )
    {
        configPRINTF( ( "Could not start session.\r\n" ) );
    }

    /* Create an object using the public key. */
    if( 0 == xResult )
    {
        xResult = pxGlobalFunctionList->C_CreateObject( xGlobalSession,
                                                        ( CK_ATTRIBUTE_PTR ) &xPublicKeyTemplate,
                                                        sizeof( xPublicKeyTemplate ) / sizeof( CK_ATTRIBUTE ),
                                                        &xObject
                                                        );

        if( 0 != xResult )
        {
            configPRINTF( ( "Could not create object.\r\n" ) );
        }
        else
        {
            xObjCreated = CK_TRUE;
        }
    }

    /* Get the (first) certificate handle. */
    if( 0 == xResult )
    {
        if( TEST_PROTECT() )
        {
            xResult = pxGlobalFunctionList->C_FindObjectsFinal( xGlobalSession );
            TEST_ASSERT_EQUAL_INT32_MESSAGE( xResult, CKR_OPERATION_NOT_INITIALIZED, "Unexpected status from C_FindObjectsFinal" );

            /* Place holder for tests. */
            xResult = 0;
        }

        xTemplate.type = CKA_CLASS;
        xTemplate.ulValueLen = sizeof( CKA_CLASS );
        xTemplate.pValue = &xObjClass;
        xObjClass = CKO_CERTIFICATE;
        xResult = pxGlobalFunctionList->C_FindObjectsInit( xGlobalSession, &xTemplate, 1 );
    }

    if( 0 == xResult )
    {
        xResult = pxGlobalFunctionList->C_FindObjects( xGlobalSession,
                                                       &xCertObj,
                                                       1,
                                                       &ulCount );
    }

    if( 0 == xResult )
    {
        xResult = pxGlobalFunctionList->C_FindObjectsFinal( xGlobalSession );
    }

    /* Free the public key. */
    if( xObjCreated == CK_TRUE )
    {
        xResult = pxGlobalFunctionList->C_DestroyObject( xGlobalSession,
                                                         xObject );
        xObject = 0;

        if( 0 != xResult )
        {
            configPRINTF( ( "Could not destroy object.\r\n" ) );
        }
    }

    TEST_ASSERT_EQUAL_INT32( 0, xResult );
}

TEST( Full_PKCS11_CryptoOperation, AFQP_FindObjects_InvalidParams )
{
    CK_RV xResult = 0;
    CK_OBJECT_CLASS xPublicKeyClass = CKO_PUBLIC_KEY;
    CK_OBJECT_HANDLE xObject = 0;
    CK_BBOOL xObjCreated = CK_FALSE;
    CK_ATTRIBUTE xTemplate = { 0 };
    CK_OBJECT_CLASS xObjClass = 0;
    CK_OBJECT_HANDLE xCertObj = 0;
    CK_ULONG ulCount = 0;
    PKCS11_KeyTemplate_t xPublicKeyTemplate;

    /* It doesn't matter what key type is passed. The key type will be automatically
     * identified. */
    CK_KEY_TYPE xKeyType = CKK_RSA;

    /* Public key configuration template. */
    /* Public key configuration template. */
    xPublicKeyTemplate.xObjectClass = ( CK_ATTRIBUTE ) {
        CKA_CLASS, &xPublicKeyClass, sizeof( xPublicKeyClass )
    };
    xPublicKeyTemplate.xKeyType = ( CK_ATTRIBUTE ) {
        CKA_KEY_TYPE, &xKeyType, sizeof( xKeyType )
    };
    xPublicKeyTemplate.xLabel = ( CK_ATTRIBUTE ) {
        CKA_LABEL, &pkcs11configLABEL_CODE_VERIFICATION_KEY, sizeof( pkcs11configLABEL_CODE_VERIFICATION_KEY )
    };
    xPublicKeyTemplate.xValue = ( CK_ATTRIBUTE ) {
        CKA_VALUE, ( CK_VOID_PTR ) pcValidRSAPublicKey, ( CK_ULONG ) strlen( pcValidRSAPublicKey ) + 1
    };

    /* Reprovision with test RSA certificate and private key. */
    prvReprovision( pcValidRSACertificate, pcValidRSAPrivateKey, CKK_RSA );

    if( 0 != xResult )
    {
        configPRINTF( ( "Could not start session.\r\n" ) );
    }

    /* Create an object using the public key. */
    if( 0 == xResult )
    {
        xResult = pxGlobalFunctionList->C_CreateObject( xGlobalSession,
                                                        ( CK_ATTRIBUTE_PTR ) &xPublicKeyTemplate,
                                                        sizeof( xPublicKeyTemplate ) / sizeof( CK_ATTRIBUTE ),
                                                        &xObject
                                                        );

        if( 0 != xResult )
        {
            configPRINTF( ( "Could not create object.\r\n" ) );
        }
        else
        {
            xObjCreated = CK_TRUE;
        }
    }

    /* Get the (first) certificate handle. */
    if( 0 == xResult )
    {
        if( TEST_PROTECT() )
        {
            xResult = pxGlobalFunctionList->C_FindObjects( xGlobalSession,
                                                           &xCertObj,
                                                           1,
                                                           &ulCount );
            TEST_ASSERT_EQUAL_INT32_MESSAGE( xResult, CKR_OPERATION_NOT_INITIALIZED, "Unexpected status from C_FindObjects" );

            xResult = 0;
        }

        xTemplate.type = CKA_CLASS;
        xTemplate.ulValueLen = sizeof( CKA_CLASS );
        xTemplate.pValue = &xObjClass;
        xObjClass = CKO_CERTIFICATE;
        xResult = pxGlobalFunctionList->C_FindObjectsInit( xGlobalSession, &xTemplate, 1 );
    }

    if( 0 == xResult )
    {
        if( TEST_PROTECT() )
        {
            xResult = pxGlobalFunctionList->C_FindObjects( xGlobalSession,
                                                           &xCertObj,
                                                           0,
                                                           &ulCount );
            TEST_ASSERT_EQUAL_INT32_MESSAGE( xResult, CKR_ARGUMENTS_BAD, "Unexpected status from C_FindObjects" );

            xResult = pxGlobalFunctionList->C_FindObjects( xGlobalSession,
                                                           NULL,
                                                           1,
                                                           &ulCount );
            TEST_ASSERT_EQUAL_INT32_MESSAGE( xResult, CKR_ARGUMENTS_BAD, "Unexpected status from C_FindObjects" );

            xResult = pxGlobalFunctionList->C_FindObjects( xGlobalSession,
                                                           &xCertObj,
                                                           1,
                                                           NULL );
            TEST_ASSERT_EQUAL_INT32_MESSAGE( xResult, CKR_ARGUMENTS_BAD, "Unexpected status from C_FindObjects" );
        }

        xResult = pxGlobalFunctionList->C_FindObjectsFinal( xGlobalSession );
    }

    /* Free the public key. */
    if( xObjCreated == CK_TRUE )
    {
        xResult = pxGlobalFunctionList->C_DestroyObject( xGlobalSession,
                                                         xObject );
        xObject = 0;

        if( 0 != xResult )
        {
            configPRINTF( ( "Could not destroy object.\r\n" ) );
        }
    }

    TEST_ASSERT_EQUAL_INT32( 0, xResult );
}

TEST( Full_PKCS11_CryptoOperation, AFQP_FindObjectsInit_InvalidParams )
{
    CK_RV xResult = 0;
    CK_OBJECT_CLASS xPublicKeyClass = CKO_PUBLIC_KEY;
    CK_OBJECT_HANDLE xObject = 0;
    CK_BBOOL xObjCreated = CK_FALSE;
    PKCS11_KeyTemplate_t xPublicKeyTemplate;

    /* It doesn't matter what key type is passed. The key type will be automatically
     * identified. */
    CK_KEY_TYPE xKeyType = CKK_RSA;

    /* Public key configuration template. */
    xPublicKeyTemplate.xObjectClass = ( CK_ATTRIBUTE ) {
        CKA_CLASS, &xPublicKeyClass, sizeof( xPublicKeyClass )
    };
    xPublicKeyTemplate.xKeyType = ( CK_ATTRIBUTE ) {
        CKA_KEY_TYPE, &xKeyType, sizeof( xKeyType )
    };
    xPublicKeyTemplate.xLabel = ( CK_ATTRIBUTE ) {
        CKA_LABEL, &pkcs11configLABEL_CODE_VERIFICATION_KEY, sizeof( pkcs11configLABEL_CODE_VERIFICATION_KEY )
    };
    xPublicKeyTemplate.xValue = ( CK_ATTRIBUTE ) {
        CKA_VALUE, ( CK_VOID_PTR ) pcValidRSAPublicKey, ( CK_ULONG ) strlen( pcValidRSAPublicKey ) + 1
    };

    /* Reprovision with test RSA certificate and private key. */
    prvReprovision( pcValidRSACertificate, pcValidRSAPrivateKey, CKK_RSA );

    if( 0 != xResult )
    {
        configPRINTF( ( "Could not start session.\r\n" ) );
    }

    /* Create an object using the public key. */
    if( 0 == xResult )
    {
        xResult = pxGlobalFunctionList->C_CreateObject( xGlobalSession,
                                                        ( CK_ATTRIBUTE_PTR ) &xPublicKeyTemplate,
                                                        sizeof( xPublicKeyTemplate ) / sizeof( CK_ATTRIBUTE ),
                                                        &xObject
                                                        );

        if( 0 != xResult )
        {
            configPRINTF( ( "Could not create object.\r\n" ) );
        }
        else
        {
            xObjCreated = CK_TRUE;
        }
    }

    /* Get the (first) certificate handle. */
    if( 0 == xResult )
    {
        if( TEST_PROTECT() )
        {
            xResult = pxGlobalFunctionList->C_FindObjectsInit( xGlobalSession, NULL, 1 );
            TEST_ASSERT_EQUAL_INT32_MESSAGE( xResult, CKR_ARGUMENTS_BAD, "Unexpected status from C_FindObjectsInit" );
        }

        xResult = pxGlobalFunctionList->C_FindObjectsFinal( xGlobalSession );
    }

    /* Free the public key. */
    if( xObjCreated == CK_TRUE )
    {
        xResult = pxGlobalFunctionList->C_DestroyObject( xGlobalSession,
                                                         xObject );
        xObject = 0;

        if( 0 != xResult )
        {
            configPRINTF( ( "Could not destroy object.\r\n" ) );
        }
    }

    TEST_ASSERT_EQUAL_INT32( 0, xResult );
}

/*-----------------------------------------------------------*/
TEST( Full_PKCS11_CryptoOperation, AFQP_GetAttributeValue_InvalidParams )
{
    CK_RV xResult = 0;
    CK_OBJECT_HANDLE xPrivateKey = 0;
    CK_BYTE pucMessage[ cryptoSHA256_DIGEST_BYTES ] = { 0 };
    CK_BYTE pucHash[ cryptoSHA256_DIGEST_BYTES ] = { 0 };

    prvReprovision( pcValidRSACertificate, pcValidRSAPrivateKey, CKK_RSA );

    /* Hash the message (the null input). */
    ( void ) mbedtls_sha256_ret( pucMessage, 0, pucHash, 0 );

    /* Get the (first) private key handle. */
    xResult = prvGetPrivateKeyHandle( pxGlobalFunctionList, xGlobalSession, &xPrivateKey );

    /* Query the key type. */
    if( 0 == xResult )
    {
        if( TEST_PROTECT() )
        {
            xResult = pxGlobalFunctionList->C_GetAttributeValue( xGlobalSession,
                                                                 xPrivateKey,
                                                                 NULL,
                                                                 1 );
            TEST_ASSERT_EQUAL_INT32_MESSAGE( xResult, CKR_ARGUMENTS_BAD, "Unexpected status from C_GetAttribute" );

            xResult = 0;
        }
    }

    TEST_ASSERT_EQUAL_INT32( 0, xResult );
}

TEST( Full_PKCS11_CryptoOperation, AFQP_GenerateRandom_HappyPath )
{
    CK_RV xResult = 0;
    CK_SLOT_ID xSlotId = 0;
    BaseType_t xSameSession = 0;
    BaseType_t xDifferentSessions = 0;

#define pkcstestRAND_BUFFER_SIZE    10 /* This number is not actually flexible anymore because of the print formatting. */
    CK_BYTE pcBuf1[ pkcstestRAND_BUFFER_SIZE ];
    CK_BYTE pcBuf2[ pkcstestRAND_BUFFER_SIZE ];
    CK_BYTE pcBuf3[ pkcstestRAND_BUFFER_SIZE ];

    /* Generate random bytes twice. */
    if( CKR_OK == xResult )
    {
        xResult = C_GenerateRandom( xGlobalSession, pcBuf1, pkcstestRAND_BUFFER_SIZE );
    }

    if( CKR_OK == xResult )
    {
        xResult = C_GenerateRandom( xGlobalSession, pcBuf2, pkcstestRAND_BUFFER_SIZE );
    }

    if( CKR_OK == xResult )
    {
        /* Close the session and PKCS#11 module */
        if( NULL != pxGlobalFunctionList )
        {
            ( void ) pxGlobalFunctionList->C_CloseSession( xGlobalSession );
            ( void ) pxGlobalFunctionList->C_Finalize( NULL );
        }
    }

    /* Re-initialize PKCS#11. */
    xResult = prvInitializeAndStartSession( &pxGlobalFunctionList,
                                            &xSlotId,
                                            &xGlobalSession );

    if( CKR_OK == xResult )
    {
        xResult = C_GenerateRandom( xGlobalSession, pcBuf3, pkcstestRAND_BUFFER_SIZE );
    }

    /* Check that the result is good. */
    TEST_ASSERT_EQUAL_INT32( CKR_OK, xResult );

    /* Check that the random bytes generated within session
     * and between initializations of PKCS module are not the same. */
    for( int i = 0; i < pkcstestRAND_BUFFER_SIZE; i++ )
    {
        if( pcBuf1[ i ] == pcBuf2[ i ] )
        {
            xSameSession++;
        }

        if( pcBuf1[ i ] == pcBuf3[ i ] )
        {
            xDifferentSessions++;
        }
    }

    if( ( xSameSession > 1 ) || ( xDifferentSessions > 1 ) )
    {
        configPRINTF( ( "First Random Bytes: %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X\r\n",
                        pcBuf1[ 0 ], pcBuf1[ 1 ], pcBuf1[ 2 ], pcBuf1[ 3 ], pcBuf1[ 4 ],
                        pcBuf1[ 5 ], pcBuf1[ 6 ], pcBuf1[ 7 ], pcBuf1[ 8 ], pcBuf1[ 9 ] ) );

        configPRINTF( ( "Second Set of Random Bytes: %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X\r\n",
                        pcBuf2[ 0 ], pcBuf2[ 1 ], pcBuf2[ 2 ], pcBuf2[ 3 ], pcBuf2[ 4 ],
                        pcBuf2[ 5 ], pcBuf2[ 6 ], pcBuf2[ 7 ], pcBuf2[ 8 ], pcBuf2[ 9 ] ) );

        configPRINTF( ( "Third Set of Random Bytes:  %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X\r\n",
                        pcBuf3[ 0 ], pcBuf3[ 1 ], pcBuf3[ 2 ], pcBuf3[ 3 ], pcBuf3[ 4 ],
                        pcBuf3[ 5 ], pcBuf3[ 6 ], pcBuf3[ 7 ], pcBuf3[ 8 ], pcBuf3[ 9 ] ) );
    }

    TEST_ASSERT_LESS_THAN( 2, xSameSession );
    TEST_ASSERT_LESS_THAN( 2, xDifferentSessions );
}

TEST( Full_PKCS11_CryptoOperation, AFQP_GenerateRandom_InvalidParams )
{
    CK_RV xResult = 0;
    CK_BYTE xBuf[ 1 ];

    /* Reprovision with test RSA certificate and private key. */
    prvReprovision( pcValidRSACertificate, pcValidRSAPrivateKey, CKK_RSA );

    if( 0 != xResult )
    {
        configPRINTF( ( "Could not start session.\r\n" ) );
    }

    if( 0 == xResult )
    {
        if( TEST_PROTECT() )
        {
            xResult = C_GenerateRandom( xGlobalSession, NULL, 1 );
            TEST_ASSERT_EQUAL_INT32_MESSAGE( xResult, CKR_ARGUMENTS_BAD, "Unexpected status from C_GenerateRandom" );

            xResult = C_GenerateRandom( xGlobalSession, xBuf, 0 );
            TEST_ASSERT_EQUAL_INT32_MESSAGE( xResult, CKR_ARGUMENTS_BAD, "Unexpected status from C_GenerateRandom" );
        }

        xResult = 0;
    }
    else
    {
        configPRINTF( ( "Could generate random numbers.\r\n" ) );
    }

    TEST_ASSERT_EQUAL_INT32( 0, xResult );
}

TEST( Full_PKCS11_CryptoOperation, AFQP_CreateObject_InvalidParams )
{
    CK_RV xResult = 0;
    CK_OBJECT_CLASS xPublicKeyClass = CKO_PUBLIC_KEY;
    CK_FUNCTION_LIST_PTR pxFunctionList = NULL;
    CK_OBJECT_HANDLE xObject = 0;
    PKCS11_KeyTemplate_t xPublicKeyTemplate;

    /* It doesn't matter what key type is passed. The key type will be automatically
     * identified. */
    CK_KEY_TYPE xKeyType = CKK_RSA;

    /* Public key configuration template. */
    xPublicKeyTemplate.xObjectClass = ( CK_ATTRIBUTE ) {
        CKA_CLASS, &xPublicKeyClass, sizeof( xPublicKeyClass )
    };
    xPublicKeyTemplate.xKeyType = ( CK_ATTRIBUTE ) {
        CKA_KEY_TYPE, &xKeyType, sizeof( xKeyType )
    };
    xPublicKeyTemplate.xLabel = ( CK_ATTRIBUTE ) {
        CKA_LABEL, &pkcs11configLABEL_CODE_VERIFICATION_KEY, sizeof( pkcs11configLABEL_CODE_VERIFICATION_KEY )
    };
    xPublicKeyTemplate.xValue = ( CK_ATTRIBUTE ) {
        CKA_VALUE, ( CK_VOID_PTR ) pcValidRSAPublicKey, ( CK_ULONG ) strlen( pcValidRSAPublicKey ) + 1
    };

    /* Reprovision with test RSA certificate and private key. */
    prvReprovision( pcValidRSACertificate, pcValidRSAPrivateKey, CKK_RSA );

    if( 0 != xResult )
    {
        configPRINTF( ( "Could not start session.\r\n" ) );
    }

    /* Create an object using the public key. */
    if( 0 == xResult )
    {
        if( TEST_PROTECT() )
        {
            xResult = pxGlobalFunctionList->C_CreateObject( xGlobalSession,
                                                            NULL,
                                                            sizeof( xPublicKeyTemplate ) / sizeof( CK_ATTRIBUTE ),
                                                            &xObject );
            TEST_ASSERT_EQUAL_INT32_MESSAGE( xResult, CKR_ARGUMENTS_BAD, "Unexpected status from object creation" );


            xResult = pxGlobalFunctionList->C_CreateObject( xGlobalSession,
                                                            ( CK_ATTRIBUTE_PTR ) &xPublicKeyTemplate,
                                                            sizeof( xPublicKeyTemplate ) / sizeof( CK_ATTRIBUTE ),
                                                            NULL );
            TEST_ASSERT_EQUAL_INT32_MESSAGE( xResult, CKR_ARGUMENTS_BAD, "Unexpected status from object creation" );

            xResult = pxGlobalFunctionList->C_CreateObject( xGlobalSession,
                                                            ( CK_ATTRIBUTE_PTR ) &xPublicKeyTemplate,
                                                            ( CK_ULONG ) -1, /* Maximum possible length (unsigned) should be invalid. */
                                                            &xObject );
            TEST_ASSERT_EQUAL_INT32_MESSAGE( xResult, CKR_ARGUMENTS_BAD, "Unexpected status from object creation" );

            /* Free the public key. */
            if( 0 == xResult )
            {
                xResult = pxFunctionList->C_DestroyObject( xGlobalSession,
                                                           xObject );
                xObject = 0;

                if( 0 != xResult )
                {
                    configPRINTF( ( "Could not destroy object.\r\n" ) );
                }
            }
            else
            {
                xResult = 0;
            }
        }
    }

    TEST_ASSERT_EQUAL_INT32( 0, xResult );
}

TEST( Full_PKCS11_CryptoOperation, AFQP_Sign_HappyPath )
{
    CK_RV xResult = 0;
    CK_ULONG ulCount = 0;
    CK_OBJECT_HANDLE xPrivateKey = 0;
    CK_MECHANISM xMech = { 0 };
    CK_BYTE pucMessage[ cryptoSHA256_DIGEST_BYTES ] = { 0 };
    CK_BYTE pucHash[ cryptoSHA256_DIGEST_BYTES ] = { 0 };
    CK_BYTE pucSignature[ 256 ] = { 0 };

    prvReprovision( pcValidRSACertificate, pcValidRSAPrivateKey, CKK_RSA );

    /* Hash the message (the null input). */
    ( void ) mbedtls_sha256_ret( pucMessage, 0, pucHash, 0 );

    /* Get the private key handle. */
    xResult = prvGetPrivateKeyHandle( pxGlobalFunctionList, xGlobalSession, &xPrivateKey );

    /* Sign a hash. */
    if( 0 == xResult )
    {
        xMech.mechanism = CKM_SHA256_RSA_PKCS;
        xResult = pxGlobalFunctionList->C_SignInit( xGlobalSession,
                                                    &xMech,
                                                    xPrivateKey );
    }

    if( 0 == xResult )
    {
        /* TODO - query the key size instead. */
        ulCount = sizeof( pucSignature );
        xResult = pxGlobalFunctionList->C_Sign( xGlobalSession,
                                                pucHash,
                                                sizeof( pucHash ),
                                                pucSignature,
                                                &ulCount );
    }

    TEST_ASSERT_EQUAL_INT32( 0, xResult );
}

TEST( Full_PKCS11_CryptoOperation, AFQP_Objects_HappyPath )
{
    CK_RV xResult = 0;
    CK_OBJECT_CLASS xPublicKeyClass = CKO_PUBLIC_KEY;
    CK_OBJECT_HANDLE xObject = 0;
    CK_BBOOL xObjCreated = CK_FALSE;

    PKCS11_KeyTemplate_t xPublicKeyTemplate;

    /* It doesn't matter what key type is passed. The key type will be automatically
     * identified. */
    CK_KEY_TYPE xKeyType = CKK_RSA;

    /* Public key configuration template. */
    xPublicKeyTemplate.xObjectClass = ( CK_ATTRIBUTE ) {
        CKA_CLASS, &xPublicKeyClass, sizeof( xPublicKeyClass )
    };
    xPublicKeyTemplate.xKeyType = ( CK_ATTRIBUTE ) {
        CKA_KEY_TYPE, &xKeyType, sizeof( xKeyType )
    };
    xPublicKeyTemplate.xLabel = ( CK_ATTRIBUTE ) {
        CKA_LABEL, &pkcs11configLABEL_CODE_VERIFICATION_KEY, sizeof( pkcs11configLABEL_CODE_VERIFICATION_KEY )
    };
    xPublicKeyTemplate.xValue = ( CK_ATTRIBUTE ) {
        CKA_VALUE, ( CK_VOID_PTR ) pcValidRSAPublicKey, ( CK_ULONG ) strlen( pcValidRSAPublicKey ) + 1
    };


    /* Reprovision with test RSA certificate and private key. */
    xResult = prvReprovision( pcValidRSACertificate, pcValidRSAPrivateKey, CKK_RSA );
    TEST_ASSERT_EQUAL_INT32( CKR_OK, xResult );

    /* Create an object using the public key. */
    if( 0 == xResult )
    {
        xResult = pxGlobalFunctionList->C_CreateObject( xGlobalSession,
                                                        ( CK_ATTRIBUTE_PTR ) &xPublicKeyTemplate,
                                                        sizeof( xPublicKeyTemplate ) / sizeof( CK_ATTRIBUTE ),
                                                        &xObject
                                                        );

        if( 0 != xResult )
        {
            configPRINTF( ( "Could not create object.\r\n" ) );
        }
        else
        {
            xObjCreated = CK_TRUE;
        }
    }

    /* Get the certificate handle. */
    xResult = prvGetCertificateHandle( pxGlobalFunctionList, xGlobalSession, &xObject );
    TEST_ASSERT_EQUAL_INT32( CKR_OK, xResult );

    /* Free the public key. */
    if( xObjCreated == CK_TRUE )
    {
        xResult = pxGlobalFunctionList->C_DestroyObject( xGlobalSession,
                                                         xObject );
        xObject = 0;

        if( 0 != xResult )
        {
            configPRINTF( ( "Could not destroy object.\r\n" ) );
        }
    }

    TEST_ASSERT_EQUAL_INT32( 0, xResult );
}

TEST( Full_PKCS11_CryptoOperation, AFQP_Verify_HappyPath )
{
    CK_RV xResult = 0;
    CK_MECHANISM xMech = { 0 };
    CK_OBJECT_HANDLE xPublicKey = 0;


    /* Reprovision with test RSA certificate and private key. */
    xResult = prvReprovision( pcValidRSACertificate, pcValidRSAPrivateKey, CKK_RSA );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    if( 0 != xResult )
    {
        configPRINTF( ( "Could not start session.\r\n" ) );
    }

    if( 0 == xResult )
    {
        xResult = prvImportPublicKey( xGlobalSession,
                                      pxGlobalFunctionList,
                                      &xPublicKey,
                                      pcValidRSAPublicKey );

        if( 0 != xResult )
        {
            configPRINTF( ( "Failed to import public key.\r\n" ) );
        }
    }

    /* Verify the signature using the same key handle. */
    if( 0 == xResult )
    {
        xMech.mechanism = CKM_SHA256_RSA_PKCS;
        xResult = pxGlobalFunctionList->C_VerifyInit( xGlobalSession,
                                                      &xMech,
                                                      xPublicKey );

        if( 0 != xResult )
        {
            configPRINTF( ( "Failed to initialize signature verification.\r\n" ) );
        }
    }

    if( 0 == xResult )
    {
        xResult = pxGlobalFunctionList->C_Verify( xGlobalSession,
                                                  ( CK_BYTE_PTR ) pcNullByteHash,
                                                  ( CK_ULONG ) sizeof( pcNullByteHash ),
                                                  ( CK_BYTE_PTR ) pcSignedData,
                                                  ( CK_ULONG ) sizeof( pcSignedData ) );
    }

    TEST_ASSERT_EQUAL_INT32( 0, xResult );
}

/*-----------------------------------------------------------*/

TEST( Full_PKCS11_GeneralPurpose, AFQP_GetFunctionListInvalidParams )
{
    CK_FUNCTION_LIST_PTR pxFunctionList = NULL;

    TEST_ASSERT_EQUAL( CKR_ARGUMENTS_BAD, C_GetFunctionList( NULL ) );

    TEST_ASSERT_EQUAL( CKR_OK, C_GetFunctionList( &pxFunctionList ) );

    /* Ensure that pxFunctionList was changed by C_GetFunctionList. */
    TEST_ASSERT_NOT_EQUAL( NULL, pxFunctionList );
}

/*-----------------------------------------------------------*/

TEST( Full_PKCS11_GeneralPurpose, AFQP_InitializeFinalizeInvalidParams )
{
    CK_FUNCTION_LIST_PTR pxFunctionList = NULL;

    CK_RV xP11Result;

    xP11Result = C_GetFunctionList( &pxFunctionList );

    if( xP11Result == CKR_OK )
    {
        xP11Result = pxFunctionList->C_Initialize( NULL );
    }

    if( xP11Result == CKR_OK )
    {
        if( TEST_PROTECT() )
        {
            /* C_Finalize should fail if pReserved isn't NULL. */
            {
                TEST_ASSERT_EQUAL( CKR_ARGUMENTS_BAD, pxFunctionList->C_Finalize( ( CK_VOID_PTR ) 0x1234 ) );
            }
        }
    }

    if( xP11Result == CKR_OK )
    {
        xP11Result = pxFunctionList->C_Finalize( NULL );
    }

    TEST_ASSERT_EQUAL( CKR_OK, xP11Result );
}

/*-----------------------------------------------------------*/

TEST( Full_PKCS11_GeneralPurpose, AFQP_GetSlotListInvalidParams )
{
    CK_FUNCTION_LIST_PTR pxFunctionList = NULL;
    CK_SLOT_ID xSlotId = 0;
    CK_ULONG ulSlotCount = 0;

    TEST_ASSERT_EQUAL( CKR_OK, C_GetFunctionList( &pxFunctionList ) );

    if( CKR_OK == pxFunctionList->C_Initialize( NULL ) )
    {
        if( TEST_PROTECT() )
        {
            /* Passing NULL as pulCount isn't valid. */
            TEST_ASSERT_EQUAL( CKR_ARGUMENTS_BAD, pxFunctionList->C_GetSlotList( CK_FALSE, NULL, NULL ) );

            /* Passing 0 for slot count and attempting to get slot list isn't valid. */
            ulSlotCount = 0;
            TEST_ASSERT_EQUAL( CKR_BUFFER_TOO_SMALL, pxFunctionList->C_GetSlotList( CK_FALSE, &xSlotId, &ulSlotCount ) );

            /* This should get the number of slots. Check that ulSlotCount changed. */
            TEST_ASSERT_EQUAL( CKR_OK, pxFunctionList->C_GetSlotList( CK_FALSE, NULL, &ulSlotCount ) );
            TEST_ASSERT_NOT_EQUAL( 0, ulSlotCount );

            /* Get slot list, all inputs valid. */
            TEST_ASSERT_EQUAL( CKR_OK, pxFunctionList->C_GetSlotList( CK_FALSE, &xSlotId, &ulSlotCount ) );
            TEST_ASSERT_NOT_EQUAL( 0, xSlotId );
        }

        pxFunctionList->C_Finalize( NULL );
    }
    else
    {
        TEST_FAIL();
    }
}

/*-----------------------------------------------------------*/

TEST( Full_PKCS11_GeneralPurpose, AFQP_OpenCloseSessionInvalidParams )
{
    CK_FUNCTION_LIST_PTR pxFunctionList = NULL;
    CK_SLOT_ID xSlotId = 0;
    CK_ULONG ulSlotCount = 0;
    CK_SESSION_HANDLE xSession = 0;

    TEST_ASSERT_EQUAL( CKR_OK, C_GetFunctionList( &pxFunctionList ) );

    if( CKR_OK == pxFunctionList->C_Initialize( NULL ) )
    {
        if( TEST_PROTECT() )
        {
            /* Get slot list. */
            TEST_ASSERT_EQUAL( CKR_OK, pxFunctionList->C_GetSlotList( CK_TRUE, NULL, &ulSlotCount ) );
            TEST_ASSERT_EQUAL( CKR_OK, pxFunctionList->C_GetSlotList( CK_TRUE, &xSlotId, &ulSlotCount ) );

            /* Test C_OpenSession with bad pxSession argument. */
            TEST_ASSERT_EQUAL( CKR_ARGUMENTS_BAD, pxFunctionList->C_OpenSession( xSlotId, CKF_SERIAL_SESSION, NULL, NULL, NULL ) );

            /* Test C_OpenSession without setting required CKF_SERIAL_SESSION flag. */
            TEST_ASSERT_EQUAL( CKR_SESSION_PARALLEL_NOT_SUPPORTED, pxFunctionList->C_OpenSession( xSlotId, 0, NULL, NULL, &xSession ) );

            /* Test C_OpenSession with valid arguments. */
            TEST_ASSERT_EQUAL( CKR_OK, pxFunctionList->C_OpenSession( xSlotId, CKF_SERIAL_SESSION, NULL, NULL, &xSession ) );
            TEST_ASSERT_NOT_EQUAL( 0, xSession );

            /* C_CloseSession with invalid argument. */
            TEST_ASSERT_EQUAL( CKR_SESSION_HANDLE_INVALID, pxFunctionList->C_CloseSession( ( CK_SESSION_HANDLE ) NULL ) );

            /* C_CloseSession with valid argument. */
            if( CKR_OK == pxFunctionList->C_CloseSession( xSession ) )
            {
                xSession = 0;
            }
            else
            {
                TEST_FAIL();
            }
        }

        /* If a session is left open due to test failure, close it. */
        if( 0 != xSession )
        {
            pxFunctionList->C_CloseSession( xSession );
        }

        pxFunctionList->C_Finalize( NULL );
    }
    else
    {
        TEST_FAIL();
    }
}

/*-----------------------------------------------------------*/

TEST( Full_PKCS11_CryptoOperation, AFQP_TestRSAParse )
{
    if( TEST_PROTECT() )
    {
        /* Ensure that valid cert and keys are successfully imported. */
        if( prvImportCertificateAndKeys( pcValidRSACertificate,
                                         pcValidRSAPrivateKey,
                                         pcValidRSAPublicKey,
                                         CKK_RSA ) != 0 )
        {
            TEST_FAIL();
        }

        /* Ensure that an invalid cert with valid keys is rejected. */
        if( prvImportCertificateAndKeys( pcInvalidRSACertificate,
                                         pcValidRSAPrivateKey,
                                         pcValidRSAPublicKey,
                                         CKK_RSA ) == 0 )
        {
            TEST_FAIL();
        }

        /* Ensure that a valid cert with invalid private key is rejected. */
        if( prvImportCertificateAndKeys( pcValidRSACertificate,
                                         pcInvalidRSAPrivateKey,
                                         pcValidRSAPublicKey,
                                         CKK_RSA ) == 0 )
        {
            TEST_FAIL();
        }

        /* Ensure that a valid cert with invalid public key is rejected. */
        if( prvImportCertificateAndKeys( pcValidRSACertificate,
                                         pcValidRSAPrivateKey,
                                         pcInvalidRSAPublicKey,
                                         CKK_RSA ) == 0 )
        {
            TEST_FAIL();
        }
    }
}

/*-----------------------------------------------------------*/

TEST( Full_PKCS11_CryptoOperation, AFQP_TestECDSAParse )
{
    if( TEST_PROTECT() )
    {
        /* Ensure that valid cert and keys are successfully parsed. */
        if( prvImportCertificateAndKeys( pcValidECDSACertificate,
                                         pcValidECDSAPrivateKey,
                                         pcValidECDSAPublicKey,
                                         CKK_EC ) != 0 )
        {
            TEST_FAIL();
        }

        /* Ensure that an invalid cert with valid keys is rejected. */
        if( prvImportCertificateAndKeys( pcInvalidECDSACertificate,
                                         pcValidECDSAPrivateKey,
                                         pcValidECDSAPublicKey,
                                         CKK_EC ) == 0 )
        {
            TEST_FAIL();
        }

        /* Ensure that a valid cert with invalid private key is rejected. */
        if( prvImportCertificateAndKeys( pcValidECDSACertificate,
                                         pcInvalidECDSAPrivateKey,
                                         pcValidECDSAPublicKey,
                                         CKK_EC ) == 0 )
        {
            TEST_FAIL();
        }

        /* Ensure that a valid cert with invalid public key is rejected. */
        if( prvImportCertificateAndKeys( pcValidECDSACertificate,
                                         pcValidECDSAPrivateKey,
                                         pcInvalidECDSAPublicKey,
                                         CKK_EC ) == 0 )
        {
            TEST_FAIL();
        }
    }
}

/*-----------------------------------------------------------*/

TEST( Full_PKCS11_CryptoOperation, AFQP_TestRSAExport )
{
    TEST_ASSERT_EQUAL_INT32( prvExportCertificateAndKeys( pcValidRSACertificate,
                                                          pcValidRSAPrivateKey,
                                                          pcValidRSAPublicKey,
                                                          CKK_RSA ),
                             0 );
}

/*-----------------------------------------------------------*/

TEST( Full_PKCS11_CryptoOperation, AFQP_TestECDSAExport )
{
    TEST_ASSERT_EQUAL_INT32( prvExportCertificateAndKeys( pcValidECDSACertificate,
                                                          pcValidECDSAPrivateKey,
                                                          pcValidECDSAPublicKey,
                                                          CKK_EC ),
                             0 );
}

/*-----------------------------------------------------------*/



/*-----------------------------------------------------------*/

TEST( Full_PKCS11_CryptoOperation, AFQP_SignVerifyRoundTripWithCorrectRSAPublicKey )
{
    /* Reprovision with test RSA certificate and private key. */
    prvReprovision( pcValidRSACertificate, pcValidRSAPrivateKey, CKK_RSA );

    TEST_ASSERT_EQUAL_INT32( prvSignVerifyRoundTrip( CKM_SHA256_RSA_PKCS,
                                                     pcValidRSAPublicKey ),
                             0 );
}

/*-----------------------------------------------------------*/

TEST( Full_PKCS11_CryptoOperation, AFQP_SignVerifyRoundTripWithWrongRSAPublicKey )
{
    /* Reprovision with test RSA certificate and private key. */
    prvReprovision( pcValidRSACertificate, pcValidRSAPrivateKey, CKK_RSA );

    /* Pass a public key that isn't paired with the default private key. Verification
     * should fail. */
    TEST_ASSERT_NOT_EQUAL( prvSignVerifyRoundTrip( CKM_SHA256_RSA_PKCS,
                                                   pcWrongRSAPublicKey ),
                           0 );
}

/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

TEST( Full_PKCS11_CryptoOperation, AFQP_SignVerifyRoundTripWithCorrectECPublicKey )
{
    /* Reprovision with test RSA certificate and private key. */
    prvReprovision( pcValidECDSACertificate, pcValidECDSAPrivateKey, CKK_EC );

    TEST_ASSERT_EQUAL_INT32( prvSignVerifyRoundTrip( CKM_ECDSA,
                                                     pcValidECDSAPublicKey ),
                             0 );
}

/*-----------------------------------------------------------*/

TEST( Full_PKCS11_CryptoOperation, AFQP_SignVerifyRoundTripWithWrongECPublicKey )
{
    /* Reprovision with test ECDSA certificate and private key. */
    prvReprovision( pcValidECDSACertificate, pcValidECDSAPrivateKey, CKK_EC );

    TEST_ASSERT_NOT_EQUAL( prvSignVerifyRoundTrip( CKM_ECDSA,
                                                   pcWrongECDSAPublicKey ),
                           0 );
}

/*-----------------------------------------------------------*/

TEST( Full_PKCS11_CryptoOperation, AFQP_SignVerifyCryptoApiInteropRSA )
{
    CK_RV xResult = 0;
    CK_OBJECT_HANDLE xCertObj = 0;
    CK_MECHANISM xMech = { 0 };
    CK_OBJECT_HANDLE xPublicKey = 0;

    /* Reprovision with test RSA certificate and private key. */
    prvReprovision( pcValidRSACertificate, pcValidRSAPrivateKey, CKK_RSA );

    /* Get the (first) certificate handle. */
    xResult = prvGetCertificateHandle( pxGlobalFunctionList, xGlobalSession, &xCertObj );

    /* Create an object using the public key. */
    if( 0 == xResult )
    {
        xResult = prvImportPublicKey( xGlobalSession,
                                      pxGlobalFunctionList,
                                      &xPublicKey,
                                      pcValidRSAPublicKey );
    }

    /* Verify the signature using the same key handle. */
    if( 0 == xResult )
    {
        xMech.mechanism = CKM_SHA256_RSA_PKCS;
        xResult = pxGlobalFunctionList->C_VerifyInit( xGlobalSession,
                                                      &xMech,
                                                      xPublicKey );
    }

    /* Attempt to verify a valid signature. */
    if( 0 == xResult )
    {
        xResult = pxGlobalFunctionList->C_Verify( xGlobalSession,
                                                  ( CK_BYTE_PTR ) pcNullByteHash,
                                                  ( CK_ULONG ) sizeof( pcNullByteHash ),
                                                  ( CK_BYTE_PTR ) pcSignedData,
                                                  ( CK_ULONG ) sizeof( pcSignedData ) );
    }

    /* Create a malformed signature by dropping a byte. Attempt to verify it.
     * Verification should fail. */
    if( 0 == xResult )
    {
        if( 0 == pxGlobalFunctionList->C_Verify( xGlobalSession,
                                                 ( CK_BYTE_PTR ) pcNullByteHash,
                                                 ( CK_ULONG ) sizeof( pcNullByteHash ),
                                                 ( CK_BYTE_PTR ) pcSignedData,
                                                 ( CK_ULONG ) sizeof( pcSignedData ) - 1 ) )
        {
            xResult = CKR_FUNCTION_FAILED;
        }
        else
        {
            xResult = CKR_OK;
        }
    }

    /* Clean-up. */
    if( 0 != xPublicKey )
    {
        xResult = pxGlobalFunctionList->C_DestroyObject( xGlobalSession,
                                                         xPublicKey );
        xPublicKey = 0;
    }

    TEST_ASSERT_EQUAL_INT32( xResult, 0 );
}

/*-----------------------------------------------------------*/

TEST( Full_PKCS11_CryptoOperation, AFQP_SignVerifyRoundTrip_MultitaskLoop )
{
    SignVerifyTaskParams_t xTaskParams[ pkcs11testSIGN_VERIFY_TASK_COUNT ];
    BaseType_t i;

    /* Initialize all of the xTestResult values to something other than 0,
     * as 0 means success. */
    memset( xTaskParams,
            0xFF,
            sizeof( SignVerifyTaskParams_t ) * pkcs11testSIGN_VERIFY_TASK_COUNT );

    /* Create the event group used to synchronize tasks. */
    xSyncEventGroup = xEventGroupCreate();

    if( xSyncEventGroup != NULL )
    {
        /* Create the sign verify loop tasks. */
        for( i = 0; i < pkcs11testSIGN_VERIFY_TASK_COUNT; i++ )
        {
            xTaskParams[ i ].xTaskNumber = i;

            xTaskCreate( prvSignVerifyTask,                     /* Task code. */
                         "SignVerifyTask",                      /* All tasks have same name, but are distinguished by task number. */
                         pkcs11testSIGN_VERIFY_TASK_STACK_SIZE, /* Task stack size. */
                         &( xTaskParams[ i ] ),                 /* Where the task writes its result. */
                         pkcs11testSIGN_VERIFY_TASK_PRIORITY,   /* Task priority. */
                         NULL );                                /* Task handle (not used). */
        }

        /* Wait for all tasks to finish. */
        if( xEventGroupWaitBits( xSyncEventGroup,
                                 pkcs11testALL_BITS,
                                 pdFALSE,
                                 pdTRUE,
                                 pkcs11testEVENT_GROUP_TIMEOUT_MS ) != pkcs11testALL_BITS )
        {
            configPRINTF( ( "Timed out waiting for SignVerify tasks to finish.\r\n" ) );
        }

        vEventGroupDelete( xSyncEventGroup );
    }

    /* Check the tasks' results. */
    if( TEST_PROTECT() )
    {
        for( i = 0; i < pkcs11testSIGN_VERIFY_TASK_COUNT; i++ )
        {
            if( xTaskParams[ i ].xTestResult != 0 )
            {
                configPRINTF( ( "SignVerify task %d returned failure.\r\n",
                                xTaskParams[ i ].xTaskNumber ) );
                TEST_FAIL();
            }
        }
    }
}


static CK_BYTE pc896BitInput[] = "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu";

static CK_BYTE pcSha256HashOf896BitInput[] =
{
    0xcf, 0x5b, 0x16, 0xa7, 0x78, 0xaf, 0x83, 0x80, 0x03, 0x6c, 0xe5, 0x9e, 0x7b, 0x04, 0x92, 0x37,
    0x0b, 0x24, 0x9b, 0x11, 0xe8, 0xf0, 0x7a, 0x51, 0xaf, 0xac, 0x45, 0x03, 0x7a, 0xfe, 0xe9, 0xd1
};


TEST( Full_PKCS11_CryptoOperation, AFQP_Digest )
{
    CK_RV xResult = 0;

    CK_MECHANISM xDigestMechanism;

    CK_BYTE pcDigestResult[ pcks11SHA256_DIGEST_LENGTH ] = { 0 };
    CK_ULONG ulDigestLength = 0;


    /* Hash with SHA256 */
    xDigestMechanism.mechanism = CKM_SHA256;

    xResult = pxGlobalFunctionList->C_DigestInit( xGlobalSession, &xDigestMechanism );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    /* Subtract one because this hash was performed on the characters without the null terminator. */
    xResult = pxGlobalFunctionList->C_DigestUpdate( xGlobalSession, pc896BitInput, sizeof( pc896BitInput ) - 1 );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    /* Call C_DigestFinal on a NULL buffer to get the buffer length required. */
    xResult = C_DigestFinal( xGlobalSession, NULL, &ulDigestLength );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );
    TEST_ASSERT_EQUAL( pcks11SHA256_DIGEST_LENGTH, ulDigestLength );

    xResult = C_DigestFinal( xGlobalSession, pcDigestResult, &ulDigestLength );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );
    TEST_ASSERT_EQUAL_INT8_ARRAY( pcSha256HashOf896BitInput, pcDigestResult, pcks11SHA256_DIGEST_LENGTH );
}


TEST( Full_PKCS11_CryptoOperation, AFQP_Digest_ErrorConditions )
{
    CK_RV xResult = 0;
    CK_MECHANISM xDigestMechanism;
    CK_BYTE pcDigestResult[ pcks11SHA256_DIGEST_LENGTH ] = { 0 };
    CK_ULONG ulDigestLength = 0;

    /* Make sure that no NULL pointers in functions to be called in this test. */
    TEST_ASSERT_NOT_EQUAL( NULL, pxGlobalFunctionList->C_DigestInit );
    TEST_ASSERT_NOT_EQUAL( NULL, pxGlobalFunctionList->C_DigestUpdate );
    TEST_ASSERT_NOT_EQUAL( NULL, pxGlobalFunctionList->C_DigestFinal );

    /* Invalid hash mechanism. */
    xDigestMechanism.mechanism = CKM_MD5;

    xResult = C_DigestInit( xGlobalSession, &xDigestMechanism );
    TEST_ASSERT_EQUAL( CKR_MECHANISM_INVALID, xResult );

    /* Null Session. */
    xDigestMechanism.mechanism = CKM_SHA256;
    xResult = C_DigestInit( ( CK_SESSION_HANDLE ) NULL, &xDigestMechanism );
    TEST_ASSERT_EQUAL( CKR_SESSION_HANDLE_INVALID, xResult );

    /* Make sure that digest update fails if DigestInit did not succeed. */
    xResult = C_DigestUpdate( xGlobalSession, pc896BitInput, sizeof( pc896BitInput ) - 1 );
    TEST_ASSERT_EQUAL( CKR_OPERATION_NOT_INITIALIZED, xResult );

    /* Initialize the session properly. */
    xResult = C_DigestInit( xGlobalSession, &xDigestMechanism );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    /* Try to update digest with a NULL session handle. */
    xResult = C_DigestUpdate( ( CK_SESSION_HANDLE ) NULL, pc896BitInput, sizeof( pc896BitInput ) - 1 );
    TEST_ASSERT_EQUAL( CKR_SESSION_HANDLE_INVALID, xResult );

    /* DigestUpdate correctly.  Note that digest is not terminated because we didn't tell the session handle last time. */
    xResult = C_DigestUpdate( xGlobalSession, pc896BitInput, sizeof( pc896BitInput ) - 1 );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );

    /* Call C_DigestFinal on a buffer that is too small. */
    ulDigestLength = pcks11SHA256_DIGEST_LENGTH - 1;
    xResult = C_DigestFinal( xGlobalSession, pcDigestResult, &ulDigestLength );
    TEST_ASSERT_EQUAL( CKR_BUFFER_TOO_SMALL, xResult );

    /* Call C_DigestFinal on a NULL session handle. */
    ulDigestLength = pcks11SHA256_DIGEST_LENGTH;
    xResult = C_DigestFinal( ( CK_SESSION_HANDLE ) NULL, pcDigestResult, &ulDigestLength );
    TEST_ASSERT_EQUAL( CKR_SESSION_HANDLE_INVALID, xResult );

    /* Call C_DigestFinal on a proper buffer size. Note that Digest is not terminated if error is "buffer too small" or if session handle wasn't present. */
    ulDigestLength = pcks11SHA256_DIGEST_LENGTH;
    xResult = C_DigestFinal( xGlobalSession, pcDigestResult, &ulDigestLength );
    TEST_ASSERT_EQUAL( CKR_OK, xResult );
    TEST_ASSERT_EQUAL_INT8_ARRAY( pcSha256HashOf896BitInput, pcDigestResult, pcks11SHA256_DIGEST_LENGTH );

    /* Call C_DigestUpdate after the digest operation has been completed. */
    xResult = C_DigestUpdate( xGlobalSession, pc896BitInput, sizeof( pc896BitInput ) - 1 );
    TEST_ASSERT_EQUAL( CKR_OPERATION_NOT_INITIALIZED, xResult );
}
/*-----------------------------------------------------------*/

TEST( Full_PKCS11_CryptoOperation, AFQP_KeyGenerationEcdsaHappyPath )
{
    CK_RV xResult = 0;
    CK_ULONG ulCount = 0;
    CK_MECHANISM xMech = { 0 };
    CK_OBJECT_HANDLE xPublicKey = 0;
    CK_OBJECT_HANDLE xPrivateKey = 0;

    PKCS11_GenerateKeyPrivateTemplate_t xPrivateTemplate;
    PKCS11_GenerateKeyPublicTemplate_t xPublicTemplate;

    xPrivateTemplate.xLabel = ( CK_ATTRIBUTE ) {
        CKA_LABEL, &pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS, sizeof( pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS )
    };
    xPublicTemplate.xEcParams = ( CK_ATTRIBUTE ) {
        CKA_EC_PARAMS, pkcs11ELLIPTIC_CURVE_NISTP256, 1 + strlen( pkcs11ELLIPTIC_CURVE_NISTP256 )
    };
    xPublicTemplate.xLabel = ( CK_ATTRIBUTE ) {
        CKA_LABEL, &pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS, sizeof( pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS )
    };

    CK_BYTE pucMessage[ cryptoSHA256_DIGEST_BYTES ] = { 0 };
    CK_BYTE pucHash[ cryptoSHA256_DIGEST_BYTES ] = { 0 };
    CK_BYTE pucSignature[ 256 ] = { 0 };

    /* Hash the message (the null input). */
    ( void ) mbedtls_sha256_ret( pucMessage, 0, pucHash, 0 );

    /* Set crypto mechanism for key generation. */
    xMech.mechanism = CKM_EC_KEY_PAIR_GEN;

    /* Create a new key pair. */
    if( 0 == xResult )
    {
        xResult = pxGlobalFunctionList->C_GenerateKeyPair( xGlobalSession,
                                                           &xMech,
                                                           ( CK_ATTRIBUTE_PTR ) &xPublicTemplate,
                                                           sizeof( xPublicTemplate ) / sizeof( CK_ATTRIBUTE ),
                                                           ( CK_ATTRIBUTE_PTR ) &xPrivateTemplate,
                                                           sizeof( xPrivateTemplate ) / sizeof( CK_ATTRIBUTE ),
                                                           &xPublicKey,
                                                           &xPrivateKey );
    }

    /* Sign a hash. */
    if( 0 == xResult )
    {
        xMech.mechanism = CKM_ECDSA;
        xResult = pxGlobalFunctionList->C_SignInit( xGlobalSession,
                                                    &xMech,
                                                    xPrivateKey );
    }

    if( 0 == xResult )
    {
        ulCount = sizeof( pucSignature );
        xResult = pxGlobalFunctionList->C_Sign( xGlobalSession,
                                                pucHash,
                                                sizeof( pucHash ),
                                                pucSignature,
                                                &ulCount );
    }

    /* Verify the signature. */
    if( 0 == xResult )
    {
        xResult = pxGlobalFunctionList->C_VerifyInit( xGlobalSession,
                                                      &xMech,
                                                      xPublicKey );
    }

    if( 0 == xResult )
    {
        xResult = pxGlobalFunctionList->C_Verify( xGlobalSession,
                                                  pucHash,
                                                  sizeof( pucHash ),
                                                  pucSignature,
                                                  ulCount );
    }

    TEST_ASSERT_EQUAL_INT32( CKR_OK, xResult );
}

/*-----------------------------------------------------------*/
