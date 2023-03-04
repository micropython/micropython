import os
import sys
import time
import shlex
import pathlib
import subprocess

# Target will be a board, "test", "docs", "mpy-cross-mac", or "windows"
TARGET = sys.argv[1]

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
        "data/nvm.toml/",
    ],
    "broadcom": ["extmod/ulab/", "lib/tinyusb/"],
    "cxd56": ["extmod/ulab/", "lib/tinyusb/"],
    "espressif": [
        "extmod/ulab/",
        "lib/certificates/nina-fw/",
        "lib/protomatter/",
        "lib/quirc/",
        "lib/tinyusb/",
    ],
    "litex": ["extmod/ulab/", "lib/tinyusb/"],
    "mimxrt10xx": ["extmod/ulab/", "lib/tinyusb/", "data/nvm.toml/"],
    "nrf": ["extmod/ulab/", "lib/mp3/", "lib/protomatter/", "lib/tinyusb/", "data/nvm.toml/"],
    "raspberrypi": [
        "extmod/ulab/",
        "lib/adafruit_floppy/",
        "lib/mbedtls/",
        "lib/mp3/",
        "lib/certificates/nina-fw/",
        "lib/protomatter/",
        "lib/quirc/",
        "lib/tinyusb/",
        "data/nvm.toml/",
    ],
    "stm": ["extmod/ulab/", "lib/mp3/", "lib/protomatter/", "lib/tinyusb/", "data/nvm.toml/"]
    # omit unix which is part of the "test" target below
}


def run(title, command, check=True):
    print("::group::" + title, flush=True)
    print(command, flush=True)
    start = time.monotonic()
    try:
        subprocess.run(shlex.split(command), stderr=subprocess.STDOUT, check=check)
    finally:
        print("::endgroup::", flush=True)
        print("Duration:", time.monotonic() - start, flush=True)


def set_output(name, value):
    if "GITHUB_OUTPUT" in os.environ:
        with open(os.environ["GITHUB_OUTPUT"], "at") as f:
            print(f"{name}={value}", file=f)
    else:
        print(f"Would set GitHub actions output {name} to '{value}'")


def main():
    submodules = []
    submodules_tags = []

    print("Target:", TARGET)

    if TARGET == "scheduler":
        # submodules = ["tools/"]
        submodules = ["extmod/ulab", "lib/", "tools/"]
    elif TARGET == "tests":
        submodules = ["extmod/ulab", "lib/", "tools/"]
    elif TARGET == "build-doc":
        # used in .readthedocs.yml to generate RTD
        submodules = ["extmod/ulab"]
        submodules_tags = ["frozen/"]
    elif TARGET == "mpy-cross" or TARGET == "mpy-cross-mac":
        submodules = ["tools/"]  # for huffman
    elif TARGET == "build-windows":
        # This builds one board from a number of ports so fill out a bunch of submodules
        submodules = ["extmod/ulab", "lib/", "tools/", "ports/", "data/nvm.toml"]
    elif TARGET == "website":
        submodules = ["tools/adabot/"]
        submodules_tags = ["frozen/"]
    elif TARGET == "pre-commit":
        submodules = ["extmod/ulab"]
    else:
        p = list(pathlib.Path(".").glob(f"ports/*/boards/{TARGET}/mpconfigboard.mk"))
        if not p:
            raise RuntimeError(f"Unsupported target: {TARGET}")

        config = p[0]
        # Add the ports folder to init submodules
        port_folder = config.parents[2]
        port = port_folder.name
        submodules.append(str(port_folder))
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
                    submodules_tags.append(lib_folder)

    print("Submodule tags[Y]:", submodules_tags)
    print("Submodule tags[N]:", submodules)

    if submodules_tags:
        run(
            "Init the submodules with tags",
            f"git submodule update --init {' '.join(submodules_tags)}",
        )

    if submodules:
        run(
            "Init the submodules without tags",
            f"git submodule update --init --depth=1 {' '.join(submodules)}",
        )

    for submodule in submodules_tags:
        if submodule.startswith("frozen"):
            set_output("frozen_tags", True)
            break
    else:
        set_output("frozen_tags", False)


if __name__ == "__main__":
    main()
