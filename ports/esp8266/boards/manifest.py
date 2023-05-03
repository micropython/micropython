freeze("$(PORT_DIR)/modules")
require("bundle-networking")
require("dht")
require("ds18x20")
require("neopixel")
require("onewire")

package("ST7735", base_path="$(PORT_DIR)/libraries")
package("CircuitOS", base_path="$(PORT_DIR)/libraries")
