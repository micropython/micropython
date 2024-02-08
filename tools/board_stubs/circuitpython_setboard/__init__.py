# SPDX-FileCopyrightText: 2024 Tim Cocks
#
# SPDX-License-Identifier: MIT
import sys
import os
import shutil


def set_board():
    chosen_board = sys.argv[1]
    print(f"setting board: {chosen_board}")
    site_packages_path = os.path.sep.join(__file__.split(os.path.sep)[:-2])
    board_defs_path = f"{site_packages_path}{os.path.sep}board_definitions{os.path.sep}"
    board_stubs_path = f"{site_packages_path}{os.path.sep}board-stubs{os.path.sep}__init__.pyi"

    if chosen_board not in os.listdir(board_defs_path):
        print(f"Board: '{chosen_board}' was not found")
        return

    shutil.copyfile(
        board_defs_path + f"{os.path.sep}{chosen_board}{os.path.sep}__init__.pyi", board_stubs_path
    )
