"""
categories: Core,Classes
description: Descriptor ``__set_name__`` functions may be called twice or missed if members of the parent class are created or removed in ``__set_name__``.
cause: The ``__set_name__`` procedure is not isolated from the underlying modify-while-iter sequence hazard of the underlying class ``__dict__``.
workaround: Avoid ``__set_name__`` implementations that add or remove members from the parent class.
"""

# This bug is EXTREMELY difficult to demonstrate with only a minimal test case
# due to the unstable iteration order of a class's namespace. This bug more-or-less
# _requires_ a stochastic test in order to guarantee it occurs or demonstrate its
# potential impact with any level of clarity.

import random
import re

letters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"

junk_re = re.compile(r"[A-Z][A-Z][A-Z][A-Z][A-Z]")


def junk_fill(obj, n=10):  # Add randomly-generated attributes to an object.
    for i in range(n):
        name = "".join(random.choice(letters) for j in range(5))
        setattr(obj, name, object())


def junk_clear(obj):  # Remove attributes added by junk_fill.
    to_del = [name for name in dir(obj) if junk_re.match(name)]
    for name in to_del:
        delattr(obj, name)


def junk_sequencer():
    global runs
    try:
        while True:
            owner, name = yield
            runs[name] = runs.get(name, 0) + 1
            junk_fill(owner)
    finally:
        junk_clear(owner)


class JunkMaker:
    def __set_name__(self, owner, name):
        global seq
        seq.send((owner, name))


runs = {}
seq = junk_sequencer()
next(seq)


class Main:
    a = JunkMaker()
    b = JunkMaker()
    c = JunkMaker()
    d = JunkMaker()
    e = JunkMaker()
    f = JunkMaker()
    g = JunkMaker()
    h = JunkMaker()
    i = JunkMaker()
    j = JunkMaker()
    k = JunkMaker()
    l = JunkMaker()
    m = JunkMaker()
    n = JunkMaker()
    o = JunkMaker()
    p = JunkMaker()
    q = JunkMaker()
    r = JunkMaker()
    s = JunkMaker()
    t = JunkMaker()
    u = JunkMaker()
    v = JunkMaker()
    w = JunkMaker()
    x = JunkMaker()
    y = JunkMaker()
    z = JunkMaker()


seq.close()

for k in letters.lower():
    print(k, runs.get(k, 0))
