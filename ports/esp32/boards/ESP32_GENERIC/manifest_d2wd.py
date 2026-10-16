# D2WD variant manifest: the 2MiB build has no room for the full frozen set
# Compared to boards/manifest.py this removes:
# - dht
# - ds18x20
# These can instead be installed from micropython-lib via mip when needed.

# Useful networking-related packages.
require("bundle-networking")

# Require some micropython-lib modules.
require("aioespnow")
require("neopixel")
require("onewire")
require("umqtt.robust")
require("umqtt.simple")
require("upysh")
