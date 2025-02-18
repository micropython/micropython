# MicroPython Test Suite

This directory contains tests for most parts of MicroPython.

To run all stable tests, run the "run-tests.py" script in this directory.  By default
that will run the test suite against the unix port of MicroPython.

To run the test suite against a bare-metal target (a board running MicroPython firmware)
use the `-t` option to specify the serial port.  This will automatically detect the
target platform and run the appropriate set of tests for that platform.  For example:

    $ ./run-tests.py -t /dev/ttyACM0

That will run tests on the `/dev/ttyACM0` serial port.  You can also use shortcut
device names like `a<n>` for `/dev/ttyACM<n>` and `c<n>` for `COM<n>`.  Use
`./run-tests.py --help` to see all of the device possibilities, and other options.

There are three kinds of tests:

* Tests that use `unittest`: these tests require `unittest` to be installed on the
  target (eg via `mpremote mip install unittest`), and are used to test things that are
  MicroPython-specific, such as behaviour that is different to CPython, modules that
  aren't available in CPython, and hardware tests.  These tests are run only under
  MicroPython and the test passes if the `unittest` runner prints "OK" at the end of the
  run.  Other output may be printed, eg for use as diagnostics, and this output does not
  affect the result of the test.

* Tests with a corresponding `.exp` file: similar to the `unittest` tests, these tests
  are for features that generally cannot be run under CPython.  In this case the test is
  run under MicroPython only and the output from MicroPython is compared against the
  provided `.exp` file.  The test passes if the output matches exactly.

