#!/usr/bin/python

import os
import sys
my_path = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.dirname(my_path))

import pytest

import src.hn_check as hn_check

get_left_hand_side_params = [
    ("return x;", "return x;"),
    ("int x = foo();", "int x"),
]
@pytest.mark.parametrize("input, expected", get_left_hand_side_params)
def test_get_left_hand_side(input, expected):
    left_hand_side = hn_check.get_left_hand_side(input)
    assert expected == left_hand_side


strip_qualifiers_params = [
    ("int x", "int x"),
    ("const int x", "int x"),
    ("extern int x", "int x"),
    ("inline int x", "int x"),
    ("register int x", "int x"),
    ("restrict int x", "int x"),
    ("signed int x", "int x"),
    ("static int x", "int x"),
    ("union int x", "int x"),
    ("unsigned int x", "int x"),
    ("volatile int x", "int x"),
    ("int const x", "int x"),
    ("static int const x", "int x"),
]
@pytest.mark.parametrize("input, expected", strip_qualifiers_params)
def test_strip_qualifiers(input, expected):
    unqualified_decl = hn_check.strip_qualifiers(input)
    assert expected == unqualified_decl


strip_pointers_params = [
    ("", ""),
    ("int x", "int x"),
    ("int * x", "int x"),
    ("int x[2]", "int x"),
]
@pytest.mark.parametrize("input, expected", strip_pointers_params)
def test_strip_pointers(input, expected):
    basic_types = hn_check.strip_pointers(input)
    assert expected == basic_types


is_var_decl_params = [
    ("int const n_foo;",     True),
    ("int foo = 12;",        True),
    ("int foo;",             True),
    ("int n_foo;",           True),
    ("int32_t const n_foo;", True),
    ("int32_t foo",          True),
    ("int32_t n_foo;",       True),
    ("uint8_t * topic;",     True),
    ("2int foo;",            False),
    ("case foo;",            False),
    ("int 2foo;",            False),
    ("int foo();",           False),
    ("intfoo;",              False),
    ("return foo;",          False),
    ("typedef int foo_t;",   False),
]
@pytest.mark.parametrize("input, expected", is_var_decl_params)
def test_is_var_decl(input, expected):
    result = hn_check.is_var_decl(input)
    assert expected == result


strip_comment_params = [
    ("/**/",              " "),
    ("int /**/ x",        "int x"),
    ("int /**/*/ x",      "int */ x"),
    ("int /*\n*/ x",      "int x"),
    ("int /*\r\n*/ x",    "int x"),
    ("int /*\r\n * */ x", "int x"),
    ("int /**/ x",        "int x"),
    ("int x /*foo*/\n",   "int x\n"),
    ("int x //foo",       "int x "),
    ("int x /*foo",       "int x "),
]
@pytest.mark.parametrize("input, expected", strip_comment_params)
def test_strip_comment(input, expected):
    result = hn_check.strip_comment(input)
    assert expected == result


delete_line_continuations_params = [
    ("foo \\\n bar",   "foo  bar"),
    ("foo \\\r\n bar", "foo  bar"),
    ("foo \\\r bar",   "foo  bar"),
]
@pytest.mark.parametrize("input, expected", delete_line_continuations_params)
def test_delete_line_continuations(input, expected):
    result = hn_check.delete_line_continuations(input)
    assert expected == result


get_base_type_params = [
    ("int x", "int"),
    ("uint8_t x", "uint8_t"),
]
@pytest.mark.parametrize("input, expected", get_base_type_params)
def test_get_base_type(input, expected):
    result = hn_check.get_base_type(input)
    assert expected == result


get_base_prefix_params = [
    ("char", "c"),
    ("hello_t", "x"),
    ("int16_t", "s"),
    ("int32_t", "l"),
    ("int64_t", "ll"),
    ("int8_t", "c"),
    ("uint16_t", "us"),
    ("uint32_t", "ul"),
    ("uint64_t", "ull"),
    ("uint8_t", "uc"),
]
@pytest.mark.parametrize("input, expected", get_base_prefix_params)
def test_get_base_prefix(input, expected):
    prefix = hn_check.get_base_prefix(input)
    assert expected == prefix


