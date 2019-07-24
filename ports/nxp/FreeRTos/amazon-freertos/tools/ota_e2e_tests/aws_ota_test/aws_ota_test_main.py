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
import json
import sys
import argparse
from threading import Thread
from functools import reduce
from operator import add
from junit_xml import TestSuite, TestCase
from .aws_ota_test_runner import *
from .aws_ota_test_result import OtaTestResult

def parseArgs():
    """Parse input arguments.
    """
    parser = argparse.ArgumentParser(description='Run OTA End to End tests on configured board(s).')
    parser.add_argument('--board-config-dir', action='store', required=False, dest='boardConfigDir', help='The directory where the board configuration JSON files are located.')
    parser.add_argument('--board-configs', nargs='*', action='append', required=False, dest='boardConfigPaths', help='Path to a board configuration or many board configurations')
    parser.add_argument('--disable-boards', nargs='*', action='append', required=False, dest='disabledBoards', help='The list of boards to disable in testing. This option will exclude boards from the complete list. This option will be ignored if --boards is specified.')
    parser.add_argument('--boards', nargs='*', action='append', required=False, dest='enabledBoards', help='The list of boards to enable in testing. This option allows for selecting specific boards.')
    parser.add_argument('--disable-tests', nargs='*', action='append', required=False, dest='disabledTests', help='The list of OTA E2E test cases to disabled. This option will excluse tests from the complete list. This option will be ignored if --tests is specified.')
    parser.add_argument('--tests', nargs='*', action='append', required=False, dest='enabledTests', help='The list of OTA E2E test cases to enable. This option allows for selecting specific tests.')
    parser.add_argument('--separate-threads-per-board', action='store_true', dest='separateThreads', help='Create a separate thread per board configuration. WARNING: You must have \"afr_root\" paths specified per board, as writing and build the project is not synchronized.')
    parser.add_argument('--stage', action='store', required=False, default='prod', dest='stage', choices=['alpha','beta','gamma','prod'], help='If the OTA E2E scripts are to use the \'beta\', \'alpha\', or \'gamma\' AWS service stack, then specify which.')
    parser.add_argument('--endpoint-url', action='store', required=False, dest='endpointUrl', help='The url for the endpoint for AWS IoT CLI operations when --stage is specified.')
    parser.add_argument('--signer-endpoint-url', action='store', required=False, dest='signerEndpointUrl', help='On certain stages AWS signer needs an endpoint URL')
    parser.add_argument('--region', action='store', required=False, dest='region', help='The region for AWS CLI operations when --stage is specified.')
    parser.add_argument('--certificate', action='store', required=False, dest='certificatePath', help='The path to the PEM encoded secure connection certificate needed for stages other than Production.')
    args = parser.parse_args()

    return args

def cleanBoardConfigsForInputArgs(args, boardConfigs):
    """Clean the board.json configuration for the options in the program arguments.
    """
    if args.enabledBoards:
        boardConfigs = \
            list(filter(
                lambda boardConfig: boardConfig['name'] in args.enabledBoards[0], 
                boardConfigs
            ))
    if args.enabledTests:
        for boardConfig in boardConfigs:
            boardConfig['ota_config']['supported_tests'] = \
                list(filter(
                    lambda testName: testName in args.enabledTests[0],
                    boardConfig['ota_config']['supported_tests']
                ))
    if not args.enabledBoards and args.disabledBoards:
        boardConfigs = \
            list(filter(
                lambda boardConfig: boardConfig['name'] not in args.disabledBoards[0],
                boardConfigs
            ))
    if not args.enabledTests and args.disabledTests:
        for boardConfig in boardConfigs:
            boardConfig['ota_config']['supported_tests'] = \
                list(filter(
                    lambda testName: testName not in args.disabledTests[0],
                    boardConfig['ota_config']['supported_tests']
                ))
    return boardConfigs

def getBoardConfigsFromInputArgs(args):
    """Get all of the boards supporting OTA from input arguments.
    CONFIG_BOARD_CONFIG_DIR is defined in aws_ota_test_config.py. Define this to the
    directory containing all of your board configurations. This directory is walked for
    all board configurations.
    Returns the the board configs in the CONFIG_BOARD_CONFIG_DIR. 
    """
    boardConfigs = []
    if args.boardConfigDir != None:
        for rootDir, dirs, files in os.walk(args.boardConfigDir):
            for file in files:
                if file.endswith('.json'):
                    boardConfig = getBoardConfigFromFilePath(os.path.join(rootDir, file))
                    boardConfigs.append(boardConfig)

    if args.boardConfigPaths != None:
        for boardConfigPath in args.boardConfigPaths[0]:
            boardConfig = getBoardConfigFromFilePath(boardConfigPath)
            boardConfigs.append(boardConfig)

    if len(boardConfigs) == 0:
        raise Exception('ERROR: No board configurations found in input paths.')

    return boardConfigs

