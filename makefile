DELETE = rm
MAKE = make
CC = gcc
CXX = g++

CFLAGS = -Wall -static -O3
CXXFLAGS = -Wall -std=c++14 -static -O3
LIBS = -Wl,--whole-archive -lpthread -Wl,--no-whole-archive

SRC = src
BUILD = build
OUT = out
DB = $(SRC)/database

OBJECTS = file_io.o param.o server.o web_server.o black_jack.o chip.o card.o deck_of_cards.o user.o database_server.o online_database.o dealer.o blackjack_user.o console.o card_hand.o
EXEC = server

DB_OBJECTS = server.o database_server.o
DB_EXEC = db

ARCHIVE = lib.a

ifeq ($(OS),Windows_NT)
	DELETE = del
	LIBS += -lws2_32
	OBJECTS += winsock2_socket.o winsock2_listen_socket.o
	CXXFLAGS += -D _WIN32_WINNT=0x0601
	EXEC = server.exe
	DB_EXEC = db.exe
else
	OBJECTS += arpa_socket.o arpa_listen_socket.o 
endif

.PHONY: all clean run db

all: $(OBJECTS) database
	cd $(BUILD)
	$(CXX) -o $(BUILD)/$(OUT)/$(EXEC) $(SRC)/main.cpp $(BUILD)/$(ARCHIVE) $(CXXFLAGS) $(LIBS)

$(OBJECTS): %.o: $(SRC)/%.cpp $(OUT)
	$(CXX) -c $(CXXFLAGS) $(LIBS) $< -o $(BUILD)/$@
	$(AR) rcs $(BUILD)/$(ARCHIVE) $(BUILD)/$@

database: $(OBJECTS)
	$(CXX) -o $(BUILD)/$(OUT)/$(DB_EXEC) $(DB)/db.cpp $(BUILD)/$(ARCHIVE) $(CXXFLAGS) $(LIBS)

ifeq ($(OS),Windows_NT)

server:
	$(BUILD)\(OUT)\$(EXEC)
db:
	$(BUILD)\$(OUT)\$(DB_EXEC)

$(BUILD):
	if not exist $(BUILD) mkdir $(BUILD)
$(OUT): $(BUILD)
	cd $(BUILD) && if not exist $(OUT) mkdir $(OUT) && cd ..

clean:
	$(DELETE) $(BUILD)\\*.*
	$(DELETE) $(BUILD)\$(OUT)\$(EXEC)
	$(DELETE) $(BUILD)\$(OUT)\$(DB_EXEC)

else

server:
	./$(BUILD)/$(OUT)/$(EXEC)
db:
	./$(BUILD)/$(OUT)/$(DB_EXEC)

$(BUILD):
	mkdir -p $(BUILD)
$(OUT): $(BUILD)
	mkdir -p $(BUILD)

clean:
	$(DELETE) $(BUILD)/*.*
	$(DELETE) $(BUILD)/$(OUT)/$(EXEC)
	$(DELETE) $(BUILD)/$(OUT)/$(DB_EXEC)

doxygen:
	cd $(SRC) && doxygen doxyconfig

endif