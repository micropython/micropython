import subprocess
import tempfile
from pathlib import Path


def run_micropython_code(source: str) -> str:
    """Run source code on MCU and return the output"""
    # TODO: add option to run against unix port ( webassembly / windows )
    # TODO: add option to run against specific port / board (e.g. pyboard, esp32, etc)
    # TODO: Skip if no mpremote / no board detected
    # TODO: add option to run against simulator (WOKWI)
    __tracebackhide__ = True  # hide helper details
    tf = None
    try:
        with tempfile.NamedTemporaryFile("w", suffix=".py", delete=False) as tf:
            tf.write(source)
            tf.close()
            cmd = ["mpremote", "resume", "run", tf.name]
            print("Running:", " ".join(cmd))
            result = subprocess.run(cmd, capture_output=True, text=True, check=True)
            return result.stdout
    except subprocess.CalledProcessError as e:
        print(f"source: {'-' * 40}\n{source}\n{'-' * 40}")
        raise Exception(
            f"mpremote exited with code {e.returncode}, stderr: {e.stderr}, stdout: {e.stdout}"
        )
    finally:
        if tf:
            Path(tf.name).unlink()


def reset_micropython_mcu() -> None:
    """Reset the MicroPython MCU to clear namespace/state"""
    __tracebackhide__ = True  # hide helper details
    try:
        cmd = ["mpremote", "reset"]
        print("Running:", " ".join(cmd))
        subprocess.run(cmd, capture_output=True, text=True, check=True)
        # Give the MCU a moment to restart
        import time

        time.sleep(0.5)
    except subprocess.CalledProcessError as e:
        raise Exception(
            f"mpremote reset failed with code {e.returncode}, stderr: {e.stderr}, stdout: {e.stdout}"
        )
