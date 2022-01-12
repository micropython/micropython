import pathlib
import sys
import shlex
import shutil
import subprocess

output_dir = pathlib.Path(sys.argv[-1])
output_dir.mkdir(parents=True, exist_ok=True)


def run(cmd):
    subprocess.run(shlex.split(cmd), capture_output=True)


for board in pathlib.Path("boards/").glob("*/"):
    board = board.name
    print(board)
    run(f"make BOARD={board} DEBUG=1 build-{board}/esp-idf/config/sdkconfig.h")
    shutil.copyfile(f"build-{board}/esp-idf/sdkconfig", output_dir / (board + "-debug.sdkconfig"))
    # run(f"python tools/update_sdkconfig.py --board={board} --update_all --debug=1")
    run(f"make BOARD={board} DEBUG=1 clean")
    run(f"make BOARD={board} build-{board}/esp-idf/config/sdkconfig.h")
    shutil.copyfile(f"build-{board}/esp-idf/sdkconfig", output_dir / (board + "-opt.sdkconfig"))
    # run(f"python tools/update_sdkconfig.py --board={board} --update_all")
    run(f"make BOARD={board} clean")
