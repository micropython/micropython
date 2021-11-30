import pathlib
import shlex
import subprocess
import sys
import time

# target will be a board, "test", "docs", "mpy-cross-mac", or "windows"

target = sys.argv[1]
ref = sys.argv[2]

print(target, ref)

# Submodules needed by port builds outside of their ports directory.
# Should we try and detect these?
port_deps = {
    "atmel-samd": [
        "extmod/ulab/",
        "lib/mp3/",
        "lib/protomatter/",
        "lib/quirc/",
        "lib/tinyusb/",
        "data/nvm.toml/",
    ],
    "broadcom": ["lib/tinyusb/"],
    "cxd56": ["extmod/ulab/", "lib/tinyusb/"],
    "espressif": ["extmod/ulab/", "lib/tinyusb/", "lib/protomatter/", "lib/quirc/"],
    "litex": ["extmod/ulab/", "lib/tinyusb/"],
    "mimxrt10xx": ["extmod/ulab/", "lib/tinyusb/", "data/nvm.toml/"],
    "nrf": ["extmod/ulab/", "lib/mp3/", "lib/protomatter/", "lib/tinyusb/", "data/nvm.toml/"],
    "raspberrypi": [
        "extmod/ulab/",
        "lib/mp3/",
        "lib/protomatter/",
        "lib/quirc/",
        "lib/tinyusb/",
        "data/nvm.toml/",
    ],
    "stm": ["extmod/ulab/", "lib/mp3/", "lib/protomatter/", "lib/tinyusb/", "data/nvm.toml/"]
    # omit unix which is part of the "test" target below
}


def run(title, command):
    print("::group::" + title, flush=True)
    print(command, flush=True)
    start = time.monotonic()
    subprocess.run(shlex.split(command), stderr=subprocess.STDOUT)
    print("Duration:", time.monotonic() - start, flush=True)
    print("::endgroup::", flush=True)


run(
    "Fetch back to the start of 2021 to get tag history",
    'git fetch --tags --recurse-submodules=no --shallow-since="2021-07-01" https://github.com/adafruit/circuitpython HEAD',
)
run(
    "Fetch back to the start of 2021 to get commit history",
    f'git fetch --recurse-submodules=no --shallow-since="2021-07-01" origin {ref}',
)
run("Init submodules", "git submodule init")
run("Submodule status", "git submodule status")

submodules = []

if target == "test":
    submodules = ["extmod/", "lib/", "tools/"]
elif target == "docs":
    submodules = ["extmod/ulab/"]
elif target == "mpy-cross-mac":
    submodules = ["tools/"]  # for huffman
elif target == "windows":
    # This builds one board from a number of ports so fill out a bunch of submodules
    submodules = ["extmod/", "lib/", "tools/", "ports/", "data/nvm.toml/"]
elif target == "website":
    # No submodules needed.
    pass
else:
    p = list(pathlib.Path(".").glob(f"ports/*/boards/{target}/mpconfigboard.mk"))
    if not p:
        raise RuntimeError(f"Unsupported target: {target}")

    config = p[0]
    # Add the ports folder to init submodules
    port_folder = config.parents[2]
    port = port_folder.name
    submodules.append(str(port_folder))
    submodules.append("tools/")  # for huffman
    submodules.extend(port_deps[port])
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

print(submodules)
if submodules:
    submodules = " ".join(submodules)
    # This line will fail because the submodule's need different commits than the tip of the branch. We
    # fix it later.
    run("Init the submodules we'll need", f"git submodule update --init -N --depth 1 {submodules}")

    run(
        "Fetch the submodule sha",
        "git submodule foreach 'git fetch --tags --depth 1 origin $sha1 && git checkout -q $sha1'",
    )
