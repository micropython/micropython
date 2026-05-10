try:
    import network
except ImportError:
    print("SKIP")
    raise SystemExit

if not hasattr(network, "WLAN"):
    print("SKIP")
    raise SystemExit

wlan = network.WLAN(network.WLAN.IF_STA)
wlan.active(True)

# protocol/bandwidth should always remain usable.
protocol_before = wlan.config("protocol")
wlan.config(protocol=protocol_before)
print("protocol", wlan.config("protocol") == protocol_before)

bandwidth_before = wlan.config("bandwidth")
wlan.config(bandwidth=bandwidth_before)
print("bandwidth", wlan.config("bandwidth") == bandwidth_before)

# band_mode API is always exposed, with 2.4GHz-only fallback on legacy targets.
band_mode_before = wlan.config("band_mode")
print("band_mode_before", band_mode_before)

wlan.config(band_mode=network.WLAN.BAND_MODE_2G_ONLY)
print("band_mode_2g", wlan.config("band_mode"))

try:
    wlan.config(band_mode=network.WLAN.BAND_MODE_AUTO)
    print("band_mode_auto", wlan.config("band_mode"))
except OSError:
    print("band_mode_auto", "unsupported")

# Restore original mode where possible.
try:
    wlan.config(band_mode=band_mode_before)
except OSError:
    pass
