"""
Amazon FreeRTOS
Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 
http://aws.amazon.com/freertos
http://www.FreeRTOS.org 

"""
from .aws_ota_test_case_greater_version import OtaTestGreaterVersion
from .aws_ota_test_case_unsigned_image import OtaTestUnsignedImage
from .aws_ota_test_case_same_version import OtaTestSameVersion
from .aws_ota_test_case_untrusted_certificate import OtaTestUntrustedCertificate
from .aws_ota_test_case_corrupt_image_before_signing import OtaTestCorruptImageBeforeSigning
from .aws_ota_test_case_previous_version import OtaTestPreviousVersion
from .aws_ota_test_case_corrupt_image_after_signing import OtaTestCorruptImageAfterSigning
from .aws_ota_test_case_corrupt_signature import OtaTestCorruptSignature
from .aws_ota_test_case_single_byte_image import OtaTestSingleByteImage
from .aws_ota_test_case_missing_filename import OtaTestMissingFilename
from .aws_ota_test_case_incorrect_platform import OtaTestIncorrectPlatform
from .aws_ota_test_case_back_to_back_downloads import OtaTestBackToBackDownloads
from .aws_ota_test_case_incorrect_wifi_password import OtaTestIncorrectWifiPassword
from .aws_ota_test_case_dummy_test import OtaTestDummyTest

"""
All OTA test cases in the system.
"""
AllOtaTestCases = {
    OtaTestGreaterVersion.NAME : OtaTestGreaterVersion,
    OtaTestUnsignedImage.NAME : OtaTestUnsignedImage,
    OtaTestSameVersion.NAME: OtaTestSameVersion,
    OtaTestUntrustedCertificate.NAME : OtaTestUntrustedCertificate,
    OtaTestCorruptImageBeforeSigning.NAME : OtaTestCorruptImageBeforeSigning,
    OtaTestPreviousVersion.NAME : OtaTestPreviousVersion,
    OtaTestCorruptImageAfterSigning.NAME : OtaTestCorruptImageAfterSigning,
    OtaTestCorruptSignature.NAME : OtaTestCorruptSignature,
    OtaTestIncorrectPlatform.NAME : OtaTestIncorrectPlatform,
    OtaTestSingleByteImage.NAME : OtaTestSingleByteImage,
    OtaTestMissingFilename.NAME : OtaTestMissingFilename,
    OtaTestBackToBackDownloads.NAME : OtaTestBackToBackDownloads,
    OtaTestIncorrectWifiPassword.NAME : OtaTestIncorrectWifiPassword,
    OtaTestDummyTest.NAME : OtaTestDummyTest
}

class OtaTestCaseFactory( object ):
    """Return OTA test case objects based on the test name.
    """
    def createTestCase(test, boardConfig, otaProject, otaAwsAgent, flashComm):
        return AllOtaTestCases[test](boardConfig, otaProject, otaAwsAgent, flashComm)
    createTestCase = staticmethod(createTestCase)
