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

# If HTSDIR is not set, default to system-wide htslib
ifndef HTSDIR
HTS_LIB=-lhts
HTS_INCLUDE=
else
HTS_LIB = $(HTSDIR)/libhts.a
HTS_INCLUDE = -I$(HTSDIR)/
endif

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

.c.o:
	$(CC) -o $@ -c $(CFLAGS) $(CPPFLAGS) $(HTS_INCLUDE) -fPIC $<

bri: $(C_OBJ)
	$(CC) -o $@ $(CFLAGS) $(CPPFLAGS) $(HTS_INCLUDE) -fPIC $(C_OBJ) $(HTS_LIB) $(LIBS)

.PHONY: test
test: $(TEST_PROGRAM)
	./$(TEST_PROGRAM)

.PHONY: clean
clean:
	rm -f $(PROGRAM) src/*.o
