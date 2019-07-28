#!/usr/bin/env python3
#
# Motivation
# - https://github.com/micropython/micropython/issues/3040
# - https://github.com/micropython/micropython/pull/3057
# - https://github.com/micropython/micropython/issues/4955
#
import os
import os.path
import argparse
import shlex
import subprocess

try:
    import mpy_cross
    USE_MPY_CROSS_WRAPPER = True
except ImportError:
    USE_MPY_CROSS_WRAPPER = False


TARGET_OPTS = {
    "unix": "-mcache-lookup-bc",
    "baremetal": "",
}


def read_args():
    argparser = argparse.ArgumentParser(description="Compile all .py files to .mpy recursively")
    argparser.add_argument("-o", "--out", help="output directory (default: input dir)")
    argparser.add_argument("--target", help="select MicroPython target config")
    argparser.add_argument("-mcache-lookup-bc", action="store_true", help="cache map lookups in the bytecode")
    argparser.add_argument("dir", help="input directory")
    args = argparser.parse_args()

    args.dir = args.dir.rstrip("/")

    if not args.out:
        args.out = args.dir

    return args


def invoke_mpy_cross(arglist):

    # MicroPython mpy-cross distribution
    # https://pypi.org/project/mpy-cross/
    if USE_MPY_CROSS_WRAPPER:
        mpy_cross_args = shlex.split(arglist)
        outcome = mpy_cross.run(*mpy_cross_args, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        outcome.wait()
        stdout, stderr = outcome.communicate()
        if outcome.returncode != 0:
            print('ERROR: Invoking "mpy_cross.run()" failed')
            cmd = '{} {}'.format(mpy_cross.mpy_cross, ' '.join(mpy_cross_args))
            print('ERROR: The command was: {}'.format(cmd))
            print()
            print('STDOUT:\n', stdout.decode())
            print('STDERR:\n', stderr.decode())
            raise RuntimeError('Invoking "mpy_cross.run()" failed')

    # os.system(mpy-cross ...)
    else:
        cmd = "mpy-cross {}".format(arglist)
        res = os.system(cmd)
        if res != 0:
            print('ERROR: Invoking "mpy-cross" directly failed')
            print('ERROR: The command was: {}'.format(cmd))
            raise RuntimeError('Invoking "mpy-cross" failed')


def run():
    args = read_args()

    path_prefix_len = len(args.dir) + 1

    for path, subdirs, files in os.walk(args.dir):
        for filepath in files:
            if not filepath.endswith(".py"):
                continue

            fpath_py = os.path.join(path, filepath)
            fpath_mpy = fpath_py[path_prefix_len:-3] + ".mpy"

            out_fpath = os.path.join(args.out, fpath_mpy)

            # Create output directory gracefully.
            out_dir = os.path.dirname(out_fpath)
            if not os.path.isdir(out_dir):
                os.makedirs(out_dir)

            target_config = TARGET_OPTS.get(args.target, "")
            fname = fpath_py[path_prefix_len:]

            mpy_cross_args = "-v -v {target_config} -s {fname} {fpath_py} -o {out_fpath}".format(**locals())
            invoke_mpy_cross(mpy_cross_args)


if __name__ == '__main__':
    run()
