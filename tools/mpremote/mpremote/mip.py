# Micropython package installer
# Ported from micropython-lib/micropython/mip/mip.py.
# MIT license; Copyright (c) 2022 Jim Mussared

import urllib.error
import urllib.request
import json
import tempfile
import os
import os.path

from .commands import CommandError, show_progress_bar


_PACKAGE_INDEX = "https://micropython.org/pi/v2"

allowed_mip_url_prefixes = ("http://", "https://", "github:", "gitlab:")


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
    if url.startswith(allowed_mip_url_prefixes):
        try:
            with urllib.request.urlopen(url) as src:
                data = src.read()
        except urllib.error.HTTPError as e:
            if e.status == 404:
                raise CommandError(f"File not found: {url}")
            else:
                raise CommandError(f"Error {e.status} requesting {url}")
        except urllib.error.URLError as e:
            raise CommandError(f"{e.reason} requesting {url}")
    else:
        if "\\" in url:
            raise CommandError(f'Use "/" instead of "\\" in file URLs: {url!r}\n')
        try:
            with open(url, "rb") as f:
                data = f.read()
        except OSError as e:
            raise CommandError(f"{e.strerror} opening {url}")

    print("Installing:", dest)
    _ensure_path_exists(transport, dest)
    transport.fs_writefile(dest, data, progress_callback=show_progress_bar)


def _install_json(transport, package_json_url, index, target, version, mpy):
    base_url = ""
    if package_json_url.startswith(allowed_mip_url_prefixes):
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
        base_url = package_json_url.rpartition("/")[0]
    elif package_json_url.endswith(".json"):
        try:
            with open(package_json_url, "r") as f:
                package_json = json.load(f)
        except OSError:
            raise CommandError(f"Error opening {package_json_url}")
        base_url = os.path.dirname(package_json_url)
    else:
        raise CommandError(f"Invalid url for package: {package_json_url}")
    for target_path, short_hash in package_json.get("hashes", ()):
        fs_target_path = target + "/" + target_path
        file_url = f"{index}/file/{short_hash[:2]}/{short_hash}"
        _download_file(transport, file_url, fs_target_path)
    for target_path, url in package_json.get("urls", ()):
        fs_target_path = target + "/" + target_path
        if base_url and not url.startswith(allowed_mip_url_prefixes):
            url = f"{base_url}/{url}"  # Relative URLs
        _download_file(transport, _rewrite_url(url, version), fs_target_path)
    for dep, dep_version in package_json.get("deps", ()):
        _install_package(transport, dep, index, target, dep_version, mpy)


def _install_package(transport, package, index, target, version, mpy):
    if package.startswith(allowed_mip_url_prefixes):
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
    elif package.endswith(".json"):
        pass
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
