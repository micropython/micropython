/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * SPDX-FileCopyrightText: Copyright (c) 2013-2020 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

// *FORMAT-OFF*

// rules for writing rules:
// - zero_or_more is implemented using opt_rule around a one_or_more rule
// - don't put opt_rule in arguments of or rule; instead, wrap the call to this or rule in opt_rule

// Generic sub-rules used by multiple rules below.

DEF_RULE_NC(generic_colon_test, and_ident(2), tok(DEL_COLON), rule(test))
DEF_RULE_NC(generic_equal_test, and_ident(2), tok(DEL_EQUAL), rule(test))

// # Start symbols for the grammar:
// #       single_input is a single interactive statement;
// #       file_input is a module or sequence of commands read from an input file;
// #       eval_input is the input for the eval() functions.
// # NB: compound_stmt in single_input is followed by extra NEWLINE! --> not in MicroPython
// single_input: NEWLINE | simple_stmt | compound_stmt
// file_input: (NEWLINE | stmt)* ENDMARKER
// eval_input: testlist NEWLINE* ENDMARKER

DEF_RULE_NC(single_input, or(3), tok(NEWLINE), rule(simple_stmt), rule(compound_stmt))
DEF_RULE(file_input, c(generic_all_nodes), and_ident(1), opt_rule(file_input_2))
DEF_RULE(file_input_2, c(generic_all_nodes), one_or_more, rule(file_input_3))
DEF_RULE_NC(file_input_3, or(2), tok(NEWLINE), rule(stmt))
DEF_RULE_NC(eval_input, and_ident(2), rule(testlist), opt_rule(eval_input_2))
DEF_RULE_NC(eval_input_2, and(1), tok(NEWLINE))

// decorator: '@' dotted_name [ '(' [arglist] ')' ] NEWLINE
// decorators: decorator+
// decorated: decorators (classdef | funcdef | async_funcdef)
// funcdef: 'def' NAME parameters ['->' test] ':' suite
// async_funcdef: 'async' funcdef
// parameters: '(' [typedargslist] ')'
// typedargslist: tfpdef ['=' test] (',' tfpdef ['=' test])* [',' ['*' [tfpdef] (',' tfpdef ['=' test])* [',' '**' tfpdef] | '**' tfpdef]] | '*' [tfpdef] (',' tfpdef ['=' test])* [',' '**' tfpdef] | '**' tfpdef
// tfpdef: NAME [':' test]
// varargslist: vfpdef ['=' test] (',' vfpdef ['=' test])* [',' ['*' [vfpdef] (',' vfpdef ['=' test])* [',' '**' vfpdef] | '**' vfpdef]] |  '*' [vfpdef] (',' vfpdef ['=' test])* [',' '**' vfpdef] | '**' vfpdef
// vfpdef: NAME

