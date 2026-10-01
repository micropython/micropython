# MicroPython UEFI port — builds the .efi (clang -> PE32+, lld-link) AND provides
# the orchestration phony targets (build/stage/test/run/debug/ovmf). The actual
# compile only ever happens inside the dev container (see IN_CONTAINER below): the
# main build/run targets auto-detect and re-exec themselves there when run bare from
# the Mac, so `make ARCH=x64` (or `make docker-<target>`, or running inside a `make
# shell` session) all do the same thing. Never build directly on the host: clang
# would need the exact same cross-target support as the container's toolchain, and
# — worse — mpy-cross/build is ONE path shared by every ARCH and invocation style;
# a host-built binary landing there is silently the wrong ABI for the *next*
# in-container build (cryptic "Exec format error", since py.mk only rebuilds it when
# missing, not when it's simply wrong for the caller).
#
# ARCH selects the target: aa64 (default, native QEMU on Apple Silicon) | x64.

ARCH ?= aa64
IN_CONTAINER := $(wildcard /.dockerenv)

ifeq ($(ARCH),aa64)
  CLANG_TARGET := aarch64-unknown-windows
  LLD_MACHINE  := arm64
  EFI_NAME     := BOOTAA64.EFI
  ARCH_CFLAGS  :=
  FW_DEP       := firmware/AAVMF_CODE.fd firmware/AAVMF_VARS.fd
else ifeq ($(ARCH),x64)
  CLANG_TARGET := x86_64-unknown-windows
  LLD_MACHINE  := x64
  EFI_NAME     := BOOTX64.EFI
  ARCH_CFLAGS  := -mno-red-zone
  FW_DEP       := firmware/OVMF_CODE.fd firmware/OVMF_VARS.fd
else
  $(error Unknown ARCH '$(ARCH)' — use aa64 or x64)
endif

# Build variant: the interactive REPL (default), or — with SERIAL_REPL=1 — the REPL
# defaulting to the byte-clean serial port. Distinct build dir + define so switching
# never reuses stale objects. (Set before mkenv.mk so its `BUILD ?= build` doesn't win.)
ifeq ($(SERIAL_REPL),1)
  # REPL on the serial port by default (byte-clean raw REPL); the ConOut console shows a
  # pointer banner and `--console` forces it back. (The standard build also honours
  # `--serial` at launch, which is what the test suite uses, so this is rarely needed.)
  APP_DEF := -DMPY_UEFI_SERIAL_REPL
  BUILD := build-$(ARCH)-serial
else
  APP_DEF :=
  BUILD := build-$(ARCH)
endif

include ../../py/mkenv.mk

# Frozen modules: freeze the in-tree asyncio package (no filesystem on UEFI), plus
# individual files pulled straight from the lib/micropython-lib submodule by path
# (manifest.py's ssl freeze; the `stage` recipe below stages unittest onto the test
# VFS the same way) — so that submodule must be checked out to build, same as it
# already was to test. We don't use micropython-lib's require()/package-metadata
# mechanism though, so point MPY_LIB_DIR at the repo root (it has a README.md) to
# satisfy the frozen recipe's submodule-presence check. mpy-cross is auto-built
# via MICROPY_MPYCROSS_DEPENDENCY.
FROZEN_MANIFEST ?= manifest.py
MPY_LIB_DIR = $(TOP)

# mpy-cross must use the same MPZ_DIG_SIZE as the target so big-int constants in
# frozen .mpy modules are compatible. Our PE target triples define _WIN64, so the
# target uses 32-bit mpz digits (py/mpz.h); mpy-cross would otherwise pick 16 or 32
# from its *build host's* arch, making frozen content host-dependent. Force it to
# 32 and build it here (clearing mkrules' auto-build rule to avoid a dup target).
MICROPY_MPYCROSS_DEPENDENCY :=
$(MICROPY_MPYCROSS):
	$(MAKE) -C $(TOP)/mpy-cross CFLAGS_EXTRA="-DMPZ_DIG_SIZE=32"
