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


print("bytearray")
test_constructor(bytearray(64), 8, 8, framebuf.GS8)

print("bytes")
test_constructor(bytes(64), 8, 8, framebuf.GS8)
