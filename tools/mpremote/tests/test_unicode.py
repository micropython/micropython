#!/usr/bin/env python3
"""
Tests for Unicode and special character handling in mpremote filesystem operations.

This tests the fix for Issue #2: Apostrophe in filenames breaks REPL string parsing.
The fix uses repr() to properly escape all special characters in filenames when
constructing Python code to send to the REPL.

Usage:
    # Run with configured devices
    pytest test_unicode.py -v

    # Configure devices via environment variable
    MPREMOTE_DEVICES=socket://localhost:2218 pytest test_unicode.py -v

    Environment variables:
    MPREMOTE_DEVICES: Comma-separated list of devices to test against
"""

import os
import subprocess
import sys
import tempfile
from pathlib import Path

import pytest
from shared_utils import run_mpremote  # type: ignore


class TestQuotePath:
    """Unit tests for the _quote_path helper function."""

    def test_quote_path_normal_filename(self):
        """Test that normal filenames are quoted correctly."""
        from mpremote.transport import _quote_path

        assert _quote_path("test.txt") == "'test.txt'"

    def test_quote_path_apostrophe(self):
        """Test that apostrophes are properly escaped."""
        from mpremote.transport import _quote_path

        quoted = _quote_path("O'zbek.txt")
        # Should use double quotes or escape the apostrophe
        assert "O'zbek.txt" not in quoted or "\\'" in quoted or quoted.startswith('"')
        # Verify it's valid Python that evaluates back to the original
        assert eval(quoted) == "O'zbek.txt"

    def test_quote_path_double_quotes(self):
        """Test that double quotes are properly escaped."""
        from mpremote.transport import _quote_path

        quoted = _quote_path('file"name.txt')
        assert eval(quoted) == 'file"name.txt'

    def test_quote_path_both_quotes(self):
        """Test filenames with both single and double quotes."""
        from mpremote.transport import _quote_path

        quoted = _quote_path('it\'s"complex".txt')
        assert eval(quoted) == 'it\'s"complex".txt'

    def test_quote_path_backslash(self):
        """Test that backslashes are properly escaped."""
        from mpremote.transport import _quote_path

        quoted = _quote_path("path\\file.txt")
        assert eval(quoted) == "path\\file.txt"

    def test_quote_path_unicode(self):
        """Test Unicode characters in filenames."""
        from mpremote.transport import _quote_path

        test_cases = [
            "Владимир.txt",  # Cyrillic
            "王明.txt",  # CJK
            "😀.txt",  # Emoji
            "Ñoño.txt",  # Spanish
        ]
        for filename in test_cases:
            quoted = _quote_path(filename)
            assert eval(quoted) == filename, f"Failed for {filename}"

    def test_quote_path_newline(self):
        """Test that newlines in filenames are escaped."""
        from mpremote.transport import _quote_path

        quoted = _quote_path("file\nname.txt")
        assert eval(quoted) == "file\nname.txt"
        assert "\n" not in quoted  # Should be escaped as \\n

    def test_quote_path_spaces(self):
        """Test filenames with spaces."""
        from mpremote.transport import _quote_path

        quoted = _quote_path("file with spaces.txt")
        assert eval(quoted) == "file with spaces.txt"


