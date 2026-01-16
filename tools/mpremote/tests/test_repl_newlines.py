#!/usr/bin/env python3
"""
Test for REPL newline handling (CR/LF).

This test verifies that the interactive REPL properly displays newlines,
specifically testing the fix for the "staircase" output bug where lines
would display without carriage returns.

The test uses pexpect with a PTY (pseudo-terminal) to simulate a real
terminal environment, which is necessary because the REPL's console
handling requires termios.

Usage:
    # Run with all configured devices
    pytest test_repl_newlines.py -v

    # Configure multiple devices via environment variable (comma-separated)
    MPREMOTE_DEVICES=socket://localhost:2218,rfc2217://localhost:2217,/dev/ttyACM0 pytest test_repl_newlines.py -v

Requirements:
    - pytest
    - pexpect (for PTY-based terminal simulation)
    - A running mpremote bridge or MicroPython device

Environment variables:
    MPREMOTE_DEVICES: Comma-separated list of devices to test against
                      If not set, device-specific tests are skipped.
"""

import os
import sys
import time

import pytest

# Import scenario helper from conftest
from conftest import get_scenario_from_device

# Check for required dependencies
try:
    import pexpect
except ImportError:
    pexpect = None


def is_wsl():
    """Detect if running under Windows Subsystem for Linux (WSL)."""
    try:
        with open("/proc/version", "r") as f:
            return "microsoft" in f.read().lower()
    except (FileNotFoundError, PermissionError):
        return False


def get_devices():
    """
    Get list of devices to test against from MPREMOTE_DEVICES env var.

    Returns list of (device_url, scenario_name) tuples.
    """
    devices_str = os.environ.get("MPREMOTE_DEVICES", "")
    if not devices_str:
        return []

    devices = [d.strip() for d in devices_str.split(",") if d.strip()]
    return [(device, get_scenario_from_device(device)) for device in devices]


def get_mpremote_path():
    """Get the path to mpremote.py."""
    test_dir = os.path.dirname(os.path.abspath(__file__))
    return os.path.join(test_dir, "..", "mpremote.py")


def build_subprocess_env(test_nodeid=None):
    """
    Build environment dict for subprocess with coverage support.

    This ensures subprocesses:
    1. Can find sitecustomize.py (via PYTHONPATH)
    2. Know the coverage config file (via COVERAGE_PROCESS_START - inherited)
    3. Know which test context to use (via MPREMOTE_COVERAGE_CONTEXT)

    Args:
        test_nodeid: The pytest node ID (already has scenario prefix from conftest)

    Returns:
        Environment dict for subprocess
    """
    env = os.environ.copy()

    # Ensure mpremote directory is in PYTHONPATH for sitecustomize.py
    mpremote_path = get_mpremote_path()
    mpremote_dir = os.path.dirname(mpremote_path)

    if "PYTHONPATH" in env:
        if mpremote_dir not in env["PYTHONPATH"]:
            env["PYTHONPATH"] = f"{mpremote_dir}:{env['PYTHONPATH']}"
    else:
        env["PYTHONPATH"] = mpremote_dir

    # Set coverage context for subprocess (nodeid from conftest already has [scenario] prefix)
    if test_nodeid:
        env["MPREMOTE_COVERAGE_CONTEXT"] = test_nodeid

    return env


# Skip conditions
requires_pexpect = pytest.mark.skipif(pexpect is None, reason="pexpect is required for REPL tests")
requires_unix = pytest.mark.skipif(
    sys.platform == "win32", reason="PTY tests only work on Unix-like systems"
)

# Get devices for parametrization at module load time
_DEVICES = get_devices()


# Parametrize decorator for device tests
device_params = pytest.mark.parametrize(
    "device,scenario",
    _DEVICES
    if _DEVICES
    else [pytest.param("", "no-device", marks=pytest.mark.skip(reason="No devices configured"))],
    ids=[d[1] for d in _DEVICES] if _DEVICES else ["no-device"],
)


