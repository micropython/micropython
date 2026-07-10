#!/usr/bin/env python3
"""Run the upstream MicroPython test suite against the UEFI-in-QEMU port.

Target arch via UEFI_TEST_ARCH (aa64 default, or x64); run each on its native host.
Runs against the STANDARD build (`make ARCH=<arch> stage`) — no special serial-only
firmware. `tests/run-tests.py` drives the interpreter over the serial raw REPL (pyboard's
`exec:` transport: QEMU `-serial stdio` piped through pyboard's ProcessToSerial). Port-
specific wrinkles handled here:

  * The raw-REPL protocol needs a byte-clean channel, but ConOut/ConIn go through the
    firmware's TerminalDxe, which mangles control bytes. So the app is launched with a
    "--serial" argument (from startup.nsh), switching its REPL to the byte-clean
    EFI_SERIAL_IO transport. The standard build handles --serial natively.
  * The port has no MicroPython soft reset, so `pyboard`'s per-test Ctrl-D reset doesn't
    apply. Instead the firmware drops into the UEFI Shell and startup.nsh relaunches the app
    in an endless loop (see setup_shell_esp); `enter_raw_repl` is overridden to cleanly EXIT
    the app between tests (Ctrl-B out of raw REPL, then Ctrl-D), which the loop relaunches as
    a fresh, hermetic interpreter — no QEMU system_reset (faster, and it avoids a wedge that
    repeated resets hit). Needs RELEASE firmware.
  * unittest is placed on the boot volume (imported via sys.path's "") rather than frozen.
  * The target's serial RX is polled, but the port implements raw-paste flow control
    (per-window grants in pyexec.c), so the upload is metered device-side — no host pacing.

    python3 tests/run_uefi_tests.py                      # full default test set
    python3 tests/run_uefi_tests.py --subset             # quick dev slice
    python3 tests/run_uefi_tests.py basics/int1.py ...   # explicit test files
"""

import atexit
import os
import runpy
import shutil
import sys
import tempfile
import time

import pexpect

# Stream output live: a pipe (docker -> CI, not a TTY) is block-buffered by default, so
# per-test progress would otherwise be withheld until the whole run finishes. run-tests.py
# runs in-process (runpy) and shares these streams, so this covers its prints too.
try:
    sys.stdout.reconfigure(line_buffering=True)
    sys.stderr.reconfigure(line_buffering=True)
except (AttributeError, ValueError):
    pass

HERE = os.path.dirname(os.path.abspath(__file__))  # ports/uefi/tests
PORT = os.path.dirname(HERE)  # ports/uefi
TOP = os.path.dirname(os.path.dirname(PORT))  # repo root
sys.path.insert(0, os.path.join(TOP, "tools"))
sys.path.insert(0, os.path.join(TOP, "tests"))  # so run-tests.py can import test_utils
sys.path.insert(0, HERE)  # so we can reuse harness device helpers
import pyboard  # noqa: E402  (after sys.path insert)
from harness import graphics_flags, net_flags  # noqa: E402  (QEMU display/NIC device flags)

# The staged boot volume. Overridable (UEFI_TEST_ESP) so two arch runs (aa64 + x64) can
# go at once, each against its own copy — the bootstrap writes/removes setup_boot.py +
# startup.nsh here, which would race if the two runs shared one directory.
ESP = os.environ.get("UEFI_TEST_ESP", os.path.join(PORT, "esp"))
RELFW = os.path.join(PORT, "firmware", "release")

# Target architecture (UEFI_TEST_ARCH, default aa64). CI runs each on its OWN native
# host — aa64 on an arm64 runner, x64 on an x86-64 runner — so QEMU is same-ISA (fast),
# never cross-emulated. Per-arch: the QEMU binary + machine, the release firmware pair,
# and the removable-media app name the firmware auto-boots.
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
# CPU model. Default per arch (TCG); override with UEFI_TEST_CPU (e.g. "host" when
# accelerating with KVM, where each app (re)launch drops from ~seconds to ~ms).
QEMU_CPU = os.environ.get("UEFI_TEST_CPU", _A["cpu"] or "")
QEMU_MACHINE = _A["machine"] + (["-cpu", QEMU_CPU] if QEMU_CPU else [])
# Optional accelerator (e.g. UEFI_TEST_ACCEL="kvm" on a same-ISA host with /dev/kvm,
# or "hvf" on macOS) for native speed; default (empty) is TCG, which works everywhere.
QEMU_ACCEL = os.environ.get("UEFI_TEST_ACCEL", "")

