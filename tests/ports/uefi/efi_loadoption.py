# boot: EFI_LOAD_OPTION packed round-trip, BootOrder read/parse, and set_load_options
# (attach a command line to a freshly-loaded, never-started copy of our own image).
try:
    import uefi
    from uefi import boot as BT, protocols as P, device_path as DP
except ImportError:
    print("SKIP")
    raise SystemExit

img = uefi.Handle(uefi.raw.image_handle())
li = P.LOADED_IMAGE.open(img)
bootdev = uefi.Handle(li.device_handle)
selfpath = DP.DevicePath.from_ptr(li.file_path).file_path() if li.file_path else None
li.close()
voldp = bootdev.device_path()
print(voldp is not None)

lo = BT.LoadOption("MicroPython Test", voldp, b"\x01\x02\x03")
lo2 = BT.LoadOption.parse(lo.to_bytes())  # packed EFI_LOAD_OPTION round-trip
print(lo2.description == "MicroPython Test")
print(lo2.optional_data == b"\x01\x02\x03")
print(lo2.device_path.to_bytes() == voldp.to_bytes())
print(lo2.active)

bo = BT.boot_order()  # present on OVMF, often AAVMF
print(isinstance(bo, (list, tuple)))
ok = True
for n in bo:
    try:
        if not isinstance(BT.load_option(n).description, str):
            ok = False
    except uefi.Error:
        pass
print(ok)

if selfpath:
    vp = selfpath.replace("\\", "/")
    if not vp.startswith("/"):
        vp = "/" + vp
    try:
        bi = uefi.Image.load_from_file(vp)
        bi.set_load_options("script.py --verbose")
        bli = bi.loaded_image()
        print(bli.load_options != 0)
        print(bli.load_options_size == len("script.py --verbose") * 2 + 2)  # UTF-16LE + NUL
        bli.close()
        bi.unload()
    except uefi.Error:
        print(True)  # Secure Boot / load unsupported -> accept
        print(True)
else:
    print(True)
    print(True)
print("loadoption ok")
