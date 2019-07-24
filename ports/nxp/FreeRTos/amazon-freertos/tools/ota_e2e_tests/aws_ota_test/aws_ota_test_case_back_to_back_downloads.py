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

class OtaTestBackToBackDownloads( OtaTestCase ):
    NAME = 'OtaTestBackToBackDownloads'
    def __init__(self, boardConfig, otaProject, otaAwsAgent, flashComm):
        super(OtaTestBackToBackDownloads, self).__init__(
            OtaTestBackToBackDownloads.NAME, 
            boardConfig,
            otaProject, 
            otaAwsAgent, 
            flashComm
        )

    def getName(self):
        return self._name
    
    def __stopTest(self, jobStatus):
        self._flashComm.stopSerialRead()
        return self.getTestResult(jobStatus, self._flashComm.getSerialLog())

    def __buildAndOtaInputVersion(self, x, y, z):
        # Build x.y.z for download
        self._otaProject.setApplicationVersion(x, y, z)
        # Build the OTA image.
        self._otaProject.buildProject()
        # Start an OTA Update.
        otaUpdateId = self._otaAwsAgent.quickCreateOtaUpdate(self._otaConfig)
        # Poll on completion
        jobStatus = self._otaAwsAgent.pollOtaUpdateCompletion(otaUpdateId, self._otaConfig['ota_timeout_sec'])
        return jobStatus

    def run(self):
        # Build 0.9.1 for download
        jobStatus = self.__buildAndOtaInputVersion(0, 9, 1)
        if jobStatus.status != 'SUCCEEDED':
            return self.__stopTest(jobStatus)

        # Build 0.9.2 for download
        jobStatus = self.__buildAndOtaInputVersion(0, 9, 2)
        if jobStatus.status != 'SUCCEEDED':
            return self.__stopTest(jobStatus)


        # Build 0.9.3 for download
        jobStatus = self.__buildAndOtaInputVersion(0, 9, 3)
        if jobStatus.status != 'SUCCEEDED':
            return self.__stopTest(jobStatus)

        return OtaTestResult(
            testName=self._name,
            result=OtaTestResult.PASS,
            reason="all builds and downloads succeeded."
        )


    def getTestResult(self, jobStatus, log):
        if (jobStatus.status == 'SUCCEEDED'):
            return OtaTestResult.testResultFromJobStatus(self._name, OtaTestResult.PASS, jobStatus)
        else:
            return OtaTestResult.testResultFromJobStatus(self._name, OtaTestResult.FAIL, jobStatus)