# The MIT License (MIT)
#
# Copyright (c) 2019 Michael Schroeder
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#

import json
import os
import re
import subprocess
import sys


SUPPORTED_PORTS = ['atmel-samd', 'esp32s2', 'litex', 'mimxrt10xx', 'nrf', 'stm']

def get_shared_bindings():
    """ Get a list of modules in shared-bindings based on folder names
    """
    return [item for item in os.listdir("./shared-bindings")]


def read_mpconfig():
    """ Open 'circuitpy_mpconfig.mk' and return the contents.
    """
    configs = []
    with open("py/circuitpy_mpconfig.mk") as mpconfig:
        configs = mpconfig.read()

    return configs


def build_module_map():
    """ Establish the base of the JSON file, based on the contents from
        `configs`. Base will contain module names, if they're part of
        the `FULL_BUILD`, or their default value (0, 1, or a list of
        modules that determine default [see audiocore, audiomixer, etc.]).

    """
    base = dict()
    modules = get_shared_bindings()
    configs = read_mpconfig()
    full_build = False
    for module in modules:
        full_name = module
        search_name = module.lstrip("_")
        re_pattern = "CIRCUITPY_{}\s*\??=\s*(.+)".format(search_name.upper())
        find_config = re.findall(re_pattern, configs)
        if not find_config:
            continue
        find_config = ", ".join([x.strip("$()") for x in find_config])

        full_build = int("CIRCUITPY_FULL_BUILD" in find_config)
        if not full_build:
            default_val = find_config
        else:
            default_val = "None"

        base[search_name] = {
            "name": full_name,
            "full_build": str(full_build),
            "default_value": default_val,
            "excluded": {}
        }

    return base

def get_settings_from_makefile(port_dir, board_name):
    """ Invoke make in a mode which prints the database, then parse it for
        settings.

        This means that the effect of all Makefile directives is taken
        into account, without having to re-encode the logic that sets them
        in this script, something that has proved error-prone
    """
    contents = subprocess.run(
            ["make", "-C", port_dir, f"BOARD={board_name}", "-qp", "print-CC"],
            encoding="utf-8",
            errors="replace",
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE
        )
    # Make signals errors with exit status 2; 0 and 1 are "non-error" statuses
    if contents.returncode not in (0, 1):
        error_msg = (
            f"Invoking '{' '.join(contents.args)}' exited with "
            f"{contents.returncode}: {contents.stderr}"
        )
        raise RuntimeError(error_msg)

    settings = {}
    for line in contents.stdout.split('\n'):
        m = re.match(r'^([A-Z][A-Z0-9_]*) = (.*)$', line)
        if m:
            settings[m.group(1)] = m.group(2)

    return settings

def lookup_setting(settings, key, default=''):
    while True:
        value = settings.get(key, default)
        if not value.startswith('$'):
            break
        key = value[2:-1]
    return value

def support_matrix_by_board():
    """ Compiles a list of the available core modules available for each
        board.
    """
    base = build_module_map()

    boards = dict()
    for port in SUPPORTED_PORTS:

        port_dir = "ports/{}/boards".format(port)
        for entry in os.scandir(port_dir):
            if not entry.is_dir():
                continue
            board_modules = []

            settings = get_settings_from_makefile(f'ports/{port}', entry.name)

            with open(os.path.join(entry.path, "mpconfigboard.h")) as get_name:
                board_contents = get_name.read()
            board_name_re = re.search("(?<=MICROPY_HW_BOARD_NAME)\s+(.+)",
                                      board_contents)
            if board_name_re:
                board_name = board_name_re.group(1).strip('"')

            board_modules = []
            for module in base:
                key = f'CIRCUITPY_{module.upper()}'
                if int(lookup_setting(settings, key, '0')):
                    board_modules.append(base[module]['name'])
            boards[board_name] = sorted(board_modules)

    #print(json.dumps(boards, indent=2))
    return boards

if __name__ == '__main__':
    print(json.dumps(support_matrix_by_board(), indent=2))
