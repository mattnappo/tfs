SRC    := src
TARGET := bin/tfs.out
MAIN   := cli.c

CC      := gcc
FLAGS   := -Wall -g
LIBS    := -lm -L/usr/local/lib -lprotobuf-c -L/usr/lib -lcrypto
INCLUDE := -I/usr/local/include

SOURCES := $(shell find src -name '*.c')
HEADERS := $(shell find src -name '*.h')

PROTO := proto/build/
PROTO_SOURCES := $(shell find $(PROTO) -name '*.c')
PROTO_HEADERS := $(shell find $(PROTO) -name '*.h')

INCLUDE_DIRS := -Isrc -I.

TEST_SRCS    := $(wildcard tests/*.c)
TEST_TARGETS := $(basename $(notdir $(TEST_SRCS)))

all: tfs tests

tfs: cli.c $(SOURCES) $(HEADERS) $(PROTO_SOURCES) $(PROTO_HEADERS)
	$(CC) $(FLAGS) -o $(TARGET) $(INCLUDE_DIRS) $(SOURCES) $< $(PROTO_SOURCES) $(LIBS) $(INCLUDE)

# ----- TESTS ----- #

tests: $(TEST_TARGETS)

$(TEST_TARGETS): $(SOURCES) $(HEADERS) $(PROTO_SOURCES) $(PROTO_HEADERS)

%: tests/%.c
	mkdir -p bin/tests
	$(CC) $(FLAGS) -o bin/tests/$@.out $(INCLUDE_DIRS) $(SOURCES) $< $(PROTO_SOURCES) $(LIBS) $(INCLUDE)

.PHONY: clean
clean:
	rm -rf bin/*

