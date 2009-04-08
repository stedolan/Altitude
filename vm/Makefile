CC = gcc
CFLAGS = -ggdb -std=c99 -Wall

OBJCOMP = $(CC) -c $(CFLAGS)

.PHONY: all

all: atom.o error.o bitops.o bitops_test hashtable.o hash_test

atom.o: atom.c atom.h
	$(OBJCOMP) atom.c

bitops.o: bitops.c bitops.h error.h
	$(OBJCOMP) bitops.c

bitops_test: tests/bitops_test.c bitops.o error.o
	cd tests; $(OBJCOMP) bitops_test.c; cd ..
	gcc tests/bitops_test.o bitops.o error.o -o tests/bitops_test

hashtable.o: hashtable.c atom.h hashtable.h
	$(OBJCOMP) hashtable.c

hash_test: tests/hash_test.c hashtable.o atom.o
	cd tests; $(OBJCOMP) hash_test.c; cd ..
	gcc tests/hash_test.o hashtable.o atom.o -o tests/hash_test

sexp.o: sexp.c sexp.h atom.h error.h
	$(OBJCOMP) sexp.c

error.o: error.c error.h
	$(OBJCOMP) error.c
	
clean:
	rm *.o
	rm tests/*.o
	rm tests/bitops_test
	rm tests/hash_test
