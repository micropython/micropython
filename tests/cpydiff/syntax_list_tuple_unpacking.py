"""
categories: Syntax,tuple,list
description: Unpacking one tuple or list's elements into a new object with ``[*things]`` or ``(*things)`` is not supported.
cause: Not implemented. Sequence unpacking is only implemented when passing arguments to functions.
workaround: use ``new = []; for t in old: new.append(t)`` or ``new = [] + old`` to copy into new lists and then cast to tuple if a tuple is needed.
"""

old_list = [1, 2, 3]
new_list = [*old_list, 4]
new_list = [0, *old_list]

old_tup = (1, 2, 3)
new_tup = (*old_tup, 4)
new_tup = (0, *old_tup)
