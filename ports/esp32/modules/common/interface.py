import buttons, time, wifi, gc, uinterface

ACTION_NO_OPERATION = 0
ACTION_CONFIRM = 2
ACTION_CANCEL = 4

DEFAULT_CHARSET = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~"
NUMERIC_CHARSET = "1234567890"

menu_state = {
    "active" : 0,
    "items": [],
    "selected": [],
    "pressed_button": ACTION_NO_OPERATION
}

text_input_state = {
    "cursor": 0,
    "charset": DEFAULT_CHARSET,
    "text": [ DEFAULT_CHARSET[0] ],
    "action": ACTION_NO_OPERATION
}

confirm_dialog_action = ACTION_NO_OPERATION

_menu_user_callback_left = None
_menu_user_callback_right = None

"""
The following functions need to be implemented in interface.py and are badge specific
"""

def _initialize_display():
    return uinterface._initialize_display()

def _clear_display():
    return uinterface._clear_display()

def connect_wifi(duration=None):
    return uinterface.connect_wifi(duration)

def loading_text(text):
    return uinterface.loading_text(text)

def _draw_menu_item(item, menu_state):
    return uinterface._draw_menu_item(item, menu_state)

def _draw_text_input_state(cursor, text):
    return uinterface._draw_text_input_state(cursor, text)

def _draw_text_input_sequence(startx, chars, colour, reverse=False):
    return uinterface._draw_text_input_sequence(startx, chars, colour, reverse=reverse)

def _draw_confirmation_dialog(text):
    return uinterface._draw_confirmation_dialog(text)

def _draw_image(image, x, y):
    return uinterface._draw_image(image, x, y)

def skippabletext(text, color=(255, 255, 255), pos=None, width=-1):
    return uinterface.skippabletext(text, color=color, pos=pos, width=width)

"""
Here follow the generic functions and helper functions
"""

def menu(items, active = 0, selected=None, callback_left=None, callback_right=None):
    global _menu_user_callback_left
    global _menu_user_callback_right
    _menu_user_callback_left = _menu_deselect_active if selected is not None else callback_left
    _menu_user_callback_right = _menu_select_active if selected is not None else callback_right
    state = _menu_init_display_and_state(items, active, selected)
    while state["pressed_button"] == ACTION_NO_OPERATION:
        time.sleep(0.1)
    _input_exit_routine()
    if state["pressed_button"] == ACTION_CONFIRM:
        return state["active"] if selected is None else state['selected']
    else:
        return None

def text_input(charset = DEFAULT_CHARSET):
    state = _text_input_init_display_and_state(charset)
    while state["action"] == ACTION_NO_OPERATION:
        time.sleep(0.1)
    _input_exit_routine()
    if state["action"] == ACTION_CONFIRM:
        return "".join(state["text"])
    else:
        return ""

def confirmation_dialog(text):
    global confirm_dialog_action
    _confirmation_dialog_initialize(text)

    while confirm_dialog_action == ACTION_NO_OPERATION:
        time.sleep(0.1)
    _input_exit_routine()

    return (confirm_dialog_action == ACTION_CONFIRM)

def _menu_select_active(pressed):
    global menu_state
    if pressed:
        app_name = menu_state['items'][menu_state['active']]
        if app_name not in menu_state['selected']:
            print('Selecting %s' % app_name)
            menu_state['selected'].append(app_name)
            _draw_menu_item(menu_state['items'][menu_state['active']], menu_state)

def _menu_deselect_active(pressed):
    global menu_state
    if pressed:
        app_name = menu_state['items'][menu_state['active']]
        if app_name in menu_state['selected']:
            print('Deselecting %s' % app_name)
            menu_state['selected'] = [name for name in menu_state['selected'] if name != app_name]
            _draw_menu_item(menu_state['items'][menu_state['active']], menu_state)

def _menu_init_display_and_state(items, active, selected):
    global menu_state
    menu_state["items"] = items
    menu_state["active"] = active
    menu_state["selected"] = selected
    menu_state["pressed_button"] = ACTION_NO_OPERATION

    _initialize_display()
    
    _menu_register_callbacks()
    _draw_menu_item(items[active], menu_state)
    return menu_state

def _menu_register_callbacks():
    buttons.attach(buttons.BTN_A, _menu_select_callback)
    buttons.attach(buttons.BTN_B, _menu_back_callback)
    buttons.attach(buttons.BTN_UP, _menu_up_callback)
    buttons.attach(buttons.BTN_DOWN, _menu_down_callback)
    buttons.attach(buttons.BTN_LEFT, _menu_left_callback)
    buttons.attach(buttons.BTN_RIGHT, _menu_right_callback)

def _text_input_init_display_and_state(charset):
    global text_input_state
    text_input_state["charset"] = charset
    text_input_state["text"] = [charset[0]]
    text_input_state["action"] = ACTION_NO_OPERATION
    text_input_state["cursor"] = 0

    _initialize_display()

    _text_input_register_callbacks()
    _draw_text_input_state(text_input_state["cursor"], text_input_state["text"])
    return text_input_state

