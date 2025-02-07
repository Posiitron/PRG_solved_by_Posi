# Makefile to compile all .c files in the current directory using -std=gnu99

# Compiler to use
CC = gcc

# Compiler flags
CFLAGS = -std=gnu99 -Wall -Wextra -O2

# Executable name
TARGET = my_program

# Get a list of all .c files in the current directory
SRCS = $(wildcard *.c)

# Generate a list of .o files from the .c files
OBJS = $(SRCS:.c=.o)

# Default target: build the executable
all: $(TARGET)

# Rule to build the executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Rule to build .o files from .c files
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Clean up generated files
clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
