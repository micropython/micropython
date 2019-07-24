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
 * copies or substantial portions of the Software. If you wish to use our Amazon
 * FreeRTOS name, please do so in a fair use way that does not cause confusion.
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

#ifndef __AWS_CODESIGN_KEYS__H__
#define __AWS_CODESIGN_KEYS__H__

#include "aws_test_ota_config.h"

/*
 * PEM-encoded code signer certificate
 *
 * Must include the PEM header and footer:
 * "-----BEGIN CERTIFICATE-----"
 * "...base64 data..."
 * "-----END CERTIFICATE-----";
 */
#if (otatestSIG_METHOD == otatestSIG_SHA256_ECDSA)
    static const char signingcredentialSIGNING_CERTIFICATE_PEM[] =
        "-----BEGIN CERTIFICATE-----\n"
        "MIIBXDCCAQOgAwIBAgIJAPMhJT8l0C6AMAoGCCqGSM49BAMCMCExHzAdBgNVBAMM\n"
        "FnRlc3Rfc2lnbmVyQGFtYXpvbi5jb20wHhcNMTgwNjI3MjAwNDQyWhcNMTkwNjI3\n"
        "MjAwNDQyWjAhMR8wHQYDVQQDDBZ0ZXN0X3NpZ25lckBhbWF6b24uY29tMFkwEwYH\n"
        "KoZIzj0CAQYIKoZIzj0DAQcDQgAEyza/tGLVbVxhL41iYtC8D6tGEvAHu498gNtq\n"
        "DtPsKaoR3t5xQx+6zdWiCi32fgFT2vkeVAmX3pf/Gl8nIP48ZqMkMCIwCwYDVR0P\n"
        "BAQDAgeAMBMGA1UdJQQMMAoGCCsGAQUFBwMDMAoGCCqGSM49BAMCA0cAMEQCIDkf\n"
        "83Oq8sOXhSyJCWAN63gc4vp9//RFCXh/hUXPYcTWAiBgmQ5JV2MZH01Upi2lMflN\n"
        "YLbC+lYscwcSlB2tECUbJA==\n"
        "-----END CERTIFICATE-----\n";
#elif (otatestSIG_METHOD == otatestSIG_SHA256_RSA)
    static const char signingcredentialSIGNING_CERTIFICATE_PEM[] =
        "-----BEGIN CERTIFICATE-----\n"
        "MIIC6TCCAdGgAwIBAgIJAJmG5mbNWlukMA0GCSqGSIb3DQEBCwUAMCExHzAdBgNV\n"
        "BAMMFnRlc3Rfc2lnbmVyQGFtYXpvbi5jb20wHhcNMTgwNjI3MjAwMDIyWhcNMTkw\n"
        "NjI3MjAwMDIyWjAhMR8wHQYDVQQDDBZ0ZXN0X3NpZ25lckBhbWF6b24uY29tMIIB\n"
        "IjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAt8SVb/B4pRHAsxtK5Xcm1iBK\n"
        "BIpiUc/ovUPzhGVlvkeZJfV2hrHc8PJx+XMyxp/MQGI1nr0AfHKkmAbisfU15/ta\n"
        "foY1D9Skomlp+QKCK4UvJCKRl/vgdgKtZ6+vEls4tguBmt0pwQsEhNP2TDUA47uq\n"
        "n7wcvgEwrrDLa9cmP7bfxRbVTWCKdchllOnRgLc05lQpRGxyMaB8zsgmx27AJlJI\n"
        "rXFmnSiOKKGo6Di9/UQTLTzC+t6YfajMSpq+Fe+td7hv/vSsZT+5yLSpJDwmHTp5\n"
        "66AljEEv0vfcJ72dStacEoOKM/qX4dfQfQOF3nyeP6ofgakc+zEjncQPCoCLUQID\n"
        "AQABoyQwIjALBgNVHQ8EBAMCB4AwEwYDVR0lBAwwCgYIKwYBBQUHAwMwDQYJKoZI\n"
        "hvcNAQELBQADggEBAHjgDAj+U4SpHIaxfvKuNxxPiGnyUSWyXS2h5HiPixL85psS\n"
        "k5hvbZaFJwRICSX1dU9EAf2sQN+s2MCLKPnm8vzKWLkyEUzWScr7C/mVXw8cEmdI\n"
        "rd3uzsN4IThHxHiwhPXu2IjjoIjSXLHcTKyZR2uHaifXbVEEwmUimg/49of02eIa\n"
        "ouLySI/+zcLgNFtTklRdSUxxGVK9y47ufiVb+sErzfDBpYx7CwZg0d+wPgfbuYE2\n"
        "8GMrCwFUbsFGMvhscPwG51rFLNonWh+ThZIwHrBwnVXmsDXayBlr6wCA50zGfBQb\n"
        "SXVRXIv/SKpj1k1slnf9IJLfArH6F0Ctjcef/Aw=\n"
        "-----END CERTIFICATE-----";
