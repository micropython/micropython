# test-images — a minimal OS for exercising the boot path

These are throwaway images for testing the MicroPython-UEFI boot mechanism
(LoadImage → EFI_LOAD_FILE2 initrd → StartImage) end to end, without depending on
a large external download.

## Build

```
make docker-test-os        # from the host (runs build-test-os.sh in the dev container)
```

`build-test-os.sh` produces (both git-ignored):

| File                     | What it is                                                        |
|--------------------------|-------------------------------------------------------------------|
| `vmlinuz-aa64`           | An EFI-stub aarch64 kernel — the Ubuntu `linux-image` generic `Image`, gunzipped (a real PE that `LoadImage` boots). ~59 MB. |
| `initramfs-aa64.cpio.gz` | A ~1 MB BusyBox initramfs: `/init` mounts `/proc`,`/sys`,`/dev`, prints a banner, and drops to a shell. |

The kernel is a stock generic build (reliable — all the QEMU `virt` drivers are
present), so it isn't byte-minimal; the *initramfs* is the minimal part. For a
smaller kernel, build one from source or use a VM-tuned config (e.g. Alpine's
`linux-virt`) — the boot path is identical.

## Boot it

From the REPL (`make run`), reading the images off the boot volume:

```python
exec(open('/samples/boot_disk.py').read())
```

`boot_disk.py` reads `/vmlinuz-aa64` and `/initramfs-aa64.cpio.gz` into pool
buffers, presents the initramfs from RAM via `EFI_LOAD_FILE2_PROTOCOL`
(`uefi.load_file`), and boots the kernel. Stage the images onto the boot volume
first (copy them into `esp/` after `make stage`). A successful boot prints:

```
==== MPY-UEFI TEST INITRAMFS: BusyBox is up ====
Linux (none) 6.8.0-… aarch64 GNU/Linux
```

and lands at a BusyBox shell. `samples/net_boot.py` does the same but downloads the
images over HTTPS instead of reading them locally.
