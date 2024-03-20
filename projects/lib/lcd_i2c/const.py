#!/usr/bin/env python3
# -*- coding: UTF-8 -*-

try:
    from micropython import const
except ImportError:
    def const(x):
        return x

# commands
#: Clear display command
LCD_CLEARDISPLAY = const(0x01)
#: Return to home position command
LCD_RETURNHOME = const(0x02)
#: Set entry mode command
LCD_ENTRYMODESET = const(0x04)
#: Control display command
LCD_DISPLAYCONTROL = const(0x08)
#: Shift cursor command
LCD_CURSORSHIFT = const(0x10)
#: Set function command
LCD_FUNCTIONSET = const(0x20)
#: Set CGRAM address command
LCD_SETCGRAMADDR = const(0x40)
#: Set DDRAM address command
LCD_SETDDRAMADDR = const(0x80)

# flags for display entry mode
#: Set display entry mode as right command
LCD_ENTRYRIGHT = const(0x00)
#: Set display entry mode as left command
LCD_ENTRYLEFT = const(0x02)
#: Set display entry mode as shift increment command
LCD_ENTRYSHIFTINCREMENT = const(0x01)
#: Set display entry mode as shift decrement command
LCD_ENTRYSHIFTDECREMENT = const(0x00)

# flags for display on/off control
#: Turn display on command
LCD_DISPLAYON = const(0x04)
#: Turn display off command
LCD_DISPLAYOFF = const(0x00)
#: Turn cursor on command
LCD_CURSORON = const(0x02)
#: Turn cursor off command
LCD_CURSOROFF = const(0x00)
#: Set curor blink command
LCD_BLINKON = const(0x01)
#: Set curor no blink command
LCD_BLINKOFF = const(0x00)

# flags for display/cursor shift
#: Display move command
LCD_DISPLAYMOVE = const(0x08)
#: Move cursor command
LCD_CURSORMOVE = const(0x00)
#: Move display shift right command
LCD_MOVERIGHT = const(0x04)
#: Move display shift left command
LCD_MOVELEFT = const(0x00)

# flags for function set
#: 8 bit mode command
LCD_8BITMODE = const(0x10)
#: 4 bit mode command
LCD_4BITMODE = const(0x00)
#: 2 line command
LCD_2LINE = const(0x08)
#: 1 line command
LCD_1LINE = const(0x00)
#: 5x10 dots display command
LCD_5x10DOTS = const(0x04)
#: 5x8 dots display command
LCD_5x8DOTS = const(0x00)

# flags for backlight control
#: Activate backlight command
LCD_BACKLIGHT = const(0x08)
#: Deactivate backlight command
LCD_NOBACKLIGHT = const(0x00)

# other
#: Enable bit
EN = const(0b00000100)
#: Read/Write bit
RW = const(0b00000010)
#: Register select bit
RS = const(0b00000001)
