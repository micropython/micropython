// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#define SHIFT_MOD 0x40
#define FN_MOD 0x80

#define KEY_OPT 0
#define KEY_Z 1
#define KEY_C 2
#define KEY_B 3
#define KEY_M 4
#define KEY_DOT 5
#define KEY_SPACE 6
#define KEY_SHIFT 7
#define KEY_S 8
#define KEY_F 9
#define KEY_H 10
#define KEY_K 11
#define KEY_SEMICOLON 12
#define KEY_ENTER 13
#define KEY_Q 14
#define KEY_E 15
#define KEY_T 16
#define KEY_U 17
#define KEY_O 18
#define KEY_LEFT_BRACKET 19
#define KEY_BACKSLASH 20
#define KEY_1 21
#define KEY_3 22
#define KEY_5 23
#define KEY_7 24
#define KEY_9 25
#define KEY_UNDERSCORE 26
#define KEY_BACKSPACE 27
#define KEY_CTRL 28
#define KEY_ALT 29
#define KEY_X 30
#define KEY_V 31
#define KEY_N 32
#define KEY_COMMA 33
#define KEY_SLASH 34
#define KEY_FN 35
#define KEY_A 36
#define KEY_D 37
#define KEY_G 38
#define KEY_J 39
#define KEY_L 40
#define KEY_APOSTROPHE 41
#define KEY_TAB 42
#define KEY_W 43
#define KEY_R 44
#define KEY_Y 45
#define KEY_I 46
#define KEY_P 47
#define KEY_RIGHT_BRACKET 48
#define KEY_GRAVE 49
#define KEY_2 50
#define KEY_4 51
#define KEY_6 52
#define KEY_8 53
#define KEY_0 54
#define KEY_EQUALS 55

#define KEY_GREATER (5 | SHIFT_MOD)
#define KEY_COLON (12 | SHIFT_MOD)
#define KEY_LEFT_CURLY_BRACKET (19 | SHIFT_MOD)
#define KEY_PIPE (20 | SHIFT_MOD)
#define KEY_EXCLAMATION (21 | SHIFT_MOD)
#define KEY_HASH (22 | SHIFT_MOD)
#define KEY_PERCENT (23 | SHIFT_MOD)
#define KEY_AMPERSAND (24 | SHIFT_MOD)
#define KEY_OPEN_PARENTHESIS (25 | SHIFT_MOD)
#define KEY_MINUS (26 | SHIFT_MOD)
#define KEY_LESS (33 | SHIFT_MOD)
#define KEY_QUESTION (34 | SHIFT_MOD)
#define KEY_DOUBLE_QUOTE (41 | SHIFT_MOD)
#define KEY_RIGHT_CURLY_BRACKET (48 | SHIFT_MOD)
#define KEY_TILDE (49 | SHIFT_MOD)
#define KEY_AT (50 | SHIFT_MOD)
#define KEY_DOLLAR (51 | SHIFT_MOD)
#define KEY_CARET (52 | SHIFT_MOD)
#define KEY_ASTERISK (53 | SHIFT_MOD)
#define KEY_CLOSE_PARENTHESIS (54 | SHIFT_MOD)
#define KEY_PLUS (55 | SHIFT_MOD)

#define KEY_DOWN (5 | FN_MOD)
#define KEY_UP (12 | FN_MOD)
#define KEY_DELETE (27 | FN_MOD)
#define KEY_LEFT (33 | FN_MOD)
#define KEY_RIGHT (34 | FN_MOD)
#define KEY_ESC (49 | FN_MOD)

int keymap[56] = {
    0,   // KEY_OPT
    'z', // KEY_Z
    'c', // KEY_C
    'b', // KEY_B
    'm', // KEY_M
    '.', // KEY_DOT
    ' ', // KEY_SPACE
    0,   // KEY_SHIFT
    's', // KEY_S
    'f', // KEY_F
    'h', // KEY_H
    'k', // KEY_K
    ';', // KEY_SEMICOLON
    '\r',// KEY_ENTER
    'q', // KEY_Q
    'e', // KEY_E
    't', // KEY_T
    'u', // KEY_U
    'o', // KEY_O
    '[', // KEY_LEFT_BRACKET
    '\\',// KEY_BACKSLASH
    '1', // KEY_1
    '3', // KEY_3
    '5', // KEY_5
    '7', // KEY_7
    '9', // KEY_9
    '_', // KEY_UNDERSCORE
    '\b',// KEY_BACKSPACE
    0,   // KEY_CTRL
    0,   // KEY_ALT
    'x', // KEY_X
    'v', // KEY_V
    'n', // KEY_N
    ',', // KEY_COMMA
    '/', // KEY_SLASH
    0,   // KEY_FN
    'a', // KEY_A
    'd', // KEY_D
    'g', // KEY_G
    'j', // KEY_J
    'l', // KEY_L
    '\'',// KEY_APOSTROPHE
    '\t',// KEY_TAB
    'w', // KEY_W
    'r', // KEY_R
    'y', // KEY_Y
    'i', // KEY_I
    'p', // KEY_P
    ']', // KEY_RIGHT_BRACKET
    '`', // KEY_GRAVE
    '2', // KEY_2
    '4', // KEY_4
    '6', // KEY_6
    '8', // KEY_8
    '0', // KEY_0
    '='  // KEY_EQUALS
};

int keymap_shifted[56] = {
    0,   // KEY_OPT
    'Z', // KEY_Z
    'C', // KEY_C
    'B', // KEY_B
    'M', // KEY_M
    '>', // KEY_DOT -> '>'
    ' ', // KEY_SPACE
    0,   // KEY_SHIFT
    'S', // KEY_S
    'F', // KEY_F
    'H', // KEY_H
    'K', // KEY_K
    ':', // KEY_SEMICOLON -> ':'
    '\r',// KEY_ENTER
    'Q', // KEY_Q
    'E', // KEY_E
    'T', // KEY_T
    'U', // KEY_U
    'O', // KEY_O
    '{', // KEY_LEFT_BRACKET -> '{'
    '|', // KEY_BACKSLASH -> '|'
    '!', // KEY_1 -> '!'
    '#', // KEY_3 -> '#'
    '%', // KEY_5 -> '%'
    '&', // KEY_7 -> '&'
    '(', // KEY_9 -> '('
    '-', // KEY_UNDERSCORE -> '-'
    '\b',// KEY_BACKSPACE
    0,   // KEY_CTRL
    0,   // KEY_ALT
    'X', // KEY_X
    'V', // KEY_V
    'N', // KEY_N
    '<', // KEY_COMMA -> '<'
    '?', // KEY_SLASH -> '?'
    0,   // KEY_FN
    'A', // KEY_A
    'D', // KEY_D
    'G', // KEY_G
    'J', // KEY_J
    'L', // KEY_L
    '"', // KEY_APOSTROPHE -> '"'
    '\t',// KEY_TAB
    'W', // KEY_W
    'R', // KEY_R
    'Y', // KEY_Y
    'I', // KEY_I
    'P', // KEY_P
    '}', // KEY_RIGHT_BRACKET -> '}'
    '~', // KEY_GRAVE -> '~'
    '@', // KEY_2 -> '@'
    '$', // KEY_4 -> '$'
    '^', // KEY_6 -> '^'
    '*', // KEY_8 -> '*'
    ')', // KEY_0 -> ')'
    '+'  // KEY_EQUALS -> '+'
};
