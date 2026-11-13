# Test that the legacy nic.ifconfig() setter leaves the DNS server usable.
#
# This test needs an already-connected network interface with a DNS server and
# Internet access.  It re-applies the interface's current IPv4 configuration
# via nic.ifconfig((ip, mask, gateway, dns)), then resolves a name.

import sys

if sys.implementation.name != "micropython":
    print("SKIP")
    raise SystemExit

try:
    import network
    import socket
except ImportError:
    print("SKIP")
    raise SystemExit


def get_nic():
    candidates = []
    if hasattr(network, "WLAN") and hasattr(network, "STA_IF"):
        candidates.append(lambda: network.WLAN(network.STA_IF))
    if hasattr(network, "LAN"):
        candidates.append(network.LAN)
        candidates.append(lambda: network.LAN(0))
    for make_nic in candidates:
        try:
            nic = make_nic()
            if nic.active() and (not hasattr(nic, "isconnected") or nic.isconnected()):
                return nic
        except (AttributeError, OSError, TypeError, ValueError):
            pass
    return None


nic = get_nic()
if nic is None:
    print("SKIP")
    raise SystemExit

ip, netmask, gateway, dns = nic.ifconfig()
if ip == "0.0.0.0" or dns == "0.0.0.0":
    print("SKIP")
    raise SystemExit

# Require working name resolution first, so a failure after reconfiguration
# can be attributed to ifconfig().
try:
    socket.getaddrinfo("micropython.org", 80)
except OSError:
    print("SKIP")
    raise SystemExit

nic.ifconfig((ip, netmask, gateway, dns))
socket.getaddrinfo("micropython.org", 80)

print("OK")
