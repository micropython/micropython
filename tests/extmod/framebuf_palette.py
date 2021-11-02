# Test blit between different color spaces
try:
    import framebuf, usys
except ImportError:
    print("SKIP")
    raise SystemExit

# Monochrome glyph/icon
w = 8
h = 8
cbuf = bytearray(w * h // 8)
fbc = framebuf.FrameBuffer(cbuf, w, h, framebuf.MONO_HLSB)
fbc.line(0, 0, 7, 7, 1)

# RGB565 destination
wd = 16
hd = 16
dest = bytearray(wd * hd * 2)
fbd = framebuf.FrameBuffer(dest, wd, hd, framebuf.RGB565)

wp = 2
bg = 0x1234
fg = 0xF800
pal = bytearray(wp * 2)
palette = framebuf.FrameBuffer(pal, wp, 1, framebuf.RGB565)
palette.pixel(0, 0, bg)
palette.pixel(1, 0, fg)

fbd.blit(fbc, 0, 0, -1, palette)

print(fbd.pixel(0, 0) == fg)
print(fbd.pixel(7, 7) == fg)
print(fbd.pixel(8, 8) == 0)  # Ouside blit
print(fbd.pixel(0, 1) == bg)
print(fbd.pixel(1, 0) == bg)
