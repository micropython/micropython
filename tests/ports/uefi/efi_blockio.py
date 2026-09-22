# BLOCK_IO: locate disks, overlay media geometry via a uctypes struct, and dispatch a
# method (Reset) through the EFIAPI trampoline.
try:
    import uefi
    from uefi import protocols as P
except ImportError:
    print("SKIP")
    raise SystemExit

disks = uefi.handle.locate_handles(uefi.guid.BLOCK_IO_PROTOCOL)
print(len(disks) > 0)
phys = None
media = None
for h in disks:
    bio = P.BLOCK_IO.open(h)
    m = bio.struct_at("media", P.BLOCK_IO_MEDIA)
    if m.media_present and not m.logical_partition and m.block_size > 0:
        phys = bio
        media = m
        break
    bio.close()
print(phys is not None)
print(media.block_size in (512, 4096))
phys.reset(False)  # method stub -> EFIAPI trampoline; raises on error
phys.close()
print("blockio ok")
