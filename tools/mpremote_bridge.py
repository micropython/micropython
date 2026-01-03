#!/usr/bin/env python3
#
# This file is part of the MicroPython project, http://micropython.org/
#
# The MIT License (MIT)
#
# Copyright (c) 2026 Jos Verlinde

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
# PEP-0723 dependencies
# /// script
# dependencies = [
#   "pyserial >= 3.3",
# ]
# ///

"""
mpremote Bridge

This tool exposes a MicroPython unix REPL as a network server, allowing remote
access to the REPL over a network connection by mpremote and other tools.

Two protocols are supported:
- RFC 2217 (port 2217): Telnet-based serial port emulation, compatible with all pyserial tools
- Raw socket (port 2218): Direct TCP connection, ~2x faster than RFC 2217

Only one client can connect at a time across both ports (exclusive access).

Usage:
    mpremote_bridge.py [options] [MICROPYTHON_PATH]

Example:
    mpremote_bridge.py
    mpremote_bridge.py ./ports/unix/build-standard/micropython
    mpremote_bridge.py -p 2217 -s 2218 -v ./micropython

Then connect with:
    mpremote connect socket://localhost:2218      # Fast (recommended)
    mpremote connect rfc2217://localhost:2217     # Compatible
    pyserial-miniterm socket://localhost:2218
"""

from __future__ import annotations

import argparse
import logging
import os
import pty
import select
import socket
import subprocess
import sys
import threading
import time
import tty
from abc import ABC, abstractmethod
from typing import Callable, Optional, Tuple

from serial.rfc2217 import PortManager as RFC2217PortManager

# Control character constants
CTRL_A = b"\x01"  # Enter raw REPL
CTRL_B = b"\x02"  # Exit raw REPL
CTRL_C = b"\x03"  # Interrupt
CTRL_D = b"\x04"  # Soft reset / EOF

# Constants for mpremote compatibility
MPREMOTE_SOFT_REBOOT = b"soft reboot\r\n"
# Raw REPL soft reboot response that mpremote expects (mimics real MCU behavior)
MPREMOTE_RAW_REPL_SOFT_REBOOT = b"OK\r\nMPY: soft reboot\r\nraw REPL; CTRL-B to exit\r\n>"

# Bridge timing constants (in seconds)
# Note: MicroPython unix port starts in ~2ms and responds to raw REPL in <1ms
# These delays are safety margins, not performance requirements
MP_BRIDGE_RAW_REPL_ENTRY_DELAY = 0.005  # Delay after sending Ctrl-A (5ms is plenty)
MP_BRIDGE_BANNER_READ_TIMEOUT = 0.05  # Timeout for reading banner (50ms max)
MP_BRIDGE_PROCESS_RESTART_DELAY = 0.01  # Delay after restarting process (10ms)
MP_BRIDGE_POLL_INTERVAL = 1  # Status line poll interval
MP_BRIDGE_READ_TIMEOUT = 0.01  # Read timeout for PTY (10ms for responsiveness)
MP_BRIDGE_READ_BUFFER_SIZE = 4096  # Read buffer size for PTY

# Type alias for restart callback
RestartCallback = Callable[[], Optional[Tuple[int, subprocess.Popen]]]


