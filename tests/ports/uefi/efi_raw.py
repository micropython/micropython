# uefi.raw layer: handle enumeration, table pointers, protocol location, PoolBuffer,
# and status -> uefi.Error mapping. (The EFIAPI call trampoline is exercised without
# console side-output in efi_load_file2.py and efi_blockio.py.)
try:
    import uefi
except ImportError:
    print("SKIP")
    raise SystemExit

# handle enumeration (LocateHandleBuffer, AllHandles=0)
st, handles = uefi.raw.locate_handle_buffer(0, None)
print(st, len(handles) > 0)
# table pointers are sane
print(uefi.raw.boot_services_ptr() != 0)
print(uefi.raw.boot_services_active())
# locate SIMPLE_TEXT_OUTPUT -> non-null interface
st, iface = uefi.raw.locate_protocol(uefi.GUID("387477c2-69c7-11d2-8e39-00a0c969723b").bytes)
print(st, iface != 0)
# PoolBuffer round-trip
pb = uefi.PoolBuffer(64)
print(pb.ptr != 0)
pb.close()
# status.check maps the error range to uefi.Error
try:
    uefi.check(uefi.status.NOT_FOUND)
    print("no raise")
except uefi.Error as e:
    print(e.status == uefi.status.NOT_FOUND, e.name)
print("uefi raw ok")
