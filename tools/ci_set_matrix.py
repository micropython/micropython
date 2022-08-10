#! /usr/bin/env python3

# SPDX-FileCopyrightText: 2021 Scott Shawcroft
# SPDX-FileCopyrightText: 2021 microDev
#
# SPDX-License-Identifier: MIT

"""
This script is used in GitHub Actions to determine what docs/boards are
built based on what files were changed. The base commit varies depending
on the event that triggered run. Pull request runs will compare to the
base branch while pushes will compare to the current ref. We override this
for the adafruit/circuitpython repo so we build all docs/boards for pushes.

"""

import re
import os
import sys
import json
import yaml

import build_board_info
from shared_bindings_matrix import get_settings_from_makefile

PORT_TO_ARCH = {
    "atmel-samd": "arm",
    "broadcom": "aarch",
    "cxd56": "arm",
    "espressif": "espressif",
    "litex": "riscv",
    "mimxrt10xx": "arm",
    "nrf": "arm",
    "raspberrypi": "arm",
    "stm": "arm",
}

IGNORE = [
    "tools/ci_set_matrix.py",
    "tools/ci_check_duplicate_usb_vid_pid.py",
]

changed_files = {}
try:
    changed_files = json.loads(os.environ["CHANGED_FILES"])
except json.decoder.JSONDecodeError as exc:
    if exc.msg != "Expecting value":
        raise


def set_boards_to_build(build_all):
    # Get boards in json format
    boards_info_json = build_board_info.get_board_mapping()
    all_board_ids = set()
    port_to_boards = {}
    board_to_port = {}
    board_settings = {}
    for board_id in boards_info_json:
        info = boards_info_json[board_id]
        if info.get("alias", False):
            continue
        all_board_ids.add(board_id)
        port = info["port"]
        if port not in port_to_boards:
            port_to_boards[port] = set()
        port_to_boards[port].add(board_id)
        board_to_port[board_id] = port

    boards_to_build = all_board_ids

    if not build_all:
        boards_to_build = set()
        board_pattern = re.compile(r"^ports/[^/]+/boards/([^/]+)/")
        port_pattern = re.compile(r"^ports/([^/]+)/")
        module_pattern = re.compile(
            r"^(ports/[^/]+/common-hal|shared-bindings|shared-module)/([^/]+)/"
        )
        for p in changed_files:
            # See if it is board specific
            board_matches = board_pattern.search(p)
            if board_matches:
                board = board_matches.group(1)
                boards_to_build.add(board)
                continue

            # See if it is port specific
            port_matches = port_pattern.search(p)
            module_matches = module_pattern.search(p)
            if port_matches and not module_matches:
                port = port_matches.group(1)
                if port != "unix":
                    boards_to_build.update(port_to_boards[port])
                continue

            # Check the ignore list to see if the file isn't used on board builds.
            if p in IGNORE:
                continue

            # Boards don't run tests or docs so ignore those as well.
            if p.startswith("tests") or p.startswith("docs"):
                continue

            # As a (nearly) last resort, for some certain files, we compute the settings from the
            # makefile for each board and determine whether to build them that way.
            if p.startswith("frozen") or p.startswith("supervisor") or module_matches:
                for board in all_board_ids:
                    if board not in board_settings:
                        board_settings[board] = get_settings_from_makefile(
                            "../ports/" + board_to_port[board], board
                        )
                    settings = board_settings[board]

                    # Check frozen files to see if they are in each board.
                    frozen = settings.get("FROZEN_MPY_DIRS", "")
                    if frozen and p.startswith("frozen") and p in frozen:
                        boards_to_build.add(board)
                        continue

                    # Check supervisor files. This is useful for limiting workflow changes to the
                    # relevant boards.
                    supervisor = settings["SRC_SUPERVISOR"]
                    if p.startswith("supervisor"):
                        if p in supervisor:
                            boards_to_build.add(board)
                            continue

                        web_workflow = settings["CIRCUITPY_WEB_WORKFLOW"]
                        while web_workflow.startswith("$("):
                            web_workflow = settings[web_workflow[2:-1]]
                        if (
                            p.startswith("supervisor/shared/web_workflow/static/")
                            and web_workflow != "0"
                        ):
                            boards_to_build.add(board)
                            continue

                    # Check module matches
                    if module_matches:
                        module = module_matches.group(2) + "/"
                        if module in settings["SRC_PATTERNS"]:
                            boards_to_build.add(board)
                            continue
                continue

            # Otherwise build it all
            boards_to_build = all_board_ids
            break

    # Split boards by architecture.
    print("Building boards:")
    arch_to_boards = {"aarch": [], "arm": [], "riscv": [], "espressif": []}
    for board in sorted(boards_to_build):
        print(" ", board)
        port = board_to_port.get(board)
        # A board can appear due to its _deletion_ (rare)
        # if this happens it's not in `board_to_port`.
        if not port:
            continue
        arch = PORT_TO_ARCH[port]
        arch_to_boards[arch].append(board)

    # Set the step outputs for each architecture
    for arch in arch_to_boards:
        print("::set-output name=boards-" + arch + "::" + json.dumps(sorted(arch_to_boards[arch])))


def set_docs_to_build(build_all):
    doc_match = build_all
    if not build_all:
        doc_pattern = re.compile(
            r"^(?:docs|extmod/ulab|(?:(?:ports/\w+/bindings|shared-bindings)\S+\.c|conf\.py|tools/extract_pyi\.py|requirements-doc\.txt)$)|(?:-stubs|\.(?:md|MD|rst|RST))$"
        )
        for p in changed_files:
            if doc_pattern.search(p):
                doc_match = True
                break

    # Set the step outputs
    print("Building docs:", doc_match)
    print("::set-output name=build-doc::" + str(doc_match))


def check_changed_files():
    if not changed_files:
        print("Building all docs/boards")
        return True
    else:
        print("Adding docs/boards to build based on changed files")
        return False


def main():
    build_all = check_changed_files()
    set_docs_to_build(build_all)
    set_boards_to_build(build_all)


if __name__ == "__main__":
    main()
