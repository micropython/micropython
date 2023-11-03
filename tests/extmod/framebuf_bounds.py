try:
    import framebuf
except ImportError:
    print("SKIP")
    raise SystemExit


def test_constructor(*args):
    try:
        framebuf.FrameBuffer(*args)
        print("Valid")
    except ValueError:
        print("ValueError")


print(framebuf.MONO_HLSB)
test_constructor(bytearray(20), 10, 10, framebuf.MONO_HLSB)
test_constructor(bytearray(20), -1, 10, framebuf.MONO_HLSB)
test_constructor(bytearray(20), 10, -1, framebuf.MONO_HLSB)
test_constructor(bytearray(20), 10, 10, framebuf.MONO_HLSB, 11)
test_constructor(bytearray(20), 10, 10, framebuf.MONO_HLSB, 10)
test_constructor(bytearray(20), 10, 10, framebuf.MONO_HLSB, 9)
test_constructor(bytearray(20), 10, 10, framebuf.MONO_HLSB, -1)

print(framebuf.MONO_VLSB)
test_constructor(bytearray(8), 8, 1, framebuf.MONO_VLSB)
test_constructor(bytearray(7), 8, 1, framebuf.MONO_VLSB)
test_constructor(bytearray(8), 8, 8, framebuf.MONO_VLSB)

for f in (framebuf.MONO_HLSB, framebuf.MONO_HMSB):
    print(f)
    test_constructor(bytearray(1), 8, 1, f)
    test_constructor(bytearray(0), 8, 1, f)
    test_constructor(bytearray(8), 8, 8, f)
    test_constructor(bytearray(9), 8, 9, f)
    test_constructor(bytearray(9), 9, 8, f)

print(framebuf.GS2_HMSB)
test_constructor(bytearray(8), 4, 8, framebuf.GS2_HMSB)
test_constructor(bytearray(15), 5, 8, framebuf.GS2_HMSB)
test_constructor(bytearray(16), 5, 8, framebuf.GS2_HMSB)
test_constructor(bytearray(9), 4, 9, framebuf.GS2_HMSB)

print(framebuf.GS4_HMSB)
test_constructor(bytearray(8), 2, 8, framebuf.GS4_HMSB)
test_constructor(bytearray(15), 3, 8, framebuf.GS4_HMSB)
test_constructor(bytearray(16), 3, 8, framebuf.GS4_HMSB)
test_constructor(bytearray(9), 2, 9, framebuf.GS4_HMSB)

print(framebuf.GS8)
test_constructor(bytearray(63), 8, 8, framebuf.GS8)
test_constructor(bytearray(64), 8, 8, framebuf.GS8)
test_constructor(bytearray(64), 9, 8, framebuf.GS8)
test_constructor(bytearray(64), 8, 9, framebuf.GS8)

print(framebuf.RGB565)
test_constructor(bytearray(127), 8, 8, framebuf.RGB565)
test_constructor(bytearray(128), 8, 8, framebuf.RGB565)
test_constructor(bytearray(128), 9, 8, framebuf.RGB565)
test_constructor(bytearray(128), 8, 9, framebuf.RGB565)