class VirtualSerialPort:
    """
    A virtual serial port that wraps a PTY file descriptor.
    This simulates a serial port interface for the RFC 2217 PortManager.
    Emulates soft reboot behavior for compatibility with mpremote.
    """

    def __init__(
        self,
        fd: int,
        timeout: float = MP_BRIDGE_READ_TIMEOUT,
        restart_callback: Optional[RestartCallback] = None,
    ) -> None:
        """Initialize with a PTY file descriptor.

        Args:
            fd: File descriptor for the PTY master
            timeout: Read timeout in seconds
            restart_callback: Optional callback to restart the process for soft reboot
        """
        self.fd = fd
        self.process: Optional[subprocess.Popen] = None
        self.timeout = timeout
        self.in_waiting = 0
        self.restart_callback = restart_callback
        self.in_raw_repl = False
        self.pending_reboot_output = b""
        self.closed = False
        self._check_buffer_lock = threading.Lock()
        self._log = logging.getLogger("virtualserial")

        # Simulate serial port settings (stored but not used for PTY)
        self.baudrate = 115200
        self.bytesize = 8
        self.parity = "N"
        self.stopbits = 1
        self.rtscts = False
        self.dsrdtr = False
        self.xonxoff = False

        # Control lines (simulated, always active for subprocess)
        self.dtr = True
        self.rts = True
        self.cts = True
        self.dsr = True
        self.ri = False
        self.cd = True
        self.break_condition = False
        self._settings_backup = None
        self.name = "MicroPython REPL (subprocess)"

    def has_process_exited(self) -> bool:
        """Check if the MicroPython process has exited."""
        return self.process is not None and self.process.poll() is not None

    def can_restart(self) -> bool:
        """Check if the process can be restarted."""
        return self.restart_callback is not None

    def do_restart(self) -> bool:
        """Restart the process. Returns True on success."""
        if not self.restart_callback:
            return False
        result = self.restart_callback()
        if result:
            self.fd, self.process = result
            self.in_raw_repl = False
            self.closed = False
            return True
        return False

    def read(self, size: int = MP_BRIDGE_READ_BUFFER_SIZE) -> bytes:
        """Read up to size bytes from the PTY."""
        # If we have pending reboot output, return that first
        if self.pending_reboot_output:
            chunk = self.pending_reboot_output[:size]
            self.pending_reboot_output = self.pending_reboot_output[size:]
            return chunk

        if self.closed:
            return b""

        try:
            ready, _, _ = select.select([self.fd], [], [], self.timeout)
            if ready:
                data = os.read(self.fd, MP_BRIDGE_READ_BUFFER_SIZE)
                self._update_raw_repl_state_from_response(data)
                return data
        except (OSError, ValueError):
            self.closed = True
        return b""

    def _update_raw_repl_state_from_response(self, data: bytes) -> None:
        """Update raw REPL state based on MicroPython response."""
        if b"raw REPL; CTRL-B to exit" in data:
            self.in_raw_repl = True
        elif b">>>" in data and self.in_raw_repl:
            self.in_raw_repl = False

    def write(self, data: bytes) -> int:
        """Write data to the PTY."""
        if self.closed:
            return 0

        # Track raw REPL state based on client commands
        if CTRL_A in data:
            self.in_raw_repl = True
        elif CTRL_B in data:
            self.in_raw_repl = False

        self._log.debug(f"write({len(data)} bytes): {data!r}")

        try:
            return os.write(self.fd, data)
        except (OSError, ValueError):
            self.closed = True
            return 0

    def update_in_waiting(self) -> None:
        """Update the number of bytes waiting to be read."""
        with self._check_buffer_lock:
            if self.closed:
                self.in_waiting = 0
                return
            try:
                ready, _, _ = select.select([self.fd], [], [], 0)
                self.in_waiting = 1 if ready else 0
            except (OSError, ValueError):
                self.closed = True
                self.in_waiting = 0

    def get_settings(self) -> dict:
        """Get current serial port settings."""
        return {
            "baudrate": self.baudrate,
            "bytesize": self.bytesize,
            "parity": self.parity,
            "stopbits": self.stopbits,
            "rtscts": self.rtscts,
            "dsrdtr": self.dsrdtr,
            "xonxoff": self.xonxoff,
        }

    def apply_settings(self, settings: dict) -> None:
        """Apply serial port settings (stored but not actually used)."""
        self.baudrate = settings.get("baudrate", self.baudrate)
        self.bytesize = settings.get("bytesize", self.bytesize)
        self.parity = settings.get("parity", self.parity)
        self.stopbits = settings.get("stopbits", self.stopbits)
        self.rtscts = settings.get("rtscts", self.rtscts)
        self.dsrdtr = settings.get("dsrdtr", self.dsrdtr)
        self.xonxoff = settings.get("xonxoff", self.xonxoff)

    def reset_input_buffer(self) -> None:
        """Reset input buffer (no-op for subprocess)."""
        pass

    def reset_output_buffer(self) -> None:
        """Reset output buffer (no-op for subprocess)."""
        pass

    def send_break(self, duration: float = 0.25) -> None:
        """Send break condition (no-op for subprocess)."""
        pass

    def flush(self) -> None:
        """Flush output buffer (no-op for PTY mode)."""
        pass

    def reset_for_new_connection(self) -> None:
        """Reset state for a new client connection."""
        self.in_raw_repl = False
        self.pending_reboot_output = b""


