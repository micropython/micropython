"""
categories: Syntax,tuple,list
description: Unpacking of tuples and lists with ``[*things]`` or ``(*things)`` raises ``SyntaxError: *x must be assignment target``.
cause: Unknown
workaround: use ``new = []; for t in old: new.append(t)`` to copy into new lists and then cast to tuple if a tuple is needed.
"""

old_list = [1, 2, 3]
new_list = [*old_list, 4]
new_list = [0, *old_list]

old_tup = (1, 2, 3)
new_tup = (*old_tup, 4)
new_tup = (0, *old_tup)
