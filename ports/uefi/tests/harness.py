#!/usr/bin/env python3
# SPDX-License-Identifier: MIT
# Copyright (c) 2026 Nicko van Someren

"""Interactive / launch-path smoke checks for the MicroPython UEFI port.

The full test suite is run by tests/run_uefi_tests.py (`make test`) over the raw REPL.
This harness covers the checks that don't fit that model — driving the live REPL
(`--mode repl`), a shell-launched script (`--mode runfile`), and a boot-option script
(`--mode bootopt`) — booting the staged ESP under QEMU + firmware for the chosen ARCH.

Designed to run inside the dev container (firmware seeded from /opt/ovmf into
./firmware by the Makefile). Always pass a timeout so a bad boot fails fast.
"""

import argparse
import os
import pathlib
import subprocess
import sys
import threading
import time

ROOT = pathlib.Path(__file__).resolve().parent.parent  # ports/uefi
FW = ROOT / "firmware"
ESP = ROOT / "esp"


# net_flags / graphics_flags below are the QEMU device flags for a NIC / a display
# adapter. They are imported by tests/run_uefi_tests.py for its --net / --graphics modes;
# this harness's own repl/runfile/bootopt smoke checks run bare (no NIC/display).
def net_flags(arch: str) -> list[str]:
    """QEMU devices for a working UEFI network stack reaching SLIRP user-net.

    virtio-rng is MANDATORY: the EDK2 upper network stack (IP4/DHCP/TCP) has a DEPEX
    on gEfiRngProtocol, produced only by VirtioRngDxe — without it only SNP loads and
    ifconfig/IP4Config2 never appear. Slots are pinned on aa64 so the NIC/RNG don't take
    slot 0x2 and shift the ESP disk's PCIe address — which would move it off FS0:, the
    filesystem the Shell's startup.nsh selects to launch the app. SLIRP hands out
    10.0.2.15 / gw 10.0.2.2 / DNS 10.0.2.3 deterministically.
    """
    rng_addr = ",addr=0xc" if arch == "aa64" else ""
    nic_addr = ",addr=0xb" if arch == "aa64" else ""
    # hostfwd maps host 127.0.0.1:<port> -> guest :<port> so the guest server
    # self-tests (listen/accept) can be driven by a host client: NET_SERVER_PORT for
    # the blocking accept (N6), NET_SERVER_PORT2 for the asyncio.start_server (N6b).
    return [
        "-device",
        "virtio-rng-pci" + rng_addr,
        "-netdev",
        "user,id=net0,hostfwd=tcp:127.0.0.1:%d-:%d,hostfwd=tcp:127.0.0.1:%d-:%d"
        % (NET_SERVER_PORT, NET_SERVER_PORT, NET_SERVER_PORT2, NET_SERVER_PORT2),
        "-device",
        "virtio-net-pci,netdev=net0" + nic_addr,
    ]


# hostfwd ports for net_flags (used when a NIC is attached).
NET_SERVER_PORT = 8888
NET_SERVER_PORT2 = 8889


def graphics_flags(arch: str, mode: str) -> list[str]:
    """QEMU flags to bring up a display adapter (so GOP is published) for `mode`.

    mode "headless": adapter present but no host window (`-display none`) — keeps
    the run hermetic/CI-safe while still exercising the GOP path (notably on the
    aa64 `virt` machine, which has no display device by default).
    mode "window": a real host window. The backend defaults to `cocoa` (macOS);
    override with QEMU_DISPLAY for other hosts (a generic seam until we pick a
    portable default). A window needs host QEMU — it can't come from the container.
    Resolution comes from GFX_XRES/GFX_YRES (EDID), default 1280x800.
    """
    xres = os.environ.get("GFX_XRES", "1280")
    yres = os.environ.get("GFX_YRES", "800")
    # A USB keyboard so the graphical console can accept input — the firmware's
    # ConSplitter folds it into ConIn (which the port polls), alongside serial. The
    # aa64 `virt` machine has no keyboard at all by default, so without this the
    # graphics window is input-dead even though the serial console still works.
    if arch == "x64":
        # q35's default adapter is std-VGA; replace it so we own the EDID sizing.
        dev = ["-vga", "none", "-device", f"VGA,edid=on,xres={xres},yres={yres}"]
        kbd = ["-device", "qemu-xhci", "-device", "usb-kbd"]
    else:
        # 'virt' has no legacy VGA; virtio-gpu-pci supports EDID-driven sizing. Pin
        # the PCIe slots (GPU 0x9, xHCI 0xa) so they don't take slot 0x2 and shift
        # the boot disk's address — which would move the ESP off FS0:, the filesystem
        # the Shell's startup.nsh selects to launch the app.
        dev = ["-device", f"virtio-gpu-pci,edid=on,xres={xres},yres={yres},addr=0x9"]
        kbd = ["-device", "qemu-xhci,addr=0xa", "-device", "usb-kbd"]
    if mode == "window":
        backend = os.environ.get("QEMU_DISPLAY", "cocoa")
        return dev + kbd + ["-display", backend]
    return dev + kbd + ["-display", "none"]


