MEMZIP - a simple readonly file system

memzip takes a zip file which is comprised of uncompressed files and
and presents it as a filesystem, allowing Python files to be imported.

The script make-memzip.py takes a directory name and will create a zip file
containing uncompressed files found in the directory. It will then generate
a C file which contains the data from the zip file.

A typical addition to a makefile would look like:
```
SRC_C += \
    shared/memzip/import.c \
    shared/memzip/lexermemzip.c \
    shared/memzip/memzip.c \

OBJ += $(BUILD)/memzip-files.o

MAKE_MEMZIP = ../shared/memzip/make-memzip.py

$(BUILD)/memzip-files.c: $(shell find ${MEMZIP_DIR} -type f)
    @$(ECHO) "Creating $@"
    $(Q)$(PYTHON) $(MAKE_MEMZIP) --zip-file $(BUILD)/memzip-files.zip --c-file $@ $(MEMZIP_DIR)
```