DEF_RULE_NC(decorator, and(4), tok(OP_AT), rule(dotted_name), opt_rule(trailer_paren), tok(NEWLINE))
DEF_RULE_NC(decorators, one_or_more, rule(decorator))
DEF_RULE(decorated, c(decorated), and_ident(2), rule(decorators), rule(decorated_body))
#if MICROPY_PY_ASYNC_AWAIT
DEF_RULE_NC(decorated_body, or(3), rule(classdef), rule(funcdef), rule(async_funcdef))
DEF_RULE_NC(async_funcdef, and(2), tok(KW_ASYNC), rule(funcdef))
#else
DEF_RULE_NC(decorated_body, or(2), rule(classdef), rule(funcdef))
#endif
DEF_RULE(funcdef, c(funcdef), and_blank(8), tok(KW_DEF), tok(NAME), tok(DEL_PAREN_OPEN), opt_rule(typedargslist), tok(DEL_PAREN_CLOSE), opt_rule(funcdefrettype), tok(DEL_COLON), rule(suite))
DEF_RULE_NC(funcdefrettype, and_ident(2), tok(DEL_MINUS_MORE), rule(test))
// note: typedargslist lets through more than is allowed, compiler does further checks
DEF_RULE_NC(typedargslist, list_with_end, rule(typedargslist_item), tok(DEL_COMMA))
DEF_RULE_NC(typedargslist_item, or(3), rule(typedargslist_name), rule(typedargslist_star), rule(typedargslist_dbl_star))
DEF_RULE_NC(typedargslist_name, and_ident(3), tok(NAME), opt_rule(generic_colon_test), opt_rule(generic_equal_test))
DEF_RULE_NC(typedargslist_star, and(2), tok(OP_STAR), opt_rule(tfpdef))
DEF_RULE_NC(typedargslist_dbl_star, and(3), tok(OP_DBL_STAR), tok(NAME), opt_rule(generic_colon_test))
DEF_RULE_NC(tfpdef, and(2), tok(NAME), opt_rule(generic_colon_test))
// note: varargslist lets through more than is allowed, compiler does further checks
DEF_RULE_NC(varargslist, list_with_end, rule(varargslist_item), tok(DEL_COMMA))
DEF_RULE_NC(varargslist_item, or(3), rule(varargslist_name), rule(varargslist_star), rule(varargslist_dbl_star))
DEF_RULE_NC(varargslist_name, and_ident(2), tok(NAME), opt_rule(generic_equal_test))
DEF_RULE_NC(varargslist_star, and(2), tok(OP_STAR), opt_rule(vfpdef))
DEF_RULE_NC(varargslist_dbl_star, and(2), tok(OP_DBL_STAR), tok(NAME))
DEF_RULE_NC(vfpdef, and_ident(1), tok(NAME))

// stmt: compound_stmt | simple_stmt

DEF_RULE_NC(stmt, or(2), rule(compound_stmt), rule(simple_stmt))

// simple_stmt: small_stmt (';' small_stmt)* [';'] NEWLINE

DEF_RULE_NC(simple_stmt, and_ident(2), rule(simple_stmt_2), tok(NEWLINE))
DEF_RULE(simple_stmt_2, c(generic_all_nodes), list_with_end, rule(small_stmt), tok(DEL_SEMICOLON))

// small_stmt: expr_stmt | del_stmt | pass_stmt | flow_stmt | import_stmt | global_stmt | nonlocal_stmt | assert_stmt
// expr_stmt: testlist_star_expr (annassign | augassign (yield_expr|testlist) | ('=' (yield_expr|testlist_star_expr))*)
// testlist_star_expr: (test|star_expr) (',' (test|star_expr))* [',']
// annassign: ':' test ['=' (yield_expr|testlist_star_expr)]
// augassign: '+=' | '-=' | '*=' | '@=' | '/=' | '%=' | '&=' | '|=' | '^=' | '<<=' | '>>=' | '**=' | '//='
// # For normal and annotated assignments, additional restrictions enforced by the interpreter

DEF_RULE_NC(small_stmt, or(8), rule(del_stmt), rule(pass_stmt), rule(flow_stmt), rule(import_stmt), rule(global_stmt), rule(nonlocal_stmt), rule(assert_stmt), rule(expr_stmt))
DEF_RULE(expr_stmt, c(expr_stmt), and(2), rule(testlist_star_expr), opt_rule(expr_stmt_2))
DEF_RULE_NC(expr_stmt_2, or(3), rule(annassign), rule(expr_stmt_augassign), rule(expr_stmt_assign_list))
DEF_RULE_NC(expr_stmt_augassign, and_ident(2), rule(augassign), rule(expr_stmt_6))
DEF_RULE_NC(expr_stmt_assign_list, one_or_more, rule(expr_stmt_assign))
DEF_RULE_NC(expr_stmt_assign, and_ident(2), tok(DEL_EQUAL), rule(expr_stmt_6))
DEF_RULE_NC(expr_stmt_6, or(2), rule(yield_expr), rule(testlist_star_expr))
DEF_RULE(testlist_star_expr, c(generic_tuple), list_with_end, rule(testlist_star_expr_2), tok(DEL_COMMA))
DEF_RULE_NC(testlist_star_expr_2, or(2), rule(star_expr), rule(test))
DEF_RULE_NC(annassign, and(3), tok(DEL_COLON), rule(test), opt_rule(expr_stmt_assign))
DEF_RULE_NC(augassign, or(13), tok(DEL_PLUS_EQUAL), tok(DEL_MINUS_EQUAL), tok(DEL_STAR_EQUAL), tok(DEL_AT_EQUAL), tok(DEL_SLASH_EQUAL), tok(DEL_PERCENT_EQUAL), tok(DEL_AMPERSAND_EQUAL), tok(DEL_PIPE_EQUAL), tok(DEL_CARET_EQUAL), tok(DEL_DBL_LESS_EQUAL), tok(DEL_DBL_MORE_EQUAL), tok(DEL_DBL_STAR_EQUAL), tok(DEL_DBL_SLASH_EQUAL))

