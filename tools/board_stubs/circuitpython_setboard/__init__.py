# SPDX-FileCopyrightText: 2024 Tim Cocks
#
# SPDX-License-Identifier: MIT
from importlib import resources
import os
import sys
import shutil


def set_board():
    if len(sys.argv) != 2:
        print(f"Incorrect args. Please call with: 'circuitpython_setboard chosen_board'")
        return

    chosen_board = sys.argv[1]
    print(f"setting board: {chosen_board}")

    board_stubs_file = resources.files("board-stubs").joinpath("__init__.pyi")
    board_definitions_path = resources.files("board_definitions").joinpath(chosen_board)
    board_definitions_file = board_definitions_path.joinpath("__init__.pyi")

    if not board_definitions_file.is_file():
        print(f"Board: '{chosen_board}' was not found")
        return

    shutil.copyfile(board_definitions_file, board_stubs_file)
