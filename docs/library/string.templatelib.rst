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

.. class:: Template(strings, interpolations)

   Represents a template string. This class cannot be instantiated directly;
   template objects are created by using t-string syntax (``t"..."``).

   .. attribute:: strings

      A tuple of string literals that appear between interpolations.

   .. attribute:: interpolations  

      A tuple of :class:`Interpolation` objects representing the interpolated
      expressions.

   .. attribute:: Template.values

      A computed property that returns a tuple of all interpolated values.

   .. method:: __iter__()

      Iterates over the template parts, alternating between string literals
      and :class:`Interpolation` objects. Empty strings are omitted from
      iteration as specified in PEP 750.

.. class:: Interpolation(value, expression, conversion, format_spec)

   Represents an interpolated expression within a template string. This class
   cannot be instantiated directly; interpolation objects are created
   automatically when using t-string syntax.

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

Like f-strings, template strings support conversion specifiers::

    value = "test"
    t = t"{value!r}"  # repr() conversion
    t = t"{value!s}"  # str() conversion  
    t = t"{value!a}"  # ascii() conversion

Format Specifications
~~~~~~~~~~~~~~~~~~~~~

Format specifications are also supported::

    pi = 3.14159
    t = t"{pi:.2f}"  # Format as float with 2 decimal places
    t = t"{pi:>10}"  # Right-align in 10 characters

Debug Format
~~~~~~~~~~~~

The debug format ``{expr=}`` is supported::

    x = 42
    t = t"{x=}"
    # t.interpolations[0].expression == "x="
    # t.interpolations[0].conversion == "r"

Differences from CPython
------------------------

This implementation follows PEP 750 with these MicroPython-specific details:

1. **No Direct Instantiation**: The :class:`Template` and :class:`Interpolation`
   classes cannot be instantiated directly using their constructors.

2. **Memory Limits**: Template strings are subject to MicroPython's memory
   constraints. Very large templates may raise ``MemoryError``.

3. **Implementation Location**: While PEP 750 places these in ``string.templatelib``,
   MicroPython implements them as builtin types for efficiency.

4. **No Concatenation**: Template concatenation (``t"a" + t"b"``) is not
   supported as per PEP 750.

Example Usage
-------------

Basic template processing::

    def process_template(template):
        """Process a template and return formatted result"""
        result = []
        for part in template:
            if isinstance(part, str):
                result.append(part)
            else:  # Interpolation object
                # Custom processing of interpolated values
                result.append(f"[{part.value}]")
        return "".join(result)
    
    name = "Alice"
    age = 30
    t = t"Hello {name}, you are {age} years old"
    print(process_template(t))
    # Output: "Hello [Alice], you are [30] years old"

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
* f-strings - Formatted string literals (use the same syntax but with ``f`` prefix)