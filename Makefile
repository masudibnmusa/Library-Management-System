# Makefile for Library Management System

# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -O2 -std=c99

# Debug flags
DEBUGFLAGS = -g -DDEBUG

# Target executable
TARGET = library

# Source files
SOURCES = library_management.c

# Object files
OBJECTS = $(SOURCES:.c=.o)

# Default target
all: $(TARGET)

# Build the executable
$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES)
	@echo "Build complete! Run with: ./$(TARGET)"

# Debug build
debug: CFLAGS += $(DEBUGFLAGS)
debug: clean $(TARGET)
	@echo "Debug build complete!"

# Compile object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -f $(TARGET) $(OBJECTS)
	@echo "Cleaned build files"

# Clean all generated files (including data)
cleanall: clean
	rm -f library.dat users.dat library.log library_backup_*.dat library_export*.txt
	@echo "Cleaned all generated files"

# Run the program
run: $(TARGET)
	./$(TARGET)

# Install (copy to /usr/local/bin on Unix-like systems)
install: $(TARGET)
	install -m 755 $(TARGET) /usr/local/bin/
	@echo "Installed to /usr/local/bin/"

# Uninstall
uninstall:
	rm -f /usr/local/bin/$(TARGET)
	@echo "Uninstalled from /usr/local/bin/"

# Check for memory leaks (requires valgrind)
memcheck: debug
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(TARGET)

# Static analysis (requires cppcheck)
check:
	cppcheck --enable=all --suppress=missingIncludeSystem $(SOURCES)

# Help
help:
	@echo "Library Management System - Makefile"
	@echo ""
	@echo "Available targets:"
	@echo "  all       - Build the program (default)"
	@echo "  debug     - Build with debug symbols"
	@echo "  clean     - Remove build files"
	@echo "  cleanall  - Remove build and data files"
	@echo "  run       - Build and run the program"
	@echo "  install   - Install to /usr/local/bin"
	@echo "  uninstall - Remove from /usr/local/bin"
	@echo "  memcheck  - Run with valgrind memory checker"
	@echo "  check     - Run static analysis with cppcheck"
	@echo "  help      - Show this help message"

.PHONY: all debug clean cleanall run install uninstall memcheck check help
