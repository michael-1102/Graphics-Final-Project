# Debug and Optimization Flags
DEBUG       := -g
OPT         := -O3
CXXFLAGS := -std=c++20 $(DEBUG) $(OPT)
LIBS := -L/usr/lib/x86_64-linux-gnu/ -lSDL2 -lGL -lGLEW -lglfw



CXX         := g++
COMP        := $(CXX) $(CXXFLAGS) -c

# Define object files
OBJ    := window.o

# Define target
TARGET := drawing

all: $(TARGET)

# Build drawing
drawing: $(OBJ)
	$(CXX) -o drawing $(OBJ) $(LIBS) -no-pie -fno-PIE -fno-pic

# Compile object files
window.o: window.cpp
	$(COMP) $<

# Clean rule
clean:
	rm -f $(OBJ) drawing
