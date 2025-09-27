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

// abstract Observer
class Observer {
public:
    virtual void update(const std::string& event) = 0;
    virtual ~Observer() = default;
};

// Forward declarations for Visitor
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

// NPC class
class NPC {
public:
    std::string name;
    int x, y;
    std::string type;

    // type, name order to match implementation
    NPC(const std::string& type, const std::string& name, int x, int y);

    // Visitor-based interaction
    virtual void accept(Visitor* v) = 0;

    // Geometry
    double distanceTo(const NPC* opponent) const;
    static bool inBounds(int px, int py);

    // Info/IO
    virtual void print() const;
    virtual std::string getType() const;
    virtual std::string getName() const;
    virtual std::string serialize() const;
};

// concrete NPC
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

// Observers
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

// Arena
class Arena {
public:
    void startBattle(std::vector<std::shared_ptr<NPC>>& npcs, double range, std::vector<std::shared_ptr<Observer>>& observers);
};

// Factory
class NPCFactory {
public:
    static std::shared_ptr<NPC> create(const std::string& type, const std::string& name, int x, int y);
    static std::shared_ptr<NPC> loadFromLine(const std::string& line);
};

// Utils / IO
void printNPCs(const std::vector<std::shared_ptr<NPC>>& npcs);
bool saveToFile(const std::vector<std::shared_ptr<NPC>>& npcs, const std::string& filename);
bool loadFromFile(std::vector<std::shared_ptr<NPC>>& out, const std::string& filename);
bool isNameUnique(const std::vector<std::shared_ptr<NPC>>& npcs, const std::string& name);

#endif