// del_stmt: 'del' exprlist
// pass_stmt: 'pass'
// flow_stmt: break_stmt | continue_stmt | return_stmt | raise_stmt | yield_stmt
// break_stmt: 'break'
// continue_stmt: 'continue'
// return_stmt: 'return' [testlist]
// yield_stmt: yield_expr
// raise_stmt: 'raise' [test ['from' test]]

DEF_RULE(del_stmt, c(del_stmt), and(2), tok(KW_DEL), rule(exprlist))
DEF_RULE(pass_stmt, c(generic_all_nodes), and(1), tok(KW_PASS))
DEF_RULE_NC(flow_stmt, or(5), rule(break_stmt), rule(continue_stmt), rule(return_stmt), rule(raise_stmt), rule(yield_stmt))
DEF_RULE(break_stmt, c(break_cont_stmt), and(1), tok(KW_BREAK))
DEF_RULE(continue_stmt, c(break_cont_stmt), and(1), tok(KW_CONTINUE))
DEF_RULE(return_stmt, c(return_stmt), and(2), tok(KW_RETURN), opt_rule(testlist))
DEF_RULE(yield_stmt, c(yield_stmt), and(1), rule(yield_expr))
DEF_RULE(raise_stmt, c(raise_stmt), and(2), tok(KW_RAISE), opt_rule(raise_stmt_arg))
DEF_RULE_NC(raise_stmt_arg, and_ident(2), rule(test), opt_rule(raise_stmt_from))
DEF_RULE_NC(raise_stmt_from, and_ident(2), tok(KW_FROM), rule(test))

// import_stmt: import_name | import_from
// import_name: 'import' dotted_as_names
// import_from: 'from' (('.' | '...')* dotted_name | ('.' | '...')+) 'import' ('*' | '(' import_as_names ')' | import_as_names)
// import_as_name: NAME ['as' NAME]
// dotted_as_name: dotted_name ['as' NAME]
// import_as_names: import_as_name (',' import_as_name)* [',']
// dotted_as_names: dotted_as_name (',' dotted_as_name)*
// dotted_name: NAME ('.' NAME)*
// global_stmt: 'global' NAME (',' NAME)*
// nonlocal_stmt: 'nonlocal' NAME (',' NAME)*
// assert_stmt: 'assert' test [',' test]