# Emulated hardware. By default only pflash + the ESP disk + serial are set up (no display
# adapter, no virtio NIC / network firmware), so the GOP/Display and NIC-dependent network
# port-tests SKIP (the firmware publishes no GOP and enumerates no EFI network interface).
# `--graphics` attaches a display adapter WITHOUT a host window (`-display none` + a GPU device
# — GOP is published but nothing is drawn on screen, so it stays CI-safe), which makes the
# GOP/Display tests run. `--net` attaches a virtio NIC + RNG on SLIRP user-net and selects the
# network-enabled firmware, so the network tests run. Both pin their PCIe slots
# (harness.graphics_flags / net_flags) so adding a device doesn't shuffle PCIe enumeration and
# move the ESP off FS0: (which startup.nsh selects to launch the app).
GFX_MODE = None  # None | "headless" (set by --graphics)
USE_NET = False  # set by --net
# Firmware: lean RELEASE by default (fast, quiet serial); the network firmware
# (release-net: RELEASE + the full net/TLS stack) is selected by --net.
RELNETFW = os.path.join(PORT, "firmware", "release-net")
CODE = os.path.join(RELFW, f"{_A['fw']}_CODE.fd")
VARS_SRC = os.path.join(RELFW, f"{_A['fw']}_VARS.fd")
EFI = _A["efi"]
BANNER = b"MicroPython on UEFI"
# unittest lives on the boot volume (importable via the "" cwd entry on sys.path) rather
# than being frozen into a special build — so the tested firmware is the standard build.
UNITTEST_SRC = os.path.join(
    TOP, "lib", "micropython-lib", "python-stdlib", "unittest", "unittest", "__init__.py"
)

# A small subset handy while developing the runner: `python3 run_uefi_tests.py --subset`.
DEV_SUBSET = [
    "basics/int1.py",
    "basics/for_break.py",
    "basics/builtin_range.py",
    "basics/string1.py",
    "basics/list1.py",
    "basics/dict1.py",
    "basics/fun1.py",
    "basics/class1.py",
    "float/float1.py",
]

# The app's clean-exit line — the resync marker between tests (see _relaunch_app).
_EXIT_MARKER = b"MPY-UEFI: exit"

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

# QMP monitor socket: used only for report_accel() (query-kvm). The shell-loop harness does
# NOT reset the VM between tests (it relaunches the app from the EFI shell instead), so there
# is no monitor_reset — which also sidesteps a QEMU wedge that repeated system_resets hit.
MONSOCK = os.path.join(tempfile.gettempdir(), f"uefi_mon_{os.getpid()}.sock")


def _qemu(varsfile, serial, no_reboot, monitor=False):
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
        serial,
    ]
    # Emulated hardware (same for the bootstrap and the test run, so the ESP stays on FS0:).
    # A default NIC is present even off --net (QEMU adds one); the bootstrap makes the UEFI
    # Shell the first boot option (Timeout=0) so OVMF never falls through to PXE regardless.
    if USE_NET:
        cmd += net_flags(ARCH)
    cmd += graphics_flags(ARCH, GFX_MODE) if GFX_MODE else ["-display", "none"]
    # QMP (not HMP) on the socket — used only by report_accel (query-kvm). HMP stays disabled.
    cmd += ["-qmp", f"unix:{MONSOCK},server,nowait"] if monitor else []
    cmd += ["-monitor", "none"]
    if no_reboot:
        cmd.append("-no-reboot")
    return cmd


def _qmp(*commands):
    """Open the QMP socket, enter command mode, run each command, return the raw replies.
    We use QMP (the JSON machine protocol), not HMP: HMP over a socket runs interactive
    readline (echo + escape sequences) that mangles a bare command on some QEMU builds."""
    import socket

    s = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
    s.connect(MONSOCK)
    s.settimeout(10)
    f = s.makefile("rwb")
    f.readline()  # QMP greeting: {"QMP": {...}}
    f.write(b'{"execute":"qmp_capabilities"}\n')
    f.flush()
    f.readline()  # {"return": {}}
    replies = []
    for c in commands:
        f.write(c + b"\n")
        f.flush()
        replies.append(f.readline())
    f.close()
    s.close()
    return replies


