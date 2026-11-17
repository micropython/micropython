# ESP32 Wi-Fi CSI Examples

These examples show how to use Wi-Fi CSI on supported ESP32 builds.

For API details, firmware requirements, and the meaning of the frame fields, see
the main `network.WLAN` documentation.

## Before running

- Use a firmware build with `MICROPY_PY_NETWORK_WLAN_CSI` enabled.
- Update `WIFI_SSID` and `WIFI_PASSWORD` in the example file.
- Make sure the ESP32 receives Wi-Fi traffic after connecting, otherwise no CSI
  frames will be captured. A simple way to generate traffic is:

```bash
ping -i 0.1 <esp32_ip>
```

## Running an example

Run examples from the MicroPython repository root:

```bash
mpremote connect /dev/ttyUSB0 run examples/esp32_wifi_csi/csi_basic.py
mpremote connect /dev/ttyUSB0 run examples/esp32_wifi_csi/csi_turbulence_monitor.py
```

Replace `/dev/ttyUSB0` with the serial device for your board.

## Included examples

### `csi_basic.py`

Connects to Wi-Fi, enables CSI, and prints basic frame information.

### `csi_turbulence_monitor.py`

Reads CSI frames and computes a simple turbulence metric from selected
subcarriers.
