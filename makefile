CC = gcc
CXX = g++
CFLAGS = -Wall -static -pthread -O3
CXXFLAGS = -Wall -static -O3 -Wl,--whole-archive -lpthread -Wl,--no-whole-archive

MAKE = make

OBJECTS = file_io.o param.o server.o web_server.o black_jack.o arpa_socket.o arpa_listen_socket.o chip.o card.o deck_of_cards.o user.o database_server.o online_database.o
EXEC = main

.PHONY: all clean run db

all: $(OBJECTS) db
	$(CXX) -o $(EXEC) main.cpp $(OBJECTS) $(CXXFLAGS)

$(OBJECTS): %.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

db:
	$(MAKE) -f makefile_db

clean:
	@rm *.o
	$(MAKE) -f makefile_db clean
	@rm $(EXEC)

run:
	@./$(EXEC)