DEF_RULE_NC(import_stmt, or(2), rule(import_name), rule(import_from))
DEF_RULE(import_name, c(import_name), and(2), tok(KW_IMPORT), rule(dotted_as_names))
DEF_RULE(import_from, c(import_from), and(4), tok(KW_FROM), rule(import_from_2), tok(KW_IMPORT), rule(import_from_3))
DEF_RULE_NC(import_from_2, or(2), rule(dotted_name), rule(import_from_2b))
DEF_RULE_NC(import_from_2b, and_ident(2), rule(one_or_more_period_or_ellipsis), opt_rule(dotted_name))
DEF_RULE_NC(import_from_3, or(3), tok(OP_STAR), rule(import_as_names_paren), rule(import_as_names))
DEF_RULE_NC(import_as_names_paren, and_ident(3), tok(DEL_PAREN_OPEN), rule(import_as_names), tok(DEL_PAREN_CLOSE))
DEF_RULE_NC(one_or_more_period_or_ellipsis, one_or_more, rule(period_or_ellipsis))
DEF_RULE_NC(period_or_ellipsis, or(2), tok(DEL_PERIOD), tok(ELLIPSIS))
DEF_RULE_NC(import_as_name, and(2), tok(NAME), opt_rule(as_name))
DEF_RULE_NC(dotted_as_name, and_ident(2), rule(dotted_name), opt_rule(as_name))
DEF_RULE_NC(as_name, and_ident(2), tok(KW_AS), tok(NAME))
DEF_RULE_NC(import_as_names, list_with_end, rule(import_as_name), tok(DEL_COMMA))
DEF_RULE_NC(dotted_as_names, list, rule(dotted_as_name), tok(DEL_COMMA))
DEF_RULE_NC(dotted_name, list, tok(NAME), tok(DEL_PERIOD))
DEF_RULE(global_stmt, c(global_nonlocal_stmt), and(2), tok(KW_GLOBAL), rule(name_list))
DEF_RULE(nonlocal_stmt, c(global_nonlocal_stmt), and(2), tok(KW_NONLOCAL), rule(name_list))
DEF_RULE_NC(name_list, list, tok(NAME), tok(DEL_COMMA))
DEF_RULE(assert_stmt, c(assert_stmt), and(3), tok(KW_ASSERT), rule(test), opt_rule(assert_stmt_extra))
DEF_RULE_NC(assert_stmt_extra, and_ident(2), tok(DEL_COMMA), rule(test))

// compound_stmt: if_stmt | while_stmt | for_stmt | try_stmt | with_stmt | funcdef | classdef | decorated | async_stmt
// if_stmt: 'if' test ':' suite ('elif' test ':' suite)* ['else' ':' suite]
// while_stmt: 'while' test ':' suite ['else' ':' suite]
// for_stmt: 'for' exprlist 'in' testlist ':' suite ['else' ':' suite]
// try_stmt: 'try' ':' suite ((except_clause ':' suite)+ ['else' ':' suite] ['finally' ':' suite] | 'finally' ':' suite)
// # NB compile.c makes sure that the default except clause is last
// except_clause: 'except' [test ['as' NAME]]
// with_stmt: 'with' with_item (',' with_item)* ':' suite
// with_item: test ['as' expr]
// suite: simple_stmt | NEWLINE INDENT stmt+ DEDENT
// async_stmt: 'async' (funcdef | with_stmt | for_stmt)

