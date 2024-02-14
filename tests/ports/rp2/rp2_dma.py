# Test rp2.DMA functionality.

import time
import machine
import rp2

src = bytes(i & 0xFF for i in range(16 * 1024))

print("# test basic usage")

dma = rp2.DMA()
print(dma)
print(rp2.DMA.unpack_ctrl(dma.pack_ctrl()))
dma.read = 0
dma.write = 0
dma.count = 0
dma.ctrl = dma.pack_ctrl()
print(dma.read, dma.write, dma.count, dma.ctrl & 0x01FFFFFF, dma.channel, dma.registers)
dma.close()

# Test closing when already closed.
dma.close()

# Test using when closed.
try:
    dma.active()
    assert False
except ValueError:
    print("ValueError")

# Test simple memory copy.
print("# test memory copy")
dest = bytearray(1024)
dma = rp2.DMA()
dma.config(read=src, write=dest, count=len(dest) // 4, ctrl=dma.pack_ctrl(), trigger=False)
print(not any(dest))
dma.active(True)
while dma.active():
    pass
print(dest[:8], dest[-8:])
dma.close()


# Test time taken for a large memory copy.
def run_and_time_dma(dma):
    ticks_us = time.ticks_us
    irq_state = machine.disable_irq()
    t0 = ticks_us()
    dma.active(True)
    while dma.active():
        pass
    t1 = ticks_us()
    machine.enable_irq(irq_state)
    return time.ticks_diff(t1, t0)


print("# test timing")
dest = bytearray(16 * 1024)
dma = rp2.DMA()
dma.read = src
dma.write = dest
dma.count = len(dest) // 4
dma.ctrl = dma.pack_ctrl()
dt = run_and_time_dma(dma)
print(70 <= dt <= 110)
print(dest[:8], dest[-8:])
dma.close()

# Test using .config(trigger=True).
print("# test immediate trigger")
dest = bytearray(1024)
dma = rp2.DMA()
dma.config(read=src, write=dest, count=len(dest) // 4, ctrl=dma.pack_ctrl(), trigger=True)
while dma.active():
    pass
print(dest[:8], dest[-8:])
dma.close()

# Test the DMA.irq() method.
print("# test irq")
dest = bytearray(1024)
dma = rp2.DMA()
dma.irq(lambda dma: print("irq fired", dma.irq().flags()))
dma.config(
    read=src, write=dest, count=len(dest) // 4, ctrl=dma.pack_ctrl(irq_quiet=0), trigger=True
)
while dma.active():
    pass
print(dest[:8], dest[-8:])
dma.close()
