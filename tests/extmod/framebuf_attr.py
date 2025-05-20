try:
    import framebuf
except ImportError:
    print("SKIP")
    raise SystemExit

# Test different formats
formats = [
    (framebuf.MONO_VLSB, "MONO_VLSB"),
    (framebuf.MONO_HLSB, "MONO_HLSB"),
    (framebuf.MONO_HMSB, "MONO_HMSB"),
    (framebuf.RGB565, "RGB565"),
    (framebuf.GS2_HMSB, "GS2_HMSB"),
    (framebuf.GS4_HMSB, "GS4_HMSB"),
    (framebuf.GS8, "GS8"),
]

# Test with different dimensions
dimensions = [(8, 4), (16, 8), (128, 64)]

for width, height in dimensions:
    for fmt, name in formats:
        # Create appropriate buffer size based on format
        if fmt == framebuf.MONO_VLSB:
            size = ((height + 7) // 8) * width
        elif fmt == framebuf.MONO_HLSB or fmt == framebuf.MONO_HMSB:
            size = ((width + 7) // 8) * height
        elif fmt == framebuf.RGB565:
            size = width * height * 2
        elif fmt == framebuf.GS2_HMSB:
            size = ((width + 3) // 4) * height
        elif fmt == framebuf.GS4_HMSB:
            size = ((width + 1) // 2) * height
        elif fmt == framebuf.GS8:
            size = width * height
        else:
            continue

        buf = bytearray(size)
        fb = framebuf.FrameBuffer(buf, width, height, fmt)

        print(name, "{}x{}:".format(width, height))
        print("width:", fb.width)
        print("height:", fb.height)
        print("format:", fb.format)
        print("stride:", fb.stride)
        print("format == {}: {}".format(name, fb.format == fmt))

        # Try accessing a non-existent attribute (should fail)
        try:
            print(fb.non_existent)
            print("ERROR: should have raised AttributeError")
        except AttributeError:
            print("Raised AttributeError correctly")

        print()
