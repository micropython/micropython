# The MIT License (MIT)
#
# SPDX-FileCopyrightText: Copyright (c) 2019 Michael Schroeder
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
import pathlib
import re
import subprocess
import sys

from concurrent.futures import ThreadPoolExecutor

SUPPORTED_PORTS = ['atmel-samd', 'broadcom', 'cxd56', 'espressif', 'litex', 'mimxrt10xx', 'nrf', 'raspberrypi', 'stm']

ALIASES_BY_BOARD = {
    "circuitplayground_express": [
        "circuitplayground_express_4h",
        "circuitplayground_express_digikey_pycon2019",
    ],
    "pybadge": ["edgebadge"],
    "pyportal": ["pyportal_pynt"],
    "gemma_m0": ["gemma_m0_pycon2018"],
}

ALIASES_BRAND_NAMES = {
    "circuitplayground_express_4h":
        "Adafruit Circuit Playground Express 4-H",
    "circuitplayground_express_digikey_pycon2019":
        "Circuit Playground Express Digi-Key PyCon 2019",
    "edgebadge":
        "Adafruit EdgeBadge",
    "pyportal_pynt":
        "Adafruit PyPortal Pynt",
    "gemma_m0_pycon2018":
        "Adafruit Gemma M0 PyCon 2018",
}

ADDITIONAL_MODULES = {
    "fontio": "CIRCUITPY_DISPLAYIO",
    "terminalio": "CIRCUITPY_DISPLAYIO",
    "adafruit_bus_device": "CIRCUITPY_BUSDEVICE",
    "adafruit_pixelbuf": "CIRCUITPY_PIXELBUF",
    "usb": "CIRCUITPY_USB_HOST",
}

FROZEN_EXCLUDES = ["examples", "docs", "tests", "utils", "conf.py", "setup.py"]
"""Files and dirs at the root of a frozen directory that should be ignored.
This is the same list as in the preprocess_frozen_modules script."""

repository_urls = {}
"""Cache of repository URLs for frozen modules."""

def get_circuitpython_root_dir():
    """ The path to the root './circuitpython' directory.
    """
    file_path = pathlib.Path(__file__).resolve()
    root_dir = file_path.parent.parent

    return root_dir

def get_shared_bindings():
    """ Get a list of modules in shared-bindings based on folder names.
    """
    shared_bindings_dir = get_circuitpython_root_dir() / "shared-bindings"
    return [item.name for item in shared_bindings_dir.iterdir()] + ["binascii", "errno", "json", "re", "ulab"]


def get_board_mapping():
    """
    Compiles the list of boards from the directories, with aliases and mapping
    to the port.
    """
    boards = {}
    for port in SUPPORTED_PORTS:
        board_path = os.path.join("../ports", port, "boards")
        for board_path in os.scandir(board_path):
            if board_path.is_dir():
                board_files = os.listdir(board_path.path)
                board_id = board_path.name
                aliases = ALIASES_BY_BOARD.get(board_path.name, [])
                boards[board_id] = {
                    "port": port,
                    "download_count": 0,
                    "aliases": aliases,
                }
                for alias in aliases:
                    boards[alias] = {
                        "port": port,
                        "download_count": 0,
                        "alias": True,
                        "aliases": [],
                    }
    return boards


