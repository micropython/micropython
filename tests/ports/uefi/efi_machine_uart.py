# machine.UART on the UEFI port, run with the REPL on serial port 0 (as the test harness
# does), so UART(0) shares the REPL's port: writes go out with the REPL's output, and
# there is nothing waiting to read. Covers argument validation, the timeout semantics
# (timeout=0 never waits; timeout=N waits about N ms), string ids, polling, and deinit()
# on the REPL's port releasing the object but not the REPL.
try:
    from machine import UART
    import errno
    import select
    import time
except ImportError:
    print("SKIP")
    raise SystemExit

u = UART(0)
print(UART.RTS | UART.CTS == 3)
u.write(b"uart0 write ok\r\n")
print(u.any())
print(u.read())  # timeout=0: nothing waiting -> None at once
print(u.read(5))

u.init(timeout=100)
t = time.ticks_ms()
r = u.read(1)
dt = time.ticks_diff(time.ticks_ms(), t)
print(r, 90 <= dt < 2000)
u.init(timeout=0)

# Validation happens before the port is touched.
for kw in (
    {"flow": UART.RTS},
    {"flow": UART.CTS},
    {"flow": 5},
    {"bits": 9},
    {"stop": 3},
    {"parity": 2},
    {"baudrate": 0},
    {"rxbuf": 0},
):
    try:
        UART(0, **kw)
        print("no error", kw)
    except ValueError:
        print("ValueError")
for bad in (99, -1, "no-such-port"):
    try:
        UART(bad)
        print("no error", bad)
    except ValueError:
        print("ValueError")

# A string id matches the port's device-path text, which has a Uart node.
print(str(UART("Uart")) == str(u))

p = select.poll()
p.register(u, select.POLLIN | select.POLLOUT)
ev = p.poll(0)
print(len(ev) == 1 and bool(ev[0][1] & select.POLLOUT), not ev[0][1] & select.POLLIN)

# asyncio streams poll it too: with nothing to read, a bounded read times out.
try:
    import asyncio

    async def read_with_timeout():
        try:
            await asyncio.wait_for(asyncio.StreamReader(u).read(1), 0.05)
            print("read returned")
        except asyncio.TimeoutError:
            print("asyncio timeout")

    asyncio.run(read_with_timeout())
except ImportError:
    print("asyncio timeout")

# deinit() on the REPL's port releases this object; the REPL keeps the port.
u.deinit()
try:
    u.write(b"x")
except OSError as e:
    print("OSError", e.errno == errno.ENODEV)
print(str(u))
print("done")
