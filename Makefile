#

# Sub directories containing source code, except for the main programs
SUBDIRS := src

#
# Set libraries, paths, flags and options
#

#Basic flags every build needs
LIBS = -lz
CFLAGS ?= -O3 -std=c99 -fsigned-char -D_FILE_OFFSET_BITS=64 -g
LDFLAGS ?=
CC ?= gcc
LIBS=-lpthread -lz

#HTS_DIR = /.mounts/simpsonlab/users/jsimpson/code/nanopolish-master/htslib/
HTS_DIR=htslib
HTS_LIB = $(HTS_DIR)/libhts.a
HTS_INCLUDE = -I$(HTS_DIR)/htslib

# Main programs to build
PROGRAM = bri

.PHONY: all
all: $(PROGRAM)

#
# Source files
#

# Find the source files by searching subdirectories
C_SRC := $(foreach dir, $(SUBDIRS), $(wildcard $(dir)/*.c))

# Automatically generated object names
C_OBJ = $(C_SRC:.c=.o)

bri: src/bri.c
	$(CC) -o $@ $(CFLAGS) $(CPPFLAGS) $(HTS_INCLUDE) -fPIC $< $(LIBS) $(HTS_LIB)

.PHONY: test
test: $(TEST_PROGRAM)
	./$(TEST_PROGRAM)

.PHONY: clean
clean:
	rm -f $(PROGRAM) src/*.o
