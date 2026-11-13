# ProtocolWatch over RegisterProtocolNotify: installing a fresh protocol interface fires
# the watch, which reports the new handle; then uninstall cleans up.
try:
    import uefi
    from uefi import event as EV
except ImportError:
    print("SKIP")
    raise SystemExit

PG = uefi.GUID("aabbccdd-1122-3344-5566-77889900aabb")
w = EV.ProtocolWatch(PG)
print(w.poll())  # nothing installed yet -> []
iface = uefi.PoolBuffer(16)  # a real (non-null) interface pointer
st, nh = uefi.raw.install_protocol_interface(0, PG.bytes, iface.ptr)
print(st)
arr = w.poll()
print(len(arr) >= 1)  # the watch saw the new handle
print(arr[0].ptr == nh)
uefi.raw.uninstall_protocol_interface(nh, PG.bytes, iface.ptr)
iface.close()
w.close()
print("protocolwatch ok")