count_indirection_params = [
    ("int x", 0),
    ("int * x", 1),
    ("int ** x", 2),
    ("int x[2]", 0),
    ("int x[2][3]", 0),
    ("int * x[2][3]", 1),
    ("CK_FUNCTION_LIST_PTR", 1),
    ("CK_FUNCTION_LIST_PTR *", 2),
    ("CK_FUNCTION_LIST_PTR_PTR", 2),
]
@pytest.mark.parametrize("input, expected", count_indirection_params)
def test_count_indirection(input, expected):
    result = hn_check.count_indirection(input)
    assert expected == result

# PKCS #11 indirection only counts indirection from _PTR
# in type name, and not due to *
count_pkcs11_indirection_params = [
    ("int x", 0),
    ("int * x", 0),
    ("int ** x", 0),
    ("CK_FUNCTION_LIST_PTR", 1),
    ("CK_FUNCTION_LIST_PTR *", 1),
    ("CK_FUNCTION_LIST_PTR_PTR", 2),
]
@pytest.mark.parametrize("input, expected", count_pkcs11_indirection_params)
def test_count_pkcs11_indirection(input, expected):
    result = hn_check.count_pkcs11_indirection(input)
    assert expected == result

get_prefix_params = [
    ("char * topic;", "pc"),
    ("char * topic;", "pc"),
    ("char * topic[3];", "pc"),
    ("char ** topic;", "ppc"),
    ("char topic[3];", "c"),
    ("char topic[3];", "c"),
    ("unsigned int ulStatus;", "ul"),
]
@pytest.mark.parametrize("input, expected", get_prefix_params)
def test_get_prefix(input, expected):
    prefix = hn_check.get_prefix(input)
    assert expected == prefix


get_identifier_params = [
    ("char * topic;", "topic"),
]
@pytest.mark.parametrize("input, expected", get_identifier_params)
def test_get_identifier(input, expected):
    result = hn_check.get_identifier(input)
    assert expected == result


prefix_is_correct_params = [
    ("    AnEnumType_t * peStatus;", True),
    ("    AnEnumType_t eStatus;", True),
    ("    char * pcTopic;", True),
    ("    char * topic;", False),
    ("    uint8_t * pcTopic;", False),
    ("    unsigned int ulStatus;", True),
    ("    extern char ulStatus;", True), # extern shall not be marked wrong
    ("AnEnumType_t * status;", True), # Globally linkable shouldn't be checked
    ("static AnEnumType_t * status;", False),
]
@pytest.mark.parametrize("input, expected", prefix_is_correct_params)
def test_prefix_is_correct(input, expected):
    result = hn_check.prefix_is_correct(input)
    assert expected == result


get_identifier_prefix_params = [
    ("asVar", ""),
    ("eVar", "e"),
    ("lVar", "l"),
    ("pxVar", "px"),
    ("success", ""),
    ("sVar", "s"),
    ("xVar", "x"),
    ("px123", "px"),
]
@pytest.mark.parametrize("input, expected", get_identifier_prefix_params)
def test_get_identifier_prefix(input, expected):
    result = hn_check.get_identifier_prefix(input)
    assert expected == result


get_base_identifier_params = [
    ("asVar", "asVar"),
    ("lVar", "Var"),
    ("pxVar", "Var"),
    ("success", "success"),
    ("sVar", "Var"),
    ("xVar", "Var"),
]
@pytest.mark.parametrize("input, expected", get_base_identifier_params)
def test_get_base_identifier(input, expected):
    result = hn_check.get_base_identifier(input)
    assert expected == result


find_line_params = [
    ("./test/source_file.c", "asdf", 0),
    ("./test/source_file.c", 'char * pcHello "Hello, World!";', 5),
]
@pytest.mark.parametrize("input_file, input_string, expected_line_number",
    find_line_params)
def test_find_line(input_file, input_string, expected_line_number):
    found_line_number = hn_check.find_line(input_file, input_string)
    assert expected_line_number == found_line_number
