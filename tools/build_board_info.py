#!/usr/bin/env python3

import json
import os
import subprocess
import sys
import sh
import base64
from datetime import date
from sh.contrib import git

sys.path.append("adabot")
import adabot.github_requests as github

SUPPORTED_PORTS = ["atmel-samd", "cxd56", "esp32s2", "litex", "mimxrt10xx", "nrf", "stm"]

BIN = ('bin',)
UF2 = ('uf2',)
BIN_UF2 = ('bin', 'uf2')
HEX = ('hex',)
HEX_UF2 = ('hex', 'uf2')
SPK = ('spk',)
DFU = ('dfu',)
BIN_DFU = ('bin', 'dfu')

# Example:
# https://downloads.circuitpython.org/bin/trinket_m0/en_US/adafruit-circuitpython-trinket_m0-en_US-5.0.0-rc.0.uf2
DOWNLOAD_BASE_URL = "https://downloads.circuitpython.org/bin"

# Default extensions
extension_by_port = {
    "nrf": UF2,
    "atmel-samd": UF2,
    "stm": BIN,
    "cxd56": SPK,
    "mimxrt10xx": HEX_UF2,
    "litex": DFU,
    "esp32s2": BIN
}

# Per board overrides
extension_by_board = {
    # samd
    "arduino_mkr1300": BIN_UF2,
    "arduino_mkrzero": BIN_UF2,
    "arduino_nano_33_iot": BIN_UF2,
    "arduino_zero": BIN_UF2,
    "feather_m0_adalogger": BIN_UF2,
    "feather_m0_basic": BIN_UF2,
    "feather_m0_rfm69": BIN_UF2,
    "feather_m0_rfm9x": BIN_UF2,
    "uchip": BIN_UF2,

    # nRF52840 dev kits that may not have UF2 bootloaders,
    "makerdiary_nrf52840_mdk": HEX,
    "makerdiary_nrf52840_mdk_usb_dongle": HEX_UF2,
    "pca10056": BIN_UF2,
    "pca10059": BIN_UF2,
    "electronut_labs_blip": HEX
}

aliases_by_board = {
    "circuitplayground_express": ["circuitplayground_express_4h", "circuitplayground_express_digikey_pycon2019"],
    "pybadge": ["edgebadge"],
    "pyportal": ["pyportal_pynt"],
    "gemma_m0": ["gemma_m0_pycon2018"],
    "pewpew10": ["pewpew13"]
}

def get_languages():
    languages = []
    for f in os.scandir("../locale"):
        if f.name.endswith(".po"):
            languages.append(f.name[:-3])
    return languages

def get_board_mapping():
    boards = {}
    for port in SUPPORTED_PORTS:
        board_path = os.path.join("../ports", port, "boards")
        for board_path in os.scandir(board_path):
            if board_path.is_dir():
                board_files = os.listdir(board_path.path)
                board_id = board_path.name
                extensions = extension_by_port[port]
                extensions = extension_by_board.get(board_path.name, extensions)
                aliases = aliases_by_board.get(board_path.name, [])
                boards[board_id] = {"port": port,
                                    "extensions": extensions,
                                    "download_count": 0,
                                    "aliases": aliases}
                for alias in aliases:
                    boards[alias] = {"port": port,
                                     "extensions": extensions,
                                     "download_count": 0,
                                     "alias": True,
                                     "aliases": []}
    return boards

def get_version_info():
    version = None
    sha = git("rev-parse", "--short", "HEAD").stdout.decode("utf-8")
    try:
        version = git("describe", "--tags", "--exact-match").stdout.decode("utf-8").strip()
    except sh.ErrorReturnCode_128:
        # No exact match
        pass

    if "GITHUB_SHA" in os.environ:
        sha = os.environ["GITHUB_SHA"]

    if not version:
        version="{}-{}".format(date.today().strftime("%Y%m%d"), sha[:7])

    return sha, version

def get_current_info():
    response = github.get("/repos/adafruit/circuitpython-org/git/refs/heads/master")
    if not response.ok:
        print(response.text)
        raise RuntimeError("cannot get master sha")
    commit_sha = response.json()["object"]["sha"]

    response = github.get("/repos/adafruit/circuitpython-org/contents/_data/files.json?ref=" + commit_sha)
    if not response.ok:
        print(response.text)
        raise RuntimeError("cannot get previous files.json")

    response = response.json()

    git_info = commit_sha, response["sha"]
    current_list = json.loads(base64.b64decode(response["content"]).decode("utf-8"))
    current_info = {}
    for info in current_list:
        current_info[info["id"]] = info
    return git_info, current_info

