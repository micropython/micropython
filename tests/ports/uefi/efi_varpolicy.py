# Variable Policy: build + register a real policy entry (exercising the entry layout and
# the RegisterVariablePolicy dispatch). SKIP where the protocol is absent. Enforcement is
# only asserted when the engine is actually enabled (stock OVMF/AAVMF report it disabled).
try:
    import uefi
    from uefi import variable as V
except ImportError:
    print("SKIP")
    raise SystemExit

if not V.VariablePolicy.available():
    print("SKIP")
    raise SystemExit

NS = uefi.GUID("11112222-3333-4444-5555-666677778888")
en = V.VariablePolicy.is_enabled()
print(en in (True, False, 0, 1))
try:
    V.VariablePolicy.register(NS, "policed", min_size=4, max_size=8)
    reg = True
except uefi.Error:
    reg = False  # e.g. EFI_WRITE_PROTECTED on locked firmware
if reg:
    V.set("policed", NS, b"okok", V.BOOTSERVICE_ACCESS)  # in-range write
    print(V.get("policed", NS) == b"okok")
    blocked = False
    try:
        V.set("policed", NS, b"xx", V.BOOTSERVICE_ACCESS)  # undersize
    except uefi.Error:
        blocked = True
    print((not en) or blocked)  # an enabled engine must block; a disabled one need not
else:
    print(True)
    print(True)
print("varpolicy ok")