#if MICROPY_PY_ASYNC_AWAIT
DEF_RULE_NC(compound_stmt, or(9), rule(if_stmt), rule(while_stmt), rule(for_stmt), rule(try_stmt), rule(with_stmt), rule(funcdef), rule(classdef), rule(decorated), rule(async_stmt))
DEF_RULE(async_stmt, c(async_stmt), and(2), tok(KW_ASYNC), rule(async_stmt_2))
DEF_RULE_NC(async_stmt_2, or(3), rule(funcdef), rule(with_stmt), rule(for_stmt))
#else
DEF_RULE_NC(compound_stmt, or(8), rule(if_stmt), rule(while_stmt), rule(for_stmt), rule(try_stmt), rule(with_stmt), rule(funcdef), rule(classdef), rule(decorated))
#endif
DEF_RULE(if_stmt, c(if_stmt), and(6), tok(KW_IF), rule(namedexpr_test), tok(DEL_COLON), rule(suite), opt_rule(if_stmt_elif_list), opt_rule(else_stmt))
DEF_RULE_NC(if_stmt_elif_list, one_or_more, rule(if_stmt_elif))
DEF_RULE_NC(if_stmt_elif, and(4), tok(KW_ELIF), rule(namedexpr_test), tok(DEL_COLON), rule(suite))
DEF_RULE(while_stmt, c(while_stmt), and(5), tok(KW_WHILE), rule(namedexpr_test), tok(DEL_COLON), rule(suite), opt_rule(else_stmt))
DEF_RULE(for_stmt, c(for_stmt), and(7), tok(KW_FOR), rule(exprlist), tok(KW_IN), rule(testlist), tok(DEL_COLON), rule(suite), opt_rule(else_stmt))
DEF_RULE(try_stmt, c(try_stmt), and(4), tok(KW_TRY), tok(DEL_COLON), rule(suite), rule(try_stmt_2))
DEF_RULE_NC(try_stmt_2, or(2), rule(try_stmt_except_and_more), rule(try_stmt_finally))
DEF_RULE_NC(try_stmt_except_and_more, and_ident(3), rule(try_stmt_except_list), opt_rule(else_stmt), opt_rule(try_stmt_finally))
DEF_RULE_NC(try_stmt_except, and(4), tok(KW_EXCEPT), opt_rule(try_stmt_as_name), tok(DEL_COLON), rule(suite))
DEF_RULE_NC(try_stmt_as_name, and_ident(2), rule(test), opt_rule(as_name))
DEF_RULE_NC(try_stmt_except_list, one_or_more, rule(try_stmt_except))
DEF_RULE_NC(try_stmt_finally, and(3), tok(KW_FINALLY), tok(DEL_COLON), rule(suite))
DEF_RULE_NC(else_stmt, and_ident(3), tok(KW_ELSE), tok(DEL_COLON), rule(suite))
DEF_RULE(with_stmt, c(with_stmt), and(4), tok(KW_WITH), rule(with_stmt_list), tok(DEL_COLON), rule(suite))
DEF_RULE_NC(with_stmt_list, list, rule(with_item), tok(DEL_COMMA))
DEF_RULE_NC(with_item, and_ident(2), rule(test), opt_rule(with_item_as))
DEF_RULE_NC(with_item_as, and_ident(2), tok(KW_AS), rule(expr))
DEF_RULE_NC(suite, or(2), rule(suite_block), rule(simple_stmt))
DEF_RULE_NC(suite_block, and_ident(4), tok(NEWLINE), tok(INDENT), rule(suite_block_stmts), tok(DEDENT))
DEF_RULE(suite_block_stmts, c(generic_all_nodes), one_or_more, rule(stmt))

// test: or_test ['if' or_test 'else' test] | lambdef
// test_nocond: or_test | lambdef_nocond
// lambdef: 'lambda' [varargslist] ':' test
// lambdef_nocond: 'lambda' [varargslist] ':' test_nocond

#if MICROPY_PY_ASSIGN_EXPR
DEF_RULE(namedexpr_test, c(namedexpr), and_ident(2), rule(test), opt_rule(namedexpr_test_2))
DEF_RULE_NC(namedexpr_test_2, and_ident(2), tok(OP_ASSIGN), rule(test))
#else
DEF_RULE_NC(namedexpr_test, or(1), rule(test))
#endif
DEF_RULE_NC(test, or(2), rule(lambdef), rule(test_if_expr))
DEF_RULE(test_if_expr, c(test_if_expr), and_ident(2), rule(or_test), opt_rule(test_if_else))
DEF_RULE_NC(test_if_else, and(4), tok(KW_IF), rule(or_test), tok(KW_ELSE), rule(test))
DEF_RULE_NC(test_nocond, or(2), rule(lambdef_nocond), rule(or_test))
DEF_RULE(lambdef, c(lambdef), and_blank(4), tok(KW_LAMBDA), opt_rule(varargslist), tok(DEL_COLON), rule(test))
DEF_RULE(lambdef_nocond, c(lambdef), and_blank(4), tok(KW_LAMBDA), opt_rule(varargslist), tok(DEL_COLON), rule(test_nocond))

// or_test: and_test ('or' and_test)*
// and_test: not_test ('and' not_test)*
// not_test: 'not' not_test | comparison
// comparison: expr (comp_op expr)*
// comp_op: '<'|'>'|'=='|'>='|'<='|'!='|'in'|'not' 'in'|'is'|'is' 'not'
// star_expr: '*' expr
// expr: xor_expr ('|' xor_expr)*
// xor_expr: and_expr ('^' and_expr)*
// and_expr: shift_expr ('&' shift_expr)*
// shift_expr: arith_expr (('<<'|'>>') arith_expr)*
// arith_expr: term (('+'|'-') term)*
// term: factor (('*'|'@'|'/'|'%'|'//') factor)*
// factor: ('+'|'-'|'~') factor | power
// power: atom_expr ['**' factor]
// atom_expr: 'await' atom trailer* | atom trailer*

