# Test interning qstrs that go over the qstr length limit (255 bytes in default configuration).
# The tests here are specifically for str formatting with %.

try:
    "" % ()
except TypeError:
    print("SKIP")
    raise SystemExit


def make_id(n, base="a"):
    return "".join(chr(ord(base) + i % 26) for i in range(n))


# modulo format with keys
for l in range(254, 259):
    id = make_id(l)
    try:
        print(("%(" + id + ")d") % {id: l})
    except RuntimeError as er:
        print("RuntimeError", er, l)
