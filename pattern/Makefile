PREFIX ?= $(PWD)
CROSS_COMPILE ?=

CC = $(CROSS_COMPILE)gcc
CPP = $(CROSS_COMPILE)gcc -E
AR = $(CROSS_COMPILE)ar
LD = $(CROSS_COMPILE)ld
NM = $(CROSS_COMPILE)nm
STRIP = $(CROSS_COMPILE)strip

DESTDIR = $(PREFIX)/out
INCLUDES = -I$(PWD)
CFLAGS = -O2 -Wall -fPIC $(INCLUDES) -D_GNU_SOURCE
LDFLAGS =
LIBS = -pthread

PATTERN_SRC = \
	pattern_generator.c

PATTERN_OBJS = \
	pattern_generator.o

SRC = \
	$(PATTERN_SRC)

OBJS = \
	$(PATTERN_OBJS)

EXEC_FILES = \
	pattern_generator

.PHONY: all

all: pattern_generator

.PHONY: pattern_generator

pattern_generator: $(PATTERN_OBJS)
	$(CC) $(PATTERN_OBJS) $(CFLAGS) $(LDFLAGS) $(LIBS) -o $@
	$(STRIP) $@

%.o: %.c
	$(CC) -c $< $(CFLAGS) $(LDFLAGS) $(LIBS) -o $@

.PHONY: install

install: all
	mkdir -p $(DESTDIR)/bin
	cp -vf $(EXEC_FILES) $(DESTDIR)/bin

.PHONY: clean

clean:
	@rm -rf *.a *.so $(OBJS) $(EXEC_FILES) $(DESTDIR)
