"""
    Accessing SparkFun's 16x2 SerLCD - RGB Text via i2c
    https://www.sparkfun.com/products/16397
    Author: Wolf Paulus wolf@paulus.com
    MIT license; Copyright (c) 2022 wolfpaulus.com
"""
from micropython import const
from utime import sleep_ms
import pybox_i2c as i2c
import pybox_ct as ct

_DEFAULT_ADDRESS = const(0x72)
_LCD_DISPLAYCONTROL = const(0x08)
_LCD_ENTRYMODESET = const(0x04)
_LCD_DISPLAYON = const(0x04)
_LCD_DISPLAYOFF = const(0x00)
_LCD_CURSORON = const(0x02)
_LCD_CURSOROFF = const(0x00)
_LCD_BLINKON = const(0x01)
_LCD_BLINKOFF = const(0x00)
_LCD_ENTRYRIGHT = const(0x00)
_LCD_ENTRYLEFT = const(0x02)
_LCD_ENTRYSHIFTINCREMENT = const(0x01)
_LCD_ENTRYSHIFTDECREMENT = const(0x00)
_LCD_SETDDRAMADDR = const(0x80)
_LCD_RETURNHOME = const(0x02)
_SPECIAL_COMMAND = const(0xFE)  # Magic number for sending a special command
_SETTING_COMMAND = const(0x7C)  # Command to change settings: baud, lines, width,..
_SET_RGB_COMMAND = const(0x2B)  # Command to set backlight RGB value
_ENABLE_SYSTEM_MESSAGE_DISPLAY = const(0x2E)  # Command to enable system messages being displayed
_DISABLE_SYSTEM_MESSAGE_DISPLAY = const(0x2F)  # Command to disable system messages being displayed
_ENABLE_SPLASH_DISPLAY = const(0x30)  # Command to enable splash screen at power on
_DISABLE_SPLASH_DISPLAY = const(0x31)  # Command to disable splash screen at power on
_SAVE_CURRENT_DISPLAY_AS_SPLASH = const(0x0A)  # Command to save current text on display as splash
_CLEAR_COMMAND = const(0x2D)  # Command to clear and home the display
_CONTRAST_COMMAND = const(0x18)  # Command to change the contrast setting

_MAX_ROWS = const(2)
_MAX_COLUMNS = const(16)


def special_command(command, count=1) -> None:
    """
    Send one (or multiple) special commands to the display. Used by other functions.
    :param command: Command to send (a single byte)
    :param count: Number of times to send the command (if ommited, then default is once)
    :return: Returns true if the I2C write was successful, otherwise False.
    :rtype: bool
    """
    for i in range(count):
        i2c.write_cmd(address, _SPECIAL_COMMAND, command)
    sleep_ms(50)


def command(command) -> None:
    """
    Send one setting command to the display. Used by other functions.
    :param command: Command to send (a single byte)
    :return: Returns true if the I2C write was successful, otherwise False.
    :rtype: bool
    """
    i2c.write_cmd(address, _SETTING_COMMAND, command)


def is_connected() -> bool:
    """
    Determine if a device is connected to the system.
    :return: True if the device is connected, otherwise False.
    :rtype: bool
    """
    return i2c.is_device_connected(address)


def enable_system_messages() -> None:
    """
    Enable system messages
    :return: Returns true if the I2C write was successful, otherwise False.
    """
    command(_ENABLE_SYSTEM_MESSAGE_DISPLAY)
    sleep_ms(10)


def disable_system_messages() -> None:
    """
    Disable system messages
    :return: Returns true if the I2C write was successful, otherwise False.
    """
    command(_DISABLE_SYSTEM_MESSAGE_DISPLAY)
    sleep_ms(10)


def enable_splash() -> None:
    """
    Enable splash screen at power on
    :return: Returns true if the I2C write was successful, otherwise False.
    """
    command(_ENABLE_SPLASH_DISPLAY)
    sleep_ms(10)


def disable_splash() -> None:
    """
    Disable splash screen at power on
    :return: Returns true if the I2C write was successful, otherwise False.
    """
    command(_DISABLE_SPLASH_DISPLAY)
    sleep_ms(10)


def save_splash() -> None:
    """
    Save the current display as the splash. Saves whatever is currently being displayed into EEPROM
    This will be displayed at next power on as the splash screen
    :return: Returns true if the I2C write was successful, otherwise False.
    """
    command(_SAVE_CURRENT_DISPLAY_AS_SPLASH)
    sleep_ms(10)


def set_backlight(r: int, g: int, b: int) -> None:
    """
    Set backlight with no LCD messages or delays
    :param r: red backlight value 0-255
    :param g: green backlight value 0-255
    :param b: blue backlight value 0-255
    :return: Returns true if the I2C write was successful, otherwise False.
    """
    # create a block of data bytes to send to the screen
    # This will include the SET_RGB_COMMAND, and three bytes of backlight values
    block = [0, 1, 2, 3]
    block[0] = _SET_RGB_COMMAND  # command
    block[1] = r
    block[2] = g
    block[3] = b
    # send the complete bytes (address, settings command , rgb command , red byte, green byte, blue byte)
    i2c.write_block(_DEFAULT_ADDRESS, _SETTING_COMMAND, block)
    sleep_ms(10)


def display():
    """
    Turn the display on quickly.
    :return: Returns true if the I2C write was successful, otherwise False.
    """
    global displayControl

    displayControl |= _LCD_DISPLAYON
    return special_command(_LCD_DISPLAYCONTROL | displayControl)


