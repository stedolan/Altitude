CC = gcc
CFLAGS = -ggdb -std=c99 -Wall

OBJCOMP = $(CC) -c $(CFLAGS)

.PHONY: all

all: atom.o bitops.o hashtable.o hash_test sexp.o

atom.o: atom.c atom.h
	$(OBJCOMP) atom.c

bitops.o: bitops.c bitops.h
	$(OBJCOMP) bitops.c

hashtable.o: hashtable.c atom.h hashtable.h
	$(OBJCOMP) hashtable.c

hash_test: tests/hash_test.c hashtable.o atom.o
	cd tests; $(OBJCOMP) hash_test.c; cd ..
	gcc tests/hash_test.o hashtable.o atom.o -o tests/hash_test

sexp.o: sexp.c sexp.h atom.h error.h
	$(OBJCOMP) sexp.c

error.o: error.c error.h
	$(OBJCOMP) error.c