@requires_pexpect
@requires_unix
class TestReplNewlines:
    """Tests for REPL newline handling in interactive mode."""

    @device_params
    def test_repl_newline_no_staircase(self, device, scenario, request):
        """
        Test that print() output has proper newlines (no staircase effect).

        The bug this tests for:
        - Without fix: each line starts where the previous ended (staircase)
        - With fix: each line starts at column 0

        Example of bug (staircase):
            >>> print("a"); print("b"); print("c")
            a
             b
              c

        Expected (correct):
            >>> print("a"); print("b"); print("c")
            a
            b
            c
        """
        mpremote = get_mpremote_path()

        # Determine connection type for timing adjustments
        is_socket = device.startswith("socket://")
        is_rfc2217 = device.startswith("rfc2217://")
        is_physical_device = not (is_socket or is_rfc2217)

        # WSL2 USB passthrough adds significant latency for device visibility
        wsl_delay = is_wsl() and is_physical_device

        # Timeouts: physical devices need more time, especially on WSL
        if wsl_delay:
            connect_timeout = 20
            prompt_timeout = 15
        elif is_physical_device:
            connect_timeout = 15
            prompt_timeout = 10
        else:  # socket or rfc2217
            connect_timeout = 10
            prompt_timeout = 5

        # Build environment for subprocess with coverage context
        env = build_subprocess_env(request.node.nodeid)

        # Spawn mpremote in a PTY
        child = pexpect.spawn(
            sys.executable,
            [mpremote, "connect", device],
            encoding="utf-8",
            timeout=connect_timeout,
            env=env,
        )
        # Use CRLF line endings for serial devices - MicroPython REPL expects CR
        child.linesep = "\r\n"

        try:
            # Wait for connection message
            index = child.expect(
                ["Connected to MicroPython", pexpect.TIMEOUT, pexpect.EOF],
                timeout=connect_timeout,
            )
            if index != 0:
                pytest.skip(f"Could not connect to {device}: {child.before!r}")

            # All non-socket connections need stabilization
            # Physical devices may be in unknown state, RFC2217 has protocol overhead
            if not is_socket:
                time.sleep(0.5)  # Brief delay for connection to stabilize
                child.sendcontrol("c")  # Interrupt any running code
                time.sleep(0.3)

            # Send Enter to trigger prompt
            child.sendline("")

            # Wait for >>> prompt
            index = child.expect([">>>", pexpect.TIMEOUT, pexpect.EOF], timeout=prompt_timeout)
            if index != 0:
                pytest.skip(f"No prompt from {device}: {child.before!r}")

            # Brief delay to let REPL settle before sending command
            time.sleep(0.3)

            # Now send our test command
            child.sendline('print("LINE_A"); print("LINE_B"); print("LINE_C")')

            # Wait for the output lines to appear (don't just wait for prompt
            # as that might already match from previous output)
            time.sleep(0.5)  # Give time for command to execute

            # Wait for LINE_C (the last output line) to appear
            index = child.expect(["LINE_C", pexpect.TIMEOUT, pexpect.EOF], timeout=prompt_timeout)
            if index != 0:
                pytest.fail(f"Did not get LINE_C in output: before={child.before!r}")

            # Collect output up through LINE_C
            output_part1 = child.before if child.before else ""

            # Now wait for the final prompt
            index = child.expect([">>>", pexpect.TIMEOUT, pexpect.EOF], timeout=prompt_timeout)
            if index != 0:
                pytest.fail(f"Did not get prompt after command: before={child.before!r}")

            # Combine all output
            output = output_part1 + "LINE_C" + (child.before if child.before else "")

            # Verify all lines are present
            assert "LINE_A" in output, f"LINE_A not found in: {output!r}"
            assert "LINE_B" in output, f"LINE_B not found in: {output!r}"
            assert "LINE_C" in output, f"LINE_C not found in: {output!r}"

            # Check for staircase effect
            # In buggy output, LINE_B and LINE_C would have leading spaces
            # The output looks like:
            #   ' print("LINE_A"); print("LINE_B"); print("LINE_C")\r\r\n'  <- echo
            #   'LINE_A\r\n'                                                 <- output
            #   'LINE_B\r\n'                                                 <- output
            #   'LINE_C\r\n'                                                 <- output
            lines = output.split("\n")

            # Find output lines (not the echoed command line)
            output_lines = []
            for line in lines:
                line_clean = line.strip()
                # Skip the echoed command (contains print(" with quotes)
                if 'print("' in line:
                    continue
                # Skip empty lines
                if not line_clean:
                    continue
                output_lines.append(line)

            # Check that output lines start at column 0 (no staircase)
            for line in output_lines:
                for marker in ["LINE_A", "LINE_B", "LINE_C"]:
                    if marker in line:
                        # The marker should be at the start of the line (column 0)
                        pos = line.find(marker)
                        leading = line[:pos]
                        # Only whitespace before the marker indicates staircase
                        # (In correct output, marker is at position 0 or after \r)
                        leading_stripped = leading.replace("\r", "")
                        assert len(leading_stripped) < 2, (
                            f"Staircase effect detected! {marker} has leading chars: {leading!r}\n"
                            f"Full line: {line!r}\n"
                            f"Full output: {output!r}"
                        )

        finally:
            # Exit the REPL gracefully to allow coverage to save
            # First try Ctrl-D (EOF) which is a clean exit
            child.sendcontrol("d")
            time.sleep(0.2)
            # Then Ctrl-] as backup
            child.sendcontrol("]")
            # Wait for process to terminate cleanly
            child.expect([pexpect.EOF, pexpect.TIMEOUT], timeout=2)
            child.close()


