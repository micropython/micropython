.. _compiler:

The Compiler
============

The compilation process in MicroPython involves the following steps:

* The lexer converts the stream of text that makes up a MicroPython program into tokens.
* The parser then converts the tokens into an abstract syntax (parse tree).
* Then bytecode or native code is emitted based on the parse tree.

For purposes of this discussion we are going to add a simple language feature ``add1``
that can be use in Python as:

.. code-block:: bash

    >>> add1 3
    4
    >>>

The ``add1`` statement takes an integer as argument and adds ``1`` to it.

Adding a grammar rule
----------------------

MicroPython's grammar is based on the `CPython grammar <https://docs.python.org/3.5/reference/grammar.html>`_
and is defined in `py/grammar.h <https://github.com/micropython/micropython/blob/master/py/grammar.h>`_.
This grammar is what is used to parse MicroPython source files.

There are two macros you need to know to define a grammar rule: ``DEF_RULE`` and ``DEF_RULE_NC``.
``DEF_RULE`` allows you to define a rule with an associated compile function,
while ``DEF_RULE_NC`` has no compile (NC) function for it.

A simple grammar definition with a compile function for our new ``add1`` statement
looks like the following:

.. code-block:: c

   DEF_RULE(add1_stmt, c(add1_stmt), and(2), tok(KW_ADD1), rule(testlist))

The second argument ``c(add1_stmt)`` is the corresponding compile function that should be implemented
in ``py/compile.c`` to turn this rule into executable code.

The third required argument can be ``or`` or ``and``. This specifies the number of nodes associated
with a statement. For example, in this case, our ``add1`` statement is similar to ADD1 in assembly
language. It takes one numeric argument. Therefore, the ``add1_stmt`` has two nodes associated with it.
One node is for the statement itself, i.e the literal ``add1`` corresponding to ``KW_ADD1``,
and the other for its argument, a ``testlist`` rule which is the top-level expression rule.

.. note::
   The ``add1`` rule here is just an example and not part of the standard
   MicroPython grammar.

The fourth argument in this example is the token associated with the rule, ``KW_ADD1``. This token should be
defined in the lexer by editing ``py/lexer.h``.

Defining the same rule without a compile function is achieved by using the ``DEF_RULE_NC`` macro
and omitting the compile function argument:

.. code-block:: c

   DEF_RULE_NC(add1_stmt, and(2), tok(KW_ADD1), rule(testlist))

The remaining arguments take on the same meaning. A rule without a compile function must
be handled explicitly by all rules that may have this rule as a node. Such NC-rules are usually
used to express sub-parts of a complicated grammar structure that cannot be expressed in a
single rule.

.. note::
   The macros ``DEF_RULE`` and ``DEF_RULE_NC`` take other arguments. For an in-depth understanding of
   supported parameters, see `py/grammar.h <https://github.com/micropython/micropython/blob/master/py/grammar.h>`_.

Adding a lexical token
----------------------

Every rule defined in the grammar should have a token associated with it that is defined in ``py/lexer.h``.
Add this token by editing the ``_mp_token_kind_t`` enum:

.. code-block:: c
   :emphasize-lines: 12

   typedef enum _mp_token_kind_t {
       ...
       MP_TOKEN_KW_OR,
       MP_TOKEN_KW_PASS,
       MP_TOKEN_KW_RAISE,
       MP_TOKEN_KW_RETURN,
       MP_TOKEN_KW_TRY,
       MP_TOKEN_KW_WHILE,
       MP_TOKEN_KW_WITH,
       MP_TOKEN_KW_YIELD,
       MP_TOKEN_KW_ADD1,
       ...
   } mp_token_kind_t;

Then also edit ``py/lexer.c`` to add the new keyword literal text:

.. code-block:: c
   :emphasize-lines: 12

   STATIC const char *const tok_kw[] = {
       ...
       "or",
       "pass",
       "raise",
       "return",
       "try",
       "while",
       "with",
       "yield",
       "add1",
       ...
   };

