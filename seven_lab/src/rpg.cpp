#include "../include/rpg.h"
#include <chrono>

std::map<NpcType, std::string> typeToString = { { NpcType::ORC, "Orc" },
                                                 { NpcType::DRUID, "Druid" },
                                                 { NpcType::SQUIRREL, "Squirrel" } };

static std::mutex global_cout_mutex;

bool Map::add(std::unique_ptr<Npc>& npc) {
    std::lock_guard lock(mutex_);
    if(npc->getPos().x < MAP_MIN_X || npc->getPos().x > MAP_MAX_X || npc->getPos().y < MAP_MIN_Y || npc->getPos().y > MAP_MAX_Y)
        return false;

    npcs.push_back(std::move(npc));

    return true;
}

bool Map::removeNpc(const std::string& name) {
    std::lock_guard lock(mutex_);
    for (auto it = npcs.begin(); it != npcs.end(); ++it) {
        if ((*it)->getName() == name) {
            npcs.erase(it);
            return true;
        }
    }
    return false;
}

Npc* Map::findNpc(const std::string& name) {
    std::shared_lock lock(mutex_);
    for (auto& npc : npcs) {
        if (npc->getName() == name)
            return npc.get();
    }
    return nullptr;
}

const Npc* Map::findNpc(const std::string& name) const {
    std::shared_lock lock(mutex_);
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
    std::lock_guard lock(global_cout_mutex);
    if (auto* s = dynamic_cast<const SpawnEvent*>(&e)) {
        std::cout << "Spawned NPC:" << typeToString[s->getType()] 
                  << " \"" << s->getName() << "\" at (" << s->getPos().x << "," << s->getPos().y << ")" << std::endl;

    } else if (auto* k = dynamic_cast<const KillEvent*>(&e)) {
        std::cout << typeToString[k->getKillerType()] << " \"" << k->getKiller() 
                  << "\" killed " << typeToString[k->getVictimType()] << " \"" << k->getVictim() << "\"" << std::endl;
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

int Game::rollDice() {
    static thread_local std::mt19937 gen(std::random_device{}());
    static thread_local std::uniform_int_distribution<> dis(1, 6);
    return dis(gen);
}

void Game::init(int count) {
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<> dis_type(0, 2);
    std::uniform_int_distribution<> dis_x(MAP_MIN_X, MAP_MAX_X);
    std::uniform_int_distribution<> dis_y(MAP_MIN_Y, MAP_MAX_Y);
    
    NpcFactory factory;
    for (int i = 0; i < count; ++i) {
        NpcType type = static_cast<NpcType>(dis_type(gen));
        std::string name = typeToString[type] + "_" + std::to_string(i);
        Point pos{dis_x(gen), dis_y(gen)};
        auto npc = factory.create(type, name, pos);
        if (npc) {
            map_.add(npc);
            publisher_.publish(SpawnEvent(name, type, pos));
        }
    }
}

void Game::movementThread() {
    std::mt19937 gen(std::random_device{}());
    while (running_) {
        auto npcs = map_.getAllAlive();
        for (size_t i = 0; i < npcs.size(); ++i) {
            int dist = npcs[i]->getMoveDistance();
            std::uniform_int_distribution<> dis(-dist, dist);
            npcs[i]->move(dis(gen), dis(gen));
            
            for (size_t j = i + 1; j < npcs.size(); ++j) {
                double d = npcs[i]->distanceTo(npcs[j]);
                int range = std::max(npcs[i]->getKillRange(), npcs[j]->getKillRange());
                
                if (d <= range) {
                    battle_queue_.push({npcs[i], npcs[j]});
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void Game::battleThread() {
    BattleRules rules;
    while (running_) {
        BattlePair pair;
        if (battle_queue_.pop(pair)) {
            if (!pair.attacker->isAlive() || !pair.defender->isAlive()) 
                continue;
            
            FightResult res1 = rules.result(pair.attacker->getType(), pair.defender->getType());
            FightResult res2 = rules.result(pair.defender->getType(), pair.attacker->getType());
            
            if (res1 == FightResult::WIN && rollDice() > rollDice()) {
                pair.defender->kill();
                publisher_.publish(KillEvent(pair.attacker->getName(), pair.defender->getName(),
                    pair.attacker->getType(), pair.defender->getType(), pair.defender->getPos()));
            } else if (res2 == FightResult::WIN && rollDice() > rollDice()) {
                pair.attacker->kill();
                publisher_.publish(KillEvent(pair.defender->getName(), pair.attacker->getName(),
                    pair.defender->getType(), pair.attacker->getType(), pair.attacker->getPos()));
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void Game::printMap() {
    const int width = 60, height = 24;
    std::vector<std::string> canvas(height, std::string(width, '.'));
    
    auto npcs = map_.getAllAlive();
    for (auto* npc : npcs) {
        Point pos = npc->getPos();
        int x = (pos.x - MAP_MIN_X) * (width - 1) / (MAP_MAX_X - MAP_MIN_X);
        int y = (pos.y - MAP_MIN_Y) * (height - 1) / (MAP_MAX_Y - MAP_MIN_Y);
        y = height - 1 - y;
        x = std::clamp(x, 0, width - 1);
        y = std::clamp(y, 0, height - 1);
        
        char marker = '?';
        if (npc->getType() == NpcType::ORC) 
            marker = 'O';
        else if (npc->getType() == NpcType::DRUID) 
            marker = 'D';
        else if (npc->getType() == NpcType::SQUIRREL) 
            marker = 'S';

        canvas[y][x] = marker;
    }
    
    std::lock_guard lock(cout_mutex_);
    std::cout << "\n=== MAP (alive: " << npcs.size() << ") ===\n";
    for (const auto& row : canvas) 
        std::cout << row << "\n";
}

void Game::run(int seconds) {
    std::thread t1([this]() { movementThread(); });
    std::thread t2([this]() { battleThread(); });
    
    auto start = std::chrono::steady_clock::now();
    while (std::chrono::steady_clock::now() - start < std::chrono::seconds(seconds)) {
        printMap();
        map_.removeDeadNpcs();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
    running_ = false;
    t1.join();
    t2.join();
    
    auto survivors = map_.getAllAlive();
    std::lock_guard lock(cout_mutex_);
    std::cout << "\n=== SURVIVORS (" << survivors.size() << ") ===\n";
    for (auto* npc : survivors) {
        Point pos = npc->getPos();
        std::cout << typeToString[npc->getType()] << " \"" << npc->getName() 
                  << "\" at (" << pos.x << "," << pos.y << ")\n";
    }
}