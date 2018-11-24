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
    
In either case where the device enters AP mode, the onboard LED ocated just above the button will blink rapidly for 5 seconds.

## Instructions for use/testing firmware:
1. Clone this repository
2. Flash device with the firmware found `micropython/ports/esp32/build/firmware.bin` (in the same directory as this README)
3. On first boot, observe the device enter AP mode (onboard LED blinks rapidly for 5 seconds)
4. Connect to wlan with SSID `IoTanium-<device MAC address>`, password `iotanium`
5. Open `micropython/webrepl/webrepl.html` in a browser
6. In the webrepl, connect to the default target `ws://192.168.4.1:8266/` password is `iotanium`
7. Edit `micropython/iotanium_cfg.json` with your wifi connection information, and save the file
8. Use the webrepl to transfer `iotanium_cfg.json` to the device.  (ensure you are successfully authenticated in step 6 above, or file transfer will fail)