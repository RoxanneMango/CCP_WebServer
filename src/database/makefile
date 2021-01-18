DELETE = rm
MAKE = make
CC = gcc
CXX = g++

CFLAGS = -Wall -static -O3
CXXFLAGS = -Wall -std=c++0x -static -O3
LIBS = -Wl,--whole-archive -lpthread -Wl,--no-whole-archive

SRC = 
OUTPUT = build/

OBJECTS = server.o database_server.o
EXEC = db

ifeq ($(OS),Windows_NT)
	DELETE = del
	LIBS += -lws2_32
	OBJECTS += winsock2_socket.o winsock2_listen_socket.o
	CXXFLAGS += -D _WIN32_WINNT=0x0601
	EXEC = db.exe
else
	OBJECTS += arpa_socket.o arpa_listen_socket.o 
endif

.PHONY: all clean run db

all: $(OBJECTS) $(OUTPUT)
	$(CXX) -o $(OUTPUT)/$(EXEC) ./database/db.cpp $(OBJECTS) $(CXXFLAGS) $(LIBS)

$(OBJECTS): %.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $(LIBS) $< -o $(OUTPUT)

clean:
	@$(DELETE) $(OUTPUT)/$(EXEC)

run:
	@$(OUTPUT)/$(EXEC)

$(OUTPUT):
	mkdir $(OUTPUT)