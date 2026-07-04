#!/usr/bin/env python3
# This file is part of the MicroPython project, http://micropython.org/
#
# The MIT License (MIT)
#
# Copyright (c) 2026 Nicko van Someren
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

"""Headless boot-and-assert driver for the MicroPython UEFI port.

The authoritative pass/fail signal behind `make test`. Boots the staged ESP under
QEMU + firmware for the chosen ARCH, then decides pass/fail deterministically:

  * x64  : QEMU `isa-debug-exit` -> process exit code 33 == pass (corroborated by
           the serial sentinel).
  * aa64 : no isa-debug-exit on the `virt` machine; key on the serial sentinel and a
           clean guest shutdown.

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
FW_NET = FW / "net"  # network-enabled firmware (docker/build-ovmf-net.sh)
ESP = ROOT / "esp"


def net_flags(arch: str) -> list[str]:
    """QEMU devices for a working UEFI network stack reaching SLIRP user-net.

    virtio-rng is MANDATORY: the EDK2 upper network stack (IP4/DHCP/TCP) has a DEPEX
    on gEfiRngProtocol, produced only by VirtioRngDxe — without it only SNP loads and
    ifconfig/IP4Config2 never appear. Slots are pinned on aa64 so the NIC/RNG don't
    take slot 0x2 and shift the ESP disk's device path (which would drop AAVMF into
    the Shell). SLIRP hands out 10.0.2.15 / gw 10.0.2.2 / DNS 10.0.2.3 deterministically.
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


NET_SERVER_PORT = 8888
NET_SERVER_PORT2 = 8889
# The guest reaches a host TLS server at 10.0.2.2 (SLIRP gateway -> host loopback);
# no hostfwd needed for guest->host. This is the TLS peer for the T-series.
TLS_SERVER_PORT = 8890
TLS_CERT = ROOT / "tests" / "tls_test_cert.pem"
TLS_KEY = ROOT / "tests" / "tls_test_key.pem"


def _ensure_tls_cert():
    """Generate the throwaway TLS test cert/key on demand (they're git-ignored).
    Normally `make stage` produces them first; this covers a direct harness run."""
    if not (TLS_CERT.exists() and TLS_KEY.exists()):
        subprocess.run(["bash", str(ROOT / "tests" / "gen-tls-test-cert.sh")], check=True)


def _tls_echo_server(port, deadline, result):
    """Host TLS echo peer: accept, handshake with the self-signed test cert, echo one
    payload. Runs until the guest completes an exchange or the deadline passes."""
    import socket as pysock
    import ssl as pyssl

    ctx = pyssl.SSLContext(pyssl.PROTOCOL_TLS_SERVER)
    ctx.load_cert_chain(str(TLS_CERT), str(TLS_KEY))
    srv = pysock.socket(pysock.AF_INET, pysock.SOCK_STREAM)
    srv.setsockopt(pysock.SOL_SOCKET, pysock.SO_REUSEADDR, 1)
    try:
        srv.bind(("127.0.0.1", port))
    except OSError as e:
        result["err"] = "bind: " + str(e)
        return
    srv.listen(5)
    srv.settimeout(1.0)
    # Serve every connection until the deadline: the guest opens several (T1 echo,
    # T2a verify-ok, T2b verify-reject). A rejected client aborts the handshake, so
    # our wrap_socket raises — expected; keep serving.
    result["served"] = 0
    while time.time() < deadline:
        try:
            conn, _ = srv.accept()
        except OSError:
            continue
        try:
            conn.settimeout(10.0)
            tconn = ctx.wrap_socket(conn, server_side=True)
            data = tconn.recv(64)
            if data:
                tconn.sendall(data)
                result["echo"] = data
                result["ok"] = True
                result["served"] += 1
            tconn.close()
        except OSError as e:
            result["err"] = str(e)  # e.g. the T2b client rejecting our cert
    srv.close()


def _net_server_probe(port, deadline, result, payload=b"HELLO-FROM-HOST"):
    """Host client for a guest server test: connect to the guest (via hostfwd),
    send a payload, and record the echo. Retries until the guest is listening."""
    import socket as pysock

    while time.time() < deadline:
        try:
            c = pysock.create_connection(("127.0.0.1", port), timeout=2)
        except OSError:
            time.sleep(0.3)
            continue
        try:
            # The guest binds its listener only when it reaches this test, so an early
            # connect can be RST (SLIRP can't reach a guest port yet) or read empty.
            # Keep retrying the whole exchange until we get the echo, or the deadline.
            c.settimeout(max(2.0, min(30.0, deadline - time.time())))
            c.sendall(payload)
            echo = c.recv(64)
            if echo:
                result["echo"] = echo
                result["ok"] = echo == payload
                return
        except OSError as e:
            result["err"] = str(e)
        finally:
            c.close()
        time.sleep(0.3)


X64_SUCCESS = 33  # (0x10 << 1) | 1
SENTINEL_PASS = "MPY-UEFI: SELFTEST PASS"  # printed by main.c on success


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
        # the boot disk's address — which would invalidate AAVMF's cached boot
        # option and drop us into the UEFI Shell instead of auto-launching.
        dev = ["-device", f"virtio-gpu-pci,edid=on,xres={xres},yres={yres},addr=0x9"]
        kbd = ["-device", "qemu-xhci,addr=0xa", "-device", "usb-kbd"]
    if mode == "window":
        backend = os.environ.get("QEMU_DISPLAY", "cocoa")
        return dev + kbd + ["-display", backend]
    return dev + kbd + ["-display", "none"]


