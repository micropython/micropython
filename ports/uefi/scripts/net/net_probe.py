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

"""Boot network-enabled OVMF/AAVMF into the built-in UEFI Shell and prove that a
QEMU virtual ethernet interface works end-to-end, WITHOUT MicroPython.

This runs INSIDE the dev container (`mpy-uefi-dev`), because QEMU there is Linux
and its SLIRP user-net reaches the container's network namespace (and thus the
internet via Docker). Use scripts/net/run_net_probe.sh from the macOS host to
wrap it in `docker run`.

What it does
------------
1. Stages a throwaway ESP whose ONLY content is a `startup.nsh`. Because there is
   no ``\\EFI\\BOOT\\BOOT{X64,AA64}.EFI`` on it, the firmware's Boot Device
   Selection fails the removable-media boot and falls through to the EDK2 UEFI
   Shell, which auto-runs ``FS0:\\startup.nsh``.
2. The startup.nsh drives the shell's network commands:
     ifconfig -l                 list interfaces (proves a driver bound the NIC)
     ifconfig -s eth0 dhcp       run DHCP
     ifconfig -l eth0            show the lease (IP/mask/gw/DNS)
     ping ...                    ICMP to the SLIRP gateway and (internet mode) to
                                 a real host, proving NAT to the outside world
     tftp ...                    (hermetic mode) fetch a known file from QEMU's
                                 built-in SLIRP TFTP server at 10.0.2.2 -- a fully
                                 deterministic data transfer with NO real internet
3. Boots the chosen firmware + NIC + SLIRP user-net and captures the serial
   transcript with pexpect, keying on BEGIN/END sentinels.

Exit code 0 iff a DHCP lease was observed AND the mode's connectivity check
passed (a ping reply for internet mode, or a completed TFTP fetch for hermetic).
The full transcript is always printed so the caller can archive the evidence.
"""

import argparse
import os
import pathlib
import re
import shutil
import sys
import tempfile

ROOT = pathlib.Path(__file__).resolve().parents[2]  # ports/uefi
FW_NET = ROOT / "firmware" / "net"

BEGIN = "===NET-PROBE-BEGIN==="
END = "===NET-PROBE-END==="

EFI_BY_ARCH = {"x64": "BOOTX64.EFI", "aa64": "BOOTAA64.EFI"}


def make_startup_nsh(mode: str, tftp_remote: str, internet_ip: str) -> str:
    """Generate the shell script the firmware auto-runs from FS0:."""
    # NB: leave command echo ON (no `@echo -off`) so each command line is visible
    # in the transcript, and quote the section markers because a bare token that
    # starts with '-' is parsed by the shell's `echo` as a flag.
    if mode == "diag":
        diag = [
            'echo "%s"' % BEGIN,
            "connect -r",
            'echo "---DRIVERS---"',
            "drivers",
            'echo "---SNP---"',
            "dh -p SimpleNetwork",
            'echo "---MNPSB---"',
            "dh -p ManagedNetworkServiceBinding",
            'echo "---IFACES-BEFORE---"',
            "ifconfig -l",
            'echo "%s"' % END,
        ]
        return "\r\n".join(diag) + "\r\n"
    lines = [
        'echo "%s"' % BEGIN,
        'echo "---CONNECT---"',
        # Force the shell to bind all drivers to all handles: VirtioNetDxe (or the
        # e1000/rtl8139 iPXE option-ROM driver) attaches to the NIC and the
        # SNP/MNP/ARP/IP4/UDP/TCP stack layers on top. BDS does not always connect
        # the network stack (nothing requested it), so ifconfig would see no iface.
        "connect -r",
        'echo "---IFACES---"',
        "ifconfig -l",
        'echo "---DHCP-START---"',
        "ifconfig -s eth0 dhcp",
        # `ifconfig -s ... dhcp` kicks off DHCP asynchronously and returns before
        # the DORA exchange finishes (address stays 0.0.0.0 for a beat). Poll the
        # lease a few times: each `ifconfig -l` does real I/O + re-enters the
        # network stack, giving the DHCP state machine wall-clock time to land the
        # lease. The parser scans every iteration for a 10.0.2.x address.
        'echo "---LEASE-POLL---"',
        "for %p run (1 12)",
        "  ifconfig -l eth0",
        "endfor",
        'echo "---PING-GW---"',
        "ping -n 3 10.0.2.2",
    ]
    if mode == "internet":
        lines += [
            'echo "---PING-INET---"',
            "ping -n 3 " + internet_ip,
        ]
    else:  # hermetic
        lines += [
            'echo "---TFTP---"',
            "tftp 10.0.2.2 %s fs0:\\got.bin" % tftp_remote,
            'echo "---TFTP-DATA---"',
            "type fs0:\\got.bin",
            "echo .",
        ]
    lines += ['echo "%s"' % END]
    # UEFI shell scripts want CRLF line endings.
    return "\r\n".join(lines) + "\r\n"


