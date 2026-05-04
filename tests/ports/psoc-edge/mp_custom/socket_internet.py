import runpy
import subprocess
import sys
import tempfile
from pathlib import Path

# Underscore-prefixed names avoid colliding with variables in the test scripts
# that run after this prologue (via --begin).
_WIFI_CONNECT = """\
import network, socket, time
_ok = False
_w = network.WLAN(network.STA_IF)
_w.active(True)
if not _w.isconnected():
    _w.connect({ssid!r}, {key!r})
    for _ in range(30):
        if _w.isconnected():
            break
        time.sleep(1)
if _w.isconnected():
    for _ in range(10):
        try:
            _s = socket.socket()
            _s.settimeout(3)
            _s.connect(socket.getaddrinfo('micropython.org', 80)[0][-1])
            _s.close()
            _ok = True
            break
        except OSError:
            try:
                _s.close()
            except Exception:
                pass
            time.sleep(1)
"""

TESTS = [
    "net_hosted/connect_nonblock.py",
    "net_hosted/connect_timeout.py",
    "net_hosted/connect_poll.py",
]

_SCRIPT_DIR = Path(__file__).parent
_TESTS_DIR = _SCRIPT_DIR.parents[2]
_MPY_ROOT = _TESTS_DIR.parent


def run_preflight(dut_port, prologue):
    code = prologue + "print('READY:' + ('1' if _ok else '0'))\n"
    result = subprocess.run(
        [
            sys.executable,
            str(_MPY_ROOT / "tools" / "pyboard.py"),
            "-d",
            dut_port,
            "-c",
            code,
        ],
        capture_output=True,
        text=True,
    )
    if result.stdout:
        print(result.stdout, end="")
    if result.stderr:
        print(result.stderr, end="", file=sys.stderr)
    return result.returncode == 0 and "READY:1" in result.stdout


def run_tests(dut_port, prologue):
    with tempfile.NamedTemporaryFile("w", suffix=".py", delete=False) as f:
        f.write(prologue)
        prologue_path = Path(f.name)
    try:
        return subprocess.run(
            [
                sys.executable,
                "run-tests.py",
                "-t",
                f"port:{dut_port}",
                "--begin",
                str(prologue_path),
                "--print-failures",
                *TESTS,
            ],
            cwd=str(_TESTS_DIR),
        ).returncode
    finally:
        prologue_path.unlink(missing_ok=True)


def main():
    if len(sys.argv) < 2:
        print("usage: socket_internet.py <dut_port> [secrets.py]", file=sys.stderr)
        return 2

    dut_port = sys.argv[1]
    secrets_path = Path(sys.argv[2]) if len(sys.argv) >= 3 else _SCRIPT_DIR / "secrets.py"
    if not secrets_path.is_absolute():
        secrets_path = _TESTS_DIR / secrets_path

    secrets = runpy.run_path(str(secrets_path))
    prologue = _WIFI_CONNECT.format(ssid=secrets["ssid"], key=secrets["key"])

    if not run_preflight(dut_port, prologue):
        print("SKIP: socket-internet preflight failed (no internet connectivity)")
        return 0

    return run_tests(dut_port, prologue)


if __name__ == "__main__":
    raise SystemExit(main())
