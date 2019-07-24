#!/usr/bin/python

import os
import sys
myPath = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.dirname(myPath))

import pytest

import src.pre_commit as pre_commit
import subprocess

from mock import patch


commit_is_ready_params = [
    (["test/foo.py"], []),
    (["test/clean.c"], []),
    ("", []),
]
@pytest.mark.parametrize("input, expected", commit_is_ready_params)
def test_commit_is_ready(input, expected):
    with patch('subprocess.check_output') as mock_check:
        mock_check.return_value = ""
        with patch('subprocess.call') as mock_call:
            mock_call.return_value = 0
            with patch('src.pre_commit.file_is_checkable') as mock_checkable:
                mock_checkable.return_value = True
                result = pre_commit.commit_is_ready(input)
                assert expected == result


file_exists_params = [
    ("test/clean.c", True),
    ("test/bar.c", False),
]
@pytest.mark.parametrize("input, expected", file_exists_params)
def test_file_exists(input, expected):
    result = pre_commit.file_exists(input)
    assert expected == result


is_source_file_params = [
    ("foo.c", True),
    ("foo.h", True),
    ("foo.txt", False),
]
@pytest.mark.parametrize("input, expected", is_source_file_params)
def test_is_source_file(input, expected):
    result = pre_commit.is_source_file(input)
    assert expected == result


is_checked_file_pattern_params = [
    ("lib/some_lib/some_file.c", True), # lib directory is checked
    ("lib/some_lib/aws_header.h", True), # check aws headers
    ("lib/some_lib/task.h", False), # Ignore kenrel headers
    ("demos/some_board/some_file.c", False), # demo board directories are not checked
]
@pytest.mark.parametrize("input, expected", is_checked_file_pattern_params)
def test_is_checked_file_pattern(input, expected):
    result = pre_commit.is_checked_file_pattern(input)
    assert expected == result


is_ignored_file_pattern_params = [
    ("lib/third_party/some_lib/some_file.c", True),
    ("lib/FreeRTOS/some_file.c", True),
    ("lib/FreeRTOS-Plus-TCP/some_file.c", True),
    ("lib/include/some_file.h", False),
]
@pytest.mark.parametrize("input, expected", is_ignored_file_pattern_params)
def test_is_ignored_file_pattern(input, expected):
    result = pre_commit.is_ignored_file_pattern(input)
    assert expected == result


file_is_checkable_params = [
    ("lib/some_lib/some_file.c", True), # File is in directory that is checked
    ("lib/some_lib/some_file.py", False), # File is not c source file
    ("foo/some_file.c", False), # File is not in checked directory
    ("lib/third_party/some_file.c", False), # File is in ignored sub-directory
]
@pytest.mark.parametrize("input, expected", file_is_checkable_params)
def test_file_is_checkable(input, expected):
    result = pre_commit.file_is_checkable(input)
    assert expected == result


def test_check_whitespace_calls_git():
    with patch('subprocess.call') as mock:
        mock.return_value = 0
        assert [] == pre_commit.check_whitespace(["foo.c"])
        mock.assert_called_once_with(
            "git diff-index --check --cached HEAD", shell=True)


def test_check_whitespace_returns_false_on_error():
    with patch('subprocess.call') as mock:
        # Error code returned from git
        mock.return_value = 2
        assert ['whitespace'] == pre_commit.check_whitespace(["foo.c"])


check_uncrustify_params = [
    (0, []),
    (1, ["foo.c"]),
    (-1, ["foo.c"]),
    (99, ["foo.c"]),
]
@pytest.mark.parametrize("return_code, expected", check_uncrustify_params)
def test_check_uncrustify(return_code, expected):
    with patch('subprocess.call') as mock:
        mock.return_value = return_code
        assert expected == pre_commit.check_uncrustify(["foo.c"])
        mock.assert_called_once_with(
            "uncrustify --check -q -c .uncrustify.cfg foo.c", shell=True)


get_modified_files_params = [
    ("foo.c\nbar.h\nbaz.py", ["foo.c", "bar.h", "baz.py"]),
    (b"foo.c\nbar.h\nbaz.py", ["foo.c", "bar.h", "baz.py"]),
]
@pytest.mark.parametrize("git_result, expected", get_modified_files_params)
def test_get_modified_files(git_result, expected):
    with patch('subprocess.check_output') as mock:
        mock.return_value = git_result
        result = pre_commit.get_modified_files()
    assert expected == result


check_hungarian_notation_params = [
    (0, []),
    (1, ["foo.c", "bar.h"]),
    (-1, ["foo.c", "bar.h"]),
    (256, ["foo.c", "bar.h"]),
]
@pytest.mark.parametrize("return_code, expected", check_hungarian_notation_params)
def test_check_hungarian_notation(return_code, expected):
    with patch('subprocess.call') as mock:
        mock.return_value = return_code
        result = pre_commit.check_hungarian_notation(["foo.c", "bar.h"])
    assert expected == result


patch_uncrustify_params = [
    ("test/unformatted.c", '"a/test/unformatted.c"'),
    ("test/clean.c", ''),
]
@pytest.mark.parametrize("input, expected", patch_uncrustify_params)
def test_patch_uncrustify(input, expected):
    result = pre_commit.patch_uncrustify(input)
    assert expected in result
