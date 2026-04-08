#!/usr/bin/env python3
"""
Tests for Issue #3: mpremote cp fails with equals sign (=) in filename.

The mpremote argument parser splits on '=' to support key-value style arguments,
which incorrectly affects filenames that legitimately contain the '=' character.

See: unicode_mpy/bug_reports/issue3_equals_sign_in_filenames.md

Usage:
    # Run with configured devices
    pytest test_equals_sign.py -v

    # Configure devices via environment variable
    MPREMOTE_DEVICES=socket://localhost:2218 pytest test_equals_sign.py -v

    Environment variables:
    MPREMOTE_DEVICES: Comma-separated list of devices to test against
"""

import pytest
from shared_utils import run_mpremote  # type: ignore


class TestEqualsSignFilenames:
    """
    Regression tests for Issue #3: mpremote cp fails with equals sign (=) in filename.

    The mpremote argument parser splits on '=' to support key-value style arguments,
    which incorrectly affects filenames that legitimately contain the '=' character.

    See: unicode_mpy/bug_reports/issue3_equals_sign_in_filenames.md
    """

    @pytest.fixture
    def temp_file_with_equals(self, tmp_path):
        """Create a temporary file with an equals sign in the name."""
        test_file = tmp_path / "H2O_E=mc2.txt"
        test_file.write_text("Test content for equals sign filename\n")
        return test_file

    def test_cp_equals_sign_filename(self, device, scenario, temp_file_with_equals):
        """
        Test copying a file with equals sign in filename to device.

        Issue #3 regression test: The filename 'H2O_E=mc2.txt' should not be
        parsed as key='H2O_E' value='mc2.txt'.
        """
        local_path = str(temp_file_with_equals)
        remote_path = f":/{temp_file_with_equals.name}"

        # Copy file to device
        returncode, stdout, stderr = run_mpremote(device, "cp", local_path, remote_path)
        combined_output = stdout + stderr

        # Should not fail with "unexpected argument" error (Issue #3)
        assert "unexpected argument" not in combined_output, (
            f"Issue #3: Equals sign caused argument parsing error: {combined_output}"
        )
        assert "signature is" not in combined_output, (
            f"Issue #3: Argument parser error: {combined_output}"
        )
        assert returncode == 0, f"cp failed: {combined_output}"

        # Verify file exists on device with correct name
        returncode, stdout, stderr = run_mpremote(device, "ls", ":/")
        assert returncode == 0, f"ls failed: {stderr}"
        assert "H2O_E=mc2.txt" in stdout, "File with equals sign not found on device"

        # Clean up
        run_mpremote(device, "rm", remote_path, check=False)

    def test_cat_equals_sign_filename(self, device, scenario, temp_file_with_equals):
        """Test reading a file with equals sign in filename from device."""
        local_path = str(temp_file_with_equals)
        remote_path = f":/{temp_file_with_equals.name}"

        # Copy file to device first
        returncode, stdout, stderr = run_mpremote(device, "cp", local_path, remote_path)
        combined_output = stdout + stderr
        assert "unexpected argument" not in combined_output, (
            f"Issue #3: Equals sign in cp caused argument parsing error: {combined_output}"
        )
        assert returncode == 0, f"cp failed: {combined_output}"

        # Cat the file
        returncode, stdout, stderr = run_mpremote(device, "cat", remote_path)
        combined_output = stdout + stderr
        assert "unexpected argument" not in combined_output, (
            f"Issue #3: Equals sign in cat caused argument parsing error: {combined_output}"
        )
        assert returncode == 0, f"cat failed: {combined_output}"
        assert "Test content" in stdout, "File content not returned"

        # Clean up
        run_mpremote(device, "rm", remote_path, check=False)

    def test_rm_equals_sign_filename(self, device, scenario, temp_file_with_equals):
        """Test removing a file with equals sign in filename from device."""
        local_path = str(temp_file_with_equals)
        remote_path = f":/{temp_file_with_equals.name}"

        # Copy file to device first
        returncode, stdout, stderr = run_mpremote(device, "cp", local_path, remote_path)
        combined_output = stdout + stderr
        assert "unexpected argument" not in combined_output, (
            f"Issue #3: Equals sign in cp caused argument parsing error: {combined_output}"
        )
        assert returncode == 0, f"cp failed: {combined_output}"

        # Remove the file
        returncode, stdout, stderr = run_mpremote(device, "rm", remote_path)
        combined_output = stdout + stderr
        assert "unexpected argument" not in combined_output, (
            f"Issue #3: Equals sign in rm caused argument parsing error: {combined_output}"
        )
        assert returncode == 0, f"rm failed: {combined_output}"

        # Verify file is gone
        returncode, stdout, stderr = run_mpremote(device, "ls", ":/")
        assert "H2O_E=mc2.txt" not in stdout, "File still exists after rm"

    @pytest.mark.parametrize(
        "filename",
        [
            "E=mc2.txt",  # Simple equals
            "a=b.log",  # Short key=value pattern
            "key=value.json",  # Common config pattern
            "base64==data.bin",  # Double equals (like Python comparison)
            "x=1+y=2.txt",  # Multiple equals
            "path/to/file=1.txt",  # Equals in path component
        ],
    )
    def test_cp_various_equals_patterns(self, device, scenario, tmp_path, filename):
        """
        Test copying files with various equals sign patterns.

        All these filenames are valid on major operating systems and should
        be handled correctly by mpremote.
        """
        # Handle subdirectories in filename
        if "/" in filename:
            subdir = tmp_path / "path" / "to"
            subdir.mkdir(parents=True, exist_ok=True)
            test_file = tmp_path / filename
        else:
            test_file = tmp_path / filename

        test_file.write_text(f"Content for {filename}\n")

        local_path = str(test_file)
        # Use basename for remote to avoid path issues
        remote_name = filename.replace("/", "_")
        remote_path = f":/{remote_name}"

        # Copy file to device
        returncode, stdout, stderr = run_mpremote(device, "cp", local_path, remote_path)
        combined_output = stdout + stderr

        assert "unexpected argument" not in combined_output, (
            f"Issue #3: Equals sign in '{filename}' caused argument parsing error: {combined_output}"
        )
        assert "signature is" not in combined_output, (
            f"Issue #3: Argument parser error for '{filename}': {combined_output}"
        )
        assert returncode == 0, f"cp failed for '{filename}': {combined_output}"

        # Clean up
        run_mpremote(device, "rm", remote_path, check=False)

    def test_mkdir_equals_sign_dirname(self, device, scenario):
        """Test creating a directory with equals sign in name."""
        remote_path = ":/test=dir"

        # Clean up any leftover from previous runs
        run_mpremote(device, "rmdir", remote_path, check=False)

        # Create directory
        returncode, stdout, stderr = run_mpremote(device, "mkdir", remote_path)
        combined_output = stdout + stderr
        assert "unexpected argument" not in combined_output, (
            f"Issue #3: Equals sign caused argument parsing error: {combined_output}"
        )
        assert "signature is" not in combined_output, (
            f"Issue #3: Argument parser error: {combined_output}"
        )
        assert returncode == 0, f"mkdir failed: {combined_output}"

        # Verify directory exists
        returncode, stdout, stderr = run_mpremote(device, "ls", ":/")
        assert "test=dir" in stdout, "Directory with equals sign not found on device"

        # Clean up
        run_mpremote(device, "rmdir", remote_path, check=False)

    def test_touch_equals_sign_filename(self, device, scenario):
        """Test creating/touching a file with equals sign in name."""
        remote_path = ":/touched=file.txt"

        # Clean up any leftover from previous runs
        run_mpremote(device, "rm", remote_path, check=False)

        # Touch the file
        returncode, stdout, stderr = run_mpremote(device, "touch", remote_path)
        combined_output = stdout + stderr
        assert "unexpected argument" not in combined_output, (
            f"Issue #3: Equals sign caused argument parsing error: {combined_output}"
        )
        assert "signature is" not in combined_output, (
            f"Issue #3: Argument parser error: {combined_output}"
        )
        assert returncode == 0, f"touch failed: {combined_output}"

        # Verify file exists
        returncode, stdout, stderr = run_mpremote(device, "ls", ":/")
        assert "touched=file.txt" in stdout, "Touched file with equals sign not found"

        # Clean up
        run_mpremote(device, "rm", remote_path, check=False)
