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

class OtaTestIncorrectWifiPassword( OtaTestCase ):
    NAME = "OtaTestIncorrectWifiPassword"
    def __init__(self, boardConfig, otaProject, otaAwsAgent, flashComm):
        super(OtaTestIncorrectWifiPassword, self).__init__(
            OtaTestIncorrectWifiPassword.NAME, 
            boardConfig,
            otaProject, 
            otaAwsAgent, 
            flashComm
        )

    def getName(self):
        return self._name
    
    def run(self):
        # Increase the version of the OTA image.
        self._otaProject.setApplicationVersion(0, 9, 1)
        # Set the WiFi password to an invalid one.
        self._otaProject.setClientCredentialsForWifi('invalid_ssid', 'invalid_password', 'eWiFiSecurityWPA2')
        # Build the project
        self._otaProject.buildProject()
        # Start an OTA Update.
        otaUpdateId = self._otaAwsAgent.quickCreateOtaUpdate(self._otaConfig)
        # Reset the Wifi credentials to valid values.
        self._otaProject.setClientCredentialsForWifi(self._boardConfig['wifi_ssid'],
                                                     self._boardConfig['wifi_password'],
                                                     self._boardConfig['wifi_security'])
        return self.getTestResultAfterOtaUpdateCompletion(otaUpdateId)

    def getTestResult(self, jobStatus, log):
        if (jobStatus.status == 'FAILED'):
            return OtaTestResult.testResultFromJobStatus(self._name, OtaTestResult.PASS, jobStatus)
        else:
            return OtaTestResult.testResultFromJobStatus(self._name, OtaTestResult.FAIL, jobStatus)