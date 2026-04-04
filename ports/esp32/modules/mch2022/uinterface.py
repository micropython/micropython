import display, buttons, wifi, time

def _initialize_display():
    display.drawFill(0)
    display.flush()

def _clear_display():
    display.drawFill(0)
    display.flush()

def connect_wifi(duration=None):
    if wifi.status():
        return True

    display.drawFill(0)
    display.drawText(0, 0, "Connecting to wifi")
    display.flush()

    wifi.connect()
    if duration is not None:
        wifi.wait(duration=duration)
    else:
        wifi.wait()

    if not wifi.status():
        display.drawFill(0)
        display.drawText(0, 0, "Connecting to wifi failed")
        display.flush()
        time.sleep(3)

    display.drawFill(0)
    display.flush()
    return wifi.status()

def loading_text(text):
    display.drawFill(0)
    display.drawText(0, 0, text)
    display.flush()

def _draw_menu_item(item, menu_state):
    display.drawFill(0)
    display.drawText(0, 0, item, (0x00FF00
                                if (menu_state['selected'] is not None and item in menu_state['selected'])
                                else 0xFFFFFF))
    display.flush()

def _draw_text_input_state(cursor, text):
    display.drawFill(0)
    before_mid = text[:cursor][-2:]
    after_mid = text[cursor+1:][:2]
    mid = text[cursor]
    step = 10
    midx = 240
    beforex = midx - step
    afterx = midx + step
    colour_selected = 0x96320A
    colour_unselected = 0x505050
    _draw_text_input_sequence(beforex, before_mid, colour_unselected, reverse=True)
    _draw_text_input_sequence(midx, mid, colour_selected)
    _draw_text_input_sequence(afterx, after_mid, colour_unselected)
    display.flush()

def _draw_text_input_sequence(startx, chars, colour, reverse=False):
    length = len(chars)
    step = 10
    curx = startx

    if reverse:
        for i in range(length - 1, -1, -1):
            display.drawText(curx, 0, chars[i], colour)
            curx -= step
    else:
        for i in range(length):
            display.drawText(curx, 0, chars[i], colour)
            curx += step

def _draw_confirmation_dialog(text):
    display.drawFill(0)
    display.drawText(0, 0, "Yes=A")
    display.drawText(0, 10, "No=B")
    display.drawText(0, 20, text)
    display.flush()

def _draw_image(image, x, y):
    display.drawRaw(x, y, image["width"], image["height"], image["data"])

def skippabletext(text, color=(255, 255, 255), pos=None, width=-1):
   
    buttons.attach(buttons.BTN_A, _abort_scroll)
    buttons.attach(buttons.BTN_B, _abort_scroll)

    display.drawText(0, 0, text)
   
    delay_loop = 50
    global abort
    abort = False

    while (not abort) and (delay_loop >= 0):
        time.sleep(0.1)
        delay_loop -= 1

    buttons.clear_button_mapping()