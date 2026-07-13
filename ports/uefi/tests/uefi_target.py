#!/usr/bin/env python3
# SPDX-License-Identifier: MIT
# Copyright (c) 2026 Nicko van Someren

"""The uefi test target: brings QEMU up into a state ready for testing, then execs
into it so its stdin/stdout become this process's own -- i.e. whatever spawned this
script ends up talking directly to the guest's byte-clean serial REPL, with nothing
in between.

This is NOT something a developer or CI runs directly. `tests/run-tests.py` is the
only command anyone ever invokes; this script is what its `-t uefi:exec:<this
script>` connects to (see ports/uefi/Makefile for how that's assembled). Nothing
here is uefi-specific glue *around* run-tests.py -- there is no such glue: this port's
raw-REPL protocol works with a completely stock, unmodified pyboard.py, the same way
real hardware and ports/qemu do. See:
  * main.c's run_repl()/uefi_launch_argv()/uefi_exit_status() for why -- `--raw` boots
    straight into raw-REPL mode, `--faux-soft` prints the "soft reboot" text stock
    pyboard.py's enter_raw_repl(soft_reset=True) expects after Ctrl-D, and every
    forced exit (raw or friendly REPL alike) is a clean exit that this port's EFI
    Shell startup.nsh loop relaunches -- a stand-in for the in-place soft reset real
    hardware has that this port doesn't.
  * tests/test_utils.py's _TARGET_CONNECT_TIMEOUT_S for the "uefi:" prefix on -t,
    which is the one piece of accommodation this port needs from the shared test
    framework: a cold VM+firmware+Shell boot takes longer than the 10s default
    upstream's own first connection allows real hardware / ports/qemu, whose device
    is already sitting idle, ever need.

Env vars (set by the Makefile; this script takes no arguments):
  UEFI_TEST_ARCH        aa64 (default) or x64
  UEFI_TEST_NET         if set, attach a NIC + RNG and use the network-enabled firmware
  UEFI_TEST_GRAPHICS    if set, attach a headless display adapter (GOP, no window)
  UEFI_TEST_CPU         override the QEMU -cpu model
  UEFI_TEST_ACCEL       e.g. "kvm" (Linux) / "hvf" (macOS) for hardware acceleration
                        (default: TCG, which works everywhere but is slower)
  UEFI_TEST_SERIAL_LOG  tee the guest serial stream to this file for post-mortem
                        diagnosis of a failing run
  UEFI_TEST_ESP         override the staged ESP directory (default: ports/uefi/esp)
  UEFI_TEST_BOOT_TIMEOUT  seconds to wait for the one-time BootOrder-bootstrap boot
                        (default 60; this port's own real cold boot is ~10s, this is
                        just headroom for a slow/loaded CI host)
"""

import os
import shutil
import sys
import tempfile
import time

import pexpect

HERE = os.path.dirname(os.path.abspath(__file__))  # ports/uefi/tests
PORT = os.path.dirname(HERE)  # ports/uefi
TOP = os.path.dirname(os.path.dirname(PORT))  # repo root
sys.path.insert(0, HERE)  # so we can reuse harness device helpers
from harness import graphics_flags, net_flags  # noqa: E402

ESP = os.environ.get("UEFI_TEST_ESP", os.path.join(PORT, "esp"))
RELFW = os.path.join(PORT, "firmware", "release")
RELNETFW = os.path.join(PORT, "firmware", "release-net")

# Target architecture. Per-arch: the QEMU binary + machine, and the removable-media
# app name the firmware auto-boots.
ARCH = os.environ.get("UEFI_TEST_ARCH", "aa64")
_ARCH = {
    "aa64": {
        "qemu": "qemu-system-aarch64",
        "machine": ["-machine", "virt"],
        "cpu": "cortex-a72",
        "fw": "AAVMF",
        "efi": "BOOTAA64.EFI",
    },
    "x64": {
        "qemu": "qemu-system-x86_64",
        "machine": ["-machine", "q35"],
        "cpu": None,  # q35's default CPU is fine for TCG
        "fw": "OVMF",
        "efi": "BOOTX64.EFI",
    },
}
if ARCH not in _ARCH:
    raise SystemExit(f"UEFI_TEST_ARCH={ARCH!r} not supported (use aa64 or x64)")
