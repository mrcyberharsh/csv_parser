CC ?= cc
CFLAGS ?= -std=c11 -O2 -Wall -Wextra -Werror
PREFIX ?= /usr/local

.PHONY: all test clean install

all: libcsvparser.so

libcsvparser.so: src/parser.c csvparser.h
	$(CC) $(CFLAGS) -fPIC -I. -shared src/parser.c -o $@

test:
	$(CC) -std=c11 -Wall -Wextra -Werror -g -fsanitize=address,undefined -I. src/parser.c test/test_parser.c -o tests
	./tests
	$(RM) tests

install:
	PREFIX="$(PREFIX)" ./install.sh

clean:
	$(RM) libcsvparser.so parser.o tests
