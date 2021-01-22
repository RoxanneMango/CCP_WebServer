# CCP_WebServer
Webserver with database and support to play games in the browser

## TODO:
- Open-closed principle in WebServer, by giving the routes (like /blackjack) via dependency injection
- Read configurations from a config file, instead of hardcoded in the main.cpp
  - ip
  - port
  - routes
  - etc
- Convert User::hand (std::vector<Card>) into its own Hand object
- House user-specific flags which currently reside in BlackJack in the User itself
- For the scheduled-task pattern, unit of work could be used
- Use authorization headers instead of tokens within the POST payload
  - Makes it possible to send GET requests with authentication
- Maybe convert Game::users into a template, instead of void pointers
  - The type of user is not going to change once a game has been initialized


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

### Concurrency pattern(s)
Readers–writer lock
- The User::balance variable is accessed and updated in both the server thread and BlackJack thread(s). To prevent errors, the User::isFree lock must first be unlocked before User::balance can be read from or written to.
- The server thread and BlackJack thread(s) will wait for five seconds for the lock to unlock, otherwise they will throw an error and skip the intended access.
