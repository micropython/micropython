include("$(PORT_DIR)/boards/manifest.py")

# Networking
require("bundle-networking")

# Utils
require("time")
require("senml")
require("logging")

# Bluetooth
require("aioble")

# Register external library
add_library("arduino-lib", "$(ARDUINO_LIB_DIR)")

# CMWX1 Lora module.
require("cmwx1", library="arduino-lib")
