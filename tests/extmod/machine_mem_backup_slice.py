# Test machine.mem_backup() function.

try:
    import machine

    mem = machine.mem_backup()
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit

if mem.itemsize != 1:
    print("SKIP")
    raise SystemExit

mem[0:1] = bytes(b"\x11")
print(mem[0:1].hex())
mem[0:3] = bytes(b"\x22\x33\x44")
print(mem[0:3].hex())
mem[1:5] = bytes(b"\x55\x66\x77\x88")
print(mem[1:5].hex())
