# tests meminfo functions in micropython module

try:
    import micropython

    micropython.mem_total
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit

t = micropython.mem_total()
c = micropython.mem_current()
p = micropython.mem_peak()

l = list(range(10000))

print(micropython.mem_total() > t)
print(micropython.mem_current() > c)
print(micropython.mem_peak() > p)
