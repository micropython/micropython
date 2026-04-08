#!/usr/bin/env python3
"""
Tests for Windows console UTF-8 handling in mpremote.

This tests the fix for split UTF-8 byte sequences that occur when:
1. Data is read byte-by-byte from serial/socket connections
2. Multi-byte UTF-8 characters (CJK, emoji, etc.) get split across reads

The fix involves:
- ConsoleWindows: Modern console detection + incremental decoder for legacy
- stdout_write_bytes: UTF-8 buffering to accumulate complete sequences

Usage:
    pytest test_windows_console_utf8.py -v

    # With device for integration tests
    MPREMOTE_DEVICES=socket://localhost:2218 pytest test_windows_console_utf8.py -v
"""

import codecs
import io
import os
import sys
from unittest.mock import MagicMock, patch

import pytest

# Test data: various UTF-8 sequences
UTF8_TEST_CASES = [
    # (description, string, utf8_bytes)
    ("ASCII", "hello", b"hello"),
    ("Latin-1 accents", "café", b"caf\xc3\xa9"),
    ("Chinese", "你好", b"\xe4\xbd\xa0\xe5\xa5\xbd"),
    ("Japanese", "日本語", b"\xe6\x97\xa5\xe6\x9c\xac\xe8\xaa\x9e"),
    ("Cyrillic", "Привет", b"\xd0\x9f\xd1\x80\xd0\xb8\xd0\xb2\xd0\xb5\xd1\x82"),
    ("Emoji", "🎉", b"\xf0\x9f\x8e\x89"),
    ("Mixed", "Hello 世界!", b"Hello \xe4\xb8\x96\xe7\x95\x8c!"),
]


@pytest.mark.skipif(sys.platform != "win32", reason="Windows-only test")
class TestConsoleWindowsModernDetection:
    """Unit tests for modern Windows console detection."""

    @pytest.fixture
    def console_class(self):
        """Import ConsoleWindows."""
        from mpremote.console import ConsoleWindows
        return ConsoleWindows

    def test_detect_windows_terminal(self, console_class, monkeypatch):
        """Windows Terminal should be detected as modern."""
        monkeypatch.setenv("WT_SESSION", "some-guid-value")
        monkeypatch.delenv("TERM_PROGRAM", raising=False)
        console = console_class()
        assert console._use_raw_output is True

    def test_detect_vscode_terminal(self, console_class, monkeypatch):
        """VS Code terminal should be detected as modern."""
        monkeypatch.delenv("WT_SESSION", raising=False)
        monkeypatch.setenv("TERM_PROGRAM", "vscode")
        console = console_class()
        assert console._use_raw_output is True

    def test_detect_conemu(self, console_class, monkeypatch):
        """ConEmu/Cmder should be detected as modern."""
        monkeypatch.delenv("WT_SESSION", raising=False)
        monkeypatch.delenv("TERM_PROGRAM", raising=False)
        monkeypatch.setenv("ConEmuANSI", "ON")
        console = console_class()
        assert console._use_raw_output is True

    def test_detect_utf8_encoding(self, console_class, monkeypatch):
        """Console with UTF-8 encoding should be detected as modern."""
        monkeypatch.delenv("WT_SESSION", raising=False)
        monkeypatch.delenv("TERM_PROGRAM", raising=False)
        monkeypatch.delenv("ConEmuANSI", raising=False)
        # Mock sys.stdout.encoding
        mock_stdout = MagicMock()
        mock_stdout.encoding = "utf-8"
        with patch.object(sys, "stdout", mock_stdout):
            console = console_class()
            assert console._use_raw_output is True

    def test_detect_legacy_console(self, console_class, monkeypatch):
        """Legacy console (no modern indicators, non-UTF-8) should use decoder."""
        monkeypatch.delenv("WT_SESSION", raising=False)
        monkeypatch.delenv("TERM_PROGRAM", raising=False)
        monkeypatch.delenv("ConEmuANSI", raising=False)
        # Mock sys.stdout.encoding as legacy code page
        mock_stdout = MagicMock()
        mock_stdout.encoding = "cp1252"
        mock_stdout.buffer = io.BytesIO()
        with patch.object(sys, "stdout", mock_stdout):
            console = console_class()
            assert console._use_raw_output is False
            assert hasattr(console, "_decoder")


