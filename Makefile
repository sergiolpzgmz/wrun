IDIR = include
CC=gcc
CFLAGS=-I$(IDIR)

ODIR=src

_DEPS = params.h proc.h utils.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = wrun.o params.o proc.o utils.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

wrun: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o wrun