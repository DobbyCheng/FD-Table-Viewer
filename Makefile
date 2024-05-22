CC=gcc
CFLAGS=-Wall -Werror -std=c99

# Object files for compilation
OBJ=fd_table_viewer.o proc_fd_ops.o display_utils.o utils.o

# Target executable name
TARGET=fd_table_viewer

# Default target
all: $(TARGET)

# Compile and link the program
$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

# To compile individual .c files into .o files
%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

# Help information
help:
	@echo "Available commands:"
	@echo "  all      : Compiles and links the program (default)"
	@echo "  clean    : Removes object files and the executable"
	@echo "  help     : Displays this help message"

# Rule to clean compiled files
clean:
	rm -f $(OBJ) $(TARGET)

