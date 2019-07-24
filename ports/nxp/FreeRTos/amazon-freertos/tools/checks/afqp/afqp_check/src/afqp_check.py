#!/usr/bin/python

import xml.etree.ElementTree as ElementTree
import sys
import argparse
import json
import os
import re
from datetime import datetime
from collections import namedtuple
from ntpath import basename


# Define error/warning tuple
# error is ( str:['error' or 'warning'] , str:Information about error or warning )
Error = namedtuple('Error', 'type, info')


# Define Amazon FreeRTOS relative path
AFR_ROOT = '../../../..'


def check_fs(root, rules_path, vendor, board, ide):
    """Returns errors and warnings according to the file system rules in rules.json.
    """
    errors = []
    with open(rules_path, 'r') as rules_file:
        rules = json.load(rules_file)

    for path in rules['error']:
        path = os.path.join(root, path.format(vendor=vendor, board=board, ide=ide))
        if not os.path.exists(path):
            errors.append(Error(type='error', info='Required path ' + path + ' is not found.'))

    for path in rules['warning']:
        path = os.path.join(root, path.format(vendor=vendor, board=board, ide=ide))
        if not os.path.exists(path):
            errors.append(Error(type='warning', info='Optional path ' + path + ' is not found.'))

    return errors

def get_platform_name_error(file_path):
    """Return an Error if configPLATFORM_NAME is not found in the input file. Otherwise
    return an empty list.
    """
    with open(file_path, 'r') as file:
        if not list(filter(lambda line: 'configPLATFORM_NAME' in line, file.readlines())):
            return [Error(type='error', info='configPLATFORM_NAME not found in ' + file.name)]
    return []


def check_platform_name(root, vendor, board):
    """Return errors if configPLATFORM_NAME doesn't exist in the tests and demos
    FreeRTOSConfig.h files.
    """
    errors = []
    freertos_config_tests_path = \
        os.path.join(root, 'tests', vendor, board, 'common/config_files/FreeRTOSConfig.h')
    freertos_config_demos_path = \
        os.path.join(root, 'demos', vendor, board, 'common/config_files/FreeRTOSConfig.h')
    errors.extend(get_platform_name_error(freertos_config_tests_path))
    errors.extend(get_platform_name_error(freertos_config_demos_path))
    return errors


def check_build_artifacts(files):
    """Return errors for all .o, .d, .map, and .elf files in the list of files.
    """
    errors = []
    o_files = list(filter(lambda f: f[-2:] == '.o', files))
    d_files = list(filter(lambda f: f[-2:] == '.d', files))
    elf_files = list(filter(lambda f: f[-4:] == '.elf', files))
    map_files = list(filter(lambda f: f[-4:] == '.map', files))
    bin_files = list(filter(lambda f: f[-4:] == '.bin', files))
    hex_files = list(filter(lambda f: f[-4:] == '.hex', files))
    if o_files:
        errors.append(Error(
            type='error',
            info='Build artifact found: ' + o_files[0] + ', please delete all build artifacts or create .gitignore before checking in.'
        ))
    if d_files:
        errors.append(Error(
            type='error',
            info='Build artifact found: ' + d_files[0] + ', please delete all build artifacts or create .gitignore before checking in.'
        ))
    if map_files:
        errors.append(Error(
            type='error',
            info='Build artifact found: ' + elf_files[0] + ', please delete all build artifacts or create .gitignore before checking in.'
        ))
    if elf_files:
        errors.append(Error(
            type='error',
            info='Build artifact found: ' + map_files[0] + ', please delete all build artifacts or create .gitignore before checking in.'
        ))
    if bin_files:
        errors.append(Error(
            type='error',
            info='Build artifact found: ' + bin_files[0] + ', please delete all build artifacts or create .gitignore before checking in.'
        ))
    if hex_files:
        errors.append(Error(
            type='error',
            info='Build artifact found: ' + hex_files[0] + ', please delete all build artifacts or create .gitignore before checking in.'
        ))
    return errors


