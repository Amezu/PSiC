CXX=g++
CXXFLAGS = -g -Wall -std=c++17

SRC = $(wildcard *.cpp)
OBJ = $(SRC:.cpp=.o)
OUT = $(SRC:.cpp=)

CXXFLAGS+=$(DEP_FLAGS)

all: $(OUT)

server: server.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

client: client.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

# run:
# 	./$(EXE)
# 	@if (valgrind ./$(EXE) 2>&1 | grep -q 'definitely lost: 0'); then echo 'OK'; else echo 'Wyciek!'; fi

# gdb:
# 	gdb ./$(EXE) --tui

# val:
#	 valgrind --leak-check=full ./$(EXE)
	
clean:
	rm -f *.o $(OUT)
