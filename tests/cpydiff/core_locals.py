"""
categories: Core,Runtime
description: Local variables aren't included in locals() result
cause: MicroPython doesn't maintain symbolic local environment, it is optimized to an array of slots. Thus, local variables can't be accessed by a name.
workaround: Unknown
"""
def test():
    val = 2
    print(locals())

test()