def report_accel():
    """Log whether QEMU is *actually* using KVM (query-kvm), so a run can be confirmed
    hardware-accelerated explicitly — not merely inferred from the wall-clock time."""
    try:
        (reply,) = _qmp(b'{"execute":"query-kvm"}')
        sys.stderr.write("runner: query-kvm -> %s\n" % reply.decode(errors="replace").strip())
    except Exception as e:  # noqa: BLE001
        sys.stderr.write("runner: query-kvm failed: %r\n" % e)


def setup_shell_esp():
    """Bootstrap a VARS snapshot whose BootOrder puts the firmware's built-in UEFI Shell FIRST
    (Timeout=0), and stage the endless-relaunch startup.nsh. Returns the VARS file.

    Booting the Shell first is what keeps every cold boot off the removable-media/PXE fallback:
    with a NIC present (QEMU adds one even off --net), OVMF otherwise falls through to PXE
    ("Start PXE over IPv4"), which stalls so the Shell is never reached. The Shell runs
    startup.nsh, which relaunches the app in a loop; each test 'reset' is a clean app exit that
    the loop relaunches — a fresh, hermetic interpreter with NO QEMU system_reset (faster, and
    the cure for the reset wedge).

    Firmware-agnostic bootstrap (the app must run once to write the boot variables): AAVMF drops
    into the Shell and runs startup.nsh (app + setup arg -> SETUP-OK); OVMF auto-boots the
    removable app at \\EFI\\BOOT\\<EFI> to its REPL with no args (that removable boot SUCCEEDS,
    so this first boot also avoids PXE), which we then drive to run the setup. Both reorder
    BootOrder to the Shell for every subsequent boot."""
    app = "\\EFI\\BOOT\\" + EFI  # stays at the removable-media path; the Shell launches it
    with open(os.path.join(ESP, "setup_boot.py"), "w") as f:
        f.write(_SETUP_SRC)
    nsh = os.path.join(ESP, "startup.nsh")
    with open(nsh, "w") as f:  # bootstrap phase: run the app once with the setup script
        f.write("@echo -off\r\nFS0:\r\n%s setup_boot.py\r\n" % app)
    tv = tempfile.NamedTemporaryFile(suffix="_VARS.fd", delete=False)
    tv.close()
    shutil.copy(VARS_SRC, tv.name)
    cmd = _qemu(tv.name, "stdio", no_reboot=True)
    boot_timeout = int(
        os.environ.get("UEFI_TEST_BOOT_TIMEOUT", "240" if (USE_NET or ARCH != "aa64") else "60")
    )
    child = pexpect.spawn(cmd[0], cmd[1:], timeout=boot_timeout, encoding="latin-1")
    try:
        # AAVMF: Shell runs startup.nsh (app + setup arg) -> SETUP-OK. OVMF: auto-boots the
        # removable app to its REPL (no args) -> drive it to run the setup script.
        if child.expect(["SETUP-OK", ">>> "]) == 1:
            child.send("exec(open('/setup_boot.py').read())\r")
            child.expect("SETUP-OK", timeout=30)
    finally:
        if child.isalive():
            child.terminate(force=True)
        os.remove(os.path.join(ESP, "setup_boot.py"))
    # Test phase: the Shell (now first in BootOrder) runs this endless relaunch loop. `--serial`
    # switches the app's REPL to the byte-clean serial transport; @echo -off keeps the loop
    # quiet (the app's own banner/exit lines are the resync markers).
    with open(nsh, "w") as f:
        f.write("@echo -off\r\nFS0:\r\n:loop\r\n%s --serial\r\ngoto loop\r\n" % app)
    return tv.name


# Stop *before* the trailing '>' — pyboard's exec_raw_no_follow consumes that itself.
_RAW_PROMPT = b"raw REPL; CTRL-B to exit\r\n"


_QEMU_CMD = None  # set by main(): the QEMU command line (without pyboard's "exec:" prefix)


# Track every QEMU we spawn so we can guarantee it dies with us. pyboard's ProcessToSerial
# does not redirect QEMU's stderr, so a QEMU left running after this process exits keeps the
# inherited stderr pipe open — which hangs whatever is reading our output (a CI step, a
# `| tail`, a watcher) until it times out. run-tests closes the pyboard on a clean finish, but
# NOT on an unhandled exception; this atexit hook covers that gap so a crash fails fast instead
# of hanging.
_QEMU_PROCS = []
_BasePTS = pyboard.ProcessToSerial
# Optional: tee everything the device sends to a file, so a boot/relaunch failure on a target
# we can't attach to (e.g. x64 under KVM in CI) is diagnosable after the fact.
_SERIAL_LOG = os.environ.get("UEFI_TEST_SERIAL_LOG")


