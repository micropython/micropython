include("$(PORT_DIR)/boards/manifest.py")

# Networking
require("webrepl")
require("urequests")

# Drivers
require("lsm6dsox")

# Bluetooth
require("aioble", client=True, central=True, l2cap=True, security=True)
