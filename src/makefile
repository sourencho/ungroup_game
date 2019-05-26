ifeq '$(findstring ;,$(PATH))' ';'
	detected_OS := Windows
else
	detected_OS := $(shell uname 2>/dev/null || echo Unknown)
	detected_OS := $(patsubst CYGWIN%,Cygwin,$(detected_OS))
	detected_OS := $(patsubst MSYS%,MSYS,$(detected_OS))
	detected_OS := $(patsubst MINGW%,MSYS,$(detected_OS))
endif

ifeq ($(detected_OS),Darwin)        # Mac OS X
	CXX = clang++
	CXXFLAGS = -std=c++11 -stdlib=libc++
	LDFLAGS = -lstdc++
	CFLAGS = -Wall -Weffc++ -Werror -pedantic -g
$(info SOUREN is $(CXX))
else
	CFLAGS = -Wall -Weffc++ -Werror -pedantic -g -fsanitize=address -fno-omit-frame-pointer
endif

SRC_CLIENT = $(wildcard *.cpp)
OBJ_CLIENT = $(SRC_CLIENT:.cpp=.o)
LDFLAGS_CLIENT = -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lsfml-network -lpthread -lasan

SRC_DEMO_CLIENT = client/main_demo_client.cpp
OBJ_DEMO_CLIENT = $(SRC_DEMO_CLIENT:.cpp=.o)
LDFLAGS_DEMO_CLIENT = -lsfml-system -lsfml-network -lpthread -lasan

SRC_SERV = server/main_server.cpp
OBJ_SERV = $(SRC_SERV:.cpp=.o)
LDFLAGS_SERV = -lsfml-network -lsfml-system -lpthread -lasan

all: ug-client ug-demo-client ug-server

ug-server: $(OBJ_SERV)
	$(CXX) $(CFLAGS) -o $@ $^ $(LDFLAGS_SERV) $(LDFLAGS)

ug-client: $(OBJ_CLIENT)
	$(CXX) $(CFLAGS) -o $@ $^ $(LDFLAGS_CLIENT) $(LDFLAGS)

ug-demo-client: $(OBJ_DEMO_CLIENT)
	$(CXX) $(CFLAGS) -o $@ $^ $(LDFLAGS_DEMO_CLIENT) $(LDFLAGS)

.PHONY: clean
clean:
	rm -rf *.o server/*.o *.gch server/*.gch client/*.o client/*.gch ug-client ug-demo-client ug-server