def no_display() -> None:
    """
    Turn the display off quickly.
    :return: Returns true if the I2C write was successful, otherwise False.
    """
    global displayControl
    displayControl &= _LCD_DISPLAYON
    special_command(_LCD_DISPLAYCONTROL | displayControl)


def blink() -> None:
    """
    Turn the blink cursor on.
    :return: Returns true if the I2C write was successful, otherwise False.
    """
    global displayControl
    displayControl |= _LCD_BLINKON
    special_command(_LCD_DISPLAYCONTROL | displayControl)


def no_blink() -> None:
    """
    Turn the blink cursor off.
    :return: Returns true if the I2C write was successful, otherwise False.
    """
    global displayControl
    displayControl &= ~_LCD_BLINKON
    special_command(_LCD_DISPLAYCONTROL | displayControl)


def cursor() -> None:
    """
    Turn the underline cursor on.
    :return: Returns true if the I2C write was successful, otherwise False.
    """
    global displayControl

    displayControl |= _LCD_CURSORON
    special_command(_LCD_DISPLAYCONTROL | displayControl)


def no_cursor() -> None:
    """
    Turn the underline cursor off.
    :return: Returns true if the I2C write was successful, otherwise False.
    """
    global displayControl
    displayControl &= ~_LCD_CURSORON
    special_command(_LCD_DISPLAYCONTROL | displayControl)


def set_cursor(col, row) -> None:
    """
    Set the cursor position to a particular column and row.
    :param col: The column postion (0-19)
    :param row: The row postion (0-3)
    :return: Returns true if the I2C write was successful, otherwise False.
    """
    row_offsets = [0x00, 0x40, 0x14, 0x54]

    # keep variables in bounds
    row = max(0, row)  # row cannot be less than 0
    row = min(row, (_MAX_ROWS - 1))  # row cannot be greater than max rows
    # construct the cursor "command"
    command = _LCD_SETDDRAMADDR | (col + row_offsets[row])
    # send the complete bytes (special command + command)
    special_command(command)


def set_contrast(contrast) -> None:
    """
    Set the contrast of the LCD screen (0-255)
    :param contrast: The new contrast value (0-255)
    :return: Returns true if the I2C write was successful, otherwise False.
    """
    # To set the contrast we need to send 3 bytes:
    # (1) SETTINGS_COMMAND
    # (2) CONTRAST_COMMAND
    # (3) contrast value
    # To do this, we are going to use writeBlock(),
    # so we need our "block of bytes" to include
    # CONTRAST_COMMAND and contrast value

    block = [_CONTRAST_COMMAND, contrast]
    # send the complete bytes (address, settings command , contrast command, contrast value)
    i2c.write_block(address, _SETTING_COMMAND, block)
    sleep_ms(10)


def home() -> None:
    """
    Send the home command to the display.
    This returns the cursor to return to the beginning of the display,
    without clearing the display.
    :return: Returns true if the I2C write was successful, otherwise False.
    """
    special_command(_LCD_RETURNHOME)
    sleep_ms(10)


def clear_screen() -> None:
    """
    Sends the command to clear the screen
    :return: Returns true if the I2C write was successful, otherwise False.
    """
    print("", "")  # this should unregister potentially running scroll tasks
    set_backlight(255, 255, 255)
    set_contrast(0)
    command(_CLEAR_COMMAND)
    sleep_ms(10)


def print(row0: str = None, row1: str = None) -> None:
    """
    Outputs the given rows.
    If a provided row in None, its current content will remain, i.e. use "" to clear that row's content
    If a provided row's content is longer than the display, the content will scroll
    Only a not scrolling row will be immediately updated
    :param row0
    :param row1
    :return: None
    """
    global scrolling
    update = row0 is not None or row1 is not None
    if update:
        content = [row0, row1]
        for r in range(_MAX_ROWS):
            if content[r] is not None:
                if len(content[r]) > _MAX_COLUMNS:  # add a gap for scrolling
                    content_buffer[r] = content[r] + 3 * " "
                else:  # fill the row
                    content_buffer[r] = content[r] + (_MAX_COLUMNS - len(content[r])) * " "
        longest = max([len(content_buffer[0]), len(content_buffer[1])])
        if longest > _MAX_COLUMNS:  # need_to_scroll
            if not scrolling:
                scrolling = True
                ct.register((print, scroll_speed))
        elif scrolling:
            ct.unregister((print, scroll_speed))
            scrolling = False

    s = ""
    for r in range(_MAX_ROWS):
        if len(content_buffer[r]) > _MAX_COLUMNS and not update:  # scroll content
            if chr(126) not in content_buffer[r]:  # move 1st char to the end
                content_buffer[r] = content_buffer[r][1:] + content_buffer[r][0]
            else:  # (->) in str  .. move last char to the start
                content_buffer[r] = content_buffer[r][-1] + content_buffer[r][0:-1]
        s += content_buffer[r][:_MAX_COLUMNS]
    i2c.write_bytes(address, bytes(s, "utf-8"))
    if update:
        sleep_ms(50)


displayControl = _LCD_DISPLAYON | _LCD_CURSOROFF | _LCD_BLINKOFF
displayMode = _LCD_ENTRYLEFT | _LCD_ENTRYSHIFTDECREMENT
content_buffer = [" " * _MAX_COLUMNS, " " * _MAX_COLUMNS]
scrolling = False
address = _DEFAULT_ADDRESS
scroll_speed = 350  # 250 .. 450 looks OK

special_command(_LCD_DISPLAYCONTROL | displayControl)
special_command(_LCD_ENTRYMODESET | displayMode)
disable_system_messages()
clear_screen()
