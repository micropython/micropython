_print_msg = True


def publicFun3():
    return 3


def __getattr__(attr):
    global _print_msg
    if _print_msg:
        # CPython calls __getattr__("__all__") twice, but MicroPython only once.
        # To make the output match, only print the message once.
        print("__getattr__", attr)
        _print_msg = False
    raise AttributeError(attr)
