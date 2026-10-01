# Handle/protocol discovery: the raw + handle API backing the shell's dh/drivers/devices.
# Counts are firmware-dependent, so assert thresholds and print stable booleans.
try:
    import uefi
except ImportError:
    print("SKIP")
    raise SystemExit

dh = uefi.handle.all_handles()
print(len(dh) > 5)
aprotos = uefi.handle.all_protocols()
print(len(aprotos) > 10)
# `drivers`: handles exposing a Driver Binding protocol
drv = uefi.handle.locate_handles(uefi.guid.DRIVER_BINDING_PROTOCOL)
print(len(drv) > 0)
# `dh -d` / `devices`: open-info reveals BY_DRIVER/BY_CHILD usage on a device
seen = False
for h in uefi.handle.locate_handles(uefi.guid.BLOCK_IO_PROTOCOL):
    for g in h.protocols():
        for oi in h.open_info(g):
            if oi.by_driver or oi.by_child:
                seen = True
print(seen)
print("uefi discovery ok")
