#include "include/rpg.h"

int main() {
    Publisher publisher;
    ConsoleObserver console_obs;
    FileObserver file_obs("battle.log");
    
    publisher.subscribe(&console_obs);
    publisher.subscribe(&file_obs);
    
    Game game(publisher);
    game.init(50);
    game.run(30);
    
    return 0;
}