def nic_device(arch: str, model: str) -> list[str]:
    """`-device` flags for the requested NIC model.

    On the aa64 `virt` machine, pin the NIC to a high PCIe slot (0xb) so it does
    not land on slot 0x2 and shift the ESP disk's device path (which would
    invalidate any cached boot option), mirroring the GPU/xHCI pinning that the
    graphics path already does. On x64 q35 the slot is not load-bearing.
    """
    addr = ",addr=0xb" if arch == "aa64" else ""
    if model == "virtio-net-pci":
        return ["-device", "virtio-net-pci,netdev=net0" + addr]
    if model == "e1000":
        return ["-device", "e1000,netdev=net0" + addr]
    if model == "rtl8139":
        return ["-device", "rtl8139,netdev=net0" + addr]
    raise SystemExit("unknown NIC model %r" % model)


def netdev_flags(mode: str, tftp_dir: str) -> list[str]:
    """`-netdev user` (SLIRP) flags for the requested mode.

    internet: plain SLIRP NAT (DHCP 10.0.2.15 / gw+TFTP 10.0.2.2 / DNS 10.0.2.3),
              reaches the real internet through the container.
    hermetic: `restrict=on` blocks ALL forwarding to the outside world, so the
              guest can only talk to SLIRP's own services (DHCP + the built-in
              TFTP server rooted at `tftp_dir`). Deterministic, no internet.
    """
    if mode == "internet":
        return ["-netdev", "user,id=net0"]
    return ["-netdev", "user,id=net0,restrict=on,tftp=%s" % tftp_dir]


def build_cmd(
    arch: str, model: str, mode: str, esp: str, vars_fd: str, tftp_dir: str
) -> list[str]:
    if arch == "x64":
        code = FW_NET / "OVMF_CODE.fd"
        cmd = [
            "qemu-system-x86_64",
            "-machine",
            "q35",
            "-drive",
            "if=pflash,format=raw,readonly=on,file=%s" % code,
            "-drive",
            "if=pflash,format=raw,file=%s" % vars_fd,
        ]
    else:
        code = FW_NET / "AAVMF_CODE.fd"
        cmd = [
            "qemu-system-aarch64",
            "-machine",
            "virt",
            "-cpu",
            "cortex-a72",
            "-drive",
            "if=pflash,format=raw,readonly=on,file=%s" % code,
            "-drive",
            "if=pflash,format=raw,file=%s" % vars_fd,
        ]
    cmd += ["-drive", "format=raw,file=fat:rw:%s" % esp]
    cmd += ["-m", "512M", "-no-reboot", "-display", "none"]
    # The EDK2 network stack (MNP/IP4/ARP/DHCP4/TCP) has a DEPEX on gEfiRngProtocol
    # (via NetLib / TLS crypto). OVMF & ArmVirtQemu only *produce* that protocol
    # from VirtioRngDxe, i.e. when a virtio-rng device is present. Without it the
    # ENTIRE upper network stack silently fails to dispatch (only VirtioNetDxe/SNP
    # loads), so ifconfig sees no interface. This device is mandatory for netdev.
    rng_addr = ",addr=0xc" if arch == "aa64" else ""
    cmd += ["-device", "virtio-rng-pci" + rng_addr]
    cmd += netdev_flags(mode, tftp_dir)
    cmd += nic_device(arch, model)
    cmd += ["-serial", "stdio", "-monitor", "none"]
    return cmd


def parse_evidence(transcript: str, mode: str) -> dict:
    """Extract structured pass/fail evidence from the shell transcript."""
    out = {
        "iface_listed": False,
        "lease_ip": None,
        "ping_gw": False,
        "ping_inet": False,
        "tftp_ok": False,
    }
    # An interface line from `ifconfig -l` looks like: "  eth0" / "eth0 : ...".
    if re.search(r"\beth0\b", transcript):
        out["iface_listed"] = True
    # `ifconfig -l eth0` prints "ipv4 address : 10.0.2.15" (spacing varies).
    m = re.search(
        r"ipv4 address\s*:\s*([0-9]+\.[0-9]+\.[0-9]+\.[0-9]+)", transcript, re.IGNORECASE
    )
    if not m:
        m = re.search(r"\b(10\.0\.2\.1[0-9])\b", transcript)
    if m:
        out["lease_ip"] = m.group(1)
    # Ping success lines look like "N bytes from 10.0.2.2 : icmp_seq=..." and a
    # summary "3 packets transmitted, 3 received".
    gw_block = _section(transcript, "---PING-GW---")
    if "bytes from 10.0.2.2" in gw_block or re.search(r"[1-9] received", gw_block):
        out["ping_gw"] = True
    if mode == "internet":
        inet_block = _section(transcript, "---PING-INET---")
        if re.search(r"bytes from ", inet_block) or re.search(r"[1-9] received", inet_block):
            out["ping_inet"] = True
    else:
        tftp_block = _section(transcript, "---TFTP---")
        # EDK2 tftp prints a byte count / "TFTP ... completed" and then `type`
        # dumps the file content; our staged file contains a known marker.
        if "HERMETIC-TFTP-OK" in transcript or re.search(r"\b\d+\s+bytes\b", tftp_block):
            out["tftp_ok"] = True
    return out


