This board need wifi_remote / esp_hosted with specific version to support wifi and ble on the ESP32C6 hosted.
modify esp32/main/idf_component.yml before compile :

## IDF Component Manager Manifest File
dependencies:
  espressif/mdns: ~1.1.0
  espressif/esp_tinyusb:
    rules:
    - if: target in [esp32s2, esp32s3]
    version: ~1.0.0
  ## Required IDF version for M5STACK tab5
  idf: '>=5.3'
  espressif/esp_hosted: 1.4.0
  espressif/esp_wifi_remote: 0.8.5
  ##chmorgan/esp-audio-player: 1.0.7
  chmorgan/esp-file-iterator: 1.0.0
  ##espressif/led_strip: 3.0.0
  espressif/esp_lcd_ili9881c: ^1.0.1