#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2014 MicroPython & CircuitPython contributors (https://github.com/adafruit/circuitpython/graphs/contributors)
#
# SPDX-License-Identifier: MIT

import json
import os
import re
import requests
import subprocess
import sys
import sh
import base64
from io import StringIO
from datetime import date
from sh.contrib import git

sys.path.append("adabot")
import adabot.github_requests as github

sys.path.append("../docs")
from shared_bindings_matrix import (
    SUPPORTED_PORTS,
    support_matrix_by_board,
    get_board_mapping,
)

LANGUAGE_ALLOW_LIST = set(
    [
        "ID",
        "de_DE",
        "en_GB",
        "en_US",
        "en_x_pirate",
        "es",
        "fil",
        "fr",
        "it_IT",
        "ja",
        "nl",
        "pl",
        "pt_BR",
        "ru",
        "sv",
        "tr",
        "zh_Latn_pinyin",
    ]
)


def get_languages(list_all=False):
    languages = set()
    for f in os.scandir("../locale"):
        if f.name.endswith(".po"):
            languages.add(f.name[:-3])
    if not list_all:
        languages = languages & LANGUAGE_ALLOW_LIST
    return sorted(list(languages), key=str.casefold)


def get_version_info():
    version = None
    buffer = StringIO()
    git("rev-parse", "--short", "HEAD", _out=buffer)
    sha = buffer.getvalue().strip()
    try:
        buffer = StringIO()
        git("describe", "--tags", "--exact-match", _out=buffer)
        version = buffer.getvalue().strip()
    except sh.ErrorReturnCode_128:
        # No exact match
        pass

    if "GITHUB_SHA" in os.environ:
        sha = os.environ["GITHUB_SHA"]

    if not version:
        # Get branch we are PR'ing into, if any.
        # Works for pull_request actions.
        branch = os.environ.get("GITHUB_BASE_REF", "")
        if not branch:
            # Works for push actions (usually a PR merge).
            branch = os.environ.get("GITHUB_REF_NAME", "")
        if not branch:
            branch = "no-branch"
        # replace slashes with underscores to prevent path subdirs.
        branch = branch.strip().replace("/", "_")

        # Get PR number, if any
        # PR jobs put the PR number in PULL.
        pull_request = os.environ.get("PULL", "")
        if not pull_request:
            # PR merge jobs put a commit message that includes the PR number in HEAD_COMMIT_MESSAGE.
            head_commit_message = os.environ.get("HEAD_COMMIT_MESSAGE", "")
            if head_commit_message:
                match = re.match(r"Merge pull request #(\d+) from", head_commit_message)
                if match:
                    pull_request = match.group(1)

        if pull_request:
            pull_request = f"-PR{pull_request}"

        date_stamp = date.today().strftime("%Y%m%d")
        short_sha = sha[:7]
        # Example: 20231121-8.2.x-PR9876-123abcd
        version = f"{date_stamp}-{branch}{pull_request}-{short_sha}"

    return sha, version


def get_current_info():
    response = github.get("/repos/adafruit/circuitpython-org/git/refs/heads/main")
    if not response.ok:
        print(response.text)
        raise RuntimeError("cannot get main sha")
    commit_sha = response.json()["object"]["sha"]

    response = github.get(
        "/repos/adafruit/circuitpython-org/contents/_data/files.json?ref=" + commit_sha
    )
    if not response.ok:
        print(response.text)
        raise RuntimeError("cannot get previous files.json")

    response = response.json()

    git_info = commit_sha, response["sha"]

    if response["content"] != "":
        # if the file is there
        current_list = json.loads(base64.b64decode(response["content"]).decode("utf-8"))
    else:
        # if too big, the file is not included
        download_url = response["download_url"]
        response = requests.get(download_url)
        if not response.ok:
            print(response.text)
            raise RuntimeError("cannot get previous files.json")
        current_list = response.json()

    current_info = {}
    for info in current_list:
        current_info[info["id"]] = info
    return git_info, current_info


