# cpp/v1-soap-ingles/Makefile
# Compilar: make
# Dependencias: sudo apt install libcurl4-openssl-dev

CXX = g++
CXXFLAGS = -std=c++17 -Wall
LIBS = -lcurl

all: servidor

servidor: main.cpp
	$(CXX) $(CXXFLAGS) -o servidor main.cpp $(LIBS)

clean:
	rm -f servidor
