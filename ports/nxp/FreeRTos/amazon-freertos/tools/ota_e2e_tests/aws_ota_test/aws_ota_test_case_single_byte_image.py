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
from .aws_ota_test_case import *
from .aws_ota_aws_agent import *
from .aws_ota_test_result import OtaTestResult

class OtaTestSingleByteImage( OtaTestCase ):
    NAME = "OtaTestSingleByteImage"
    def __init__(self, boardConfig, otaProject, otaAwsAgent, flashComm):
        super(OtaTestSingleByteImage, self).__init__(
            OtaTestSingleByteImage.NAME, 
            boardConfig,
            otaProject, 
            otaAwsAgent, 
            flashComm
        )

        # Create a large-ish file that is not a working binary image.
        self.singleByteFileName = 'singe_byte.bin'
        outfile = open(self.singleByteFileName, 'wb')
        charToWrite = "a"
        outfile.write(charToWrite.encode())

    def getName(self):
        return self._name
    
    def run(self):
        # Upload the bad image to s3.
        self._otaAwsAgent.uploadFirmwareToS3Bucket(
            self.singleByteFileName,
            self.singleByteFileName
        )

        # Sign the corrupted image in s3 from the unsigned bucket to the signed bucket.
        signerJobId = self._otaAwsAgent.signFirmwareInS3Bucket(
            self.singleByteFileName,
            self._otaConfig['aws_signer_certificate_arn'],
            self._otaConfig['aws_signer_certificate_file_name'],
            self._otaConfig['aws_signer_platform']
        )
        # Create a stream from the image in the signed image in the signed bucket.
        streamId = self._otaAwsAgent.createIotStream(
            self._otaAwsAgent.getS3BucketName(),
            signerJobId
        )
        # Create the OTA update job.
        otaUpdateId = self._otaAwsAgent.createOtaUpdate(
            deploymentFiles = [
                {
                    'fileName': self._otaConfig['device_firmware_file_name'],
                    'fileVersion': '1',
                    'fileLocation': {
                        'stream':{
                            'streamId': streamId,
                            'fileId': 0
                        },
                    },
                    'codeSigning': {
                        'awsSignerJobId': signerJobId
                    }
                },
            ]
        )

        return self.getTestResultAfterOtaUpdateCompletion(otaUpdateId)

    def getTestResult(self, jobStatus, log):
        if (jobStatus.status == 'FAILED'):
            return OtaTestResult.testResultFromJobStatus(self._name, OtaTestResult.PASS, jobStatus)
        else:
            return OtaTestResult.testResultFromJobStatus(self._name, OtaTestResult.FAIL, jobStatus)