# SPDX-FileCopyrightText: 2024 Justin Myers
#
# SPDX-License-Identifier: MIT

import json
import os
import re
import sys
from collections import defaultdict


def get_board_pins(pin_filename):
    records = []

    with open(pin_filename, encoding="utf-8") as pin_file:
        for line in pin_file:
            if line.strip()[0:2] == "//":
                continue

            # \s* means any amount of whitespaces (no whitespaces allowed too)
            # related issue: https://github.com/adafruit/circuitpython/issues/9407

            search = re.search(r"MP_ROM_QSTR\(MP_QSTR_(.*?)\),\s*MP_ROM_PTR", line)
            if search is None:
                search = re.search(r"MP_OBJ_NEW_QSTR\(MP_QSTR_(.*?)\),\s*MP_ROM_PTR", line)
            if search is None:
                continue

            board_member = search.group(1)

            board_type_search = re.search(r"MP_ROM_PTR\(&pin_(.*?)\)", line)
            if board_type_search:
                board_type = "pin"
            else:
                if board_type_search is None:
                    board_type_search = re.search(r"MP_ROM_PTR\(&(.*?)_obj", line)
                if board_type_search is None:
                    board_type_search = re.search(
                        r"MP_ROM_PTR\(&(.*?)\[0\].[display|epaper_display]", line
                    )
                if board_type_search is None:
                    records.append(["unmapped", None, line])
                    continue

                board_type = board_type_search.group(1)

            extra_search = None
            extra = None

            if board_type == "pin":
                extra_search = re.search(r"&pin_(.*?)\)", line)

            elif board_type == "displays":
                extra_search = re.search(r"&displays\[0\].(.*?)\)", line)

            if extra_search:
                extra = extra_search.group(1)

            records.append([board_type, board_member, extra])

    return records


def create_board_stubs(board_id, records, mappings, board_filename):
    pins = []
    members = []
    unmapped = []

    needs_busio = False
    needs_displayio = False
    needs_microcontroller = False

    for board_type, board_member, extra in records:
        if board_type == "pin":
            needs_microcontroller = True
            comment = f"  # {extra}"
            pins.append(f"{board_member}: microcontroller.Pin{comment}")

        elif board_type == "board_i2c":
            needs_busio = True
            import_name = "I2C"
            class_name = f"busio.{import_name}"
            member_data = f"def {board_member}() -> {class_name}:\n"
            member_data += f'    """Returns the `{class_name}` object for the board\'s designated I2C bus(es).\n'
            member_data += f"    The object created is a singleton, and uses the default parameter values for `{class_name}`.\n"
            member_data += '    """\n'
            members.append(member_data)

        elif board_type == "board_spi":
            needs_busio = True
            import_name = "SPI"
            class_name = f"busio.{import_name}"
            member_data = f"def {board_member}() -> {class_name}:\n"
            member_data += f'    """Returns the `{class_name}` object for the board\'s designated SPI bus(es).\n'
            member_data += f"    The object created is a singleton, and uses the default parameter values for `{class_name}`.\n"
            member_data += '    """\n'
            members.append(member_data)

        elif board_type == "board_uart":
            needs_busio = True
            import_name = "UART"
            class_name = f"busio.{import_name}"
            member_data = f"def {board_member}() -> {class_name}:\n"
            member_data += f'    """Returns the `{class_name}` object for the board\'s designated UART bus(es).\n'
            member_data += f"    The object created is a singleton, and uses the default parameter values for `{class_name}`.\n"
            member_data += '    """\n'
            members.append(member_data)

        elif board_type == "displays":
            needs_displayio = True
            if extra == "display":
                import_name = "Display"
            elif extra == "epaper_display":
                import_name = "EPaperDisplay"
            class_name = f"displayio.{import_name}"
            member_data = (
                f'"""Returns the `{class_name}` object for the board\'s built in display.\n'
            )
            member_data += f"The object created is a singleton, and uses the default parameter values for `{class_name}`.\n"
            member_data += '"""\n'
            member_data += f"{board_member}: {class_name}\n"
            members.append(member_data)

        elif board_type == "unmapped":
            unmapped.append(extra)

    libraries = mappings["libraries"]
    included_modules = "Unknown"
    frozen_libraries = "Unknown"
    if "modules" in libraries:
        included_modules = ", ".join(libraries["modules"])
    if "frozen_libraries" in libraries:
        frozen_libraries = ", ".join(libraries["frozen_libraries"])

    with open(board_filename, "w") as boards_file:
        boards_file.write("# SPDX-FileCopyrightText: 2024 Justin Myers\n")
        boards_file.write("#\n")
        boards_file.write("# SPDX-License-Identifier: MIT\n")
        boards_file.write('"""\n')
        boards_file.write(f'Board stub for {mappings["board_name"]}\n')
        boards_file.write(f' - port: {mappings["port"]}\n')
        boards_file.write(f" - board_id: {board_id}\n")
        boards_file.write(f' - NVM size: {mappings["nvm_size"]}\n')
        boards_file.write(f" - Included modules: {included_modules}\n")
        boards_file.write(f" - Frozen libraries: {frozen_libraries}\n")
        boards_file.write('"""\n\n')
        boards_file.write("# Imports\n")
        if needs_busio:
            boards_file.write("import busio\n")
        if needs_displayio:
            boards_file.write("import displayio\n")
        if needs_microcontroller:
            boards_file.write("import microcontroller\n")

        boards_file.write("\n\n")
        boards_file.write("# Board Info:\n")
        boards_file.write("board_id: str\n")

        boards_file.write("\n\n")
        boards_file.write("# Pins:\n")
        for pin in pins:
            boards_file.write(f"{pin}\n")

        boards_file.write("\n\n")
        boards_file.write("# Members:\n")
        for member in members:
            boards_file.write(f"{member}\n")

        boards_file.write("\n")
        boards_file.write("# Unmapped:\n")
        if not unmapped:
            boards_file.write("#   none\n")
        for record in unmapped:
            boards_file.write(f"# {record}")


