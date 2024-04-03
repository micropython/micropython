# test custom native class

try:
    import cexample
    import time
    import gc
except ImportError:
    print("SKIP")
    raise SystemExit

a = 100
b = 20

timer = cexample.Timer()
c = 4
d = 5
timer = None
gc.collect()
print("done")
