# Test network.USBD_NCM API surface.

try:
    import network

    network.USBD_NCM
except:
    print("SKIP")
    raise SystemExit

# USBD_NCM() must return a singleton.
nic = network.USBD_NCM()
assert nic is network.USBD_NCM(), "USBD_NCM() must be a singleton"

# Required methods must be present.
for method in ("active", "isconnected", "status", "ifconfig", "ipconfig"):
    assert hasattr(nic, method), "missing method: {}".format(method)

# active() with no args returns a bool.
state = nic.active()
assert isinstance(state, bool), "active() must return bool"

# isconnected() returns a bool.
assert isinstance(nic.isconnected(), bool), "isconnected() must return bool"

# status() returns an int (link status code).
assert isinstance(nic.status(), int), "status() must return int"

print("done")
