# LOADED_IMAGE on our own image handle (uctypes field reads) plus device-path parse:
# from the boot volume's raw device-path pointer, serialize byte-identically and
# round-trip through to_text / from_text.
try:
    import uefi
    import uctypes
    from uefi import protocols as P, device_path as DP
except ImportError:
    print("SKIP")
    raise SystemExit

img = uefi.Handle(uefi.raw.image_handle())
li = P.LOADED_IMAGE.open(img)
print(li.image_base != 0)
print(li.image_size > 0)
dev = li.device_handle
li.close()
print(dev != 0)
print(img.supports(uefi.guid.LOADED_IMAGE_PROTOCOL))

devh = uefi.Handle(dev)
ph = devh.open(uefi.guid.DEVICE_PATH_PROTOCOL)
dp = DP.DevicePath.from_ptr(ph.ptr)
print(len(dp) >= 1)
mine = dp.to_bytes()
print(uctypes.bytes_at(ph.ptr, len(mine)) == mine)  # parse -> serialize is byte-identical
ph.close()
txt = dp.to_text()
print(len(txt) > 0)
print(DP.DevicePath.from_text(txt).to_text() == txt)  # text round-trip
print(len(devh.device_path()) == len(dp))
print("loaded image ok")
