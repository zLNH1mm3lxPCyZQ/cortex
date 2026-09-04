CC := gcc
CFLAGS := -std=c11 -Wall -Wextra -Wpedantic
TARGET := target/cortex
SOURCES := src/main.c src/buffer.c

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SOURCES) include/buffer.h
	mkdir -p target
	$(CC) $(CFLAGS) -Iinclude $(SOURCES) -o $(TARGET)

clean:
	rm -f $(TARGET)
