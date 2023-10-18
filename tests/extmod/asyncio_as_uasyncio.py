try:
    import uasyncio
    import asyncio
except ImportError:
    print("SKIP")
    raise SystemExit

x = set(dir(uasyncio))
y = set(dir(asyncio)) - set(["event", "lock", "stream", "funcs"])

print(x - y)
print(y - x)
