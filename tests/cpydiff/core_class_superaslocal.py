"""
categories: Core,Classes
description: Bug when using "super" as a local
cause: Unknown
workaround: Unknown
"""
class A:
    def foo(self):
        super = [1]
        super.pop()
        print(super)

A().foo()