# =============================================================================
# Unit Tests - Direct testing of console.py terminal settings
# =============================================================================


class TestConsolePosixTerminalSettings:
    """
    Unit tests for ConsolePosix terminal configuration.

    These tests verify that the terminal flags are set correctly to handle
    newline translation (OPOST/ONLCR), preventing the "staircase" effect.
    """

    @pytest.mark.skipif(sys.platform == "win32", reason="POSIX-only test")
    def test_console_enter_preserves_output_processing(self):
        """
        Test that ConsolePosix.enter() enables OPOST and ONLCR flags.

        OPOST: Enable output post-processing
        ONLCR: Translate NL to CR-NL on output

        Without these flags, output containing only LF (\\n) will cause the
        "staircase" effect where each line starts where the previous ended
        instead of at column 0.
        """
        import pty
        import termios

        # Create a PTY to test with (we need a real terminal)
        master_fd, slave_fd = pty.openpty()

        try:
            # Import the console module
            # We need to patch stdin to use our PTY
            original_stdin = sys.stdin

            # Create a file object from the slave fd
            slave_file = os.fdopen(slave_fd, "r", closefd=False)

            try:
                sys.stdin = slave_file

                # Now import and create the console
                # We need to reload to pick up the new stdin
                from mpremote.console import ConsolePosix

                console = ConsolePosix()

                # Get the original terminal attributes
                orig_attr = termios.tcgetattr(slave_fd)

                # Call enter() to set raw mode
                console.enter()

                # Get the modified terminal attributes
                new_attr = termios.tcgetattr(slave_fd)

                # Check that OPOST is enabled (output post-processing)
                opost_enabled = bool(new_attr[1] & termios.OPOST)
                assert opost_enabled, (
                    "OPOST flag should be enabled for output post-processing. "
                    f"oflag = {new_attr[1]:#o}, OPOST = {termios.OPOST:#o}"
                )

                # Check that ONLCR is enabled (NL -> CR-NL translation)
                onlcr_enabled = bool(new_attr[1] & termios.ONLCR)
                assert onlcr_enabled, (
                    "ONLCR flag should be enabled for NL->CR-NL translation. "
                    f"oflag = {new_attr[1]:#o}, ONLCR = {termios.ONLCR:#o}"
                )

                # Restore terminal settings
                console.exit()

            finally:
                sys.stdin = original_stdin
                slave_file.close()

        finally:
            os.close(master_fd)
            os.close(slave_fd)

    @pytest.mark.skipif(sys.platform == "win32", reason="POSIX-only test")
    def test_console_exit_restores_original_settings(self):
        """
        Test that ConsolePosix.exit() restores the original terminal settings.
        """
        import pty
        import termios

        master_fd, slave_fd = pty.openpty()

        try:
            original_stdin = sys.stdin
            slave_file = os.fdopen(slave_fd, "r", closefd=False)

            try:
                sys.stdin = slave_file

                from mpremote.console import ConsolePosix

                console = ConsolePosix()

                # Get original attributes
                orig_attr = termios.tcgetattr(slave_fd)

                # Enter and exit
                console.enter()
                console.exit()

                # Get restored attributes
                restored_attr = termios.tcgetattr(slave_fd)

                # The important flags should be restored
                # Compare the main flag groups
                assert orig_attr[0] == restored_attr[0], "iflag not restored"
                assert orig_attr[1] == restored_attr[1], "oflag not restored"
                assert orig_attr[3] == restored_attr[3], "lflag not restored"

            finally:
                sys.stdin = original_stdin
                slave_file.close()

        finally:
            os.close(master_fd)
            os.close(slave_fd)

    @pytest.mark.skipif(sys.platform == "win32", reason="POSIX-only test")
    def test_oflag_includes_required_bits(self):
        """
        Direct test that the oflag value in enter() includes OPOST | ONLCR.

        This is a code inspection test that verifies the fix is in place.
        """
        import termios

        # The expected oflag value
        expected_oflag = termios.OPOST | termios.ONLCR

        # Import and check the source
        import inspect

        from mpremote.console import ConsolePosix

        source = inspect.getsource(ConsolePosix.enter)

        # Check that the source contains the fix
        # It should have OPOST and ONLCR, not just "= 0"
        assert "OPOST" in source, (
            "ConsolePosix.enter() should set OPOST flag. "
            "Without it, output post-processing is disabled."
        )
        assert "ONLCR" in source, (
            "ConsolePosix.enter() should set ONLCR flag. "
            "Without it, NL->CR-NL translation is disabled, causing staircase effect."
        )

        # Make sure it's not the buggy "attr[1] = 0" pattern
        assert "attr[1] = 0" not in source, (
            "ConsolePosix.enter() should NOT set attr[1] = 0. "
            "This disables all output processing and causes the staircase bug."
        )

    @pytest.mark.skipif(sys.platform == "win32", reason="POSIX-only test")
    def test_readchar_returns_none_when_no_input(self):
        """
        Test that ConsolePosix.readchar() returns None when no input is available.

        This covers the readchar() method's select-based input checking.
        """
        import pty
        import termios

        master_fd, slave_fd = pty.openpty()

        try:
            original_stdin = sys.stdin
            slave_file = os.fdopen(slave_fd, "r", closefd=False)

            try:
                sys.stdin = slave_file

                from mpremote.console import ConsolePosix

                console = ConsolePosix()
                console.enter()

                try:
                    # With no input pending, readchar should return None
                    result = console.readchar()
                    assert result is None, (
                        f"readchar() should return None when no input, got {result!r}"
                    )
                finally:
                    console.exit()

            finally:
                sys.stdin = original_stdin
                slave_file.close()

        finally:
            os.close(master_fd)
            os.close(slave_fd)

    @pytest.mark.skipif(sys.platform == "win32", reason="POSIX-only test")
    def test_readchar_returns_bytes_when_input_available(self):
        """
        Test that ConsolePosix.readchar() returns bytes when input is available.

        This covers the readchar() method's actual character reading path.
        """
        import pty
        import termios

        master_fd, slave_fd = pty.openpty()

        try:
            original_stdin = sys.stdin
            slave_file = os.fdopen(slave_fd, "r", closefd=False)

            try:
                sys.stdin = slave_file

                from mpremote.console import ConsolePosix

                console = ConsolePosix()
                console.enter()

                try:
                    # Write a character to the master side (simulating user input)
                    os.write(master_fd, b"x")

                    # Give a moment for the data to be available
                    time.sleep(0.05)

                    # readchar should now return the character
                    result = console.readchar()
                    assert result == b"x", f"readchar() should return b'x', got {result!r}"
                finally:
                    console.exit()

            finally:
                sys.stdin = original_stdin
                slave_file.close()

        finally:
            os.close(master_fd)
            os.close(slave_fd)


