"""
This is a helper script for merging in new versions of MicroPython. You *must*
evaluate it's correctness and adapt it for each MP version. This is committed
in the repo more for reference than "fire and forget" use.
"""

import sh
from sh import git
from io import StringIO

out_buf = StringIO()

ports_to_delete = [
    "mimxrt",
    "powerpc",
    "samd",
    "javascript",
    "stm32",
    "esp32",
    "cc3200",
    "teensy",
    "windows",
    "zephyr",
    "minimal",
    "esp8266",
    "pic16bit",
    "qemu-arm",
    "bare-arm",
    "rp2",
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
    "pyboard",
    "library/pyb.*",
    "library/esp*",
    "library/lcd160cr.rst",
    "esp8266",
    "wipy",
    "esp32",
    "library/machine.*",
    "library/network.*",
    "library/ubluetooth.rst",
    "library/ucryptolib.rst",
    "library/uos.rst",
    "library/usocket.rst",
    "library/ussl.rst",
    "library/ustruct.rst",
    "develop",
    "reference",
    "make.bat",
    "templates/topindex.html",
]
for d in docs_to_delete:
    try:
        git.rm("-rf", "docs/" + d)
    except sh.ErrorReturnCode_128:
        pass

tests_to_delete = [
    "wipy",
    "pyb",
    "multi_net",
    "net_inet",
    "multi_bluetooth",
    "esp32",
    "net_hosted",
]
for t in tests_to_delete:
    try:
        git.rm("-rf", "tests/" + t)
    except sh.ErrorReturnCode_128:
        pass


libs_to_delete = [
    "mynewt-nimble",
    "nxp_driver",
    "mbedtls",
    "mbedtls_errors",
    "asf4",
    "btstack",
]
for l in libs_to_delete:
    try:
        git.rm("-rf", "lib/" + l)
    except sh.ErrorReturnCode_128:
        pass

extmod_to_delete = [
    "machine_*",
    "webrepl",
    "uzlib",
    "ussl*",
    "modlwip.c",
    "moducryptolib.c",
    "modbluetooth*",
    "network*",
    "nimble",
    "btstack",
    "mpbthci*",
]
for e in extmod_to_delete:
    try:
        git.rm("-rf", "extmod/" + e)
    except sh.ErrorReturnCode_128 as error:
        print(error)
        pass

top_delete = [
    "drivers",
    ".travis.yml",
    ".github/FUNDING.yml",
    ".github/workflows/docs.yml",
    "README.md",
    "CODEOFCONDUCT.md",
    "CODECONVENTIONS.md",
    "examples",
]
for t in top_delete:
    try:
        git.rm("-rf", t)
    except sh.ErrorReturnCode_128:
        pass

# Always ours:
always_ours = [
    "devices",
    "supervisor",
    "shared-bindings",
    "shared-module",
    "ports/atmel-samd",
    "ports/cxd56",
    "ports/esp32s2",
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

# Check to see if any files changed only in formatting
out_buf = StringIO()
git.status("--porcelain=1", ".", _out=out_buf)
out_buf.seek(0)
line = out_buf.readline()
while line:
    state = line.split()[0]
    if state in ("D", "R", "DD"):
        line = out_buf.readline()
        continue
    state, path = line.split()
    log_buf = StringIO()
    git.log("--pretty=tformat:%H", "25ae98f..HEAD", path, _out=log_buf, _tty_out=False)
    log_buf.seek(0)
    commits = []
    for line in log_buf.readlines():
        commits.append(line.strip())
    if state in ["UU", "M"] and commits == ["a52eb88031620a81521b937f2a0651dbac2bb350"]:
        git.checkout("--theirs", path)
        git.add(path)
    line = out_buf.readline()
