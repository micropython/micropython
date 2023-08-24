DEBUG = False

import gc
import os

NORMAL, WHITESPACE = 0, 1


def garbage_collect(msg, verbose=DEBUG):
    free_before = gc.mem_free()
    gc.collect()
    free_after = gc.mem_free()
    if verbose:
        print("%s: %d --gc--> %d bytes free" % (msg, free_before, free_after))


def split_tokens(line):
    buf = ""
    tokens = []
    state = NORMAL
    for c in line:
        if c in "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_":
            if state != NORMAL:
                if len(buf) > 0:
                    tokens.append(buf)
                    buf = ""
                state = NORMAL
            buf += c
        elif c in " \t":
            if state != WHITESPACE:
                if len(buf) > 0:
                    tokens.append(buf)
                    buf = ""
                state = WHITESPACE
            buf += c
        else:
            if len(buf) > 0:
                tokens.append(buf)
                buf = ""
            tokens.append(c)

    if len(buf) > 0:
        tokens.append(buf)

    return tokens


def validate_expression(param):
    for token in split_tokens(param):
        state = 0
        for c in token:
            if c not in ' \t+-*/%()<>&|~xX0123456789abcdefABCDEF':
                return False

            # the following allows hex digits a-f after 0x but not otherwise
            if state == 0:
                if c in 'abcdefABCDEF':
                    return False
                if c == '0':
                    state = 1
                continue

            if state == 1:
                state = 2 if c in 'xX' else 0
                continue

            if state == 2:
                if c not in '0123456789abcdefABCDEF':
                    state = 0
    return True


def file_exists(filename):
    try:
        os.stat(filename)
        return True
    except OSError:
        pass
    return False