def getBoardConfigFromFilePath(boardConfigPath):
    boardConfig = {}
    with open(boardConfigPath, 'r') as boardFile:
        boardConfig = json.load(boardFile)
        if ( boardConfig['exclude'] == False ) and ( 'ota_config' in boardConfig ):
            # Format all references to other variables in the file.
            formatBoardConfig(boardConfig)
    return boardConfig

def formatBoardConfig(boardConfig):
    formatConfigValues(boardConfig, boardConfig)
    formatConfigValues(boardConfig['ota_config'], boardConfig, boardConfig['ota_config'])
    formatConfigValues(boardConfig['build_config'], boardConfig, boardConfig['build_config'])
    formatConfigValues(boardConfig['flash_config'], boardConfig, boardConfig['flash_config'])    

def formatConfigValues(targetConfig, formatConfig1, formatConfig2 = {}):
    """Format the target configuration field references with the input formatConfigs dictionary.
    Args:
        targetConfig(dict): Dictionary with strings to format.
        formatConfig1(dict): All of the dictionaries to replaces references in targetConfig with.
        formatConfig2(dict): Another dictionary to replace references in targetConfig with.
    """
    for key, value in targetConfig.items():
        if isinstance(value, str):
            value = value.format(**formatConfig1, **formatConfig2)
            targetConfig[key] = value

def getBoardOtaTestResult(boardConfig, stageParams, otaTestResults):
    """Get the OTA test case results from the board under test specified in the boardConfig.
    Args:
        boardConfig(dict): Configuration for the board under test.
        otaTestResults(list(obj:OtaTestResult)): Array of test results to update.
    """
    # Create a OtaTestRunner instance and call RunTests -> returns results.
    otaTestRunner = OtaTestRunner(boardConfig, stageParams)
    otaTestResults += otaTestRunner.runTests()

def createJunitTestResults(boardToResults, fileName):
    """Create junit xml test result.
    Args:
        boardToResults(dict[str:obj(OtaTestResult)]): Dictionary of the board name to it's OtaTestResult.
        fileName: The name of the junit test file to create.
    """
    testSuites = []
    for board in boardToResults.keys():
        testCases = []
        for otaTestResult in boardToResults[board]:
            testCase = TestCase(otaTestResult.testName, classname=board + '.OTAEndToEndTests')
            testCases.append(testCase)
            if otaTestResult.result != OtaTestResult.PASS:
                testCases[-1].add_failure_info(message=otaTestResult.reason)
        testSuites.append(TestSuite(board, test_cases=testCases, package=board))
        
    with open(fileName, 'w') as f:
        TestSuite.to_file(f, testSuites)

def getStageParameters(args):
    stageParams = {}
    if args.stage != 'prod':
        if args.endpointUrl == None:
            raise Exception('ERROR: --endpoint-url must be specified for stages other that Production.')
        if args.region == None:
            raise Exception('ERROR: --region must be specified for stages other than Production.')
        stageParams['stage'] = args.stage
        stageParams['endpoint-url'] = args.endpointUrl
        stageParams['region'] = args.region
        if args.certificatePath:
            with open(args.certificatePath, 'r') as certFile:
                lines = certFile.readlines()
                lines = list(map(lambda line: line.replace('\n', '\\n'), lines))
                certificate = reduce(add, lines)
                stageParams['certificate'] = certificate
        stageParams['signer-endpoint-url'] = args.signerEndpointUrl
    return stageParams

def otaTestMain():
    """Get all of the OTA results from the tests defined in board.json.
    """
    args = parseArgs()
    boardConfigs = getBoardConfigsFromInputArgs(args)
    boardConfigs = cleanBoardConfigsForInputArgs(args, boardConfigs)
    stageParams = getStageParameters(args)
    boardToResults = {}
    threads = []
    for boardConfig in boardConfigs:
        if boardConfig['exclude']:
            continue
        boardToResults[boardConfig['name']] = []
        if args.separateThreads == True:
            threads.append(Thread(
                target=getBoardOtaTestResult, \
                args=(boardConfig, stageParams, boardToResults[boardConfig['name']])
            ))
        else:
            getBoardOtaTestResult(boardConfig, stageParams, boardToResults[boardConfig['name']])
        
    for i in range(len(threads)):
        threads[i].start()

    for i in range(len(threads)):
        threads[i].join()

    # Compile results into a final junit file
    createJunitTestResults(boardToResults, 'ota_test_results.xml')
