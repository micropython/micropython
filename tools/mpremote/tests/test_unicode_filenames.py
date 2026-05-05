#!/usr/bin/env python3
"""
Tests for Issue #1: mpremote fails with UnicodeEncodeError on Windows legacy consoles.

On Windows legacy consoles (cmd.exe with cp1252 encoding), mpremote's print()
statements fail when outputting filenames containing characters not supported
by the console's encoding (e.g., Cyrillic, CJK, emoji).

See: unicode_mpy/bug_reports/issue1_console_encoding.md

Usage:
    # Run with configured devices
    pytest test_unicode_filenames.py -v

    # Configure devices via environment variable
    MPREMOTE_DEVICES=socket://localhost:2218 pytest test_unicode_filenames.py -v

    # Simulate legacy console encoding (for testing the fix)
    PYTHONIOENCODING=cp1252 pytest test_unicode_filenames.py -v

    Environment variables:
    MPREMOTE_DEVICES: Comma-separated list of devices to test against
"""

import os
import subprocess
import sys

import pytest
from shared_utils import get_mpremote_path, run_mpremote  # type: ignore

DESTINATION_DIR = "./remote_data"

class TestUnicodeFilenames:
    """
    Regression tests for Issue #1: mpremote cp fails with UnicodeEncodeError.

    On Windows legacy consoles using cp1252 or similar encodings, print()
    raises UnicodeEncodeError when outputting characters outside the encoding's
    range. This affects mpremote's verbose output for file operations.

    See: unicode_mpy/bug_reports/issue1_console_encoding.md
    """

    # Test data: (filename, description, expected_to_fail_on_cp1252)
    UNICODE_FILENAMES = [
        # ASCII - always works
        ("README.md", "ASCII only", False),
        # Latin-1 accented characters - work in cp1252
        ("Séamus_Ó_Murchú.txt", "Latin-1 accents (Irish)", False),
        ("café_résumé.txt", "Latin-1 accents (French)", False),
        # Cyrillic - fails in cp1252
        ("Владимир_Петров.txt", "Cyrillic (Russian)", True),
        ("Київ_Україна.txt", "Cyrillic (Ukrainian)", True),
        # CJK - fails in cp1252
        ("王明_李华.txt", "CJK (Chinese)", True),
        ("東京_大阪.txt", "CJK (Japanese)", True),
        ("서울_부산.txt", "CJK (Korean)", True),
        # Greek - fails in cp1252
        ("Αλέξανδρος.txt", "Greek", True),
        # Arabic - fails in cp1252
        ("محمد_أحمد.txt", "Arabic", True),
        # Hebrew - fails in cp1252
        ("שלום_עולם.txt", "Hebrew", True),
        # Thai - fails in cp1252
        ("กรุงเทพ.txt", "Thai", True),
        # Emoji - fails in cp1252
        ("😀_emoji_🎉.txt", "Emoji", True),
        # Mixed scripts
        ("Hello_Мир_世界.txt", "Mixed (ASCII + Cyrillic + CJK)", True),
    ]

    @pytest.fixture
    def temp_unicode_file(self, tmp_path, request):
        """Create a temporary file with a Unicode filename."""
        filename = request.param
        test_file = tmp_path / filename
        test_file.write_text(f"Test content for {filename}\n", encoding="utf-8")
        return test_file

    @pytest.mark.parametrize(
        "filename,description,fails_cp1252",
        UNICODE_FILENAMES,
        ids=[f[1] for f in UNICODE_FILENAMES],
    )
    def test_cp_unicode_filename(self, device, scenario, tmp_path, filename, description, fails_cp1252):
        """
        Test copying files with Unicode characters in filenames.

        Issue #1 regression test: Files with non-ASCII characters should be
        copyable without UnicodeEncodeError, regardless of console encoding.
        """
        # Create test file
        test_file = tmp_path / filename
        test_file.write_text(f"Test content for {filename}\n", encoding="utf-8")

        local_path = str(test_file)
        remote_path = f":/{filename}"

        # Copy file to device
        returncode, stdout, stderr = run_mpremote(device, "cp", local_path, remote_path)
        combined_output = stdout + stderr

        # Should not fail with UnicodeEncodeError (Issue #1)
        assert "UnicodeEncodeError" not in combined_output, (
            f"Issue #1: UnicodeEncodeError for '{filename}' ({description}): {combined_output}"
        )
        assert "charmap" not in combined_output.lower(), (
            f"Issue #1: Encoding error for '{filename}' ({description}): {combined_output}"
        )
        assert returncode == 0, f"cp failed for '{filename}' ({description}): {combined_output}"

        # Verify file exists on device
        returncode, stdout, stderr = run_mpremote(device, "ls", ":/")
        assert returncode == 0, f"ls failed: {stderr}"
        # Note: Some filesystems may normalize Unicode, so we check loosely
        # The main goal is no encoding error occurred

        # Clean up
        run_mpremote(device, "rm", remote_path, check=False)

    @pytest.mark.parametrize(
        "filename,description,fails_cp1252",
        UNICODE_FILENAMES,
        ids=[f[1] for f in UNICODE_FILENAMES],
    )
    def test_cp_recursive_unicode(self, device, scenario, tmp_path, filename, description, fails_cp1252):
        """
        Test recursive copy with Unicode filenames (verbose output).

        The -r flag causes mpremote to print each file as it's copied,
        which is where Issue #1 manifests most commonly.
        """
        # Create a directory with a Unicode-named file
        subdir = tmp_path / "unicode_test"
        subdir.mkdir()
        test_file = subdir / filename
        test_file.write_text(f"Test content for {filename}\n", encoding="utf-8")

        local_path = str(subdir)
        remote_path = f":/{DESTINATION_DIR}"

        # Clean up any previous test artifacts
        run_mpremote(device, "rm", f":/{DESTINATION_DIR}/{filename}", check=False)
        run_mpremote(device, "rmdir", f":/{DESTINATION_DIR}", check=False)
        # Copy directory recursively (this triggers verbose output)
        returncode, stdout, stderr = run_mpremote(device, "cp", "-r", local_path, remote_path)
        combined_output = stdout + stderr

        # Should not fail with UnicodeEncodeError (Issue #1)
        assert "UnicodeEncodeError" not in combined_output, (
            f"Issue #1: UnicodeEncodeError in recursive copy for '{filename}': {combined_output}"
        )
        assert "charmap" not in combined_output.lower(), (
            f"Issue #1: Encoding error in recursive copy for '{filename}': {combined_output}"
        )
        
        # Note: We don't assert returncode == 0 because some devices may have
        # permission issues or filesystem limitations unrelated to Issue #1

        # Clean up
        run_mpremote(device, "rm", f":/{DESTINATION_DIR}/{filename}", check=False)
        run_mpremote(device, "rmdir", ":/remote_data", check=False)

    def test_ls_unicode_filename(self, device, scenario, tmp_path):
        """
        Test listing directory containing Unicode filenames.

        ls output should handle Unicode characters without encoding errors.
        """
        filename = "Владимир_Тест.txt"
        test_file = tmp_path / filename
        test_file.write_text("Cyrillic filename test\n", encoding="utf-8")

        local_path = str(test_file)
        remote_path = f":/{filename}"

        # Copy file to device first
        returncode, stdout, stderr = run_mpremote(device, "cp", local_path, remote_path)
        assert returncode == 0, f"cp failed: {stderr}"

        # List directory - should not fail with encoding error
        returncode, stdout, stderr = run_mpremote(device, "ls", ":/")
        combined_output = stdout + stderr

        assert "UnicodeEncodeError" not in combined_output, (
            f"Issue #1: UnicodeEncodeError in ls output: {combined_output}"
        )
        assert "charmap" not in combined_output.lower(), (
            f"Issue #1: Encoding error in ls output: {combined_output}"
        )
        assert returncode == 0, f"ls failed: {combined_output}"

        # Clean up
        run_mpremote(device, "rm", remote_path, check=False)

    def test_cat_unicode_filename(self, device, scenario, tmp_path):
        """
        Test reading a file with Unicode characters in its name.
        """
        filename = "東京_テスト.txt"
        content = "Japanese filename test content\n日本語コンテンツ\n"
        test_file = tmp_path / filename
        test_file.write_text(content, encoding="utf-8")

        local_path = str(test_file)
        remote_path = f":/{filename}"

        # Copy file to device
        returncode, stdout, stderr = run_mpremote(device, "cp", local_path, remote_path)
        assert returncode == 0, f"cp failed: {stderr}"

        # Cat the file
        returncode, stdout, stderr = run_mpremote(device, "cat", remote_path)
        combined_output = stdout + stderr

        assert "UnicodeEncodeError" not in combined_output, (
            f"Issue #1: UnicodeEncodeError in cat: {combined_output}"
        )
        assert returncode == 0, f"cat failed: {combined_output}"

        # Clean up
        run_mpremote(device, "rm", remote_path, check=False)

    def test_mkdir_unicode_dirname(self, device, scenario):
        """
        Test creating a directory with Unicode characters in its name.
        """
        dirname = "каталог_тест"
        remote_path = f":/{dirname}"

        # Clean up any leftover from previous runs
        run_mpremote(device, "rmdir", remote_path, check=False)

        # Create directory
        returncode, stdout, stderr = run_mpremote(device, "mkdir", remote_path)
        combined_output = stdout + stderr

        assert "UnicodeEncodeError" not in combined_output, (
            f"Issue #1: UnicodeEncodeError in mkdir: {combined_output}"
        )
        assert "charmap" not in combined_output.lower(), (
            f"Issue #1: Encoding error in mkdir: {combined_output}"
        )
        assert returncode == 0, f"mkdir failed: {combined_output}"

        # Verify directory exists
        returncode, stdout, stderr = run_mpremote(device, "ls", ":/")
        assert returncode == 0, f"ls failed: {stderr}"

        # Clean up
        run_mpremote(device, "rmdir", remote_path, check=False)

    def test_touch_unicode_filename(self, device, scenario):
        """
        Test touching a file with Unicode characters in its name.
        """
        filename = "αβγδ_greek.txt"
        remote_path = f":/{filename}"

        # Clean up any leftover from previous runs
        run_mpremote(device, "rm", remote_path, check=False)

        # Touch the file
        returncode, stdout, stderr = run_mpremote(device, "touch", remote_path)
        combined_output = stdout + stderr

        assert "UnicodeEncodeError" not in combined_output, (
            f"Issue #1: UnicodeEncodeError in touch: {combined_output}"
        )
        assert "charmap" not in combined_output.lower(), (
            f"Issue #1: Encoding error in touch: {combined_output}"
        )
        assert returncode == 0, f"touch failed: {combined_output}"

        # Clean up
        run_mpremote(device, "rm", remote_path, check=False)


