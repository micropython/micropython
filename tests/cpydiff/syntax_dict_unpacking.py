"""
categories: Syntax,dict
description: Unpacking one dict's elements into a new dict with ``{'new_elem': val, **old_dict}`` is not supported.
cause: Not implemented. Dict unpacking is only implemented when passing dict elements as function kwargs.
workaround: use ``new_dict = {}; new_dict.update(old_dict)`` to copy values into new dicts.
"""

old_dict = {'a': 1, 'b': 2}
new_dict = {**old_dict, 'c': 3}
new_dict = {'c': 3, **old_dict}
