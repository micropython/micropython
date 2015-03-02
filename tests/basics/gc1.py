# basic tests for gc module

try:
    import gc
except ImportError:
    print("SKIP")
    import sys
    sys.exit()

print(gc.isenabled())
gc.disable()
print(gc.isenabled())
gc.enable()
print(gc.isenabled())

gc.collect()

if hasattr(gc, 'mem_free'):
    # uPy has these extra functions
    # just test they execute and return an int
    assert type(gc.mem_free()) is int
    assert type(gc.mem_alloc()) is int
