CC = gcc
CFLAGS = -ggdb -std=c99 -Wall -pedantic -Wextra

OBJCOMP = $(CC) -c $(CFLAGS)

.PHONY: all tests clean

all: altitude tests

atom.o: atom.c atom.h
	$(OBJCOMP) atom.c

bitops.o: bitops.c bitops.h error.h
	$(OBJCOMP) bitops.c

tests/bitops_test: tests/bitops_test.c bitops.o error.o
	cd tests; $(OBJCOMP) bitops_test.c; cd ..
	gcc tests/bitops_test.o bitops.o error.o -o tests/bitops_test

hashtable.o: hashtable.c atom.h hashtable.h
	$(OBJCOMP) hashtable.c

tests/hash_test: tests/hash_test.c hashtable.o atom.o error.o
	cd tests; $(OBJCOMP) hash_test.c; cd ..
	gcc tests/hash_test.o hashtable.o atom.o error.o -o tests/hash_test

sexp.o: sexp.c sexp.h atom.h error.h
	$(OBJCOMP) sexp.c

tests/sexp_parse_test: tests/sexp_parse_test.c sexp.o atom.o error.o
	cd tests; $(OBJCOMP) sexp_parse_test.c; cd ..
	gcc tests/sexp_parse_test.o sexp.o atom.o error.o -o tests/sexp_parse_test


tests: tests/sexp_parse_test tests/hash_test tests/bitops_test

error.o: error.c error.h
	$(OBJCOMP) error.c

main.o: main.c
	$(OBJCOMP) main.c

bytecode.o: bytecode.c bytecode.h
	$(OBJCOMP) bytecode.c

memtypes.o: memtypes.c memtypes.h
	$(OBJCOMP) memtypes.c

interpreter.o: interpreter.c interpreter.h
	$(OBJCOMP) interpreter.c

callstack.o: callstack.c callstack.h
	$(OBJCOMP) callstack.c

altitude: main.o error.o sexp.o hashtable.o bitops.o atom.o bytecode.o memtypes.o interpreter.o callstack.o
	gcc main.o error.o sexp.o hashtable.o bitops.o atom.o bytecode.o memtypes.o interpreter.o callstack.o -o altitude

clean:
	rm *.o
	rm tests/*.o
	rm tests/*_test

