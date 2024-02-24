"""
categories: Core,Classes
description: Private Class Members name mangling is not implemented
cause: The MicroPython compiler does not implement name mangling for private class members.
workaround: Avoid using or having a collision with global names, by adding a unique prefix to the private class member name manually.
"""


def __print_string(string):
    print(string)


class Foo:
    def __init__(self, string):
        self.string = string

    def do_print(self):
        __print_string(self.string)


example_string = "Example String to print."

class_item = Foo(example_string)
print(class_item.string)

class_item.do_print()
