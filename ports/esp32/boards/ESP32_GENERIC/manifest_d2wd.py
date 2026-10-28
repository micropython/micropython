# D2WD variant manifest: asyncio and the hardware libs only. The packages
# dropped relative to boards/manifest.py (umqtt, aioespnow, upysh and the
# networking bundle) install from micropython-lib via mip when needed.
freeze("$(PORT_DIR)/modules")
include("$(MPY_DIR)/extmod/asyncio")

# Require some micropython-lib modules.
require("dht")
require("ds18x20")
require("neopixel")
require("onewire")
