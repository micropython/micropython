"""
Test the "micropython" CPython module
"""

from pathlib import Path
import json
import os
import subprocess

import micropython

# NOTE: pyodide (browser/emscripten) environment does not support subprocess
# use_subprocess=True allows to capture stdout/stderr, useful for debugging exceptions
def run_file(path : Path, use_subprocess=False):

    args = [ 'micropython', path ]
    if use_subprocess:
        subprocess.check_output(args)
    else:
        return micropython.run_main(args)

def run_code(code : str, use_subprocess=False):

    args = [ 'micropython', '-c', code ]
    if use_subprocess:
        subprocess.check_output(args)
    else:
        return micropython.run_main(args)


# Simple MicroPython program. Checks module imports, writing to disk
micropython_program_dump_implementation = """\
import json
import sys
out_file = 'out.txt'
with open(out_file, 'w') as f:
    o = sys.implementation
    s = json.dumps(dict(name=o.name, version=o.version, _mpy=o._mpy))
    f.write(s)
    f.flush()
"""

def test_run_file_simple():

    # clean existing outputs
    out_file = 'out.txt'
    if os.path.exists(out_file):
        os.unlink(out_file)

    # recreate script on-disk
    script_path = 'script.py'
    if os.path.exists(script_path):
        os.unlink(script_path)
    with open(script_path, 'w') as f:
        f.write(micropython_program_dump_implementation)

    # run as script
    run_file(script_path)

    # check output
    assert os.path.exists(out_file)
    with open(out_file) as f:
        data = json.load(f)

    assert data['name'] == 'micropython'

def test_run_code_simple():

    # clean existing outputs
    out_file = 'out.txt'
    if os.path.exists(out_file):
        os.unlink(out_file)

    code = micropython_program_dump_implementation
    run_code(code)

    assert os.path.exists(out_file)

    with open(out_file) as f:
        data = json.load(f)

    assert data['name'] == 'micropython'

def main():
    test_run_code_simple()
    test_run_file_simple()

if __name__ == '__main__':
    main()
