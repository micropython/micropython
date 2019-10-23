# This is the default variant when you `make` the Unix port.

PROG ?= micropython

# install micropython in /usr/local/bin
TARGET = micropython
PREFIX = $(DESTDIR)/usr/local
BINDIR = $(PREFIX)/bin

install: micropython
	install -d $(BINDIR)
	install $(TARGET) $(BINDIR)/$(TARGET)

# uninstall micropython
uninstall:
	-rm $(BINDIR)/$(TARGET)
