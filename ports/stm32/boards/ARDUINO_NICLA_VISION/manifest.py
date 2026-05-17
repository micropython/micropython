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

# RPC
require("msgpackrpc", library="arduino-lib")

# SE05x driver
require("se05x", library="arduino-lib")
