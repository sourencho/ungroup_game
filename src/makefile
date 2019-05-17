SRC_CLIENT = $(wildcard *.cpp)
OBJ_CLIENT = $(SRC_CLIENT:.cpp=.o)
LDFLAGS_CLIENT = -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio

SRC_SERV = server/main_server.cpp
OBJ_SERV = $(SRC_SERV:.cpp=.o)
LDFLAGS_SERV = -lsfml-network -lsfml-system

CFLAGS = -Wall -Weffc++ -Werror -pedantic -g

all: ug-client ug-server

ug-server: $(OBJ_SERV)
	$(CXX) $(CFLAGS) -o $@ $^ $(LDFLAGS_SERV)

ug-client: $(OBJ_CLIENT)
	$(CXX) $(CFLAGS) -o $@ $^ $(LDFLAGS_CLIENT)

.PHONY: clean
clean:
	rm -rf *.o server/*.o *.gch server/*.gch ug-client ug-server