class BaseRedirector(ABC):
    """
    Base class for redirectors that copy data between socket and subprocess.
    Handles the common soft reboot logic.
    """

    def __init__(self, serial: VirtualSerialPort, socket_conn: socket.socket) -> None:
        self.serial = serial
        self.socket = socket_conn
        self._write_lock = threading.Lock()
        self.log = logging.getLogger(self._get_logger_name())
        self.alive = False

    @abstractmethod
    def _get_logger_name(self) -> str:
        """Return the logger name for this redirector."""
        pass

    @abstractmethod
    def _send_to_client(self, data: bytes) -> None:
        """Send data to the client (with any necessary escaping)."""
        pass

    @abstractmethod
    def _receive_from_client(self) -> Optional[bytes]:
        """Receive and filter data from the client.

        Returns:
            bytes: Data to write to subprocess (may be empty if all control sequences)
            None: Connection was closed
        """
        pass

    def shortcircuit(self) -> None:
        """Connect the subprocess to the TCP port by copying data bidirectionally."""
        self.alive = True
        self.thread_read = threading.Thread(target=self.reader)
        self.thread_read.daemon = True
        self.thread_read.name = f"{self._get_logger_name()}:reader"
        self.thread_read.start()
        self._start_additional_threads()
        self.writer()

    def _start_additional_threads(self) -> None:
        """Start any additional threads needed by subclasses."""
        pass

    def reader(self) -> None:
        """Loop forever and copy subprocess output -> socket."""
        self.log.debug("reader thread started")
        while self.alive:
            try:
                if self._handle_process_exit():
                    continue  # Process was restarted, continue loop

                data = self.serial.read(MP_BRIDGE_READ_BUFFER_SIZE)
                if data:
                    self._send_to_client(data)
            except socket.error as e:
                self.log.error(f"Socket error in reader: {e}")
                break
            except Exception as e:
                self.log.error(f"Reader error: {e}")
                break
        self.alive = False
        self.log.debug("reader thread terminated")

    def _handle_process_exit(self) -> bool:
        """
        Handle MicroPython process exit (soft reboot).
        Returns True if process was restarted and loop should continue.
        """
        if not self.serial.has_process_exited():
            return False

        self.log.info("MicroPython process exited - performing auto-restart (soft reboot)")
        was_in_raw_repl = self.serial.in_raw_repl
        self.log.debug(f"was_in_raw_repl = {was_in_raw_repl}")

        # Send early soft reboot message if NOT in raw REPL
        if not was_in_raw_repl:
            self._safe_send(MPREMOTE_SOFT_REBOOT)

        # Restart the process
        if not self.serial.can_restart():
            self.log.error("No restart callback available")
            self.alive = False
            return False

        try:
            if not self.serial.do_restart():
                self.log.error("Process restart returned no result")
                self.alive = False
                return False

            self.log.info("Process restarted successfully")
            self._complete_restart(was_in_raw_repl)
            return True

        except Exception as e:
            self.log.error(f"Process restart failed: {e}")
            self.alive = False
            return False

    def _complete_restart(self, was_in_raw_repl: bool) -> None:
        """Complete the restart by reading banner and optionally re-entering raw REPL."""
        time.sleep(MP_BRIDGE_PROCESS_RESTART_DELAY)

        # Read and optionally forward the banner
        banner = self._read_banner()

        if was_in_raw_repl:
            self._reenter_raw_repl()
        elif banner:
            self._safe_send(banner)

    def _read_banner(self) -> bytes:
        """Read the startup banner from MicroPython."""
        try:
            ready, _, _ = select.select([self.serial.fd], [], [], MP_BRIDGE_BANNER_READ_TIMEOUT)
            if ready:
                banner = os.read(self.serial.fd, MP_BRIDGE_READ_BUFFER_SIZE)
                self.log.debug(f"Read banner: {banner!r}")
                return banner
        except OSError as e:
            self.log.error(f"Error reading banner: {e}")
        return b""

    def _reenter_raw_repl(self) -> None:
        """Re-enter raw REPL mode after process restart."""
        self.log.info("Re-entering raw REPL mode after soft reboot")
        try:
            os.write(self.serial.fd, CTRL_A)
            time.sleep(MP_BRIDGE_RAW_REPL_ENTRY_DELAY)

            # Read and discard the raw REPL response
            ready, _, _ = select.select([self.serial.fd], [], [], MP_BRIDGE_BANNER_READ_TIMEOUT)
            if ready:
                raw_response = os.read(self.serial.fd, MP_BRIDGE_READ_BUFFER_SIZE)
                self.log.debug(f"Raw REPL response: {raw_response!r}")

            # Send the expected soft reboot response to the client
            self._safe_send(MPREMOTE_RAW_REPL_SOFT_REBOOT)
            self.serial.in_raw_repl = True

        except OSError as e:
            self.log.error(f"Error re-entering raw REPL: {e}")
            self._safe_send(MPREMOTE_SOFT_REBOOT)

    def _safe_send(self, data: bytes) -> None:
        """Send data to client, ignoring errors."""
        try:
            self._send_to_client(data)
        except socket.error:
            pass

    def write_to_socket(self, data: bytes) -> None:
        """Thread-safe socket write."""
        with self._write_lock:
            try:
                self.socket.sendall(data)
            except socket.error:
                self.alive = False

    def writer(self) -> None:
        """Loop forever and copy socket -> subprocess input."""
        while self.alive:
            try:
                data = self._receive_from_client()
                if data is None:
                    break  # Connection closed
                if data:  # Only write if there's data after filtering
                    self.serial.write(data)
            except socket.error as e:
                self.log.error(f"Socket error in writer: {e}")
                break
            except Exception as e:
                self.log.error(f"Writer error: {e}")
                break
        self.stop()

    def stop(self) -> None:
        """Stop copying data."""
        self.log.debug("stopping")
        if self.alive:
            self.alive = False
            if hasattr(self, "thread_read"):
                self.thread_read.join(timeout=1)
            self._stop_additional_threads()

    def _stop_additional_threads(self) -> None:
        """Stop any additional threads started by subclasses."""
        pass


