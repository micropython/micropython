"""
categories: Core,Classes
description: Keyword arguments are not passed to __init_subclass__.
cause: Micropython doesn't allow kwargs in a base class list.
workaround: Unknown
"""


class Philosopher:
    def __init_subclass__(cls, default_name, **kwargs):
        super().__init_subclass__(**kwargs)
        cls.default_name = default_name


class AustralianPhilosopher(Philosopher, default_name="Bruce"):
    pass


print(AustralianPhilosopher.default_name)
