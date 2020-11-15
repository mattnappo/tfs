SRC  := ./src
BIN  := ./bin
EXEC := $(BIN)/memsec.out

SOURCES := $(shell find $(SRC) -name '*.c')
HEADERS := $(shell find $(SRC) -name '*.h')

memsec: $(SOURCES) $(HEADERS)
	mkdir -p $(BIN)
	gcc -o $(EXEC) $(SOURCES)

clean:
	rm bin/*
