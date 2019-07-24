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

class OtaTestCorruptImageBeforeSigning( OtaTestCase ):
    """This test uploads and OTA's a non-runnable image before code signing.
    """
    NAME = "OtaTestCorruptImageBeforeSigning"
    def __init__(self, boardConfig, otaProject, otaAwsAgent, flashComm):
        super(OtaTestCorruptImageBeforeSigning, self).__init__(
            OtaTestCorruptImageBeforeSigning.NAME, 
            boardConfig,
            otaProject, 
            otaAwsAgent, 
            flashComm
        )

        # Create a large-ish file that is not a working binary image.
        self.bad_image_fn = 'broken_demo.bin'
        self.bad_image_path = self.bad_image_fn
        outfile = open(self.bad_image_path, 'wb')
        oroboros = "the end is never "
        for i in range(0, 2500):
          outfile.write(oroboros.encode())
        outfile.close()

    def getName(self):
        return self._name
    
    def run(self):
        # Upload the bad image to s3.
        self._otaAwsAgent.uploadFirmwareToS3Bucket(
            self.bad_image_path,
            self.bad_image_fn
        )
        if self._otaAwsAgent._stageParams:
            # Create a job.
            otaUpdateId = self._otaAwsAgent.createOtaUpdate(
                deploymentFiles = [
                    {
                        'fileName': self._otaConfig['device_firmware_file_name'],
                        'fileVersion': '1',
                        'fileLocation': {
                            's3Location': {
                                'bucket': self._otaConfig['aws_s3_bucket_name'],
                                'key': self.bad_image_fn,
                                'version': self._otaAwsAgent.getS3ObjectVersion(self.bad_image_fn)
                            }
                        },
                        'codeSigning': { 
                            "startSigningJobParameter": {
                                'signingProfileName': "%s%s"%(self._otaConfig['aws_signer_certificate_arn'][-10:], self._otaAwsAgent._boardName[:10]),
                                'signingProfileParameter': {
                                    'platform': self._otaConfig['aws_signer_platform'],
                                    'certificateArn': self._otaConfig['aws_signer_certificate_arn'],
                                    'certificatePathOnDevice': self._otaConfig['aws_signer_certificate_file_name']
                                }
                            }
                        }
                    },
                ]
            )
        else:
            # Sign the corrupted image in s3 from the unsigned bucket to the signed bucket.
            signerJobId = self._otaAwsAgent.signFirmwareInS3Bucket(
                self.bad_image_fn,
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