IDIR = include
CC = gcc
CFLAGS = -I$(IDIR) -Wall -Wextra -pedantic -std=c99

SRCDIR = src
BUILDDIR = build

_DEPS = params.h proc.h utils.h kill.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = wrun.o params.o proc.o utils.o kill.o
OBJ = $(patsubst %,$(BUILDDIR)/%,$(_OBJ))

all: wrun

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

wrun: $(OBJ) | $(BUILDDIR)
	$(CC) -o $(BUILDDIR)/wrun $^ $(CFLAGS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.c | $(BUILDDIR)
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	rm -rf $(BUILDDIR)

valgrind: $(BUILDDIR)/wrun
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes $(BUILDDIR)/wrun

install: wrun
	cp $(BUILDDIR)/wrun /usr/local/bin/wrun

uninstall:
	rm -f /usr/local/bin/wrun

.PHONY: all clean valgrind install uninstall
