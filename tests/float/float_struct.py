# test struct package with floats
try:
    try:
        import ustruct as struct
    except:
        import struct
except ImportError:
    import sys
    print("SKIP")
    sys.exit()

i = 1. + 1/2
# TODO: it looks like '=' format modifier is not yet supported
# for fmt in ('f', 'd', '>f', '>d', '<f', '<d', '=f', '=d'):
for fmt in ('f', 'd', '>f', '>d', '<f', '<d'):
    x = struct.pack(fmt, i)
    v = struct.unpack(fmt, x)[0]
    print('%2s: %.17f - %s' % (fmt, v, (i == v) and 'passed' or 'failed'))

