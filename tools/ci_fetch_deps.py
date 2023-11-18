import os
import sys
import time
import shlex
import pathlib
import re
import subprocess

TOP = pathlib.Path(__file__).parent.parent


def _git_version():
    version_str = subprocess.check_output(["git", "--version"], encoding="ascii", errors="replace")
    version_str = re.search("([0-9]\.*)*[0-9]", version_str).group(0)
    return tuple(int(part) for part in version_str.split("."))


clone_supports_filter = (
    False if "NO_USE_CLONE_FILTER" in os.environ else _git_version() >= (2, 36, 0)
)

if clone_supports_filter:
    filter_maybe = "--filter=blob:none"
else:
    filter_maybe = ""


def _all_submodules():
    submodule_str = subprocess.check_output(
        ["git", "submodule", "status"], encoding="ascii", errors="replace", cwd=TOP
    )
    return [row.split()[1] for row in submodule_str.strip().split("\n")]


all_submodules = _all_submodules()


def matching_submodules(s):
    if s.endswith("/"):
        return [m for m in all_submodules if m.startswith(s)]
    elif s not in all_submodules:
        raise ValueError(f"{s!r} is not a submodule")
    return [s]


# Submodules needed by port builds outside of their ports directory.
# Should we try and detect these?
PORT_DEPS = {
    "atmel-samd": [
        "extmod/ulab/",
        "lib/adafruit_floppy/",
        "lib/mp3/",
        "lib/protomatter/",
        "lib/quirc/",
        "lib/tinyusb/",
        "lib/tlsf",
        "data/nvm.toml/",
    ],
    "broadcom": ["extmod/ulab/", "lib/tlsf", "lib/tinyusb/"],
    "cxd56": ["extmod/ulab/", "lib/tlsf", "lib/tinyusb/"],
    "espressif": [
        "extmod/ulab/",
        "lib/certificates/",
        "lib/protomatter/",
        "lib/quirc/",
        "lib/tlsf",
        "lib/tinyusb/",
    ],
    "litex": ["extmod/ulab/", "lib/tinyusb/", "lib/tlsf"],
    "mimxrt10xx": ["extmod/ulab/", "lib/tinyusb/", "lib/tlsf", "data/nvm.toml/"],
    "nrf": [
        "extmod/ulab/",
        "lib/mp3/",
        "lib/protomatter/",
        "lib/tinyusb/",
        "lib/tlsf",
        "data/nvm.toml/",
    ],
    "raspberrypi": [
        "extmod/ulab/",
        "lib/adafruit_floppy/",
        "lib/mbedtls/",
        "lib/mp3/",
        "lib/certificates/",
        "lib/protomatter/",
        "lib/quirc/",
        "lib/tinyusb/",
        "lib/tlsf",
        "data/nvm.toml/",
    ],
    "silabs": ["extmod/ulab/", "data/nvm.toml/", "lib/tlsf"],
    "stm": [
        "extmod/ulab/",
        "lib/mp3/",
        "lib/protomatter/",
        "lib/tinyusb/",
        "lib/tlsf",
        "data/nvm.toml/",
    ]
    # omit unix which is part of the "test" target below
}


def run(title, command, cwd):
    print("::group::" + title, flush=True)
    print(f"{command} (in {cwd})", flush=True)
    start = time.monotonic()
    try:
        subprocess.run(shlex.split(command), stderr=subprocess.STDOUT, check=True, cwd=cwd)
    finally:
        print("::endgroup::", flush=True)
        print("Duration:", time.monotonic() - start, flush=True)


def matching_submodules(where):
    for m in all_submodules:
        if m in where:
            yield m
        for w in where:
            if m.startswith(f"{w}/"):
                yield m
                break


def fetch(where):
    if clone_supports_filter:
        run(
            "Init submodules (using filter)",
            f"git submodule update --init {filter_maybe} {' '.join(where)}",
            cwd=TOP,
        )
    else:
        run(
            "Init submodules (using depth)",
            f"git submodule update --init --depth 1 {' '.join(where)}",
            cwd=TOP,
        )
        for s in matching_submodules([w for w in where if w.startswith("frozen")]):
            run(f"Ensure tags exist in {s}", "git fetch --tags --depth 1", cwd=TOP / s)


def set_output(name, value):
    if "GITHUB_OUTPUT" in os.environ:
        with open(os.environ["GITHUB_OUTPUT"], "at") as f:
            print(f"{name}={value}", file=f)
    else:
        print(f"{name}: {value!r}")


SUBMODULES_BY_TARGET = {}


def main(target):
    submodules = []

    print("Target:", target)

    if target == "all":
        submodules = [".", "frozen"]  # explicitly list frozen to get tags
    elif target == "scheduler":
        submodules = ["extmod/ulab", "lib/", "tools/"]
    elif target == "tests":
        submodules = [
            "extmod/ulab",
            "lib/",
            "tools/",
            "frozen/Adafruit_CircuitPython_asyncio",
            "frozen/Adafruit_CircuitPython_Ticks",
        ]
    elif target == "docs":
        # used in .readthedocs.yml to generate RTD
        submodules = ["extmod/ulab", "frozen"]
    elif target == "mpy-cross" or target == "mpy-cross-mac":
        submodules = ["tools/"]  # for huffman
    elif target == "windows":
        # This builds one board from a number of ports so fill out a bunch of submodules
        for port in ("atmel-samd", "nrf", "raspberrypi", "stm"):
            submodules.append(f"ports/{port}")
            submodules.extend(PORT_DEPS[port])
        unique_submodules = set(submodules)
        submodules = list(unique_submodules)
    elif target == "website":
        submodules = ["tools/adabot", "frozen"]
    elif target == "pre-commit":
        submodules = ["extmod/ulab"]
    elif target in PORT_DEPS:
        submodules = ["data", "extmod", "lib", "tools", "frozen", f"ports/{target}"] + PORT_DEPS[
            target
        ]
    else:
        p = list(pathlib.Path(TOP).glob(f"ports/*/boards/{target}/mpconfigboard.mk"))
        if not p:
            raise RuntimeError(f"Unsupported target: {target}")

        config = p[0]
        # Add the ports folder to init submodules
        port_folder = config.parents[2]
        port = port_folder.name
        submodules.append(f"ports/{port}")
        submodules.append("tools/")  # for huffman
        submodules.extend(PORT_DEPS[port])
        with config.open() as f:
            for line in f.readlines():
                prefix = "FROZEN_MPY_DIRS += $(TOP)/"
                if line.startswith(prefix):
                    lib_folder = line.strip()[len(prefix) :]
                    # Drop everything after the second folder because the frozen
                    # folder may be inside the submodule.
                    if lib_folder.count("/") > 1:
                        lib_folder = lib_folder.split("/", maxsplit=2)
                        lib_folder = "/".join(lib_folder[:2])
                    submodules.append(lib_folder)

    print("Submodules:", " ".join(submodules))

    if submodules:
        fetch(submodules)

    for submodule in submodules:
        if submodule.startswith("frozen"):
            set_output("frozen_tags", True)
            break
    else:
        set_output("frozen_tags", False)


if __name__ == "__main__":
    if len(sys.argv) < 2:
        raise SystemExit("Usage: ci_fetch_deps dep...")

    run("Sync submodule URLs", "git submodule sync --quiet", cwd=TOP)

    # Target will be a board, "test", "docs", "mpy-cross-mac", or "windows"
    for target in sys.argv[1:]:
        main(target)