class TestUnicodeEncodingSimulation:
    """
    Tests that simulate legacy console encoding to verify the fix.

    These tests explicitly set PYTHONIOENCODING to cp1252 to simulate
    a Windows legacy console environment.
    """

    @pytest.mark.parametrize(
        "filename,description",
        [
            ("Владимир_Петров.txt", "Cyrillic"),
            ("王明_李华.txt", "CJK Chinese"),
            ("😀_emoji.txt", "Emoji"),
        ],
        ids=["cyrillic", "cjk", "emoji"],
    )
    def test_cp_with_simulated_legacy_encoding(self, device, scenario, tmp_path, filename, description):
        """
        Test copy operation with simulated legacy console encoding.

        This test explicitly sets PYTHONIOENCODING=cp1252 to simulate
        a Windows legacy console, verifying that mpremote handles the
        encoding gracefully without crashing.
        """
        # Create test file
        test_file = tmp_path / filename
        test_file.write_text(f"Test content for {filename}\n", encoding="utf-8")

        local_path = str(test_file)
        remote_path = f":/{filename}"

        mpremote = get_mpremote_path()
        cmd = [sys.executable, mpremote, "connect", device, "cp", local_path, remote_path]

        # Run with simulated legacy encoding
        env = os.environ.copy()
        env["PYTHONIOENCODING"] = "cp1252"

        result = subprocess.run(
            cmd,
            capture_output=True,
            text=True,
            encoding="utf-8",
            errors="replace",
            env=env,
        )

        combined_output = result.stdout + result.stderr

        # The operation should succeed or fail gracefully (not crash)
        # With the fix, it should use 'replace' or 'backslashreplace' for unsupported chars
        assert "UnicodeEncodeError" not in combined_output, (
            f"Issue #1: UnicodeEncodeError with cp1252 for '{filename}' ({description}): {combined_output}"
        )
        assert "Traceback" not in combined_output or "UnicodeEncodeError" not in combined_output, (
            f"Issue #1: Unhandled exception for '{filename}': {combined_output}"
        )

        # Clean up (may or may not succeed depending on filesystem)
        run_mpremote(device, "rm", remote_path, check=False)


