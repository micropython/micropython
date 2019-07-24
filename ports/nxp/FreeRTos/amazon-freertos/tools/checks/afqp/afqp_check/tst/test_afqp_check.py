#!/usr/bin/python

import os
import sys
my_path = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.dirname(my_path))

import pytest
import json
from datetime import datetime
import src.afqp_check as afqp_check


check_fs_params_existing = [
    (afqp_check.AFR_ROOT, 'rules.json', 'ti', 'cc3220_launchpad', 'ccs'),
    (afqp_check.AFR_ROOT, 'rules.json', 'microchip', 'curiosity_pic32mzef', 'mplab'),
    (afqp_check.AFR_ROOT, 'rules.json', 'espressif', 'esp32_devkitc_esp_wrover_kit', 'make'),
    (afqp_check.AFR_ROOT, 'rules.json', 'infineon', 'xmc4800_iotkit', 'dave4'),
    (afqp_check.AFR_ROOT, 'rules.json', 'nxp', 'lpc54018iotmodule', 'iar'),
    (afqp_check.AFR_ROOT, 'rules.json', 'nxp', 'lpc54018iotmodule', 'mcuxpresso'),
    (afqp_check.AFR_ROOT, 'rules.json', 'st', 'stm32l475_discovery', 'ac6'),
    (afqp_check.AFR_ROOT, 'rules.json', 'xilinx', 'microzed', 'xsdk'),
    (afqp_check.AFR_ROOT, 'rules.json', 'vendor', 'board', 'ide')
]
@pytest.mark.parametrize("root, rules_path, vendor, board, ide", check_fs_params_existing)
def test_fs_check_existing(root, rules_path, vendor, board, ide):
    results = afqp_check.check_fs(root, rules_path, vendor, board, ide)
    for result in results:
        assert result.type == 'warning', '({}, {})'.format(result.type, result.info)
        assert result.type != 'error', '({}, {})'.format(result.type, result.info)


check_fs_params_nonexisting = [
    (afqp_check.AFR_ROOT, 'rules.json', 'dummy_vendor', 'dummy_board', 'dummy_ide')
]
@pytest.mark.parametrize('root, rules_path, vendor, board, ide', check_fs_params_nonexisting)
def test_fs_check_nonexisting(root, rules_path, vendor, board, ide):
    results = afqp_check.check_fs(root, rules_path, vendor, board, ide)
    with open(rules_path, 'r') as rules_file:
        rules = json.load(rules_file)
    sum_errors = 0
    sum_warnings = 0
    for result in results:
        if result.type == 'error':
            sum_errors += 1
        if result.type == 'warning':
            sum_warnings += 1
    assert sum_errors == len(rules['error'])
    assert sum_warnings == len(rules['warning'])


check_platform_name_params = [
    (afqp_check.AFR_ROOT, 'ti', 'cc3220_launchpad'),
    (afqp_check.AFR_ROOT, 'microchip', 'curiosity_pic32mzef'),
    (afqp_check.AFR_ROOT, 'espressif', 'esp32_devkitc_esp_wrover_kit'),
    (afqp_check.AFR_ROOT, 'infineon', 'xmc4800_iotkit'),
    (afqp_check.AFR_ROOT, 'nxp', 'lpc54018iotmodule'),
    (afqp_check.AFR_ROOT, 'st', 'stm32l475_discovery'),
    (afqp_check.AFR_ROOT, 'xilinx', 'microzed'),
    (afqp_check.AFR_ROOT, 'vendor', 'board')
]
@pytest.mark.parametrize('root, vendor, board', check_platform_name_params)
def test_check_platform_name(root, vendor, board):
    results = afqp_check.check_platform_name(root, vendor, board)
    assert len(list(filter(lambda r : r.type == 'error', results))) == 0


def test_get_platform_name_error():
    result = afqp_check.get_platform_name_error('tst/missing_platform_name.h')
    assert result != []
    assert result[0].type == 'error'


check_build_artifacts_params = [
    (   
        [
            'hello.c',
            '.gitginore',
            "world.h"
        ],
        0
    ),
    (
        [
            'hello.o',
            'world.o',
            'hello.map',
            'world.map',
            'hello.elf',
            'world.elf',
            'hello.d',
            'world.d',
            'hello.c',
            'world.c',
            'hello.bin',
            'world.bin',
            'hello.hex',
            'world.hex'
        ],
        6
    )
]
@pytest.mark.parametrize('input_files, expected_errors', check_build_artifacts_params)
def test_check_build_artifacts(input_files, expected_errors):
    results = afqp_check.check_build_artifacts(input_files)
    assert len(list(filter(lambda r : r.type == 'error', results))) == expected_errors


def test_get_license():
    with open(os.path.join('tst','license_test_0.h'), 'r') as f:
        license = afqp_check.get_license(f)
        assert 'Amazon FreeRTOS' in license
        assert 'http://www.FreeRTOS.org' in license
        assert '#include \"dummy.h\"' not in license
    
    with open('tst/license_test_1.h', 'r') as f:
        license = afqp_check.get_license(f)
        assert 'Amazon FreeRTOS' in license
        assert 'http://www.FreeRTOS.org' in license
        assert '#ifndef DUMMY' not in license


def test_get_license_match_error():
    with open(os.path.join('tst','license_test_unmatching.h'), 'r') as f:
        license = afqp_check.get_license(f)
        result = afqp_check.get_license_match_error(license, f.name)
        assert result == []

    with open(os.path.join('tst', 'license_test_0.h'), 'r') as f:
        license = afqp_check.get_license(f)
        result = afqp_check.get_license_match_error(license, f.name)
        assert result != []
        assert result[0].type == 'warning', '({}, {})'.format(result.type, result.info)


