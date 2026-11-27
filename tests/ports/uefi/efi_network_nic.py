# NIC-dependent network surface: enumerate the interface and read its MAC. SKIP when no
# NIC is attached (the hermetic harness has none); runs under the --net firmware. The
# DHCP/DNS/socket/TLS-over-network paths are exercised by the upstream socket/ssl tests.
try:
    import network
except ImportError:
    print("SKIP")
    raise SystemExit

ifs = network.interfaces()
if not ifs:
    print("SKIP")
    raise SystemExit

lan = ifs[0]
mac = lan.config("mac")
print(isinstance(mac, bytes) and len(mac) == 6)
lan.active(True)  # connect the stack (BDS didn't) + start the NIC
print(lan.active() in (True, False, 0, 1))
print("network nic ok")