def process(board_mappings, export_dir):
    total_boards = 0
    total_pins = 0
    total_members = 0
    total_unmapped = 0
    skipped_boards = []
    unmapped_boards = defaultdict(int)
    unmapped_values = defaultdict(list)

    for board_id, mappings in board_mappings.items():
        total_boards += 1

        if "pins_filename" not in mappings:
            skipped_boards.append(board_id)
            continue

        pin_filename = mappings["pins_filename"]

        sub_dir = f"{export_dir}/{board_id}"
        if not os.path.exists(sub_dir):
            os.makedirs(sub_dir)

        try:
            records = get_board_pins(pin_filename)
            create_board_stubs(board_id, records, mappings, f"{sub_dir}/__init__.pyi")

            for board_type, board_member, extra in records:
                if board_type == "pin":
                    total_pins += 1
                elif board_type == "unmapped":
                    unmapped_boards[board_id] += 1
                    unmapped_values[extra].append(board_id)
                    total_unmapped += 1
                else:
                    total_members += 1

        except Exception as e:
            print(f" - {e}")

    unmapped_percent = total_unmapped / (total_pins + total_members + total_unmapped)

    print("\nTotals:")
    print(f"  boards: {total_boards}")
    print(f"  pins: {total_pins}")
    print(f"  members: {total_members}")
    print(f"  unmapped: {total_unmapped} ({unmapped_percent:.5f}%)")
    print("\n\nSkipped Boards")
    for board in skipped_boards:
        print(f"  {board}")
    print("\n\nBoards with Unmapped Pins:")
    for board, total in unmapped_boards.items():
        print(f"  {board}: {total}")
    print("\n\nUnmapped Pins:")
    for unmapped, boards in unmapped_values.items():
        print(f"  {unmapped.strip()}")
        for board in boards:
            print(f"    - {board}")


def build_stubs(circuitpython_dir, circuitpython_org_dir, export_dir, version="8.2.9"):
    if circuitpython_dir[-1] != "/":
        circuitpython_dir = circuitpython_dir + "/"

    sys.path.append(circuitpython_dir)
    from docs import shared_bindings_matrix

    if not os.path.exists(export_dir):
        os.makedirs(export_dir)

    libraries = {}
    if circuitpython_org_dir is None:
        libraries = shared_bindings_matrix.support_matrix_by_board(
            use_branded_name=False, withurl=False
        )
    else:
        with open(f"{circuitpython_org_dir}/_data/files.json") as libraries_file:
            libraries_list = json.load(libraries_file)
        for library in libraries_list:
            board = library["id"]
            for version_data in library["versions"]:
                if version_data["version"] == version:
                    libraries[board] = version_data

    aliases = {}
    for board, renames in shared_bindings_matrix.ALIASES_BY_BOARD.items():
        for rename in renames:
            aliases[rename] = board

    board_mappings = shared_bindings_matrix.get_board_mapping()
    for board, board_data in board_mappings.items():
        if board in aliases:
            lookup = aliases[board]
        else:
            lookup = board

        port_path = f'{circuitpython_dir}ports/{board_data["port"]}/'
        board_path = f"{port_path}boards/{lookup}/"
        pins_path = f"{board_path}pins.c"
        if not os.path.isfile(pins_path):
            print(f"Could not find pin file for {lookup}")
            continue

        board_mappings[board]["pins_filename"] = pins_path

        nvm_size = "Unknown"
        with open(f"{port_path}/mpconfigport.h") as get_name:
            port_contents = get_name.read()
            port_nvm_re = re.search(
                r"(?<=#define CIRCUITPY_INTERNAL_NVM_SIZE)\s+(.+)", port_contents
            )
            if port_nvm_re:
                nvm_size = port_nvm_re.group(1).strip()

        board_name = board
        with open(f"{board_path}/mpconfigboard.h") as get_name:
            board_contents = get_name.read()
            board_name_re = re.search(r"(?<=MICROPY_HW_BOARD_NAME)\s+(.+)", board_contents)
            if board_name_re:
                board_name = board_name_re.group(1).strip('"')

            port_nvm_re = re.search(
                r"(?<=#define CIRCUITPY_INTERNAL_NVM_SIZE)\s+(.+)", port_contents
            )
            if port_nvm_re:
                nvm_size = port_nvm_re.group(1).strip()

        nvm_size_re = re.search(r"^[0-9\(\) *]*$", nvm_size)
        if nvm_size_re:
            nvm_size = eval(nvm_size_re.group(0))

        board_mappings[board]["board_name"] = board_name
        board_mappings[board]["nvm_size"] = nvm_size
        board_mappings[board]["libraries"] = libraries.get(board, None)

    process(board_mappings, export_dir)


if __name__ == "__main__":
    build_stubs("./", None, "circuitpython-stubs/board_definitions/")
