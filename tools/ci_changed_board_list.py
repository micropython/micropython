#! /usr/bin/env python3

"""This script is used in GitHub Actions to determine what boards are built
  based on what files were changed. The base commit varies depending on the
  event that triggered run. Pull request runs will compare to the base branch
  while pushes will compare to the current ref. We override this for the
  adafruit/circuitpython repo so we build all boards for pushes.
  """

# SPDX-FileCopyrightText: 2021 Scott Shawcroft
#
# SPDX-License-Identifier: MIT

import sys
import os
import json
import yaml
import re

import build_board_info

PORT_TO_ARCH = {
    "atmel-samd": "arm",
    "cxd56": "arm",
    "espressif": "espressif",
    "litex": "riscv",
    "mimxrt10xx": "arm",
    "nrf": "arm",
    "raspberrypi": "arm",
    "stm": "arm",
}

changed_files = json.loads(os.environ["CHANGED_FILES"])
print("changed_files")
print(changed_files)

# Get boards in json format
boards_info_json = build_board_info.get_board_mapping()
all_board_ids = set()
port_to_boards = {}
board_to_port = {}
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

# Build all boards if this is a push to an adafruit/circuitpython branch because we save the artifacts.
boards_to_build = all_board_ids
if not changed_files or (
    os.environ.get("GITHUB_EVENT_NAME", "") == "push"
    and os.environ.get("GITHUB_REPOSITORY", "") == "adafruit/circuitpython"
):
    print("Building all boards because of adafruit/circuitpython branch")
else:
    print("Adding boards to build based on changed files")
    boards_to_build = set()
    board_pattern = re.compile(r"ports/\w+/boards/(\w+)/")
    port_pattern = re.compile(r"ports/(\w+)/")
    for p in changed_files:
        # See if it is board specific
        board_matches = board_pattern.search(p)
        if board_matches:
            board = board_matches.group(1)
            boards_to_build.add(board)
            continue

        # See if it is port specific
        port_matches = port_pattern.search(p)
        if port_matches:
            port = port_matches.group(1)
            boards_to_build.update(port_to_boards[port])
            continue

        # Otherwise build it all
        boards_to_build = all_board_ids
        break

# Split boards by architecture.
print("Building boards:")
arch_to_boards = {"arm": [], "riscv": [], "espressif": []}
for board in boards_to_build:
    print(" ", board)
    arch = PORT_TO_ARCH[board_to_port[board]]
    arch_to_boards[arch].append(board)

# Set the step outputs for each architecture
for arch in arch_to_boards:
    print("::set-output name=" + arch + "-boards::" + json.dumps(sorted(arch_to_boards[arch])))
