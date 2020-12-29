#!/usr/bin/env python3

# This file is part of the MicroPython project, http://micropython.org/
# The MIT License (MIT)
# Copyright (c) 2019 Damien P. George

import os
import subprocess
import sys
import argparse
from glob import glob

sys.path.append('../tools')
import pyboard

# Paths for host executables
if os.name == 'nt':
    CPYTHON3 = os.getenv('MICROPY_CPYTHON3', 'python3.exe')
    MICROPYTHON = os.getenv('MICROPY_MICROPYTHON', '../ports/windows/micropython.exe')
else:
    CPYTHON3 = os.getenv('MICROPY_CPYTHON3', 'python3')
    MICROPYTHON = os.getenv('MICROPY_MICROPYTHON', '../ports/unix/micropython')

PYTHON_TRUTH = CPYTHON3

BENCH_SCRIPT_DIR = 'perf_bench/'

def compute_stats(lst):
    avg = 0
    var = 0
    for x in lst:
        avg += x
        var += x * x
    avg /= len(lst)
    var = max(0, var / len(lst) - avg ** 2)
    return avg, var ** 0.5

def run_script_on_target(target, script):
    output = b''
    err = None

    if isinstance(target, pyboard.Pyboard):
        # Run via pyboard interface
        try:
            target.enter_raw_repl()
            output = target.exec_(script)
        except pyboard.PyboardError as er:
            err = er
    else:
        # Run local executable
        try:
            p = subprocess.run(target, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, input=script)
            output = p.stdout
        except subprocess.CalledProcessError as er:
            err = er

    return str(output.strip(), 'ascii'), err

def run_feature_test(target, test):
    with open('feature_check/' + test + '.py', 'rb') as f:
        script = f.read()
    output, err = run_script_on_target(target, script)
    if err is None:
        return output
    else:
        return 'CRASH: %r' % err

def run_benchmark_on_target(target, script):
    output, err = run_script_on_target(target, script)
    if err is None:
        time, norm, result = output.split(None, 2)
        try:
            return int(time), int(norm), result
        except ValueError:
            return -1, -1, 'CRASH: %r' % output
    else:
        return -1, -1, 'CRASH: %r' % err

def run_benchmarks(target, param_n, param_m, n_average, test_list):
    skip_complex = run_feature_test(target, 'complex') != 'complex'
    skip_native = run_feature_test(target, 'native_check') != ''

    for test_file in sorted(test_list):
        print(test_file + ': ', end='')

        # Check if test should be skipped
        skip = (
            skip_complex and test_file.find('bm_fft') != -1
            or skip_native and test_file.find('viper_') != -1
        )
        if skip:
            print('skip')
            continue

        # Create test script
        with open(test_file, 'rb') as f:
            test_script = f.read()
        with open(BENCH_SCRIPT_DIR + 'benchrun.py', 'rb') as f:
            test_script += f.read()
        test_script += b'bm_run(%u, %u)\n' % (param_n, param_m)

        # Write full test script if needed
        if 0:
            with open('%s.full' % test_file, 'wb') as f:
                f.write(test_script)

        # Run MicroPython a given number of times
        times = []
        scores = []
        error = None
        result_out = None
        for _ in range(n_average):
            time, norm, result = run_benchmark_on_target(target, test_script)
            if time < 0 or norm < 0:
                error = result
                break
            if result_out is None:
                result_out = result
            elif result != result_out:
                error = 'FAIL self'
                break
            times.append(time)
            scores.append(1e6 * norm / time)

        # Check result against truth if needed
        if error is None and result_out != 'None':
            _, _, result_exp = run_benchmark_on_target(PYTHON_TRUTH, test_script)
            if result_out != result_exp:
                error = 'FAIL truth'

        if error is not None:
            print(error)
        else:
            t_avg, t_sd = compute_stats(times)
            s_avg, s_sd = compute_stats(scores)
            print('{:.2f} {:.4f} {:.2f} {:.4f}'.format(t_avg, 100 * t_sd / t_avg, s_avg, 100 * s_sd / s_avg))
            if 0:
                print('  times: ', times)
                print('  scores:', scores)

        sys.stdout.flush()

def parse_output(filename):
    with open(filename) as f:
        params = f.readline()
        n, m, _ = params.strip().split()
        n = int(n.split('=')[1])
        m = int(m.split('=')[1])
        data = []
        for l in f:
            if l.find(': ') != -1 and l.find(': skip') == -1 and l.find('CRASH: ') == -1:
                name, values = l.strip().split(': ')
                values = tuple(float(v) for v in values.split())
                data.append((name,) + values)
    return n, m, data

