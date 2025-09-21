# Space Football Game Makefile
# This will speed up your compilation significantly!

# Compiler settings
CXX = g++
CXXFLAGS = -IC:/msys64/ucrt64/include/SDL2
LDFLAGS = -lmingw32 -lSDL2main -lSDL2

# Project files
SOURCES = main.cpp settings.cpp utils.cpp object.cpp gameplay.cpp artist.cpp
OBJECTS = $(SOURCES:.cpp=.o)
TARGET = main.exe

# Main build target
$(TARGET): $(OBJECTS)
	@echo "Linking $(TARGET)..."
	$(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS)
	@echo "Build complete! ✓"

# Pattern rule: how to make .o files from .cpp files
%.o: %.cpp
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Individual file dependencies (optional - helps with header changes)
main.o: main.cpp main.h settings.h utils.h gameplay.h artist.h
settings.o: settings.cpp settings.h
utils.o: utils.cpp utils.h settings.h
object.o: object.cpp object.h settings.h utils.h
gameplay.o: gameplay.cpp gameplay.h object.h settings.h utils.h
artist.o: artist.cpp artist.h settings.h utils.h gameplay.h object.h

# Utility targets
.PHONY: all clean run help

# Default target
all: $(TARGET)

# Clean build files
clean:
	@echo "Cleaning build files..."
	-del *.o 2>nul
	-del $(TARGET) 2>nul
	@echo "Clean complete! ✓"

# Build and run
run: $(TARGET)
	@echo "Running $(TARGET)..."
	./$(TARGET)

# Quick rebuild (clean + build)
rebuild: clean all

# Help information
help:
	@echo "Available commands:"
	@echo "  make         - Build the game (incremental)"
	@echo "  make clean   - Remove build files"
	@echo "  make run     - Build and run the game"
	@echo "  make rebuild - Clean build from scratch"
	@echo "  make help    - Show this help"

# Show what will be built
info:
	@echo "Compiler: $(CXX)"
	@echo "Flags: $(CXXFLAGS)"
	@echo "Libraries: $(LDFLAGS)"
	@echo "Sources: $(SOURCES)"
	@echo "Target: $(TARGET)"