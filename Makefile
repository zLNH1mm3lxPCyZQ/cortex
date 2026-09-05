CC := gcc
CFLAGS := -std=c11 -Wall -Wextra -Wpedantic
AR := ar
ARFLAGS := rcs
TARGET := target/cortex
TEST_TARGET := target/test
LIBRARY := target/cortex.lib
SOURCES := $(wildcard src/*.c)
HEADERS := $(wildcard include/*.h)
LIBRARY_SOURCES := $(filter-out src/main.c,$(SOURCES))
LIBRARY_OBJECTS := $(LIBRARY_SOURCES:src/%.c=target/%.o)
TEST_SOURCES := $(wildcard test/*.c) $(LIBRARY_SOURCES)
TEST_HEADERS := $(wildcard test/*.h)

.PHONY: all lib test clean

all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
	mkdir -p target
	$(CC) $(CFLAGS) -Iinclude $(SOURCES) -o $(TARGET)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

$(TEST_TARGET): $(TEST_SOURCES) $(HEADERS) $(TEST_HEADERS)
	mkdir -p target
	$(CC) $(CFLAGS) -Iinclude $(TEST_SOURCES) -o $(TEST_TARGET)

lib: $(LIBRARY)

$(LIBRARY): $(LIBRARY_OBJECTS)
	$(AR) $(ARFLAGS) $@ $^

target/%.o: src/%.c $(HEADERS)
	mkdir -p target
	$(CC) $(CFLAGS) -Iinclude -c $< -o $@

clean:
	rm -f $(TARGET) $(TEST_TARGET) $(LIBRARY) $(LIBRARY_OBJECTS)