_A = _ARCH[ARCH]
QEMU = _A["qemu"]
QEMU_CPU = os.environ.get("UEFI_TEST_CPU", _A["cpu"] or "")
QEMU_MACHINE = _A["machine"] + (["-cpu", QEMU_CPU] if QEMU_CPU else [])
QEMU_ACCEL = os.environ.get("UEFI_TEST_ACCEL", "")

USE_NET = bool(os.environ.get("UEFI_TEST_NET"))
GFX_MODE = "headless" if os.environ.get("UEFI_TEST_GRAPHICS") else None

# Firmware: lean RELEASE by default (fast, quiet serial); UEFI_TEST_NET selects the
# network-enabled firmware (release-net: RELEASE + the full net/TLS stack).
CODE = os.path.join(RELNETFW if USE_NET else RELFW, f"{_A['fw']}_CODE.fd")
VARS_SRC = os.path.join(RELNETFW if USE_NET else RELFW, f"{_A['fw']}_VARS.fd")
EFI = _A["efi"]
APP = "\\EFI\\BOOT\\" + EFI  # stays at the removable-media path; the Shell launches it

_SERIAL_LOG = os.environ.get("UEFI_TEST_SERIAL_LOG")

# Bootstrap script (run once, in the app): make the firmware's built-in UEFI Shell the FIRST
# boot option, with a zero timeout. Booting the Shell first keeps the cold boot off the
# removable-media/PXE fallback — with a NIC present OVMF otherwise detours into "Start PXE over
# IPv4" and never reaches the Shell. The Shell then runs startup.nsh's relaunch loop.
_SETUP_SRC = r"""from uefi import boot as BT, variable as V, guid as G
a = V.NON_VOLATILE | V.BOOTSERVICE_ACCESS | V.RUNTIME_ACCESS
shell = None
order = BT.boot_order()
for num in list(order) + [n for n in range(0x40) if n not in order]:
    name = "Boot%04X" % num
    if not V.exists(name, G.GLOBAL_VARIABLE):
        continue
    try:
        lo = BT.LoadOption.parse(V.get(name, G.GLOBAL_VARIABLE))
    except Exception:
        continue
    if "Shell" in lo.description:
        shell = num
        break
if shell is None:
    print("SETUP-FAIL")
else:
    BT.set_boot_order([shell])
    V.set("Timeout", G.GLOBAL_VARIABLE, b"\x00\x00", a)
    print("SETUP-OK")
"""


def _expect_heartbeat(child, patterns, timeout, interval=4):
    """child.expect(patterns), but polling in `interval`-second slices and printing a
    harmless placeholder line to *our own* stdout between them.

    tests/test_utils.py's enter_raw_repl() call only widens the *overall* connection
    timeout (_TARGET_CONNECT_TIMEOUT_S); pyboard.py's read_until also has a separate,
    unconfigurable ~10s *per-character* (silence) timeout on the raw-REPL side. This
    process's own stdout is what pyboard.py is actually reading (see main()'s exec
    below), and it stays completely silent for however long this bootstrap phase
    takes -- so without this, a single slow bootstrap step could trip that silence
    timeout even though the overall connection budget hasn't been exceeded. The
    placeholder text can never be mistaken for a real banner (nothing we wait for
    starts with '#'), so it's always safe to interleave.
    """
    start = time.time()
    while True:
        remaining = timeout - (time.time() - start)
        if remaining <= 0:
            return child.expect(patterns, timeout=0)
        try:
            return child.expect(patterns, timeout=min(interval, remaining))
        except pexpect.TIMEOUT:
            sys.stdout.write("# uefi target: still booting (%ds)\n" % int(time.time() - start))
            sys.stdout.flush()


def _qemu(varsfile, no_reboot):
    cmd = [QEMU, *QEMU_MACHINE]
    if QEMU_ACCEL:
        cmd += ["-accel", QEMU_ACCEL]
    cmd += [
        "-drive",
        f"if=pflash,format=raw,readonly=on,file={CODE}",
        "-drive",
        f"if=pflash,format=raw,file={varsfile}",
        "-drive",
        f"format=raw,file=fat:rw:{ESP}",
        "-m",
        "512M",
        "-serial",
        "stdio",
    ]
    # Emulated hardware (same for the bootstrap and the real run, so the ESP stays on FS0:).
    # A default NIC is present even without UEFI_TEST_NET (QEMU adds one); the bootstrap
    # makes the UEFI Shell the first boot option (Timeout=0) so OVMF never falls through to PXE.
    if USE_NET:
        cmd += net_flags(ARCH)
    cmd += graphics_flags(ARCH, GFX_MODE) if GFX_MODE else ["-display", "none"]
    cmd += ["-monitor", "none"]
    if no_reboot:
        cmd.append("-no-reboot")
    return cmd


