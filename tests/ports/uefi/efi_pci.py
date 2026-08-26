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
