# IoTanium DevKit PREVIEW Firmware
---
## Boot workflow:
- **Button not held during boot:**
    - Attempts to connect to all wlans configured in `iotanium_cfg.json` on the filesystem
    - Skips configured wlans that are not in range of the device
    - If no config exists, or all wlan connections fail, enables the AP
- **Button held during boot:**
    - skips connecting to wlans
    - enables the AP
    
In either case where the device enters AP mode, the onboard LED located just above the button will blink rapidly for 5 seconds.

## Instructions for use/testing firmware:

### Flash your device
1. Clone this repository
1. Flash device with the firmware found at `micropython/ports/esp32/build/firmware.bin` (in the same directory as this README)

### Connect your workstation to device AP
1. On first boot, observe the device enter AP mode (onboard LED blinks rapidly for 5 seconds)
1. Connect your workstation to WLAN with SSID `IoTanium-<device MAC address>`, password `iotanium`

### Connect to webrepl and upload your wifi credentials
1. Open `micropython/webrepl/webrepl.html` in a browser
1. In the webrepl, connect to the default target `ws://192.168.4.1:8266/` password is `iotanium`
1. Edit the file in the repo `micropython/iotanium_cfg.json` with your wifi connection information, and **save the file**
1. Use the webrepl's `Choose File > Send To Device` feature to upload `iotanium_cfg.json` to the device.  (ensure you are successfully authenticated in step 2. above, or file transfer will fail)

### Connect your device to wifi
1. In the webrepl console, run `iotanium.setup()` and observe the output, similar to that below:
```
>>> iotanium.setup()
config file ./iotanium_cfg.json found
wifi scanning
found wlan MY_WIFI
connecting to wlan home office/MY_WIFI...
connection to wlan home office/MY_WIFI successful! To continue:
1. reconnect your workstation to wifi network: MY_WIFI
2. once reconnected, connect to webrepl on:
ws://192.168.1.243:8266/                               
>>>   
```
2. Make a note of the webrepl target in your output, similar to: `ws://192.168.1.243:8266/` (your output will be the same format, but a different address)

### Connect to your device over wifi
1. Disconnect from the AP, and reconnect to your local WIFI
2. **Refresh** the webrepl client in your browser, and reconnect to the target noted above, password `iotanium`
3. You should now be connected to your device via webrepl over your wifi:
```
Welcome to MicroPython!
Password: <masked>
WebREPL connected
>>>
```

### Test Safe Mode Button Behavior
1. Connect to device via serial to observe console logging
2. Test button press during boot to validate device skips wifi and enables AP

### Test IoTanium helper functions
```
>>>iotanium.led('on')
>>>iotanium.led('off')
>>>iotanium.blink(100,10) #delay in milliseconds, duration in seconds
>>>iotanium.hall() #hold magnet close to ESP32 chip to change reading
>>>iotanium.temp() #value is crude, not calibrated to anything, and only changes in small increments
>>>iotanium.touch() #capacitive touch on pin 12
>>> iotanium.read_all() #returns {"touch": 414, "hall": 74, "temp": 126}
```
