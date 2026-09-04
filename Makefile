CC := gcc
CFLAGS := -std=c11 -Wall -Wextra -Wpedantic
AR := ar
ARFLAGS := rcs
TARGET := target/cortex
LIBRARY := target/buffer.lib
LIBRARY_OBJECT := target/buffer.o
SOURCES := src/main.c src/buffer.c

.PHONY: all lib clean

all: $(TARGET)

$(TARGET): $(SOURCES) include/buffer.h
	mkdir -p target
	$(CC) $(CFLAGS) -Iinclude $(SOURCES) -o $(TARGET)

lib: $(LIBRARY)

$(LIBRARY): $(LIBRARY_OBJECT)
	$(AR) $(ARFLAGS) $@ $^

$(LIBRARY_OBJECT): src/buffer.c include/buffer.h
	mkdir -p target
	$(CC) $(CFLAGS) -Iinclude -c $< -o $@

clean:
	rm -f $(TARGET) $(LIBRARY) $(LIBRARY_OBJECT)
