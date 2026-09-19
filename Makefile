IDIR = include
CC=gcc
CFLAGS = -I$(IDIR) -Wall -Wextra -pedantic -std=c99

BUILDDIR = build
SRCDIR = src
BINDIR = bin
INSTALLDIR = /usr/bin

_DEPS = kill.h params.h proc.h utils.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = kill.o params.o proc.o utils.o wrun.o
OBJ = $(patsubst %,$(BUILDDIR)/%,$(_OBJ))

wrun: $(BINDIR)/wrun

$(BUILDDIR) $(BINDIR):
	mkdir -p $@

$(BUILDDIR)/%.o: $(SRCDIR)/%.c $(DEPS) | $(BUILDDIR)
	$(CC) -c -o $@ $< $(CFLAGS)

$(BINDIR)/wrun: $(OBJ) | $(BINDIR)
	$(CC) -o $@ $^ $(CFLAGS)

valgrind: $(BINDIR)/wrun
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes $(BINDIR)/wrun

install: wrun
	cp $(BINDIR)/wrun $(INSTALLDIR)

uninstall:
	rm -f $(INSTALLDIR)/wrun

.PHONY: clean wrun valgrind install uninstall

clean:
	rm -f $(BUILDDIR)/*.o $(BINDIR)/wrun *~ core $(IDIR)/*~