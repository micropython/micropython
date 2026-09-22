# SPDX-License-Identifier: MIT
# Copyright (c) 2026 Nicko van Someren

# net_boot.py — a network boot loader.
#   exec(open('/samples/net_boot.py').read())
# Needs networking — launch with `make run`.
#
# Downloads a Linux kernel (an EFI-stub vmlinuz) and an initramfs over HTTPS into
# memory, then boots the kernel via the UEFI LoadImage/StartImage mechanism. The
# initramfs is handed to the kernel entirely from RAM using EFI_LOAD_FILE2_PROTOCOL
# (uefi.load_file) — no disk involved. Content-Length is read before the body so we
# can size the buffer and draw a progress bar.

import time
import socket
import tls
import network
import uctypes
import uefi
from uefi import load_file
from uefi.buffer import PoolBuffer, LOADER_DATA

# NOTE: an EFI-stub kernel is a PE image for a specific architecture. LoadImage
# rejects a mismatch ("Image type X64 can't be loaded on AARCH64 UEFI system"), so
# the kernel below (x86-64) boots on the x64 firmware; use an aarch64 vmlinuz on aa64.
KERNEL_URL = "https://notlo.st/boot-images/vmlinuz-stock.efi"
INITRD_URL = "https://notlo.st/boot-images/initramfs-stock.img"
# Serial on both the aa64 'virt' (ttyAMA0) and x64 q35 (ttyS0) QEMU machines.
CMDLINE = "console=ttyAMA0 console=ttyS0"


def _split_url(url):
    if not url.startswith("https://"):
        raise ValueError("only https:// is supported")
    rest = url[len("https://") :]
    slash = rest.find("/")
    if slash < 0:
        return rest, "/"
    return rest[:slash], rest[slash:]


def _bar(name, done, total):
    width = 30
    frac = (done / total) if total else 0
    filled = int(frac * width)
    print(
        "\r  %-8s [%s%s] %3d%%  %d/%d KiB"
        % (
            name,
            "#" * filled,
            "-" * (width - filled),
            int(frac * 100),
            done // 1024,
            total // 1024,
        ),
        end="",
    )


def download(url, name):
    """Fetch `url` over TLS into a pool buffer; return the PoolBuffer (caller frees)."""
    host, path = _split_url(url)
    addr = socket.getaddrinfo(host, 443)[0][-1]
    s = socket.socket()
    s.connect(addr)
    # No CA bundle in firmware -> we don't verify the chain here (CERT_NONE); SNI is
    # still sent. Provide a CA and CERT_REQUIRED to verify for real.
    ctx = tls.SSLContext(tls.PROTOCOL_TLS_CLIENT)
    ctx.verify_mode = tls.CERT_NONE
    ss = ctx.wrap_socket(s, server_hostname=host)
    ss.write(
        b"GET "
        + path.encode()
        + b" HTTP/1.1\r\nHost: "
        + host.encode()
        + b"\r\nConnection: close\r\n\r\n"
    )

    # Pull headers first, so Content-Length is known before the bulk body.
    hdr = b""
    while b"\r\n\r\n" not in hdr:
        chunk = ss.read(256)
        if not chunk:
            raise OSError("connection closed during headers")
        hdr += chunk
    head, _, body = hdr.partition(b"\r\n\r\n")
    lines = head.split(b"\r\n")
    status_line = lines[0].decode()
    if " 200 " not in status_line:
        raise OSError("HTTP " + status_line)
    length = None
    for ln in lines[1:]:
        if ln.lower().startswith(b"content-length:"):
            length = int(ln.split(b":", 1)[1].strip())
            break
    if length is None:
        raise OSError("no Content-Length header")
    print("  %s: %d bytes" % (name, length))

    buf = PoolBuffer(length, memory_type=LOADER_DATA)
    got = 0
    # Body bytes already read alongside the headers (a small tail; copy directly).
    if body:
        n = min(len(body), length)
        dst = uctypes.bytearray_at(buf.ptr, n)
        for i in range(n):
            dst[i] = body[i]
        got = n
    # Bulk body: read straight into the pool buffer via readinto (a C-level copy —
    # no intermediate bytes, no per-byte Python loop), which matters at ~100 MB.
    # The progress bar is throttled to ~500 ms so it doesn't flood the serial console.
    last = time.ticks_ms()
    while got < length:
        want = min(65536, length - got)
        view = uctypes.bytearray_at(buf.ptr + got, want)
        n = ss.readinto(view)
        if not n:
            break
        got += n
        if time.ticks_diff(time.ticks_ms(), last) >= 500:
            _bar(name, got, length)
            last = time.ticks_ms()
    _bar(name, got, length)  # final 100%
    print()
    ss.close()
    if got != length:
        buf.close()
        raise OSError("short read: %d of %d bytes" % (got, length))
    return buf


def main():
    nic = network.LAN()
    nic.active(True)
    nic.ipconfig(dhcp4=True)
    t0 = time.ticks_ms()
    while not nic.isconnected() and time.ticks_diff(time.ticks_ms(), t0) < 15000:
        time.sleep_ms(200)
    if not nic.isconnected():
        raise OSError("no network — see net_info.py")
    # SLIRP's DNS can't forward inside the dev container; use a public resolver.
    network.ipconfig(dns="8.8.8.8")

    print("Downloading kernel...")
    kernel = download(KERNEL_URL, "kernel")
    print("Downloading initramfs...")
    initrd = download(INITRD_URL, "initrd")

    # Present the initramfs from RAM; the kernel's EFI stub pulls it via LoadFile2.
    # Must stay alive across StartImage (which is why it isn't a 'with' block).
    provider = load_file.present_initrd(initrd)

    # Load the kernel from its buffer (memory-to-memory), attach the command line,
    # and start it. For a kernel that boots, StartImage does not return — the stub
    # calls ExitBootServices itself.
    st, img_handle = uefi.raw.load_image(
        False, uefi.raw.image_handle(), 0, kernel.ptr, kernel.size
    )
    uefi.status.check(st)
    image = uefi.Image(img_handle)
    image.set_load_options(CMDLINE)

    print("Booting Linux (%s)..." % CMDLINE)
    exit_status, _ = image.start()

    # Only reached if the kernel returned instead of booting.
    print("kernel exited: 0x%x" % exit_status)
    provider.close()
    kernel.close()
    initrd.close()


main()