class Redirector(BaseRedirector):
    """
    RFC 2217 redirector - handles telnet IAC escaping and modem line polling.
    Based on pyserial's rfc2217_server.py example.
    """

    def __init__(
        self, virtual_serial: VirtualSerialPort, socket_conn: socket.socket, debug: bool = False
    ) -> None:
        super().__init__(virtual_serial, socket_conn)
        # Note: PortManager expects a connection with write() method
        self.rfc2217 = RFC2217PortManager(
            self.serial,
            self,  # type: ignore[arg-type]
            logger=logging.getLogger("rfc2217.server") if debug else None,
        )

    def _get_logger_name(self) -> str:
        return "redirector"

    def _send_to_client(self, data: bytes) -> None:
        """Send data with RFC 2217 escaping."""
        escaped = b"".join(self.rfc2217.escape(data))
        self.write_to_socket(escaped)

    def _receive_from_client(self) -> Optional[bytes]:
        """Receive data and filter RFC 2217 control sequences."""
        data = self.socket.recv(1024)
        if not data:
            return None  # Connection closed
        filtered = b"".join(self.rfc2217.filter(data))
        return filtered  # May be empty if all control sequences

    def _start_additional_threads(self) -> None:
        """Start the modem status line polling thread."""
        self.thread_poll = threading.Thread(target=self._statusline_poller)
        self.thread_poll.daemon = True
        self.thread_poll.name = "rfc2217:status-poll"
        self.thread_poll.start()

    def _statusline_poller(self) -> None:
        """Poll for modem status line changes."""
        self.log.debug("status line poll thread started")
        while self.alive:
            time.sleep(MP_BRIDGE_POLL_INTERVAL)
            self.rfc2217.check_modem_lines()
        self.log.debug("status line poll thread terminated")

    def _stop_additional_threads(self) -> None:
        """Stop the polling thread."""
        if hasattr(self, "thread_poll"):
            self.thread_poll.join(timeout=1)

    # Required by RFC2217 PortManager - called for control responses
    def write(self, data: bytes) -> None:
        """Write method required by RFC 2217 PortManager."""
        self.write_to_socket(data)


