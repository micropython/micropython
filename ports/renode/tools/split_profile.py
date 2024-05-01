# Split a profile file into smaller chunks.

import pathlib
import sys

input_file = pathlib.Path(sys.argv[-1])

supervisor = input_file.with_suffix(".supervisor" + input_file.suffix)
boot_py = input_file.with_suffix(".boot_py" + input_file.suffix)
code_py = input_file.with_suffix(".code_py" + input_file.suffix)

supervisor_f = supervisor.open("w")
boot_py_f = boot_py.open("w")
code_py_f = code_py.open("w")

for line in input_file.open():
    if "run_boot_py" in line:
        boot_py_f.write(line)
    if "run_code_py" in line:
        code_py_f.write(line)

supervisor_f.close()
boot_py_f.close()
code_py_f.close()