def get_license(file):
    """Returns the license from the input file.
    """
    # Collect the license
    lic = ''
    for line in file:
        if line.startswith('#include') or line.startswith('#ifndef'):
            break
        else:
            lic += line
    return lic


def get_license_match_error(lic, lic_file_path):
    """Returns an Error of the type 'warning' if the Amazon FreeRTOS license is present in the
    input file. Otherwise an empty list is returned.
    """
    # Get the words in the license template
    with open('license.templ', 'r') as file:
        template_lic = file.read()
        template_lic_words = list(filter(None, re.split('[^0-9a-zA-Z]+', template_lic)))

    # Split on non-alphanumeric characters
    # re.split() will match the empty string.
    lic_words = list(filter(None, re.split('[^0-9a-zA-Z]+', lic)))
    i = 0
    same = False
    for i, word in enumerate(lic_words):
        if word == template_lic_words[0]:
            # Element wise comparison of the two arrays.
            if lic_words[i:i+len(template_lic_words)] == template_lic_words:
                same = True
                break
    if same:
        return [Error(type='warning', info='Amazon FreeRTOS license is in file: ' + lic_file_path)]
    return []


PORTABLE_LAYER_NAMES = {
    'aws_ota_pal.c': 'OTA PAL',
    'aws_pkcs11_pal.c': 'PKCS #11 PAL',
    'aws_secure_sockets.c':'Secure Sockets',
    'aws_wifi.c': 'Wi-Fi'
}

COPYRIGHT_NAME = 'Amazon FreeRTOS'
FIRST_VERSION = 'V1.0.0'

def get_copyright_errors(lic, file_path, is_config_file):
    """Returns Errors for errors in the Amazon FreeRTOS copyright that is possibly inside of the
    input license, lic. Otherwise an empty list is returned.
    """
    errors = []
    # Flag if the version and copyright are found in the license.
    version_found = False
    copyright_found = False
    for line in lic.split('\n'):

        # Config files do not need a version.
        if COPYRIGHT_NAME in line and is_config_file:
            version_found = True
            continue

        # Portable layer files need a version.
        if COPYRIGHT_NAME in line and 'V' in line:
            version_found = True

            # Get the version error for the portable layer code
            if FIRST_VERSION not in line:
                errors.append(Error(
                    type='error',
                    info=file_path + ' should be the first version of the file (V1.0.0).'
                ))

            # Get the portable layer name error
            start = line.find(COPYRIGHT_NAME) + len(COPYRIGHT_NAME) + 1 # +1 for space
            end = line.find('for') - 1 # -1 for space
            name = line[start:end]
            file_name = basename(file_path)
            if name != PORTABLE_LAYER_NAMES[file_name]:
                errors.append(Error(
                    type='error',
                    info='\"' + name + '\" is not the valid portable layer name. It should be changed to: ' + PORTABLE_LAYER_NAMES[file_name]
                ))
            continue

        # Get an error if the copyright line is not found for all types of files.
        if 'Copyright (C)' in line and 'Amazon.com, Inc. or its affiliates.  All Rights Reserved.' in line:
            copyright_found = True
            year = str(datetime.now().year)
            if year not in line:
                errors.append(Error(type='error', info=file_path + ' is a new file and it\'s year should be ' + year))

    # If the version line was not found flag errors.
    if not version_found:
        if basename(file_path) == 'FreeRTOSIPConfig.h' or basename(file_path) == 'FreeRTOSConfig.h':
            pass # Skip version warnings for FreeRTOS config files.
        elif is_config_file:
            errors.append(Error(
                type='error',
                info=file_path + ' is missing required version line: \"Amazon FreeRTOS VX.Y.Z\"'
            ))
        else:
            errors.append(Error(
                type='error',
                info=file_path + 'is missing: \"Amazon FreeRTOS ... for ... V1.0.0\"'
            ))
    # If the copyright was not found flag errors.
    if not copyright_found:
        errors.append(Error(
            type='error',
            info=file_path + ' is missing: \"Copyright (C)' + str(datetime.now().year) + ' Amazon.com, Inc. or its affiliates.  All Rights Reserved.\"'
        ))
    return errors

