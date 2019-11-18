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


def parse_port_config(contents, chip_keyword=None):
    """ Compile a dictionary of port-wide module configs, which may
        be categorized by chipset.
    """
    chip_fam = "all"
    ifeq_found = False
    port_config_results = {"all": []}

    chip_pattern = ""
    if chip_keyword:
        chip_pattern = (
            re.compile("(?<=ifeq\s\(\$\({}\)\,)(\w+)".format(chip_keyword))
        )

    for line in contents:
        if chip_keyword:
            if not ifeq_found:
                check_ifeq = chip_pattern.search(line)
                if check_ifeq:
                    ifeq_found = True
                    chip_fam = check_ifeq.group(1)
                    #print("found chip:", chip_fam)
                else:
                    ifeq_found = False
                    chip_fam = "all"
            else:
                if "endif" in line:
                    ifeq_found = False
                    chip_fam = "all"

        if "CIRCUITPY_" in line:
            if chip_fam in port_config_results:
                port_config_results[chip_fam].append(line.rstrip("\n"))
            else:
                port_config_results[chip_fam] = [line.rstrip("\n")]

    #print(port_config_results)
    return port_config_results

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
        re_pattern = "CIRCUITPY_{}\s=\s(.+)".format(search_name.upper())
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

    #print(base)
    return base


def get_excluded_boards(base):
    """ Cycles through each board's `mpconfigboard.mk` file to determine
        if each module is included or not. Boards are selected by existence
        in a port listed in `SUPPORTED_PORTS` (e.g. `/port/nrf/feather_52840`)

        Boards are further categorized by their respective chipset (SAMD21,
        SAMD51, nRF52840, etc.)
    """
    modules = list(base.keys())

    re_board_chip = None
    chip_keyword = None
    for port in SUPPORTED_PORTS:
        # each port appears to use its own define for the chipset
        if port in ["atmel-samd"]:
            re_board_chip = re.compile("CHIP_FAMILY\s=\s(\w+)")
            chip_keyword = "CHIP_FAMILY"
        elif port in ["nrf"]:
            re_board_chip = re.compile("MCU_VARIANT\s=\s(\w+)")

        port_dir = "ports/{}".format(port)

        port_config_contents = ""
        with open(os.path.join(port_dir, "mpconfigport.mk")) as port_config:
            port_config_contents = port_config.readlines()
        port_config = parse_port_config(port_config_contents, chip_keyword)

        for entry in os.scandir(os.path.join(port_dir, "boards")):
            if not entry.is_dir():
                continue

            contents = ""
            board_dir = os.path.join(entry.path, "mpconfigboard.mk")
            with open(board_dir) as board:
                contents = board.read()

            board_chip = re_board_chip.search(contents)
            #print(entry.name, board_chip.group(1))
            if not board_chip:
                board_chip = "Unknown Chip"
            else:
                board_chip = board_chip.group(1)

            # add port_config results to contents
            contents += "\n" + "\n".join(port_config["all"])
            if board_chip in port_config:
                contents += "\n" + "\n".join(port_config[board_chip])

            check_dependent_modules = dict()
            for module in modules:
                board_is_excluded = False
                # check if board uses `SMALL_BUILD`. if yes, and current
                # module is marked as `FULL_BUILD`, board is excluded
                small_build = re.search("CIRCUITPY_SMALL_BUILD = 1", contents)
                if small_build and base[module]["full_build"] == "1":
                    board_is_excluded = True

                # check if board uses `MINIMAL_BUILD`. if yes, and current
                # module is marked as `DEFAULT_BUILD`, board is excluded
                min_build = re.search("CIRCUITPY_MINIMAL_BUILD = 1", contents)
                if min_build and base[module]["default_value"] == "CIRCUITPY_DEFAULT_BUILD":
                    board_is_excluded = True

                # check if module is specifically disabled for this board
                re_pattern = "CIRCUITPY_{}\s=\s(\w)".format(module.upper())
                find_module = re.search(re_pattern, contents)
                if not find_module:
                    if base[module]["default_value"].isdigit():
                        # check if default inclusion is off ('0'). if the board doesn't
                        # have it explicitly enabled, its excluded.
                        if base[module]["default_value"] == "0":
                            board_is_excluded = True
                    else:
                        # this module is dependent on another module. add it
                        # to the list to check after processing all other modules.
                        # only need to check exclusion if it isn't already excluded.
                        if (not board_is_excluded and
                            base[module]["default_value"] not in [
                                "None",
                                "CIRCUITPY_DEFAULT_BUILD"
                            ]):
                                check_dependent_modules[module] = base[module]["default_value"]
                else:
                    if (find_module.group(1) == "0" and
                        find_module.group(1) != base[module]["default_value"]):
                            board_is_excluded = True

                if board_is_excluded:
                    if board_chip in base[module]["excluded"]:
                        base[module]["excluded"][board_chip].append(entry.name)
                    else:
                        base[module]["excluded"][board_chip] = [entry.name]

            for module in check_dependent_modules:
                depend_results = set()

                parents = check_dependent_modules[module].split("CIRCUITPY_")
                parents = [item.strip(", ").lower() for item in parents if item]

                for parent in parents:
                    if parent in base:
                        if (board_chip in base[parent]["excluded"] and
                            entry.name in base[parent]["excluded"][board_chip]):
                                depend_results.add(False)
                        else:
                            depend_results.add(True)

                # only exclude the module if there were zero parents enabled
                # as determined by the 'depend_results' set.
                if not any(depend_results):
                    if board_chip in base[module]["excluded"]:
                        base[module]["excluded"][board_chip].append(entry.name)
                    else:
                        base[module]["excluded"][board_chip] = [entry.name]

    #print(json.dumps(base, indent=2))
    return base


def support_matrix_excluded_boards():
    """ Compiles a list of available modules, and which board definitions
        do not include them.
    """
    base = build_module_map()

    return get_excluded_boards(base)

def support_matrix_by_board():
    """ Compiles a list of the available core modules available for each
        board.
    """
    base = build_module_map()
    base_with_exclusions = get_excluded_boards(base)

    boards = dict()
    for port in SUPPORTED_PORTS:
        port_dir = "ports/{}/boards".format(port)
        for entry in os.scandir(port_dir):
            if not entry.is_dir():
                continue
            board_modules = []

            board_name = entry.name
            board_contents = ""
            with open(os.path.join(entry.path, "mpconfigboard.h")) as get_name:
                board_contents = get_name.read()
            board_name_re = re.search("(?<=MICROPY_HW_BOARD_NAME)\s+(.+)",
                                      board_contents)
            if board_name_re:
                board_name = board_name_re.group(1).strip('"')

            for module in base_with_exclusions.keys():
                #print(module)
                board_has_module = True
                if base_with_exclusions[module]["excluded"]:
                    for port in base_with_exclusions[module]["excluded"].values():
                        #print(port)
                        if entry.name in port:
                            board_has_module = False

                if board_has_module:
                    board_modules.append(base_with_exclusions[module]["name"])
            boards[board_name] = sorted(board_modules)

    #print(json.dumps(boards, indent=2))
    return boards
