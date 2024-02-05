try:
    import framebuf
    from array import array
except ImportError:
    print("SKIP")
    raise SystemExit


# TODO: poly needs functions that aren't in dynruntime.h yet.
if not hasattr(framebuf.FrameBuffer, "poly"):
    print("SKIP")
    raise SystemExit


def print_buffer(buffer, width, height):
    for row in range(height):
        for col in range(width):
            val = buffer[(row * width) + col]
            print(" {:02x}".format(val) if val else " ··", end="")
        print()


buf = bytearray(70 * 70)

w = 30
h = 25
fbuf = framebuf.FrameBuffer(buf, w, h, framebuf.GS8)
col = 0xFF
col_fill = 0x99

# This describes a arbitrary polygon (this happens to be a concave polygon in
# the shape of an upper-case letter 'M').
poly = array(
    "h",
    (
        0,
        20,
        3,
        20,
        3,
        10,
        6,
        17,
        9,
        10,
        9,
        20,
        12,
        20,
        12,
        3,
        9,
        3,
        6,
        10,
        3,
        3,
        0,
        3,
    ),
)
# This describes the same polygon, but the points are in reverse order
# (it shouldn't matter if the polygon has clockwise or anti-clockwise
# winding). Also defined as a bytes instead of array.
poly_reversed = bytes(
    (
        0,
        3,
        3,
        3,
        6,
        10,
        9,
        3,
        12,
        3,
        12,
        20,
        9,
        20,
        9,
        10,
        6,
        17,
        3,
        10,
        3,
        20,
        0,
        20,
    )
)

# Draw the line polygon (at the origin) and the reversed-order polygon (offset).
fbuf.fill(0)
fbuf.poly(0, 0, poly, col)
fbuf.poly(15, -2, poly_reversed, col)
print_buffer(buf, w, h)
print()

# Same but filled.
fbuf.fill(0)
fbuf.poly(0, 0, poly, col_fill, True)
fbuf.poly(15, -2, poly_reversed, col_fill, True)
print_buffer(buf, w, h)
print()

# Draw the fill then the outline to ensure that no fill goes outside the outline.
fbuf.fill(0)
fbuf.poly(0, 0, poly, col_fill, True)
fbuf.poly(0, 0, poly, col)
fbuf.poly(15, -2, poly, col_fill, True)
fbuf.poly(15, -2, poly, col)
print_buffer(buf, w, h)
print()

# Draw the outline then the fill to ensure the fill completely covers the outline.
fbuf.fill(0)
fbuf.poly(0, 0, poly, col)
fbuf.poly(0, 0, poly, col_fill, True)
fbuf.poly(15, -2, poly, col)
fbuf.poly(15, -2, poly, col_fill, True)
print_buffer(buf, w, h)
print()

# Draw polygons that will go out of bounds at each of the edges.
for x, y in (
    (
        -8,
        -8,
    ),
    (
        24,
        -6,
    ),
    (
        20,
        12,
    ),
    (
        -2,
        10,
    ),
):
    fbuf.fill(0)
    fbuf.poly(x, y, poly, col)
    print_buffer(buf, w, h)
    print()
    fbuf.fill(0)
    fbuf.poly(x, y, poly_reversed, col, True)
    print_buffer(buf, w, h)
    print()

# Edge cases: These two lists describe self-intersecting polygons
poly_hourglass = array("h", (0, 0, 9, 0, 0, 19, 9, 19))
poly_star = array("h", (7, 0, 3, 18, 14, 5, 0, 5, 11, 18))

# As before, fill then outline.
fbuf.fill(0)
fbuf.poly(0, 2, poly_hourglass, col_fill, True)
fbuf.poly(0, 2, poly_hourglass, col)
fbuf.poly(12, 2, poly_star, col_fill, True)
fbuf.poly(12, 2, poly_star, col)
print_buffer(buf, w, h)
print()

# Outline then fill.
fbuf.fill(0)
fbuf.poly(0, 2, poly_hourglass, col)
fbuf.poly(0, 2, poly_hourglass, col_fill, True)
fbuf.poly(12, 2, poly_star, col)
fbuf.poly(12, 2, poly_star, col_fill, True)
print_buffer(buf, w, h)
print()

# Edge cases: These are "degenerate" polygons.
poly_empty = array("h")  # Will draw nothing at all.
poly_one = array("h", (20, 20))  # Will draw a single point.
poly_two = array("h", (10, 10, 5, 5))  # Will draw a single line.
poly_wrong_length = array("h", (2, 2, 4))  # Will round down to one point.

fbuf.fill(0)
fbuf.poly(0, 0, poly_empty, col)
fbuf.poly(0, 0, poly_one, col)
fbuf.poly(0, 0, poly_two, col)
fbuf.poly(0, 0, poly_wrong_length, col)
print_buffer(buf, w, h)
print()

# A shape with a horizontal overhang.
poly_overhang = array("h", (0, 0, 0, 5, 5, 5, 5, 10, 10, 10, 10, 0))

fbuf.fill(0)
fbuf.poly(0, 0, poly_overhang, col)
fbuf.poly(0, 0, poly_overhang, col_fill, True)
print_buffer(buf, w, h)
print()

fbuf.fill(0)
fbuf.poly(0, 0, poly_overhang, col_fill, True)
fbuf.poly(0, 0, poly_overhang, col)
print_buffer(buf, w, h)
print()

# Triangles
w = 70
h = 70
fbuf = framebuf.FrameBuffer(buf, w, h, framebuf.GS8)
t1 = array("h", [40, 0, 20, 68, 62, 40])
t2 = array("h", [40, 0, 0, 16, 20, 68])

fbuf.fill(0)
fbuf.poly(0, 0, t1, 0xFF, False)
fbuf.poly(0, 0, t2, 0xFF, False)
print_buffer(buf, w, h)

fbuf.fill(0)
fbuf.poly(0, 0, t1, 0xFF, True)
fbuf.poly(0, 0, t2, 0xFF, True)
print_buffer(buf, w, h)
