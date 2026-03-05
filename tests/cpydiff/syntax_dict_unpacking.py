"""
categories: Syntax,dict
description: Unpacking of dicts with ``{**things}}`` raises ``SyntaxError: invalid syntax``.
cause: Unknown
workaround: use ``new_dict = {}; new_dict.update(old_dict)`` to copy values into new dicts.
"""

old_dict = {'a': 1, 'b': 2}
new_dict = {**old_dict, 'c': 3}
new_dict = {'c': 3, **old_dict}
