/*
 * Amazon FreeRTOS
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
#ifndef AWS_TEST_TCP_H
#define AWS_TEST_TCP_H


// #define ROUTER1
// #define ROUTER2
#define ROUTER3


/* Non-Encrypted Echo Server.
 * Update tcptestECHO_SERVER_ADDR# and
 * tcptestECHO_PORT with IP address
 * and port of unencrypted TCP echo server. */
#if defined(ROUTER1)
#define tcptestECHO_SERVER_ADDR0    192
#define tcptestECHO_SERVER_ADDR1    168
#define tcptestECHO_SERVER_ADDR2    12
#define tcptestECHO_SERVER_ADDR3    1
#define tcptestECHO_PORT            ( 8883 )
#elif defined(ROUTER2)
#define tcptestECHO_SERVER_ADDR0    192
#define tcptestECHO_SERVER_ADDR1    168
#define tcptestECHO_SERVER_ADDR2    0
#define tcptestECHO_SERVER_ADDR3    100
#define tcptestECHO_PORT            ( 8883 )
#elif defined(ROUTER3)
#define tcptestECHO_SERVER_ADDR0    192
#define tcptestECHO_SERVER_ADDR1    168
#define tcptestECHO_SERVER_ADDR2    2
#define tcptestECHO_SERVER_ADDR3    100
#define tcptestECHO_PORT            ( 8883 )
#endif


/* Encrypted Echo Server.
 * If tcptestSECURE_SERVER is set to 1, the following must be updated:
 * 1. aws_clientcredential.h to use a valid AWS endpoint.
 * 2. aws_clientcredential_keys.h with corresponding AWS keys.
 * 3. tcptestECHO_SERVER_TLS_ADDR0-3 with the IP address of an
 * echo server using TLS.
 * 4. tcptestECHO_PORT_TLS, with the port number of the echo server
 * using TLS.
 * 5. tcptestECHO_HOST_ROOT_CA with the trusted root certificate of the
 * echo server using TLS. */
#define tcptestSECURE_SERVER            1

#if defined(ROUTER1)
#define tcptestECHO_SERVER_TLS_ADDR0    192
#define tcptestECHO_SERVER_TLS_ADDR1    168
#define tcptestECHO_SERVER_TLS_ADDR2    12
#define tcptestECHO_SERVER_TLS_ADDR3    1
#define tcptestECHO_PORT_TLS            ( 443 )
#elif defined(ROUTER2)
#define tcptestECHO_SERVER_TLS_ADDR0    192
#define tcptestECHO_SERVER_TLS_ADDR1    168
#define tcptestECHO_SERVER_TLS_ADDR2    0
#define tcptestECHO_SERVER_TLS_ADDR3    100
#define tcptestECHO_PORT_TLS            ( 443 )
#elif defined(ROUTER3)
#define tcptestECHO_SERVER_TLS_ADDR0    192
#define tcptestECHO_SERVER_TLS_ADDR1    168
#define tcptestECHO_SERVER_TLS_ADDR2    2
#define tcptestECHO_SERVER_TLS_ADDR3    100
#define tcptestECHO_PORT_TLS            ( 443 )
#endif

/* Number of times to retry a connection if it fails. */
#define tcptestRETRY_CONNECTION_TIMES    3

/* The root certificate used for the encrypted echo server.
 * This certificate is self-signed, and not in the trusted catalog. */
static const char tcptestECHO_HOST_ROOT_CA[] = 
"-----BEGIN CERTIFICATE-----\n"
"MIID6zCCAtOgAwIBAgIJAIHTdkTq3UueMA0GCSqGSIb3DQEBCwUAMIGLMQswCQYD\n"
"VQQGEwJVUzELMAkGA1UECAwCV0ExDjAMBgNVBAcMBVBsYWNlMRQwEgYDVQQKDAtZ\n"
"b3VyQ29tcGFueTELMAkGA1UECwwCSVQxFjAUBgNVBAMMDXd3dy55b3Vycy5jb20x\n"
"JDAiBgkqhkiG9w0BCQEWFW1hcmlhbi5jaW5nZWxAbnhwLmNvbTAeFw0xODA0MDUw\n"
"OTQ3MTJaFw0xOTA0MDUwOTQ3MTJaMIGLMQswCQYDVQQGEwJVUzELMAkGA1UECAwC\n"
"V0ExDjAMBgNVBAcMBVBsYWNlMRQwEgYDVQQKDAtZb3VyQ29tcGFueTELMAkGA1UE\n"
"CwwCSVQxFjAUBgNVBAMMDXd3dy55b3Vycy5jb20xJDAiBgkqhkiG9w0BCQEWFW1h\n"
"cmlhbi5jaW5nZWxAbnhwLmNvbTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoC\n"
"ggEBAL9FfiLglU0AMd5HyGtrDWQIAtm4hGjZJ96vQbQvGecmGzfDvQQu2W8BkU9e\n"
"N7qCMczIhbVgFk9c32DEx0UHngXgJ/yRk4bGjZCDXrsrq41sWpLZIlGGET7A4ETz\n"
"69VBacr4h4CqZ+SN6takN5yWGF5nQ3brtBBCQWZUyoT3VjKQldKI3FCq/3os+t7V\n"
"6zVeCWC2W50q/pY+qBtMo8N66m9TEh5/11P4dFiKeioRasq7WiPiPMl6BirRorec\n"
"GPEe6WJiRjD2p8dHGLrQezzGpNURDghdNEei44+QC3ZWkVVs1K+8yryVFs4qIgq7\n"
"bOsu9cmjjdaPraR/Bw3XYDTcYrMCAwEAAaNQME4wHQYDVR0OBBYEFD5udhpjV1rH\n"
"NbC0QisHwLj0TsESMB8GA1UdIwQYMBaAFD5udhpjV1rHNbC0QisHwLj0TsESMAwG\n"
"A1UdEwQFMAMBAf8wDQYJKoZIhvcNAQELBQADggEBAKIyCf4d4vyk9SRZYrY+Drcp\n"
"eq+lZzJG9e4vFYhY3T+5nyDiCIyptOJCoYWhusURIWN3Azk/yfwfFNb/LVSvw+8g\n"
"Kq3QWU3hSOT+o4lqJvjQSNN3JTniV81wCua7j+uuwnTeIqZaEHs6nlZN8gDXxuOF\n"
"42052inkxJQpYc9VDFxo5bDLwQIgyxQ3H++QBAFGWq5H7IFdrVo4kMouCig6y/fW\n"
"Gxi+RiHCrChtre6yQh1eWKsV2sHfBGLliNNCPd1bYkdwSySzVC/UMWXNwNbwO4iT\n"
"0lK+FkzHcVm0/Re4575KVTU88ZsJNWYaDBodfeoJQMhJvU93ZKuqM4qA7arXBjY=\n"
"-----END CERTIFICATE-----\n";

#endif /* ifndef AWS_TEST_TCP_H */