def _text_input_register_callbacks():
    buttons.attach(buttons.BTN_A, _text_input_confirm_callback)
    buttons.attach(buttons.BTN_B, _text_input_cancel_callback)
    buttons.attach(buttons.BTN_UP, _text_input_up_callback)
    buttons.attach(buttons.BTN_DOWN, _text_input_down_callback)
    buttons.attach(buttons.BTN_LEFT, _text_input_left_callback)
    buttons.attach(buttons.BTN_RIGHT, _text_input_right_callback)

def _confirmation_dialog_initialize(text):
    global confirm_dialog_action

    confirm_dialog_action = ACTION_NO_OPERATION
    _initialize_display()

    _confirmation_dialog_register_callbacks()
    _draw_confirmation_dialog(text)

def _confirmation_dialog_register_callbacks():
    buttons.attach(buttons.BTN_A, _confirmation_dialog_yes_callback)
    buttons.attach(buttons.BTN_B, _confirmation_dialog_no_callback)

def _menu_up_callback(pressed):
    global menu_state
    if pressed:
        active = menu_state["active"]
        item_count = len(menu_state["items"])
        active -= 1
        if (active < 0):
            active = item_count - 1

        menu_state["active"] = active
        _draw_menu_item(menu_state["items"][active], menu_state)

def _menu_down_callback(pressed):
    global menu_state
    if pressed:
        active = menu_state["active"]
        item_count = len(menu_state["items"])
        active += 1
        if (active >= item_count):
            active = 0

        menu_state["active"] = active
        _draw_menu_item(menu_state["items"][active], menu_state)

def _menu_left_callback(pressed):
    global menu_state
    if callable(_menu_user_callback_left):
        _menu_user_callback_left(pressed)
        if menu_state['selected'] is None:
            _menu_back_callback(pressed)


def _menu_right_callback(pressed):
    global menu_state
    if callable(_menu_user_callback_right):
        _menu_user_callback_right(pressed)
        if menu_state['selected'] is None:
            _menu_back_callback(pressed)


def _menu_back_callback(pressed):
    global menu_state
    if pressed:
        menu_state["pressed_button"] = _add_action_state(menu_state["pressed_button"], ACTION_CANCEL)

def _menu_select_callback(pressed):
    global menu_state
    if pressed:
        menu_state["pressed_button"] = _add_action_state(menu_state["pressed_button"], ACTION_CONFIRM)

def _text_input_up_callback(pressed):
    global text_input_state
    if pressed:
        cursor = text_input_state["cursor"]
        text = text_input_state["text"]
        charset = text_input_state["charset"]

        index = charset.index(text[cursor]) + 1
        if index >= len(charset):
            index = 0
        
        text[cursor] = charset[index]
        text_input_state["text"] = text
        _draw_text_input_state(cursor, text)

def _text_input_down_callback(pressed):
    global text_input_state
    if pressed:
        cursor = text_input_state["cursor"]
        text = text_input_state["text"]
        charset = text_input_state["charset"]

        index = charset.index(text[cursor]) - 1
        if index < 0:
            index = len(charset) - 1

        text[cursor] = charset[index]
        text_input_state["text"] = text
        _draw_text_input_state(cursor, text)

def _text_input_left_callback(pressed):
    global text_input_state
    if pressed:
        cursor = text_input_state["cursor"]
        text = text_input_state["text"]
        cursor -= 1
        if cursor < 0:
            cursor = 0
        text_input_state["cursor"] = cursor
        _draw_text_input_state(cursor, text)

def _text_input_right_callback(pressed):
    global text_input_state
    if pressed:
        cursor = text_input_state["cursor"]
        text = text_input_state["text"]
        charset = text_input_state["charset"]
        cursor += 1
        if cursor == len(text):
            text.append(charset[0])
        text_input_state["cursor"] = cursor
        text_input_state["text"] = text
        _draw_text_input_state(cursor, text)

def _text_input_cancel_callback(pressed):
    global text_input_state
    if pressed:
        text = text_input_state["text"]
        cursor = text_input_state["cursor"]
        if len(text) > 1:
            text = text[:-1]
            if (cursor == len(text)):
                cursor -= 1
            text_input_state["cursor"] = cursor
            text_input_state["text"] = text
            _draw_text_input_state(cursor, text)
        else:
            text_input_state["action"] = _add_action_state(text_input_state["action"], ACTION_CANCEL)

def _text_input_confirm_callback(pressed):
    global text_input_state
    if pressed:
        text_input_state["action"] = _add_action_state(text_input_state["action"], ACTION_CONFIRM)

def _confirmation_dialog_yes_callback(pressed):
    global confirm_dialog_action
    if pressed:
        confirm_dialog_action = _add_action_state(confirm_dialog_action, ACTION_CONFIRM)

def _confirmation_dialog_no_callback(pressed):
    global confirm_dialog_action
    if pressed:
        confirm_dialog_action = _add_action_state(confirm_dialog_action, ACTION_CANCEL)

def _input_exit_routine():
    _clear_display()
    buttons.detachAll()

def _add_action_state(action_state, added_state):
    return action_state | added_state

def _abort_scroll(pressed):
    if pressed:
        global abort
        abort = True