class SocketRedirector(BaseRedirector):
    """
    Simple socket redirector - direct byte pass-through without RFC 2217 overhead.
    Much faster than RFC 2217 for local connections.
    """

    def __init__(
        self, serial: VirtualSerialPort, socket_conn: socket.socket, debug: bool = False
    ) -> None:
        # debug parameter kept for API compatibility with Redirector
        super().__init__(serial, socket_conn)
        # Silence unused parameter warning
        _ = debug

    def _get_logger_name(self) -> str:
        return "socket-redirector"

    def _send_to_client(self, data: bytes) -> None:
        """Send data directly without escaping."""
        self.write_to_socket(data)

    def _receive_from_client(self) -> Optional[bytes]:
        """Receive data directly without filtering."""
        data = self.socket.recv(MP_BRIDGE_READ_BUFFER_SIZE)
        if not data:
            return None  # Connection closed
        return data


def parse_arguments() -> argparse.Namespace:
    """Parse command line arguments."""
    script_dir = os.path.dirname(os.path.abspath(__file__))
    default_micropython = os.path.normpath(
        os.path.join(script_dir, "..", "ports", "unix", "build-standard", "micropython")
    )

    parser = argparse.ArgumentParser(
        description="mpremote Bridge - Expose MicroPython REPL via RFC 2217 and raw socket.",
        epilog="""\
NOTE: No security measures are implemented. Anyone can remotely connect
to this service over the network.

Only one connection at once is supported. When the connection is terminated,
it waits for the next connect.

The MicroPython process persists across connections (like a physical MCU).
Use 'mpremote resume' to preserve state between connections.

Two protocols are supported:
  - RFC 2217 (port 2217): Compatible with pyserial, supports serial port emulation
  - Raw socket (port 2218): Faster, no protocol overhead (~300ms faster per connection)

Examples:
  %(prog)s                           # Use default MicroPython
  %(prog)s -p 2217 -s 2218           # Use RFC 2217 on 2217, socket on 2218 (default)
  %(prog)s -s 0                      # Disable raw socket port
  %(prog)s ./my_micropython          # Use custom MicroPython path
  %(prog)s --cwd /tmp/mp_root        # Use /tmp/mp_root as filesystem root
  %(prog)s -O -O -X heapsize=4M      # Pass options to MicroPython
""",
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )

    parser.add_argument(
        "MICROPYTHON_PATH",
        nargs="?",
        default=default_micropython,
        help="Path to the MicroPython executable (default: %(default)s)",
    )

    parser.add_argument(
        "-p",
        "--port",
        type=int,
        default=2217,
        metavar="PORT",
        help="RFC 2217 TCP port (default: %(default)s)",
    )

    parser.add_argument(
        "-s",
        "--socket-port",
        type=int,
        default=2218,
        metavar="PORT",
        help="Raw socket TCP port for faster connections (default: %(default)s, 0 to disable)",
    )

    parser.add_argument(
        "-H",
        "--host",
        default="",
        metavar="HOST",
        help="Local host/interface to bind to (default: all interfaces)",
    )

    parser.add_argument(
        "-v",
        "--verbose",
        dest="verbosity",
        action="count",
        default=0,
        help="Increase verbosity of the bridge (can be given multiple times)",
    )

    # MicroPython-specific arguments
    mp_group = parser.add_argument_group(
        "MicroPython options",
        "These options are passed to the MicroPython executable",
    )

    mp_group.add_argument(
        "-O",
        dest="mp_optimize",
        action="count",
        default=0,
        help="Apply bytecode optimizations (can be given multiple times: -O, -OO, -OOO)",
    )

    mp_group.add_argument(
        "-X",
        dest="mp_impl_opts",
        action="append",
        default=[],
        metavar="OPTION",
        help="Implementation-specific options (e.g., -X heapsize=4M, -X emit=native)",
    )

    mp_group.add_argument(
        "--mp-verbose",
        dest="mp_verbose",
        action="count",
        default=0,
        help="MicroPython verbose mode (trace operations); can be given multiple times",
    )

    mp_group.add_argument(
        "--micropython-args",
        default="",
        metavar="ARGS",
        help='Additional arguments to pass to MicroPython (e.g., "-i")',
    )

    mp_group.add_argument(
        "--cwd",
        dest="cwd",
        default=None,
        metavar="DIR",
        help="Working directory for MicroPython (used as filesystem root for unix port)",
    )

    return parser.parse_args()


