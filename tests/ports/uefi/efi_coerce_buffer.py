# Coercion classes 4 & 5: objects that *are* byte blocks (PoolBuffer, DevicePath) are
# accepted wherever bytes are, and a device path is accepted in any of its four
# interchangeable spellings — object, packed bytes, text, or firmware pointer.
try:
    import uefi
except ImportError:
    print("SKIP")
    raise SystemExit

NS = uefi.GUID("2f4e6b1a-1111-4c2d-9e3a-0123456789ab")
VOLATILE = uefi.variable.BOOTSERVICE_ACCESS  # never write flash from a test
PAYLOAD = b"micropython-uefi"

# ── PoolBuffer behaves as the block of memory it is ─────────────────────────
pb = uefi.PoolBuffer.from_bytes(PAYLOAD)
print(len(pb) == len(PAYLOAD))
print(bytes(pb.view) == PAYLOAD)  # a real buffer view over pool memory
print(pb.view[:5] == b"micro")
print(pb.bytes() == PAYLOAD)

# ... so it is a valid `data` argument, no .bytes()/.ptr unpacking needed
uefi.variable.set("MpyCoercePb", NS, pb, VOLATILE)
print(uefi.variable.get("MpyCoercePb", NS) == PAYLOAD)
uefi.variable.delete("MpyCoercePb", NS)
pb.close()

# ── a DevicePath goes wherever its packed bytes go ──────────────────────────
li = uefi.protocols.LOADED_IMAGE.open(uefi.raw.image_handle())
dp = uefi.Handle(li.device_handle).device_path()
li.close()
packed = dp.to_bytes()
print(dp is not None and len(packed) > 4)

uefi.variable.set("MpyCoerceDp", NS, dp, VOLATILE)
print(uefi.variable.get("MpyCoerceDp", NS) == packed)
uefi.variable.delete("MpyCoerceDp", NS)

print(uefi.DevicePath.from_bytes(dp).to_bytes() == packed)  # accepts itself
buf = uefi.PoolBuffer(len(packed))
buf.write(dp)  # PoolBuffer.write takes any byte block
print(buf.bytes() == packed)
buf.close()

# ── every spelling reaches a device-path parameter ──────────────────────────
lo = uefi.LoadOption("Coerce probe", packed)  # packed bytes
print(lo.device_path.to_bytes() == packed)

try:
    uefi.device_path.DEVICE_PATH_FROM_TEXT.locate()
    has_from_text = True
except uefi.Error:
    has_from_text = False

if has_from_text:
    text = dp.to_text()
    print(uefi.LoadOption("Coerce probe", text).device_path.to_bytes() == packed)
    # load_from_device_path takes text too. A firmware status is a fine outcome;
    # an AttributeError from a str that never reached the firmware is not.
    try:
        uefi.Image.load_from_device_path(text)
        print(True)
    except uefi.Error:
        print(True)
else:
    print(True)
    print(True)

print("uefi buffer coercion ok")
