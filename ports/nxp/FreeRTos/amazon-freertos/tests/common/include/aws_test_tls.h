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
#ifndef _AWS_TLS_TEST_H_
#define _AWS_TLS_TEST_H_

/*
* PEM-encoded client certificate.
*
* Certificate for P-256 elliptic curve key.
*/
static const char tlstestCLIENT_CERTIFICATE_PEM_EC[] = "Paste client certificate here.";

/*
* PEM-encoded client private key.
*
* This is a P-256 elliptic curve key.
*/
static const char tlstestCLIENT_PRIVATE_KEY_PEM_EC[] = "Paste client private key here.";

/* One character of this certificate has been changed in the issuer
 * name from Amazon Web Services to Amazon Web Cervices. */
static const char tlstestCLIENT_CERTIFICATE_PEM_MALFORMED[] = "Paste client certificate here.";

/* Certificate which is not trusted by the broker. */
static const char tlstestCLIENT_UNTRUSTED_CERTIFICATE_PEM[] = "Paste client certificate here.";

/* Private key corresponding to the untrusted certificate. */
static const char tlstestCLIENT_UNTRUSTED_PRIVATE_KEY_PEM[] = "Paste client private key here.";

/* Device certificate created using BYOC instructions. */
static const char tlstestCLIENT_BYOC_CERTIFICATE_PEM[] = "Paste client certificate here.";

/* Device private key created using BYOC instructions. */
static const char tlstestCLIENT_BYOC_PRIVATE_KEY_PEM[] = "Paste client private key here.";

#endif
