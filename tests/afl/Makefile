all:
	@echo
	@echo 'Use the following sequence:'
	@echo
	@echo '  make build    -  build micropython unix port with afl instrumentation and mpy-cross'
	@echo '  make prepare  -  pre-compile tests using mpy-cross'
	@echo '  make params   -  set optimal kernel parameters for testing (this one has to be run as root!)'
	@echo '  make run      -  run american fuzzy lop'

build:
	make -C ../../mpy-cross
	make -C ../../unix CC=afl-gcc

prepare:
	find ../basics -name '*.py' -exec ../../mpy-cross/mpy-cross -mcache-lookup-bc {} \;
	mkdir tests/
	mv ../basics/*.mpy tests/

# must be run as root
params:
	echo core > /proc/sys/kernel/core_pattern
	echo performance | tee /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor

run:
	afl-fuzz -i tests -o fuzz -f afltest.mpy ../../unix/micropython -c 'import afltest'

clean:
	rm -rf fuzz/ tests/