@pytest.mark.skipif(sys.platform != "win32", reason="Windows-only test")
class TestConsoleWindowsIncrementalDecoder:
    """Unit tests for the incremental UTF-8 decoder in legacy mode."""

    @pytest.fixture
    def legacy_console(self):
        """Create a ConsoleWindows instance forced to legacy mode."""
        from mpremote.console import ConsoleWindows
        console = ConsoleWindows()
        # Force legacy mode
        console._use_raw_output = False
        console._decoder = codecs.getincrementaldecoder("utf-8")(errors="replace")
        return console

    @pytest.mark.parametrize("desc,text,utf8_bytes", UTF8_TEST_CASES)
    def test_complete_sequence(self, legacy_console, desc, text, utf8_bytes, capsys):
        """Complete UTF-8 sequences should decode correctly."""
        legacy_console.write(utf8_bytes)
        captured = capsys.readouterr()
        assert captured.out == text, f"Failed for {desc}"

    def test_split_2byte_sequence(self, legacy_console, capsys):
        """2-byte UTF-8 sequences split across writes should reassemble."""
        # é = \xc3\xa9 (2 bytes)
        legacy_console.write(b"\xc3")  # First byte
        captured = capsys.readouterr()
        assert captured.out == ""  # Should buffer, not output yet
        
        legacy_console.write(b"\xa9")  # Second byte
        captured = capsys.readouterr()
        assert captured.out == "é"

    def test_split_3byte_sequence(self, legacy_console, capsys):
        """3-byte UTF-8 sequences (CJK) split across writes should reassemble."""
        # 中 = \xe4\xb8\xad (3 bytes)
        legacy_console.write(b"\xe4")
        assert capsys.readouterr().out == ""
        
        legacy_console.write(b"\xb8")
        assert capsys.readouterr().out == ""
        
        legacy_console.write(b"\xad")
        assert capsys.readouterr().out == "中"

    def test_split_4byte_sequence(self, legacy_console, capsys):
        """4-byte UTF-8 sequences (emoji) split across writes should reassemble."""
        # 🎉 = \xf0\x9f\x8e\x89 (4 bytes)
        legacy_console.write(b"\xf0")
        assert capsys.readouterr().out == ""
        
        legacy_console.write(b"\x9f")
        assert capsys.readouterr().out == ""
        
        legacy_console.write(b"\x8e")
        assert capsys.readouterr().out == ""
        
        legacy_console.write(b"\x89")
        assert capsys.readouterr().out == "🎉"

    def test_mixed_ascii_and_multibyte(self, legacy_console, capsys):
        """Mixed ASCII and multi-byte sequences should handle correctly."""
        # "Hi 中!" split awkwardly
        legacy_console.write(b"Hi ")
        assert capsys.readouterr().out == "Hi "
        
        legacy_console.write(b"\xe4\xb8")  # Partial 中
        assert capsys.readouterr().out == ""
        
        legacy_console.write(b"\xad!")  # Complete 中 + !
        assert capsys.readouterr().out == "中!"

    def test_string_input_passthrough(self, legacy_console, capsys):
        """String input should pass through without decoding."""
        legacy_console.write("Hello 世界")
        captured = capsys.readouterr()
        assert captured.out == "Hello 世界"


@pytest.mark.skipif(sys.platform != "win32", reason="Windows-only test")
class TestConsoleWindowsModernOutput:
    """Unit tests for modern console raw output mode."""

    @pytest.fixture
    def modern_console(self):
        """Create a ConsoleWindows instance forced to modern mode."""
        from mpremote.console import ConsoleWindows
        console = ConsoleWindows()
        # Force modern mode with mock outfile
        console._use_raw_output = True
        console.outfile = io.BytesIO()
        return console

    @pytest.mark.parametrize("desc,text,utf8_bytes", UTF8_TEST_CASES)
    def test_bytes_written_directly(self, modern_console, desc, text, utf8_bytes):
        """Bytes should be written directly to outfile."""
        modern_console.write(utf8_bytes)
        modern_console.outfile.seek(0)
        assert modern_console.outfile.read() == utf8_bytes

    def test_string_encoded_to_utf8(self, modern_console):
        """String input should be encoded to UTF-8."""
        modern_console.write("你好")
        modern_console.outfile.seek(0)
        assert modern_console.outfile.read() == b"\xe4\xbd\xa0\xe5\xa5\xbd"


