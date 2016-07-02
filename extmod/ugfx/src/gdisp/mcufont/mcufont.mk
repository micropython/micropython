# Makefile fragment listing the source files for the mcufont decoder.

# Directory where the decoder source code recides
# Usually you'll want to set this in your own Makefile.
MFDIR ?= mcufont/decoder

# Name of the include directory
MFINC = $(MFDIR)

# Source code files to include
MFSRC = \
    $(MFDIR)/mf_encoding.c \
    $(MFDIR)/mf_font.c \
    $(MFDIR)/mf_justify.c \
    $(MFDIR)/mf_kerning.c \
    $(MFDIR)/mf_rlefont.c \
    $(MFDIR)/mf_bwfont.c \
    $(MFDIR)/mf_scaledfont.c \
    $(MFDIR)/mf_wordwrap.c
