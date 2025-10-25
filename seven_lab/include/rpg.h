#ifndef RPG_H
#define RPG_H

#include <vector>
#include <memory>
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cmath>
#include <map>
#include <sstream>
#include <shared_mutex>
#include <mutex>
#include <queue>
#include <thread>
#include <atomic>
#include <random>

// Base entities
const int MAP_MIN_X = 0;
const int MAP_MAX_X = 500;
const int MAP_MIN_Y = 0;
const int MAP_MAX_Y = 500;

enum class NpcType {
    ORC,
    DRUID,
    SQUIRREL
};

struct Point {
    int x{0}, y{0};
};

class Visitor; 

class Npc {
    protected:
        NpcType type;
        std::string name;
        bool alive = true;
        Point pos;
        mutable std::shared_mutex mutex_;

    public:
        Npc(NpcType type, const std::string& name, Point pos) : type(type), name(name), pos(pos) {}
        
        const std::string& getName() const {
            return name;
        }

        NpcType getType() const {
            return type;
        }
        
        Point getPos() const {
            std::shared_lock lock(mutex_);
            return pos;
        }

        void setPos(Point p) {
            std::lock_guard lock(mutex_);
            pos = p;
        }
        
        bool isAlive() const {
            std::shared_lock lock(mutex_);
            return alive;
        }

        void kill() {
            std::lock_guard lock(mutex_);
            alive = false;
        }

        void move(int dx, int dy) {
            std::lock_guard lock(mutex_);
            pos.x = std::clamp(pos.x + dx, MAP_MIN_X, MAP_MAX_X);
            pos.y = std::clamp(pos.y + dy, MAP_MIN_Y, MAP_MAX_Y);
        }

        int getMoveDistance() const {
            if (type == NpcType::ORC) return 20;
            if (type == NpcType::DRUID) return 10;
            return 5;
        }

        int getKillRange() const {
            if (type == NpcType::ORC) return 10;
            if (type == NpcType::DRUID) return 10;
            return 5;
        }

        double distanceTo(const Npc* other) const;

        virtual void accept(Visitor& visitor) const = 0;

        virtual ~Npc() = default;
};

class Orc : public Npc {
    public:
        Orc(const std::string& name, Point pos) : Npc(NpcType::ORC, name, pos) {};
        void accept(Visitor& visitor) const override;
};

class Druid : public Npc {
    public:
        Druid(const std::string& name, Point pos) : Npc(NpcType::DRUID, name, pos) {};
        void accept(Visitor& visitor) const override;
};

class Squirrel : public Npc {
    public:
        Squirrel(const std::string& name, Point pos) : Npc(NpcType::SQUIRREL, name, pos) {};
        void accept(Visitor& visitor) const override;
};

class Map {
    protected:
        std::vector<std::unique_ptr<Npc>> npcs;
        mutable std::shared_mutex mutex_;
    public:
        bool add(std::unique_ptr<Npc>& npc);

        bool removeNpc(const std::string& name);

        Npc* findNpc(const std::string& name);
        const Npc* findNpc(const std::string& name) const;

        size_t size() const {
            std::shared_lock lock(mutex_);
            return npcs.size();
        }

        std::vector<Npc*> getAllAlive() const {
            std::shared_lock lock(mutex_);
            std::vector<Npc*> result;
            for (const auto& npc : npcs) {
                if (npc->isAlive()) result.push_back(npc.get());
            }
            return result;
        }

        void removeDeadNpcs() {
            std::lock_guard lock(mutex_);
            npcs.erase(std::remove_if(npcs.begin(), npcs.end(),
                [](const auto& npc) { return !npc->isAlive(); }), npcs.end());
        }

        Npc* at(size_t idx) {
            return npcs.at(idx).get();
        }

        const Npc* at(size_t idx) const {
            return npcs.at(idx).get();
        }

        void clear() {
            npcs.clear();
        }
};

//Observer pattern
class Event {
    public:
        virtual ~Event() = default;
};

class IObserver {
    public:
        virtual ~IObserver() = default;
        virtual void onEvent(const Event& event) = 0;
};

class FileObserver : public IObserver {
    public: 
    FileObserver (const std::string& filename) {
        log_file.open(filename);
    };

    void onEvent(const Event& e) override;
    ~FileObserver() {
        log_file.close();
    };

    private:
        std::ofstream log_file;
};

class ConsoleObserver : public IObserver {
    public:
        void onEvent(const Event& e) override;
};

class SpawnEvent final : public Event {
    public: 
        SpawnEvent(const std::string& name, NpcType type, Point pos) : name(name), type(type), pos(pos) {}

        const std::string& getName() const {
            return name;
        }

