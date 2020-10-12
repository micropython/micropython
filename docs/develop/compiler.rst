.. _compiler:

The Compiler
=============

The compilation process in MicroPython involves the following steps:

* The lexer converts the stream of text that makes up a MicroPython program into tokens.
* The parser then converts the tokens into an abstract syntax (parse tree).
* Then bytecode or native code is emitted based on the parse tree.

For purposes of this discussion, we are going to add a simple language feature ``add1``
that can be use in Python as:

.. code-block:: bash

    >>> add1 3
    4
    >>>

The ``add1`` statement takes an integer as argument and adds ``1`` to it.

Adding a grammar rule
----------------------

MicroPython's grammar is based on the `CPython grammar <https://docs.python.org/3.5/reference/grammar.html>`_
and is defined in ``py/grammar.h``. This grammar is what is used to parse MicroPython source files.

There are two macros you need to know to define a grammar rule i.e ``DEF_RULE`` or ``DEF_RULE_NC``.
``DEF_RULE`` allows you to define a rule with a compile function. 

A simple grammar definition with a compile function looks like the following:

.. code-block:: c

   DEF_RULE(add1_stmt, c(add1_stmt), and(2), tok(KW_ADD), rule(testlist))

The second argument ``c(add1_stmt)`` is the corresponding compile function that should be implemented
in ``py/compile.c`` for this rule. 

The third important argument can be ``or`` or ``and``. This specifies the number of nodes associated
with a statement. For example, in this case, our ``add1`` statement is similar to ADD1 in assembly
language. It takes one numeric argument. Therefore, the ``add1_stmt`` has two nodes associated with it.
One for the statement itself i.e ``add1`` and the other for its argument. ``testlist`` is the top-level
expression rule.

.. note::
   The ``add1`` rule here is just an example and not part of the standard
   MicroPython grammar.

Finally, the fourth argument in this example is the token associated with the rule. This token should be
defined in the lexer by editing ``py/lexer.h``.

Defining the same rule without a compile function is achieved by using the ``DEF_RULE_NC`` macro
and omitting the compile function argument:

.. code-block:: c

   DEF_RULE_NC(add1_stmt, and(2), tok(KW_ADD))

The remaining arguments take on the same meaning.

.. note::
   The macro ``DEF_RULE`` or ``DEF_RULE_NC`` takes other arguments. For an in-depth understanding of
   supported parameters, see `py/grammar.h <https://github.com/micropython/micropython/blob/master/py/grammar.h>`_.

Adding a lexical token
-----------------------

Every rule defined in the grammar should have a token associated with it that is defined in ``py/lexer.h``. 
Add this token by editing the ``_mp_token_kind_t`` enum:

.. code-block:: c
   :emphasize-lines: 12

   typedef enum _mp_token_kind_t {
    ...
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
   } mp_token_kind_t;

Since, we are adding a keyword, edit ``py/lexer.c`` to add the new keyword:

.. code-block:: c
   :emphasize-lines: 12

   STATIC const char *const tok_kw[] = {
    ...
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
   };

Notice the keyword is named depending on what you want it to be. For consistency, maintain the
naming standard accordingly.

.. note::
   The order of these keywords in ``py/lexer.c`` should match the order of tokens in the enum
   defined in ``py/lexer.h``.

Parsing
--------

The parser takes the tokens produced by the lexer and converts them to an abstract syntax tree (AST) or
*parse tree*. The implementation for the parser is defined in `py/parse.c <https://github.com/micropython/micropython/blob/master/py/parse.c>`_. 

The parser also maintains a table of constants for use in different aspects of parsing, similar to what a `symbol 
table <https://steemit.com/programming/@drifter1/writing-a-simple-compiler-on-my-own-symbol-table-basic-structure>`_ 
does.

Several optimizations like `constant folding <http://compileroptimizations.com/category/constant_folding.htm>`_ 
on integers for all operations i.e logical, binary, unary, etc, optimizing enhancements on parenthesis
around expressions are performed during this phase and optimizations on strings.

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
       mp_raw_code_t *rc = mp_compile_to_raw_code(parse_tree, source_file, is_repl);
       // return function that executes the outer module
       return mp_make_function_from_raw_code(rc, MP_OBJ_NULL, MP_OBJ_NULL);
   }

The compiler compiles the code in several passes.

First pass
~~~~~~~~~~

In the first pass, the compiler computes the stack sizes in scope:

