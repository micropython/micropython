# MicroPython UEFI port — builds the .efi (clang -> PE32+, lld-link) AND provides
# the orchestration phony targets (test/run/debug/ovmf). Runs inside the dev
# container; from the Mac use `make docker-<target>` (bottom of file).
#
# ARCH selects the target: aa64 (default, native QEMU on Apple Silicon) | x64.

ARCH ?= aa64

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

# `make test` builds a headless self-test variant; every other goal builds the
# interactive REPL. Distinct build dir + define so switching never reuses stale
# objects. (Set before mkenv.mk so its `BUILD ?= build` doesn't win.)
ifneq ($(filter test test-net test-gfx,$(MAKECMDGOALS)),)
  APP_DEF := -DMPY_UEFI_SELFTEST
  BUILD := build-$(ARCH)-selftest
else
  APP_DEF :=
  BUILD := build-$(ARCH)
endif

include ../../py/mkenv.mk

# Frozen modules: freeze the in-tree asyncio package (no filesystem on UEFI). We
# don't use micropython-lib, so point MPY_LIB_DIR at the repo root (it has a
# README.md) to satisfy the frozen recipe's submodule check without initialising
# that submodule. mpy-cross is auto-built via MICROPY_MPYCROSS_DEPENDENCY.
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
	modefi.c \
	moduefi_raw.c \
	modnetwork_uefi.c \
	modsocket_uefi.c \
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
SRC_QSTR += machine_timer.c modefi.c moduefi_raw.c uefi_vfs.c modnetwork_uefi.c modsocket_uefi.c

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

all: $(EFI)

$(EFI): $(OBJ)
	$(ECHO) "LINK $@"
	$(Q)lld-link $(LDFLAGS_EFI) $(OBJ) -out:$@
	-$(Q)llvm-size $@ 2>/dev/null || true

# ---- orchestration ----
.PHONY: stage test test-tls test-net test-gfx test-repl test-runfile run run-gfx debug ovmf test-os distclean shell

