# CCP_WebServer
Webserver with database and support to play games in the browser

## Patterns:
### Creational pattern(s):
Factory pattern
- static create( ... ) functions return new {OBJECT} pointers
- available for the classes
  - User -- user.h
  - BlackjackUser -- blackjack_user.h
  - Dealer -- dealer.h
- used in
  - WebServer -- webserver.cpp
  - BlackJack -- black_jack.cpp

### Structural pattern(s):
Adapter pattern
- The Param class converts raw POST requests into Param objects, which are used as input for other functions in the classes
  - WebServer
  - Game

### Behavioral pattern(s):
Chain of responsibility pattern
- Everything originally gets funneled into the WebServer, which passes it to other functions, which again passes it to other functions, etc, eventually leading to a server response
- Example: main() -> Server::write( WebServer::processParam() -> buffer.assign(BlackJack::Input(Param=getHands) -> return BlackJack::getHands() ) );

Template method pattern
- Game is an abstract interface class containing pure virtual function, serving as a template for Game classes such as BlackJack
- Pure virtual functions are implemented in classes inheriting from it
- Non-pure virtual functions can be overridden, such as the User::addUser() function, which itself adds a standard User \* to its list of users, whereas BlackJack::addUser() adds a BlackJackUser \*.
- Because Game objects are polymorphic, the Game object list in WebServer can refer to any object inheriting from Game, with each their own behavior.

Scheduled-task pattern
- Both the server (main.cpp) and database (database/db.cpp) have autosave functions running on one minute intervals.
- The server autosave sends UPDATE queries to the database
- The database writes its current state to the savefile (database.db)

Observer pattern
- BlackJack::run() runs on a seperate thread than the server once the game is started
- BlackJack bases its state and actions on (a combination of) flag(s) which it constantly observes in the BlackJack::run() function
- BlackJack flags are set by the server

### Concurrency pattern(s)
Readers–writer lock
- The User::balance variable is accessed and updated in both the server thread and BlackJack thread(s). To prevent errors, the User::isFree lock must first be unlocked before User::balance can be read from or written to.
- The server thread and BlackJack thread(s) will wait for five seconds for the lock to unlock, otherwise they will throw an error and skip the intended access.
