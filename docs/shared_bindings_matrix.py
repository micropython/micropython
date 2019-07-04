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


SUPPORTED_PORTS = ["atmel-samd", "nrf"]


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


def build_json(modules, configs):
    """ Establish the base of the JSON file, based on the contents from
        `configs`. Base will contain module names, if they're part of
        the `FULL_BUILD`, or their default value (0 | 1).

    """
    base_json = dict()
    full_build = False
    for module in modules:
        full_name = module
        search_name = module.lstrip("_")
        re_pattern = "CIRCUITPY_{}\s=\s(.+)".format(search_name.upper())
        find_config = re.search(re_pattern, configs)
        #print(module, "|", find_config)
        if not find_config:
            continue
        full_build = int("FULL_BUILD" in find_config[0])
        #print(find_config[1])
        if not full_build:
            default_val = find_config[1]
        else:
            default_val = "None"
        base_json[search_name] = {
            "name": full_name,
            "full_build": str(full_build),
            "default_value": default_val,
            "excluded": []
        }

    return get_excluded_boards(base_json)


def get_excluded_boards(base_json):
    """ Cycles through each board's `mpconfigboard.mk` file to determine
        if each module is included or not. Boards are selected by existence
        in a port listed in `SUPPORTED_PORTS` (e.g. `/port/nrf/feather_52840`)
    """
    modules = list(base_json.keys())
    for port in SUPPORTED_PORTS:
        port_dir = "ports/{}/boards".format(port)
        with os.scandir(port_dir) as boards:
            for entry in boards:
                if not entry.is_dir():
                    continue
                contents = ""
                board_dir = os.path.join(entry.path, "mpconfigboard.mk")
                #print(board_dir)
                with open(board_dir) as board:
                    contents = board.read()
                for module in modules:
                    # check if board uses `SMALL_BUILD`. if yes, and current
                    # module is marked as `FULL_BUILD`, board is excluded
                    small_build = re.search("CIRCUITPY_SMALL_BUILD = 1", contents)
                    if small_build and base_json[module]["full_build"] == "1":
                        base_json[module]["excluded"].append(entry.name)
                        continue

                    # check if module is specifically disabled for this board
                    re_pattern = "CIRCUITPY_{}\s=\s(\w)".format(module.upper())
                    find_module = re.search(re_pattern, contents)
                    if not find_module:
                        continue
                    if (find_module[1] == "0" and
                       find_module[1] != base_json[module]["default_value"]):
                            base_json[module]["excluded"].append(entry.name)

    return base_json

if __name__ == "__main__":
    modules = get_shared_bindings()
    configs = read_mpconfig()
    base = build_json(sorted(modules), configs)

    final_json = json.dumps(base, indent=2)

    shared_bindings_json = 'support_matrix.json'
    if 'TRAVIS' in os.environ:
        shared_bindings_json = os.path.join('$HOME', shared_bindings_json)
    else:
        print(final_json)
        shared_bindings_json = os.path.join('shared-bindings', shared_bindings_json)
    with open(shared_bindings_json, "w") as matrix:
        json.dump(base, matrix, indent=2)