#elif (otatestSIG_METHOD == otatestSIG_SHA1_RSA)
    static const char signingcredentialSIGNING_CERTIFICATE_PEM[] =
        "-----BEGIN CERTIFICATE-----\n"
        "MIIDPDCCAiSgAwIBAgIBATANBgkqhkiG9w0BAQUFADB8MQswCQYDVQQGEwJJTDEP\n"
        "MA0GA1UECAwGU2hhcm9uMRAwDgYDVQQHDAdSYWFuYW5hMR8wHQYDVQQKDBZUZXhh\n"
        "cyBJbnN0cnVtZW50cyBJbmMuMQwwCgYDVQQLDANSTkQxGzAZBgNVBAMMEmR1bW15\n"
        "LXJvb3QtY2EtY2VydDAeFw0xODA2MjcxOTI4NTNaFw0xOTA2MjcxOTI4NTNaMCEx\n"
        "HzAdBgNVBAMMFnRlc3Rfc2lnbmVyQGFtYXpvbi5jb20wggEiMA0GCSqGSIb3DQEB\n"
        "AQUAA4IBDwAwggEKAoIBAQC5craIexo46oq3OTj+3fLz17MCtw/icXku8sJ16el0\n"
        "29Cibq4u8bdJeDnGlc42UySw0UlodWnzd5I52tXoXcQZs++m6uQ3JvvRHFXpNuV6\n"
        "Cyo//AdQ2MoqdKK8/pZqu4X71/xXHcIMBb3kS1RSkunJB65mPuPOfjcem6IXDtRk\n"
        "E346aN6wMfeIlmwOoFK4RBoWse459ELB5aq0tSwUmHv7UcyHYQf4Cxkz5HjWfx0y\n"
        "NYKCR4tPoudjg8E9R6uqik7DFlqrijNW2XdXS67d/cVToDJFn1SnYcHStARAoZUK\n"
        "gP0JwXTapO31QThi9q+q63FETBcnEQ29rfmeN1/pBOI/AgMBAAGjJDAiMAsGA1Ud\n"
        "DwQEAwIHgDATBgNVHSUEDDAKBggrBgEFBQcDAzANBgkqhkiG9w0BAQUFAAOCAQEA\n"
        "W29xvDV66B7NYAOSqXIG11JH+Pk2GFzZl5gNehkcDPPMHumBRNHHZqVAO4mah3Ej\n"
        "StqWmTXnIs5YKX9Ro7ch3NfKHMt4/JFvR7v9VolVIVwZZB+AQU1BD8OAGlHNEq28\n"
        "TbOdkpdv9Q69W4TVsqXAkVhONekLkEJQTZyhW7db28nb/LizftfN4ps+uuE2Xl9c\n"
        "YHmgWb/xqi9NIcsyQL08urJVCnyGuLQgj+GfitELFsCfc3ohhacNENsXupRIOz08\n"
        "NCa9WuCyk5uwoo6mn6JIErBMLqLTBcs82vq9d7WIFHf4QpgTs2FuelY/Hyw7HRFo\n"
        "Ml3tXnR4B4lqeJy/BP6/GA==\n"
        "-----END CERTIFICATE-----";
#else
    static const char signingcredentialSIGNING_CERTIFICATE_PEM[] = "Paste code signing certificate here.";
#endif

#endif /* ifndef __AWS_CODESIGN_KEYS__H__ */