def qemu_cmd(arch: str, serial: str = "file") -> list[str]:
    fw = FW
    common = ["-m", "512M", "-no-reboot"]
    drives = ["-drive", f"format=raw,file=fat:rw:{ESP}"]
    if arch == "x64":
        cmd = [
            "qemu-system-x86_64",
            "-machine",
            "q35",
            "-drive",
            f"if=pflash,format=raw,readonly=on,file={fw}/OVMF_CODE.fd",
            "-drive",
            f"if=pflash,format=raw,file={fw}/OVMF_VARS.fd",
            *drives,
            "-device",
            "isa-debug-exit,iobase=0xf4,iosize=0x04",
            "-debugcon",
            f"file:{FW}/debug.log",
            "-global",
            "isa-debugcon.iobase=0x402",
        ]
    elif arch == "aa64":
        cmd = [
            "qemu-system-aarch64",
            "-machine",
            "virt",
            "-cpu",
            "cortex-a72",
            "-drive",
            f"if=pflash,format=raw,readonly=on,file={fw}/AAVMF_CODE.fd",
            "-drive",
            f"if=pflash,format=raw,file={fw}/AAVMF_VARS.fd",
            *drives,
        ]
    else:
        raise SystemExit(f"unknown arch {arch!r} (use x64 or aa64)")
    cmd += common
    cmd += ["-display", "none"]
    # "file": serial captured to a log. "stdio": serial on the process stdin/stdout so
    # the REPL can be driven (repl/runfile/bootopt modes). Monitor disabled either way.
    if serial == "stdio":
        # signal=off so a Ctrl-C (0x03) on stdin is delivered to the guest serial
        # (our ConIn) instead of raising SIGINT in QEMU and killing it — required
        # to exercise the REPL's break-into-running-code path.
        cmd += [
            "-chardev",
            "stdio,id=ttyS0,signal=off",
            "-serial",
            "chardev:ttyS0",
            "-monitor",
            "none",
        ]
    else:
        cmd += ["-serial", f"file:{FW}/serial.log", "-monitor", "none"]
    return cmd


def _tail(text: str, n: int = 40) -> str:
    lines = text.splitlines()
    return "\n".join(lines[-n:])


def _print_tail(name: str, n: int = 40) -> None:
    p = FW / name
    if p.exists() and p.stat().st_size:
        print(f"\n===== {name} (last {n} lines; full file at {p}) =====")
        print(_tail(p.read_text(errors="replace"), n))


REPL_STEPS = [
    ("2 + 3", "5"),
    ("zz\x7f\x7f11 * 11", "121"),
    ("print('hi', 6 * 7)", "hi 42"),
    ("xs = [i * i for i in range(5)]", ">>>"),
    ("print(xs)", "[0, 1, 4, 9, 16]"),
    ("1 / 0", "ZeroDivisionError"),
]