DEF_RULE(or_test, c(or_and_test), list, rule(and_test), tok(KW_OR))
DEF_RULE(and_test, c(or_and_test), list, rule(not_test), tok(KW_AND))
DEF_RULE_NC(not_test, or(2), rule(not_test_2), rule(comparison))
DEF_RULE(not_test_2, c(not_test_2), and(2), tok(KW_NOT), rule(not_test))
DEF_RULE(comparison, c(comparison), list, rule(expr), rule(comp_op))
DEF_RULE_NC(comp_op, or(9), tok(OP_LESS), tok(OP_MORE), tok(OP_DBL_EQUAL), tok(OP_LESS_EQUAL), tok(OP_MORE_EQUAL), tok(OP_NOT_EQUAL), tok(KW_IN), rule(comp_op_not_in), rule(comp_op_is))
DEF_RULE_NC(comp_op_not_in, and(2), tok(KW_NOT), tok(KW_IN))
DEF_RULE_NC(comp_op_is, and(2), tok(KW_IS), opt_rule(comp_op_is_not))
DEF_RULE_NC(comp_op_is_not, and(1), tok(KW_NOT))
DEF_RULE(star_expr, c(star_expr), and(2), tok(OP_STAR), rule(expr))
DEF_RULE(expr, c(binary_op), list, rule(xor_expr), tok(OP_PIPE))
DEF_RULE(xor_expr, c(binary_op), list, rule(and_expr), tok(OP_CARET))
DEF_RULE(and_expr, c(binary_op), list, rule(shift_expr), tok(OP_AMPERSAND))
DEF_RULE(shift_expr, c(term), list, rule(arith_expr), rule(shift_op))
DEF_RULE_NC(shift_op, or(2), tok(OP_DBL_LESS), tok(OP_DBL_MORE))
DEF_RULE(arith_expr, c(term), list, rule(term), rule(arith_op))
DEF_RULE_NC(arith_op, or(2), tok(OP_PLUS), tok(OP_MINUS))
DEF_RULE(term, c(term), list, rule(factor), rule(term_op))
DEF_RULE_NC(term_op, or(5), tok(OP_STAR), tok(OP_AT), tok(OP_SLASH), tok(OP_PERCENT), tok(OP_DBL_SLASH))
DEF_RULE_NC(factor, or(2), rule(factor_2), rule(power))
DEF_RULE(factor_2, c(factor_2), and_ident(2), rule(factor_op), rule(factor))
DEF_RULE_NC(factor_op, or(3), tok(OP_PLUS), tok(OP_MINUS), tok(OP_TILDE))
DEF_RULE(power, c(power), and_ident(2), rule(atom_expr), opt_rule(power_dbl_star))
#if MICROPY_PY_ASYNC_AWAIT
DEF_RULE_NC(atom_expr, or(2), rule(atom_expr_await), rule(atom_expr_normal))
DEF_RULE(atom_expr_await, c(atom_expr_await), and(3), tok(KW_AWAIT), rule(atom), opt_rule(atom_expr_trailers))
#else
DEF_RULE_NC(atom_expr, or(1), rule(atom_expr_normal))
#endif
DEF_RULE(atom_expr_normal, c(atom_expr_normal), and_ident(2), rule(atom), opt_rule(atom_expr_trailers))
DEF_RULE_NC(atom_expr_trailers, one_or_more, rule(trailer))
DEF_RULE_NC(power_dbl_star, and_ident(2), tok(OP_DBL_STAR), rule(factor))

// atom: '(' [yield_expr|testlist_comp] ')' | '[' [testlist_comp] ']' | '{' [dictorsetmaker] '}' | NAME | NUMBER | STRING+ | '...' | 'None' | 'True' | 'False'
// testlist_comp: (test|star_expr) ( comp_for | (',' (test|star_expr))* [','] )
// trailer: '(' [arglist] ')' | '[' subscriptlist ']' | '.' NAME

