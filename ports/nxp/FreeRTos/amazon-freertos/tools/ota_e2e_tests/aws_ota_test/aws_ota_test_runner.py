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
import os
import traceback
from .aws_flash_serial_comm import FlashSerialComm
from .aws_ota_project import OtaAfrProject
from .aws_ota_aws_agent import OtaAwsAgent
from .aws_ota_test_case_factory import OtaTestCaseFactory
from .aws_ota_test_result import OtaTestResult

class OtaTestRunner:
    """Run all of the OTA tests.
    Before we run the tests we need to initialize all of the resource needed by the tests.
    There is one test runner per board. 
    Args:
        boardConfig(dict): The full board.json configuration.
        stage(dict): What development environment stage the AWS service stack is in to run OTA tests.
    Attributes:
        _flashComm(obj:FlashSerialComm): MCU flash and serial communication resource.
        _otaProject(obj:OtaAfrProject): Amazon FreeRTOS code resource.
        _otaAwsAgent(obj:OtaAwsAgent): Interface to AWS CLI resource.
        _otaTestCases(list(obj:OtaTestCase)): All of the OTA test cases in this run.
    Methods:
        runTests(): Run all of the tests found in the boardConfig during initialization.
        __initializeOtaProject(): Initialize _otaProject resource for testing.
        __getOtaTestCases(_otaConfig): Get all of the tests supported by the input board.
        __runTest(otaTestCase): Run a single test case.
        __cleanup(): Clean up all resource created during initialization.
    Example:
        testRunner = OtaTestRunner(boardConfig)
        testRunner.runTests()
    """
    def __init__(self, boardConfig, stageParams):
        self._boardConfig = boardConfig
        self._stageParams = stageParams
        self._otaConfig = boardConfig['ota_config']
        self._otaProject = OtaAfrProject(os.path.join(boardConfig['afr_root'], boardConfig['demos_or_tests']), boardConfig['vendor_board_path'], boardConfig['build_config'])
        self._otaAwsAgent = OtaAwsAgent(self._boardConfig['name'], self._otaConfig['aws_ota_update_role_arn'], self._otaConfig['aws_s3_bucket_name'], stageParams, True)
        # FlashSerialComm opens a thread. If there is an exception in OtaAwsAgent we want to exit the program, so this is initialized last.
        self._flashComm = FlashSerialComm(boardConfig['flash_config'], boardConfig['flash_config']['output'], self._otaConfig['device_firmware_file_name'])

        # Get the test cases from the board's ota config.
        self._otaTestCases = self.__getOtaTestCases(self._otaConfig)
        # Initialize the Amazon FreeRTOS project for OTA.
        self.__intializeOtaProject()

    def __intializeOtaProject(self):
        """Initialize the OtaAfrProject object for OTA."""
        try:
            self._otaProject.initializeOtaProject()
            self._otaProject.setClientCredentialsForAwsIotEndpoint(self._otaAwsAgent.getAwsIotEndpoint())
            self._otaProject.setClientCredentialsForWifi(self._boardConfig['wifi_ssid'], self._boardConfig['wifi_password'], self._boardConfig['wifi_security'])
            self._otaProject.setClientCredentialForThingName(self._otaAwsAgent.getThingName())
            self._otaProject.setClientCredentialKeys(self._otaAwsAgent.getThingCertificate(), self._otaAwsAgent.getThingPrivateKey())
            self._otaProject.copyCodesignerCertificateToBootloader(self._otaAwsAgent.getCodeSignerCertificateFromArn(self._otaConfig['aws_signer_certificate_arn']))
            self._otaProject.setMqttLogsOn()
            self._otaProject.setFreeRtosConfigNetworkInterface(self._boardConfig.get('windows_network_interface', 0))
            if self._otaConfig.get('compile_codesigner_certificate', False):
                self._otaProject.setCodesignerCertificate(self._otaAwsAgent.getCodeSignerCertificateFromArn(self._otaConfig['aws_signer_certificate_arn']))
            if self._stageParams:
                self._otaProject.setOtaDemoRunnerForSNIDisabled()
                # Currently 12/13/2018 only the TI CC3220SF LaunchpadXL needs the Root CA to connect to other internal Amazon development stage endpoints.
                if 'cc3220' in self._boardConfig['name']:
                    self._otaProject.setOtaUpdateDemoForRootCA()
                    self._otaProject.addRootCAToClientCredentialKeys(self._stageParams['certificate'])
                else:
                    self._otaProject.setOtaUpdateDemoForNullCertificate()
        except Exception as e:
            print(e)
            self.__cleanup()
            raise

    def __getOtaTestCases(self, _otaConfig):
        """Get all of the tests supported by the input board.
        Args:
            _otaConfig(dict): 'ota_config' in board.json containing the 'supported_tests' key.
        Returns a list of OtaTestCase objects.
        """
        otaTestCases = []
        try:
            for test in _otaConfig['supported_tests']:
                otaTestCases.append(
                    OtaTestCaseFactory.createTestCase(
                        test, 
                        self._boardConfig,
                        self._otaProject,
                        self._otaAwsAgent,
                        self._flashComm
                    )
                )
        except Exception as e:
            print(e)
            self.__cleanup()
            raise
        return otaTestCases

    def __runTest(self, otaTestCase):
        """
        - Build the code with the version desired. 
        - Flash the code built. 
        """
        testResult = OtaTestResult(testName='InvalidTest', result=OtaTestResult.FAIL, reason='Initialized test result.')
        testResult = otaTestCase.runTest()

        testResult.board = self._boardConfig['name']
        return testResult

    def runTests(self):
        """Run all tests this Test Runner object holds.
        Returns the results of the tests.
        """
        testResults = []
        for otaTestCase in self._otaTestCases:
            testResults.append(self.__runTest(otaTestCase))

        # All tests are over so clean up.
        self.__cleanup()
        return testResults

    def __cleanup(self):
        """Free resources used by the OTA agent and the flash serial object.
        """
        self._otaAwsAgent.cleanup()
        self._flashComm.cleanup()