$(BUILD)/frozen_content.c: $(MICROPY_MPYCROSS)

# mpy-tool emits frozen big-int constants with this digit size and guards the
# target on it; default is 16, but our target uses 32 (see above), so match it.
# Deterministic regardless of build host.
MPY_TOOL_FLAGS = -mmpz-dig-size=32

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

# Keep the build self-contained: no ROM text compression tooling for now.
MICROPY_ROM_TEXT_COMPRESSION ?= 0

# ---- TLS / ssl backend (compile-time; see README.md) ----
#   TLS=mbedtls (default) — vendor mbedtls (extmod/modtls_mbedtls.c + lib/mbedtls)
#   TLS=efi               — EFI_TLS_PROTOCOL backend (modtls_efitls.c)
#   TLS=none              — no tls/ssl module
# Set before extmod.mk so it wires MICROPY_PY_SSL / MICROPY_SSL_MBEDTLS.
TLS ?= mbedtls
ifeq ($(TLS),mbedtls)
  MICROPY_PY_SSL = 1
  MICROPY_SSL_MBEDTLS = 1
else ifeq ($(TLS),efi)
  MICROPY_PY_SSL = 1
  CFLAGS_EXTMOD += -DMICROPY_SSL_EFITLS=1   # folded into CFLAGS by extmod.mk
else ifeq ($(TLS),none)
else
  $(error Unknown TLS '$(TLS)' — use mbedtls, efi, or none)
endif

include $(TOP)/py/py.mk
include $(TOP)/extmod/extmod.mk

# Toolchain: clang cross-targets PE for both arches; lld-link emits the EFI app.
CC := clang

INC += -I.
INC += -Iinclude          # libc shims first
INC += -I$(TOP)
INC += -I$(BUILD)

CFLAGS += $(INC) -target $(CLANG_TARGET) $(ARCH_CFLAGS)
CFLAGS += -ffreestanding -fshort-wchar -fno-stack-protector -fno-stack-check
CFLAGS += -std=gnu11 -Os -DNDEBUG -Wall -Wno-unused-function $(APP_DEF) $(COPT)

LDFLAGS_EFI := -subsystem:efi_application -entry:efi_main -nodefaultlib -fixed:no \
               -machine:$(LLD_MACHINE)

SRC_C = \
	main.c \
	mphalport.c \
	uefi_time.c \
	uefi_event.c \
	machine_timer.c \
	machine_rtc.c \
	uefi_event_obj.c \
	moduefi_raw.c \
	modnetwork.c \
	modsocket.c \
	uefi_net.c \
	uefi_vfs.c \
	uefi_rng.c \
	uefi_gccollect.c \
	libc_extra.c \
	math_extra.c \
	uefi_stubs.c \
	py/ringbuf.c \
	shared/libc/string0.c \
	shared/libc/printf.c \
	shared/runtime/stdout_helpers.c \
	shared/runtime/sys_stdio_mphal.c \
	shared/runtime/pyexec.c \
	shared/runtime/mpirq.c \
	shared/readline/readline.c \
	shared/timeutils/timeutils.c \

# mbedtls platform hooks (entropy/time) — only when vendoring mbedtls.
ifeq ($(TLS),mbedtls)
SRC_C += uefi_mbedtls_port.c
endif

# EFI_TLS backend: the port-local `tls` module driving EFI_TLS_PROTOCOL
# (uefi_net.c — already in SRC_C — supplies the net_sb_* child helpers).
ifeq ($(TLS),efi)
SRC_C += modtls_efitls.c
SRC_QSTR += modtls_efitls.c
endif

