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
import os
from .aws_ota_test_result import OtaTestResult

class OtaTestUntrustedCertificate( OtaTestCase ):
    NAME = "OtaTestUntrustedCertificate"
    def __init__(self, boardConfig, otaProject, otaAwsAgent, flashComm):
        super(OtaTestUntrustedCertificate, self).__init__(
            OtaTestUntrustedCertificate.NAME, 
            boardConfig,
            otaProject, 
            otaAwsAgent, 
            flashComm
        )
        # Define invalid/valid signing certificates for later use.
        self._validSignerArn = "%s"%self._otaConfig['aws_signer_certificate_arn']
        self._bogusSignerArn = "%s"%self._otaConfig['aws_untrusted_signer_certificate_arn']
        # Set a semi-unique 'signing type' name based on the invalid certificate's ARN to avoid conflicts.
        self._signingProfileName = "%s%s"%(self._bogusSignerArn[-10:], self._otaAwsAgent._boardName[:10])

    def getName(self):
        return self._name
    
    def run(self):
        # Increase the version of the OTA image.
        self._otaProject.setApplicationVersion(0, 9, 1)
        # Use an invalid signing certificate.
        self._otaConfig['aws_signer_certificate_arn'] = self._bogusSignerArn
        # Build the OTA image.
        self._otaProject.buildProject()
        # Run the test.

        # Upload the OTA image to s3.
        self._otaAwsAgent.uploadFirmwareToS3Bucket(
            self._otaConfig['ota_firmware_file_path'],
            os.path.basename(self._otaConfig['ota_firmware_file_path'])
        )
        # Create a job.
        if self._otaAwsAgent._stageParams:
            otaUpdateId = self._otaAwsAgent.createOtaUpdate(
                deploymentFiles = [
                    {
                        'fileName': os.path.basename(self._otaConfig['ota_firmware_file_path']),
                        'fileVersion': '1',
                        'fileLocation': {
                            's3Location': {
                                'bucket': self._otaConfig['aws_s3_bucket_name'],
                                'key': os.path.basename(self._otaConfig['ota_firmware_file_path']),
                                'version': self._otaAwsAgent.getS3ObjectVersion(os.path.basename(self._otaConfig['ota_firmware_file_path']))
                            }
                        },
                        'codeSigning': { 
                            "startSigningJobParameter": {
                                'signingProfileName': self._signingProfileName,
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
                os.path.basename(self._otaConfig['ota_firmware_file_path']),
                self._otaConfig['aws_signer_certificate_arn'],
                self._otaConfig['aws_signer_certificate_file_name'],
                self._otaConfig['aws_signer_platform'],
                self._signingProfileName
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

        # Reset to a valid signing key.
        self._otaConfig['aws_signer_certificate_arn'] = self._validSignerArn
        # Wait for the job to complete.
        return self.getTestResultAfterOtaUpdateCompletion(otaUpdateId)

    def getTestResult(self, jobStatus, log):
        if (jobStatus.status == 'FAILED'):
            # (Overwrite the confusing hex message presented as the 'jobStatus' on this type of failure)
            job_status = namedtuple('JobStatus', 'status reason')('PASS', 'Invalid signing certificate test passed.')
            return OtaTestResult.testResultFromJobStatus(self._name, OtaTestResult.PASS, job_status)
        else:
            return OtaTestResult.testResultFromJobStatus(self._name, OtaTestResult.FAIL, jobStatus)
