# Test that the CPU frequency is correctly restored when waking from lightsleep.

try:
    from machine import freq, lightsleep
except ImportError:
    print("SKIP")
    raise SystemExit

old_freq = freq()
freq(50_000_000)
print(freq())
lightsleep(500)
print(freq())
freq(51_000_000, 48_000_000)
print(freq())
lightsleep(500)
print(freq())
freq(old_freq)
