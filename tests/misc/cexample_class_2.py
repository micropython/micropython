# test custom native class

try:
    import cexample
    import gc
except ImportError:
    print("SKIP")
    raise SystemExit

timer = cexample.Timer()
timer = None
gc.collect()
print("done")
