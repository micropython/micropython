.. _internals_core:

The MicroPython Core Runtime
============================

This section decribes the core interpreter implementation.

The Grammar
-----------

* A reference of py/grammar.h.

The Lexer
---------

* A reference of py/lexer.c, py/lexer.h.

The Parser
----------

* A reference of py/parse.c, py/parse.h.

The Compiler
------------

The subsection details the compiler implementation, any intermediate 
representations and optimizations done. It also includes how bytecode
and native code is emitted, memory management (GC) and type checking.

* A reference of py/compile.c, py/compile.h.

Emitting Bytecode
~~~~~~~~~~~~~~~~~~~~~
Emitting Native Code
~~~~~~~~~~~~~~~~~~~~~~~~
Type Checking
~~~~~~~~~~~~~

Memory Management
-----------------

Optimizations
-------------

String Interning
~~~~~~~~~~~~~~~~
Open Addressing
~~~~~~~~~~~~~~~
Linear Probing
~~~~~~~~~~~~~~

Support for C Modules
------------------------

Writing Tests
-------------

Adding a Core Module
--------------------

The Public API
--------------

* A reference of runtime.h and obj.h.

The Dynamically Loaded Modules
------------------------------

* A reference of dynruntime.h.

What really Happens when you run a MicroPython Program
------------------------------------------------------

For example, what all happens where in the code 
when you do something like "import foo; foo.bar()".
Suggested by @tve
