import pathlib
import sys
import shlex
import subprocess


def run(cmd):
    proc = subprocess.run(shlex.split(cmd), capture_output=True)
    return proc.returncode == 0


failed_count = 0
for board in sorted(list(pathlib.Path("boards/").glob("*/"))):
    board = board.name
    print(board)
    if not run(f"make BOARD={board} clean update-board-sdkconfig"):
        print("  Failed!")
        failed_count += 1

print(f"{failed_count} boards failed")