def validate_arguments(args: argparse.Namespace) -> None:
    """Validate command line arguments, exit on error."""
    if not os.path.isfile(args.MICROPYTHON_PATH):
        print(f"Error: MicroPython executable not found: {args.MICROPYTHON_PATH}", file=sys.stderr)
        sys.exit(1)

    if not os.access(args.MICROPYTHON_PATH, os.X_OK):
        print(
            f"Error: MicroPython executable is not executable: {args.MICROPYTHON_PATH}",
            file=sys.stderr,
        )
        sys.exit(1)

    if args.cwd:
        if not os.path.isdir(args.cwd):
            print(f"Error: Working directory does not exist: {args.cwd}", file=sys.stderr)
            sys.exit(1)
        args.cwd = os.path.abspath(args.cwd)


def setup_logging(verbosity: int) -> None:
    """Configure logging based on verbosity level."""
    verbosity = min(verbosity, 3)
    level = (logging.WARNING, logging.INFO, logging.DEBUG, logging.NOTSET)[verbosity]
    logging.basicConfig(
        level=logging.INFO, format="%(asctime)s - %(name)s - %(levelname)s - %(message)s"
    )
    logging.getLogger("rfc2217").setLevel(level)


def build_micropython_command(args: argparse.Namespace) -> list[str]:
    """Build the MicroPython command line."""
    cmd = [args.MICROPYTHON_PATH]
    cmd.extend(["-v"] * args.mp_verbose)
    cmd.extend(["-O"] * args.mp_optimize)
    for opt in args.mp_impl_opts:
        cmd.extend(["-X", opt])
    if args.micropython_args:
        cmd.extend(args.micropython_args.split())
    return cmd


