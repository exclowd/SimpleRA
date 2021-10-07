# *****************************************************
# Variables to control Makefile operation

CXX = g++
ifeq ($(DEBUG),1)
CXXFLAGS = -std=c++17 -ggdb3 -fsanitize=address -fsanitize=undefined -fno-sanitize-recover -Wall -Wextra -Wshadow -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -D_FORTIFY_SOURCE=2
else 
CXXFLAGS = -std=c++17 -Wall -O2
endif

VALGRIND=valgrind --tool=memcheck --leak-check=yes

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

run: server
	$(VALGRIND) ./server

clean:
	rm -f *.o *~
	rm -f $(EXEC_DIR)/*.o $(EXEC_DIR)/*~
	rm -f server
	rm -f log

%.o: %.cpp global.h

$(EXEC_DIR)/%.o: $(EXEC_DIR)/.cpp global.h
