# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -Werror -std=c11

# Source files
SRCS = $(wildcard Kernel/*.c)

# Object files
OBJS = $(SRCS:.c=.o)

# Executable name
EXEC = LewDos

# Default target
all: clean $(EXEC) run

# Link object files to create executable
$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Compile source files to object files
%.o: Kernel/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(OBJS) $(EXEC)

# Run the executable
run:
	./$(EXEC)

.PHONY: all clean run