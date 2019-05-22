SRC_CLIENT = $(wildcard *.cpp)
OBJ_CLIENT = $(SRC_CLIENT:.cpp=.o)
LDFLAGS_CLIENT = -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio

SRC_DEMO_CLIENT = client/main_demo_client.cpp
OBJ_DEMO_CLIENT = $(SRC_DEMO_CLIENT:.cpp=.o)
LDFLAGS_DEMO_CLIENT = -lsfml-system -lsfml-network -lpthread

SRC_SERV = server/main_server.cpp
OBJ_SERV = $(SRC_SERV:.cpp=.o)
LDFLAGS_SERV = -lsfml-network -lsfml-system

CFLAGS = -Wall -Weffc++ -Werror -pedantic -g

all: ug-client ug-demo-client ug-server

ug-server: $(OBJ_SERV)
	$(CXX) $(CFLAGS) -o $@ $^ $(LDFLAGS_SERV)

ug-client: $(OBJ_CLIENT)
	$(CXX) $(CFLAGS) -o $@ $^ $(LDFLAGS_CLIENT)

ug-demo-client: $(OBJ_DEMO_CLIENT)
	$(CXX) $(CFLAGS) -o $@ $^ $(LDFLAGS_DEMO_CLIENT)

.PHONY: clean
clean:
	rm -rf *.o server/*.o *.gch server/*.gch client/*.o client/*.gch ug-client ug-demo-client ug-server
