// Test expanding the MicroPython GC heap.

const mp = await (await import(process.argv[2])).loadMicroPython();

mp.runPython(`
import gc
bs = []
for i in range(24):
    b = bytearray(1 << i)
    bs.append(b)
    gc.collect()
    print(gc.mem_free())
for b in bs:
    print(len(b))
`);
