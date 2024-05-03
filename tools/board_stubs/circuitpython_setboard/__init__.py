# SPDX-FileCopyrightText: 2024 Tim Cocks
#
# SPDX-License-Identifier: MIT
import argparse
import sys
import shutil
from importlib import resources


def set_board():
    parser = argparse.ArgumentParser(
        prog=__name__,
        usage="Install CircuitPython board-specific stubs",
    )
    parser.add_argument("chosen_board", help="selected board", nargs="?")
    parser.add_argument("-l", "--list", help="show available boards", action="store_true")

    args = parser.parse_args()

    if args.list:
        sys.stdout.write("Available boards are: \n")

        for board in resources.files("board_definitions").iterdir():
            sys.stdout.write(f"{board.name}\n")

        sys.exit(0)

    if args.chosen_board is None:
        sys.stderr.write("Must select a board")
        sys.exit(1)

    print(f"setting board: {args.chosen_board}")

    board_stubs_file = resources.files("board-stubs").joinpath("__init__.pyi")
    board_definitions_path = resources.files("board_definitions").joinpath(args.chosen_board)
    board_definitions_file = board_definitions_path.joinpath("__init__.pyi")

    if not board_definitions_file.is_file():
        print(f"Board: '{args.chosen_board}' was not found")
        sys.exit(1)

    shutil.copyfile(board_definitions_file, board_stubs_file)