class _TrackedProcessToSerial(_BasePTS):
    def __init__(self, cmd):
        _BasePTS.__init__(self, cmd)
        _QEMU_PROCS.append(self)

    def read(self, size=1):
        d = _BasePTS.read(self, size)
        if _SERIAL_LOG and d:
            with open(_SERIAL_LOG, "ab") as f:
                f.write(d)
        return d


def _kill_all_qemu():
    for p in _QEMU_PROCS:
        try:
            p.close()  # ProcessToSerial.close() SIGTERMs the process group
        except Exception:  # noqa: BLE001
            pass


def _relaunch_qemu(self):
    """Self-heal fallback: kill QEMU and spawn a fresh one. The shell-loop should never wedge
    (no system_resets), but if a relaunch ever fails to produce a banner this recovers the run
    from a completely fresh VM rather than failing every subsequent test."""
    try:
        self.serial.close()  # ProcessToSerial.close() SIGTERMs the process group
    except Exception:  # noqa: BLE001
        pass
    time.sleep(0.5)  # let the old process release the QMP socket before the new one rebinds
    self.serial = pyboard.ProcessToSerial(_QEMU_CMD)


def _relaunch_app(self, boot_to):
    """End the current test by cleanly exiting the app; the EFI shell's startup.nsh loop then
    relaunches it as a fresh, hermetic interpreter — no QEMU reset. From raw REPL: Ctrl-B
    returns to the friendly REPL, then Ctrl-D on the empty line forces the clean exit."""
    self.serial.write(b"\x02")  # Ctrl-B: raw REPL -> friendly REPL
    self.read_until(1, b">>> ", timeout_overall=boot_to)
    self.serial.write(b"\x04")  # Ctrl-D on an empty line: forced clean exit -> back to shell
    self.read_until(1, _EXIT_MARKER, timeout_overall=boot_to)  # wait for the app's exit line


def _uefi_enter_raw_repl(self, soft_reset=True, timeout_overall=10):
    """Relaunch-as-reset: this port has no MicroPython soft reboot, so each per-test 'reset' is
    a fresh launch of the app. On the first call the app is just booting; on later calls we
    cleanly exit the app (which the shell's startup.nsh loop immediately relaunches) and wait
    for the fresh banner. No QEMU system_reset — far faster, and it avoids the reset wedge."""
    # Use pyboard's raw-paste upload path (default): the port implements raw-paste flow
    # control (pyexec.c), so the device meters the host with per-window grants — reliable
    # uploads with no open-loop host pacing.
    first_call = not getattr(self, "_uefi_booted", False)
    # The FIRST boot is a full cold start (firmware init + the UEFI Shell's ~5 s startup.nsh
    # countdown + the app boot), so give it a generous budget; a relaunch is just the shell
    # loop re-exec'ing the resident app, so 30 s is plenty there.
    if first_call:
        boot_to = int(os.environ.get("UEFI_TEST_BOOT_TIMEOUT", "240" if ARCH != "aa64" else "60"))
    else:
        boot_to = max(timeout_overall, 30)
    if not first_call:
        _relaunch_app(self, boot_to)
    if not self.read_until(1, BANNER, timeout_overall=boot_to).endswith(BANNER):
        # Self-heal: an exit/relaunch was somehow lost. A fresh QEMU always recovers, and the
        # shell relaunches the app cold.
        if not first_call:
            _relaunch_qemu(self)
        if not self.read_until(1, BANNER, timeout_overall=boot_to).endswith(BANNER):
            raise pyboard.PyboardError("uefi: interpreter banner not seen after (re)launch")
    self.read_until(1, b">>> ", timeout_overall=boot_to)
    self._uefi_booted = True
    # Fresh interpreter -> enter raw REPL (no Ctrl-D soft reset; this port has none).
    self.serial.write(b"\r\x01")
    if not self.read_until(1, _RAW_PROMPT, timeout_overall=timeout_overall).endswith(_RAW_PROMPT):
        raise pyboard.PyboardError("uefi: could not enter raw repl")
    self.in_raw_repl = True
    if first_call:
        report_accel()  # log KVM-vs-TCG once, from QEMU itself