def create_server_sockets(
    args: argparse.Namespace,
) -> list[Tuple[socket.socket, str, type]]:
    """Create and bind server sockets. Returns list of (socket, protocol_name, redirector_class)."""
    servers: list[Tuple[socket.socket, str, type]] = []
    bind_addr = args.host or "0.0.0.0"

    # Create RFC 2217 server socket
    srv_rfc2217 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    srv_rfc2217.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    try:
        srv_rfc2217.bind((args.host, args.port))
        srv_rfc2217.listen(1)
        servers.append((srv_rfc2217, "rfc2217", Redirector))
        logging.info(f"RFC 2217 server listening on {bind_addr}:{args.port}")
        logging.info(f"  Connect with: mpremote connect rfc2217://localhost:{args.port}")
    except OSError as e:
        logging.error(f"Could not bind RFC 2217 to {args.host}:{args.port}: {e}")
        sys.exit(1)

    # Create raw socket server (optional)
    if args.socket_port > 0:
        srv_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        srv_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        try:
            srv_socket.bind((args.host, args.socket_port))
            srv_socket.listen(1)
            servers.append((srv_socket, "socket", SocketRedirector))
            logging.info(f"Raw socket server listening on {bind_addr}:{args.socket_port}")
            logging.info(f"  Connect with: mpremote connect socket://localhost:{args.socket_port}")
        except OSError as e:
            logging.warning(f"Could not bind raw socket to {args.host}:{args.socket_port}: {e}")

    return servers


class MicroPythonProcessManager:
    """Manages the MicroPython subprocess lifecycle."""

    def __init__(self, cmd: list[str], cwd: Optional[str] = None) -> None:
        self.cmd = cmd
        self.cwd = cwd
        self.master_fd: Optional[int] = None
        self.process: Optional[subprocess.Popen] = None

    def create_process(self) -> Tuple[int, subprocess.Popen]:
        """Create and return a new MicroPython process with PTY."""
        # Close old master_fd if it exists
        if self.master_fd is not None:
            try:
                os.close(self.master_fd)
            except OSError:
                pass

        # Create a pseudo-terminal for the process
        new_master_fd, new_slave_fd = pty.openpty()

        # Set the terminal to raw mode to avoid line buffering
        try:
            tty.setraw(new_master_fd)
        except (OSError, tty.error):
            pass  # Ignore errors setting raw mode

        self.process = subprocess.Popen(
            self.cmd,
            stdin=new_slave_fd,
            stdout=new_slave_fd,
            stderr=new_slave_fd,
            cwd=self.cwd,
            close_fds=False,
        )

        # Close the slave fd in the parent process
        os.close(new_slave_fd)

        self.master_fd = new_master_fd
        return (new_master_fd, self.process)

    def restart(self) -> Tuple[int, subprocess.Popen]:
        """Restart MicroPython process for soft reboot."""
        logging.info(f"Restarting MicroPython for soft reboot: {' '.join(self.cmd)}")
        return self.create_process()

    def cleanup(self) -> None:
        """Clean up resources on shutdown."""
        if self.master_fd is not None:
            try:
                os.close(self.master_fd)
            except OSError:
                pass

        if self.process and self.process.poll() is None:
            logging.info("Terminating MicroPython process...")
            self.process.terminate()
            try:
                self.process.wait(timeout=2)
            except subprocess.TimeoutExpired:
                logging.warning("MicroPython process did not terminate, killing...")
                self.process.kill()
                self.process.wait()


def _reject_pending_connections(
    servers: list[Tuple[socket.socket, str, type]], active_protocol: str
) -> None:
    """Reject any pending connections on other server sockets while one is active."""
    for srv, protocol_name, _ in servers:
        if protocol_name == active_protocol:
            continue
        # Check for pending connections without blocking
        try:
            readable, _, _ = select.select([srv], [], [], 0)
            if readable:
                # Accept and immediately close with a message
                try:
                    pending_socket, addr = srv.accept()
                    logging.info(
                        f"Rejected {protocol_name} connection from {addr[0]}:{addr[1]} "
                        f"- device busy ({active_protocol} client connected)"
                    )
                    # Send a brief error message before closing
                    try:
                        pending_socket.sendall(
                            b"\r\nError: Device busy - another client is connected\r\n"
                        )
                    except socket.error:
                        pass
                    pending_socket.close()
                except socket.error:
                    pass
        except (ValueError, OSError):
            pass


