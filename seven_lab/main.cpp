#include <thread>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>
#include <queue>
#include <random>
#include <chrono>
#include <atomic>
#include <map>
#include <cstdlib>
#include <ctime>

#include "../include/rpg.h"

// ---- Карта для потоковой версии ----
const int TMAP_W = 100;
const int TMAP_H = 100;

// ---- Глобальные мьютексы для вывода и доступа к NPC ----
std::mutex cout_mtx;
std::shared_mutex npcs_mtx;

// ---- Очередь задач боёв: пара индексов в векторе NPC ----
std::mutex fights_mtx;
std::condition_variable fights_cv;
std::queue<std::pair<int,int>> fights_q;

// ---- Простые таблицы "хода" и "дистанции убийства" ----
int moveDist(const NPC* n) {
    std::string t = n->getType();
    if (t == "Orc") return 20;
    if (t == "Druid") return 10;
    if (t == "Squirrel") return 5;
    return 5;
}
int killDist(const NPC* n) {
    std::string t = n->getType();
    if (t == "Orc") return 10;
    if (t == "Druid") return 10;
    if (t == "Squirrel") return 5;
    return 5;
}

// Совместимость убийств по прежним правилам
bool canKill(const NPC* a, const NPC* b) {
    if (a->getType() == "Orc" && b->getType() == "Druid") return true;
    if (a->getType() == "Druid" && b->getType() == "Squirrel") return true;
    return false;
}

// «Боевая кость» 1..6
int d6() {
    return (std::rand() % 6) + 1;
}

// Простейший контейнер «жив/мертв»
struct Actor {
    std::shared_ptr<NPC> npc;
    bool alive;
};

// Случайное смещение в пределах длины шага (манхэттен-стиль, чтобы не «ускорять» диагонали)
static int clamp(int v, int lo, int hi) { return v < lo ? lo : (v > hi ? hi : v); }

void randomStep(Actor& a) {
    if (!a.alive) return;
    int step = moveDist(a.npc.get());
    // случайное смещение от -step до +step по x и y, но обычным образом ограничим карту
    int dx = (std::rand() % (2*step + 1)) - step;
    int dy = (std::rand() % (2*step + 1)) - step;

    int nx = clamp(a.npc->x + dx, 0, TMAP_W);
    int ny = clamp(a.npc->y + dy, 0, TMAP_H);
    a.npc->x = nx;
    a.npc->y = ny;
}

// Печать карты (живые отображаются первой буквой типа)
void printMap(const std::vector<Actor>& actors) {
    const int W = TMAP_W, H = TMAP_H;
    // грубая, но простая печать: покажем только список координат, чтобы не засорять экран
    std::lock_guard<std::mutex> lk(cout_mtx);
    std::cout << "---- MAP ----" << std::endl;
    for (size_t i = 0; i < actors.size(); ++i) {
        if (!actors[i].alive) continue;
        char c = '?';
        std::string t = actors[i].npc->getType();
        if (t == "Orc") c = 'O';
        else if (t == "Druid") c = 'D';
        else if (t == "Squirrel") c = 'S';
        std::cout << c << " \"" << actors[i].npc->getName() << "\" at ("
                  << actors[i].npc->x << "," << actors[i].npc->y << ")\n";
    }
    std::cout << "-------------" << std::endl;
}

