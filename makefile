DELETE = rm
MAKE = make
CC = gcc
CXX = g++

CFLAGS = -Wall -static -O3
CXXFLAGS = -Wall -std=c++14 -static -O3
LIBS = -Wl,--whole-archive -lpthread -Wl,--no-whole-archive

OBJECTS = file_io.o param.o server.o web_server.o black_jack.o chip.o card.o deck_of_cards.o user.o database_server.o online_database.o dealer.o blackjack_user.o
EXEC = main

ifeq ($(OS),Windows_NT)
	DELETE = del
	LIBS += -lws2_32
	OBJECTS += winsock2_socket.o winsock2_listen_socket.o
	CXXFLAGS += -D _WIN32_WINNT=0x0601
	EXEC = main.exe
else
	OBJECTS += arpa_socket.o arpa_listen_socket.o 
endif

.PHONY: all clean run db

all: $(OBJECTS) db
	$(CXX) -o $(EXEC) main.cpp $(OBJECTS) $(CXXFLAGS) $(LIBS)

$(OBJECTS): %.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $(LIBS) $< -o $@

db:
	$(MAKE) -f makefile_db

clean:
	@$(DELETE) *.o
	@$(DELETE) $(EXEC)
	@$(MAKE) -f makefile_db clean

run:
	@./$(EXEC)
