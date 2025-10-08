#include "include/rpg.h"

int main() {
    std::vector<std::shared_ptr<NPC>> npcs;
    std::vector<std::shared_ptr<Observer>> observers;

    observers.push_back(std::make_shared<ConsoleObserver>());
    observers.push_back(std::make_shared<FileObserver>("log.txt"));

    std::shared_ptr<NPC> a = NPCFactory::create("Orc", "Grom", 100, 200);
    std::shared_ptr<NPC> b = NPCFactory::create("Druid", "Leaf", 150, 220);
    std::shared_ptr<NPC> c = NPCFactory::create("Squirrel", "Nutty", 200, 250);

    if (a && isNameUnique(npcs, a->getName())) 
        npcs.push_back(a);

    if (b && isNameUnique(npcs, b->getName())) 
        npcs.push_back(b);

    if (c && isNameUnique(npcs, c->getName())) 
        npcs.push_back(c);

    std::cout << "Created NPCs:\n";
    printNPCs(npcs);

    if (!saveToFile(npcs, "npcs.txt")) {
        std::cerr << "Failed to save npcs.txt\n";
    }

    std::vector<std::shared_ptr<NPC>> loadedNPCs;
    if (!loadFromFile(loadedNPCs, "npcs.txt")) {
        std::cerr << "Failed to load npcs.txt\n";
    }

    std::cout << "\nLoaded NPCs from file:\n";
    printNPCs(loadedNPCs);

    Arena arena;
    double range = 120.0;
    std::cout << "\nBattle Begins on Arena (range=" << range << "):\n";
    arena.startBattle(loadedNPCs, range, observers);

    std::cout << "\nSurvivors:\n";
    printNPCs(loadedNPCs);

    return 0;
}
