# CPU_IO2 cross-checked against PCI_IO and machine.mem32: three routes to one device
# register. On x64, also CPU_IO2 port I/O through the 0xCF8/0xCFC config mechanism.
try:
    import machine
    import uctypes
    import uefi
    from uefi import protocols as P
except ImportError:
    print("SKIP")
    raise SystemExit

pci = uefi.handle.locate_handles(uefi.guid.PCI_IO_PROTOCOL)
if not pci:
    print("SKIP")
    raise SystemExit

W32 = 2  # Uint32 in both the PCI_IO and CPU_IO2 width enums


def bar_base(p, bar):
    # The BAR's CPU address as GetBarAttributes reports it: a QWORD address-space
    # descriptor whose AddrRangeMin is the host address, any root-bridge translation
    # already applied. (BarIndex counts BARs, not config dwords, so config space
    # can't be indexed by it once a 64-bit BAR comes first.)
    res = bytearray(8)
    p.get_bar_attributes(bar, None, res)
    rp = int.from_bytes(res, "little")
    d = uctypes.bytes_at(rp, 22)
    uefi.raw.free_pool(rp)
    if d[0] != 0x8A or d[3] != 0:
        return None  # not described as memory (seen on real hardware): can't cross-check
    return int.from_bytes(d[14:22], "little")


# Find a memory BAR that GetBarAttributes also describes: PCI_IO.mem_read rejects
# the other kinds.
buf = bytearray(4)
found = None
for h in pci:
    p = P.PCI_IO.open(h)
    for bar in range(6):
        try:
            p.mem_read(W32, bar, 0, 1, buf)
        except uefi.Error:
            continue
        base = bar_base(p, bar)
        if base is None:
            continue
        found = (p, base, int.from_bytes(buf, "little"))
        break
    if found:
        break
    p.close()
p, base, val = found

cpu = P.CPU_IO2.locate()
b2 = bytearray(4)
cpu.mem_read(W32, base, 1, b2)
print("cpu_io2 mem_read matches", int.from_bytes(b2, "little") == val)
print("machine.mem32 matches", machine.mem32[base] == val)

# Our own PE header says which architecture we are.
li = P.LOADED_IMAGE.open(uefi.Handle(uefi.raw.image_handle()))
pe = li.image_base + machine.mem32[li.image_base + 0x3C]
x64 = machine.mem16[pe + 4] == 0x8664
li.close()
if x64:
    seg, bus, dev, fn = p.get_location()
    addr = 0x80000000 | bus << 16 | dev << 11 | fn << 8
    cpu.io_write(W32, 0xCF8, 1, addr.to_bytes(4, "little"))
    d = bytearray(4)
    cpu.io_read(W32, 0xCFC, 1, d)
    ids = bytearray(4)
    p.pci_read(W32, 0, 1, ids)
    ok = d == ids
else:
    ok = True  # no x86 port I/O on aarch64
print("port io matches", ok)
p.close()
cpu.close()
print("cpu io ok")
