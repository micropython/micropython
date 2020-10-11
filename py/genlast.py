#!/usr/bin/env python3

import sys
from concurrent.futures import ThreadPoolExecutor
import multiprocessing
import threading
import subprocess


def checkoutput1(args):
    info = subprocess.run(args, check=True, stdout=subprocess.PIPE, input='')
    return info.stdout

idx1 = sys.argv.index('--')
idx2 = sys.argv.index('--', idx1+1)
check = sys.argv[1:idx1]
always = sys.argv[idx1+1:idx2]
command = sys.argv[idx2+1:]

output_lock = threading.Lock()
def preprocess(fn):
    output = checkoutput1(command + [fn])
    # Ensure our output doesn't interleave with others
    # a threading.Lock is not a context manager object :(
    try:
        output_lock.acquire()
        sys.stdout.buffer.write(output)
    finally:
        output_lock.release()

def maybe_preprocess(fn):
    if subprocess.call(["grep", "-lqE", "(MP_QSTR|translate)", fn]) == 0:
        preprocess(fn)

executor = ThreadPoolExecutor(max_workers=multiprocessing.cpu_count() + 1)
executor.map(maybe_preprocess, check)
executor.map(preprocess, always)
executor.shutdown()