// Поток движения + обнаружения боёв
void movementThread(std::vector<Actor>& actors,
                    std::atomic<bool>& running) {
    while (running.load()) {
        {
            // перемещение
            std::unique_lock<std::shared_mutex> ul(npcs_mtx);
            for (size_t i = 0; i < actors.size(); ++i) {
                if (actors[i].alive) randomStep(actors[i]);
            }
        }

        // поиск пар на «расстоянии убийства»
        {
            std::shared_lock<std::shared_mutex> sl(npcs_mtx);
            for (size_t i = 0; i < actors.size(); ++i) {
                if (!actors[i].alive) continue;
                for (size_t j = i + 1; j < actors.size(); ++j) {
                    if (!actors[j].alive) continue;
                    double dist = actors[i].npc->distanceTo(actors[j].npc.get());
                    int kd = std::min(killDist(actors[i].npc.get()),
                                      killDist(actors[j].npc.get()));
                    if (dist <= kd) {
                        std::lock_guard<std::mutex> ql(fights_mtx);
                        fights_q.push({(int)i,(int)j});
                    }
                }
            }
        }
        fights_cv.notify_one();

        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}

// Поток боёв: достаёт пары из очереди и разыгрывает d6
void combatThread(std::vector<Actor>& actors,
                  std::vector<std::shared_ptr<Observer>>& observers,
                  std::atomic<bool>& running) {
    while (running.load()) {
        std::unique_lock<std::mutex> ql(fights_mtx);
        fights_cv.wait_for(ql, std::chrono::milliseconds(200), []{ return !fights_q.empty(); });
        if (!fights_q.empty()) {
            auto p = fights_q.front(); fights_q.pop();
            ql.unlock();

            int i = p.first, j = p.second;

            // копия указателей + проверка статусов под shared-lock
            std::shared_lock<std::shared_mutex> sl(npcs_mtx);
            if (i < 0 || j < 0 || i >= (int)actors.size() || j >= (int)actors.size()) continue;
            if (!actors[i].alive || !actors[j].alive) continue;
            NPC* a = actors[i].npc.get();
            NPC* b = actors[j].npc.get();
            double dist = a->distanceTo(b);
            int kd = std::min(killDist(a), killDist(b));
            if (dist > kd) continue; // вдруг разошлись
            bool aCan = canKill(a, b);
            bool bCan = canKill(b, a);
            sl.unlock();

            // если никто никого не может убивать — пропускаем
            if (!aCan && !bCan) continue;

            // кидаем кубики
            int atkA = aCan ? d6() : 0;
            int defB = aCan ? d6() : 0;
            int atkB = bCan ? d6() : 0;
            int defA = bCan ? d6() : 0;

            bool killB = aCan && (atkA > defB);
            bool killA = bCan && (atkB > defA);

            // отметим смерти под unique-lock
            {
                std::unique_lock<std::shared_mutex> ul(npcs_mtx);
                if (killB && actors[j].alive) {
                    actors[j].alive = false;
                    std::string ev = actors[i].npc->getName() + " killed " + actors[j].npc->getName();
                    for (size_t k = 0; k < observers.size(); ++k) observers[k]->update(ev);
                    std::lock_guard<std::mutex> lk(cout_mtx);
                    std::cout << ev << std::endl;
                }
                if (killA && actors[i].alive) {
                    actors[i].alive = false;
                    std::string ev = actors[j].npc->getName() + " killed " + actors[i].npc->getName();
                    for (size_t k = 0; k < observers.size(); ++k) observers[k]->update(ev);
                    std::lock_guard<std::mutex> lk(cout_mtx);
                    std::cout << ev << std::endl;
                }
            }
        }
    }
}

// Основной поток: раз в секунду печатает карту
void printerLoop(const std::vector<Actor>& actors,
                 std::atomic<bool>& running) {
    while (running.load()) {
        {
            std::shared_lock<std::shared_mutex> sl(npcs_mtx);
            printMap(actors);
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

int main() {
    std::srand((unsigned)std::time(nullptr));

    // Наблюдатели
    std::vector<std::shared_ptr<Observer>> observers;
    observers.push_back(std::make_shared<ConsoleObserver>());
    observers.push_back(std::make_shared<FileObserver>("log.txt"));

    // 50 случайных NPC
    std::vector<Actor> actors;
    for (int i = 0; i < 50; ++i) {
        int kind = std::rand() % 3; // 0=Orc, 1=Druid, 2=Squirrel
        int x = std::rand() % (TMAP_W + 1);
        int y = std::rand() % (TMAP_H + 1);
        std::string name = "N" + std::to_string(i);
        std::shared_ptr<NPC> npc;
        if (kind == 0) npc = NPCFactory::create("Orc", name, x, y);
        else if (kind == 1) npc = NPCFactory::create("Druid", name, x, y);
        else npc = NPCFactory::create("Squirrel", name, x, y);
        Actor a; a.npc = npc; a.alive = true;
        actors.push_back(a);
    }

    std::atomic<bool> running(true);

    // Потоки
    std::thread mover([&]{ movementThread(actors, running); });
    std::thread fighter([&]{ combatThread(actors, observers, running); });
    std::thread printer([&]{ printerLoop(actors, running); });

    // 30 секунд игры
    std::this_thread::sleep_for(std::chrono::seconds(30));
    running.store(false);
    fights_cv.notify_all();

    mover.join();
    fighter.join();
    printer.join();

    // Выводим выживших
    {
        std::shared_lock<std::shared_mutex> sl(npcs_mtx);
        std::lock_guard<std::mutex> lk(cout_mtx);
        std::cout << "\n=== SURVIVORS ===\n";
        for (size_t i = 0; i < actors.size(); ++i) {
            if (actors[i].alive) {
                actors[i].npc->print();
            }
        }
        std::cout << "=================\n";
    }

    return 0;
}
