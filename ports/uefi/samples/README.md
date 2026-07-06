# uefi port — sample scripts

These are staged into the boot volume at `/samples` and run from the REPL:

```python
exec(open('/samples/sysinfo.py').read())
```

(`exec(open(...).read())` runs the script fresh each time and leaves its objects in
the REPL, so you can keep poking at them afterwards.)

| Script         | What it shows                                              | Needs        |
|----------------|-----------------------------------------------------------|--------------|
| `sysinfo.py`   | Firmware vendor, heap size, display mode, boot volume path | —            |
| `devices.py`   | The handle database with protocol names (a mini `dh`)      | —            |
| `paths.py`     | Parse + format this image's and the boot volume's paths    | —            |
| `variables.py` | Enumerate / read / write / delete UEFI NVRAM variables     | —            |
| `hotplug.py`   | Watch for a protocol install (self-triggered demo)         | —            |
| `draw.py`      | Open GOP and draw lines + circles to the screen            | `make run-gfx` |
| `clock.py`     | A ticking analogue clock via `uefi.Display` (a framebuf)   | `make run-gfx` |
| `args.py`      | Prints `sys.argv` — for the `app.efi script.py ...` launch  | shell launch |
| `net_info.py`  | Bring up the interface (DHCP) and show IP / gateway / MAC   | networking   |
| `net_ping.py`  | ICMP-ping a host and report round-trip times               | networking   |
| `net_get.py`   | Resolve a name, open TCP, GET a URL, print the response     | networking   |
| `net_https.py` | Fetch https://www.google.com/ over TLS via asyncio          | networking   |
| `net_server.py`| A tiny HTTP server on :8080 (reachable from the host)       | networking   |
| `net_boot.py`  | Network boot loader: HTTPS-download a kernel+initramfs, boot | networking   |
| `wifi_connect.py`| Scan, join a WPA2 network, get a DHCP lease over EFI WiFi2 | WiFi hardware |
| `boot_disk.py` | Boot a kernel+initramfs read from the boot volume (no network)| boot images  |

`draw.py` and `clock.py` need a display, so launch with `make run-gfx` (not `make run`).

The `net_*.py` samples need networking. `make run` and `make run-gfx` attach a
virtio-net NIC on QEMU user-net (which reaches the outside world) and select the
network firmware — build it once with `bash docker/build-ovmf-net.sh`. After
`net_server.py` is serving, open <http://localhost:8080/> on your host (the run
targets forward host tcp/8080 to the guest).

`wifi_connect.py` needs real WiFi hardware with UEFI WiFi2 drivers (QEMU has no
802.11), so it only runs on metal — edit `SSID`/`KEY` and run it over the serial
REPL.

## Running a script directly (like `python foo.py`)

Besides the interactive REPL, the app runs a script if you launch it with a
filename argument from the UEFI shell:

```
FS0:\EFI\BOOT\BOOTAA64.EFI FS0:\samples\args.py hello world
```

`sys.argv` is set to `['FS0:\\samples\\args.py', 'hello', 'world']`, the file runs,
and the app exits back to the shell — no REPL. A bare launch (no filename) still
drops into the REPL.
