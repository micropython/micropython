import importlib.util
import os
import subprocess
import sys
import tempfile


def load_secrets(secrets_path):
    spec = importlib.util.spec_from_file_location("socket_test_secrets", secrets_path)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def build_prologue(ssid, key):
    return f"""import network, socket, time
w = network.WLAN(network.STA_IF)
w.active(True)
if not w.isconnected():
    w.connect({ssid!r}, {key!r})
    for _ in range(30):
        if w.isconnected():
            break
        time.sleep(1)

for _ in range(10):
    if not w.isconnected():
        break
    try:
        probe = socket.socket()
        probe.settimeout(3)
        probe.connect(socket.getaddrinfo('micropython.org', 80)[0][-1])
        probe.close()
        break
    except OSError:
        try:
            probe.close()
        except Exception:
            pass
        time.sleep(1)
"""


def main():
    if len(sys.argv) < 2:
        print("usage: socket_internet.py <dut_port> [secrets.py]", file=sys.stderr)
        return 2

    dut_port = sys.argv[1]
    script_dir = os.path.abspath(os.path.dirname(__file__))
    tests_dir = os.path.abspath(os.path.join(script_dir, "..", "..", ".."))

    if len(sys.argv) >= 3:
        secrets_path = sys.argv[2]
        if not os.path.isabs(secrets_path):
            secrets_path = os.path.join(tests_dir, secrets_path)
    else:
        secrets_path = os.path.join(script_dir, "secrets.py")

    secrets = load_secrets(secrets_path)

    tests = [
        "net_hosted/connect_nonblock.py",
        "net_hosted/connect_timeout.py",
        "net_hosted/connect_poll.py",
    ]

    with tempfile.NamedTemporaryFile("w", suffix=".py", delete=False) as prologue_file:
        prologue_file.write(build_prologue(secrets.ssid, secrets.key))
        prologue_path = prologue_file.name

    try:
        cmd = [
            sys.executable,
            "run-tests.py",
            "-t",
            f"port:{dut_port}",
            "--begin",
            prologue_path,
        ]
        cmd.extend(tests)
        return subprocess.run(cmd, cwd=tests_dir).returncode
    finally:
        os.unlink(prologue_path)


if __name__ == "__main__":
    raise SystemExit(main())
