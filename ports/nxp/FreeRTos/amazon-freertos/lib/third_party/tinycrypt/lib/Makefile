################################################################################
#
#      Copyright (C) 2017 by Intel Corporation, All Rights Reserved.
#
# 			Cryptographic Primitives Makefile.
#
################################################################################

include ../config.mk

# Edit the OBJS content to add/remove primitives needed from TinyCrypt library:
OBJS:=aes_decrypt.o \
	aes_encrypt.o \
	cbc_mode.o \
	ctr_mode.o \
	ctr_prng.o \
	hmac.o \
	hmac_prng.o \
	sha256.o \
	ecc.o \
	ecc_dh.o \
	ecc_dsa.o \
	ccm_mode.o \
	cmac_mode.o \
	utils.o

DEPS:=$(OBJS:.o=.d)

all: libtinycrypt.a

libtinycrypt.a: $(OBJS)
	$(AR) $(ARFLAGS) $@ $^

.PHONY: clean

clean:
	-$(RM) *.exe $(OBJS) $(DEPS) *~ libtinycrypt.a

-include $(DEPS)
