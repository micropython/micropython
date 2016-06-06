all:
	@echo
	@echo 'Use the following sequence:'
	@echo
	@echo '  make build    -  build micropython unix port with libfuzzer instrumentation'
	@echo '  make prepare  -  generate test cases and set up folders for the fuzzer'
	@echo '  make run      -  run the fuzzer'
	@echo
	@echo 'Run "make clean" to really clean up - this also deletes the test cases and crashes!'
	@echo

build:
	# setup
	git submodule update --init
	make -C ../../unix deplibs

	# get + build libfuzzer
	-git clone https://chromium.googlesource.com/chromium/llvm-project/llvm/lib/Fuzzer
	clang++ -c -g -O2 -std=c++11 Fuzzer/*.cpp -IFuzzer
	ar ruv libFuzzer.a Fuzzer*.o
	rm Fuzzer*.o
	
	# build micropython and pyexec.c + readline.c
	# clang fails at pyexec.c for some reason (just comment out the offending line manually for now)

	# it also will fail to build the actual target, since main() in main.c has to be renamed temporarily
	# not linking main.o is not an option, as stuff outside of main() in main.c is used in other files
	sed -i 's/main(/main2(/g' ../../unix/main.c

	# UBSAN and ASAN do NOT like micropython's way to handle memory in their default configuration at all!
	# Still need to investigate what to disable to still get useful output
	#make -C ../../unix CC=clang LIB_SRC_C_EXTRA="utils/pyexec.c mp-readline/readline.c" COPT="-fsanitize=address -fsanitize=undefined -fno-sanitize-recover=undefined -fsanitize-coverage=edge,indirect-calls,8bit-counters,trace-cmp" LDFLAGS_EXTRA="-fsanitize=address -fsanitize=undefined -fno-sanitize-recover=undefined -fsanitize-coverage=edge,indirect-calls,8bit-counters,trace-cmp"
	
	# use only LSAN for now
	-make -C ../../unix CC=clang LIB_SRC_C_EXTRA="utils/pyexec.c mp-readline/readline.c" COPT="-fsanitize=leak -fsanitize-coverage=edge,indirect-calls,8bit-counters,trace-cmp" LDFLAGS_EXTRA="-fsanitize=leak -fsanitize-coverage=edge,indirect-calls,8bit-counters,trace-cmp"

	# revert the edits:
	sed -i 's/main2(/main(/g' ../../unix/main.c
	
	# build fuzzer
	#clang -c python_fuzzer.cc -g -fsanitize=address -fsanitize=undefined -fno-sanitize-recover=undefined -fsanitize-coverage=edge,indirect-calls,8bit-counters,trace-cmp \
	clang -c python_fuzzer.cc -g -fsanitize=leak -fsanitize-coverage=edge,indirect-calls,8bit-counters,trace-cmp \
	-I ../../ \
	-I ../../unix \
	-I ../../unix/build

	# link
	# (ideally code from main.c won't be needed in the future any more, so no need to link main.o)
	#clang++ -g -o python_fuzzer -fsanitize=address -fsanitize=undefined -fno-sanitize-recover=undefined libFuzzer.a python_fuzzer.o \
	clang++ -g -o python_fuzzer -fsanitize=leak libFuzzer.a python_fuzzer.o \
	../../unix/build/alloc.o \
	../../unix/build/coverage.o \
	../../unix/build/fatfs_port.o \
	../../unix/build/file.o \
	../../unix/build/gccollect.o \
	../../unix/build/input.o \
	../../unix/build/main.o \
	../../unix/build/mod*.o \
	../../unix/build/unix_mphal.o \
	../../unix/build/build/*.o \
	../../unix/build/extmod/*.o \
	../../unix/build/lib/fatfs/*.o \
	../../unix/build/lib/fatfs/option/*.o \
	../../unix/build/lib/mp-readline/*.o \
	../../unix/build/lib/timeutils/*.o \
	../../unix/build/lib/utils/*.o \
	../../unix/build/py/*.o \
	../../lib/libffi/build_dir/out/lib/libffi.a \
	../../lib/axtls/ssl/*.o \
	../../lib/axtls/crypto/*.o

prepare:
	mkdir -p corpus/
	mkdir -p crashes/
	mkdir -p tests/
	mkdir -p manual_tests/

	# Bootstrap python syntax from an emoji of a pile of poo.
	# See http://lcamtuf.blogspot.co.at/2014/11/pulling-jpegs-out-of-thin-air.html
	echo -e "\xf0\x9f\x92\xa9" > ./manual_tests/poo.py
	echo "print('Hello World!')" > ./manual_tests/hello_world.py
	echo "import math\n1 + 1 == 3" > ./manual_tests/math.py
	# maybe also use existing tests as seed here?
	# some do crash though...
	find ../../tests -name \*.py -exec cp {} ./tests/ \;

run:
	# max. 3 seconds for one input
	# consider it "slow" if it takes more than 2 seconds
	# use experimental trace feature
	# up to 128 results (uses half of all CPU cores available by default and will spawn a new worker once a timeout or bug was found)
	# only generate ascii files
	# write "artifacts" aka crashes to the ./crashes directory
	# disable stdout for fuzzers, as it will only spam the console/logs (especially once they discover multiplication on strings...)
	# load test cases from ./corpus and ./manual_tests
	./python_fuzzer -timeout=3 -report_slow_units=2 -use_traces=1 -jobs=2048 -only_ascii=1 -artifact_prefix=./crashes/ -close_fd_mask=1 ./corpus/ ./manual_tests/

	# for later...
	#./python_fuzzer -timeout=3 -report_slow_units=1 -use_traces=1 -jobs=128 -only_ascii=1 -artifact_prefix=./crashes/ -close_fd_mask=1 ./corpus/ ./manual_tests/ ./tests/

coverage:
	mkdir -p cov/
	# only one single run of all test cases, with higher timeouts
	# needs sancov from llvm 3.9 for html-report apparently
	LSAN_OPTIONS="coverage=1:html_cov_report=1:coverage_dir=./cov/" ./python_fuzzer -timeout=10 -report_slow_units=5 -artifact_prefix=./crashes/ -close_fd_mask=1 -runs=0 ./corpus/ ./manual_tests/
	sancov -not-covered-functions ./cov/python_fuzzer.*.sancov -obj=python_fuzzer
	sancov -covered-functions ./cov/python_fuzzer.*.sancov -obj=python_fuzzer

consolidate:
	mv ./corpus ./corpus_old
	mkdir -p ./corpus
	# merges all relevant tests (using higher timeouts to catch relatively slow ones too) into the first folder
	./python_fuzzer -timeout=10 -report_slow_units=5 -artifact_prefix=./crashes/ -close_fd_mask=1 -runs=0 -merge=1 ./corpus/ ./corpus_old/ ./manual_tests/
	#./python_fuzzer -timeout=10 -report_slow_units=5 -artifact_prefix=./crashes/ -close_fd_mask=1 -runs=0 -merge=1 ./corpus/ ./corpus_old/ ./manual_tests/ ./tests/
	rm -rf ./corpus_old

clean:
	rm -rf *.log libFuzzer.a python_fuzzer.o python_fuzzer ./corpus ./cov ./crashes ./Fuzzer ./manual_tests ./tests