DEF_RULE_NC(atom, or(12), tok(NAME), tok(INTEGER), tok(FLOAT_OR_IMAG), tok(STRING), tok(BYTES), tok(ELLIPSIS), tok(KW_NONE), tok(KW_TRUE), tok(KW_FALSE), rule(atom_paren), rule(atom_bracket), rule(atom_brace))
DEF_RULE(atom_paren, c(atom_paren), and(3), tok(DEL_PAREN_OPEN), opt_rule(atom_2b), tok(DEL_PAREN_CLOSE))
DEF_RULE_NC(atom_2b, or(2), rule(yield_expr), rule(testlist_comp))
DEF_RULE(atom_bracket, c(atom_bracket), and(3), tok(DEL_BRACKET_OPEN), opt_rule(testlist_comp), tok(DEL_BRACKET_CLOSE))
DEF_RULE(atom_brace, c(atom_brace), and(3), tok(DEL_BRACE_OPEN), opt_rule(dictorsetmaker), tok(DEL_BRACE_CLOSE))
DEF_RULE_NC(testlist_comp, and_ident(2), rule(testlist_comp_2), opt_rule(testlist_comp_3))
DEF_RULE_NC(testlist_comp_2, or(2), rule(star_expr), rule(namedexpr_test))
DEF_RULE_NC(testlist_comp_3, or(2), rule(comp_for), rule(testlist_comp_3b))
DEF_RULE_NC(testlist_comp_3b, and_ident(2), tok(DEL_COMMA), opt_rule(testlist_comp_3c))
DEF_RULE_NC(testlist_comp_3c, list_with_end, rule(testlist_comp_2), tok(DEL_COMMA))
DEF_RULE_NC(trailer, or(3), rule(trailer_paren), rule(trailer_bracket), rule(trailer_period))
DEF_RULE(trailer_paren, c(trailer_paren), and(3), tok(DEL_PAREN_OPEN), opt_rule(arglist), tok(DEL_PAREN_CLOSE))
DEF_RULE(trailer_bracket, c(trailer_bracket), and(3), tok(DEL_BRACKET_OPEN), rule(subscriptlist), tok(DEL_BRACKET_CLOSE))
DEF_RULE(trailer_period, c(trailer_period), and(2), tok(DEL_PERIOD), tok(NAME))

// subscriptlist: subscript (',' subscript)* [',']
// subscript: test | [test] ':' [test] [sliceop]
// sliceop: ':' [test]

#if MICROPY_PY_BUILTINS_SLICE
DEF_RULE(subscriptlist, c(generic_tuple), list_with_end, rule(subscript), tok(DEL_COMMA))
DEF_RULE_NC(subscript, or(2), rule(subscript_3), rule(subscript_2))
DEF_RULE(subscript_2, c(subscript), and_ident(2), rule(test), opt_rule(subscript_3))
DEF_RULE(subscript_3, c(subscript), and(2), tok(DEL_COLON), opt_rule(subscript_3b))
DEF_RULE_NC(subscript_3b, or(2), rule(subscript_3c), rule(subscript_3d))
DEF_RULE_NC(subscript_3c, and(2), tok(DEL_COLON), opt_rule(test))
DEF_RULE_NC(subscript_3d, and_ident(2), rule(test), opt_rule(sliceop))
DEF_RULE_NC(sliceop, and(2), tok(DEL_COLON), opt_rule(test))
#else
DEF_RULE(subscriptlist, c(generic_tuple), list_with_end, rule(test), tok(DEL_COMMA))
#endif

// exprlist: (expr|star_expr) (',' (expr|star_expr))* [',']
// testlist: test (',' test)* [',']
// dictorsetmaker: (test ':' test (comp_for | (',' test ':' test)* [','])) | (test (comp_for | (',' test)* [',']))

