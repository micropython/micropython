"""
This is a helper script for merging in new versions of MicroPython. You *must*
evaluate its correctness and adapt it for each MP version. This is committed
in the repo more for reference than "fire and forget" use.

I have found I have to run each piece separately, because there are some errors.
For instance, there are file renames in the porcelain output that are not handled.
I add a sys.exit(0) after a section, and once a section runs, I delete it temporarily
and move on to the next section. -- dhalbert

Updated for v1.21.0 merge - dhalbert

"""

from io import StringIO

import sh
from sh import git

out_buf = StringIO()

ports_to_delete = [
    "bare-arm",
    "cc3200",
    "embed",
    "esp32",
    "esp8266",
    "mimxrt",
    "minimal",
    "pic16bit",
    "powerpc",
    "qemu-arm",
    "raspberrypi",
    "renesas-ra",
    "rp2",
    "samd",
    "stm32",
    "teensy",
    "webassembly",
    "windows",
    "zephyr",
]
for p in ports_to_delete:
    try:
        git.rm("-rf", "ports/" + p)
    except sh.ErrorReturnCode_128:
        pass

# We inherit stm32 changes into stm because we did a git rename.
git.status("--porcelain=1", "ports/stm", _out=out_buf)
out_buf.seek(0)
line = out_buf.readline()
while line:
    state, path = line.split()
    if state == "UU":
        git.checkout("--ours", path)
        git.add(path)
    elif state == "UA":
        git.rm(path)
    line = out_buf.readline()

# MicroPython added their nrf code in ports/nrf too. So, we always take our version.
out_buf = StringIO()
git.status("--porcelain=1", "ports/nrf", _out=out_buf)
out_buf.seek(0)
line = out_buf.readline()
while line:
    state, path = line.split()
    if state == "UU":
        git.checkout("--ours", path)
        git.add(path)
    elif state == "UA":
        git.rm(path)
    elif state == "AA":
        git.rm("-f", path)
    elif state == "A":
        git.rm("-f", path)
    elif state == "DU":
        git.rm(path)
    elif state == "DD":
        git.rm(path)
    else:
        print(state, path)
    line = out_buf.readline()


# MicroPython has their own CI settings. Let's not use them now.
out_buf = StringIO()
git.status("--porcelain=1", ".github/workflows", _out=out_buf)
out_buf.seek(0)
line = out_buf.readline()
while line:
    state, path = line.split()
    if state == "A":
        git.rm("-f", path)
    else:
        print(state, path)
    line = out_buf.readline()

# Delete docs and tests for things we don't need anymore
docs_to_delete = [
    "conf.py",
    "develop",
    "differences",
    "esp32",
    "esp8266",
    "library/bluetooth.rst",
    "library/btree.rst",
    "library/cryptolib.rst",
    "library/esp*.rst",
    "library/framebuf.rst",
    "library/hashlib.rst",
    "library/lcd160cr.rst",
    "library/machine*.rst",
    "library/math.rst",
    "library/network*.rst",
    "library/os.rst",
    "library/pyb*.rst",
    "library/random.rst",
    "library/rp2*.rst",
    "library/uos.rst",
    "library/socket.rst",
    "library/ssl.rst",
    "library/stm.rst",
    "library/struct.rst",
    "library/_thread.rst",
    "library/time.rst",
    "library/uasyncio.rst",
    "library/uctypes.rst",
    "library/wipy.rst",
    "library/wm8960.rst",
    "library/zephyr*.rst",
    "library/zlib.rst",
    "make.bat",
    "mimxrt",
    "pyboard",
    "reference",
    "renesas-ra",
    "rp2",
    "samd",
    "templates/topindex.html",
    "wipy",
    "zephyr",
]
for d in docs_to_delete:
    try:
        git.rm("-rf", "docs/" + d)
    except sh.ErrorReturnCode_128:
        pass