class TestStdoutWriteBytesBuffering:
    """Unit tests for stdout_write_bytes UTF-8 buffering."""

    @pytest.fixture(autouse=True)
    def reset_buffer(self):
        """Reset the global buffer before each test."""
        import mpremote.transport as transport
        transport._stdout_buffer = b""
        yield
        transport._stdout_buffer = b""

    @pytest.fixture
    def mock_stdout(self):
        """Mock sys.stdout.buffer for capturing output."""
        buffer = io.BytesIO()
        mock = MagicMock()
        mock.buffer = buffer
        mock.buffer.write = buffer.write
        mock.buffer.flush = MagicMock()
        return mock, buffer

    def test_complete_utf8_written_immediately(self, mock_stdout):
        """Complete UTF-8 sequences should be written immediately."""
        from mpremote.transport import stdout_write_bytes
        mock, buffer = mock_stdout
        
        with patch.object(sys, "stdout", mock):
            stdout_write_bytes(b"Hello")
            buffer.seek(0)
            assert buffer.read() == b"Hello"

    def test_split_utf8_buffered(self, mock_stdout):
        """Split UTF-8 sequences should be buffered until complete."""
        from mpremote.transport import stdout_write_bytes
        mock, buffer = mock_stdout
        
        with patch.object(sys, "stdout", mock):
            # Write first byte of 3-byte sequence
            stdout_write_bytes(b"\xe4")
            buffer.seek(0)
            assert buffer.read() == b""  # Should be buffered
            
            # Write second byte
            stdout_write_bytes(b"\xbd")
            buffer.seek(0)
            assert buffer.read() == b""  # Still buffered
            
            # Write third byte - now complete
            stdout_write_bytes(b"\xa0")
            buffer.seek(0)
            assert buffer.read() == b"\xe4\xbd\xa0"

    def test_ascii_plus_partial_utf8(self, mock_stdout):
        """ASCII followed by partial UTF-8 should write ASCII, buffer partial."""
        from mpremote.transport import stdout_write_bytes
        import mpremote.transport as transport
        mock, buffer = mock_stdout
        
        with patch.object(sys, "stdout", mock):
            # Write ASCII + first byte of multi-byte
            stdout_write_bytes(b"Hi\xe4")
            buffer.seek(0)
            assert buffer.read() == b"Hi"  # ASCII written
            assert transport._stdout_buffer == b"\xe4"  # Partial buffered

    def test_control_char_removed(self, mock_stdout):
        """Control character 0x04 should be removed."""
        from mpremote.transport import stdout_write_bytes
        mock, buffer = mock_stdout
        
        with patch.object(sys, "stdout", mock):
            stdout_write_bytes(b"Hello\x04World")
            buffer.seek(0)
            assert buffer.read() == b"HelloWorld"

    def test_empty_input_noop(self, mock_stdout):
        """Empty input should be a no-op."""
        from mpremote.transport import stdout_write_bytes
        mock, buffer = mock_stdout
        
        with patch.object(sys, "stdout", mock):
            stdout_write_bytes(b"")
            assert not mock.buffer.flush.called

    @pytest.mark.parametrize("desc,text,utf8_bytes", UTF8_TEST_CASES)
    def test_byte_by_byte_reassembly(self, mock_stdout, desc, text, utf8_bytes):
        """Bytes written one at a time should reassemble correctly."""
        from mpremote.transport import stdout_write_bytes
        import mpremote.transport as transport
        mock, buffer = mock_stdout
        
        with patch.object(sys, "stdout", mock):
            # Write each byte individually (simulating slow serial read)
            for byte in utf8_bytes:
                stdout_write_bytes(bytes([byte]))
            
            # All bytes should now be written
            buffer.seek(0)
            written = buffer.read()
            # Check that we got the complete sequence (may have been written in chunks)
            assert written == utf8_bytes, f"Failed for {desc}: got {written!r}"


