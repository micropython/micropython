# NVRAM variables via uefi.variable, in a private GUID namespace and volatile
# (BOOTSERVICE_ACCESS, no NON_VOLATILE) so nothing persists / no flash is written.
try:
    import uefi
    from uefi import variable as V
except ImportError:
    print("SKIP")
    raise SystemExit

NS = uefi.GUID("11112222-3333-4444-5555-666677778888")
print(V.exists("mpytest", NS))
V.set("mpytest", NS, b"hello", V.BOOTSERVICE_ACCESS)
print(V.exists("mpytest", NS))
print(V.get("mpytest", NS))
a, d = V.get_with_attributes("mpytest", NS)
print(d, bool(a & V.BOOTSERVICE_ACCESS))
names = [n for (n, g) in V.enumerate(NS)]
print("mpytest" in names)
V.set("mpytest", NS, b"world!!", V.BOOTSERVICE_ACCESS)
print(V.get("mpytest", NS))
V.delete("mpytest", NS)
print(V.exists("mpytest", NS))
print("uefi variables ok")