def create_json(updated):
    # Convert the dictionary to a list of boards. Liquid templates only handle arrays.
    updated_list = []
    all_ids = sorted(updated.keys())
    for id in all_ids:
        info = updated[id]
        info["id"] = id
        updated_list.append(info)
    return json.dumps(updated_list, sort_keys=True, indent=1).encode("utf-8") + b"\n"


def create_pr(changes, updated, git_info, user):
    commit_sha, original_blob_sha = git_info
    branch_name = "new_release_" + changes["new_release"]
    updated = create_json(updated)
    # print(updated.decode("utf-8"))

    pr_title = "Automated website update for release {}".format(changes["new_release"])
    boards = ""
    if changes["new_boards"]:
        boards = "New boards:\n* " + "\n* ".join(sorted(changes["new_boards"]))
    languages = ""
    if changes["new_languages"]:
        languages = "New languages:\n* " + "\n* ".join(sorted(changes["new_languages"]))
    message = "Automated website update for release {} by Blinka.\n\n{}\n\n{}\n".format(
        changes["new_release"], boards, languages
    )

    create_branch = {"ref": "refs/heads/" + branch_name, "sha": commit_sha}
    response = github.post("/repos/{}/circuitpython-org/git/refs".format(user), json=create_branch)
    if not response.ok and response.json()["message"] != "Reference already exists":
        print("unable to create branch")
        print(response.text)
        return

    update_file = {
        "message": message,
        "content": base64.b64encode(updated).decode("utf-8"),
        "sha": original_blob_sha,
        "branch": branch_name,
    }

    response = github.put(
        "/repos/{}/circuitpython-org/contents/_data/files.json".format(user), json=update_file
    )
    if not response.ok:
        print("unable to post new file")
        print(response.text)
        return
    pr_info = {
        "title": pr_title,
        "head": user + ":" + branch_name,
        "base": "main",
        "body": message,
        "maintainer_can_modify": True,
    }
    response = github.post("/repos/adafruit/circuitpython-org/pulls", json=pr_info)
    if not response.ok:
        print("unable to create pr")
        print(response.text)
        return
    print(changes)
    print(pr_info)


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

    changes = {"new_release": new_tag, "new_boards": [], "new_languages": []}

    user = print_active_user()

    sha, this_version = get_version_info()

    git_info, current_info = get_current_info()

    languages = get_languages()

    support_matrix = support_matrix_by_board(use_branded_name=False, withurl=False)

    new_stable = "-" not in new_tag

    previous_releases = set()
    previous_languages = set()

    # Delete the release we are replacing
    for board in current_info:
        info = current_info[board]
        for version in list(info["versions"]):
            previous_releases.add(version["version"])
            previous_languages.update(version["languages"])
            if version["stable"] == new_stable or (
                new_stable and version["version"].startswith(this_version)
            ):
                info["versions"].remove(version)

    board_mapping = get_board_mapping()

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
                        current_info[alias] = {"downloads": 0, "versions": []}
                    new_version = {
                        "stable": new_stable,
                        "version": new_tag,
                        "languages": languages,
                        # add modules, extensions, frozen_libraries explicitly
                        "modules": support_matrix[alias]["modules"],
                        "extensions": support_matrix[alias]["extensions"],
                        "frozen_libraries": support_matrix[alias]["frozen_libraries"],
                    }
                    current_info[alias]["downloads"] = alias_info["download_count"]
                    current_info[alias]["versions"].append(new_version)

    changes["new_languages"] = set(languages) - previous_languages

    if changes["new_release"] and user:
        create_pr(changes, current_info, git_info, user)
    else:
        if "DEBUG" in os.environ:
            print(create_json(current_info).decode("utf8"))
        else:
            print("No new release to update")


if __name__ == "__main__":
    if "RELEASE_TAG" in os.environ and os.environ["RELEASE_TAG"]:
        generate_download_info()
    else:
        print("skipping website update because this isn't a tag")
