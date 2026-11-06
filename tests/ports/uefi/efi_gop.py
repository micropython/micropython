# GOP framebuffer: overlay the mode info and dispatch Blt(BufferToVideo) — the path the
# drawing sample uses. GOP is optional (a headless machine has no GPU), so SKIP when the
# firmware publishes no GOP; run with a display adapter to exercise it.
try:
    import uefi
    import uctypes
    from uefi import protocols as P
except ImportError:
    print("SKIP")
    raise SystemExit

try:
    gop = P.GOP.locate()
except uefi.Error:
    print("SKIP")
    raise SystemExit

mode = gop.struct_at("mode", P.GOP_MODE)
info = mode.struct_at("info", P.GOP_INFO)
w = info.horizontal_resolution
h = info.vertical_resolution
print(w > 0 and h > 0)
bb = bytearray(bytes((0x20, 0, 0, 0)) * (w * h))  # dark blue back-buffer
gop.blt(uctypes.addressof(bb), 2, 0, 0, 0, 0, w, h, 0)  # 2 = EfiBltBufferToVideo
gop.close()
print("gop ok")
