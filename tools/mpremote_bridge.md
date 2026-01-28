# mpremote Bridge

This tool exposes a MicroPython REPL as a network server, allowing remote access to the REPL over a network connection without requiring an intermediate serial port.

## Overview

The bridge supports two protocols simultaneously:
- **RFC 2217** (port 2217): Telnet-based serial port emulation, compatible with all pyserial tools
- **Raw socket** (port 2218): Direct TCP connection, ~2x faster than RFC 2217

The bridge creates a virtual serial port server that:

1. Starts a MicroPython executable in a pseudo-terminal (PTY)
2. Creates servers on two network ports (RFC 2217 and raw socket)
3. Bridges data bidirectionally between the socket and MicroPython process
4. Handles graceful shutdown and cleanup
5. Ensures exclusive access - only one client can connect at a time across both ports

## Requirements

- Python 3.x
- pyserial package (also used by mpremote)
- MicroPython unix executable (built from `ports/unix/`)

## Usage

### Basic Usage

```bash
# From the tools directory - uses default path automatically
cd tools
python3 mpremote_bridge.py

# From the repository root
python3 tools/mpremote_bridge.py

# With a custom MicroPython path
python3 tools/mpremote_bridge.py /path/to/micropython
```

The script automatically uses `../ports/unix/build-standard/micropython` relative to its location, so you don't need to specify a path when running from the MicroPython repository.

### Bridge Options

```bash
python3 tools/mpremote_bridge.py -p 2217 -s 2218 -v
```

Options:
- `MICROPYTHON_PATH`: Optional path to the MicroPython executable (default: `../ports/unix/build-standard/micropython` relative to script)
- `-p PORT, --port PORT`: RFC 2217 TCP port (default: 2217)
- `-s PORT, --socket-port PORT`: Raw socket TCP port for faster connections (default: 2218, use 0 to disable)
- `-H HOST, --host HOST`: Local host/interface to bind to (default: all interfaces)
- `-v, --verbose`: Increase bridge verbosity (can be repeated: -v, -vv, -vvv)

### MicroPython Options

These options are passed through to the MicroPython executable:

- `-O`: Apply bytecode optimizations (can be repeated: `-O`, `-OO`, `-OOO`)
- `-X OPTION`: Implementation-specific options (e.g., `-X heapsize=4M`, `-X emit=native`)
- `--cwd DIR`: Working directory for MicroPython (used as filesystem root for unix port)
- `--mp-verbose`: MicroPython verbose mode (trace operations); can be repeated
- `--micropython-args ARGS`: Additional raw arguments to pass to MicroPython

### Examples

```bash
# Default MicroPython, default ports (2217 and 2218)
python3 tools/mpremote_bridge.py

# RFC 2217 only (disable raw socket port)
python3 tools/mpremote_bridge.py -s 0

# Custom ports
python3 tools/mpremote_bridge.py -p 3000 -s 3001

# With bytecode optimization level 2
python3 tools/mpremote_bridge.py -O -O

# Use an empty directory as filesystem root
mkdir -p /tmp/mp_root
python3 tools/mpremote_bridge.py --cwd /tmp/mp_root

# With custom heap size and native code emission
python3 tools/mpremote_bridge.py -X heapsize=4M -X emit=native

# Custom MicroPython path with options
python3 tools/mpremote_bridge.py ./my_micropython -O -O -X heapsize=4M

# Bridge verbose and MicroPython verbose
python3 tools/mpremote_bridge.py -vv --mp-verbose
```

## Connecting to the Server

Once the bridge is running, you can connect using either protocol:

### Using mpremote (Recommended)

```bash
# Fast connection via raw socket (recommended):
mpremote connect socket://localhost:2218 eval "print('Hello')"

# RFC 2217 connection (compatible with all tools):
mpremote connect rfc2217://localhost:2217 eval "print('Hello')"

# Use 'resume' to preserve state across connections:
mpremote connect socket://localhost:2218 resume exec "x = 42"
mpremote connect socket://localhost:2218 resume exec "print(x)"  # prints 42
```

### Performance Comparison

| Protocol | Mode | Typical Time |
|----------|------|--------------|
| socket:// | resume | **~450ms** |
| socket:// | soft reset | **~480ms** |
| rfc2217:// | resume | **~870ms** |
| rfc2217:// | soft reset | **~900ms** |

The raw socket protocol is ~2x faster because it skips the RFC 2217 telnet negotiation.

### Using Python with pyserial

```python
import serial

# Raw socket (faster)
ser = serial.serial_for_url('socket://localhost:2218', timeout=2)

# Or RFC 2217 (more features)
ser = serial.serial_for_url('rfc2217://localhost:2217', baudrate=115200, timeout=2)

# Read initial output
data = ser.read(1000)
print(data.decode('utf-8'))

# Send commands
ser.write(b'print("Hello World")\r\n')

# Read response
response = ser.read(1000)
print(response.decode('utf-8'))

ser.close()
```

### Using pyserial-miniterm

