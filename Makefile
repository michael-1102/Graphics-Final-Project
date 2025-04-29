# Debug and Optimization Flags
DEBUG       := -g
OPT         := -O3
CXXFLAGS := -std=c++20 $(DEBUG) $(OPT)
LIBS := -L/usr/lib/x86_64-linux-gnu/ -lSDL2 -lGL -lGLEW -lglfw



CXX         := g++
COMP        := $(CXX) $(CXXFLAGS) -c

# Define object files
OBJ    := window.o drawing.o shader.o multishape.o multishape_2d.o styled_multishape_2d.o multishape_3d.o lit_multishape_3d.o

# Define target
TARGET := drawing

all: $(TARGET)

# Build drawing
drawing: $(OBJ)
	$(CXX) -o drawing $(OBJ) $(LIBS) -no-pie -fno-PIE -fno-pic

# Compile object files
window.o: window.cpp drw_file.hpp drawing.cpp drawing.hpp view.hpp camera.hpp
	$(COMP) $<

multishape.o: multishape.cpp multishape.hpp shader.cpp shader.hpp shape.hpp
	$(COMP) $<

multishape_2d.o: multishape_2d.cpp multishape_2d.hpp multishape.cpp multishape.hpp shader.cpp shader.hpp shape.hpp
	$(COMP) $<

styled_multishape_2d.o: styled_multishape_2d.cpp styled_multishape_2d.hpp multishape_2d.cpp multishape_2d.hpp multishape.cpp multishape.hpp shader.cpp shader.hpp shape.hpp
	$(COMP) $<

multishape_3d.o: multishape_3d.cpp multishape_3d.hpp camera.hpp shape_2d.hpp multishape.cpp multishape.hpp shader.cpp shader.hpp shape.hpp
	$(COMP) $<

lit_multishape_3d.o: lit_multishape_3d.cpp lit_multishape_3d.hpp multishape_3d.cpp multishape_3d.hpp camera.hpp lighting.hpp shape_2d.hpp multishape.cpp multishape.hpp shader.cpp shader.hpp shape.hpp
	$(COMP) $<

drawing.o: drawing.cpp drawing.hpp view.hpp
	$(COMP) $<

shader.o: shader.cpp shader.hpp
	$(COMP) $<

# Clean rule
clean:
	rm -f $(OBJ) drawing
