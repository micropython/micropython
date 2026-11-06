# uefi.guid registry cross-check: the named GUIDs the port knows must match the bytes
# the firmware actually uses, so a few always-present protocols resolve both ways.
try:
    import uefi
except ImportError:
    print("SKIP")
    raise SystemExit

aprotos = uefi.handle.all_protocols()
reg = uefi.guid.registry()
found = set(n for n, g in reg.items() if g in aprotos)
for must in ("LOADED_IMAGE_PROTOCOL", "DEVICE_PATH_PROTOCOL", "BLOCK_IO_PROTOCOL"):
    print(must in found)
print(uefi.guid.name_for(uefi.guid.BLOCK_IO_PROTOCOL))
print(uefi.guid.name_for(uefi.GUID("00000000-0000-0000-0000-000000000000")))
print("uefi guid registry ok")
