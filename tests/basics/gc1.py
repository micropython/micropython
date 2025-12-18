# basic tests for gc module

try:
    import gc
except ImportError:
    print("SKIP")
    raise SystemExit

print(gc.isenabled())
gc.disable()
print(gc.isenabled())
gc.enable()
print(gc.isenabled())

gc.collect()

if hasattr(gc, 'mem_free'):
    # MicroPython has these extra functions
    # just test they execute and return an int
    assert type(gc.mem_free()) is int
    assert type(gc.mem_alloc()) is int

if hasattr(gc, 'threshold'):
    # MicroPython has this extra function
    # check execution and returns
    assert(gc.threshold(1) is None)
    assert(gc.threshold() == 0)
    assert(gc.threshold(-1) is None)
    assert(gc.threshold() == -1)

    # Setting a low threshold should trigger collection at the list alloc
    gc.threshold(1)
    [[], []]
    gc.threshold(-1)