# Valid portable layer paths to the all current set of portable code.
PORTABLE_LAYER_PATHS = [
    'lib/pkcs11/portable/{vendor}/{board}/aws_pkcs11_pal.c',
    'lib/wifi/portable/{vendor}/{board}/aws_wifi.c',
    'lib/ota/portable/{vendor}/{board}/aws_ota_pal.c',
    'lib/secure_sockets/portable/{vendor}/{board}/aws_secure_sockets.c'
]

# Files to ignore in the check_license routine.
IGNORED_FILES = {
    "unity_config.h",
    "lwipopts.h",
    "trcConfig.h",
    "trcSnapshotConfig.h"
}


def check_license(root, vendor, board):
    """If the portable layer exists then make sure the license is valid.
    """
    errors = []
    for portable_layer_path in PORTABLE_LAYER_PATHS:
        path = os.path.join(root, portable_layer_path.format(vendor=vendor, board=board))
        if os.path.exists(path):
            with open(path, 'r') as file:
                lic = get_license(file)
                errors.extend(get_license_match_error(lic, file.name))
                errors.extend(get_copyright_errors(lic, file.name, False))

    config_files_tests_path = os.path.join(
        root, 'demos/{vendor}/{board}/common/config_files'.format(vendor=vendor, board=board)
    )
    config_files_demos_path = os.path.join(
        root, 'tests/{vendor}/{board}/common/config_files'.format(vendor=vendor, board=board)
    )
    for file in os.listdir(config_files_tests_path):
        if file in IGNORED_FILES:
            continue
        with open(os.path.join(config_files_tests_path, file), 'r') as file:
            lic = get_license(file)
            errors.extend(get_copyright_errors(lic, file.name, True))
    for file in os.listdir(config_files_demos_path):
        if file in IGNORED_FILES:
            continue
        with open(os.path.join(config_files_demos_path, file), 'r') as file:
            lic = get_license(file)
            errors.extend(get_copyright_errors(lic, file.name, True))
    return errors


def _get_line_number(file_lines, pattern):
    """Get the index of the first element in file_lines with the string in pattern
    """
    return next(i for i, line in enumerate(file_lines) if pattern in line) + 1


def get_eclipse_project_errors(project_path):
    """Get all errors in the .project eclipse file. These include:
        1. The project not being named "aws_demos"
        2. Missing <locationURI> tags in linked resources
        3. Multiple root variables extended from in non-virtual linked resources.
    Returns all the root variables in root_var_set, in addition to the errors.
    """
    errors = []
    root_var_set = set()

    file_lines = []
    with open(project_path, 'r') as project_file:
        file_lines = project_file.readlines()

    # All camel case variables below are elements in the XML tree.

    # Get the project name error, project must be named "aws_demos"
    tree = ElementTree.parse(project_path)
    projectDescription = tree.getroot() # eclipse .project document xml root element is <projectDescription>
    if projectDescription.find('name').text != 'aws_demos':
        errors.append(Error(
            type='error',
            info='In .project Eclipse IDE project name must be \"aws_demos\" at line {}.'.format(_get_line_number(file_lines, '<name>'))
        ))

    linkedResources = projectDescription.find('linkedResources')
    for link in linkedResources.findall('link'):
        locationURI = link.find('locationURI')
        name = link.find('name')
        if locationURI is not None:
            if locationURI.text == 'virtual:/virtual':
                continue
            root_var_set.add(locationURI.text[:locationURI.text.find('/')])
        else:
            error_line = _get_line_number(file_lines, '<name>{}</name>'.format(name.text)) - 1
            errors.append(Error(
                type='error',
                info='Eclipse .project linked resource {} MUST have a <locationURI> tag at line {}.'.format(name.text, error_line)
            ))
    if len(root_var_set) > 1:
        errors.append(Error(
            type='error',
            info='Eclipse .project linked resources must all extend from a single root variable. {} root variables were found:\n' + '\n'.join(root_var_set)
        ))
    return errors, root_var_set


