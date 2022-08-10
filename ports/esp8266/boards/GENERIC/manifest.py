# base modules
include("$(PORT_DIR)/boards/manifest.py")

# uasyncio
include("$(MPY_DIR)/extmod/uasyncio/manifest.py")

# drivers
freeze("$(MPY_DIR)/drivers/display", "ssd1306.py")

# micropython-lib: file utilities
freeze("$(MPY_LIB_DIR)/micropython/upysh", "upysh.py")

# micropython-lib: requests
freeze("$(MPY_LIB_DIR)/python-ecosys/urequests", "urequests.py")
freeze("$(MPY_LIB_DIR)/micropython/urllib.urequest", "urllib/urequest.py")

# micropython-lib: umqtt
freeze("$(MPY_LIB_DIR)/micropython/umqtt.simple", "umqtt/simple.py")
freeze("$(MPY_LIB_DIR)/micropython/umqtt.robust", "umqtt/robust.py")
