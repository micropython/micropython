# test custom native class

try:
    import cexample
    import time
except ImportError:
    print("SKIP")
    raise SystemExit


SLEEP_MS = 100
TOLERANCE_MS = 20

timer = cexample.Timer()

t_start = timer.time()

time.sleep_ms(100)

t_end = timer.time()

print(timer)
print(0 <= t_start <= TOLERANCE_MS)
print(SLEEP_MS - TOLERANCE_MS <= t_end <= SLEEP_MS + TOLERANCE_MS)

advanced_timer = cexample.AdvancedTimer()

time.sleep_ms(100)

print(repr(advanced_timer))
print(str(advanced_timer))

print(advanced_timer.seconds)
advanced_timer.seconds = 123
print(advanced_timer.seconds)
print(advanced_timer.time() < 123000 + TOLERANCE_MS)

try:
    advanced_timer.seconds = "bad input"
except TypeError:
    print("TypeError")
