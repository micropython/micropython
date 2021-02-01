#!/usr/bin/env python3
import argparse
import os
import os.path

argparser = argparse.ArgumentParser(description="Compile all .py files to .mpy recursively")
argparser.add_argument("-o", "--out", help="output directory (default: input dir)")
argparser.add_argument("--target", help="select MicroPython target config")
argparser.add_argument(
    "-mcache-lookup-bc", action="store_true", help="cache map lookups in the bytecode"
)
argparser.add_argument("dir", help="input directory")
args = argparser.parse_args()

TARGET_OPTS = {
    "unix": "-mcache-lookup-bc",
    "baremetal": "",
}

args.dir = args.dir.rstrip("/")

if not args.out:
    args.out = args.dir

path_prefix_len = len(args.dir) + 1

for path, subdirs, files in os.walk(args.dir):
    for f in files:
        if f.endswith(".py"):
            fpath = path + "/" + f
            # print(fpath)
            out_fpath = args.out + "/" + fpath[path_prefix_len:-3] + ".mpy"
            out_dir = os.path.dirname(out_fpath)
            if not os.path.isdir(out_dir):
                os.makedirs(out_dir)
            cmd = "mpy-cross -v -v %s -s %s %s -o %s" % (
                TARGET_OPTS.get(args.target, ""),
                fpath[path_prefix_len:],
                fpath,
                out_fpath,
            )
            # print(cmd)
            res = os.system(cmd)
            assert res == 0