# Stage the built .efi into the virtual FAT ESP at the firmware's auto-launch path.
stage: $(EFI) tests/tls_test_cert.der
	@mkdir -p esp/EFI/BOOT
	$(Q)cp $(EFI) esp/EFI/BOOT/$(EFI_NAME)
	@printf 'hello from efi fs' > esp/hello.txt
	@printf 'VALUE = 42\n' > esp/fsmod.py
	@# A precompiled .mpy, to exercise runtime bytecode import off the VFS.
	@printf 'def answer():\n    return 6 * 7\nWHO = "mpy-load"\n' > $(BUILD)/mpytest.py
	$(Q)$(MICROPY_MPYCROSS) -o esp/mpytest.mpy $(BUILD)/mpytest.py
	@# Sample scripts, mounted at /samples in the REPL (see samples/README.md).
	@mkdir -p esp/samples
	$(Q)cp samples/*.py esp/samples/
	@# The CA the network TLS self-test verifies against (DER: both backends parse it).
	$(Q)cp tests/tls_test_cert.der esp/tls_test_cert.der
	@# Auto-launch our app if BDS drops into the UEFI Shell. This happens when a
	@# display adapter is attached (make test-gfx / run-gfx): AAVMF then prefers
	@# its boot-manager/Shell flow over the removable-media boot option. Dormant on
	@# the headless path, which boots the app directly and never enters the Shell.
	@printf '@echo -off\r\nFS0:\r\n\\EFI\\BOOT\\$(EFI_NAME)\r\n' > esp/startup.nsh
	$(ECHO) "staged $(EFI) -> esp/EFI/BOOT/$(EFI_NAME)"

# TLS test cert/key/DER for the network self-test — generated on demand (git-ignored).
# The .der target also produces the .pem cert + key the harness's TLS peer uses.
tests/tls_test_cert.der:
	$(Q)bash tests/gen-tls-test-cert.sh

# firmware: seed ./firmware from the image-baked /opt/ovmf if not present
firmware/%.fd:
	@mkdir -p firmware
	@if [ -d /opt/ovmf ]; then cp -n /opt/ovmf/*.fd firmware/ 2>/dev/null || true; fi
	@test -f $@ || { echo "Missing $@ — run 'make ovmf' (host) to build firmware"; exit 1; }

test: stage $(FW_DEP)
	$(Q)python3 tests/harness.py --arch $(ARCH) --mode selftest

# TLS parity gate: run the selftest under each TLS backend in turn. The selftest build
# dir is shared per-arch and its generated qstrs differ per backend, so it must be
# removed between backends (a plain `clean` targets the non-selftest dir). Run for
# ARCH=aa64 and x64.
TLS_BACKENDS ?= mbedtls efi
test-tls: $(FW_DEP)
	@for b in $(TLS_BACKENDS); do \
	  echo "===== TLS=$$b ARCH=$(ARCH) ====="; \
	  rm -rf build-$(ARCH)-selftest; \
	  $(MAKE) --no-print-directory test TLS=$$b ARCH=$(ARCH) || exit 1; \
	done
	@# Remove the last backend's selftest build so a later default-TLS `make test`
	@# rebuilds cleanly rather than reusing this dir (backends can't share it).
	$(Q)rm -rf build-$(ARCH)-selftest
	@echo "test-tls: all backends [$(TLS_BACKENDS)] passed on $(ARCH)"

# Like `test`, but on the network firmware (firmware/net, built by
# docker/build-ovmf-net.sh) with a virtio-net NIC on SLIRP user-net — so the
# self-test's network phases (DHCP lease, DNS, sockets) run end-to-end. virtio-rng
# is added because the EDK2 upper network stack has a DEPEX on the RNG protocol.
test-net: stage
	@test -f firmware/net/$(if $(filter x64,$(ARCH)),OVMF,AAVMF)_CODE.fd \
	  || { echo "Missing firmware/net — run 'bash docker/build-ovmf-net.sh' first"; exit 1; }
	$(Q)python3 tests/harness.py --arch $(ARCH) --mode selftest --net

# Like `test`, but with a display adapter attached (headless: no host window) so
# the GOP path is exercised on both arches — on aa64 `virt` GOP is otherwise
# absent. Stays hermetic, so it runs in the container like `test`.
test-gfx: stage $(FW_DEP)
	$(Q)python3 tests/harness.py --arch $(ARCH) --mode selftest --graphics headless

# Headless REPL smoke test: pipes Python in over serial and asserts on the output.
test-repl: stage $(FW_DEP)
	$(Q)python3 tests/harness.py --arch $(ARCH) --mode repl

# Launch from the shell with a script argument (`app.efi foo.py ...`) and assert it
# runs with sys.argv set. Uses the shell-launch (startup.nsh) path — aa64.
test-runfile: stage $(FW_DEP)
	$(Q)python3 tests/harness.py --arch $(ARCH) --mode runfile

# `make run` drops you into a live, interactive MicroPython REPL in QEMU;
# `make debug` boots with a gdbstub. Both put the serial console on your terminal.
# From the host they auto-wrap in an interactive (-it) container; inside the
# container they exec QEMU directly. So `make run` works the same everywhere.
IN_CONTAINER := $(wildcard /.dockerenv)
ifeq ($(IN_CONTAINER),)
run debug:
	docker run --rm -it -v $(REPO):/work -w /work/ports/uefi $(DEV_IMG) make $@ ARCH=$(ARCH)
else
run: stage $(FW_DEP)
	$(Q)./scripts/run.sh $(ARCH)
debug: stage $(FW_DEP)
	$(Q)./scripts/debug.sh $(ARCH)
endif

# Visible graphics window so GOP is on a usable screen. Host-only: a cocoa window
# can't come from the Linux container, so this runs host QEMU directly (no docker
# wrap) and needs a prior staged build — `make docker-stage ARCH=$(ARCH)`.
run-gfx:
	$(Q)./scripts/run-gfx.sh $(ARCH)

ovmf:
	$(Q)./docker/build-ovmf.sh

# Build the minimal aarch64 test OS (EFI-stub kernel + BusyBox initramfs) into
# test-images/, for exercising the boot path (samples/boot_disk.py, net_boot.py).
# Needs apt/busybox, so run it in the dev container: `make docker-test-os`.
test-os:
	$(Q)bash test-images/build-test-os.sh

distclean: clean
	$(RM) -rf build-aa64 build-x64 esp firmware/serial.log firmware/debug.log

# ---- run any target inside the dev container (from the Mac) ----
# e.g. `make docker-test ARCH=aa64`, `make shell`
REPO := $(abspath $(TOP))
DEV_IMG ?= mpy-uefi-dev
docker-%:
	docker run --rm -v $(REPO):/work -w /work/ports/uefi $(DEV_IMG) make $* ARCH=$(ARCH)
shell:
	docker run --rm -it -v $(REPO):/work -w /work/ports/uefi $(DEV_IMG) bash

include $(TOP)/py/mkrules.mk
