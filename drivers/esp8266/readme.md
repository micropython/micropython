# Controlling ESP8266 via Micropython

Enables controlling the ESP8266 board via Python and - in the background - AT commands.

## Content
esp8266.py: ESP8266 library.
Handles UART-based communication to ESP8266 board, translates Python commands to AT+ commands for ESP8266 and processes responses.

settings.py:
To define UART parameters (hardware setup), WiFi parameters (ssid and password), and parameters for get / put / post requests.

example.py:
Example file, showing how to connect to the ESP8266 board, list WiFi access points, connect to WiFi, perform a get / put / post request, etc.

## Usage
Copy all 3 files:
  esp8266.py
  settings.py
  example.py
to the root of your micropython board.

Edit the settings.py file:
  - UART parameters (hardware setup),
  - WiFi parameters (ssid and password),
  - parameters for get / put / post requests.

Launch example.py

Tested with Raspberry Pico + Waveshare ESP8266
