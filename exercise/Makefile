TARGET = $(shell find . -regex ".*.c" | sed -r "s/\.c/.out/g")
CFLAGS = -O3 -flto -Wall -Wextra -Wno-unused-result -std=gnu11
LDFLAGS = -llapack -lblas -lm
CC = gcc

.PHONY: all

all: ${TARGET}

.SUFFIXES: .c .o

.c.out:
	${CC} $< ${CFLAGS} ${LDFLAGS} -o $@

.PHONY: clean

clean:
	${RM} *.out