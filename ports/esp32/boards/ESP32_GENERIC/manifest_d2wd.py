# D2WD variant manifest: asyncio and the hardware libs only. The 2MiB build
# has no room for the full frozen set alongside BLE, so the packages dropped
# relative to boards/manifest.py (umqtt, aioespnow, upysh and the networking
# bundle) install from micropython-lib via mip when needed.
freeze("$(PORT_DIR)/modules")
include("$(MPY_DIR)/extmod/asyncio")

# Require some micropython-lib modules.
require("dht")
require("ds18x20")
require("neopixel")
require("onewire")
