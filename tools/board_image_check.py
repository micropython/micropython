#!/usr/bin/env python
import glob
import json
import os.path
import sys
import urllib.request
from urllib.error import HTTPError

# Check that board image files are present in the media repo
# (if not, they need to be merged to https://github.com/micropython/micropython-media/)

# This URL on GitHub corresponds to https://micropython.org/resources/micropython-media/boards once published
BASE_URL = "https://raw.githubusercontent.com/micropython/micropython-media/refs/heads/main/boards"


def check_board(board_json_path):
    # Board name is the name of the directory containing the board.json file
    board_name = os.path.basename(os.path.dirname(os.path.realpath(board_json_path)))

    try:
        with open(board_json_path, "r") as f:
            contents = json.load(f)
    except json.JSONDecodeError:
        print(f"ERROR: Board file {board_json_path} is not valid JSON")
        return False

    images = contents.get("images", [])
    if not images:
        print(f"WARNING: Board {board_name} has no images.")

    for image in images:
        url = f"{BASE_URL}/{board_name}/{image}"
        print(f"Testing {url}")
        req = urllib.request.Request(url, method="HEAD")
        try:
            with urllib.request.urlopen(req) as response:
                for k, v in response.getheaders():
                    if k.lower() == "content-type":
                        if "image" not in v:
                            print(f"... ERROR: Unexpected Content-Type: {v}")
                            return False
                        break
                print("... OK")
                return True
        except HTTPError as e:
            print(f"... ERROR: {e}")
            return False


def main(paths_to_test):
    missing = [a for a in paths_to_test if not os.path.exists(a)]
    if any(missing):
        raise SystemExit(f"File(s) not found: {missing}")

    failed = [b for b in paths_to_test if not check_board(b)]
    print("***")
    if not failed:
        print("All board files found in micropython-media repository")
    else:
        print(
            f"{len(failed)}/{len(paths_to_test)} boards have images missing from micropython-media repository:"
        )
        for f in failed:
            print(f"  - {f}")
        print(
            "To correct, please raise a PR in https://github.com/micropython/micropython-media adding the images under boards/"
        )
        raise SystemExit(1)


if __name__ == "__main__":
    if len(sys.argv) == 1 or sys.argv[1] in ("-h", "--help"):
        print("Usage: tools.board_image_check.py [--all|BOARD_JSON_FILES]")
    elif "--all" in sys.argv:
        print("Scanning all board.json files under current directory...")
        main(glob.glob("**/board.json", recursive=True))
    else:
        main(sys.argv[1:])