# Wall-clock cap for one test's output. pyboard.follow only applies an *inter-character*
# timeout, so a test that spins while printing (e.g. a poll loop on an fd our port can't
# support) streams forever and never trips it. This bounds total time per output phase; on
# expiry follow raises, the test is marked failed, and the next test's enter_raw_repl relaunches
# the app. Scale up (UEFI_TEST_FOLLOW) for a slow cross-emulated target, where a heavy-output
# test (e.g. float/math_fun) would otherwise be truncated before it finishes.
_FOLLOW_OVERALL = int(os.environ.get("UEFI_TEST_FOLLOW", "60"))


def _uefi_follow(self, timeout, data_consumer=None):
    data = self.read_until(
        1, b"\x04", timeout=timeout, data_consumer=data_consumer, timeout_overall=_FOLLOW_OVERALL
    )
    if not data.endswith(b"\x04"):
        raise pyboard.PyboardError("timeout waiting for first EOF reception")
    data = data[:-1]
    data_err = self.read_until(1, b"\x04", timeout=timeout, timeout_overall=_FOLLOW_OVERALL)
    if not data_err.endswith(b"\x04"):
        raise pyboard.PyboardError("timeout waiting for second EOF reception")
    return data, data_err[:-1]


def main():
    global GFX_MODE, USE_NET, CODE, VARS_SRC, _QEMU_CMD
    # Runner-level flags (consumed here; the rest pass through to run-tests.py). `--graphics`
    # attaches a headless display adapter (GOP published, nothing drawn); `--net` attaches a
    # NIC and switches to the network firmware.
    args = sys.argv[1:]
    if "--graphics" in args:
        args.remove("--graphics")
        GFX_MODE = "headless"
    if "--net" in args:
        args.remove("--net")
        USE_NET = True
        CODE = os.path.join(RELNETFW, f"{_A['fw']}_CODE.fd")
        VARS_SRC = os.path.join(RELNETFW, f"{_A['fw']}_VARS.fd")
        if not os.path.exists(CODE):
            raise SystemExit(
                f"missing {CODE} — build the network firmware first "
                f"(docker/build-ovmf-release-net.sh)"
            )
    for f in (CODE, VARS_SRC, os.path.join(ESP, "EFI", "BOOT", EFI)):
        if not os.path.exists(f):
            raise SystemExit(
                f"missing {f} — build firmware (docker/build-ovmf-release.sh) "
                f"and stage the app (make ARCH={ARCH} stage)"
            )
    # Put unittest on the boot volume so the standard build imports it via sys.path's "".
    os.makedirs(os.path.join(ESP, "unittest"), exist_ok=True)
    shutil.copy(UNITTEST_SRC, os.path.join(ESP, "unittest", "__init__.py"))
    # No args -> let run-tests.py discover its full default set (basics, micropython, misc,
    # extmod, stress, float). `--subset` runs a quick dev slice; otherwise the args are
    # passed through verbatim (explicit files, or run-tests flags like -d).
    if args == ["--subset"]:
        args = DEV_SUBSET

    pyboard.Pyboard.enter_raw_repl = _uefi_enter_raw_repl
    pyboard.Pyboard.follow = _uefi_follow
    # Guarantee every QEMU we spawn is killed on exit (incl. an unhandled exception), so a
    # crash can't leave an orphaned QEMU holding our output pipe open and hanging the caller.
    pyboard.ProcessToSerial = _TrackedProcessToSerial
    atexit.register(_kill_all_qemu)
    tv = setup_shell_esp()
    # `-serial stdio` + pyboard's `exec:` (ProcessToSerial, stdin/stdout pipes). _QEMU_CMD is
    # stashed so the self-heal fallback (_relaunch_qemu) can respawn an identical QEMU. No
    # `-no-reboot`: harmless here since nothing triggers a machine reset (the app relaunch is
    # a shell-driven re-exec, not a QEMU reset).
    _QEMU_CMD = " ".join(_qemu(tv, "stdio", no_reboot=False, monitor=True))

    os.chdir(os.path.join(TOP, "tests"))
    sys.argv = ["run-tests.py", "-t", "exec:" + _QEMU_CMD, *args]
    try:
        runpy.run_path(os.path.join(TOP, "tests", "run-tests.py"), run_name="__main__")
    finally:
        for path in (tv, MONSOCK):
            try:
                os.unlink(path)
            except OSError:
                pass


if __name__ == "__main__":
    main()
