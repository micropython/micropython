# Coercion class 1: every object that carries a firmware token as `.ptr` converts to
# that pointer under int(), so it can be handed straight to the raw C surface (which
# reads pointer arguments as plain integers) with no `.ptr` at the call site.
try:
    import uefi
except ImportError:
    print("SKIP")
    raise SystemExit

G = uefi.guid.BLOCK_IO_PROTOCOL
agent = uefi.raw.image_handle()

# ── Handle ──────────────────────────────────────────────────────────────────
h = uefi.handle.locate_handles(G)[0]
print(int(h) == h.ptr)
print(uefi.Handle(h) == h)  # Handle(Handle) round-trips
print(uefi.raw.protocols_per_handle(h)[0] == uefi.status.SUCCESS)
st, iface = uefi.raw.open_protocol(h, G, uefi.Handle(agent), 0, uefi.handle.GET_PROTOCOL)
print(st == uefi.status.SUCCESS and iface != 0)  # handle AND agent as objects

# ── ProtocolHandle / ProtocolWrapper ────────────────────────────────────────
ph = uefi.handle.ProtocolHandle.open(h, G)
print(int(ph) == ph.ptr)
w = uefi.protocols.BLOCK_IO.wrap(ph)  # wrap() takes the ProtocolHandle itself
print(int(w) == ph.ptr and w.revision != 0)
ph.close()

# ── PoolBuffer ──────────────────────────────────────────────────────────────
pb = uefi.PoolBuffer(64)
print(int(pb) == pb.ptr)
print(uefi.raw.free_pool(pb) == uefi.status.SUCCESS)
pb.ptr = 0  # freed by hand above; disarm close()/__del__

# ── Event (the C type) ──────────────────────────────────────────────────────
ev = uefi.Event(timer=True)
print(int(ev) == ev.ptr)
print(uefi.raw.set_timer(ev, 0, 0) == uefi.status.SUCCESS)  # 0 = cancel, a no-op
ev.close()

# ── Image ───────────────────────────────────────────────────────────────────
img = uefi.Image(agent)
print(int(img) == agent)
li = uefi.protocols.LOADED_IMAGE.open(img)  # an Image is handle-shaped
print(li.ptr != 0)
li.close()

print("uefi ptr coercion ok")
