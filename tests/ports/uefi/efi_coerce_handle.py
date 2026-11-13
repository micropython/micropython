# Coercion class 3: the Python layer takes a bare EFI_HANDLE int wherever it takes a
# Handle. Descriptor fields (LoadedImage.device_handle) and raw.image_handle() hand
# back plain integers, and those are exactly the handles you want to pass onwards.
try:
    import uefi
except ImportError:
    print("SKIP")
    raise SystemExit

ih = uefi.raw.image_handle()

# ── Handle() is the universal coercion ──────────────────────────────────────
print(uefi.Handle(ih).ptr == ih)
print(uefi.Handle(uefi.Handle(ih)) == uefi.Handle(ih))
print(uefi.Handle(uefi.Image(ih)) == uefi.Handle(ih))  # any .ptr-bearing object

# ── a descriptor opens on a raw int handle ──────────────────────────────────
li = uefi.protocols.LOADED_IMAGE.open(ih)
dh = li.device_handle  # a bare pointer read out of the interface struct
li.close()
print(isinstance(dh, int) and dh != 0)

ph = uefi.handle.ProtocolHandle.open(ih, uefi.guid.LOADED_IMAGE_PROTOCOL)
print(ph.ptr != 0)
ph.close()

# ── ... and that int feeds the volume API without re-wrapping ───────────────
print(uefi.fs.open_volume(dh) is not None)
print(uefi.handle.Handle(dh).supports(uefi.guid.SIMPLE_FILE_SYSTEM_PROTOCOL))

# ── the driver model takes either form ──────────────────────────────────────
# A firmware status is a fine outcome here; a TypeError/AttributeError is not.
for form in (ih, uefi.Handle(ih)):
    try:
        uefi.driver.connect(form)
        print(True)
    except uefi.Error:
        print(True)
print(uefi.driver.driver_name(ih) == uefi.driver.driver_name(uefi.Handle(ih)))

print("uefi handle coercion ok")
