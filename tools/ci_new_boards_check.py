#! /usr/bin/env python3

# SPDX-FileCopyrightText: 2014 MicroPython & CircuitPython contributors (https://github.com/adafruit/circuitpython/graphs/contributors)
#
# SPDX-License-Identifier: MIT

import sys
import os
import json
import yaml

import build_board_info

workflow_file = ".github/workflows/build.yml"

# Get boards in json format
boards_info_json = build_board_info.get_board_mapping()

# Get all the boards out of the json format
info_boards = [
    board for board in boards_info_json.keys() if not boards_info_json[board].get("alias", False)
]

# We need to know the path of the workflow file
base_path = os.path.dirname(__file__)
yml_path = os.path.abspath(os.path.join(base_path, "..", workflow_file))

# Loading board list based on build jobs in the workflow file.
ci_boards = []
with open(yml_path, "r") as f:
    workflow = yaml.safe_load(f)

ok = True
for job in workflow["jobs"]:
    if not job.startswith("build"):
        continue
    job_boards = workflow["jobs"][job]["strategy"]["matrix"]["board"]
    if job_boards != sorted(job_boards):
        print('Boards for job "{}" not sorted. Must be:'.format(job))
        print('        - "' + '"\n        - "'.join(sorted(job_boards)) + '"')
        ok = False
    ci_boards.extend(job_boards)

# All the travis_boards elements must be on info_boards
info_boards.sort()
ci_boards.sort()

missing_boards = set(info_boards) - set(ci_boards)

if missing_boards:
    ok = False
    print("Boards missing in {}:".format(workflow_file))
    for board in missing_boards:
        print(board)

if not ok:
    sys.exit(1)