def bootstrap():
    """Bootstrap a VARS snapshot whose BootOrder puts the firmware's built-in UEFI Shell
    FIRST (Timeout=0), and stage the always-raw relaunch startup.nsh. Returns the VARS
    file path.

    Booting the Shell first is what keeps every cold boot off the removable-media/PXE
    fallback: with a NIC present (QEMU adds one even without UEFI_TEST_NET), OVMF
    otherwise falls through to PXE ("Start PXE over IPv4"), which stalls so the Shell is
    never reached. The Shell runs startup.nsh: a plain cold-boot line, then a loop that
    relaunches with --faux-soft added -- see main.c for how those two flags make an
    unmodified pyboard.py drive this port directly.

    Firmware-agnostic bootstrap (the app must run once to write the boot variables): AAVMF
    drops into the Shell and runs startup.nsh (app + setup arg -> SETUP-OK); OVMF auto-boots
    the removable app at \\EFI\\BOOT\\<EFI> to its REPL with no args (that removable boot
    SUCCEEDS, so this first boot also avoids PXE), which we then drive to run the setup.
    Both reorder BootOrder to the Shell for every subsequent boot."""
    with open(os.path.join(ESP, "setup_boot.py"), "w") as f:
        f.write(_SETUP_SRC)
    nsh = os.path.join(ESP, "startup.nsh")
    with open(nsh, "w") as f:  # bootstrap phase: run the app once with the setup script
        f.write("@echo -off\r\nFS0:\r\n%s setup_boot.py\r\n" % APP)
    tv = tempfile.NamedTemporaryFile(suffix="_VARS.fd", delete=False)
    tv.close()
    shutil.copy(VARS_SRC, tv.name)
    cmd = _qemu(tv.name, no_reboot=True)
    boot_timeout = int(os.environ.get("UEFI_TEST_BOOT_TIMEOUT", "60"))
    child = pexpect.spawn(cmd[0], cmd[1:], timeout=boot_timeout, encoding="latin-1")
    try:
        # AAVMF: Shell runs startup.nsh (app + setup arg) -> SETUP-OK. OVMF: auto-boots the
        # removable app to its REPL (no args) -> drive it to run the setup script.
        if _expect_heartbeat(child, ["SETUP-OK", ">>> "], boot_timeout) == 1:
            child.send("exec(open('/setup_boot.py').read())\r")
            _expect_heartbeat(child, "SETUP-OK", 30)
    finally:
        if child.isalive():
            child.terminate(force=True)
        os.remove(os.path.join(ESP, "setup_boot.py"))
    # Test phase: the Shell (now first in BootOrder) runs this. The cold-boot line has no
    # --faux-soft (it's a genuine cold boot, not standing in for a reset); every relaunch
    # does, so main.c only ever claims "soft reboot" when that's actually what's happening.
    with open(nsh, "w") as f:
        f.write(
            "@echo -off\r\nFS0:\r\n%s --serial --raw\r\n:loop\r\n"
            "%s --serial --raw --faux-soft\r\ngoto loop\r\n" % (APP, APP)
        )
    return tv.name


def main():
    for f in (CODE, VARS_SRC, os.path.join(ESP, "EFI", "BOOT", EFI)):
        if not os.path.exists(f):
            sys.stderr.write(
                f"uefi target: missing {f} -- build and stage first "
                f"(make ARCH={ARCH} stage; UEFI_TEST_NET needs "
                f"docker/build-ovmf-release-net.sh)\n"
            )
            sys.exit(1)

    tv = bootstrap()
    cmd = _qemu(tv, no_reboot=False)
    if _SERIAL_LOG:
        # A shell pipe so the tee still applies after we replace ourselves below -- os.execvp
        # can't set up a pipeline on its own. killpg (Pyboard.close()) still reaches both
        # pipeline stages: they inherit our process group, unmodified.
        os.execvp("/bin/sh", ["/bin/sh", "-c", f"{' '.join(cmd)} | tee {_SERIAL_LOG}"])
    else:
        # Replace this process with QEMU outright: its stdin/stdout become exactly what
        # spawned us (pyboard.py's pipes), with no buffering layer of ours in between.
        os.execvp(cmd[0], cmd)


if __name__ == "__main__":
    main()
