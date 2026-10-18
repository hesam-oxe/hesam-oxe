# BEHEMOTH build
CC ?= gcc
CFLAGS ?= -O2 -Wall -Wextra -Iinclude
SRC = src/vm.c src/compiler.c src/main.c
OBJ = $(SRC:.c=.o)
BIN = behemoth

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)

%.o: %.c include/beast.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(BIN)

test: $(BIN)
	@bash tests/run_tests.sh

bench: $(BIN)
	@bash tests/bench.sh

.PHONY: all clean test bench
