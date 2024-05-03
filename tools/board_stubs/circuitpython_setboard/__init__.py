# SPDX-FileCopyrightText: 2024 Tim Cocks
#
# SPDX-License-Identifier: MIT
import argparse
import sys
import shutil
from collections import defaultdict
from importlib import resources
from importlib.abc import Traversable


def get_definitions_or_raise(board: str) -> Traversable:
    """Get the definitions file for a board given its name."""

    path = resources.files("board_definitions").joinpath(board)

    file = path.joinpath("__init__.pyi")
    if not file.is_file():
        sys.stderr.write(f"Definitions for: '{board}' were not found\n")
        sys.exit(1)

    return file


def get_doc_or_raise(board: str) -> str:
    """Get the docstring for a board given its name."""

    with get_definitions_or_raise(board).open("r") as f:
        return f.read().split('"""')[1]


def header(txt: str) -> str:
    """Helper text formatter."""
    return txt + "\n" + "-" * len(txt) + "\n"


def set_board():
    parser = argparse.ArgumentParser(
        prog=__name__,
        usage="Install CircuitPython board-specific stubs",
    )
    parser.add_argument("chosen_board", help="selected board", nargs="?")
    parser.add_argument(
        "-l",
        "--list",
        help=f"show available boards. can filter eg: '{__name__} -l feather'",
        action="store_true",
    )

    args = parser.parse_args()

    if args.list:
        port_boards: defaultdict[str, list[str]] = defaultdict(list)

        for board in resources.files("board_definitions").iterdir():
            # if we receive both the list flag and a board name, use it as filter
            if (
                args.chosen_board is not None
                and args.chosen_board.lower() not in board.name.lower()
            ):
                continue

            # NOTE: For the hand-crafted finding of port in the docstring, its
            #       format is assumed to be:
            #
            # <empty line>
            # Board stub for ...
            #  - port: ...
            #  - board_id: ...
            #  - NVM size: ...
            #  - Included modules: ...
            #  - Frozen libraries: ...
            #

            lines = get_doc_or_raise(board).split("\n")
            port = lines[2].split("-")[1].split(":")[1].strip()

            port_boards[port].append(board.name)

        if not port_boards:
            sys.stdout.write("Nothing found, check out your filter.\n")
            sys.exit(0)

        sys.stdout.write("Available boards are: \n")
        # sort by port name
        for port, boards in sorted(port_boards.items(), key=lambda kv: kv[0]):
            sys.stdout.write(
                header(port)
                + "  * "
                # sort by board name
                + "\n  * ".join(sorted(boards))
                + "\n\n"
            )

        sys.exit(0)

    if args.chosen_board is None:
        sys.stderr.write("Must select a board\n")
        sys.exit(1)

    board_definitions_file = get_definitions_or_raise(args.chosen_board)

    board_stubs_file = resources.files("board-stubs").joinpath("__init__.pyi")
    shutil.copyfile(board_definitions_file, board_stubs_file)

    sys.stdout.write(
        header("Information about the board") + get_doc_or_raise(args.chose_board) + "\n"
    )
