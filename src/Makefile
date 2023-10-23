# *****************************************************
# Variables to control Makefile operation
# CC=/path/to/cross-compiler/bin/gcc
# CXX=/path/to/cross-compiler/bin/g++

# CXX = clang++ -std=c++11
CXX = g++
CXXFLAGS = -g -I .

SRC := $(wildcard *.cpp)
OBJS = $(SRC:.cpp=.o)

EXEC_DIR = ./executors
EXEC_SRC := $(wildcard $(EXEC_DIR)/*.cpp)
EXEC_OBJS = $(EXEC_SRC:.cpp=.o)

# ****************************************************
# Targets needed to bring the executable up to date

all: server

server: $(OBJS) $(EXEC_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(EXEC_OBJS)

clean:
	rm -f *.o *~
	rm -f $(EXEC_DIR)/*.o $(EXEC_DIR)/*~
	rm -f server
	rm -f log

%.o: %.cpp global.h

$(EXEC_DIR)/%.o: $(EXEC_DIR)/.cpp global.h
