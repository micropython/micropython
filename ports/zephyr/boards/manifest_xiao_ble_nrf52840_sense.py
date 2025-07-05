# This is an example frozen module manifest. Enable this by configuring
# the Zephyr project and enabling the frozen modules config feature.

# freeze("../modules")
include("$(MPY_DIR)/extmod/asyncio")

# Require a micropython-lib module.
require("upysh")
require("aioble")
require("neopixel")
require("aiorepl")
