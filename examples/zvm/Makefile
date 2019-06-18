MPTOP = ../..
CFLAGS = -std=c99 -I. -I$(MPTOP) -DNO_QSTR
LDFLAGS = -L.

test-tvm: test-tvm.o -ltvm

-ltvm:
	$(MAKE) -f $(MPTOP)/examples/zvm/Makefile.upylib MPTOP=$(MPTOP)
