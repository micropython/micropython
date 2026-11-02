# Web workflow over an ESP32 co-processor (`_airlift`)

Manual tests for web workflow on boards whose WiFi is an ESP32 co-processor
running nina-fw, such as the Adafruit Fruit Jam. They run on a host computer
on the same network as the board.

## Prerequisites

- The board's `settings.toml` sets `CIRCUITPY_WIFI_SSID`,
  `CIRCUITPY_WIFI_PASSWORD` and `CIRCUITPY_WEB_API_PASSWORD`. The host tests
  read the web API password from that file, so pass its path (normally on the
  mounted CIRCUITPY drive).
- Python 3 on the host. `run_on_board.py` and `esp_wedge_host.py` also need
  `pyserial` and the board's USB serial port. The port defaults to the first
  Fruit Jam in `/dev/serial/by-id`; set `AIRLIFT_TEST_SERIAL` to use another one.
- Find the board's IP address in the serial console's title bar, or in
  `http://circuitpython.local/cp/version.json`.

## Tests

Each test prints what it checks and ends with `PASS` or `FAIL`.

- `web_workflow_files_host.py <ip> <settings.toml> [upload dir]`: lists `/`,
  downloads `boot_out.txt`, and uploads, reads back and deletes a 20 KB file.
  The upload dir defaults to `/saves/`, which web workflow can write to while
  the host has CIRCUITPY mounted.
- `web_workflow_download_host.py <ip> <settings.toml> <n> [size]`: uploads one
  file (100 KB by default) to `/saves/` and downloads it `n` times, checking
  every byte.
- `web_workflow_serial_host.py <ip> <settings.toml>`: drives the REPL through
  the `/cp/serial/` websocket, including a soft reboot over the same
  connection.
- `esp_wedge_host.py <ip> [runs]`: runs `esp_wedge.py` on the board, which
  leaves the ESP32 unresponsive while the VM keeps running. Web workflow should
  come back on its own, without a reset of the board.
- `no_web_workflow.py`: with `CIRCUITPY_WEB_API_PASSWORD` removed from
  `settings.toml`, the core must leave the ESP32 to user code. Run it with
  `run_on_board.py no_web_workflow.py`; it needs `adafruit_esp32spi` in `lib/`.

## Expected output

`esp_wedge_host.py`:

```
run 1: web workflow up before: True
  board: START_SERVER_TCP before WiFi: TimeoutError('Timed out waiting for SPI char',)
  board: WEDGED
  board: status after: 3
  board: PASS
  board: [end]
  board: [end]
  board: >
  web workflow back after 4.2 s
1 of 1 passed
```

`web_workflow_serial_host.py`:

```
ctrl-c -> prompt: True
eval 6*7 -> 42: True 140 ms
3000-char line arrives whole: True
ctrl-d -> soft reboot seen: True
code.py ran after reload: True
same websocket at the REPL again: True
eval after reload -> 43: True
```
