all:
	g++ -g -o sample Affirm.cpp --std=c++11

clean:
	rm -rf sample
