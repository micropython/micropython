# Shared manifest for LEGO_HUB_NO6 & LEGO_HUB_NO7.

include("$(PORT_DIR)/boards/manifest.py")

# Modules for application firmware update.
freeze("$(PORT_DIR)/mboot", "fwupdate.py", opt=3)
freeze("$(BOARD_DIR)", ("spiflash.py", "appupdate.py"), opt=3)
