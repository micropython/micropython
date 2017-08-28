#!/usr/bin/env python
#

import os
import subprocess
import glob
import json
import re


def get_boards():
    """Get a list of all boards.

    """

    paths = glob.glob("src/boards/*")

    return [os.path.basename(path) for path in paths]


def get_mcus():
    """Get a list of all mcus.

    """

    paths = glob.glob("src/mcus/*")

    return [os.path.basename(path) for path in paths]


def get_make_variable(board, variable):
    """Get the value of given make variable.

    """

    return subprocess.check_output(["make",
                                    "-s",
                                    "BOARD=" + board,
                                    "print-" + variable],
                                   cwd="examples/default-configuration")


def get_default_configuration(board):
    """Get the default configuration for given board.

    """

    return subprocess.check_output(["make",
                                    "-s",
                                    "BOARD=" + board,
                                    "pumbaa-default-configuration"],
                                   cwd="examples/default-configuration")


def main():
    """Main.

    """

    database = {
        "boards": dict([(board, {}) for board in get_boards()]),
        "mcus": {}
    }

    for board in database["boards"]:
        # Get the descriptive name, pinout and homepage
        variables = [
            "BOARD_DESC",
            "BOARD_HOMEPAGE",
            "BOARD_PINOUT",
            "SETTING_MEMORY",
            "SETTING_OFFSET",
            "SETTING_SIZE",
            "ENDIANESS"
        ]
        for variable in variables:
            value = get_make_variable(board, variable)
            database["boards"][board][variable.lower()] = value.strip()

        pumbaa_root = get_make_variable(board, "PUMBAA_ROOT").strip()
        pumbaa_root += '/'

        # Get the board default configuration.
        re_default_config = re.compile(r"#define (\w+) (.*)")
        default_config = []
        for line in get_default_configuration(board).splitlines():
            mo = re_default_config.match(line)
            default_config.append((mo.group(1), mo.group(2)))
        default_config.sort()
        database["boards"][board]["default-configuration"] = default_config
        
        # Get board drivers.
        drivers_src = get_make_variable(board, "DRIVERS_SRC").split()
        drivers = []
        for src in drivers_src:
            if not os.path.split(src)[0]:
                drivers.append(os.path.splitext(src)[0])
        database["boards"][board]["drivers"] = drivers

        # Get board include paths.
        inc = get_make_variable(board, "INC").split()
        inc = [i.replace(pumbaa_root, "") for i in inc]
        inc = list(set(inc) - set("."))
        database["boards"][board]["inc"] = inc

        # Get board sources.
        src = get_make_variable(board, "SRC").split()
        # The re lib sources are included in another source file.
        src += glob.glob(os.path.join(pumbaa_root,
                                      "micropython/extmod/re1.5/*.c"))
        src = list(set(src) - set(["main.c",
                                   os.path.join("build", board, "frozen.c"),
                                   os.path.join("build", board, "settings.c")]))
        src = [s.replace(pumbaa_root, "") for s in src]
        database["boards"][board]["src"] = src

        # Get CFLAGS.
        cflags = get_make_variable(board, "CFLAGS").split()
        database["boards"][board]["cflags"] = cflags

        # Get CXXFLAGS.
        cxxflags = get_make_variable(board, "CXXFLAGS").split()
        database["boards"][board]["cxxflags"] = cxxflags

        # Get CDEFS.
        cdefs = get_make_variable(board, "CDEFS").split()
        database["boards"][board]["cdefs"] = cdefs

        # Get LDFLAGS.
        ldflags = get_make_variable(board, "LDFLAGS").split()
        database["boards"][board]["ldflags"] = ldflags

        # Get LDFLAGS_AFTER.
        ldflags_after = get_make_variable(board, "LDFLAGS_AFTER").split()
        database["boards"][board]["ldflags_after"] = ldflags_after

        # Get LIB.
        lib = get_make_variable(board, "LIB").split()
        database["boards"][board]["lib"] = lib

        # Get LIBPATH.
        libpath = get_make_variable(board, "LIBPATH").split()
        libpath = [l.replace(pumbaa_root, "") for l in libpath]
        database["boards"][board]["libpath"] = libpath

        # Get linker file.
        try:
            linker_script = get_make_variable(board,
                                              "LINKER_SCRIPT").strip()
        except:
            linker_script = ""

        database["boards"][board]["linker_script"] = linker_script

        # get the board MCU
        mcu = get_make_variable(board, "MCU").strip()
        mcu = mcu.replace("/", "")
        database["boards"][board]["mcu"] = mcu

        if mcu not in database["mcus"]:
            database["mcus"][mcu] = {}
            # Get the descriptive name and homepage
            for variable in ["MCU_NAME", "MCU_HOMEPAGE", "ARCH", "FAMILY", "MCU_DESC"]:
                value = get_make_variable(board, variable)
                database["mcus"][mcu][variable.lower()] = value.strip()

    print json.dumps(database, indent=4)


if __name__ == "__main__":
    main()
