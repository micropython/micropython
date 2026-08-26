# The worked example from README.md ('The uefi API' -> 'Values compose'), run
# verbatim, so the documented composition can never drift from what the API does.
# Keep the body in step with the README snippet; the tail after it is this test's own.
try:
    import uefi
except ImportError:
    print("SKIP")
    raise SystemExit

# A handle read out of an interface struct is a plain integer ...
li = uefi.protocols.LOADED_IMAGE.open(uefi.raw.image_handle())
volume = li.device_handle
li.close()

# ... and that is enough to mount it, ask what it supports, or name its path.
uefi.fs.mount(volume, "/boot")
print(uefi.Handle(volume).supports(uefi.guid.SIMPLE_FILE_SYSTEM_PROTOCOL))
path = uefi.Handle(volume).device_path()

# A GUID goes to the raw layer as-is, in any spelling ...
status, block_io = uefi.raw.locate_protocol(uefi.guid.BLOCK_IO_PROTOCOL)

# ... a device path is a byte block wherever one is wanted ...
MY_VARS = uefi.GUID("2f4e6b1a-1111-4c2d-9e3a-0123456789ab")  # your own namespace
uefi.variable.set("MyPath", MY_VARS, path, uefi.variable.BOOTSERVICE_ACCESS)

# ... and its text is a device path wherever one is wanted.
option = uefi.LoadOption("My OS", path.to_text())

# (not part of the README snippet: tidy up and prove the results are real)
print(isinstance(volume, int), option.description)
print(status == 0 and uefi.variable.get("MyPath", MY_VARS) == path.to_bytes())
uefi.variable.delete("MyPath", MY_VARS)
uefi.fs.umount("/boot")
print("README example ok")
