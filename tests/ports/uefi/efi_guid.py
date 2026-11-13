# uefi.GUID casting: GUID / canonical string / bare-hex / {braces} / bytes are all
# accepted and equal, endpoints take any form interchangeably, bad strings raise.
try:
    import uefi
except ImportError:
    print("SKIP")
    raise SystemExit

ga = uefi.GUID("964e5b21-6459-11d2-8e39-00a0c969723b")
gb = uefi.GUID("964e5b21645911d28e3900a0c969723b")
gc = uefi.GUID("{964E5B21-6459-11D2-8E39-00A0C969723B}")
gd = uefi.GUID(ga.bytes)
print(ga == gb == gc == gd == uefi.guid.BLOCK_IO_PROTOCOL)
print(uefi.GUID(uefi.guid.BLOCK_IO_PROTOCOL) == ga)
print(uefi.guid.name_for(uefi.guid.BLOCK_IO_PROTOCOL))
print(uefi.guid.name_for(uefi.GUID("00000000-0000-0000-0000-000000000000")))
# every form locates the same handles
for gform in (
    uefi.guid.BLOCK_IO_PROTOCOL,
    "964e5b21-6459-11d2-8e39-00a0c969723b",
    "964e5b21645911d28e3900a0c969723b",
    ga.bytes,
):
    assert len(uefi.handle.locate_handles(gform)) > 0
try:
    uefi.GUID("nonsense")
    print("bad guid accepted")
except ValueError:
    print("ValueError")
print("uefi guid casting ok")
