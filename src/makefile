SRC = $(wildcard *.cpp)
OBJ = $(SRC:.cpp=.o)

CFLAGS = -Wall -Weffc++ -Werror -pedantic -g
LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio

all: ungroup_client

ungroup_client: $(OBJ)
	$(CXX) $(CFLAGS) -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -rf *.o *.gch ungroup_client