def run_repl(arch: str, timeout: int = 90) -> int:
    import pexpect  # provided by the dev image's uv venv

    FW.mkdir(exist_ok=True)
    cmd = qemu_cmd(arch, serial="stdio")
    child = pexpect.spawn(
        cmd[0], cmd[1:], timeout=timeout, encoding="utf-8", codec_errors="replace"
    )
    transcript = []
    child.logfile_read = type(
        "W", (), {"write": lambda _self, s: transcript.append(s), "flush": lambda _self: None}
    )()
    try:
        # Sync past the verbose NOOPT firmware boot output, then the first prompt.
        child.expect_exact("MicroPython on UEFI")
        child.expect_exact(">>> ")
        for line, expect in REPL_STEPS:
            child.send(line + "\r")
            child.expect_exact(expect)
        # Arrow keys: the firmware translates terminal escape sequences (ESC[A etc.)
        # into UEFI scan codes, which the ConIn notify maps to readline's keys.
        # Up-arrow recalls the previous history line; left-arrow moves the cursor so
        # an inserted digit lands mid-line.
        child.send("print(0x6b)\r")  # prints 107
        child.expect_exact("107")
        child.expect_exact(">>> ")
        child.send("\x1b[A")  # up arrow -> recall "print(0x6b)"
        child.send("\r")
        child.expect_exact("107")  # re-run from history
        child.expect_exact(">>> ")
        # Line-tail redraw regression guard. readline moves the cursor left (left
        # arrow / Home) WITHOUT redrawing the tail, so it relies on the move being
        # non-destructive. If the port moves with a backspace and the console's BS
        # is destructive, the tail of the line is erased on every leftward move
        # (reappearing only when a rightward move reprints it). The port instead
        # repositions via ConOut->SetCursorPosition, which emits no backspace — so
        # assert no BS byte (0x08) is produced while editing this line.
        edit_mark = len("".join(transcript))
        child.send("12")
        child.send("\x1b[D")  # left arrow -> cursor between 1 and 2
        child.send("0\r")  # insert '0' -> evaluates 102
        child.expect_exact("102")
        child.expect_exact(">>> ")
        if "\x08" in "".join(transcript)[edit_mark:]:
            raise AssertionError(
                "cursor moved with a destructive backspace (line-tail redraw regression)"
            )
        # A periodic machine.Timer callback must fire while the REPL is idle waiting
        # on input, with NO keypress: the timer notify schedules the callback and the
        # scheduler hook signals the wakeup event, breaking the blocked WaitForEvent.
        # (Regression: previously callbacks only ran when a key was pressed.)
        child.send("import machine\r")
        child.expect_exact(">>> ")
        child.send("def cb(t): print('TICK')\r")  # -> "..." continuation
        child.send("\r")  # blank line defines cb
        child.expect_exact(">>> ")
        child.send(
            "tmr = machine.Timer(-1, period=150, mode=machine.Timer.PERIODIC, callback=cb)\r"
        )
        child.expect_exact(">>> ")
        child.expect_exact("TICK")  # fires with no further input
        child.expect_exact("TICK")  # and keeps firing
        child.send("tmr.deinit()\r")
        child.expect_exact(">>> ")

        # Ctrl-C must break into *running* code. The interrupt char is armed by pyexec
        # only around code execution, so we must wait until the loop is genuinely
        # running before interrupting (else 0x03 is buffered and ignored). The loop
        # prints a marker; we wait for two *consecutive* markers — a pattern that can
        # only come from runtime output, never the single echoed input line — then
        # send Ctrl-C and require a clean KeyboardInterrupt + fresh prompt.
        child.send("while 1: print('RUN')\r")  # -> "..." continuation
        child.send("\r")  # blank line -> loop runs
        child.expect("RUN[\r\n]+RUN")  # genuine runtime output (not echo)
        child.sendcontrol("c")
        child.expect_exact("KeyboardInterrupt")
        child.expect_exact(">>> ")

        # stdout control-char handling (mphalport.c): ConOut only handles NUL/BS/LF/
        # CR itself, so the port adds the common effectors. We assert on the emitted
        # bytes (a raw byte sink here, not a rendered terminal): BEL/NUL are dropped,
        # TAB expands to spaces (no raw 0x09), and FF triggers ClearScreen (ESC[2J).
        child.send("import sys\r")
        child.expect_exact(">>> ")
        cc_mark = len("".join(transcript))
        child.send("sys.stdout.write('p\\tq\\a\\x00r')\r")  # tab-expanded; BEL/NUL dropped
        child.expect_exact(">>> ")
        cc_out = "".join(transcript)[cc_mark:]
        # The echoed input contains '\t','\a','\x00' as backslash escapes (two chars
        # each), never the raw control bytes — so a raw 0x07/0x00/0x09 in the stream
        # can only come from the rendered write().
        if "\x07" in cc_out or "\x00" in cc_out:
            raise AssertionError("BEL/NUL not dropped from stdout")
        if "\x09" in cc_out:
            raise AssertionError("TAB not expanded to spaces in stdout")
        ff_mark = len("".join(transcript))
        child.send("sys.stdout.write('\\f')\r")  # form feed -> ClearScreen
        child.expect_exact(">>> ")
        if "\x1b[2J" not in "".join(transcript)[ff_mark:]:
            raise AssertionError("form feed did not clear the screen (ClearScreen)")

        # Ctrl-D exits the REPL; a clean app exit returns control to its caller
        # (shell / boot manager) — it must NOT power the machine off. We key on the
        # exit marker rather than EOF, since the app returns instead of terminating
        # QEMU (the firmware decides what runs next; we terminate QEMU ourselves).
        child.sendcontrol("d")
        child.expect_exact("MPY-UEFI: exit")
    except (pexpect.TIMEOUT, pexpect.EOF, AssertionError) as e:
        print(f"FAIL (repl {arch}): {type(e).__name__}: {e}")
        print(_tail("".join(transcript), 50))
        return 1
    finally:
        if child.isalive():
            child.terminate(force=True)
    print(f"PASS (repl {arch}): REPL evaluated all steps and exited")
    return 0


