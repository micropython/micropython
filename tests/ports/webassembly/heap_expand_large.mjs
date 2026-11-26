// Test expanding a small MicroPython GC heap for allocations larger than it.

const mp = await (await import(process.argv[2])).loadMicroPython({
    heapsize: 64 * 1024,
});

mp.runPython(`
for size in (64 * 1024, 256 * 1024, 1024 * 1024, 4 * 1024 * 1024):
    buf = bytearray(size)
    buf[-1] = 1
    print(size, len(buf), buf[-1])
    buf = None
`);
