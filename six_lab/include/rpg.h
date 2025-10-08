#ifndef RPG_H
#define RPG_H

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <cmath>
#include <sstream>
#include <fstream>

// Arena size
const int MAP_MIN_X = 0;
const int MAP_MAX_X = 500;
const int MAP_MIN_Y = 0;
const int MAP_MAX_Y = 500;

class Observer {
public:
    virtual void update(const std::string& event) = 0;
    virtual ~Observer() = default;
};

class NPC;
class Orc;
class Druid;
class Squirrel;

class Visitor {
public:
    virtual void visit(Orc* o) = 0;
    virtual void visit(Druid* d) = 0;
    virtual void visit(Squirrel* s) = 0;
    virtual ~Visitor() = default;
};

class FightVisitor : public Visitor {
public:
    NPC* attacker;
    NPC* defender;
    bool attackerDead;
    bool defenderDead;

    explicit FightVisitor(NPC* attacker);

    void visit(Orc* d) override;
    void visit(Druid* d) override;
    void visit(Squirrel* d) override;
};

class NPC {
public:
    std::string name;
    int x, y;
    std::string type;
    bool alive = true;

    bool isDead() const noexcept { 
        return !alive; 
    }
    
    void kill() noexcept { 
        alive = false; 
    }

    void revive() noexcept {
        alive = true; 
    }
    
    NPC(const std::string& type, const std::string& name, int x, int y);

    virtual void accept(Visitor* v) = 0;

    double distanceTo(const NPC* opponent) const;
    static bool inBounds(int px, int py);

    virtual void print() const;
    virtual std::string getType() const;
    virtual std::string getName() const;
    virtual std::string serialize() const;

};

class Orc : public NPC {
public:
    Orc(const std::string& name, int x, int y);
    void accept(Visitor* v) override;
};

class Squirrel : public NPC {
public:
    Squirrel(const std::string& name, int x, int y);
    void accept(Visitor* v) override;
};

class Druid : public NPC {
public:
    Druid(const std::string& name, int x, int y);
    void accept(Visitor* v) override;
};

class ConsoleObserver : public Observer {
public:
    void update(const std::string& event) override;
};

class FileObserver : public Observer {
private:
    std::ofstream logFile;
public:
    FileObserver(const std::string& filename);
    void update(const std::string& event) override;
    ~FileObserver();
};

class Arena {
public:
    void startBattle(std::vector<std::shared_ptr<NPC>>& npcs, double range, std::vector<std::shared_ptr<Observer>>& observers);
};

class NPCFactory {
public:
    static std::shared_ptr<NPC> create(const std::string& type, const std::string& name, int x, int y);
    static std::shared_ptr<NPC> loadFromLine(const std::string& line);
};

void printNPCs(const std::vector<std::shared_ptr<NPC>>& npcs);
bool saveToFile(const std::vector<std::shared_ptr<NPC>>& npcs, const std::string& filename);
bool loadFromFile(std::vector<std::shared_ptr<NPC>>& out, const std::string& filename);
bool isNameUnique(const std::vector<std::shared_ptr<NPC>>& npcs, const std::string& name);

#endif
