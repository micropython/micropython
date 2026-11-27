# uefi.buffer.PageBuffer (BS->AllocatePages/FreePages) and the raw allocate_pages pair.
try:
    import uefi
    from uefi import buffer, raw
except ImportError:
    print("SKIP")
    raise SystemExit

# Rounded up to whole, page-aligned pages; readable and writable through the view.
with buffer.PageBuffer(5000) as pb:
    print(len(pb), pb.pages, pb.ptr % buffer.PAGE_SIZE)
    pb.write(b"hello", 4090)
    print(pb.bytes(4090, 5), pb[4094])
    print(repr(pb).startswith("PageBuffer(ptr=0x"))
print(pb.ptr, len(pb))  # closed: freed and emptied

# A 32-bit DMA limit keeps the whole buffer below 4 GiB.
with buffer.PageBuffer(8192, max_address=0xFFFF_FFFF) as pb:
    print(pb.ptr + len(pb) - 1 <= 0xFFFF_FFFF)

# AllocateAddress: free a page, then ask for exactly that address back.
st, addr = raw.allocate_pages(buffer.ALLOCATE_ANY_PAGES, buffer.BOOT_SERVICES_DATA, 1)
print(st == 0, raw.free_pages(addr, 1) == 0)
with buffer.PageBuffer(1, address=addr) as pb:
    print(pb.ptr == addr)

# from_bytes is inherited from PoolBuffer.
with buffer.PageBuffer.from_bytes(b"abc") as pb:
    print(pb.bytes(0, 3), len(pb))

for kw in ({"max_address": 0xFFFF_FFFF, "address": 0x100000}, {"size": -1}):
    try:
        buffer.PageBuffer(**dict({"size": 1}, **kw))
    except ValueError as e:
        print("ValueError:", e)

# Firmware errors surface as uefi.Error (an unaligned AllocateAddress is rejected).
try:
    buffer.PageBuffer(1, address=addr + 1)
except uefi.Error as e:
    print(e.name)
