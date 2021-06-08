# base modules
include("$(PORT_DIR)/boards/manifest.py")

# uasyncio
include("$(MPY_DIR)/extmod/uasyncio/manifest.py")

# drivers
freeze("$(MPY_DIR)/drivers/display", "ssd1306.py")

# Libraries from micropython-lib, include only if the library directory exists
if os.path.isdir(convert_path("$(MPY_LIB_DIR)")):
    # file utilities
    freeze("$(MPY_LIB_DIR)/micropython/upysh", "upysh.py")

    # requests
    freeze("$(MPY_LIB_DIR)/python-ecosys/urequests", "urequests.py")
    freeze("$(MPY_LIB_DIR)/micropython/urllib.urequest", "urllib/urequest.py")

    # umqtt
    freeze("$(MPY_LIB_DIR)/micropython/umqtt.simple", "umqtt/simple.py")
    freeze("$(MPY_LIB_DIR)/micropython/umqtt.robust", "umqtt/robust.py")
