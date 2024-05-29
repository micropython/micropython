# The MIT License (MIT)
#
# SPDX-FileCopyrightText: Copyright (c) 2024 Tim Chinowsky
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

import re
import sys

sys.path.append("../docs")

from shared_bindings_matrix import support_matrix_by_board


def module_incidence_matrix_csvs(support_matrix, rows=1000, present="1", absent=""):
    all_modules = set()
    for info in support_matrix.values():
        all_modules.update(info["modules"])
    all_modules = sorted(all_modules)
    csvs = []
    row = 0
    for board, info in support_matrix.items():
        if (row % rows) == 0:
            csv = ["board,branded_name,mcu,flash,port,pins," + ",".join(all_modules) + "\n"]
        chip_pin_set = set([chip_pin for _, chip_pin in info["pins"]])
        n_chip_pins = len(chip_pin_set)
        module_incidence = [present if m in info["modules"] else absent for m in all_modules]
        line = (
            f"{board},{info.get('branded_name')},{info.get('mcu')},"
            + f"{info.get('flash')},{info.get('port')},{n_chip_pins},"
            + ",".join(module_incidence)
            + "\n"
        )
        csv.append(line)
        row += 1
        if (row % rows) == 0:
            csvs.append(csv)
    if (row % rows) != 0:
        csvs.append(csv)
    return csvs


def frozen_incidence_matrix_csvs(support_matrix, rows=1000, present="1", absent=""):
    all_frozen = set()
    for info in support_matrix.values():
        frozen = info["frozen_libraries"]
        frozen = [f[0] if type(f) == tuple else f for f in frozen]
        all_frozen.update(frozen)
    all_frozen = sorted(all_frozen)
    csvs = []
    row = 0
    for board, info in support_matrix.items():
        if (row % rows) == 0:
            csv = ["board,branded_name,mcu,flash,port,pins," + ",".join(all_frozen) + "\n"]
        # remove urls if present
        frozen = info["frozen_libraries"]

        chip_pin_set = set([chip_pin for _, chip_pin in info["pins"]])
        n_chip_pins = len(chip_pin_set)

        frozen = [f[0] if type(f) == tuple else f for f in frozen]
        frozen_incidence = [present if f in frozen else absent for f in all_frozen]
        line = (
            f"{board},{info.get('branded_name')},{info.get('mcu')},"
            + f"{info.get('flash')},{info.get('port')},{n_chip_pins},"
            + ",".join(frozen_incidence)
            + "\n"
        )
        csv.append(line)
        row += 1
        if (row % rows) == 0:
            csvs.append(csv)
    if (row % rows) != 0:
        csvs.append(csv)
    return csvs


def summarize_pins(pins):
    """Given a list of pin names, generate a compact string
    like "BUTTON, LED1:4, PA0:10;13:14, PB5:7, SCL, SDA"
    summarizing the names in the list"""
    pin_prefixes = {}
    for p in pins:
        match = re.match(r"^(.*?)(\d*)$", p)
        if match:
            prefix = match.group(1)
            n_str = match.group(2)
        else:
            raise ValueError("Cannot parse pin name")
        if prefix in pin_prefixes:
            pin_prefixes[prefix].add(n_str)
        else:
            pin_prefixes[prefix] = {n_str}

    return ", ".join(
        [f"{prefix}{span_string(pin_prefixes[prefix])}" for prefix in sorted(pin_prefixes.keys())]
    )


def int_or_zero(s):
    try:
        return int(s)
    except ValueError:
        return 0


def span_string(number_strs):
    int_strs = sorted([(int_or_zero(s), s) for s in number_strs])
    last_i = None
    last_s = None
    run = False
    for i, s in int_strs:
        if last_i is None:
            out = s
        elif i != last_i + 1:
            if run:
                out += f":{last_s};{s}"
            else:
                out += f";{s}"
            run = False
        else:
            run = True
        last_i = i
        last_s = s
    if run:
        out += f":{s}"
    return out


def board_pins_matrix_csvs(support_matrix, rows=1000):
    csvs = []
    row = 0
    for board, info in support_matrix.items():
        if (row % rows) == 0:
            csv = [
                "board,branded_name,mcu,flash,port,n_board_pins,"
                "board_pins,n_chip_pins,chip_pins\n"
            ]
        board_pins = [board_pin for board_pin, _ in info["pins"]]
        chip_pins = [chip_pin for _, chip_pin in info["pins"]]
        line = (
            f"{board},{info.get('branded_name')},{info.get('mcu')},"
            + f"{info.get('flash')},{info.get('port')},"
            + str(len(set(board_pins)))
            + ',"'
            + summarize_pins(board_pins)
            + '",'
            + str(len(set(chip_pins)))
            + ',"'
            + summarize_pins(chip_pins)
            + '"\n'
        )
        csv.append(line)
        row += 1
        if (row % rows) == 0:
            csvs.append(csv)
    if (row % rows) != 0:
        csvs.append(csv)
    return csvs


def write_csvs(rows=1000, present="1", absent="0"):
    print("generating csvs...")
    s = support_matrix_by_board(
        use_branded_name=False, add_port=True, add_chips=True, add_pins=True, add_branded_name=True
    )
    csvs = {
        "modules": module_incidence_matrix_csvs(s, rows=rows, present=present, absent=absent),
        "frozen": frozen_incidence_matrix_csvs(s, rows=rows, present=present, absent=absent),
        "pins": board_pins_matrix_csvs(s, rows=rows),
    }
    for key in csvs:
        for i in range(len(csvs[key])):
            filename = f"{key}_{i}.csv"
            print(f"writing {filename}")
            with open(filename, "w") as f:
                f.writelines(csvs[key][i])


if __name__ == "__main__":
    write_csvs()
