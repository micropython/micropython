# Device-path file-path accessor: the Media/FilePath node of our own LoadedImage
# resolves to the app's .efi filename. (Parsing / to_text round-trips: efi_loaded_image.py.)
try:
    import uefi
    from uefi import protocols as P, device_path as DP
except ImportError:
    print("SKIP")
    raise SystemExit

img = uefi.Handle(uefi.raw.image_handle())
li = P.LOADED_IMAGE.open(img)
fp = li.file_path
if fp:
    name = DP.DevicePath.from_ptr(fp).file_path()  # e.g. \EFI\BOOT\BOOTAA64.EFI
    print(name.upper().endswith(".EFI"))
else:
    print(True)  # some launch paths have no file path node
li.close()
print("uefi devicepath ok")