def run_runfile(arch: str, timeout: int = 60) -> int:
    """Launch the app from the shell with a script argument (`app.efi foo.py a b`)
    and check the script runs with sys.argv set — the `python foo.py` path.

    Relies on the shell launching us (which installs EFI_SHELL_PARAMETERS_PROTOCOL);
    that's the aa64 auto-boot path (via startup.nsh). We swap in a startup.nsh that
    passes arguments, then restore the normal one afterwards.
    """
    import pexpect  # provided by the dev image's uv venv

    efi = "BOOTAA64.EFI" if arch == "aa64" else "BOOTX64.EFI"
    normal_nsh = "@echo -off\r\nFS0:\r\n\\EFI\\BOOT\\%s\r\n" % efi
    (ESP / "argtest.py").write_text("import sys\nprint('ARGV', sys.argv)\n")
    (ESP / "startup.nsh").write_text(
        "@echo -off\r\nFS0:\r\n\\EFI\\BOOT\\%s argtest.py alpha beta\r\n" % efi
    )

    cmd = qemu_cmd(arch, serial="stdio")
    child = pexpect.spawn(
        cmd[0], cmd[1:], timeout=timeout, encoding="utf-8", codec_errors="replace"
    )
    transcript = []
    child.logfile_read = type(
        "W", (), {"write": lambda _self, s: transcript.append(s), "flush": lambda _self: None}
    )()
    try:
        child.expect_exact("ARGV ['argtest.py', 'alpha', 'beta']")
        ok = True
    except (pexpect.TIMEOUT, pexpect.EOF):
        ok = False
    finally:
        if child.isalive():
            child.terminate(force=True)
        (ESP / "startup.nsh").write_text(normal_nsh)  # restore normal auto-launch
        try:
            (ESP / "argtest.py").unlink()
        except OSError:
            pass

    if ok:
        print(f"PASS (runfile {arch}): script ran with sys.argv")
        return 0
    print(f"FAIL (runfile {arch}): script did not run as expected")
    print(_tail("".join(transcript), 40))
    return 1


# Script (run in phase 1, in the shell/ShellParameters flow) that writes a Boot####
# entry whose device path points at our own app on the boot volume and whose
# OptionalData is the command line "argtest.py", then points BootNext at it. NOTE the
# manual UTF-16LE encoder: MicroPython's str.encode("utf-16-le") silently emits UTF-8,
# which would corrupt the file-path node and the optional data.
_BOOTOPT_MK = r"""def u16(s):
    b = bytearray()
    for ch in s:
        c = ord(ch); b.append(c & 0xFF); b.append((c >> 8) & 0xFF)
    return bytes(b)
import uefi
from uefi import boot as BT, protocols as P, variable as V, guid as G, device_path as DP
img = uefi.Handle(uefi.raw.image_handle())
li = P.LOADED_IMAGE.open(img)
voldp = uefi.Handle(li.device_handle).device_path()
li.close()
node = DP.DevicePathNode(DP.MEDIA, DP.MEDIA_FILEPATH, u16("\\EFI\\BOOT\\__EFI__\x00"))
full = DP.DevicePath(list(voldp) + [node])
lo = BT.LoadOption("MP LoadOpt Test", full, u16("argtest.py\x00"))
a = V.NON_VOLATILE | V.BOOTSERVICE_ACCESS | V.RUNTIME_ACCESS
V.set("Boot0007", G.GLOBAL_VARIABLE, lo.to_bytes(), a)
V.set("BootNext", G.GLOBAL_VARIABLE, b"\x07\x00", a)
print("MKBOOT-OK")
"""