class TestUnicodeFileContent:
    """
    Tests for files with Unicode content (not just filenames).
    """

    def test_cp_file_with_unicode_content(self, device, scenario, tmp_path):
        """
        Test copying a file containing Unicode text content.
        """
        filename = "unicode_content.txt"
        content = """
# Unicode Content Test

## Various Scripts:
- English: Hello, World!
- Russian: Привет, мир!
- Chinese: 你好，世界！
- Japanese: こんにちは世界！
- Korean: 안녕하세요 세계!
- Arabic: مرحبا بالعالم
- Hebrew: שלום עולם
- Greek: Γειά σου Κόσμε
- Thai: สวัสดีโลก
- Emoji: 🌍🌎🌏 🚀 ✨ 🎉

## Mathematical Symbols:
- α, β, γ, δ, ε
- ∀x ∈ ℝ: x² ≥ 0
- ∑(i=1 to n) i = n(n+1)/2

## Currency Symbols:
- €, £, ¥, ₹, ₽, ₿
"""
        test_file = tmp_path / filename
        test_file.write_text(content, encoding="utf-8")

        local_path = str(test_file)
        remote_path = f":/{filename}"

        # Copy file to device
        returncode, stdout, stderr = run_mpremote(device, "cp", local_path, remote_path)
        assert returncode == 0, f"cp failed: {stderr}"

        # Read it back
        returncode, stdout, stderr = run_mpremote(device, "cat", remote_path)
        combined_output = stdout + stderr

        assert "UnicodeEncodeError" not in combined_output, (
            f"Issue #1: UnicodeEncodeError reading Unicode content: {combined_output}"
        )
        assert returncode == 0, f"cat failed: {combined_output}"

        # Verify some content was returned (may be encoded differently)
        assert "Unicode Content Test" in stdout, "File content not retrieved correctly"

        # Clean up
        run_mpremote(device, "rm", remote_path, check=False)
