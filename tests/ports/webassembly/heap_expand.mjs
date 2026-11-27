// Test expanding the MicroPython GC heap.

const mp = await (await import(process.argv[2])).loadMicroPython();

mp.runPython(`
import gc

bs = []

gc.collect()
last_mem_free = gc.mem_free()

# allocate bytearray's in increasing size to eventually expand the heap
for i in range(24):
    b = bytearray(1 << i)
    bs.append(b)
    gc.collect()
    mem_free = gc.mem_free()
    print(mem_free - last_mem_free)
    last_mem_free = mem_free

# check bytearray sizes remain correct
for b in bs:
    print(len(b))
`);
