# PCI_IO.get_location through the pre-baked descriptor + EFIAPI trampoline. SKIP where no
# PCI bus is present.
try:
    import uefi
    from uefi import protocols as P
except ImportError:
    print("SKIP")
    raise SystemExit

pci = uefi.handle.locate_handles(uefi.guid.PCI_IO_PROTOCOL)
if not pci:
    print("SKIP")
    raise SystemExit

p = P.PCI_IO.open(pci[0])
loc = p.get_location()  # four Out(U64) slots -> (seg, bus, dev, fn), decoded
print(len(loc) == 4 and all(isinstance(v, int) for v in loc))
p.close()
print("pci ok")

# poll_mem (spec: Width, BarIndex, Offset, Mask, Value, Delay, *Result). Find a memory
# BAR to poll: other BAR kinds are rejected with an error, so try each in turn.
W32 = 2  # EfiPciIoWidthUint32
found = None
for h in pci:
    p = P.PCI_IO.open(h)
    for bar in range(6):
        try:
            # Mask 0 matches on the first read, which comes back as the result.
            r = p.poll_mem(W32, bar, 0, 0, 0, 0)
        except uefi.Error:
            continue
        found = (p, bar, r)
        break
    if found:
        break
    p.close()
print("mem bar found", found is not None)
p, bar, r = found
buf = bytearray(4)
p.mem_read(W32, bar, 0, 1, buf)
print("poll result is the register", r == int.from_bytes(buf, "little"))
# A value the register can't hold must time out after Delay (1000 x 100 ns), which only
# happens when Delay sits in its own slot rather than receiving the Result pointer.
try:
    p.poll_mem(W32, bar, 0, 0xFFFFFFFF, r ^ 0xFFFFFFFF, 1000)
    print("no timeout")
except uefi.Error as e:
    print(e.name)

# DMA buffer round trip: AllocateBuffer -> Map(CommonBuffer) -> Unmap -> FreeBuffer.
host = bytearray(8)
p.allocate_buffer(0, 4, 1, host, 0)  # AllocateAnyPages, EfiBootServicesData, 1 page
ha = int.from_bytes(host, "little")
nbytes = bytearray((4096).to_bytes(8, "little"))
dev = bytearray(8)
mapping = bytearray(8)
p.map(2, ha, nbytes, dev, mapping)  # EfiPciIoOperationBusMasterCommonBuffer
da = int.from_bytes(dev, "little")
print("mapped", int.from_bytes(nbytes, "little") == 4096, da != 0 and da % 4096 == 0)
p.unmap(int.from_bytes(mapping, "little"))
p.free_buffer(1, ha)
p.close()
print("dma ok")
