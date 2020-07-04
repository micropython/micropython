include("manifest.py")

# drivers
freeze("$(MPY_DIR)/drivers/display", "ssd1306.py")

# file utilities
freeze("$(MPY_LIB_DIR)/upysh", "upysh.py")

# requests
freeze("$(MPY_LIB_DIR)/urequests", "urequests.py")
freeze("$(MPY_LIB_DIR)/urllib.urequest", "urllib/urequest.py")

# umqtt with examples
freeze("$(MPY_LIB_DIR)/umqtt.simple", "umqtt/simple.py")
freeze("$(MPY_LIB_DIR)/umqtt.robust", "umqtt/robust.py")
freeze("$(MPY_LIB_DIR)/umqtt.simple", "example_pub_button.py")
freeze("$(MPY_LIB_DIR)/umqtt.simple", "example_sub_led.py")

# HTTP examples
freeze("$(MPY_DIR)/examples/network", "http_client.py")
freeze("$(MPY_DIR)/examples/network", "http_client_ssl.py")
freeze("$(MPY_DIR)/examples/network", "http_server.py")
freeze("$(MPY_DIR)/examples/network", "http_server_ssl.py")
