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
	LDFLAGS = -lasan
endif

CMN_DIR = ./common

CLIENT_LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lsfml-network -lpthread
CLIENT_SRC_DIR = ./client
CLIENT_SRC_LIST = $(wildcard $(CMN_DIR)/*.cpp) $(wildcard $(CLIENT_SRC_DIR)/*.cpp)
CLIENT_OBJ_LIST = $(CLIENT_SRC_LIST:.cpp=.o)

SERVER_LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lsfml-network -lpthread
SERVER_SRC_DIR = ./server
SERVER_SRC_LIST = $(wildcard $(CMN_DIR)/*.cpp) $(wildcard $(SERVER_SRC_DIR)/*.cpp)
SERVER_OBJ_LIST = $(SERVER_SRC_LIST:.cpp=.o)

all: ug-server ug-client

ug-server: $(SERVER_OBJ_LIST)
	$(CXX) $(CFLAG) -o $@ $^ $(LDFLAGS) $(SERVER_LDFLAGS)

ug-client: $(CLIENT_OBJ_LIST)
	$(CXX) $(CFLAG) -o $@ $^ $(LDFLAGS) $(CLIENT_LDFLAGS)

.PHONY: clean
clean:
	rm -rf server/*.o common/*.o client/*.o ug-server ug-client
