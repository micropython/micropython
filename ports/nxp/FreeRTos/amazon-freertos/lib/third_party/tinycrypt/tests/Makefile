################################################################################
#
#      Copyright (C) 2017 by Intel Corporation, All Rights Reserved.
#
# 								           Tests Makefile.
#
################################################################################

include ../config.mk

TEST_LIB_FILE:=test_ecc_utils.c
TEST_SOURCE:=$(filter-out $(TEST_LIB_FILE), $(wildcard test_*.c))

TEST_OBJECTS:=$(TEST_SOURCE:.c=.o)
TEST_DEPS:=$(TEST_SOURCE:.c=.d)
TEST_BINARY:=$(TEST_SOURCE:.c=$(DOTEXE))

# Edit the 'all' content to add/remove tests needed from TinyCrypt library:
all: $(TEST_BINARY)

clean:
	-$(RM) $(TEST_BINARY) $(TEST_OBJECTS) $(TEST_DEPS)
	-$(RM) *~ *.o *.d

# Dependencies
test_aes$(DOTEXE): test_aes.o  aes_encrypt.o aes_decrypt.o utils.o
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@

test_cbc_mode$(DOTEXE): test_cbc_mode.o cbc_mode.o \
		aes_encrypt.o aes_decrypt.o utils.o
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@

test_ctr_mode$(DOTEXE): test_ctr_mode.o ctr_mode.o \
		aes_encrypt.o utils.o
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@

test_ctr_prng$(DOTEXE): test_ctr_prng.o ctr_prng.o \
		aes_encrypt.o utils.o
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@

test_cmac_mode$(DOTEXE): test_cmac_mode.o aes_encrypt.o utils.o \
		cmac_mode.o
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@

test_ccm_mode$(DOTEXE): test_ccm_mode.o aes_encrypt.o \
		utils.o ccm_mode.o
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@

test_hmac$(DOTEXE): test_hmac.o  hmac.o sha256.o utils.o
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@

test_hmac_prng$(DOTEXE): test_hmac_prng.o hmac_prng.o hmac.o \
		sha256.o utils.o
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@

test_sha256$(DOTEXE): test_sha256.o sha256.o utils.o
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@

test_ecc_dh$(DOTEXE): test_ecc_dh.o ecc.o ecc_dh.o test_ecc_utils.o ecc_platform_specific.o
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@

test_ecc_dsa$(DOTEXE): test_ecc_dsa.o ecc.o utils.o ecc_dh.o \
		ecc_dsa.o sha256.o test_ecc_utils.o ecc_platform_specific.o
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@


-include $(TEST_DEPS)
