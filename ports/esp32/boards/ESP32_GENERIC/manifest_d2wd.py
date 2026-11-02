# D2WD variant manifest: the 2MiB build has no room for the full frozen set
# Compared to boards/manifest.py this removes:
# - aioespnow
# - dht
# - ds18x20
# - onewire
# - umqtt.robust
# - umqtt.simple
# - upysh
# - webrepl
# These can instead be installed from micropython-lib via mip when needed.

freeze("$(PORT_DIR)/modules")
include("$(MPY_DIR)/extmod/asyncio")

# Require some micropython-lib modules.
require("neopixel")
require("mip")
require("ntptime")
require("ssl")
require("requests")