def read_mpconfig():
    """ Open 'circuitpy_mpconfig.mk' and return the contents.
    """
    configs = []
    cpy_mpcfg = get_circuitpython_root_dir() / "py" / "circuitpy_mpconfig.mk"
    with open(cpy_mpcfg) as mpconfig:
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
        if module in ADDITIONAL_MODULES:
            search_identifier = ADDITIONAL_MODULES[module]
        else:
            search_identifier = 'CIRCUITPY_'+module.lstrip("_").upper()
        re_pattern = f"{re.escape(search_identifier)}\s*\??=\s*(.+)"
        find_config = re.findall(re_pattern, configs)
        if not find_config:
            continue
        find_config = ", ".join([x.strip("$()") for x in find_config])

        full_build = int("CIRCUITPY_FULL_BUILD" in find_config)
        if not full_build:
            default_val = find_config
        else:
            default_val = "None"

        base[module] = {
            "name": full_name,
            "full_build": str(full_build),
            "default_value": default_val,
            "excluded": {},
            "key": search_identifier,
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
        # Handle both = and := definitions.
        m = re.match(r'^([A-Z][A-Z0-9_]*) :?= (.*)$', line)
        if m:
            settings[m.group(1)] = m.group(2)

    return settings

def get_repository_url(directory):
    if directory in repository_urls:
        return repository_urls[directory]
    readme = None
    for readme_path in (
            os.path.join(directory, "README.rst"),
            os.path.join(os.path.dirname(directory), "README.rst")
        ):
        if os.path.exists(readme_path):
            readme = readme_path
            break
    path = None
    if readme:
        with open(readme, "r") as fp:
            for line in fp.readlines():
                if m := re.match("\s+:target:\s+(http\S+(docs.circuitpython|readthedocs)\S+)\s*", line):
                    path = m.group(1)
                    break
                if m := re.search("<(http[^>]+)>", line):
                    path = m.group(1)
                    break
    if path is None:
        contents = subprocess.run(
            ["git", "remote", "get-url", "origin"],
            encoding="utf-8",
            errors="replace",
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            cwd=directory
        )
        path = contents.stdout.strip()
    repository_urls[directory] = path
    return path

def frozen_modules_from_dirs(frozen_mpy_dirs, withurl):
    """
    Go through the list of frozen directories and extract the python modules.
    Paths are of the type:
        $(TOP)/frozen/Adafruit_CircuitPython_CircuitPlayground
        $(TOP)/frozen/circuitpython-stage/meowbit
    Python modules are at the root of the path, and are python files or directories
    containing python files. Except the ones in the FROZEN_EXCLUDES list.
    """
    frozen_modules = []
    for frozen_path in filter(lambda x: x, frozen_mpy_dirs.split(" ")):
        source_dir = get_circuitpython_root_dir() / frozen_path[7:]
        url_repository = get_repository_url(source_dir)
        for sub in source_dir.glob("*"):
            if sub.name in FROZEN_EXCLUDES:
                continue
            if sub.name.endswith(".py"):
                if withurl:
                    frozen_modules.append((sub.name[:-3], url_repository))
                else:
                    frozen_modules.append(sub.name[:-3])
                continue
            if next(sub.glob("**/*.py"), None): # tests if not empty
                if withurl:
                    frozen_modules.append((sub.name, url_repository))
                else:
                    frozen_modules.append(sub.name)
    return frozen_modules

def lookup_setting(settings, key, default=''):
    while True:
        value = settings.get(key, default)
        if not value.startswith('$'):
            break
        key = value[2:-1]
    return value

def all_ports_all_boards(ports=SUPPORTED_PORTS):
    for port in ports:

        port_dir = get_circuitpython_root_dir() / "ports" / port
        for entry in (port_dir / "boards").iterdir():
            if not entry.is_dir():
                continue
            yield (port, entry)

def support_matrix_by_board(use_branded_name=True, withurl=True):
    """ Compiles a list of the available core modules available for each
        board.
    """
    base = build_module_map()

    def support_matrix(arg):
        port, entry = arg
        port_dir = get_circuitpython_root_dir() / "ports" / port
        settings = get_settings_from_makefile(str(port_dir), entry.name)

        if use_branded_name:
            with open(entry / "mpconfigboard.h") as get_name:
                board_contents = get_name.read()
            board_name_re = re.search(r"(?<=MICROPY_HW_BOARD_NAME)\s+(.+)",
                                      board_contents)
            if board_name_re:
                board_name = board_name_re.group(1).strip('"')
        else:
            board_name = entry.name

        board_modules = []
        for module in base:
            key = base[module]['key']
            if int(lookup_setting(settings, key, '0')):
                board_modules.append(base[module]['name'])
        board_modules.sort()

        if "CIRCUITPY_BUILD_EXTENSIONS" in settings:
            board_extensions = [
                extension.strip() for extension in
                settings["CIRCUITPY_BUILD_EXTENSIONS"].split(",")
            ]
        else:
            raise OSError(f"Board extensions undefined: {board_name}.")

        frozen_modules = []
        if "FROZEN_MPY_DIRS" in settings:
            frozen_modules = frozen_modules_from_dirs(settings["FROZEN_MPY_DIRS"], withurl)
            if frozen_modules:
                frozen_modules.sort()

        # generate alias boards too
        board_matrix = [(
            board_name, {
                "modules": board_modules,
                "frozen_libraries": frozen_modules,
                "extensions": board_extensions,
            }
        )]
        if entry.name in ALIASES_BY_BOARD:
            for alias in ALIASES_BY_BOARD[entry.name]:
                if use_branded_name:
                    if alias in ALIASES_BRAND_NAMES:
                        alias = ALIASES_BRAND_NAMES[alias]
                    else:
                        alias = alias.replace("_"," ").title()
                board_matrix.append((
                    alias, {
                        "modules": board_modules,
                        "frozen_libraries": frozen_modules,
                        "extensions": board_extensions,
                    },
                ))

        return board_matrix # this is now a list of (board,modules)

    executor = ThreadPoolExecutor(max_workers=os.cpu_count())
    mapped_exec = executor.map(support_matrix, all_ports_all_boards())
    # flatmap with comprehensions
    boards = dict(sorted([board for matrix in mapped_exec for board in matrix], key=lambda x: x[0]))

    return boards

if __name__ == '__main__':
    print(json.dumps(support_matrix_by_board(), indent=2))
