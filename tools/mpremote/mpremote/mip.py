# MicroPython package installer
# Ported from micropython-lib/micropython/mip/mip.py.
# MIT license; Copyright (c) 2022 Jim Mussared

import urllib.error
import urllib.request
import json
import re
import hashlib
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


# Check if the specified path exists and matches the hash.
def _check_exists(transport, path, short_hash):
    try:
        remote_hash = transport.fs_hashfile(path, "sha256")
    except FileNotFoundError:
        return False
    return remote_hash.hex()[: len(short_hash)] == short_hash


def _rewrite_url(url, branch=None):
    if not branch or branch == "latest":
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


def _download_file(transport, url, dest, package_info, target):
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
    assert dest.startswith(target)
    relative_dest = dest[len(target) :].lstrip("/")
    if "files" not in package_info:
        # this step is postponed in order to get "files" as last entry of the package info
        package_info["files"] = []
    package_info["files"].append(
        {"path": relative_dest, "sha256": hashlib.sha256(data).hexdigest()}
    )


def _install_json(
    transport, package_json_url, index, target, version, mpy, package_info, target_info
):
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

    if package_json_url.startswith(index):
        package_info["index"] = index
        reported_version = package_json.get("version")
        # Note that the reliability of the reported version is guaranteed only in case of index packages.
        # Even if a GitHub repo has corresponding tag, its other commits (between releases)
        # may also contain the same reported version.
        if reported_version is not None:
            package_info["resolved_version"] = reported_version
    package_info["metadata"] = package_json

    for target_path, short_hash in package_json.get("hashes", ()):
        fs_target_path = target + "/" + target_path
        if _check_exists(transport, fs_target_path, short_hash):
            print("Exists:", fs_target_path)
        else:
            file_url = f"{index}/file/{short_hash[:2]}/{short_hash}"
            _download_file(transport, file_url, fs_target_path, package_info, target)
    for target_path, url in package_json.get("urls", ()):
        fs_target_path = target + "/" + target_path
        if base_url and not url.startswith(allowed_mip_url_prefixes):
            url = f"{base_url}/{url}"  # Relative URLs
        _download_file(transport, _rewrite_url(url, version), fs_target_path, package_info, target)
    for dep, dep_version in package_json.get("deps", ()):
        _install_package(transport, dep, index, target, dep_version, mpy, target_info)

    if "files" not in package_info:
        # make sure "files" is always present
        package_info["files"] = []