license_path = 'a/b/c/aws_ota_pal.c'

license_path_config = 'a/b/c/aws_ota_agent_config.h'

license_no_errors = \
    """/*\nAmazon FreeRTOS OTA PAL for CC3220SF-LAUNCHXL V1.0.0
    Copyright (C) <year> Amazon.com, Inc. or its affiliates.  All Rights Reserved.\n
    Permission is hereby granted, free of charge, to any person obtaining a copy 
    of\n""".replace('<year>', str(datetime.now().year))
 
license_no_errors_config = \
    """/*\nAmazon FreeRTOS V1.0.0
    Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.\n
    Permission is hereby granted, free of charge, to any person obtaining a copy 
    of\n""".replace('<year>', str(datetime.now().year))

get_copyright_errors_params = [
    (license_no_errors, license_path, False, 0),
    (license_no_errors_config, license_path_config, True, 0),
    ('No copyright here.\nNo copyright here.\nNo copyright here.\n', license_path, False, 2),
    (license_no_errors.replace('2018', '1999'), license_path, False, 1),
    (license_no_errors_config.replace('2018', '1999'), license_path_config, True, 1),
    (license_no_errors.replace('/*\nAmazon FreeRTOS OTA PAL for CC3220SF-LAUNCHXL V1.0.0\n', ''), license_path, False, 1),
    (license_no_errors_config.replace('/*\nAmazon FreeRTOS V1.0.0\n', ''), license_path_config, True, 1),
    (license_no_errors.replace(' V1.0.0', ''), license_path, False, 1),
    (license_no_errors_config.replace(' V1.0.0', ''), license_path_config, True, 0),
    (license_no_errors.replace('Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.\n\n', ''), license_path, False, 1),
    (license_no_errors_config.replace('Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.\n\n', ''), license_path_config, True, 1),
    (license_no_errors.replace('  All Rights Reserved.', ''), license_path, False, 1),
    (license_no_errors_config.replace('  All Rights Reserved.', ''), license_path_config, True, 1),
    (license_no_errors.replace('V1.0.0', 'V2.0.0'), license_path, False, 1),
    (license_no_errors.replace('OTA PAL', 'WRONG'), license_path, False, 1)
]
@pytest.mark.parametrize('input_license, license_path, is_config, expected_num_errors', get_copyright_errors_params)
def test_get_copyright_errors(input_license, license_path, is_config, expected_num_errors):
    results = afqp_check.get_copyright_errors(input_license, license_path, is_config)
    assert len(results) == expected_num_errors
    for result in results:
        assert result.type == 'error', '({}, {})'.format(result.type, result.info)


def test_get_line_number():
    with open('tst/.project_compliant', 'r') as f:
        lines = f.readlines()
    assert afqp_check._get_line_number(lines, 'aws_demos') == 3
    assert afqp_check._get_line_number(lines, '<name>lib/aws</name>') == 50
    assert afqp_check._get_line_number(lines, '<name>lib/aws/tls/aws_tls.c</name>') == 175


get_eclipse_project_errors_params = [
    (os.path.join(afqp_check.AFR_ROOT, 'demos/ti/cc3220_launchpad/ccs/.project'), 0),
    (os.path.join(afqp_check.AFR_ROOT, 'demos/st/stm32l475_discovery/ac6/.project'), 0),
    (os.path.join(afqp_check.AFR_ROOT, 'demos/infineon/xmc4800_iotkit/dave4/.project'), 0),
    (os.path.join(afqp_check.AFR_ROOT, 'demos/nxp/lpc54018iotmodule/mcuxpresso/.project'), 0),
    (os.path.join(afqp_check.AFR_ROOT, 'demos/xilinx/microzed/xsdk/aws_demos/.project'), 0),
    ('tst/.project_compliant', 0),
    ('tst/.project_invalid_project_name', 1),
    ('tst/.project_multiple_roots', 1),
    ('tst/.project_seven_missing_locationURI', 7)
]
@pytest.mark.parametrize('test_file_path, expected_num_errors', get_eclipse_project_errors_params)
def test_get_eclipse_project_errors(test_file_path, expected_num_errors):
    results, dummy_set = afqp_check.get_eclipse_project_errors(test_file_path)
    assert len(results) == expected_num_errors
    for result in results:
        assert result.type == 'error', '({}, {})'.format(result.type, result.info)


eclipse_check_params = [
    (afqp_check.AFR_ROOT, '', 'ti', 'cc3220_launchpad', 'ccs', 2),
    (afqp_check.AFR_ROOT, '', 'st', 'stm32l475_discovery', 'ac6', 0),
    (afqp_check.AFR_ROOT, '', 'infineon', 'xmc4800_iotkit', 'dave4', 0),
    (afqp_check.AFR_ROOT, '', 'nxp', 'lpc54018iotmodule', 'mcuxpresso', 0),
    (afqp_check.AFR_ROOT, 'aws_demos', 'xilinx', 'microzed', 'xsdk', 0)
]
@pytest.mark.parametrize('root, project_path, vendor, board, ide, expected_num_errors', eclipse_check_params)
def test_eclipse_check(root, project_path, vendor, board, ide, expected_num_errors):
    results = afqp_check.check_eclipse(root, project_path, vendor, board, ide)
    assert len(results) == expected_num_errors


get_eclipse_cproject_errors_params = [
    ('tst/.cproject_compliant', 0),
    ('tst/.cproject_four_different_roots', 4)
]
@pytest.mark.parametrize('test_file_path, expected_num_warnings', get_eclipse_cproject_errors_params)
def test_get_eclipse_cproject_errors(test_file_path, expected_num_warnings):
    results = afqp_check.get_eclipse_cproject_errors(test_file_path, 'AFR_ROOT')
    assert len(results) == expected_num_warnings
