include("manifest.py")

freeze("$(MPY_LIB_DIR)/upysh", "upysh.py")
freeze("$(MPY_LIB_DIR)/urequests", "urequests.py")
freeze("$(MPY_LIB_DIR)/umqtt.simple", "umqtt/simple.py")
freeze("$(MPY_LIB_DIR)/umqtt.robust", "umqtt/robust.py")
