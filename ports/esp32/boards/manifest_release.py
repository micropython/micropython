include("manifest.py")

freeze("$(MPY_LIB_DIR)/python-ecosys/urequests", "urequests.py")

freeze("$(MPY_LIB_DIR)/micropython/upysh", "upysh.py")
freeze("$(MPY_LIB_DIR)/micropython/umqtt.simple", "umqtt/simple.py")
freeze("$(MPY_LIB_DIR)/micropython/umqtt.robust", "umqtt/robust.py")
