# Micropython package installer
# Ported from micropython-lib/micropython/mip/mip.py.
# MIT license; Copyright (c) 2022 Jim Mussared

import urllib.error
import urllib.request
import json
import tempfile
import os

from .commands import CommandError, show_progress_bar


_PACKAGE_INDEX = "https://micropython.org/pi/v2"


# This implements os.makedirs(os.dirname(path))
def _ensure_path_exists(transport, path):
    split = path.split("/")

    # Handle paths starting with "/".
    if not split[0]:
        split.pop(0)
        split[0] = "/" + split[0]

    prefix = ""
    for i in range(len(split) - 1):
        prefix += split[i]
        if not transport.fs_exists(prefix):
            transport.fs_mkdir(prefix)
        prefix += "/"


def _rewrite_url(url, branch=None):
    if not branch:
        branch = "HEAD"
    if url.startswith("github:"):
        url = url[7:].split("/")
        url = (
            "https://raw.githubusercontent.com/"
            + url[0]
            + "/"
            + url[1]
            + "/"
            + branch
            + "/"
            + "/".join(url[2:])
        )
    elif url.startswith("gitlab:"):
        url = url[7:].split("/")
        url = (
            "https://gitlab.com/"
            + url[0]
            + "/"
            + url[1]
            + "/-/raw/"
            + branch
            + "/"
            + "/".join(url[2:])
        )
    return url


def _download_file(transport, url, dest):
    try:
        with urllib.request.urlopen(url) as src:
            data = src.read()
            print("Installing:", dest)
            _ensure_path_exists(transport, dest)
            transport.fs_writefile(dest, data, progress_callback=show_progress_bar)
    except urllib.error.HTTPError as e:
        if e.status == 404:
            raise CommandError(f"File not found: {url}")
        else:
            raise CommandError(f"Error {e.status} requesting {url}")
    except urllib.error.URLError as e:
        raise CommandError(f"{e.reason} requesting {url}")


def _install_json(transport, package_json_url, index, target, version, mpy):
    try:
        with urllib.request.urlopen(_rewrite_url(package_json_url, version)) as response:
            package_json = json.load(response)
    except urllib.error.HTTPError as e:
        if e.status == 404:
            raise CommandError(f"Package not found: {package_json_url}")
        else:
            raise CommandError(f"Error {e.status} requesting {package_json_url}")
    except urllib.error.URLError as e:
        raise CommandError(f"{e.reason} requesting {package_json_url}")
    for target_path, short_hash in package_json.get("hashes", ()):
        fs_target_path = target + "/" + target_path
        file_url = f"{index}/file/{short_hash[:2]}/{short_hash}"
        _download_file(transport, file_url, fs_target_path)
    for target_path, url in package_json.get("urls", ()):
        fs_target_path = target + "/" + target_path
        _download_file(transport, _rewrite_url(url, version), fs_target_path)
    for dep, dep_version in package_json.get("deps", ()):
        _install_package(transport, dep, index, target, dep_version, mpy)


def _install_package(transport, package, index, target, version, mpy):
    if (
        package.startswith("http://")
        or package.startswith("https://")
        or package.startswith("github:")
        or package.startswith("gitlab:")
    ):
        if package.endswith(".py") or package.endswith(".mpy"):
            print(f"Downloading {package} to {target}")
            _download_file(
                transport, _rewrite_url(package, version), target + "/" + package.rsplit("/")[-1]
            )
            return
        else:
            if not package.endswith(".json"):
                if not package.endswith("/"):
                    package += "/"
                package += "package.json"
            print(f"Installing {package} to {target}")
    else:
        if not version:
            version = "latest"
        print(f"Installing {package} ({version}) from {index} to {target}")

        mpy_version = "py"
        if mpy:
            transport.exec("import sys")
            mpy_version = transport.eval("getattr(sys.implementation, '_mpy', 0) & 0xFF") or "py"

        package = f"{index}/package/{mpy_version}/{package}/{version}.json"

    _install_json(transport, package, index, target, version, mpy)


def do_mip(state, args):
    state.did_action()

    if args.command[0] == "install":
        state.ensure_raw_repl()

        for package in args.packages:
            version = None
            if "@" in package:
                package, version = package.split("@")

            print("Install", package)

            if args.index is None:
                args.index = _PACKAGE_INDEX

            if args.target is None:
                state.transport.exec("import sys")
                lib_paths = [p for p in state.transport.eval("sys.path") if p.endswith("/lib")]
                if lib_paths and lib_paths[0]:
                    args.target = lib_paths[0]
                else:
                    raise CommandError(
                        "Unable to find lib dir in sys.path, use --target to override"
                    )

            if args.mpy is None:
                args.mpy = True

            try:
                _install_package(
                    state.transport,
                    package,
                    args.index.rstrip("/"),
                    args.target,
                    version,
                    args.mpy,
                )
            except CommandError:
                print("Package may be partially installed")
                raise
            print("Done")
    else:
        raise CommandError(f"mip: '{args.command[0]}' is not a command")
