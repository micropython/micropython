# LOAD_FILE2 provider: present an in-memory blob (as a Linux initrd would be), then read
# it back by invoking the installed protocol's LoadFile through the real EFIAPI path
# (uefi.raw.call) — the same two-phase probe/copy the OS loader does.
try:
    import uefi
    import uctypes
    from uefi import load_file as LF
except ImportError:
    print("SKIP")
    raise SystemExit

blob = b"INITRD-ACID-TEST-" + bytes(range(48))
fp = LF.FileProvider(blob, LF.linux_initrd_path())
print(fp.handle != 0)
lf_fn = int.from_bytes(uctypes.bytes_at(fp._token, 8), "little")  # record[0] = LoadFile fn
sz = uefi.PoolBuffer(8)
sz.write((0).to_bytes(8, "little"))
r1 = uefi.raw.call(lf_fn, fp._token, 0, 0, sz.ptr, 0)  # probe: Buffer=NULL
need = int.from_bytes(sz.bytes(0, 8), "little")
print(r1 != 0, need == len(blob))  # EFI_BUFFER_TOO_SMALL
out = uefi.PoolBuffer(need)
sz.write(need.to_bytes(8, "little"))
r2 = uefi.raw.call(lf_fn, fp._token, 0, 0, sz.ptr, out.ptr)
print(r2)
print(out.bytes(0, need) == blob)
out.close()
sz.close()
fp.close()
print("load_file2 provider ok")
