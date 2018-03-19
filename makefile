CXX=g++
CPPFLAGS=-Wall -IAffirm.hpp -g
all:
	$(CXX) $(CPPFLAGS) -o sample Affirm.cpp --std=c++11

clean:
	rm -rf sample
