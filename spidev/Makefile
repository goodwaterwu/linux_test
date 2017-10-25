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
CFLAGS = -O2 -Wall -fPIC $(INCLUDES)
LDFLAGS =
LIBS =

TEST_SRC = \
	spidev_test.c

TEST_OBJS = \
	spidev_test.o

SRC = \
	$(TEST_SRC)

OBJS = \
	$(TEST_OBJS)

EXEC_FILES = \
	spidev_test

.PHONY: all

all: spidev_test

.PHONY: spidev_test

spidev_test: $(TEST_OBJS)
	$(CC) $(TEST_OBJS) $(CFLAGS) $(LDFLAGS) $(LIBS) -o $@
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