```bash
# Raw socket
pyserial-miniterm socket://localhost:2218

# RFC 2217
pyserial-miniterm rfc2217://localhost:2217 115200
```

## Exclusive Access

The bridge exposes a single MicroPython process on two ports. To prevent conflicts, **only one client can connect at a time** across both protocols:

- If a client is connected via socket, RFC 2217 connections are rejected
- If a client is connected via RFC 2217, socket connections are rejected
- Rejected clients receive an error message: `Error: Device busy - another client is connected`

This mimics the behavior of a physical serial port which can only have one connection.

## How It Works

1. **PTY Creation**: The tool creates a pseudo-terminal (PTY) pair, which simulates a real terminal device
2. **Process Spawning**: MicroPython is launched with its stdin/stdout/stderr connected to the PTY slave
3. **Dual Server**: Two TCP servers are created - RFC 2217 (with telnet IAC escaping) and raw socket (direct passthrough)
4. **Connection Guard**: When a client connects, connections on the other port are rejected to prevent conflicts
5. **Data Bridging**: Data flows bidirectionally:
   - Client → Socket → (RFC 2217 filter if applicable) → PTY master → MicroPython
   - MicroPython → PTY master → (RFC 2217 escape if applicable) → Socket → Client
6. **Persistent Mode**: The MicroPython process persists across client connections, just like a physical MCU stays powered. State is preserved between connections when using `resume`. Soft resets (Ctrl-D) restart the process, matching real MCU behavior

## Use Cases

- Remote access to MicroPython REPL without physical serial port
- Testing MicroPython code over the network
- Integration with serial port monitoring tools
- Automation and scripting with network-based REPL access
- Debugging and development from remote machines

## Limitations

- **No Security**: The bridge has no authentication or encryption. Use only on trusted networks or over VPN/SSH tunnels
- **Single Connection**: Only one client can connect at a time (across both ports)
- **Persistent Mode**: The MicroPython process persists across connections (like a physical MCU). Use `mpremote resume` to preserve state between connections. Soft resets restart the process, clearing RAM state.
- **Filesystem**: The unix port uses the host filesystem with the current working directory as root. Use `--cwd DIR` to specify a dedicated directory. Files persist across soft resets (unlike RAM state).
- **Linux/macOS Only**: Currently requires PTY support. See `mpremote_bridge_windows.md` for Windows support plans.

## Security Considerations

⚠️ **WARNING**: This tool implements no security measures. Anyone who can reach the TCP port can access the MicroPython REPL, which provides full code execution capabilities.

For secure remote access, consider:
- Using SSH port forwarding: `ssh -L 2217:localhost:2217 remote-host`
- Running on localhost only: `-H localhost`
- Using a VPN or firewall to restrict access
- Running behind a reverse proxy with authentication

## Troubleshooting

### Connection refused or timeout
- Check that the server is running and listening on the expected port
- Verify firewall settings allow connections to the port
- Ensure the correct host/port in the client connection string

### No output from MicroPython
- The tool uses a PTY to ensure MicroPython outputs its banner
- If still no output, try with `-vv` for debug logging
- Check if the MicroPython executable is working: `./micropython -i`

### Process doesn't terminate
- The tool tries to gracefully terminate processes (SIGTERM)
- If that fails after 2 seconds, it forcefully kills them (SIGKILL)
- Zombie processes should be automatically reaped

## Example Session

```
$ python3 tools/mpremote_bridge.py -v -O -O
2026-01-03 12:00:00,000 - root - INFO - mpremote Bridge - type Ctrl-C to quit
2026-01-03 12:00:00,000 - root - INFO - MicroPython executable: /home/user/micropython/ports/unix/build-standard/micropython
2026-01-03 12:00:00,000 - root - INFO - MicroPython optimization: -OO
2026-01-03 12:00:00,000 - root - INFO - RFC 2217 server listening on 0.0.0.0:2217
2026-01-03 12:00:00,000 - root - INFO -   Connect with: mpremote connect rfc2217://localhost:2217
2026-01-03 12:00:00,000 - root - INFO - Raw socket server listening on 0.0.0.0:2218
2026-01-03 12:00:00,000 - root - INFO -   Connect with: mpremote connect socket://localhost:2218
2026-01-03 12:00:00,000 - root - INFO - Persistent mode: MicroPython process persists across connections
2026-01-03 12:00:00,000 - root - INFO - Starting MicroPython: .../micropython -O -O
2026-01-03 12:00:00,000 - root - INFO - Waiting for connection...
2026-01-03 12:00:10,000 - root - INFO - Connected via socket by 127.0.0.1:54321
...
2026-01-03 12:00:15,000 - root - INFO - Rejected rfc2217 connection from 127.0.0.1:54322 - device busy (socket client connected)
...
2026-01-03 12:00:20,000 - root - INFO - Disconnected
2026-01-03 12:00:20,000 - root - INFO - Waiting for connection...
```

## License

This tool is part of the MicroPython project and is licensed under the MIT License.
