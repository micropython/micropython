#!/usr/bin/env python3

import glob
import json
import os
import sys

VALID_FEATURES = {
    # Connectivity
    "BLE",
    "CAN",
    "Ethernet",
    "LoRa",
    "USB",
    "USB-C",
    "WiFi",
    # MCU features
    "Dual-core",
    "External Flash",
    "External RAM",
    # Form factor
    "Feather",
    # Connectors / sockets
    "JST-PH",
    "JST-SH",
    "mikroBUS",
    "microSD",
    "SDCard",
    # Sensors
    "Environment Sensor",
    "IMU",
    # Other
    "Audio Codec",
    "Battery Charging",
    "Camera",
    "DAC",
    "Display",
    "Microphone",
    "PoE",
    "RGB LED",
    "Secure Element",
}


def main(repo_path, output_path):
    boards_index = []
    board_ids = set()

    for board_json in glob.glob(os.path.join(repo_path, "ports/*/boards/*/board.json")):
        # Relative path to the board directory (e.g. "ports/stm32/boards/PYBV11").
        board_dir = os.path.dirname(board_json)
        # Relative path to the port (e.g. "ports/stm32")
        port_dir = os.path.dirname(os.path.dirname(board_dir))

        with open(board_json, "r") as f:
            blob = json.load(f)

            features = set(blob.get("features", []))
            if not features.issubset(VALID_FEATURES):
                print(
                    board_json,
                    "unknown features:",
                    features.difference(VALID_FEATURES),
                    file=sys.stderr,
                )
                sys.exit(1)

            # The ID of a board is the board directory (e.g. "PYBV11").
            blob["id"] = os.path.basename(board_dir)

            # Check for duplicate board IDs.
            if blob["id"] in board_ids:
                print("Duplicate board ID: '{}'".format(blob["id"]), file=sys.stderr)
            board_ids.add(blob["id"])

            # Add in default fields.
            blob["port"] = os.path.basename(port_dir)
            blob["build"] = os.path.basename(board_dir)
            boards_index.append(blob)

        # Create the board markdown, which is the concatenation of the
        # default "board.md" file (if exists), as well as any flashing
        # instructions.
        board_markdown = os.path.join(board_dir, "board.md")
        with open(os.path.join(output_path, blob["id"] + ".md"), "w") as f:
            if os.path.exists(board_markdown):
                with open(board_markdown, "r") as fin:
                    f.write(fin.read())

            if blob["deploy"]:
                f.write("\n\n## Installation instructions\n")
            for deploy in blob["deploy"]:
                with open(os.path.join(board_dir, deploy), "r") as fin:
                    body = fin.read()
                    # any key in the board.json file can be substituted via Python str.format()
                    try:
                        body = body.format(**blob)
                    except Exception as e:
                        raise RuntimeError(
                            "Failed to format deploy file {} according to {}: {}".format(
                                fin.name, board_json, e
                            )
                        )
                    f.write(body)
                    f.write("\n")

    # Write the full index for the website to load.
    with open(os.path.join(output_path, "index.json"), "w") as f:
        json.dump(boards_index, f, indent=4, sort_keys=True)
        f.write("\n")


if __name__ == "__main__":
    main(sys.argv[1], sys.argv[2])