def _cmd_with_vars(arch: str, vars_path: str) -> list[str]:
    """qemu_cmd (serial on stdio) with the pflash VARS store swapped for `vars_path`,
    so the two boot phases share a private, writable variable store."""
    vfile = str(FW / ("AAVMF_VARS.fd" if arch == "aa64" else "OVMF_VARS.fd"))
    return [arg.replace(vfile, str(vars_path)) for arg in qemu_cmd(arch, serial="stdio")]


def run_bootopt(arch: str = "aa64", timeout: int = 60) -> int:
    """Verify the app consumes a boot option's OptionalData as its command line — the
    boot-manager launch path (LoadedImage->LoadOptions), distinct from the shell
    (ShellParameters) path. Two phases share one private vars store: phase 1 (shell)
    runs a script that writes a Boot#### entry (device path -> our app, OptionalData
    "argtest.py") and BootNext; phase 2 boots fresh, so BDS honours BootNext and
    launches us with those LoadOptions. Hermetic; uses the aa64 startup.nsh shell flow.
    """
    import pexpect
    import shutil
    import tempfile

    efi = "BOOTAA64.EFI" if arch == "aa64" else "BOOTX64.EFI"
    normal_nsh = "@echo -off\r\nFS0:\r\n\\EFI\\BOOT\\%s\r\n" % efi
    (ESP / "mkbootnext.py").write_text(_BOOTOPT_MK.replace("__EFI__", efi))
    (ESP / "argtest.py").write_text('import sys\nprint("LOADOPT", sys.argv)\n')
    vars_src = FW / ("AAVMF_VARS.fd" if arch == "aa64" else "OVMF_VARS.fd")
    tv = tempfile.NamedTemporaryFile(suffix="_VARS.fd", delete=False)
    tv.close()
    shutil.copy(vars_src, tv.name)

    def _phase(want: str) -> bool:
        cmd = _cmd_with_vars(arch, tv.name)
        child = pexpect.spawn(
            cmd[0], cmd[1:], timeout=timeout, encoding="utf-8", codec_errors="replace"
        )
        try:
            child.expect_exact(want)
            return True
        except (pexpect.TIMEOUT, pexpect.EOF):
            return False
        finally:
            if child.isalive():
                child.terminate(force=True)

    p1 = p2 = False
    try:
        # Phase 1: shell writes the boot option + BootNext (persisted to the vars store).
        (ESP / "startup.nsh").write_text(
            "@echo -off\r\nFS0:\r\n\\EFI\\BOOT\\%s mkbootnext.py\r\nexit\r\n" % efi
        )
        p1 = _phase("MKBOOT-OK")
        # Phase 2: fresh boot; BDS honours BootNext -> app launched with LoadOptions.
        (ESP / "startup.nsh").write_text("@echo -off\r\n")
        p2 = _phase("LOADOPT ['argtest.py']")
    finally:
        (ESP / "startup.nsh").write_text(normal_nsh)  # restore normal auto-launch
        for f in ("mkbootnext.py", "argtest.py"):
            try:
                (ESP / f).unlink()
            except OSError:
                pass
        try:
            os.unlink(tv.name)
        except OSError:
            pass

    if p1 and p2:
        print(f"PASS (bootopt {arch}): boot-option OptionalData ran as the command line")
        return 0
    print(f"FAIL (bootopt {arch}): setup={p1} launch={p2}")
    return 1


def main() -> int:
    # Docker hands us a non-blocking stdout pipe; force blocking so large or
    # bursty writes can't raise BlockingIOError mid-print.
    try:
        os.set_blocking(sys.stdout.fileno(), True)
    except (ValueError, OSError):
        pass
    # The full test suite is run by tests/run_uefi_tests.py (make test); this harness
    # covers the interactive/launch-path smoke checks that don't fit the raw-REPL runner.
    ap = argparse.ArgumentParser()
    ap.add_argument("--arch", choices=["x64", "aa64"], default="aa64")
    ap.add_argument("--mode", choices=["repl", "runfile", "bootopt"], default="repl")
    ap.add_argument("--timeout", type=int, default=60)
    ap.add_argument("path", nargs="?")
    a = ap.parse_args()
    if a.mode == "runfile":
        return run_runfile(a.arch, max(a.timeout, 60))
    if a.mode == "bootopt":
        return run_bootopt(a.arch, max(a.timeout, 60))
    return run_repl(a.arch, max(a.timeout, 90))


if __name__ == "__main__":
    sys.exit(main())