def create_pr(changes, updated, git_info, user):
    commit_sha, original_blob_sha = git_info
    branch_name = "new_release_" + changes["new_release"]

    # Convert the dictionary to a list of boards. Liquid templates only handle arrays.
    updated_list = []
    all_ids = sorted(updated.keys())
    for id in all_ids:
        info = updated[id]
        info["id"] = id
        updated_list.append(info)

    updated = json.dumps(updated_list, sort_keys=True, indent=4).encode("utf-8") + b"\n"
    #print(updated.decode("utf-8"))
    pr_title = "Automated website update for release {}".format(changes["new_release"])
    boards = ""
    if changes["new_boards"]:
        boards = "New boards:\n* " + "\n* ".join(changes["new_boards"])
    languages = ""
    if changes["new_languages"]:
        languages = "New languages:\n* " + "\n* ".join(changes["new_languages"])
    message = "Automated website update for release {} by Blinka.\n\n{}\n\n{}\n".format(
        changes["new_release"],
        boards,
        languages
    )

    create_branch = {
        "ref": "refs/heads/" + branch_name,
        "sha": commit_sha
    }
    response = github.post("/repos/{}/circuitpython-org/git/refs".format(user), json=create_branch)
    if not response.ok and response.json()["message"] != "Reference already exists":
        print("unable to create branch")
        print(response.text)
        return

    update_file = {
        "message": message,
        "content": base64.b64encode(updated).decode("utf-8"),
        "sha": original_blob_sha,
        "branch": branch_name
    }

    response = github.put("/repos/{}/circuitpython-org/contents/_data/files.json".format(user), json=update_file)
    if not response.ok:
        print("unable to post new file")
        print(response.text)
        return
    pr_info = {
        "title": pr_title,
        "head": user + ":" + branch_name,
        "base": "master",
        "body": message,
        "maintainer_can_modify": True
    }
    response = github.post("/repos/adafruit/circuitpython-org/pulls", json=pr_info)
    if not response.ok:
        print("unable to create pr")
        print(response.text)
        return
    print(changes)
    print(pr_info)

def update_downloads(boards, release):
    response = github.get("/repos/adafruit/circuitpython/releases/tags/{}".format(release))
    if not response.ok:
        print(response.text)
        raise RuntimeError("cannot get previous release info")

    assets = response.json()["assets"]
    for asset in assets:
        board_name = asset["name"].split("-")[2]
        if board_name not in boards:
            continue
        boards[board_name]["download_count"] += asset["download_count"]


def print_active_user():
    response = github.get("/user")
    if response.ok:
        user = response.json()["login"]
        print("Logged in as {}".format(user))
        return user
    else:
        print("Not logged in")
    return None

def generate_download_info():
    boards = {}
    errors = []

    new_tag = os.environ["RELEASE_TAG"]

    changes = {
        "new_release": new_tag,
        "new_boards": [],
        "new_languages": []
    }

    user = print_active_user()

    sha, this_version = get_version_info()

    git_info, current_info = get_current_info()

    languages = get_languages()

    new_stable = "-" not in new_tag

    previous_releases = set()
    previous_languages = set()

    # Delete the release we are replacing
    for board in current_info:
        info = current_info[board]
        for version in info["versions"]:
            previous_releases.add(version["version"])
            previous_languages.update(version["files"].keys())
            if version["stable"] == new_stable:
                info["versions"].remove(version)

    board_mapping = get_board_mapping()

    for release in previous_releases:
        update_downloads(board_mapping, release)

    for port in SUPPORTED_PORTS:
        board_path = os.path.join("../ports", port, "boards")
        for board_path in os.scandir(board_path):
            if board_path.is_dir():
                board_files = os.listdir(board_path.path)
                board_id = board_path.name
                board_info = board_mapping[board_id]

                for alias in [board_id] + board_info["aliases"]:
                    alias_info = board_mapping[alias]
                    if alias not in current_info:
                        changes["new_boards"].append(alias)
                        current_info[alias] = {"downloads": 0,
                                                "versions": []}

                    new_version = {
                        "stable": new_stable,
                        "version": new_tag,
                        "files": {}
                    }
                    for language in languages:
                        files = []
                        new_version["files"][language] = files
                        for extension in board_info["extensions"]:
                            files.append(
                                "{base_url}/{alias}/{language}/adafruit-circuitpython-{alias}-{language}-{tag}.{extension}"
                                .format(
                                    base_url=DOWNLOAD_BASE_URL,
                                    tag=new_tag,
                                    alias=alias,
                                    language=language,
                                    extension=extension))
                    current_info[alias]["downloads"] = alias_info["download_count"]
                    current_info[alias]["versions"].append(new_version)

    changes["new_languages"] = set(languages) - previous_languages

    if changes["new_release"] and user:
        create_pr(changes, current_info, git_info, user)
    else:
        print("No new release to update")

if __name__ == "__main__":
    if "RELEASE_TAG" in os.environ and os.environ["RELEASE_TAG"]:
        generate_download_info()
    else:
        print("skipping website update because this isn't a tag")