class TestFilesystemApostrophe:
    """Integration tests for filesystem operations with apostrophe in filenames."""

    @pytest.fixture
    def temp_file_with_apostrophe(self, tmp_path):
        """Create a temporary file with an apostrophe in the name."""
        test_file = tmp_path / "O'zbek_test.txt"
        test_file.write_text("Test content for apostrophe filename\n")
        return test_file

    def test_cp_apostrophe_filename(self, device, scenario, temp_file_with_apostrophe):
        """Test copying a file with apostrophe in filename to device."""
        local_path = str(temp_file_with_apostrophe)
        remote_path = f":/{temp_file_with_apostrophe.name}"

        # Copy file to device
        returncode, stdout, stderr = run_mpremote(device, "cp", local_path, remote_path)

        assert returncode == 0, f"cp failed: {stderr}"
        assert "SyntaxError" not in stderr, "Apostrophe caused SyntaxError"

        # Verify file exists on device
        returncode, stdout, stderr = run_mpremote(device, "ls", ":/")
        assert returncode == 0, f"ls failed: {stderr}"
        assert "O'zbek_test.txt" in stdout, "File not found on device"

        # Clean up
        run_mpremote(device, "rm", remote_path, check=False)

    def test_cat_apostrophe_filename(self, device, scenario, temp_file_with_apostrophe):
        """Test reading a file with apostrophe in filename from device."""
        local_path = str(temp_file_with_apostrophe)
        remote_path = f":/{temp_file_with_apostrophe.name}"

        # Copy file to device first
        returncode, stdout, stderr = run_mpremote(device, "cp", local_path, remote_path)
        assert returncode == 0, f"cp failed: {stderr}"

        # Cat the file
        returncode, stdout, stderr = run_mpremote(device, "cat", remote_path)
        assert returncode == 0, f"cat failed: {stderr}"
        assert "Test content" in stdout, "File content not returned"

        # Clean up
        run_mpremote(device, "rm", remote_path, check=False)

    def test_rm_apostrophe_filename(self, device, scenario, temp_file_with_apostrophe):
        """Test removing a file with apostrophe in filename from device."""
        local_path = str(temp_file_with_apostrophe)
        remote_path = f":/{temp_file_with_apostrophe.name}"

        # Copy file to device first
        returncode, stdout, stderr = run_mpremote(device, "cp", local_path, remote_path)
        assert returncode == 0, f"cp failed: {stderr}"

        # Remove the file
        returncode, stdout, stderr = run_mpremote(device, "rm", remote_path)
        assert returncode == 0, f"rm failed: {stderr}"
        assert "SyntaxError" not in stderr, "Apostrophe caused SyntaxError"

        # Verify file is gone
        returncode, stdout, stderr = run_mpremote(device, "ls", ":/")
        assert "O'zbek_test.txt" not in stdout, "File still exists after rm"

    def test_mkdir_apostrophe_dirname(self, device, scenario):
        """Test creating a directory with apostrophe in name."""
        remote_path = ":/test'dir"

        # Clean up any leftover from previous runs
        run_mpremote(device, "rmdir", remote_path, check=False)

        # Create directory
        returncode, stdout, stderr = run_mpremote(device, "mkdir", remote_path)
        assert returncode == 0, f"mkdir failed: {stderr}"
        assert "SyntaxError" not in stderr, "Apostrophe caused SyntaxError"

        # Verify directory exists
        returncode, stdout, stderr = run_mpremote(device, "ls", ":/")
        assert "test'dir" in stdout, "Directory not found on device"

        # Clean up
        run_mpremote(device, "rmdir", remote_path, check=False)


class TestFilesystemUnicode:
    """Integration tests for filesystem operations with Unicode filenames."""

    @pytest.fixture
    def temp_file_unicode(self, tmp_path):
        """Create a temporary file with Unicode in the name."""
        test_file = tmp_path / "Владимир_Петров.txt"
        test_file.write_text("Unicode test content - Кириллица\n", encoding="utf-8")
        return test_file

    def test_cp_unicode_filename(self, device, scenario, temp_file_unicode):
        """Test copying a file with Unicode (Cyrillic) in filename to device."""
        local_path = str(temp_file_unicode)
        remote_path = f":/{temp_file_unicode.name}"

        # Copy file to device
        returncode, stdout, stderr = run_mpremote(device, "cp", local_path, remote_path)
        assert returncode == 0, f"cp failed: {stderr}"

        # Verify file exists on device
        returncode, stdout, stderr = run_mpremote(device, "ls", ":/")
        assert returncode == 0, f"ls failed: {stderr}"
        assert "Владимир_Петров.txt" in stdout, "Unicode file not found on device"

        # Clean up
        run_mpremote(device, "rm", remote_path, check=False)

    def test_cat_unicode_filename(self, device, scenario, temp_file_unicode):
        """Test reading a file with Unicode filename from device."""
        local_path = str(temp_file_unicode)
        remote_path = f":/{temp_file_unicode.name}"

        # Copy file to device first
        returncode, stdout, stderr = run_mpremote(device, "cp", local_path, remote_path)
        assert returncode == 0, f"cp failed: {stderr}"

        # Cat the file
        returncode, stdout, stderr = run_mpremote(device, "cat", remote_path)
        assert returncode == 0, f"cat failed: {stderr}"
        assert "Unicode test content" in stdout, "File content not returned"

        # Clean up
        run_mpremote(device, "rm", remote_path, check=False)


class TestFilesystemSpecialChars:
    """Integration tests for various special characters in filenames."""

    @pytest.mark.parametrize(
        "filename,content",
        [
            ("file with spaces.txt", "spaces test"),
            ("file'apostrophe.txt", "apostrophe test"),
            # Note: equals sign may fail due to argument parser issue (separate bug)
            # ("E=mc2.txt", "equals test"),
        ],
    )
    def test_cp_special_chars(self, device, scenario, tmp_path, filename, content):
        """Test copying files with various special characters."""
        # Create local test file
        test_file = tmp_path / filename
        test_file.write_text(content + "\n")

        local_path = str(test_file)
        remote_path = f":/{filename}"

        # Copy file to device
        returncode, stdout, stderr = run_mpremote(device, "cp", local_path, remote_path)
        assert returncode == 0, f"cp failed for '{filename}': {stderr}"
        assert "SyntaxError" not in stderr, f"Special char in '{filename}' caused SyntaxError"

        # Verify file exists
        returncode, stdout, stderr = run_mpremote(device, "ls", ":/")
        assert filename in stdout, f"File '{filename}' not found on device"

        # Clean up
        run_mpremote(device, "rm", remote_path, check=False)
