include("$(PORT_DIR)/boards/manifest.py")

# Modules for application firmware update.
freeze("$(PORT_DIR)/mboot", "fwupdate.py", opt=3)
freeze("$(PORT_DIR)/boards/LEGO_HUB_NO6", ("spiflash.py", "appupdate.py"), opt=3)
