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

PING_SRC = \
	wdt_ping.c

PING_OBJS = \
	wdt_ping.o

TEST_SRC = \
	wdt_test.c

TEST_OBJS = \
	wdt_test.o

SRC = \
	$(PING_SRC) \
	$(TEST_SRC)

OBJS = \
	$(PING_OBJS) \
	$(TEST_OBJS)

EXEC_FILES = \
	wdt_ping \
	wdt_test

.PHONY: all

all: wdt_ping wdt_test

.PHONY: wdt_ping

wdt_ping: $(PING_OBJS)
	$(CC) $(PING_OBJS) $(CFLAGS) $(LDFLAGS) $(LIBS) -o $@
	$(STRIP) $@

.PHONY: wdt_test

wdt_test: $(TEST_OBJS)
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
