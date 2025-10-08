#include "include/rpg.h"
#include <thread>
#include <mutex>
#include <chrono>
#include <atomic>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <iostream>

std::vector<std::shared_ptr<NPC>> npcs;
std::mutex npcs_mx;
std::atomic<bool> game_running(true);
std::mutex count_mx;

int moveStepFor(const std::string& type) {
    if (type == "Orc") {
        return 20;
    }
    if (type == "Druid") {
        return 10;
    }
    return 5;
}

int killRangeFor(const std::string& type) {
    if (type == "Orc") {
        return 10;
    }
    if (type == "Druid") {
        return 10;
    }
    if (type == "Squirrel") {
        return 5;
    }
    return 10;
}

int KillRange() {
    return std::max({killRangeFor("Orc"), killRangeFor("Druid"), killRangeFor("Squirrel")});
}

int projectValue(int value, int minValue, int maxValue, int limit) {
    int result = 0;
    if (maxValue != minValue) {
        result = (value - minValue) * (limit - 1) / (maxValue - minValue);
    }
    if (result < 0) {
        result = 0;
    }
    if (result >= limit) {
        result = limit - 1;
    }
    return result;
}

void renderMap(int width = 60, int height = 24) {
    std::vector<std::string> canvas(height, std::string(width, '.'));
    std::size_t alive = 0;

    {
        std::lock_guard<std::mutex> locker(npcs_mx);
        alive = npcs.size();
        for (std::size_t i = 0; i < npcs.size(); ++i) {
            std::shared_ptr<NPC> npc = npcs[i];
            std::string type = npc->getType();
            char marker = '?';
            if (type == "Orc") {
                marker = 'O';
            } else if (type == "Druid") {
                marker = 'D';
            } else if (type == "Squirrel") {
                marker = 'S';
            }

            std::pair<int, int> pos = npc->getPosition();
            int cx = projectValue(pos.first, MAP_MIN_X, MAP_MAX_X, width);
            int cy = projectValue(pos.second, MAP_MIN_Y, MAP_MAX_Y, height);
            int row = height - 1 - cy;
            if (row < 0) {
                row = 0;
            }
            if (row >= height) {
                row = height - 1;
            }
            canvas[row][cx] = marker;
        }
    }

    std::lock_guard<std::mutex> outlk(count_mx);
    std::cout << "\n=== MAP (alive: " << alive << ") ===\n";
    for (int i = 0; i < height; ++i) {
        std::cout << canvas[i] << "\n";
    }
}

void initNPCs() {
    std::vector<std::string> types;
    types.push_back("Orc");
    types.push_back("Druid");
    types.push_back("Squirrel");

    std::lock_guard<std::mutex> locker(npcs_mx);
    npcs.clear();

    int created = 0;
    int attempts = 0;
    while (created < 50 && attempts < 5000) {
        attempts++;

        std::string type = types[std::rand() % types.size()];
        std::string name = type + "_" + std::to_string(created + 1);
        int x = MAP_MIN_X + std::rand() % (MAP_MAX_X - MAP_MIN_X + 1);
        int y = MAP_MIN_Y + std::rand() % (MAP_MAX_Y - MAP_MIN_Y + 1);

        std::shared_ptr<NPC> npc = NPCFactory::create(type, name, x, y);
        if (npc && isNameUnique(npcs, name)) {
            npcs.push_back(npc);
            created++;
        }
    }
}

void movementThread() {
    while (game_running.load(std::memory_order_relaxed)) {
        {
            std::lock_guard<std::mutex> locker(npcs_mx);
            for (std::size_t i = 0; i < npcs.size(); ++i) {
                std::shared_ptr<NPC> npc = npcs[i];
                int maxStep = moveStepFor(npc->getType());
                int dx = std::rand() % (maxStep * 2 + 1) - maxStep;
                int dy = std::rand() % (maxStep * 2 + 1) - maxStep;
                npc->translateClamped(dx, dy);
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void fightsThread(std::vector<std::shared_ptr<Observer>> observers) {
    Arena arena;
    double range = static_cast<double>(KillRange());

    while (game_running.load(std::memory_order_relaxed)) {
        {
            std::lock_guard<std::mutex> locker(npcs_mx);
            if (!npcs.empty()) {
                arena.startBattle(npcs, range, observers);
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

int main() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    std::vector<std::shared_ptr<Observer>> observers;
    observers.push_back(std::make_shared<ConsoleObserver>());
    observers.push_back(std::make_shared<FileObserver>("battle.log"));

    initNPCs();

    std::thread movement(movementThread);
    std::thread fights(fightsThread, observers);

    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
    while (std::chrono::steady_clock::now() - start < std::chrono::seconds(30)) {
        renderMap();
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::lock_guard<std::mutex> locker(npcs_mx);
        if (npcs.empty()) {
            break;
        }
    }

    game_running.store(false);

    if (movement.joinable()) {
        movement.join();
    }
    if (fights.joinable()) {
        fights.join();
    }

    {
        std::lock_guard<std::mutex> outlk(count_mx);
        std::cout << "\n=== SURVIVORS (" << npcs.size() << ") ===\n";
    }
    {
        std::lock_guard<std::mutex> locker(npcs_mx);
        printNPCs(npcs);
    }

    return 0;
}