Notice the keyword is named depending on what you want it to be. For consistency, maintain the
naming standard accordingly.

.. note::
   The order of these keywords in ``py/lexer.c`` must match the order of tokens in the enum
   defined in ``py/lexer.h``.

Parsing
-------

In the parsing stage the parser takes the tokens produced by the lexer and converts them to an abstract syntax tree (AST) or
*parse tree*. The implementation for the parser is defined in `py/parse.c <https://github.com/micropython/micropython/blob/master/py/parse.c>`_.

The parser also maintains a table of constants for use in different aspects of parsing, similar to what a
`symbol table <https://steemit.com/programming/@drifter1/writing-a-simple-compiler-on-my-own-symbol-table-basic-structure>`_
does.

Several optimizations like `constant folding <http://compileroptimizations.com/category/constant_folding.htm>`_
on integers for most operations e.g. logical, binary, unary, etc, and optimizing enhancements on parenthesis
around expressions are performed during this phase, along with some optimizations on strings.

It's worth noting that *docstrings* are discarded and not accessible to the compiler.
Even optimizations like `string interning <https://en.wikipedia.org/wiki/String_interning>`_ are
not applied to *docstrings*.

Compiler passes
---------------

Like many compilers, MicroPython compiles all code to MicroPython bytecode or native code. The functionality
that achieves this is implemented in `py/compile.c <https://github.com/micropython/micropython/blob/master/py/compile.c>`_.
The most relevant method you should know about is this:

.. code-block:: c

   mp_obj_t mp_compile(mp_parse_tree_t *parse_tree, qstr source_file, bool is_repl) {
       // Compile the input parse_tree to a raw-code structure.
       mp_raw_code_t *rc = mp_compile_to_raw_code(parse_tree, source_file, is_repl);
       // Create and return a function object that executes the outer module.
       return mp_make_function_from_raw_code(rc, MP_OBJ_NULL, MP_OBJ_NULL);
   }

The compiler compiles the code in four passes: scope, stack size, code size and emit.
Each pass runs the same C code over the same AST data structure, with different things
being computed each time based on the results of the previous pass.

First pass
~~~~~~~~~~

In the first pass, the compiler learns about the known identifiers (variables) and
their scope, being global, local, closed over, etc. In the same pass the emitter
(bytecode or native code) also computes the number of labels needed for the emitted
code.

.. code-block:: c

   // Compile pass 1.
   comp->emit = emit_bc;
   comp->emit_method_table = &emit_bc_method_table;

   uint max_num_labels = 0;
   for (scope_t *s = comp->scope_head; s != NULL && comp->compile_error == MP_OBJ_NULL; s = s->next) {
       if (s->emit_options == MP_EMIT_OPT_ASM) {
           compile_scope_inline_asm(comp, s, MP_PASS_SCOPE);
       } else {
           compile_scope(comp, s, MP_PASS_SCOPE);

           // Check if any implicitly declared variables should be closed over.
           for (size_t i = 0; i < s->id_info_len; ++i) {
               id_info_t *id = &s->id_info[i];
               if (id->kind == ID_INFO_KIND_GLOBAL_IMPLICIT) {
                   scope_check_to_close_over(s, id);
               }
           }
       }
       ...
   }

Second and third passes
~~~~~~~~~~~~~~~~~~~~~~~

The second and third passes involve computing the Python stack size and code size
for the bytecode or native code. After the third pass the code size cannot change,
otherwise jump labels will be incorrect.

.. code-block:: c

   for (scope_t *s = comp->scope_head; s != NULL && comp->compile_error == MP_OBJ_NULL; s = s->next) {
       ...

       // Pass 2: Compute the Python stack size.
       compile_scope(comp, s, MP_PASS_STACK_SIZE);

       // Pass 3: Compute the code size.
       if (comp->compile_error == MP_OBJ_NULL) {
           compile_scope(comp, s, MP_PASS_CODE_SIZE);
       }

       ...
   }

