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
b = [bytearray(8) for _ in range(4)]
p.get_location(b[0], b[1], b[2], b[3])  # seg / bus / dev / fn
loc = tuple(int.from_bytes(x, "little") for x in b)
print(len(loc) == 4 and all(isinstance(v, int) for v in loc))
p.close()
print("pci ok")
