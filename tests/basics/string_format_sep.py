try:
    "%d" % 1
except TypeError:
    print("SKIP")
    raise SystemExit

for v in (0, 0x10, 0x1000, -0x10, -0x1000):
    for sz in range(1, 12): print(("{:0%d,d}" % sz).format(v))
    for sz in range(1, 12): print(("{:0%d_x}" % sz).format(v))