Just before pass two there is a selection for the type of code to be emitted, which can
either be native or bytecode.

.. code-block:: c

   // Choose the emitter type.
   switch (s->emit_options) {
       case MP_EMIT_OPT_NATIVE_PYTHON:
       case MP_EMIT_OPT_VIPER:
           if (emit_native == NULL) {
               emit_native = NATIVE_EMITTER(new)(&comp->compile_error, &comp->next_label, max_num_labels);
           }
           comp->emit_method_table = NATIVE_EMITTER_TABLE;
           comp->emit = emit_native;
           break;

       default:
           comp->emit = emit_bc;
           comp->emit_method_table = &emit_bc_method_table;
           break;
   }

The bytecode option is the default but something unique to note for the native
code option is that there is another option via ``VIPER``. See the
:ref:`Emitting native code <emitting_native_code>` section for more details on
viper annotations.

There is also support for *inline assembly code*, where assembly instructions are
written as Python function calls but are emitted directly as the corresponding
machine code. This assembler has only three passes (scope, code size, emit)
and uses a different implementation, not the ``compile_scope`` function.
See the `inline assembler tutorial <https://docs.micropython.org/en/latest/pyboard/tutorial/assembler.html#pyboard-tutorial-assembler>`_
for more details.

Fourth pass
~~~~~~~~~~~

The fourth pass emits the final code that can be executed, either bytecode in
the virtual machine, or native code directly by the CPU.

.. code-block:: c

   for (scope_t *s = comp->scope_head; s != NULL && comp->compile_error == MP_OBJ_NULL; s = s->next) {
       ...

       // Pass 4: Emit the compiled bytecode or native code.
       if (comp->compile_error == MP_OBJ_NULL) {
           compile_scope(comp, s, MP_PASS_EMIT);
       }
   }

Emitting bytecode
-----------------

Statements in Python code usually correspond to emitted bytecode, for example ``a + b``
generates "push a" then "push b" then "binary op add". Some statements do not emit
anything but instead affect other things like the scope of variables, for example
``global a``.

The implementation of a function that emits bytecode looks similar to this:

.. code-block:: c

   void mp_emit_bc_unary_op(emit_t *emit, mp_unary_op_t op) {
       emit_write_bytecode_byte(emit, 0, MP_BC_UNARY_OP_MULTI + op);
   }

We use the unary operator expressions for an example here but the implementation
details are similar for other statements/expressions. The method ``emit_write_bytecode_byte()``
is a wrapper around the main function ``emit_get_cur_to_write_bytecode()`` that all
functions must call to emit bytecode.

.. _emitting_native_code:

Emitting native code
---------------------

Similar to how bytecode is generated, there should be a corresponding function in ``py/emitnative.c`` for each
code statement:

.. code-block:: c

   STATIC void emit_native_unary_op(emit_t *emit, mp_unary_op_t op) {
        vtype_kind_t vtype;
        emit_pre_pop_reg(emit, &vtype, REG_ARG_2);
        if (vtype == VTYPE_PYOBJ) {
            emit_call_with_imm_arg(emit, MP_F_UNARY_OP, op, REG_ARG_1);
            emit_post_push_reg(emit, VTYPE_PYOBJ, REG_RET);
        } else {
            adjust_stack(emit, 1);
            EMIT_NATIVE_VIPER_TYPE_ERROR(emit,
                MP_ERROR_TEXT("unary op %q not implemented"), mp_unary_op_method_name[op]);
        }
   }

The difference here is that we have to handle *viper typing*. Viper annotations allow
us to handle more than one type of variable. By default all variables are Python objects,
but with viper a variable can also be declared as a machine-typed variable like a native
integer or pointer. Viper can be thought of as a superset of Python, where normal Python
objects are handled as usual, while native machine variables are handled in an optimised
way by using direct machine instructions for the operations. Viper typing may break
Python equivalence because, for example, integers become native integers and can overflow
(unlike Python integers which extend automatically to arbitrary precision).
