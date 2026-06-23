CXX = g++
CXXFLAGS = -std=c++17 -Wall
LIBS = -lcurl

all: servidor

servidor: main.cpp
	$(CXX) $(CXXFLAGS) -o servidor main.cpp $(LIBS)

clean:
	rm -f servidor