def get_eclipse_cproject_errors(cproject_path, afr_root_var):
    """Get all the errors (type warning) in the eclipse .cproject file. Currently this is multiple root variables
    on the incude paths. Having multiple root variables may cause project warnings after OCW cleans the project file.
    """
    file_lines = []
    with open(cproject_path, 'r', encoding='utf-8') as project_file:
        file_lines = project_file.readlines()

    errors = []
    afr_root_var_ref = '${' + afr_root_var + '}' # ${AFR_ROOT}
    workspace_ref = '${workspace_loc:/' # ${workspace_loc:/
    tree = ElementTree.parse(cproject_path)
    option_include_path_xpath = './/option[@valueType=\"includePath\"]'
    option_include_path_elements = tree.getroot().findall(option_include_path_xpath)
    for option_include_path_element in option_include_path_elements:
        for listOptionValue in option_include_path_element.findall('listOptionValue'):
            value = listOptionValue.get('value')
            if afr_root_var_ref in value:
                continue
            elif workspace_ref in value:
                continue
            else:
                if value.startswith('\"') and value.endswith('\"'):
                    value = value[1:-1]
                error_line = _get_line_number(file_lines, value)
                errors.append(Error(
                    type='warning',
                    info='Eclipse .cproject include path \"{}\" does not extend from {} at line {}'.format(value, afr_root_var_ref, error_line)
                ))
    return errors


def check_eclipse(root, project_path, vendor, board, ide):
    '''Returns errors found in the eclipse .project and .cproject files.
    '''
    errors = []
    root_var_set = set()
    demo_project_path = os.path.join(root, 'demos', vendor, board, ide, project_path, '.project')
    demo_cproject_path = os.path.join(root, 'demos', vendor, board, ide, project_path, '.cproject')
    project_errors, root_var_set = get_eclipse_project_errors(demo_project_path)
    errors.extend(project_errors)
    errors.extend(get_eclipse_cproject_errors(demo_cproject_path, next(iter(root_var_set))))
    return errors


def report_errors(errors):
    """Print errors and warnings to stdout.
    """
    for error in errors:
        print(error.type.upper() + ': ' + error.info)


def main():
    parser = argparse.ArgumentParser(description='AFQP File System Check')
    parser.add_argument('--root', action='store', required=False, dest='root', default=AFR_ROOT, help='Amazon FreeRTOS Root Path. This path is relative to the /afqp_check folder by default ({}).'.format(AFR_ROOT))
    parser.add_argument('--rules', action='store', required=False, dest='rules_path', default='rules.json', help='The path to the rules json file. This path is relative to the /afqp_check folder by default.')
    parser.add_argument('--files', nargs='+', required=False, dest='files', help='List of files changed.')
    parser.add_argument('--vendor', action='store', required=True, dest='vendor', help='Expected vendor directory name.')
    parser.add_argument('--board', action='store', required=True, dest='board', help='Expected board directory name.')
    parser.add_argument('--ide', action='store', required=True, dest='ide', help='Expected IDE directory name.')
    parser.add_argument('--eclipse', '--eclipse', action='store_true', required=False, help='Set this option if an eclipse IDE project is being used.')
    parser.add_argument('--project', action='store', required=False, dest='project_path', default='', help='The path to the eclipse project relative to demos(tests)/{vendor}/{board}/{ide}. This is set to an empty string by default.')
    args = parser.parse_args()

    errors = []
    errors.extend(check_fs(args.root, args.rules_path, args.vendor, args.board, args.ide))
    errors.extend(check_platform_name(args.root, args.vendor, args.board))
    if args.files:
        errors.extend(check_build_artifacts(args.files))
    errors.extend(check_license(args.root, args.vendor, args.board))
    if args.eclipse:
        errors.extend(check_eclipse(args.root, args.project_path, args.vendor, args.board, args.ide))

    report_errors(errors)
    sys.exit(len(list(filter(lambda e: e.type == 'error', errors))))


if __name__ == "__main__":  # pragma: no cover
    main()
