SRC  := ./src
BIN  := ./bin
EXEC := $(BIN)/tfs.out

FLAGS := -Wall -g -Isrc
LIBS := -lm

SOURCES := $(shell find $(SRC) -name '*.c')
HEADERS := $(shell find $(SRC) -name '*.h')

tfs: $(SOURCES) $(HEADERS)
	mkdir -p $(BIN)
	gcc $(FLAGS) -o $(EXEC) -Isrc $(SOURCES) $(LIBS)

# TESTS
test_ftable: ./src/util.c ./src/ftable.c ./tests/ftable.c
	mkdir -p $(BIN)/tests/
	gcc -o $(BIN)/tests/$@.out $(FLAGS) $^ $(LIBS)

clean:
	rm -rf bin/*