def _install_package(transport, package, index, target, version, mpy, target_info):
    installed_version = _get_installed_version(package, target_info)
    if installed_version is not None:
        if _installed_version_matches_requirement(installed_version, version):
            print(f"Version {installed_version} of {package} is already installed")
            return
        else:
            _uninstall_package(transport, package, target, target_info)

    if version is None:
        version = "latest"

    package_info = {
        "requested_version": version,
    }
    package = _normalize_package_specifier(package)
    target_info["packages"][package] = package_info

    if package.startswith(allowed_mip_url_prefixes):
        if package.startswith("github:") or package.startswith("gitlab:"):
            resolved_version = _try_resolve_version_as_git_reference(package, version)
            if resolved_version is not None:
                package_info["resolved_version"] = resolved_version
                version = resolved_version
        if package.endswith(".py") or package.endswith(".mpy"):
            print(f"Downloading {package} to {target}")
            fs_target_path = target + "/" + package.rsplit("/")[-1]
            _download_file(
                transport,
                _rewrite_url(package, version),
                fs_target_path,
                package_info,
                target,
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
        print(f"Installing {package} ({version}) from {index} to {target}")

        mpy_version = "py"
        if mpy:
            transport.exec("import sys")
            mpy_version = transport.eval("getattr(sys.implementation, '_mpy', 0) & 0xFF") or "py"

        package = f"{index}/package/{mpy_version}/{package}/{version}.json"

    _install_json(transport, package, index, target, version, mpy, package_info, target_info)


def _list_packages(args, target_info):
    for key, entry in target_info["packages"].items():
        print(key, end="")

        version = entry.get("resolved_version", entry.get("requested_version")) or "latest"
        if version != "latest":
            print(f"@{version}", end="")

        index = entry.get("index")
        if index is not None and index != _PACKAGE_INDEX:
            print(f" from {index}")

        print()


def _uninstall_package(transport, package, target, target_info):
    target = target.rstrip("/")
    dirs_to_check = []
    package = _normalize_package_specifier(package)
    if package not in target_info["packages"]:
        raise CommandError(f"mip: package '{package}' not found")

    package_info = target_info["packages"][package]

    print(f"Uninstalling {package} from {target}")
    for file in package_info["files"]:
        full_path = target + "/" + file["path"]
        print("Uninstalling:", full_path)
        if transport.fs_exists(full_path):
            transport.fs_rmfile(full_path)
            parent_dir = full_path.rsplit("/", maxsplit=1)[0]
            if parent_dir not in dirs_to_check:
                dirs_to_check.append(parent_dir)

    # remove directories, which became empty because of this uninstall (except target)
    while dirs_to_check:
        dir_to_check = dirs_to_check.pop(0)
        if dir_to_check != target and not transport.fs_listdir(dir_to_check):
            print("Removing empty directory:", dir_to_check)
            transport.fs_rmdir(dir_to_check)
            parent_dir = dir_to_check.rsplit("/", maxsplit=1)[0]
            if parent_dir not in dirs_to_check and parent_dir != target:
                dirs_to_check.append(parent_dir)

    del target_info["packages"][package]


def _get_installed_version(package, target_info):
    spec = target_info["packages"].get(_normalize_package_specifier(package), None)
    if spec is not None:
        return spec.get("resolved_version", spec["requested_version"])

    return None


def _installed_version_matches_requirement(installed_version, requirement):
    if requirement == "latest":
        # explicit "latest" always requires re-install
        return False

    return requirement is None or installed_version == requirement


def _get_target_info_path(args):
    return args.target.rstrip("/") + "/mip-packages.json"


def _normalize_package_specifier(package):
    if package.endswith(".json") and os.path.isfile(package):
        return os.path.normpath(os.path.abspath(package))

    package_json_url_suffix = "/package.json"
    if package.startswith(allowed_mip_url_prefixes) and package.endswith(package_json_url_suffix):
        return package[: -len(package_json_url_suffix)]

    # TODO: What to do with trailing slashes?g

    if ":" not in package:
        # index package
        # TODO: is this right?
        return re.sub(r"[-_.]+", "-", package.lower()).strip("-")

    return package


def _load_target_info(state, args):
    path = _get_target_info_path(args)
    if state.transport.fs_exists(path):
        return json.loads(state.transport.fs_readfile(path).decode("utf-8"))
    else:
        return {"packages": {}}


def _save_target_info(state, args, target_info, pretty=False):
    if pretty:
        json_str = json.dumps(target_info, indent=2)
    else:
        json_str = json.dumps(target_info)

    state.transport.fs_writefile(_get_target_info_path(args), json_str.encode("utf-8"))


def _try_resolve_version_as_git_reference(package, version):
    if version in ["latest", None]:
        git_ref = "HEAD"
    else:
        git_ref = version

    if package.startswith("github:"):
        host = "https://github.com"
        org, project = package[7:].split("/")[:2]
    elif package.startswith("gitlab:"):
        host = "https://gitlab.com"
        org, project = package[7:].split("/")[:2]
    else:
        raise ValueError("Unexpected package reference: " + package)

    repo_url = f"{host}/{org}/{project}.git"

    tags, heads = _fetch_git_refs(repo_url)
    if git_ref in tags:
        return git_ref

    if git_ref in heads:
        return heads[git_ref]

    if re.fullmatch("[0-9a-fA-F]{7,40}", git_ref):
        # looks like commit hash
        return git_ref

    return heads.get(git_ref, None)


def _fetch_git_refs(repo_url):
    """
    Returns two dictionaries mapping tags to commit hashes and branches (including pseudo-branch HEAD) to commit hashes
    """
    assert repo_url.endswith(".git")

    req = urllib.request.Request(
        repo_url + "/info/refs?service=git-upload-pack",
        headers={"User-Agent": "python-ref-resolver/0.2"},
    )
    data = urllib.request.urlopen(req, timeout=15).read()

    def pkt_lines(raw: bytes):
        i = 0
        while i < len(raw):
            n = int(raw[i : i + 4], 16)
            i += 4
            if n == 0:  # flush
                continue
            yield raw[i : i + n - 4].rstrip(b"\r\n")
            i += n - 4

    tags = {}
    heads = {}

    for pl in pkt_lines(data):
        if pl.startswith(b"#"):  # “# service=…”
            continue

        sha, rest = pl.split(b" ", 1)
        name, *cap = rest.split(b"\0", 1)
        name = name.decode()
        sha = sha.decode()

        if name.endswith("^{}"):  # peeled helper line
            continue
        elif name == "HEAD":
            heads[name] = sha
        elif name.startswith("refs/tags/"):
            tags[name[10:]] = sha
        elif name.startswith("refs/heads/"):
            heads[name[11:]] = sha

    return tags, heads


def do_mip(state, args):
    state.did_action()

    if args.command[0] in ["install", "list", "uninstall"]:
        state.ensure_raw_repl()

        if args.target is None:
            state.transport.exec("import sys")
            lib_paths = [
                p
                for p in state.transport.eval("sys.path")
                if not p.startswith("/rom") and p.endswith("/lib")
            ]
            if lib_paths and lib_paths[0]:
                args.target = lib_paths[0]
            else:
                raise CommandError("Unable to find lib dir in sys.path, use --target to override")

        target_info = _load_target_info(state, args)
    else:
        raise CommandError(f"mip: '{args.command[0]}' is not a command")

    if args.command[0] == "install":
        if not args.packages:
            raise CommandError("mip: install requires one or more package arguments")

        for package in args.packages:
            version = None
            if "@" in package:
                package, version = package.split("@")

            print("Install", package)

            if args.index is None:
                args.index = _PACKAGE_INDEX

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
                    target_info,
                )
            except CommandError:
                print("Package may be partially installed")
                raise
            print("Done")

        _save_target_info(state, args, target_info)

    elif args.command[0] == "list":
        if args.packages:
            raise CommandError("mip: list does not take package arguments")
        _list_packages(args, target_info)

    elif args.command[0] == "uninstall":
        if not args.packages:
            raise CommandError("mip: uninstall requires one or more package arguments")

        for package in args.packages:
            print("Uninstall", package)
            _uninstall_package(state.transport, package, args.target, target_info)
        if target_info["packages"] == {}:
            target_info_path = _get_target_info_path(args)
            if state.transport.fs_exists(target_info_path):
                print("Removing empty", target_info_path)
                state.transport.fs_rmfile(target_info_path)
        else:
            _save_target_info(state, args, target_info)
