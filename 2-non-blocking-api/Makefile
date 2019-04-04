CXX=g++
CXXFLAGS = -g -Wall

SRC = $(wildcard *.cpp)
OBJ = $(SRC:.cpp=.o)
DEP = $(SRC:.cpp=.d)

DEP_FLAGS=-MMD -MP
CXXFLAGS+=$(DEP_FLAGS)

all: server client

server: server.o functions.o
	$(CXX) $(CXXFLAGS) $^ -o $@

client: client.o functions.o
	$(CXX) $(CXXFLAGS) $^ -o $@

# 

runs: server
	./server

runc: client
	./client 4242 "%H:%M:%S" 2 5

# 

vals: server
	valgrind --leak-check=full ./server

valc: client
	valgrind --leak-check=full ./client 4242 "%H:%M:%S" 2 5
	
# 

clean:
	rm -f *.o *.d server client

.PHONY: all runs runc vals valc clean

-include $(DEP)
