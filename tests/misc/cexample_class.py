# test custom native class

try:
    import cexample
    import time
except ImportError:
    print("SKIP")
    raise SystemExit

t = cexample.Timer()

print(t)
print(t.time() <= 1)

time.sleep_ms(100)

elapsed = t.time()

if not (99 <= elapsed <= 110):
    print("Elapsed time should be approx. 100ms but it is", elapsed)
