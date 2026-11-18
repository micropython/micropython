# Test choosing the heap area that allocations go in, with gc.area().

try:
    import gc

    gc.area
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit

areas = gc.areas()
if len(areas) < 2:
    print("SKIP")
    raise SystemExit

# Each entry is (start, total, free, max_free, attrs).
print(all(len(a) == 5 and a[1] >= a[2] >= a[3] for a in areas))

# Allocations go in the selected area, and the selection is restored on exit.
outside = gc.area_of(bytearray(16))
ok = []
for i in range(len(areas)):
    with gc.area(i):
        b = bytearray(1000)
        l = [i]
    ok.append(gc.area_of(b) == i and gc.area_of(l) == i)
print(all(ok))
print(gc.area_of(bytearray(16)) == outside)

# Nested selections, and gc.area() with no arguments restores the default.
with gc.area(1):
    with gc.area(0):
        print(gc.area_of(bytearray(16)) == 0)
    print(gc.area_of(bytearray(16)) == 1)
    with gc.area():
        print(gc.area_of(bytearray(16)) == outside)
    print(gc.area_of(bytearray(16)) == 1)

# The selection is restored when an exception leaves the block.
try:
    with gc.area(1):
        raise ValueError
except ValueError:
    pass
print(gc.area_of(bytearray(16)) == outside)

# Several areas can be selected.
with gc.area(0, 1):
    print(gc.area_of(bytearray(16)) in (0, 1))

# A strict selection raises MemoryError when its areas are full.  Nothing else
# can be allocated in the block after that, so check the results outside it.
last = len(areas) - 1
gc.collect()
keep = []
try:
    with gc.area(last, strict=True):
        while True:
            keep.append(bytearray(4096))
except MemoryError:
    print("MemoryError")
print(all(gc.area_of(b) == last for b in keep))

# Fill the gaps left in the area, with allocations of a single block.
chain = None
try:
    with gc.area(last, strict=True):
        while True:
            chain = (chain,)
except MemoryError:
    print("MemoryError")

# By default a selection isn't strict, and falls back to the other areas.
with gc.area(last):
    b = bytearray(4096)
print(gc.area_of(b) != last)
keep = chain = None
gc.collect()

# A block that has to move when it grows stays in its area.
with gc.area(1):
    b = bytearray(100)
    blocker = bytearray(16)
for _ in range(8):
    b.extend(bytes(500))
print(gc.area_of(b) == 1)

# Aligned allocations, also combined with a selection.
try:
    from uctypes import addressof
except ImportError:
    addressof = None
for align in (32, 64, 256, 4096):
    with gc.area(align=align):
        bufs = [bytearray(n) for n in (1, 33, 100)]
    print(align, addressof is None or all(addressof(b) % align == 0 for b in bufs))
with gc.area(1, align=128):
    b = bytearray(10)
print(gc.area_of(b) == 1, addressof is None or addressof(b) % 128 == 0)

# Selecting by attribute.
for attr in (gc.AREA_EXTERNAL, gc.AREA_DMA):
    try:
        sel = gc.area(attrs=attr)
    except ValueError:
        # No area has this attribute.
        print(True)
        continue
    with sel:
        print(gc.areas()[gc.area_of(bytearray(16))][4] & attr == attr)

# area_of() of an object that isn't in the heap.
print(gc.area_of(None), gc.area_of(1), gc.area_of(print))

# Invalid areas and alignments.
for args, kwargs in (
    ((-1,), {}),
    ((len(areas),), {}),
    ((), {"attrs": 0x10000}),
    ((), {"align": 3}),
    ((), {"align": -8}),
):
    try:
        gc.area(*args, **kwargs)
    except ValueError:
        print("ValueError")
