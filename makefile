SRC  := ./src
BIN  := ./bin
EXEC := $(BIN)/tfs.out

FLAGS := -Wall -g

SOURCES := $(shell find $(SRC) -name '*.c')
HEADERS := $(shell find $(SRC) -name '*.h')

tfs: $(SOURCES) $(HEADERS)
	mkdir -p $(BIN)
	gcc $(FLAGS) -o $(EXEC) $(SOURCES)

clean:
	rm bin/*