        const NpcType getType() const {
            return type;
        }

        const Point getPos() const {
            return pos;
        }

    private:
        std::string name;
        NpcType type;
        Point pos;
};

class KillEvent final : public Event {
    public:
        KillEvent(const std::string& killer, const std::string& victim, NpcType killer_type, NpcType victim_type, Point muerder_pos)
                : killer(killer), victim(victim), killer_type(killer_type), victim_type(victim_type), murder_pos(muerder_pos) {}

        const std::string& getKiller() const {
            return killer;
        }

        const std::string& getVictim() const {
            return victim;
        }

        NpcType getKillerType() const {
            return killer_type;
        }

        NpcType getVictimType() const {
            return victim_type;

        }

        Point getMurderPos() const {
            return murder_pos;
        }
    private:
        std::string killer;
        std::string victim;
        NpcType killer_type;
        NpcType victim_type;
        Point murder_pos;
};

class Publisher {
    private:
        std::vector<IObserver*> observers;
    public:
        void subscribe(IObserver* observer) {
            observers.push_back(observer);
        }

        void unsubscribe(IObserver* observer) {
            observers.erase(std::remove(observers.begin(), observers.end(), observer), observers.end());
        }

        void publish(const Event& event) const {
            for (auto observer : observers)
                observer->onEvent(event);
        }
};


//Factory pattern
class Factory {
    public:
        virtual ~Factory() = default;
        virtual std::unique_ptr<Npc> create(NpcType type, const std::string& name, Point pos) const = 0;
};

class NpcFactory final : public Factory {
    public:
        std::unique_ptr<Npc> create(NpcType type, const std::string& name, Point pos) const override;
};

//File storage

bool save(const Map& map, const std::string& filename);
bool load(Map& map, const std::string& filename, Publisher& bus);

//Visitor pattern
enum class FightResult {
    WIN,
    LOSE,
    DRAW
};

class Visitor {
    public:
        virtual ~Visitor() = default;
        virtual void visit(const Orc& o) = 0;
        virtual void visit(const Druid& d) = 0;
        virtual void visit(const Squirrel& s) = 0;
};


class BattleRules {
    public:
        FightResult result(NpcType attacker, NpcType deffender) {
            if (attacker == NpcType::ORC) {
                if (deffender == NpcType::DRUID)
                    return FightResult::WIN;
            } else if (attacker == NpcType::DRUID) {
                if (deffender == NpcType::SQUIRREL)
                    return FightResult::WIN;
                else if (deffender == NpcType::ORC)
                    return FightResult::LOSE;
            } else if (attacker == NpcType::SQUIRREL) {
                if (deffender == NpcType::DRUID)
                    return FightResult::LOSE;
            }
            
            return FightResult::DRAW;
        }
};

class FightVisitor : public Visitor {
    public:
        FightVisitor(NpcType attacker_type, const std::string& attacker_name, BattleRules& rules) 
            : attacker_type(attacker_type), attacker_name(attacker_name), rules(rules) {}

        void visit(const Orc& o) override {
            fight_result = rules.result(attacker_type, NpcType::ORC);
        }

        void visit(const Druid& d) override {
            fight_result = rules.result(attacker_type, NpcType::DRUID);
        }

        void visit(const Squirrel& s) override {
            fight_result = rules.result(attacker_type, NpcType::SQUIRREL);
        }

        FightResult getResult() const {
            return fight_result;
        }

    private:
        NpcType attacker_type;
        const std::string& attacker_name;
        FightResult fight_result = FightResult::DRAW;
        BattleRules& rules;
};


class BattleEngine {
    public:
        BattleEngine(int radius, Publisher& bus) : radius_(radius), bus_(bus) {};
        void run(Map& map);

    private:
        int radius_;
        Publisher& bus_;
};

struct BattlePair {
    Npc* attacker;
    Npc* defender;
};

class BattleQueue {
    private:
        std::queue<BattlePair> queue_;
        mutable std::mutex mutex_;
    public:
        void push(const BattlePair& pair) {
            std::lock_guard lock(mutex_);
            queue_.push(pair);
        }

        bool pop(BattlePair& pair) {
            std::lock_guard lock(mutex_);
            if (queue_.empty()) return false;
            pair = queue_.front();
            queue_.pop();
            return true;
        }
};

class Game {
    private:
        Map map_;
        Publisher& publisher_;
        BattleQueue battle_queue_;
        std::atomic<bool> running_{true};
        mutable std::mutex cout_mutex_;

        void movementThread();
        void battleThread();
        void printMap();
        int rollDice();

    public:
        Game(Publisher& pub) : publisher_(pub) {}
        void init(int count);
        void run(int seconds);
};

#endif