* Tests without a corresponding `.exp` file (and don't use `unittest`): these tests are
  used to test MicroPython behaviour that should precisely match CPython.  These tests
  are first run under CPython and the output captured, and then run under MicroPython
  and the output compared to the CPython output.  The test passes if the output matches
  exactly.  If the output differs then the test fails and the outputs are saved in a
  `.exp` and a `.out` file respectively.

In all three cases above, the test can usually be run directly on the target MicroPython
instance, either using the unix port with `micropython <test.py>`, or on a board with
`mpremote run <test.py>`.  This is useful for creating and debugging tests.

Tests of capabilities not supported on all platforms should be written
to check for the capability being present. If it is not, the test
should merely output 'SKIP' followed by the line terminator, and call
sys.exit() to raise SystemExit, instead of attempting to test the
missing capability. The testing framework (run-tests.py in this
directory, test_main.c in qemu_arm) recognizes this as a skipped test.

There are a few features for which this mechanism cannot be used to
condition a test. The run-tests.py script uses small scripts in the
feature_check directory to check whether each such feature is present,
and skips the relevant tests if not.

When creating new tests, anything that relies on float support should go in the
float/ subdirectory.  Anything that relies on import x, where x is not a built-in
module, should go in the import/ subdirectory.

## perf_bench

The `perf_bench` directory contains some performance benchmarks that can be used
to benchmark different MicroPython firmwares or host ports.

The runner utility is `run-perfbench,py`. Execute `./run-perfbench.py --help`
for a full list of command line options.

### Benchmarking a target

To run tests on a firmware target using `pyboard.py`, run the command line like
this:

```
./run-perfbench.py -p -d /dev/ttyACM0 168 100
```

* `-p` indicates running on a remote target via pyboard.py, not the host.
* `-d PORTNAME` is the serial port, `/dev/ttyACM0` is the default if not
  provided.
* `168` is value `N`, the approximate CPU frequency in MHz (in this case Pyboard
  V1.1 is 168MHz). It's possible to choose other values as well: lower values
  like `10` will run much the tests much quicker, higher values like `1000` will
  run much longer.
* `100` is value `M`, the approximate heap size in kilobytes (can get this from
  `import micropython; micropython.mem_info()` or estimate it). It's possible to
  choose other values here too: lower values like `10` will run shorter/smaller
  tests, and higher values will run bigger tests. The maximum value of `M` is
  limited by available heap, and the tests are written so the "recommended"
  value is approximately the upper limit.

### Benchmarking the host

To benchmark the host build (unix/Windows), run like this:

```
./run-perfbench.py 2000 10000
```

The output of perfbench is a list of tests and times/scores, like this:

```
N=2000 M=10000 n_average=8
perf_bench/bm_chaos.py: SKIP
perf_bench/bm_fannkuch.py: 94550.38 2.9145 84.68 2.8499
perf_bench/bm_fft.py: 79920.38 10.0771 129269.74 8.8205
perf_bench/bm_float.py: 43844.62 17.8229 353219.64 17.7693
perf_bench/bm_hexiom.py: 32959.12 15.0243 775.77 14.8893
perf_bench/bm_nqueens.py: 40855.00 10.7297 247776.15 11.3647
perf_bench/bm_pidigits.py: 64547.75 2.5609 7751.36 2.5996
perf_bench/core_import_mpy_multi.py: 15433.38 14.2733 33065.45 14.2368
perf_bench/core_import_mpy_single.py: 263.00 11.3910 3858.35 12.9021
perf_bench/core_qstr.py: 4929.12 1.8434 8117.71 1.7921
perf_bench/core_yield_from.py: 16274.25 6.2584 12334.13 5.8125
perf_bench/misc_aes.py: 57425.25 5.5226 17888.60 5.7482
perf_bench/misc_mandel.py: 40809.25 8.2007 158107.00 9.8864
perf_bench/misc_pystone.py: 39821.75 6.4145 100867.62 6.5043
perf_bench/misc_raytrace.py: 36293.75 6.8501 26906.93 6.8402
perf_bench/viper_call0.py: 15573.00 14.9931 19644.99 13.1550
perf_bench/viper_call1a.py: 16725.75 9.8205 18099.96 9.2752
perf_bench/viper_call1b.py: 20752.62 8.3372 14565.60 9.0663
perf_bench/viper_call1c.py: 20849.88 5.8783 14444.80 6.6295
perf_bench/viper_call2a.py: 16156.25 11.2956 18818.59 11.7959
perf_bench/viper_call2b.py: 22047.38 8.9484 13725.73 9.6800
```

The numbers across each line are times and scores for the test:

* Runtime average (microseconds, lower is better)
* Runtime standard deviation as a percentage
* Score average (units depend on the benchmark, higher is better)
* Score standard deviation as a percentage

### Comparing performance

Usually you want to know if something is faster or slower than a reference. To
do this, copy the output of each `run-perfbench.py` run to a text file.

This can be done multiple ways, but one way on Linux/macOS is with the `tee`
utility: `./run-perfbench.py -p 168 100 | tee pyb-run1.txt`

Once you have two files with output from two different runs (maybe with
different code or configuration), compare the runtimes with `./run-perfbench.py
-t pybv-run1.txt pybv-run2.txt` or compare scores with `./run-perfbench.py -s
pybv-run1.txt pybv-run2.txt`:

```
> ./run-perfbench.py -s pyb-run1.txt pyb-run2.txt
diff of scores (higher is better)
N=168 M=100                pyb-run1.txt -> pyb-run2.txt         diff      diff% (error%)
bm_chaos.py                    352.90 ->     352.63 :      -0.27 =  -0.077% (+/-0.00%)
bm_fannkuch.py                  77.52 ->      77.45 :      -0.07 =  -0.090% (+/-0.01%)
bm_fft.py                     2516.80 ->    2519.74 :      +2.94 =  +0.117% (+/-0.00%)
bm_float.py                   5749.27 ->    5749.65 :      +0.38 =  +0.007% (+/-0.00%)
bm_hexiom.py                    42.22 ->      42.30 :      +0.08 =  +0.189% (+/-0.00%)
bm_nqueens.py                 4407.55 ->    4414.44 :      +6.89 =  +0.156% (+/-0.00%)
bm_pidigits.py                 638.09 ->     632.14 :      -5.95 =  -0.932% (+/-0.25%)
core_import_mpy_multi.py       477.74 ->     477.57 :      -0.17 =  -0.036% (+/-0.00%)
core_import_mpy_single.py       58.74 ->      58.72 :      -0.02 =  -0.034% (+/-0.00%)
core_qstr.py                    63.11 ->      63.11 :      +0.00 =  +0.000% (+/-0.01%)
core_yield_from.py             357.57 ->     357.57 :      +0.00 =  +0.000% (+/-0.00%)
misc_aes.py                    397.27 ->     396.47 :      -0.80 =  -0.201% (+/-0.00%)
misc_mandel.py                3375.70 ->    3375.84 :      +0.14 =  +0.004% (+/-0.00%)
misc_pystone.py               2265.36 ->    2265.97 :      +0.61 =  +0.027% (+/-0.01%)
misc_raytrace.py               367.61 ->     368.15 :      +0.54 =  +0.147% (+/-0.01%)
viper_call0.py                 605.92 ->     605.92 :      +0.00 =  +0.000% (+/-0.00%)
viper_call1a.py                576.78 ->     576.78 :      +0.00 =  +0.000% (+/-0.00%)
viper_call1b.py                452.45 ->     452.46 :      +0.01 =  +0.002% (+/-0.01%)
viper_call1c.py                457.39 ->     457.39 :      +0.00 =  +0.000% (+/-0.00%)
viper_call2a.py                561.37 ->     561.37 :      +0.00 =  +0.000% (+/-0.00%)
viper_call2b.py                389.49 ->     389.50 :      +0.01 =  +0.003% (+/-0.01%)
```

Note in particular the error percentages at the end of each line. If these are
high relative to the percentage difference then it indicates high variability in
the test runs, and the absolute difference value is unreliable. High error
percentages are particularly common on PC builds, where the host OS may
influence test run times. Increasing the `N` value may help average this out by
running each test longer.

## internal_bench

The `internal_bench` directory contains a set of tests for benchmarking
different internal Python features. By default, tests are run on the (unix or
Windows) host, but the `--pyboard` option allows them to be run on an attached
board instead.

Tests are grouped by the first part of the file name, and the test runner compares
output between each group of tests.

The benchmarks measure the elapsed (wall time) for each test, according
to MicroPython's own time module.

If run without any arguments, all test groups are run. Otherwise, it's possible
to manually specify which test cases to run.

Example:

```
$ ./run-internalbench.py internal_bench/bytebuf-*.py
internal_bench/bytebuf:
    0.094s (+00.00%) internal_bench/bytebuf-1-inplace.py
    0.471s (+399.24%) internal_bench/bytebuf-2-join_map_bytes.py
    0.177s (+87.78%) internal_bench/bytebuf-3-bytarray_map.py
1 tests performed (3 individual testcases)
```

## Test key/certificates

SSL/TLS tests in `multi_net` and `net_inet` use self-signed key/cert pairs
that are randomly generated to be used for testing/demonstration only.

To run tests on-device the `.der` files should be copied and the current time
set to ensure certs validity. This can be done with:
```
$ mpremote rtc --set cp multi_net/*.der net_inet/*.der :
```

### Generating new test key/certificates

The keys used for the unit tests are included in the tests folders so don't generally
need to be re-created by end users. This section is included here for reference only.

A new self-signed RSA key/cert pair can be created with openssl:
```
$ openssl req -x509 -newkey rsa:2048 -keyout rsa_key.pem -out rsa_cert.pem -days 3650 -nodes -subj '/CN=micropython.local/O=MicroPython/C=AU'
```
In this case CN is: micropython.local

Convert them to DER format:
```
$ openssl pkey -in rsa_key.pem -out rsa_key.der -outform DER
$ openssl x509 -in rsa_cert.pem -out rsa_cert.der -outform DER
```

For elliptic curve tests using key/cert pairs, create a key then a certificate using:
```
$ openssl ecparam -name prime256v1 -genkey -noout -out ec_key.pem
$ openssl pkey -in ec_key.pem -out ec_key.der -outform DER
$ openssl req -new -x509 -key ec_key.pem -out ec_cert.der -outform DER -days 3650 -nodes -subj '/CN=micropython.local/O=MicroPython/C=AU'
```
