include("../manifest.py")

module("upip.py", base_path="$(MPY_DIR)/tools", opt=3)
module("upip_utarfile.py", base_path="$(MPY_DIR)/tools", opt=3)

require("ntptime")
require("urequests")