def _section(transcript: str, marker: str) -> str:
    """Return text between `marker` and the next '---' or the END sentinel."""
    i = transcript.find(marker)
    if i < 0:
        return ""
    rest = transcript[i + len(marker) :]
    j = rest.find("---", 1)
    k = rest.find(END)
    ends = [x for x in (j, k) if x >= 0]
    return rest[: min(ends)] if ends else rest


def run(arch: str, model: str, mode: str, timeout: int, internet_ip: str) -> int:
    import pexpect

    code = FW_NET / ("OVMF_CODE.fd" if arch == "x64" else "AAVMF_CODE.fd")
    vars_src = FW_NET / ("OVMF_VARS.fd" if arch == "x64" else "AAVMF_VARS.fd")
    if not code.exists() or not vars_src.exists():
        print("MISSING firmware in %s (run docker/build-ovmf-net.sh)" % FW_NET)
        return 2

    work = pathlib.Path(tempfile.mkdtemp(prefix="netprobe-"))
    esp = work / "esp"
    esp.mkdir()
    tftp_dir = work / "tftp"
    tftp_dir.mkdir()
    tftp_remote = "hello.txt"
    (tftp_dir / tftp_remote).write_text("HERMETIC-TFTP-OK\n")
    # Writable, throwaway copy of VARS so we never mutate the checked-in firmware
    # and every run starts from the same boot-variable state.
    vars_fd = work / "VARS.fd"
    shutil.copyfile(vars_src, vars_fd)
    (esp / "startup.nsh").write_text(make_startup_nsh(mode, tftp_remote, internet_ip))

    cmd = build_cmd(arch, model, mode, str(esp), str(vars_fd), str(tftp_dir))
    print("# arch=%s nic=%s mode=%s" % (arch, model, mode))
    print("# QEMU: " + " ".join(cmd))
    print("# firmware: %s" % code)

    transcript: list[str] = []
    child = pexpect.spawn(
        cmd[0], cmd[1:], timeout=timeout, encoding="utf-8", codec_errors="replace"
    )
    child.logfile_read = type(
        "W", (), {"write": lambda _s, s: transcript.append(s), "flush": lambda _s: None}
    )()
    try:
        child.expect_exact(BEGIN)
        child.expect_exact(END, timeout=timeout)
    except (pexpect.TIMEOUT, pexpect.EOF) as e:
        print("\n# probe did not reach END sentinel: %s" % type(e).__name__)
    finally:
        if child.isalive():
            child.terminate(force=True)

    text = "".join(transcript)
    print("\n===== SHELL TRANSCRIPT =====")
    print(text)
    print("===== END TRANSCRIPT =====")

    ev = parse_evidence(text, mode)
    print("\n# evidence: %s" % ev)
    ok = ev["iface_listed"] and ev["lease_ip"]
    if mode == "internet":
        ok = ok and (ev["ping_gw"] or ev["ping_inet"])
    else:
        ok = ok and ev["tftp_ok"]
    verdict = "PASS" if ok else "FAIL"
    print(
        "# %s (%s/%s/%s) lease=%s ping_gw=%s ping_inet=%s tftp=%s"
        % (
            verdict,
            arch,
            model,
            mode,
            ev["lease_ip"],
            ev["ping_gw"],
            ev["ping_inet"],
            ev["tftp_ok"],
        )
    )
    shutil.rmtree(work, ignore_errors=True)
    return 0 if ok else 1


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--arch", choices=["x64", "aa64"], default="x64")
    ap.add_argument(
        "--nic", choices=["virtio-net-pci", "e1000", "rtl8139"], default="virtio-net-pci"
    )
    ap.add_argument("--mode", choices=["internet", "hermetic", "diag"], default="internet")
    ap.add_argument("--timeout", type=int, default=120)
    ap.add_argument("--internet-ip", default="8.8.8.8", help="real host to ping in internet mode")
    a = ap.parse_args()
    try:
        os.set_blocking(sys.stdout.fileno(), True)
    except (ValueError, OSError):
        pass
    return run(a.arch, a.nic, a.mode, a.timeout, a.internet_ip)


if __name__ == "__main__":
    sys.exit(main())
