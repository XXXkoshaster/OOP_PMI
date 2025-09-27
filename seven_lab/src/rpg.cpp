#include "../include/rpg.h"

void ConsoleObserver::update(const std::string& event) {
    std::cout << event << std::endl;
}

FileObserver::FileObserver(const std::string& filename) {
    logFile.open(filename.c_str(), std::ios::app);
    if (!logFile.is_open()) {
        std::cerr << "cannot open log file: " << filename << std::endl;
    }
}
void FileObserver::update(const std::string& event) {
    if (logFile.is_open()) {
        logFile << event << std::endl;
    }
}
FileObserver::~FileObserver() {
    if (logFile.is_open()) logFile.close();
}

NPC::NPC(const std::string& t, const std::string& n, int px, int py)
    : type(t), name(n), x(px), y(py) {}

std::string NPC::getType() const { return type; }
std::string NPC::getName() const { return name; }

void NPC::print() const {
    std::cout << type << " \"" << name << "\" at (" << x << "," << y << ")\n";
}

double NPC::distanceTo(const NPC* other) const {
    double dx = (double)x - (double)other->x;
    double dy = (double)y - (double)other->y;
    return std::sqrt(dx*dx + dy*dy);
}

std::string NPC::serialize() const {
    std::ostringstream oss;
    oss << type << " " << name << " " << x << " " << y;
    return oss.str();
}

bool NPC::inBounds(int px, int py) {
    return px >= MAP_MIN_X && px <= MAP_MAX_X && py >= MAP_MIN_Y && py <= MAP_MAX_Y;
}

Orc::Orc(const std::string& name, int x, int y) : NPC("Orc", name, x, y) {}
void Orc::accept(Visitor* v) { v->visit(this); }

Druid::Druid(const std::string& name, int x, int y) : NPC("Druid", name, x, y) {}
void Druid::accept(Visitor* v) { v->visit(this); }

Squirrel::Squirrel(const std::string& name, int x, int y) : NPC("Squirrel", name, x, y) {}
void Squirrel::accept(Visitor* v) { v->visit(this); }

class FightVisitor : public Visitor {
public:
    NPC* attacker;
    NPC* defender;
    bool attackerDead;
    bool defenderDead;

    explicit FightVisitor(NPC* a)
        : attacker(a), defender(nullptr), attackerDead(false), defenderDead(false) {}

    void visit(Orc* d) override {
        defender = d;
        if (attacker->getType() == "Druid") {}
        else if (attacker->getType() == "Orc") {} 
        else if (attacker->getType() == "Squirrel") {}
    }

    void visit(Druid* d) override {
        defender = d;
        if (attacker->getType() == "Orc") {
            defenderDead = true; 
        } else if (attacker->getType() == "Druid") {
        } else if (attacker->getType() == "Squirrel") {}
    }

    void visit(Squirrel* d) override {
        defender = d;
        if (attacker->getType() == "Druid") {
            defenderDead = true; 
        } else if (attacker->getType() == "Orc") {
        } else if (attacker->getType() == "Squirrel") {}
    }
};
    
std::shared_ptr<NPC> NPCFactory::create(const std::string& type,
                                        const std::string& name,
                                        int x, int y) {
    if (!NPC::inBounds(x, y)) return std::shared_ptr<NPC>();
    if (type == "Orc") return std::make_shared<Orc>(name, x, y);
    if (type == "Druid") return std::make_shared<Druid>(name, x, y);
    if (type == "Squirrel") return std::make_shared<Squirrel>(name, x, y);
    return std::shared_ptr<NPC>();
}

std::shared_ptr<NPC> NPCFactory::loadFromLine(const std::string& line) {
    std::istringstream iss(line);
    std::string type, name;
    int x, y;
    if (!(iss >> type >> name >> x >> y)) return std::shared_ptr<NPC>();
    return create(type, name, x, y);
}

void Arena::startBattle(std::vector<std::shared_ptr<NPC>>& npcs,
                        double range,
                        std::vector<std::shared_ptr<Observer>>& observers) {
    // живые флаги
    std::vector<int> alive(npcs.size(), 1);

    for (size_t i = 0; i < npcs.size(); ++i) {
        if (!alive[i]) continue;
        for (size_t j = i + 1; j < npcs.size(); ++j) {
            if (!alive[j]) continue;

            NPC* a = npcs[i].get();
            NPC* b = npcs[j].get();

            if (a->distanceTo(b) > range) continue;

            FightVisitor v1(a);
            b->accept(&v1);

            FightVisitor v2(b);
            if (!v1.defenderDead && !v1.attackerDead) {
                a->accept(&v2);
            }

            bool killA = v1.attackerDead || v2.defenderDead; 
            bool killB = v1.defenderDead || v2.attackerDead; 

            if (killA && alive[i]) {
                alive[i] = 0;
                std::string ev = b->getName() + " killed " + a->getName();
                for (size_t k = 0; k < observers.size(); ++k) observers[k]->update(ev);
            }
            if (killB && alive[j]) {
                alive[j] = 0;
                std::string ev = a->getName() + " killed " + b->getName();
                for (size_t k = 0; k < observers.size(); ++k) observers[k]->update(ev);
            }
        }
    }

    // Удаляем мёртвых
    std::vector<std::shared_ptr<NPC>> aliveList;
    for (size_t i = 0; i < npcs.size(); ++i) {
        if (alive[i]) aliveList.push_back(npcs[i]);
    }
    npcs.swap(aliveList);
}
 
void printNPCs(const std::vector<std::shared_ptr<NPC>>& npcs) {
    for (size_t i = 0; i < npcs.size(); ++i) {
        npcs[i]->print();
    }
}

bool saveToFile(const std::vector<std::shared_ptr<NPC>>& npcs,
                const std::string& filename) {
    std::ofstream out(filename.c_str());
    if (!out.is_open()) return false;
    for (size_t i = 0; i < npcs.size(); ++i) {
        out << npcs[i]->serialize() << "\n";
    }
    return true;
}

bool loadFromFile(std::vector<std::shared_ptr<NPC>>& out,
                  const std::string& filename) {
    std::ifstream in(filename.c_str());
    if (!in.is_open()) return false;
    std::string line;
    while (std::getline(in, line)) {
        std::shared_ptr<NPC> npc = NPCFactory::loadFromLine(line);
        if (npc) out.push_back(npc);
    }
    return true;
}

bool isNameUnique(const std::vector<std::shared_ptr<NPC>>& npcs,
                  const std::string& name) {
    for (size_t i = 0; i < npcs.size(); ++i) {
        if (npcs[i]->getName() == name) return false;
    }
    return true;
}
