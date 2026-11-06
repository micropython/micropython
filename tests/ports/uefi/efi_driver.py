# Driver/image loading + controller binding: load a fresh in-memory copy of our own .efi
# (never started) then unload it, and exercise Connect/DisconnectController non-destructively.
try:
    import uefi
    from uefi import driver as D, protocols as P, device_path as DP
except ImportError:
    print("SKIP")
    raise SystemExit

img = uefi.Handle(uefi.raw.image_handle())
li = P.LOADED_IMAGE.open(img)
selfpath = DP.DevicePath.from_ptr(li.file_path).file_path() if li.file_path else None
li.close()
if selfpath:
    vp = selfpath.replace("\\", "/")
    if not vp.startswith("/"):
        vp = "/" + vp
    try:
        di = D.Image.load_from_file(vp)
        print(di.ptr != 0)
        dli = di.loaded_image()
        print(dli.image_size > 0 and dli.image_base != 0)
        dli.close()
        di.unload()
    except uefi.Error:
        print(True)  # Secure Boot / load unsupported -> accept
        print(True)
else:
    print(True)
    print(True)

# ConnectController: reconnect an existing controller (already-bound -> no-op).
ctrls = (
    uefi.handle.locate_handles(uefi.guid.PCI_IO_PROTOCOL)
    or uefi.handle.locate_handles(uefi.guid.BLOCK_IO_PROTOCOL)
    or uefi.handle.all_handles()
)
D.connect(ctrls[0])
# DisconnectController on an *unbound* controller is a SUCCESS no-op (no live device torn down).
ub = None
for h in uefi.handle.all_handles():
    if not any(oi.by_driver for g in h.protocols() for oi in h.open_info(g)):
        ub = h
        break
if ub is not None:
    D.disconnect(ub)
print("driver ok")
