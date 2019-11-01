# Micropython + lvgl

**For information abound Micropython lvgl bindings please refrer to [lv_bindings/README.md](https://github.com/littlevgl/lv_bindings/blob/master/README.md)**

See also [Micropython + LittlevGL](https://blog.littlevgl.com/2019-02-20/micropython-bindings) blog post.

## Build Instructions

1. `sudo apt-get install build-essential libreadline-dev libffi-dev git pkg-config libsdl2-2.0-0 libsdl2-dev python`
2. `git clone --recurse-submodules https://github.com/littlevgl/lv_micropython.git`
3. `cd lv_micropython`
4. `make -C mpy-cross`
5. `make -C ports/unix/`
6. `./ports/unix/micropython`

### For ESP32 port

Please set `ESPIDF` parameter for the esp-idf install dir.
It needs to match Micropython expected esp-idf, otherwise a warning will be displayed (and build will probably fail)
For more details refer to [Setting up the toolchain and ESP-IDF](https://github.com/littlevgl/lv_micropython/blob/master/ports/esp32/README.md#setting-up-the-toolchain-and-esp-idf)

When using IL9341 driver, the color depth and swap mode need to be set to match ILI9341. This can be done from the command line.
Here is the command to build ESP32 + LittlevGL which is compatible with ILI9341 driver:

```
make -C mpy-cross
make -C ports/esp32 LV_CFLAGS="-DLV_COLOR_DEPTH=16 -DLV_COLOR_16_SWAP=1" BOARD=GENERIC_SPIRAM PYTHON=python2 deploy
```

Explanation about the paramters:
- `LV_CFLAGS` are used to override color depth and swap mode, for ILI9341 compatibility.  
  - `LV_COLOR_DEPTH=16` is needed if you plan to use the ILI9341 driver.
  - `LV_COLOR_16_SWAP=1` is needed if you plan to use the [Pure Micropython Display Driver](https://blog.littlevgl.com/2019-08-05/micropython-pure-display-driver).
- `BOARD` - I use WROVER board with SPIRAM. You can choose other boards from `ports/esp32/boards/` directory.
- `PYTHON=python2` - depending on your installed `pyparsing` version, you might or might not need this.
- `deploy` - make command will create ESP32 port of Micropython, and will try to deploy it through USB-UART bridge.

For more details please refer to [Micropython ESP32 README](https://github.com/micropython/micropython/blob/master/ports/esp32/README.md).

### For JavaScript port

You need Emscripten installed and working. There are lots of guides about that on the web, but here's the [official one](https://emscripten.org/docs/getting_started/index.html).

Once you have Emscripten working, you also need to install the `clang` package:
1. `cd <path to emsdk>`
2. `./emsdk install clang-e<sdk version>-64bit # for example: clang-e1.30.0-64bit`
3. `./emsdk activate clang-e<sdk version>-64bit`

Now you can build the JavaScript port.

1. `cd <path to lv_micropython>`
2. `. ./emsdk_env.sh`
3. `git checkout lvgl_javascript`
4. `git submodule update --init --recursive` (*can be very important!*)
5. `cd ports/javascript`
6. `make`
7. Run an HTTP server that serves files from the current directory.
8. Browse to `/lvgl_editor.html` on the HTTP Server.

## Super Simple Example

First, LittlevGL needs to be imported and initialized

```python
import lvgl as lv
lv.init()
```

Then display driver and input driver needs to be registered.
Refer to [Porting the library](https://docs.littlevgl.com/#Porting) for more information.
Here is an example of registering SDL drivers on Micropython unix port:

```python
import SDL
SDL.init()

# Register SDL display driver.

disp_buf1 = lv.disp_buf_t()
buf1_1 = bytearray(480*10)
lv.disp_buf_init(disp_buf1,buf1_1, None, len(buf1_1)//4)
disp_drv = lv.disp_drv_t()
lv.disp_drv_init(disp_drv)
disp_drv.buffer = disp_buf1
disp_drv.flush_cb = SDL.monitor_flush
disp_drv.hor_res = 480
disp_drv.ver_res = 320
lv.disp_drv_register(disp_drv)

# Regsiter SDL mouse driver

indev_drv = lv.indev_drv_t()
lv.indev_drv_init(indev_drv) 
indev_drv.type = lv.INDEV_TYPE.POINTER;
indev_drv.read_cb = SDL.mouse_read;
lv.indev_drv_register(indev_drv);
```

Here is an alternative example, for registering ILI9341 drivers on Micropython ESP32 port:

```python
import lvgl as lv
import lvesp32

# Import ILI9341 driver and initialized it

from ili9341 import ili9341
disp = ili9341()

# Import XPT2046 driver and initalize it

from xpt2046 import xpt2046
touch = xpt2046()
```

By default, both ILI9341 and XPT2046 are initialized on the same SPI bus with the following parameters:

- ILI9341: `miso=5, mosi=18, clk=19, cs=13, dc=12, rst=4, power=14, backlight=15, spihost=esp.HSPI_HOST, mhz=40, factor=4, hybrid=True`
- XPT2046: `cs=25, spihost=esp.HSPI_HOST, mhz=5, max_cmds=16, cal_x0 = 3783, cal_y0 = 3948, cal_x1 = 242, cal_y1 = 423, transpose = True, samples = 3`

You can change any of these parameters on ili9341/xpt2046 constructor.
You can also initalize them on different SPI buses if you want, by providing miso/mosi/clk parameters. Set them to -1 to use existing (initialized) spihost bus.

Now you can create the GUI itself:

```python

# Create a screen with a button and a label

scr = lv.obj()
btn = lv.btn(scr)
btn.align(lv.scr_act(), lv.ALIGN.CENTER, 0, 0)
label = lv.label(btn)
label.set_text("Hello World!")

# Load the screen

lv.scr_load(scr)

```

## More information

More info about LittlevGL: 
- Website https://littlevgl.com
- GitHub: https://github.com/littlevgl/lvgl

More info about lvgl Micropython bindings:
- https://github.com/littlevgl/lv_bindings/blob/master/README.md

Discussions about the Microptyhon binding: https://github.com/littlevgl/lvgl/issues/557

More info about the unix port: https://github.com/micropython/micropython/wiki/Getting-Started#debian-ubuntu-mint-and-variants


