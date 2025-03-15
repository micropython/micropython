# Board files for M5Stack Core2

To use, follow the instructions in the [root README](../../../../README.md) for this repository as well as in the [README for the ESP32 port](../../README.md) to prepare the prerequisites. 
Then to compile and deploy the MicroPython firmware, go to the `ports/esp32` directory and execute:

```text
export PORT=<your serial port>
export BOARD=M5CORE2
esptool.py erase_flash && \
make submodules && \
make -j $(nproc) && \
make deploy
```

After the firmware is deployed to the device, and it is restarted, you should upload a `main.py` file which contains your program.
In your program you can import `m5core2` module, and call `init()` function to initialize the device (power, display, touch sensor and LVGL).

Example of `main.py` file:
```python
import m5core2
m5core2.init()

import lvgl as lv
scr = lv.obj()
btn = lv.btn(scr)
btn.align(lv.ALIGN.CENTER, 0, 0)
label = lv.label(btn)
label.set_text("Button")
lv.scr_load(scr)
```