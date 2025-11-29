:mod:`string.templatelib` -- Template String Support
====================================================

.. module:: string.templatelib
   :synopsis: PEP 750 template string support

This module provides support for template strings (t-strings) as defined in
`PEP 750 <https://peps.python.org/pep-0750/>`_. Template strings are created
using the ``t`` prefix and provide access to both the literal string parts and
interpolated values before they are combined.

Classes
-------

.. class:: Template(*args)

   Represents a template string. Template objects are typically created by
   t-string syntax (``t"..."``) but can also be constructed directly using
   the constructor.

   .. attribute:: strings

      A tuple of string literals that appear between interpolations.

   .. attribute:: interpolations  

      A tuple of :class:`Interpolation` objects representing the interpolated
      expressions.

   .. attribute:: values

      A read-only property that returns a tuple containing the ``value`` 
      attribute from each interpolation in the template.

   .. method:: __iter__()

      Iterate over the template contents, yielding string parts and
      :class:`Interpolation` objects in the order they appear. Empty strings
      are omitted.

   .. method:: __add__(other)

      Concatenate two templates. Returns a new :class:`Template` combining
      the strings and interpolations from both templates.

      :raises TypeError: if *other* is not a :class:`Template`

      Template concatenation with ``str`` is prohibited to avoid ambiguity
      about whether the string should be treated as a literal or interpolation::

         t1 = t"Hello "
         t2 = t"World"
         result = t1 + t2  # Valid

         # TypeError: cannot concatenate str to Template
         result = t1 + "World"

.. class:: Interpolation(value, expression='', conversion=None, format_spec='')

   Represents an interpolated expression within a template string. All
   arguments can be passed as keyword arguments.

   .. attribute:: value

      The evaluated value of the interpolated expression.

   .. attribute:: expression

      The string representation of the expression as it appeared in the
      template string.

   .. attribute:: conversion

      The conversion specifier (``'s'``, ``'r'``, or ``'a'``) if present,
      otherwise ``None``.

   .. attribute:: format_spec

      The format specification string if present, otherwise an empty string.

Template String Syntax
----------------------

Template strings use the same syntax as f-strings but with a ``t`` prefix::

    name = "World"
    template = t"Hello {name}!"
    
    # Access template components
    print(template.strings)        # ('Hello ', '!')
    print(template.values)         # ('World',)
    print(template.interpolations[0].expression)  # 'name'

Conversion Specifiers
~~~~~~~~~~~~~~~~~~~~~

Template strings store conversion specifiers as metadata. Unlike f-strings,
the conversion is not applied automatically::

    value = "test"
    t = t"{value!r}"
    # t.interpolations[0].value == "test" (not repr(value))
    # t.interpolations[0].conversion == "r"

Processing code must explicitly apply conversions when needed.

Format Specifications
~~~~~~~~~~~~~~~~~~~~~

Format specifications are stored as metadata in the ``Interpolation`` object.
Unlike f-strings, formatting is not applied automatically::

    pi = 3.14159
    t = t"{pi:.2f}"
    # t.interpolations[0].value == 3.14159 (not formatted)
    # t.interpolations[0].format_spec == ".2f"

Per PEP 750, processing code is not required to use format specifications, but
when present they should be respected and match f-string behavior where possible.

Debug Format
~~~~~~~~~~~~

The debug format ``{expr=}`` is supported::

    x = 42
    t = t"{x=}"
    # t.strings == ("x=", "")
    # t.interpolations[0].expression == "x"
    # t.interpolations[0].conversion == "r"

.. admonition:: Important
   :class: attention

   As per PEP 750, unlike f-strings, template strings do not automatically
   apply conversions or format specifications. This is by design to allow
   processing code to control how these are handled. Processing code must
   explicitly handle these attributes.

   MicroPython does not provide the ``format()`` built-in function. Use
   string formatting methods like ``str.format()`` instead.

Differences from CPython
------------------------

This implementation follows PEP 750 with these MicroPython-specific details:

**Memory Limits**: Template strings are subject to MicroPython's memory
constraints. Very large templates may raise ``MemoryError``.

Availability
------------

Template strings require ``MICROPY_PY_TSTRINGS`` to be enabled at compile time.
They are enabled by default in the unix, windows, and webassembly ports.

Example Usage
-------------

Basic processing without format support::

    def simple_process(template):
        """Simple template processing"""
        parts = []
        for item in template:
            if isinstance(item, str):
                parts.append(item)
            else:
                parts.append(str(item.value))
        return "".join(parts)

Processing template with format support::

    from string.templatelib import Template, Interpolation

    def convert(value, conversion):
        """Apply conversion specifier to value"""
        if conversion == "r":
            return repr(value)
        elif conversion == "s":
            return str(value)
        elif conversion == "a":
            return ascii(value)
        return value

    def process_template(template):
        """Process template with conversion and format support"""
        result = []
        for part in template:
            if isinstance(part, str):
                result.append(part)
            else:  # Interpolation
                value = convert(part.value, part.conversion)
                if part.format_spec:
                    # Apply format specification using str.format
                    value = ("{:" + part.format_spec + "}").format(value)
                else:
                    value = str(value)
                result.append(value)
        return "".join(result)
    
    pi = 3.14159
    name = "Alice"
    t = t"{name!r}: {pi:.2f}"
    print(process_template(t))
    # Output: "'Alice': 3.14"
    
    # Other format specifications work too
    value = 42
    print(process_template(t"{value:>10}"))  # "        42"
    print(process_template(t"{value:04d}"))  # "0042"

HTML escaping example::

    def html_escape(value):
        """Escape HTML special characters"""
        if not isinstance(value, str):
            value = str(value)
        return value.replace("&", "&amp;").replace("<", "&lt;").replace(">", "&gt;")
    
    def safe_html(template):
        """Convert template to HTML-safe string"""
        result = []
        for part in template:
            if isinstance(part, str):
                result.append(part)
            else:
                result.append(html_escape(part.value))
        return "".join(result)
    
    user_input = "<script>alert('xss')</script>"
    t = t"User said: {user_input}"
    print(safe_html(t))
    # Output: "User said: &lt;script&gt;alert('xss')&lt;/script&gt;"

See Also
--------

* `PEP 750 <https://peps.python.org/pep-0750/>`_ - Template Strings specification
* :ref:`python:formatstrings` - Format string syntax
* `Formatted string literals <https://docs.python.org/3/reference/lexical_analysis.html#f-strings>`_ - f-strings in Python