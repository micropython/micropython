import sys
from array import array

try:
    import framebuf
except ImportError:
    print("SKIP")
    raise SystemExit


def print_buffer(buffer, width, height):
    for row in range(height):
        for col in range(width):
            val = buffer[(row * width) + col]
            sys.stdout.write(" {:02x}".format(val) if val else " ··")
        sys.stdout.write("\n")


w = 30
h = 25
buf = bytearray(w * h)
fbuf = framebuf.FrameBuffer(buf, w, h, framebuf.GS8)
col = 0xFF
col_fill = 0x99

# Polygon methods are not present in dynamic natmod context
if not hasattr(fbuf, "poly"):
    print("SKIP")
    raise SystemExit

# This describes a arbitrary polygon (this happens to be a concave polygon in the shape of an upper-case letter 'M')
poly = [
    (0, 20),
    (3, 20),
    (3, 10),
    (6, 17),
    (9, 10),
    (9, 20),
    (12, 20),
    (12, 3),
    (9, 3),
    (6, 10),
    (3, 3),
    (0, 3),
]
# This describes the same polygon, but the points are in reverse order (it shouldn't matter if the polygon has
# clockwise or anti-clockwise winding)
poly_reversed = [
    (0, 3),
    (3, 3),
    (6, 10),
    (9, 3),
    (12, 3),
    (12, 20),
    (9, 20),
    (9, 10),
    (6, 17),
    (3, 10),
    (3, 20),
    (0, 20),
]
# This describes the same polygon, but given as a flat array of half words instead of a list of tuples (this is
# more memory efficient way of passing point data)
poly_array = array(
    "h",
    [
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
    ],
)

# Draw the line polygon
fbuf.poly(0, 0, poly, col)

# Same again, but use a non-zero origin point (i.e., translate the polygon by the given x and y)
# and use the reversed points list
fbuf.poly(15, -2, poly_reversed, col)

print_buffer(buf, w, h)
fbuf.fill(0)
print()

# Draw the filled polygon
fbuf.fill_poly(0, 0, poly, col_fill)

# Same again, but use a non-zero origin point (i.e., translate the polygon by the given x and y)
# and use the reversed points list
fbuf.fill_poly(15, -2, poly_reversed, col_fill)

print_buffer(buf, w, h)
fbuf.fill(0)
print()

# Draw line and filled polygons using a flat point array instead of a list of tuples
fbuf.fill_poly(0, 0, poly_array, col_fill)
fbuf.poly(0, 0, poly_array, col)
fbuf.fill_poly(15, -2, poly_array, col_fill)
fbuf.poly(15, -2, poly_array, col)

print_buffer(buf, w, h)
fbuf.fill(0)
print()

# Edge cases: These two lists describe self-intersecting polygons
poly_hourglass = [(0, 0), (9, 0), (0, 19), (9, 19)]
poly_star = [(7, 0), (3, 18), (14, 5), (0, 5), (11, 18)]

fbuf.fill_poly(0, 2, poly_hourglass, col_fill)
fbuf.poly(0, 2, poly_hourglass, col)
fbuf.fill_poly(12, 2, poly_star, col_fill)
fbuf.poly(12, 2, poly_star, col)

print_buffer(buf, w, h)
fbuf.fill(0)
print()

# Edge cases: These are "degenerate" polygons
poly_empty = []  # Will draw nothing at all
poly_one = [(20, 20)]  # Will draw a single point
poly_two = [(10, 10), (5, 5)]  # Will draw a single line
poly_invalid = [(10, 10, 10), (5, 5), (10, 5)]  # Error, points must contain 2 coords

fbuf.poly(0, 0, poly_empty, col)
fbuf.poly(0, 0, poly_one, col)
fbuf.poly(0, 0, poly_two, col)

try:
    fbuf.poly(0, 0, poly_invalid, col)
except ValueError:
    print("ValueError")
try:
    fbuf.fill_poly(0, 0, poly_invalid, col_fill)
except ValueError:
    print("ValueError")

print_buffer(buf, w, h)
fbuf.fill(0)
print()
