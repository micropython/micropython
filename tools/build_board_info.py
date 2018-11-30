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

SUPPORTED_PORTS = ["nrf", "esp8266", "atmel-samd"]

BIN = ('bin',)
UF2 = ('uf2',)
BIN_UF2 = ('bin', 'uf2')
HEX = ('hex',)

# Default extensions
extension_by_port = {
    "nrf": BIN,
    "esp8266": BIN,
    "atmel-samd": UF2,
}

# Per board overrides
extension_by_board = {
    "feather_nrf52832": BIN,
    "arduino_mkr1300": BIN,
    "arduino_zero": BIN,
    "feather_m0_adalogger": BIN_UF2,
    "feather_m0_basic": BIN_UF2,
    "feather_m0_rfm69": BIN,
    "feather_m0_rfm9x": BIN,
    "makerdiary_nrf52840_mdk": HEX
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
                boards[board_id] = {"port": port, "extensions": extensions, "download_count": 0}
    return boards

def get_version_info():
    version = None
    sha = git("rev-parse", "--short", "HEAD").stdout.decode("utf-8")
    try:
        version = git("describe", "--tags", "--exact-match").stdout.decode("utf-8")
    except sh.ErrorReturnCode_128:
        # No exact match
        pass

    if "TRAVIS" in os.environ and os.environ["TRAVIS"] == "true":
        sha = os.environ["TRAVIS_COMMIT"]
        if os.environ["TRAVIS_PULL_REQUEST"] != "false":
            sha = os.environ["TRAVIS_PULL_REQUEST_SHA"]

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
    return git_info, json.loads(base64.b64decode(response["content"]))

def create_pr(changes, updated, git_info):
    commit_sha, original_blob_sha = git_info
    branch_name = "new_release_" + changes["new_release"]

    updated = json.dumps(updated, sort_keys=True, indent=4).encode("utf-8") + b"\n"
    print(updated.decode("utf-8"))
    pr_title = "Automated website update for release {}".format(changes["new_release"])
    boards = ""
    if changes["new_boards"]:
        boards = "New boards:\n* " + "\n* ".join(changes["new_boards"])
    languages = ""
    if changes["new_languages"]:
        languages = "New languages:\n* " + "\n* ".join(changes["new_languages"])
    message = "Automated website update for release {} by AdaBot.\n\n{}\n\n{}\n".format(
        changes["new_release"],
        boards,
        languages
    )

    create_branch = {
        "ref": "refs/heads/" + branch_name,
        "sha": commit_sha
    }
    response = github.post("/repos/adafruit-adabot/circuitpython-org/git/refs", json=create_branch)
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

    response = github.put("/repos/adafruit-adabot/circuitpython-org/contents/_data/files.json", json=update_file)
    if not response.ok:
        print("unable to post new file")
        print(response.text)
        return
    pr_info = {
        "title": pr_title,
        "head": "adafruit-adabot:" + branch_name,
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
        boards[asset["name"].split("-")[2]]["download_count"] += asset["download_count"]


def print_active_user():
    response = github.get("/user")
    if response.ok:
        print("Logged in as {}".format(response.json()["login"]))
    else:
        print("Not logged in")

def generate_download_info():
    boards = {}
    errors = []

    new_tag = os.environ.get("TRAVIS_TAG", "4.0.0-alpha.3")

    changes = {
        "new_release": new_tag,
        "new_boards": [],
        "new_languages": []
    }

    print_active_user()

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

    print(previous_releases)
    for release in previous_releases:
        update_downloads(board_mapping, release)

    for port in SUPPORTED_PORTS:
        board_path = os.path.join("../ports", port, "boards")
        for board_path in os.scandir(board_path):
            if board_path.is_dir():
                board_files = os.listdir(board_path)
                board_id = board_path.name
                board_info = board_mapping[board_id]

                if board_id not in current_info:
                    changes["new_boards"].append(board_id)
                    current_info[board_id] = {"downloads": 0,
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
                        files.append("https://github.com/adafruit/circuitpython/releases/download/{tag}/adafruit-circuitpython-{board}-{language}-{tag}.{extension}".format(tag=new_tag, board=board_id, language=language, extension=extension))
                current_info[board_id]["downloads"] = board_info["download_count"]
                current_info[board_id]["versions"].append(new_version)

    changes["new_languages"] = set(languages) - previous_languages

    if changes["new_languages"]:
        create_pr(changes, current_info, git_info)

if __name__ == "__main__":
    if "TRAVIS_TAG" in os.environ:
        print("tag", os.environ["TRAVIS_TAG"])
        generate_download_info()
    else:
        print("skipping website update because this isn't a tag")