@requires_pexpect
@requires_unix
class TestReplMultipleCommands:
    """Additional REPL tests to verify multiple tests can run sequentially."""

    @device_params
    def test_repl_simple_expression(self, device, scenario, request):
        """
        Test that a simple expression evaluates correctly in the REPL.

        This is a simpler test that verifies basic REPL functionality
        and that multiple tests can run against the same device sequentially.
        """
        mpremote = get_mpremote_path()

        # Determine connection type for timing adjustments
        is_socket = device.startswith("socket://")
        is_rfc2217 = device.startswith("rfc2217://")
        is_physical_device = not (is_socket or is_rfc2217)

        wsl_delay = is_wsl() and is_physical_device

        if wsl_delay:
            connect_timeout = 20
            prompt_timeout = 15
        elif is_physical_device:
            connect_timeout = 15
            prompt_timeout = 10
        else:
            connect_timeout = 10
            prompt_timeout = 5

        # Build environment for subprocess with coverage context
        env = build_subprocess_env(request.node.nodeid)

        child = pexpect.spawn(
            sys.executable,
            [mpremote, "connect", device],
            encoding="utf-8",
            timeout=connect_timeout,
            env=env,
        )
        child.linesep = "\r\n"

        try:
            index = child.expect(
                ["Connected to MicroPython", pexpect.TIMEOUT, pexpect.EOF],
                timeout=connect_timeout,
            )
            if index != 0:
                pytest.skip(f"Could not connect to {device}")

            # All non-socket connections need stabilization
            if not is_socket:
                time.sleep(0.5)
                child.sendcontrol("c")
                time.sleep(0.3)

            child.sendline("")
            index = child.expect([">>>", pexpect.TIMEOUT, pexpect.EOF], timeout=prompt_timeout)
            if index != 0:
                pytest.skip(f"No prompt from {device}")

            time.sleep(0.3)

            # Send a simple arithmetic expression
            child.sendline("2 + 3")

            # Wait for the result
            time.sleep(0.5)
            index = child.expect(["5", pexpect.TIMEOUT], timeout=prompt_timeout)
            assert index == 0, "Expected result '5' from expression '2 + 3'"

            # Wait for prompt
            index = child.expect([">>>", pexpect.TIMEOUT], timeout=prompt_timeout)
            assert index == 0, "Expected prompt after expression"

        finally:
            child.sendcontrol("d")
            time.sleep(0.2)
            child.sendcontrol("]")
            child.expect([pexpect.EOF, pexpect.TIMEOUT], timeout=2)
            child.close()


if __name__ == "__main__":
    # Allow running directly
    pytest.main([__file__, "-v"])