# extmod modules (modtime/modos/modre/modjson/vfs/...) come in via extmod.mk below,
# gated by their MICROPY_PY_* flags. Only the port-local + shared sources are listed
# above and need explicit qstr collection here.
SRC_QSTR += shared/runtime/pyexec.c shared/readline/readline.c shared/runtime/sys_stdio_mphal.c
SRC_QSTR += shared/runtime/mpirq.c
SRC_QSTR += machine_timer.c machine_rtc.c uefi_event_obj.c moduefi_raw.c uefi_vfs.c modnetwork.c modsocket.c

# Double-precision libm (MICROPY_FLOAT_IMPL_DOUBLE): the bundled lib/libm_dbl,
# software sqrt (we exclude thumb_vfp_sqrt.c, which is ARM-thumb-only).
LIBM_SRC_C := $(addprefix lib/libm_dbl/, \
	$(notdir $(filter-out %/thumb_vfp_sqrt.c, $(wildcard $(TOP)/lib/libm_dbl/*.c))))

OBJ += $(PY_CORE_O)
OBJ += $(addprefix $(BUILD)/, $(SRC_EXTMOD_C:.c=.o)) # extmod modules (extmod.mk)
OBJ += $(addprefix $(BUILD)/, $(SRC_THIRDPARTY_C:.c=.o)) # mbedtls lib (extmod.mk, TLS=mbedtls)
OBJ += $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(LIBM_SRC_C:.c=.o))
# Frozen module bytecode (asyncio); generated from manifest.py by py/mkrules.mk.
OBJ += $(BUILD)/frozen_content.o

EFI := $(BUILD)/mpy-uefi.efi

$(EFI): $(OBJ)
	$(ECHO) "LINK $@"
	$(Q)lld-link $(LDFLAGS_EFI) $(OBJ) -out:$@
	-$(Q)llvm-size $@ 2>/dev/null || true

# ---- orchestration ----
.PHONY: all build stage test test-net test-gfx test-repl test-runfile test-bootopt run run-gfx debug ovmf test-os distclean shell

# `all`/`stage`/`build` (build is `stage`'s alias — README's documented entry point)
# only ever run for real inside the dev container (see IN_CONTAINER, top of file).
# Bare on the host they re-exec themselves there via docker instead of trying to
# compile locally — see the file header for why that matters. `run`/`debug` do the
# same, but need `-it` for the interactive REPL that lands on your terminal.
ifeq ($(IN_CONTAINER),)
all stage build:
	docker run --rm --user "$$(id -u):$$(id -g)" -e HOME=/tmp \
		-v $(REPO):/work -w /work/ports/uefi $(DEV_IMG) make $@ ARCH=$(ARCH)
run debug:
	docker run --rm -it --user "$$(id -u):$$(id -g)" -e HOME=/tmp \
		-v $(REPO):/work -w /work/ports/uefi $(DEV_IMG) make $@ ARCH=$(ARCH)
else
all: $(EFI)

build: stage

# Stage the built .efi into the virtual FAT ESP at the firmware's auto-launch path.
stage: $(EFI) tests/tls_test_cert.der
	@mkdir -p esp/EFI/BOOT
	$(Q)cp $(EFI) esp/EFI/BOOT/$(EFI_NAME)
	@printf 'hello from efi fs' > esp/hello.txt
	@printf 'VALUE = 42\n' > esp/fsmod.py
	@# A precompiled .mpy, to exercise runtime bytecode import off the VFS.
	@printf 'def answer():\n    return 6 * 7\nWHO = "mpy-load"\n' > $(BUILD)/mpytest.py
	$(Q)$(MICROPY_MPYCROSS) -o esp/mpytest.mpy $(BUILD)/mpytest.py
	@# Example scripts, mounted at /samples in the REPL (see examples/uefi/README.md).
	@mkdir -p esp/samples
	$(Q)cp $(TOP)/examples/uefi/*.py esp/samples/
	@# The CA the network TLS self-test verifies against (DER: both backends parse it).
	$(Q)cp tests/tls_test_cert.der esp/tls_test_cert.der
	@# unittest lives on the boot volume (imported via sys.path's "" cwd entry) rather
	@# than being frozen into a special build — so the tested firmware is the standard one.
	@mkdir -p esp/unittest
	$(Q)cp $(TOP)/lib/micropython-lib/python-stdlib/unittest/unittest/__init__.py esp/unittest/__init__.py
	@# Default startup.nsh: auto-launch our app when the firmware runs the Shell. AAVMF
	@# (aa64) drops into the Shell and runs this; OVMF (x64) auto-boots the removable-media
	@# app directly instead. `make test` and the harness modes stage their own startup.nsh
	@# during a run (this is what `make run`/`run-gfx` use).
	@printf '@echo -off\r\nFS0:\r\n\\EFI\\BOOT\\$(EFI_NAME)\r\n' > esp/startup.nsh
	$(ECHO) "staged $(EFI) -> esp/EFI/BOOT/$(EFI_NAME)"

run: stage $(FW_DEP)
	$(Q)./scripts/run.sh $(ARCH)
debug: stage $(FW_DEP)
	$(Q)./scripts/debug.sh $(ARCH)
endif

# TLS test cert/key/DER for the network self-test — generated on demand (git-ignored).
# The .der target also produces the .pem cert + key the harness's TLS peer uses.
tests/tls_test_cert.der:
	$(Q)bash tests/gen-tls-test-cert.sh

# firmware: seed ./firmware from the image-baked /opt/ovmf if not present
firmware/%.fd:
	@mkdir -p firmware
	@if [ -d /opt/ovmf ]; then cp -n /opt/ovmf/*.fd firmware/ 2>/dev/null || true; fi
	@test -f $@ || { echo "Missing $@ — run 'make ovmf' (host) to build firmware"; exit 1; }

# `make test`: run the upstream, unmodified tests/run-tests.py (which includes the
# port's own tests/ports/uefi checks) against QEMU on the host's NATIVE architecture —
# aa64 on an arm64 host, x64 on x86-64 — so QEMU runs the same ISA and is fast (no
# cross-emulation). Builds+stages the app in the container first, then drives QEMU
# natively on the host.
#
# `-t uefi:exec:"<engine>"` is the ONLY uefi-specific thing here: tests/uefi_target.py
# is a thin device driver (bring QEMU up, then exec into it so its stdin/stdout become
# exactly what run-tests.py's pyboard.py is talking to — see that file and main.c for
# why an unmodified pyboard.py can drive this port directly). The "uefi:" prefix
# selects this port's longer initial-connection timeout in tests/test_utils.py
# (_TARGET_CONNECT_TIMEOUT_S) — a cold VM+firmware boot needs more than the 10s default
# real hardware/ports/qemu, whose device is already sitting idle, ever need.
#
# This is the only command anyone runs: never invoke tests/uefi_target.py directly.
# Needs the RELEASE firmware (tools/build-ovmf.sh --release) and pexpect (a .venv is
# used automatically if present). Extra tests/flags via TESTS=, e.g.
# `make test TESTS="ports/uefi/efi_raw.py"` — passed straight through to run-tests.py.
NATIVE_ARCH := $(shell uname -m | sed -e 's/^arm64$$/aa64/' -e 's/^aarch64$$/aa64/' \
                                      -e 's/^x86_64$$/x64/' -e 's/^amd64$$/x64/')
# Host Python for the engine (needs pexpect); a local .venv wins. Distinct from
# mkenv.mk's own PYTHON (used for the build). Override with TEST_PYTHON=... if needed.
TEST_PYTHON ?= $(shell test -x .venv/bin/python && echo .venv/bin/python || echo python3)
# Absolute paths: the exec: command below runs with whatever cwd run-tests.py has
# (tests/, since that's where it lives), not ports/uefi/, so a relative .venv path
# would resolve wrong. A bare command name (no "/") still resolves via $PATH either way.
ENGINE := $(abspath tests/uefi_target.py)
ifneq ($(findstring /,$(TEST_PYTHON)),)
TEST_PYTHON_ABS := $(abspath $(TEST_PYTHON))
else
TEST_PYTHON_ABS := $(TEST_PYTHON)
endif
test:
	$(MAKE) docker-stage ARCH=$(NATIVE_ARCH)
	$(Q)cd $(TOP)/tests && UEFI_TEST_ARCH=$(NATIVE_ARCH) ./run-tests.py \
		-t 'uefi:exec:$(TEST_PYTHON_ABS) $(ENGINE)' $(TESTS)

# `make test-gfx` / `test-net`: the same suite with a headless display adapter
# (GOP/Display tests run) or a NIC + network firmware (network tests run) — set via
# env var, since tests/uefi_target.py takes no arguments of its own (see that file).
test-gfx:
	$(Q)UEFI_TEST_GRAPHICS=1 $(MAKE) --no-print-directory test TESTS="$(TESTS)"
test-net:
	$(Q)UEFI_TEST_NET=1 $(MAKE) --no-print-directory test TESTS="$(TESTS)"

# Headless REPL smoke test: pipes Python in over serial and asserts on the output.
test-repl: stage $(FW_DEP)
	$(Q)$(TEST_PYTHON) tests/harness.py --arch $(ARCH) --mode repl

# Launch from the shell with a script argument (`app.efi foo.py ...`) and assert it
# runs with sys.argv set. Uses the shell-launch (startup.nsh) path — aa64.
test-runfile: stage $(FW_DEP)
	$(Q)$(TEST_PYTHON) tests/harness.py --arch $(ARCH) --mode runfile

# Launch via a boot-manager entry (BootNext) whose OptionalData is a script argument,
# and assert it runs with sys.argv — the LoadedImage->LoadOptions path (distinct from
# the shell's ShellParameters). Two-phase (write entry, then fresh boot) — aa64.
test-bootopt: stage $(FW_DEP)
	$(Q)$(TEST_PYTHON) tests/harness.py --arch $(ARCH) --mode bootopt

# `make run` drops you into a live, interactive MicroPython REPL in QEMU; `make debug`
# boots with a gdbstub. Both put the serial console on your terminal. See IN_CONTAINER
# (top of file) for the host auto-wrap that makes both work the same from the Mac,
# inside `make shell`, or in CI.

# Visible graphics window so GOP is on a usable screen. Host-only: a cocoa window
# can't come from the Linux container, so this runs host QEMU directly (no docker
# wrap) and needs a prior staged build — `make docker-stage ARCH=$(ARCH)`.
run-gfx:
	$(Q)./scripts/run-gfx.sh $(ARCH)

ovmf:
	$(Q)./tools/build-ovmf.sh

# Build the minimal aarch64 test OS (EFI-stub kernel + BusyBox initramfs) into
# test-images/, for exercising the boot path (examples/uefi/boot_disk.py, net_boot.py).
# Needs apt/busybox, so run it in the dev container: `make docker-test-os`.
test-os:
	$(Q)bash tools/build-test-os.sh

distclean: clean
	$(RM) -rf build-aa64 build-x64 esp firmware/serial.log firmware/debug.log

# ---- run any target inside the dev container (from the Mac) ----
# e.g. `make docker-test ARCH=aa64`, `make shell`. --user matches the auto-wrap above
# (and CI): keeps everything the container writes into the host-mounted tree owned by
# you, not root.
REPO := $(abspath $(TOP))
DEV_IMG ?= mpy-uefi-dev
docker-%:
	docker run --rm --user "$$(id -u):$$(id -g)" -e HOME=/tmp \
		-v $(REPO):/work -w /work/ports/uefi $(DEV_IMG) make $* ARCH=$(ARCH)
shell:
	docker run --rm -it --user "$$(id -u):$$(id -g)" -e HOME=/tmp \
		-v $(REPO):/work -w /work/ports/uefi $(DEV_IMG) bash

include $(TOP)/py/mkrules.mk
