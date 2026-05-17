# test struct package with floats
try:
    import struct
except ImportError:
    print("SKIP")
    raise SystemExit

i = 1.0 + 1 / 2
# TODO: it looks like '=' format modifier is not yet supported
# for fmt in ('f', 'd', '>f', '>d', '<f', '<d', '=f', '=d'):
for fmt in ("e", "f", "d", ">e", ">f", ">d", "<e", "<f", "<d"):
    x = struct.pack(fmt, i)
    v = struct.unpack(fmt, x)[0]
    print("%2s: %.17f - %s" % (fmt, v, (i == v) and "passed" or "failed"))
