
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#


MODULE_IFLAGS += -I../../include -fno-tree-switch-conversion -Os

#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS = 

ROM_CSRC = aes.o	\
	   arc4.o	\
	   asn1parse.o	\
	   asn1write.o 	\
	   base64.o	\
	   bignum.o	\
	   ctr_drbg.o	\
	   des.o	\
	   dhm.o	\
	   ecdh.o	\
	   ecdsa.o	\
	   ecp.o	\
	   ecp_curves.o	\
	   hmac_drbg.o	\
	   md.o		\
	   md_wrap.o	\
	   md5.o	\
	   oid.o	\
	   pem.o	\
	   pk.o		\
	   pk_wrap.o	\
	   pkwrite.o	\
	   rsa.o	\
	   sha1.o	\
	   sha256.o	\
	   sha512.o     \
	   ../../../ssl_ram_map/rom/rom_ssl_ram_map.o

OBJS = $(ROM_CSRC)

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#

# Define the Rules to build the core targets
all: CORE_TARGETS RENAME_ROM_OBJS COPY_ROM_OBJS

	
#*****************************************************************************#
#              GENERATE OBJECT FILE 
#*****************************************************************************#
CORE_TARGETS:	$(OBJS) $(ASM_OBJS)



#*****************************************************************************#
#                        RULES TO CLEAN TARGETS                               #
#*****************************************************************************#
clean:
	$(REMOVE) *.o
	$(REMOVE) *.i
	$(REMOVE) *.s
	$(REMOVE) *.d

-include $(DEPS)
