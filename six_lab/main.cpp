#include "include/rpg.h"

extern std::map<NpcType, std::string> typeToString;

void add_npc(NpcType type, std::string name, Point pos, Map& map, Publisher& bus) {
    NpcFactory factory;
    std::unique_ptr<Npc> npc = factory.create(type, name, pos);
    if (!npc)
        std::cout << "Factory returned nullptr" << std::endl;
        
    if(map.add(npc))
        bus.publish(SpawnEvent(name, type, {pos.x, pos.y}));
}

void print_list(Map& map) {
    for (size_t i = 0; i < map.size(); ++i) {
        Npc* npc = map.at(i);
        if (npc) {
            std::cout << i << ") " << typeToString[npc->getType()] << " \"" << npc->getName() 
                << "\" at (" << npc->getPos().x << "," << npc->getPos().y << ") " 
                << (npc->isAlive() ? "alive" : "dead") << std::endl;
        } else {
            std::cout << i << ") empty" << std::endl;
        }
    }
}

void save_map(Map& map, const std::string& filename) {
    save(map, filename);
}

void load_map(Map& map, const std::string& filename, Publisher& bus) {
    load(map, filename, bus);
}

void run_battle(Map& map, int r, Publisher& bus) {
    if (r > 0) {
        BattleEngine battle{r, bus};
        battle.run(map);
    }
}

int main() {
    Publisher bus;
    ConsoleObserver con;
    FileObserver file("log.txt");

    bus.subscribe(&con);
    bus.subscribe(&file);
    Map world;
    std::string line;

    while (std::getline(std::cin, line)) {
        std::istringstream iss(line);
        std::string cmd;
        iss >> cmd;

        if (cmd == "add") {
            std::string type_str, name;
            int x, y;
            iss >> type_str >> name >> x >> y;
            NpcType type;

            if (type_str == "Orc") 
                type = NpcType::ORC;
            else if (type_str == "Druid") 
                type = NpcType::DRUID;
            else if (type_str == "Squirrel") 
                type = NpcType::SQUIRREL;
            else { 
                std::cout << "Unknown type\n";
                continue; 
            }

            add_npc(type, name, {x, y}, world, bus);
        }
        else if (cmd == "list") {
            print_list(world);
        }
        else if (cmd == "save") {
            std::string filename;
            iss >> filename;
            save_map(world, filename);
        }
        else if (cmd == "load") {
            std::string filename;
            iss >> filename;
            load_map(world, filename, bus);
        }
        else if (cmd == "battle") {
            int radius;
            iss >> radius;
            run_battle(world, radius, bus);
        }
        else if (cmd == "exit") {
            break;
        }
        else {
            std::cout << "Unknown command\n";
        }
    } 
    return 0;
}