tests_to_delete = [
    "esp32",
    "multi_bluetooth",
    "multi_espnow",
    "multi_net",
    "net_hosted",
    "net_inet",
    "pyb",
    "wipy",
]
for t in tests_to_delete:
    try:
        git.rm("-rf", "tests/" + t)
    except sh.ErrorReturnCode_128:
        pass


libs_to_delete = [
    "asf4",
    "btstack",
    "libhydrogen",
    "lwip",
    "micropython-lib",
    "mynewt-nimble",
    "nrfx",
    "nxp_driver",
    "pico-sdk",
    "protobuf-c",
    "stm32lib",
    "wiznet5k",
]
for l in libs_to_delete:
    try:
        git.rm("-rf", "lib/" + l)
    except sh.ErrorReturnCode_128:
        pass

extmod_to_delete = [
    "btstack",
    "extmod.cmake",
    "machine_*",
    "mbedtls",
    "modbluetooth.*",
    "modbtree.*",
    "modframebuf.*",
    "modlwip.*",
    "modnetwork.*",
    "modonewire.*",
    "moducryptolib.*",
    "modsocket.*",
    "modssl_*.*",
    "modtimeq.*",
    "modwebsocket.*",
    "modwebrepl.*",
    "mpbthci.*",
    "network_*.*",
    "nimble",
]
for e in extmod_to_delete:
    try:
        git.rm("-rf", "extmod/" + e)
    except sh.ErrorReturnCode_128 as error:
        print(error)

top_delete = [
    "drivers",
    "README.md",
    "CODEOFCONDUCT.md",
    "CODECONVENTIONS.md",
]
for t in top_delete:
    try:
        git.rm("-rf", t)
    except sh.ErrorReturnCode_128:
        pass

dot_github_delete = [
    ".github/dependabot.yml",
    ".github/FUNDING.yml",
    ".github/ISSUE_TEMPLATE/documentation.md",
    ".github/ISSUE_TEMPLATE/security.md",
    ".github/workflows/code_formatting.yml",
    ".github/workflows/code_size_comment.yml",
    ".github/workflows/code_size.yml",
    ".github/workflows/commit_formatting.yml",
    ".github/workflows/docs.yml",
    ".github/workflows/examples.yml",
    ".github/workflows/mpremote.yml",
    ".github/workflows/mpy_format.yml",
    ".github/workflows/mpy_format.yml",
    ".github/workflows/ports_*.yml",
]
for t in dot_github_delete:
    try:
        git.rm("-rf", t)
    except sh.ErrorReturnCode_128:
        pass

# Always ours:
always_ours = [
    ".github",
    "devices",
    "supervisor",
    "shared-bindings",
    "shared-module",
    "ports/atmel-samd",
    "ports/cxd56",
    "ports/espressif",
    "ports/mimxrt10xx",
    "ports/raspberrypi",
    "ports/stm",
]
for ours in always_ours:
    out_buf = StringIO()
    git.status("--porcelain=1", ours, _out=out_buf)
    out_buf.seek(0)
    line = out_buf.readline()
    while line:
        state, path = line.split()
        if state == "UU":
            print("ours", path)
            git.checkout("--ours", path)
            git.add(path)
        else:
            print(state, path)
        line = out_buf.readline()

# # Check to see if any files changed only in formatting
# out_buf = StringIO()
# git.status("--porcelain=1", ".", _out=out_buf)
# out_buf.seek(0)
# line = out_buf.readline()
# while line:
#     state = line.split()[0]
#     if state in ("D", "R", "DD"):
#         line = out_buf.readline()
#         continue
#     state, path = line.split()
#     log_buf = StringIO()
#     git.log("--pretty=tformat:%H", "25ae98f..HEAD", path, _out=log_buf, _tty_out=False)
#     log_buf.seek(0)
#     commits = []
#     for line in log_buf.readlines():
#         commits.append(line.strip())
#     if state in ["UU", "M"] and commits == ["a52eb88031620a81521b937f2a0651dbac2bb350"]:
#         git.checkout("--theirs", path)
#         git.add(path)
#     line = out_buf.readline()
