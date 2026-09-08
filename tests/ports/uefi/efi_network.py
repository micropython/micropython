# network module surface that's present with or without a NIC (the hermetic harness has
# none): the module loads, discovery is lazy, and hostname/country round-trip. NIC/DHCP/
# Wi-Fi paths need the --net firmware and live in efi_network_nic.py (SKIP without a NIC).
try:
    import network
except ImportError:
    print("SKIP")
    raise SystemExit

ifs = network.interfaces()
print(isinstance(ifs, (list, tuple)))
network.hostname("mpy-uefi")
print(network.hostname() == "mpy-uefi")
print(network.country() == "XX")  # default
network.country("GB")
print(network.country() == "GB")
print("network ok")
