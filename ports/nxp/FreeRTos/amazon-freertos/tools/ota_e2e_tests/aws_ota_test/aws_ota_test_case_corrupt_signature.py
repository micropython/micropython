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
import json

class OtaTestCorruptSignature( OtaTestCase ):
    NAME = 'OtaTestCorruptSignature'
    def __init__(self, boardConfig, otaProject, otaAwsAgent, flashComm):
        super(OtaTestCorruptSignature, self).__init__(
            OtaTestCorruptSignature.NAME, 
            boardConfig,
            otaProject, 
            otaAwsAgent, 
            flashComm
        )
    
    def run(self):
        fileName = "temp_signed_signature_to_corrupt.json"

        # Increase the version of the OTA image.
        self._otaProject.setApplicationVersion(0, 9, 1)
        # Build the OTA image.
        self._otaProject.buildProject()

        # Upload the image to s3.
        self._otaAwsAgent.uploadFirmwareToS3Bucket(
            self._otaConfig['ota_firmware_file_path'],
            os.path.basename(self._otaConfig['ota_firmware_file_path'])
        )

        # Sign the image in S3.
        signerJobId = self._otaAwsAgent.signFirmwareInS3Bucket(
            os.path.basename(self._otaConfig['ota_firmware_file_path']),
            self._otaConfig['aws_signer_certificate_arn'],
            self._otaConfig['aws_signer_certificate_file_name'],
            self._otaConfig['aws_signer_platform']
        )

        # Corrupt the payload of the signed image.
        self._otaAwsAgent.downloadS3File(
            signerJobId,
            fileName
        )

        # The signed image is a JSON so we can load it then corrupt it.
        signedImage = {}
        with open(fileName, 'r+') as signedImageFile:
            signedImageJson = json.load(signedImageFile)
            for i in range(0, 13):
                signedImageJson['signature'] = signedImageJson['signature'].replace(chr(ord('A') + i), chr(ord('Z') - i))
            signedImageFile.seek(0)
            json.dump(signedImageJson, signedImageFile)
            signedImageFile.truncate()
        
        # Upload the file back to s3
        self._otaAwsAgent.uploadFirmwareToS3Bucket(
            fileName,
            signerJobId
        )

        # Create a stream.
        streamId = self._otaAwsAgent.createIotStream(
            self._otaAwsAgent.getS3BucketName(),
            signerJobId
        )

        # Create a job.
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
        if (jobStatus.status != 'FAILED'):
            return OtaTestResult(testName=self._name, result=OtaTestResult.FAIL, reason='AWS Job Status: ' + jobStatus.status + ", reason: " + jobStatus.reason)
        else:
            return OtaTestResult(testName=self._name, result=OtaTestResult.PASS, reason='')