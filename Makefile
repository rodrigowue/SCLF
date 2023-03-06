CXX       := g++
CXX_FLAGS := -std=c++2a -ggdb

BIN     := bin
SRC     := src
INCLUDE := include

LIBRARIES   := map.cpp transistor.cpp
EXECUTABLE  := filter


all: $(EXECUTABLE)

run: ./$(EXECUTABLE)

$(EXECUTABLE): *.cpp
	$(CXX) $(CXX_FLAGS) main.cpp $(LIBRARIES) -o $(EXECUTABLE)

clean:
	-rm $(EXECUTABLE)