class TestIntegrationModernConsole:
    """Integration tests for modern Windows console (requires device)."""

    @pytest.fixture
    def device(self):
        """Get device from environment, skip if not available."""
        device = os.environ.get("MPREMOTE_DEVICE") or os.environ.get("MPREMOTE_DEVICES", "").split(",")[0]
        if not device:
            pytest.skip("No device configured (set MPREMOTE_DEVICES)")
        return device.strip()

    @pytest.mark.parametrize("desc,text,_", UTF8_TEST_CASES)
    def test_print_unicode(self, device, desc, text, _, tmp_path):
        """Printing Unicode characters should work correctly."""
        import subprocess
        
        # Create a test script that prints the text
        script = tmp_path / "test_print.py"
        script.write_text(f'print({text!r})', encoding="utf-8")
        
        # Run via mpremote - capture as binary to avoid encoding issues
        result = subprocess.run(
            [sys.executable, "-m", "mpremote", "connect", device, "run", str(script)],
            capture_output=True,
            timeout=30,
        )
        
        assert result.returncode == 0, f"Failed for {desc}: {result.stderr.decode('utf-8', errors='replace')}"
        # Decode stdout as UTF-8 explicitly
        stdout = result.stdout.decode("utf-8", errors="replace")
        assert text in stdout, f"Expected {text!r} in output for {desc}, got {stdout!r}"


class TestIntegrationLegacyConsole:
    """Integration tests simulating legacy Windows console."""

    @pytest.fixture
    def device(self):
        """Get device from environment, skip if not available."""
        device = os.environ.get("MPREMOTE_DEVICE") or os.environ.get("MPREMOTE_DEVICES", "").split(",")[0]
        if not device:
            pytest.skip("No device configured (set MPREMOTE_DEVICES)")
        return device.strip()

    def test_print_unicode_legacy_simulation(self, device, tmp_path):
        """Test Unicode output with simulated legacy console environment."""
        import subprocess
        
        # Create a test script
        script = tmp_path / "test_print.py"
        script.write_text('print("你好世界")', encoding="utf-8")
        
        # Run with environment variables cleared to simulate legacy
        env = os.environ.copy()
        env.pop("WT_SESSION", None)
        env.pop("TERM_PROGRAM", None)
        env.pop("ConEmuANSI", None)
        
        # Capture as binary to avoid encoding issues
        result = subprocess.run(
            [sys.executable, "-m", "mpremote", "connect", device, "run", str(script)],
            capture_output=True,
            timeout=30,
            env=env,
        )
        
        # Should still work due to SetConsoleOutputCP(65001) and buffering
        assert result.returncode == 0, f"Failed: {result.stderr.decode('utf-8', errors='replace')}"
        # Decode stdout as UTF-8 explicitly
        stdout = result.stdout.decode("utf-8", errors="replace")
        assert "你好世界" in stdout, f"Expected '你好世界' in output, got {stdout!r}"


class TestConfigureUnicodeOutput:
    """Unit tests for configure_unicode_output function."""

    def test_noop_on_posix(self, monkeypatch):
        """Should be a no-op on non-Windows platforms."""
        monkeypatch.setattr(sys, "platform", "linux")
        from mpremote.console import configure_unicode_output
        # Should not raise
        configure_unicode_output()

    @pytest.mark.skipif(sys.platform != "win32", reason="Windows-only")
    def test_sets_console_code_page(self, monkeypatch):
        """Should call SetConsoleOutputCP(65001) on Windows."""
        import ctypes
        calls = []
        
        def mock_set_cp(cp):
            calls.append(cp)
        
        mock_kernel32 = MagicMock()
        mock_kernel32.SetConsoleOutputCP = mock_set_cp
        
        with patch.object(ctypes, "windll", MagicMock(kernel32=mock_kernel32)):
            from mpremote.console import configure_unicode_output
            configure_unicode_output()
        
        assert 65001 in calls