.. code-block:: c

   // compile pass 1
   comp->emit = emit_bc;
   comp->emit_method_table = &emit_bc_method_table;
  
   uint max_num_labels = 0;
   for (scope_t *s = comp->scope_head; s != NULL && comp->compile_error == MP_OBJ_NULL; s = s->next) {
       
        if (s->emit_options == MP_EMIT_OPT_ASM) {
            compile_scope_inline_asm(comp, s, MP_PASS_SCOPE);
        } else
        {
            compile_scope(comp, s, MP_PASS_SCOPE);

            // Check if any implicitly declared variables should be closed over
            for (size_t i = 0; i < s->id_info_len; ++i) {
                id_info_t *id = &s->id_info[i];
                if (id->kind == ID_INFO_KIND_GLOBAL_IMPLICIT) {
                    scope_check_to_close_over(s, id);
                }
            }
        }
   ..
   }

Other computations regarding scopes and identifiers are computed too.
At this point, the number of labels that will be required in the emitted code is also determined and set.

Second and third passes
~~~~~~~~~~~~~~~~~~~~~~~

The second and third passes involve computing the code size and emitting the *inline assembler code* for
the different architectures:

.. code-block:: c
   
   // compile pass 2 and 3
   #if MICROPY_EMIT_NATIVE
   emit_t *emit_native = NULL;
   #endif
   for (scope_t *s = comp->scope_head; s != NULL && comp->compile_error == MP_OBJ_NULL; s = s->next) {
        #if MICROPY_EMIT_INLINE_ASM
        if (s->emit_options == MP_EMIT_OPT_ASM) {
            // inline assembly
            if (comp->emit_inline_asm == NULL) {
                comp->emit_inline_asm = ASM_EMITTER(new)(max_num_labels);
            }
            comp->emit = NULL;
            comp->emit_inline_asm_method_table = ASM_EMITTER_TABLE;
            compile_scope_inline_asm(comp, s, MP_PASS_CODE_SIZE);
            #if MICROPY_EMIT_INLINE_XTENSA
            // Xtensa requires an extra pass to compute size of l32r const table
            // TODO this can be improved by calculating it during SCOPE pass
            // but that requires some other structural changes to the asm emitters
            #if MICROPY_DYNAMIC_COMPILER
            if (mp_dynamic_compiler.native_arch == MP_NATIVE_ARCH_XTENSA)
            #endif
            {
                compile_scope_inline_asm(comp, s, MP_PASS_CODE_SIZE);
            }
            #endif
            if (comp->compile_error == MP_OBJ_NULL) {
                compile_scope_inline_asm(comp, s, MP_PASS_EMIT);
            }
        } else

The inline assembler code comprises assembly instructions in a Python function.
See `the inline assembler tutorial 
<https://docs.micropython.org/en/latest/pyboard/tutorial/assembler.html#pyboard-tutorial-assembler>`_ 
for more details.

Fourth, and final passes
~~~~~~~~~~~~~~~~~~~~~~~~~

The fourth pass emits the final code, but at this point, passes 2 and 3 are also executed:

.. code-block:: c
   
   compile_scope(comp, s, MP_PASS_STACK_SIZE);

   if (comp->compile_error == MP_OBJ_NULL) {
        compile_scope(comp, s, MP_PASS_CODE_SIZE);
   }

   if (comp->compile_error == MP_OBJ_NULL) {
    compile_scope(comp, s, MP_PASS_EMIT);
   }

Before these passes, there is a selection for the type of code to be emitted which can either be native or
bytecode.

.. code-block:: c

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

The bytecode option is the default but something unique to note for the native code option is that there is another
option via ``VIPER``. See the :ref:`Emitting native code <emitting_native_code>` section for
more details on viper annotations.

.. note::
   For inline-assembler code there are 3 passes (scope, code size, emit) but for normal Python functions
   (regardless of the emitter) there are 4 passes (scope, stack size, code size, emit). You can think of
   there always being 4 passes but with inline-assembler pass 2 does nothing.

Emitting bytecode
-----------------

Statements in Python code usually correspond to emitted bytecode, for example ``a + b``
generates ing but instead affect other things like the scope of variables, for example
``global a``. The implementation of the function that emits this bytecode looks similar
to this:

.. code-block:: c
   
   void mp_emit_bc_unary_op(emit_t *emit, mp_unary_op_t op) {
       emit_write_bytecode_byte(emit, 0, MP_BC_UNARY_OP_MULTI + op);
   }

We use the ``yield`` statement for an example here but the implementation details are similar for other statements.
The method ``emit_write_bytecode_byte()`` is a wrapper around the main function ``emit_get_cur_to_write_bytecode()``
that all functions must call to emit byte code.

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
us to emit more than one type of object. By default, Python objects are emitted
but with Viper, something can be declared as a Python object or any type. Viper is
therefore a subset of Python objects, in fact, if anything is declared in Viper
as a Python object, it acts as native Python. Viper typing may break Python equivalence as
integers become native integers and not Python objects.
