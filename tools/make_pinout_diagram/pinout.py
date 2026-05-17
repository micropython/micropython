import re

pinout_str = """
       \u001b[44;1m          WeAct Studio RP2350B Core Board          \u001b[0m

                        ╭── RESET
       Key (GP23) ───╮  │  ╭── BOOT
               ╭────────────────────────────────╮
 [26 ]  [25 ]  │⌾ ⌾ ╭─╮╭─╮╭─╮     ╭─[CLK]    ⌾ ⌾│  [24 ]  [ 23 ]
 [28 ]  [27 ]  │⌾ ⌾ │⬤││⬤││⬤│     │ ╭─[DIO]  ⌾ ⌾│  [22 ]  [ 21 ]
 [30 ]  [29 ]  │⌾ ⌾ ╰─╯╰─╯╰─╯[⏚]╮ │ │ ╭─[3V3]⌾ ⌾│  [20 ]  [ 19 ]
 [32 ]  [31 ]  │⌾ ⌾    LED ▩    ⌾ ⌾ ⌾ ⌾      ⌾ ⌾│  [18 ]  [ 17 ]
 [34 ]  [33 ]  │⌾ ⌾   (GP25)   ⟋⟍            ⌾ ⌾│  [16 ]  [ 15 ]
 [36 ]  [35 ]  │⌾ ⌾          ⟋    ⟍          ⌾ ⌾│  [14 ]  [ 13 ]
 [38 ]  [37 ]  │⌾ ⌾        ⟋        ⟍        ⌾ ⌾│  [12 ]  [ 11 ]
 [40 ]  [39 ]  │⌾ ⌾        ⟍        ⟋        ⌾ ⌾│  [10 ]  [  9 ]
 [42 ]  [41 ]  │⌾ ⌾          ⟍    ⟋          ⌾ ⌾│  [ 8 ]  [  7 ]
 [44 ]  [43 ]  │⌾ ⌾            ⟍⟋            ⌾ ⌾│  [ 6 ]  [  5 ]
 [46 ]  [45 ]  │⌾ ⌾                          ⌾ ⌾│  [ 4 ]  [  3 ]
 [RUN]  [47 ]  │⌾ ⌾                          ⌾ ⌾│  [ 2 ]  [  1 ]
 [3V3]  [3V3]  │⌾ ⌾         ┌──────┐         ⌾ ⌾│  [ 0 ]  [VREF]
 [ ⏚ ]  [EN ]  │▣ ⌾         │      │         ▣ ▣│  [ ⏚ ]  [ ⏚  ]
 [ ⏚ ]  [ ⏚ ]  │▣ ▣         │      │         ⌾ ⌾│  [5V ]  [VBUS]
               ╰────────────└──────┘────────────╯
"""


def ansi_colour(wrap_str: str, colours: tuple[int | None, int | None]) -> str:
    """Wrap a string in the ANSI foreground and background colour escape sequences."""
    wrapped_str = ""

    wrapped_str += "\u001b[38;5;" + str(colours[0]) + "m" if colours[0] else ""
    wrapped_str += "\u001b[48;5;" + str(colours[1]) + "m" if colours[1] else ""
    wrapped_str += wrap_str
    wrapped_str += "\u001b[0m" if colours[0] or colours[1] else ""

    return wrapped_str


def add_colour(pinout_str):
    symbol_colours = {
        "⌾": (220, None),  # Pin (Yellow)
        "▣": (220, None),  # Ground pin (Yellow)
        "↺": (15, None),  # Reset (White)
        "▩": (129, None),  # LED (Purple)
    }

    for symbol, colours in symbol_colours.items():
        pinout_str = pinout_str.replace(symbol, ansi_colour(symbol, colours))
    return pinout_str


def colour_tags(matchobj: re.Match) -> str:
    white_on_red = (15, 1)
    white_on_peach = (15, 216)
    white_on_dark_green = (15, 28)
    white_on_black = (15, 16)
    black_on_pink = (16, 224)

    tag_colours = {
        "5V": white_on_red,
        "VBUS": white_on_red,
        "VREF": white_on_dark_green,
        "3V3": white_on_red,
        "⏚": white_on_black,
        "EN": black_on_pink,
        "CLK": white_on_peach,
        "DIO": white_on_peach,
    }

    if matchobj.group(2) not in tag_colours.keys():
        return matchobj.group(0)

    pin_colours = tag_colours[matchobj.group(2)]

    return ansi_colour(matchobj.group(1), pin_colours)


def replace_tags(pinout_str):
    return re.sub(r"(\[\s*(\S+)\s*\])", colour_tags, pinout_str)


def colour_pins(matchobj: re.Match) -> str:
    # Regular GPIO is green, ADC pins (40-47) are darker greeen
    gpio_colours = (15, 34)  # White on green
    adc_colours = (15, 28)  # White on dark green
    pin_number = int(matchobj.group(2))

    pin_colours = gpio_colours if pin_number < 40 else adc_colours

    return ansi_colour(matchobj.group(1), pin_colours)


def replace_gpio(pinout_str) -> str:
    return re.sub(r"(\[\s*(\d+)\s*\])", colour_pins, pinout_str)


pinout_str = replace_gpio(replace_tags(add_colour(pinout_str)))
# Colours include a square bracket; temporarily change them!
pinout_str = pinout_str.replace("\u001b[", "~")
pinout_str = pinout_str.replace("[", " ").replace("]", " ")
pinout_str = pinout_str.replace("~", "\u001b[")  # Put them back

print(pinout_str)
