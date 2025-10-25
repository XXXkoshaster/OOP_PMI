#include "../include/rpg.h"

std::map<NpcType, std::string> typeToString = { { NpcType::ORC, "Orc" },
                                                 { NpcType::DRUID, "Druid" },
                                                 { NpcType::SQUIRREL, "Squirrel" } };

bool Map::add(std::unique_ptr<Npc>& npc) {
    if(npc->getPos().x < MAP_MIN_X || npc->getPos().x > MAP_MAX_X || npc->getPos().y < MAP_MIN_Y || npc->getPos().y > MAP_MAX_Y)
        return false;

    npcs.push_back(std::move(npc));

    return true;
}

bool Map::removeNpc(const std::string& name) {
    for (auto it = npcs.begin(); it != npcs.end(); ++it) {
        if ((*it)->getName() == name) {
            npcs.erase(it);
            return true;
        }
    }
    return false;
}

Npc* Map::findNpc(const std::string& name) {
    for (auto& npc : npcs) {
        if (npc->getName() == name)
            return npc.get();
    }
    return nullptr;
}

const Npc* Map::findNpc(const std::string& name) const {
    for (const auto& npc : npcs) {
        if (npc->getName() == name)
            return npc.get();
    }
    return nullptr;
}

std::unique_ptr<Npc> NpcFactory::create(NpcType type, const std::string& name, Point pos) const {
    if (name.empty())
        return nullptr;

    if (pos.x < MAP_MIN_X || pos.x > MAP_MAX_X || pos.y < MAP_MIN_Y || pos.y > MAP_MAX_Y)
        return nullptr;

    if (type == NpcType::ORC)
        return std::make_unique<Orc>(name, pos);
    else if (type == NpcType::DRUID)
        return std::make_unique<Druid>(name, pos);
    else if (type == NpcType::SQUIRREL)
        return std::make_unique<Squirrel>(name, pos);
    
    return nullptr;
}

void FileObserver::onEvent(const Event& e) {
    if (auto* s = dynamic_cast<const SpawnEvent*>(&e)) {
        log_file << "Spawned NPC:" << typeToString[s->getType()] 
                  << " \"" << s->getName() << "\" at (" << s->getPos().x << "," << s->getPos().y << ")" << std::endl;

    } else if (auto* k = dynamic_cast<const KillEvent*>(&e)) {
        log_file << "Killed NPC:" << typeToString[k->getVictimType()] 
                  << " \"" << k->getVictim() << "\"" << "by" << typeToString[k->getKillerType()]
                  << " \"" << k->getKiller() << "\"" << "at (" << k->getMurderPos().x << "," << k->getMurderPos().y << ")" << std::endl;
    }
}

void ConsoleObserver::onEvent(const Event& e) {
    if (auto* s = dynamic_cast<const SpawnEvent*>(&e)) {
        std::cout << "Spawned NPC:" << typeToString[s->getType()] 
                  << " \"" << s->getName() << "\" at (" << s->getPos().x << "," << s->getPos().y << ")" << std::endl;

    } else if (auto* k = dynamic_cast<const KillEvent*>(&e)) {
        std::cout << "Killed NPC:" << typeToString[k->getVictimType()] 
                  << " \"" << k->getVictim() << "\"" << "by" << typeToString[k->getKillerType()]
                  << " \"" << k->getKiller() << "\"" << "at (" << k->getMurderPos().x << "," << k->getMurderPos().y << ")" << std::endl;
    }
}

bool save(const Map& map, const std::string& filename) {
    std::ofstream out(filename);
    if (!out.is_open())
        return false;

    for (size_t i = 0; i < map.size(); ++i) {
        const Npc* npc = map.at(i);
        out << typeToString[npc->getType()] << " " 
            << npc->getName() << " " 
            << npc->getPos().x << " " 
            << npc->getPos().y << "\n";
    }

    return true;
}

bool load(Map& map, const std::string& filename, Publisher& bus) {
    map.clear();

    std::ifstream in(filename);
    if (!in.is_open()) 
        return false;
    
    NpcFactory factory;
    std::string line;

    while (std::getline(in, line)) {
        std::istringstream iss(line);
        std::string type_str, name;
        int x, y;
        
        if (!(iss >> type_str >> name >> x >> y))
            continue;
        
        NpcType type;
        if (type_str == "Orc") 
            type = NpcType::ORC;
        else if (type_str == "Druid") 
            type = NpcType::DRUID;
        else if (type_str == "Squirrel") 
            type = NpcType::SQUIRREL;
        else 
            continue;
        
        auto npc = factory.create(type, name, {x, y});
        if (npc) {
            map.add(npc);
            bus.publish(SpawnEvent(name, type, {x, y}));
        }
    }
    return true;
}

double Npc::distanceTo(const Npc* other) const {
    double dx = getPos().x - other->getPos().x;
    double dy = getPos().y - other->getPos().y;
    return std::sqrt(dx*dx + dy*dy);
}

void BattleEngine::run(Map& map) {
    BattleRules rules;
    std::vector<Npc*> died_npcs;
    
    for (size_t i = 0; i < map.size(); ++i) {
        for (size_t j = i + 1; j < map.size(); ++j) {
            Npc* npc1 = map.at(i);
            Npc* npc2 = map.at(j);
            
            if (npc1->isAlive() && npc2->isAlive() && npc1->distanceTo(npc2) <= radius_) {
                FightVisitor atk(npc1->getType(), npc1->getName(), rules);
                npc2->accept(atk);

                FightResult res = atk.getResult();
                if (res == FightResult::WIN) {
                    npc2->kill();
                    died_npcs.push_back(npc2);
                    bus_.publish(KillEvent(npc1->getName(), npc2->getName(), npc1->getType(), npc2->getType(), npc2->getPos()));
                } else if (res == FightResult::LOSE) {
                    npc1->kill();
                    died_npcs.push_back(npc1);
                    bus_.publish(KillEvent(npc2->getName(), npc1->getName(), npc2->getType(), npc1->getType(), npc1->getPos()));
                } else if (res == FightResult::DRAW) {
                    continue;
                }
            }
        }
    }

    for (auto npc : died_npcs) {
        map.removeNpc(npc->getName());
    }
}

void Orc::accept(Visitor& visitor) const {
    visitor.visit(*this);
}

void Druid::accept(Visitor& visitor) const {
    visitor.visit(*this);
}

void Squirrel::accept(Visitor& visitor) const {
    visitor.visit(*this);
}