DEF_RULE_NC(exprlist, list_with_end, rule(exprlist_2), tok(DEL_COMMA))
DEF_RULE_NC(exprlist_2, or(2), rule(star_expr), rule(expr))
DEF_RULE(testlist, c(generic_tuple), list_with_end, rule(test), tok(DEL_COMMA))
// TODO dictorsetmaker lets through more than is allowed
DEF_RULE_NC(dictorsetmaker, and_ident(2), rule(dictorsetmaker_item), opt_rule(dictorsetmaker_tail))
#if MICROPY_PY_BUILTINS_SET
DEF_RULE(dictorsetmaker_item, c(dictorsetmaker_item), and_ident(2), rule(test), opt_rule(generic_colon_test))
#else
DEF_RULE(dictorsetmaker_item, c(dictorsetmaker_item), and(3), rule(test), tok(DEL_COLON), rule(test))
#endif
DEF_RULE_NC(dictorsetmaker_tail, or(2), rule(comp_for), rule(dictorsetmaker_list))
DEF_RULE_NC(dictorsetmaker_list, and(2), tok(DEL_COMMA), opt_rule(dictorsetmaker_list2))
DEF_RULE_NC(dictorsetmaker_list2, list_with_end, rule(dictorsetmaker_item), tok(DEL_COMMA))

// classdef: 'class' NAME ['(' [arglist] ')'] ':' suite

DEF_RULE(classdef, c(classdef), and_blank(5), tok(KW_CLASS), tok(NAME), opt_rule(classdef_2), tok(DEL_COLON), rule(suite))
DEF_RULE_NC(classdef_2, and_ident(3), tok(DEL_PAREN_OPEN), opt_rule(arglist), tok(DEL_PAREN_CLOSE))

// arglist: (argument ',')* (argument [','] | '*' test (',' argument)* [',' '**' test] | '**' test)

// TODO arglist lets through more than is allowed, compiler needs to do further verification
DEF_RULE_NC(arglist, list_with_end, rule(arglist_2), tok(DEL_COMMA))
DEF_RULE_NC(arglist_2, or(3), rule(arglist_star), rule(arglist_dbl_star), rule(argument))
DEF_RULE_NC(arglist_star, and(2), tok(OP_STAR), rule(test))
DEF_RULE_NC(arglist_dbl_star, and(2), tok(OP_DBL_STAR), rule(test))

// # The reason that keywords are test nodes instead of NAME is that using NAME
// # results in an ambiguity. ast.c makes sure it's a NAME.
// argument: test [comp_for] | test '=' test  # Really [keyword '='] test
// comp_iter: comp_for | comp_if
// comp_for: 'for' exprlist 'in' or_test [comp_iter]
// comp_if: 'if' test_nocond [comp_iter]

DEF_RULE_NC(argument, and_ident(2), rule(test), opt_rule(argument_2))
#if MICROPY_PY_ASSIGN_EXPR
DEF_RULE_NC(argument_2, or(3), rule(comp_for), rule(generic_equal_test), rule(argument_3))
DEF_RULE_NC(argument_3, and(2), tok(OP_ASSIGN), rule(test))
#else
DEF_RULE_NC(argument_2, or(2), rule(comp_for), rule(generic_equal_test))
#endif
DEF_RULE_NC(comp_iter, or(2), rule(comp_for), rule(comp_if))
DEF_RULE_NC(comp_for, and_blank(5), tok(KW_FOR), rule(exprlist), tok(KW_IN), rule(or_test), opt_rule(comp_iter))
DEF_RULE_NC(comp_if, and(3), tok(KW_IF), rule(test_nocond), opt_rule(comp_iter))

// # not used in grammar, but may appear in "node" passed from Parser to Compiler
// encoding_decl: NAME

// yield_expr: 'yield' [yield_arg]
// yield_arg: 'from' test | testlist

DEF_RULE(yield_expr, c(yield_expr), and(2), tok(KW_YIELD), opt_rule(yield_arg))
DEF_RULE_NC(yield_arg, or(2), rule(yield_arg_from), rule(testlist))
DEF_RULE_NC(yield_arg_from, and(2), tok(KW_FROM), rule(test))
