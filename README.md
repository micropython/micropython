1. `sudo apt-get install build-essential libreadline-dev libffi-dev git pkg-config libsdl2-2.0-0 libsdl2-dev`
2. `git clone --recurse-submodules https://github.com/kisvegabor/lv_mpy.git`
3. `cd ./micropython/ports/unix`
4. `make axtls`
5. `make`
6. `./micropython`


Test code to craete a button and a label:
```python
>>> import lvgl
>>> btn = lvgl.btn(lvgl.scr_act())
>>> btn.align(lvgl.scr_act(), lvgl.ALIGN.
>>> btn.align(lvgl.scr_act(), lvgl.ALIGN.CENTER, 0, 0)
>>> label = lvgl.label(btn)
>>> label.set_text("Button")
```

More info about LittlevGL: 
- Website https://littlevgl.com
- GitHub: https://github.com/amirgon/lvgl

Discussiona about the Microptyhon binding: https://github.com/littlevgl/lvgl/issues/557

More info about the unix port: https://github.com/micropython/micropython/wiki/Getting-Started#debian-ubuntu-mint-and-variants


