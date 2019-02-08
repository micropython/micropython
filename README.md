# Micropython + lvgl

**For information abound Micropython lvgl bindings please refrer to [lv_bindings/README.md](https://github.com/littlevgl/lv_bindings/blob/master/README.md)**

## Build Instructions

1. `sudo apt-get install build-essential libreadline-dev libffi-dev git pkg-config libsdl2-2.0-0 libsdl2-dev`
2. `git clone --recurse-submodules https://github.com/kisvegabor/lv_mpy.git`
3. `make -C ports/unix/`
6. `./ports/unix/micropython`

### For ESP32 port

Please set `ESPIDF` parameter for the esp-idf install dir.
It needs to match Micropython expected esp-idf, otherwise a warning will be displayed (and build will probably fail)
For more details refer to [Setting up the toolchain and ESP-IDF](https://github.com/littlevgl/lv_micropython/blob/master/ports/esp32/README.md#setting-up-the-toolchain-and-esp-idf)

For example:
`make -C ports/esp32/ ESPIDF=~/esp/projects/lv_mpy_deps/esp-idf`

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

# Register SDL display driver.

disp_drv = lv.disp_drv_t()
lv.disp_drv_init(disp_drv)
disp_drv.disp_flush = SDL.monitor_flush
disp_drv.disp_fill = SDL.monitor_fill
disp_drv.disp_map = SDL.monitor_map
lv.disp_drv_register(disp_drv)

# Regsiter SDL mouse driver

indev_drv = lv.indev_drv_t()
lv.indev_drv_init(indev_drv) 
indev_drv.type = lv.INDEV_TYPE.POINTER;
indev_drv.read = SDL.mouse_read;
lv.indev_drv_register(indev_drv);
```

Now you can create the GUI itself

```python

# Create a screen with a button and a label

scr = lv.obj()
btn = lv.btn(scr)
btn.align(lv.scr_act(), lv.ALIGN.CENTER, 0, 0)
label = lv.label(btn)
label.set_text("Button")

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


