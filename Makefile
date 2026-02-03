IDIR = include
CC=gcc
CFLAGS=-I$(IDIR) -fdiagnostics-color=always -g

ODIR=src

_DEPS = params.h proc.h utils.h kill.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = wrun.o params.o proc.o utils.o kill.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

wrun: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean valgrind

clean:
	rm -f $(ODIR)/*.o wrun

valgrind: wrun
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./wrun