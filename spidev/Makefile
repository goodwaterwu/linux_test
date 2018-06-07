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

LOOP_SRC = \
	spidev_loop.c

LOOP_OBJS = \
	spidev_loop.o

NOR_SRC = \
	spidev_nor.c

NOR_OBJS = \
	spidev_nor.o

SRC = \
	$(TEST_SRC) \
	$(LOOP_SRC) \
	$(NOR_SRC)

OBJS = \
	$(TEST_OBJS) \
	$(LOOP_OBJS) \
	$(NOR_OBJS)

EXEC_FILES = \
	spidev_test \
	spidev_loop \
	spidev_nor

.PHONY: all

all: spidev_test spidev_loop spidev_nor

.PHONY: spidev_test

spidev_test: $(TEST_OBJS)
	$(CC) $(TEST_OBJS) $(CFLAGS) $(LDFLAGS) $(LIBS) -o $@
	$(STRIP) $@

.PHONY: spidev_loop

spidev_loop: $(LOOP_OBJS)
	$(CC) $(LOOP_OBJS) $(CFLAGS) $(LDFLAGS) $(LIBS) -o $@
	$(STRIP) $@

.PHONY: spidev_nor

spidev_nor: $(NOR_OBJS)
	$(CC) $(NOR_OBJS) $(CFLAGS) $(LDFLAGS) $(LIBS) -o $@
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
