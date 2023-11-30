CXX=g++
CXXFLAGS=-O3

hello_world: hello_world.cpp
	$(CXX) -o $@ $^ $(CXXFLAGS)