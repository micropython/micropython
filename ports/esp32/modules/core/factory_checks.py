import esp32

nvs = esp32.NVS("system")
nvs.set_i32('factory_checked', 3)
nvs.commit()