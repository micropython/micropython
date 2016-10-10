#
# @file pumbaa.mk
#
# @section License
# Copyright (C) 2016, Erik Moqvist
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# This file is part of the Pumbaa project.
#

INC += \
	. \
	$(PUMBAA_ROOT)/src \
	$(PUMBAA_ROOT)/src/port \
	$(PUMBAA_ROOT)/src/boards/$(BOARD)

PUMBAA_SRC += \
	boards/$(BOARD)/module_board.c \
	builtin_help.c \
	builtin_input.c \
	port/port.c \
	module_io.c \
	module_os.c \
	module_kernel.c \
	module_drivers.c \
	module_drivers/class_pin.c \
	module_kernel/class_timer.c \
	module_select.c \
	module_socket.c \
	module_sync.c \
	module_sync/class_event.c \
	module_sync/class_queue.c \
	module_sys.c \
	module_time.c

ifeq ($(BOARD),arduino_due)
PUMBAA_SRC += \
	module_drivers/class_dac.c \
	module_drivers/class_exti.c \
	module_drivers/class_spi.c \
	module_drivers/class_sd.c \
	boards/arduino_due/gccollect.c \
	boards/arduino_due/gchelper.S \
	module_thread.c \
	port/lexer_port.c
endif

ifeq ($(BOARD),linux)
PUMBAA_SRC += \
	boards/linux/gccollect.c \
	module_drivers/class_dac.c \
	module_drivers/class_exti.c \
	module_drivers/class_spi.c \
	module_drivers/class_sd.c \
	module_drivers/class_i2c_soft.c \
	module_thread.c
endif

ifeq ($(BOARD),$(filter $(BOARD), esp12e esp01))
PUMBAA_SRC += \
	mcus/esp8266/gccollect.c \
	mcus/esp8266/gchelper.S \
	module_drivers/class_exti.c \
	port/lexer_port.c
endif

ifeq ($(BOARD),photon)
PUMBAA_SRC += \
	boards/photon/gccollect.c \
	boards/photon/gchelper.S \
	module_thread.c \
	port/lexer_port.c
endif

SRC += $(PUMBAA_SRC:%=$(PUMBAA_ROOT)/src/%)

MICROPYTHON_ROOT = $(PUMBAA_ROOT)/..

INC += $(MICROPYTHON_ROOT)

MICROPYTHON_SRC += \
	extmod/modubinascii.c \
	extmod/moduhashlib.c \
	extmod/modujson.c \
	extmod/modurandom.c \
	extmod/moduzlib.c \
	lib/utils/pyhelp.c \
	lib/utils/pyexec.c \
	lib/mp-readline/readline.c \
	py/argcheck.c \
	py/asmarm.c \
	py/asmthumb.c \
	py/asmx64.c \
	py/asmx86.c \
	py/bc.c \
	py/binary.c \
	py/builtinevex.c \
	py/builtinimport.c \
	py/compile.c \
	py/emitbc.c \
	py/emitcommon.c \
	py/emitglue.c \
	py/emitinlinethumb.c \
	py/emitnative.c \
	py/formatfloat.c \
	py/frozenmod.c \
	py/gc.c \
	py/lexer.c \
	py/lexerstr.c \
	py/lexerunix.c \
	py/malloc.c \
	py/map.c \
	py/mkenv.mk \
	py/mkrules.mk \
	py/modarray.c \
	py/modbuiltins.c \
	py/modcmath.c \
	py/modcollections.c \
	py/modgc.c \
	py/modio.c \
	py/modmath.c \
	py/modmicropython.c \
	py/modstruct.c \
	py/modsys.c \
	py/modthread.c \
	py/moduerrno.c \
	py/mpprint.c \
	py/mpstate.c \
	py/mpz.c \
	py/nativeglue.c \
	py/nlrsetjmp.c \
	py/nlrthumb.c \
	py/nlrxtensa.S \
	py/objarray.c \
	py/objattrtuple.c \
	py/objbool.c \
	py/objboundmeth.c \
	py/obj.c \
	py/objcell.c \
	py/objclosure.c \
	py/objcomplex.c \
	py/objdict.c \
	py/objenumerate.c \
	py/objexcept.c \
	py/objfilter.c \
	py/objfloat.c \
	py/objfun.c \
	py/objgenerator.c \
	py/objgetitemiter.c \
	py/objint.c \
	py/objint_longlong.c \
	py/objint_mpz.c \
	py/objlist.c \
	py/objmap.c \
	py/objmodule.c \
	py/objnamedtuple.c \
	py/objnone.c \
	py/objobject.c \
	py/objpolyiter.c \
	py/objproperty.c \
	py/objrange.c \
	py/objreversed.c \
	py/objset.c \
	py/objsingleton.c \
	py/objslice.c \
	py/objstr.c \
	py/objstringio.c \
	py/objstrunicode.c \
	py/objtuple.c \
	py/objtype.c \
	py/objzip.c \
	py/opmethods.c \
	py/parse.c \
	py/parsenumbase.c \
	py/parsenum.c \
	py/qstr.c \
	py/repl.c \
	py/runtime.c \
	py/runtime_utils.c \
	py/scope.c \
	py/sequence.c \
	py/showbc.c \
	py/smallint.c \
	py/stackctrl.c \
	py/stream.c \
	py/unicode.c \
	py/vm.c \
	py/vstr.c \
	py/warning.c

ifeq ($(BOARD),$(filter $(BOARD), esp12e esp01))
MICROPYTHON_SRC += \
	lib/libm/acoshf.c \
	lib/libm/asinfacosf.c \
	lib/libm/asinhf.c \
	lib/libm/atan2f.c \
	lib/libm/atanf.c \
	lib/libm/atanhf.c \
	lib/libm/ef_rem_pio2.c \
	lib/libm/ef_sqrt.c \
	lib/libm/erf_lgamma.c \
	lib/libm/fdlibm.h \
	lib/libm/fmodf.c \
	lib/libm/kf_cos.c \
	lib/libm/kf_rem_pio2.c \
	lib/libm/kf_sin.c \
	lib/libm/kf_tan.c \
	lib/libm/libm.h \
	lib/libm/log1pf.c \
	lib/libm/math.c \
	lib/libm/roundf.c \
	lib/libm/sf_cos.c \
	lib/libm/sf_erf.c \
	lib/libm/sf_frexp.c \
	lib/libm/sf_ldexp.c \
	lib/libm/sf_modf.c \
	lib/libm/sf_sin.c \
	lib/libm/sf_tan.c \
	lib/libm/wf_lgamma.c \
	lib/libm/wf_tgamma.c
endif

SRC += $(MICROPYTHON_SRC:%=$(MICROPYTHON_ROOT)/%)

-include $(PUMBAA_ROOT)/src/boards/$(BOARD)/board.mk
