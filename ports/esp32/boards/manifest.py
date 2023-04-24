freeze("$(PORT_DIR)/modules")
include("$(MPY_DIR)/extmod/uasyncio")

package("ST7735", base_path="$(PORT_DIR)/libraries")
package("ILI9341", base_path="$(PORT_DIR)/libraries")
package("CircuitOS", base_path="$(PORT_DIR)/libraries")

# Useful networking-related packages.
require("bundle-networking")

require("urequests")
require("mip")
require("ntptime")
require("webrepl")

# Require some micropython-lib modules.
require("dht")
require("ds18x20")
require("neopixel")
require("onewire")
require("umqtt.robust")
require("umqtt.simple")
require("upysh")
