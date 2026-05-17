# Shared manifest for LEGO_HUB_NO6 & LEGO_HUB_NO7.

include("$(PORT_DIR)/boards/manifest.py")

# Modules for application firmware update.
module("fwupdate.py", base_path="$(PORT_DIR)/mboot", opt=3)
module("appupdate.py", opt=3)
