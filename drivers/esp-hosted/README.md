# esp-hosted driver

This is a MicroPython driver for the Espressif
[esp_hosted](https://github.com/espressif/esp-hosted/#readme) communications
coprocessor, which allows creating a Wi-Fi and/or Bluetooth interface from
MicroPython to a separate connected ESP32 compatible device running the
`esp_hosted` firmware.

## Building

Enable this driver by setting `MICROPY_PY_NETWORK_ESP_HOSTED` to 1 in your
Makefile. If `MICROPY_PY_BLUETOOTH` is set then the Bluetooth host driver will
also be built.

In addition to normal MicroPython build requirements, building this driver
requires the [protocol buffer
compiler](https://github.com/protocolbuffers/protobuf#protobuf-compiler-installation)
(protoc) to be installed.

On Debian/Ubuntu, it can be installed by running:

```
sudo apt-get install protobuf-compiler
```