def run_server_loop(
    servers: list[Tuple[socket.socket, str, type]],
    virtual_serial: VirtualSerialPort,
    process_manager: MicroPythonProcessManager,
    debug: bool,
) -> None:
    """Main server loop - accept connections and handle them.

    Only one client connection is allowed at a time across all protocols.
    Connections on other ports are rejected while a client is connected.
    """
    server_map = {srv: (proto, redir_cls) for srv, proto, redir_cls in servers}
    server_sockets = [srv for srv, _, _ in servers]

    while True:
        try:
            logging.info("Waiting for connection...")
            readable, _, _ = select.select(server_sockets, [], [])

            for srv in readable:
                protocol_name, redirector_class = server_map[srv]
                client_socket, addr = srv.accept()
                logging.info(f"Connected via {protocol_name} by {addr[0]}:{addr[1]}")
                client_socket.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)

                # Check if process is still alive, restart if needed
                if virtual_serial.has_process_exited():
                    logging.info("MicroPython process has exited, restarting...")
                    fd, proc = process_manager.create_process()
                    virtual_serial.fd = fd
                    virtual_serial.process = proc
                    virtual_serial.closed = False

                virtual_serial.reset_for_new_connection()

                redirector = redirector_class(virtual_serial, client_socket, debug)
                try:
                    # Start a background thread to reject connections on other ports
                    reject_thread_alive = True

                    def reject_loop() -> None:
                        while reject_thread_alive:
                            _reject_pending_connections(servers, protocol_name)
                            time.sleep(0.1)  # Check every 100ms

                    reject_thread = threading.Thread(target=reject_loop, daemon=True)
                    reject_thread.name = "connection-guard"
                    reject_thread.start()

                    redirector.shortcircuit()
                finally:
                    reject_thread_alive = False
                    logging.info("Disconnected")
                    redirector.stop()
                    client_socket.close()

        except KeyboardInterrupt:
            sys.stdout.write("\n")
            break
        except socket.error as e:
            logging.error(f"Socket error: {e}")
        except Exception as e:
            logging.error(f"Unexpected error: {e}", exc_info=debug)


def main() -> None:
    """Main entry point."""
    args = parse_arguments()
    validate_arguments(args)
    setup_logging(args.verbosity)

    logging.info("mpremote Bridge - type Ctrl-C to quit")
    logging.info(f"MicroPython executable: {args.MICROPYTHON_PATH}")
    if args.cwd:
        logging.info(f"MicroPython working directory: {args.cwd}")
    if args.mp_verbose:
        logging.info(f"MicroPython verbosity: -{args.mp_verbose * 'v'}")
    if args.mp_optimize:
        logging.info(f"MicroPython optimization: -{args.mp_optimize * 'O'}")
    if args.mp_impl_opts:
        logging.info(f"MicroPython options: {', '.join(args.mp_impl_opts)}")

    cmd = build_micropython_command(args)
    servers = create_server_sockets(args)
    logging.info("MicroPython process persists across connections")

    process_manager = MicroPythonProcessManager(cmd, args.cwd)
    logging.info(f"Starting MicroPython: {' '.join(cmd)}")
    master_fd, process = process_manager.create_process()

    virtual_serial = VirtualSerialPort(
        master_fd,
        timeout=0.1,
        restart_callback=process_manager.restart,
    )
    virtual_serial.process = process

    try:
        run_server_loop(servers, virtual_serial, process_manager, args.verbosity > 0)
    finally:
        logging.info("Shutting down bridge...")
        process_manager.cleanup()
        for srv, _, _ in servers:
            try:
                srv.close()
            except OSError:
                pass
        logging.info("--- exit ---")


if __name__ == "__main__":
    main()