def qemu_cmd(
    arch: str, serial: str = "file", gdb: bool = False, graphics: str = "off", net: bool = False
) -> list[str]:
    fw = FW_NET if net else FW  # network firmware for --net, lean default otherwise
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
    if net:
        cmd += net_flags(arch)
    cmd += ["-display", "none"] if graphics == "off" else graphics_flags(arch, graphics)
    # "file": serial captured to a log (selftest). "stdio": serial on the process
    # stdin/stdout so the REPL can be driven (repl mode). Monitor disabled either way.
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
    if gdb:
        cmd += ["-gdb", "tcp::1234"]
    return cmd


def _tail(text: str, n: int = 40) -> str:
    lines = text.splitlines()
    return "\n".join(lines[-n:])


def _print_tail(name: str, n: int = 40) -> None:
    p = FW / name
    if p.exists() and p.stat().st_size:
        print(f"\n===== {name} (last {n} lines; full file at {p}) =====")
        print(_tail(p.read_text(errors="replace"), n))


def run_selftest(arch: str, timeout: int = 60, graphics: str = "off", net: bool = False) -> int:
    FW.mkdir(exist_ok=True)
    (FW / "serial.log").write_text("")
    (FW / "debug.log").write_text("")
    probe: dict = {}
    probe2: dict = {}
    tls_probe: dict = {}
    if net:
        # Drive the guest's TCP servers from the host while QEMU runs: the blocking
        # accept (N6) on NET_SERVER_PORT, the asyncio.start_server (N6b) on ...PORT2.
        # Also run a host TLS echo peer the guest connects OUT to (T-series).
        _ensure_tls_cert()
        deadline = time.time() + timeout
        threading.Thread(
            target=_net_server_probe, args=(NET_SERVER_PORT, deadline, probe), daemon=True
        ).start()
        threading.Thread(
            target=_net_server_probe,
            args=(NET_SERVER_PORT2, deadline, probe2, b"ASYNC-FROM-HOST"),
            daemon=True,
        ).start()
        threading.Thread(
            target=_tls_echo_server, args=(TLS_SERVER_PORT, deadline, tls_probe), daemon=True
        ).start()
    try:
        proc = subprocess.run(qemu_cmd(arch, graphics=graphics, net=net), timeout=timeout)
        rc = proc.returncode
    except subprocess.TimeoutExpired:
        print(f"FAIL: QEMU timed out after {timeout}s")
        _print_tail("serial.log")
        return 1

    if net:
        print(f"host->guest server probe (N6):  {probe or 'no connection'}")
        print(f"host->guest server probe (N6b): {probe2 or 'no connection'}")
        print(f"guest->host TLS echo peer (T1): {tls_probe or 'no connection'}")
    log = (FW / "serial.log").read_text(errors="replace")
    sentinel = SENTINEL_PASS in log
    ok = (rc == X64_SUCCESS or sentinel) if arch == "x64" else sentinel

    if ok:
        print(f"PASS ({arch}, qemu exit {rc}, sentinel found)")
        return 0
    print(f"FAIL ({arch}, qemu exit {rc}, sentinel={'yes' if sentinel else 'no'})")
    _print_tail("serial.log")
    if arch == "x64":
        _print_tail("debug.log")
    return 1


# (input line, substring expected in the response) pairs driven over the live REPL.
# \x7f exercises the Delete key (TerminalDxe surfaces it as SCAN_DELETE): the two
# deletes must erase "zz", leaving "11 * 11" -> 121. If Delete didn't erase, the
# line would be "zz11 * 11" and raise NameError instead.
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


def main() -> int:
    # Docker hands us a non-blocking stdout pipe; force blocking so large or
    # bursty writes can't raise BlockingIOError mid-print.
    try:
        os.set_blocking(sys.stdout.fileno(), True)
    except (ValueError, OSError):
        pass
    ap = argparse.ArgumentParser()
    ap.add_argument("--arch", choices=["x64", "aa64"], default="aa64")
    ap.add_argument("--mode", choices=["selftest", "repl", "runfile"], default="selftest")
    ap.add_argument(
        "--graphics",
        choices=["off", "headless", "window"],
        default="off",
        help="attach a display adapter so GOP is published (default: off)",
    )
    ap.add_argument("--timeout", type=int, default=60)
    ap.add_argument(
        "--net",
        action="store_true",
        help="use the network firmware (firmware/net) + a virtio-net NIC on SLIRP",
    )
    ap.add_argument("path", nargs="?")
    a = ap.parse_args()
    if a.mode == "repl":
        return run_repl(a.arch, max(a.timeout, 90))
    if a.mode == "runfile":
        return run_runfile(a.arch, max(a.timeout, 60))
    # Network firmware boots slower and the self-test polls up to ~20s for a lease.
    return run_selftest(a.arch, max(a.timeout, 120) if a.net else a.timeout, a.graphics, net=a.net)


if __name__ == "__main__":
    sys.exit(main())
