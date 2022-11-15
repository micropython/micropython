include("$(PORT_DIR)/boards/manifest.py")

# Networking
require("webrepl")
require("urequests")
require("ntptime")

# Drivers
require("lsm6dsox")
require("espflash")

# Utils
require("logging")

# Bluetooth
require("aioble", client=True, central=True, l2cap=True, security=True)
