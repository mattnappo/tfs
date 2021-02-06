SRC  := ./src
BIN  := ./bin
EXEC := $(BIN)/tfs.out
MAIN := ./cli.c

FLAGS   := -Wall -g
LIBS    := -lm -L/usr/local/lib -lprotobuf-c -L/usr/lib -lcrypto
INCLUDE := -I/usr/local/include

SOURCES := $(shell find $(SRC) -name '*.c')
HEADERS := $(shell find $(SRC) -name '*.h')

PROTO := ./proto/build/
PROTO_SOURCES := $(shell find $(PROTO) -name '*.c')
PROTO_HEADERS := $(shell find $(PROTO) -name '*.h')

INCLUDE_DIRS := -I$(SRC) -I.
# INCLUDE_DIRS := -I$(PROTO) -I$(SRC)

TESTS   := $(wildcard tests/*.c)
TARGETS := $(notdir $(TESTS:.c=))

all: $(TARGETS)

# $(TARGETS): $(STUFF)

test_%: ./tests/%.c $(SOURCES) $(PROTO_HEADERS)
	mkdir -p $(BIN)/tests/
	gcc $(FLAGS) -o $(BIN)/tests/$@.out $(INCLUDE_DIRS) $(SOURCES) $< $(PROTO_SOURCES) $(LIBS) $(INCLUDE)

tfs: $(SOURCES) $(HEADERS) $(PROTO_HEADERS) $(MAIN)
	mkdir -p $(BIN)
	gcc $(FLAGS) -o $(EXEC) $(INCLUDE_DIRS) $(SOURCES) $(MAIN) $(PROTO_SOURCES) $(LIBS) $(INCLUDE)

.PHONY: clean
clean:
	rm -rf bin/*

# ----- TESTS ----- #

# test_main: ./tests/test_main.c $(SOURCES) $(HEADERS) $(PROTO_HEADERS)
# 	mkdir -p $(BIN)/tests/
# 	gcc $(FLAGS) -o $(BIN)/tests/$@.out $(INCLUDE_DIRS) $(SOURCES) $< $(PROTO_SOURCES) $(LIBS) $(INCLUDE)
# 
# test_serialize: ./tests/test_serialize.c $(SOURCES) $(HEADERS) $(PROTO_HEADERS)
# 	mkdir -p $(BIN)/tests/
# 	gcc $(FLAGS) -o $(BIN)/tests/$@.out $(INCLUDE_DIRS) $(SOURCES) $< $(PROTO_SOURCES) $(LIBS) $(INCLUDE)
# 
# test_server: ./tests/test_server.c $(SOURCES) $(HEADERS) $(PROTO_HEADERS)
# 	mkdir -p $(BIN)/tests/
# 	gcc $(FLAGS) -o $(BIN)/tests/$@.out $(INCLUDE_DIRS) $(SOURCES) $< $(PROTO_SOURCES) $(LIBS) $(INCLUDE)
# 
# test_client: ./tests/test_client.c $(SOURCES) $(HEADERS) $(PROTO_HEADERS)
# 	mkdir -p $(BIN)/tests/
# 	gcc $(FLAGS) -o $(BIN)/tests/$@.out $(INCLUDE_DIRS) $(SOURCES) $< $(PROTO_SOURCES) $(LIBS) $(INCLUDE)
# 
# test_protocol: ./tests/test_protocol.c $(SOURCES) $(HEADERS) $(PROTO_HEADERS)
# 	mkdir -p $(BIN)/tests/
# 	gcc $(FLAGS) -o $(BIN)/tests/$@.out $(INCLUDE_DIRS) $(SOURCES) $< $(PROTO_SOURCES) $(LIBS) $(INCLUDE)
# 