def compute_diff(file1, file2, diff_score):
    # Parse output data from previous runs
    n1, m1, d1 = parse_output(file1)
    n2, m2, d2 = parse_output(file2)

    # Print header
    if diff_score:
        print('diff of scores (higher is better)')
    else:
        print('diff of microsecond times (lower is better)')
    if n1 == n2 and m1 == m2:
        hdr = 'N={} M={}'.format(n1, m1)
    else:
        hdr = 'N={} M={} vs N={} M={}'.format(n1, m1, n2, m2)
    print('{:24} {:>10} -> {:>10}   {:>10}   {:>7}% (error%)'.format(hdr, file1, file2, 'diff', 'diff'))

    # Print entries
    while d1 and d2:
        if d1[0][0] == d2[0][0]:
            # Found entries with matching names
            entry1 = d1.pop(0)
            entry2 = d2.pop(0)
            name = entry1[0].rsplit('/')[-1]
            av1, sd1 = entry1[1 + 2 * diff_score], entry1[2 + 2 * diff_score]
            av2, sd2 = entry2[1 + 2 * diff_score], entry2[2 + 2 * diff_score]
            sd1 *= av1 / 100 # convert from percent sd to absolute sd
            sd2 *= av2 / 100 # convert from percent sd to absolute sd
            av_diff = av2 - av1
            sd_diff = (sd1 ** 2 + sd2 ** 2) ** 0.5
            percent = 100 * av_diff / av1
            percent_sd = 100 * sd_diff / av1
            print('{:24} {:10.2f} -> {:10.2f} : {:+10.2f} = {:+7.3f}% (+/-{:.2f}%)'.format(name, av1, av2, av_diff, percent, percent_sd))
        elif d1[0][0] < d2[0][0]:
            d1.pop(0)
        else:
            d2.pop(0)

def main():
    cmd_parser = argparse.ArgumentParser(description='Run benchmarks for MicroPython')
    cmd_parser.add_argument('-t', '--diff-time', action='store_true', help='diff time outputs from a previous run')
    cmd_parser.add_argument('-s', '--diff-score', action='store_true', help='diff score outputs from a previous run')
    cmd_parser.add_argument('-p', '--pyboard', action='store_true', help='run tests via pyboard.py')
    cmd_parser.add_argument('-d', '--device', default='/dev/ttyACM0', help='the device for pyboard.py')
    cmd_parser.add_argument('-a', '--average', default='8', help='averaging number')
    cmd_parser.add_argument('--emit', default='bytecode', help='MicroPython emitter to use (bytecode or native)')
    cmd_parser.add_argument('N', nargs=1, help='N parameter (approximate target CPU frequency)')
    cmd_parser.add_argument('M', nargs=1, help='M parameter (approximate target heap in kbytes)')
    cmd_parser.add_argument('files', nargs='*', help='input test files')
    args = cmd_parser.parse_args()

    if args.diff_time or args.diff_score:
        compute_diff(args.N[0], args.M[0], args.diff_score)
        sys.exit(0)

    # N, M = 50, 25 # esp8266
    # N, M = 100, 100 # pyboard, esp32
    # N, M = 1000, 1000 # PC
    N = int(args.N[0])
    M = int(args.M[0])
    n_average = int(args.average)

    if args.pyboard:
        target = pyboard.Pyboard(args.device)
        target.enter_raw_repl()
    else:
        target = [MICROPYTHON, '-X', 'emit=' + args.emit]

    if len(args.files) == 0:
        tests_skip = ('benchrun.py',)
        if M <= 25:
            # These scripts are too big to be compiled by the target
            tests_skip += ('bm_chaos.py', 'bm_hexiom.py', 'misc_raytrace.py')
        tests = sorted(
            BENCH_SCRIPT_DIR + test_file for test_file in os.listdir(BENCH_SCRIPT_DIR)
                if test_file.endswith('.py') and test_file not in tests_skip
        )
    else:
        tests = sorted(args.files)

    print('N={} M={} n_average={}'.format(N, M, n_average))

    run_benchmarks(target, N, M, n_average, tests)

    if isinstance(target, pyboard.Pyboard):
        target.exit_raw_repl()
        target.close()

if __name__ == "__main__":
    main()
