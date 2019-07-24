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
class OtaTestResult:
    """Object representing a OTA test result.
    Attributes
        board(str): The name of the board this result is for.
        testName(str): The name of the OTA test.
        result(str): The result of the OTA test.
        reason(str): The reason for the result.
    Methods:
        testResultFromJobStatus(passOrFail, jobStatus)
    Example:
        passingResult = OtaTestResult(OtaTestResult.PASS, 'accepted')
        failingResult = OtaTestResult(OtaTestResult.FAIL, 'rejected')
    """
    PASS = 'PASS'
    FAIL = 'FAIL'
    def __init__(self, *, testName, result, reason, board=''):
        self.board = ''
        self.testName = testName
        self.result = result
        self.reason = reason

    def testResultFromJobStatus(testName, passOrFail, jobStatus):
        """Quickly turn the Test result from OtaAwsAgent into a OtaTest Result.
        Args:
            testName(str): The name of the test case.
            passOrFail(str): OtaTestResult.PASS or OtaTestResult.FAIL
            jobStatus(nametuple(status reason)): This is typically from OtaAwsAgent.pollOtaUpdateCompletion() or OtaAwsAgent.__getJobStatus().
        Returns an OtaTestResult.
        """
        if passOrFail == OtaTestResult.PASS:
            return OtaTestResult(testName=testName, result=OtaTestResult.PASS, reason=jobStatus.reason)
        else:
            return OtaTestResult(testName=testName, result=OtaTestResult.FAIL, reason=jobStatus.status + ': ' + jobStatus.reason)
    staticmethod = testResultFromJobStatus