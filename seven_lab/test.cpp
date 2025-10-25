#include <gtest/gtest.h>
#include "../include/rpg.h"

TEST(NPCTest, CreateNPC) {
    NpcFactory factory;
    auto npc = factory.create(NpcType::ORC, "Grom", {100, 200});
    ASSERT_NE(npc, nullptr);
    EXPECT_EQ(npc->getType(), NpcType::ORC);
    EXPECT_EQ(npc->getName(), "Grom");
    Point pos = npc->getPos();
    EXPECT_EQ(pos.x, 100);
    EXPECT_EQ(pos.y, 200);
}

TEST(NPCTest, InvalidPosition) {
    NpcFactory factory;
    auto npc = factory.create(NpcType::ORC, "Test", {-10, 200});
    EXPECT_EQ(npc, nullptr);
}

TEST(NPCTest, EmptyName) {
    NpcFactory factory;
    auto npc = factory.create(NpcType::ORC, "", {100, 200});
    EXPECT_EQ(npc, nullptr);
}

TEST(NPCTest, DistanceToNPC) {
    NpcFactory factory;
    auto npc1 = factory.create(NpcType::ORC, "Grom", {0, 0});
    auto npc2 = factory.create(NpcType::SQUIRREL, "Nutty", {3, 4});
    EXPECT_DOUBLE_EQ(npc1->distanceTo(npc2.get()), 5.0);
}

TEST(NPCTest, Movement) {
    NpcFactory factory;
    auto npc = factory.create(NpcType::ORC, "Grom", {100, 100});
    npc->move(10, 10);
    Point pos = npc->getPos();
    EXPECT_EQ(pos.x, 110);
    EXPECT_EQ(pos.y, 110);
}

TEST(NPCTest, KillAndAlive) {
    NpcFactory factory;
    auto npc = factory.create(NpcType::ORC, "Grom", {100, 100});
    EXPECT_TRUE(npc->isAlive());
    npc->kill();
    EXPECT_FALSE(npc->isAlive());
}

TEST(MapTest, AddNpc) {
    Map map;
    NpcFactory factory;
    auto npc = factory.create(NpcType::ORC, "Grom", {100, 200});
    EXPECT_TRUE(map.add(npc));
    EXPECT_EQ(map.size(), 1u);
}

TEST(MapTest, FindNpc) {
    Map map;
    NpcFactory factory;
    auto npc = factory.create(NpcType::ORC, "Grom", {100, 200});
    map.add(npc);
    EXPECT_NE(map.findNpc("Grom"), nullptr);
    EXPECT_EQ(map.findNpc("Unknown"), nullptr);
}

TEST(MapTest, RemoveNpc) {
    Map map;
    NpcFactory factory;
    auto npc = factory.create(NpcType::ORC, "Grom", {100, 200});
    map.add(npc);
    EXPECT_TRUE(map.removeNpc("Grom"));
    EXPECT_EQ(map.size(), 0u);
}

TEST(MapTest, RemoveDeadNpcs) {
    Map map;
    NpcFactory factory;
    auto npc1 = factory.create(NpcType::ORC, "Grom", {100, 200});
    auto npc2 = factory.create(NpcType::DRUID, "Leaf", {150, 250});
    npc1->kill();
    map.add(npc1);
    map.add(npc2);
    map.removeDeadNpcs();
    EXPECT_EQ(map.size(), 1u);
}

TEST(BattleRulesTest, OrcKillsDruid) {
    BattleRules rules;
    EXPECT_EQ(rules.result(NpcType::ORC, NpcType::DRUID), FightResult::WIN);
}

TEST(BattleRulesTest, DruidKillsSquirrel) {
    BattleRules rules;
    EXPECT_EQ(rules.result(NpcType::DRUID, NpcType::SQUIRREL), FightResult::WIN);
}

TEST(BattleRulesTest, DruidLosesToOrc) {
    BattleRules rules;
    EXPECT_EQ(rules.result(NpcType::DRUID, NpcType::ORC), FightResult::LOSE);
}

TEST(BattleRulesTest, SameTypeDraw) {
    BattleRules rules;
    EXPECT_EQ(rules.result(NpcType::ORC, NpcType::ORC), FightResult::DRAW);
}

TEST(BattleEngineTest, OrcKillsDruid) {
    Map map;
    Publisher bus;
    NpcFactory factory;
    auto orc = factory.create(NpcType::ORC, "Grom", {100, 100});
    auto druid = factory.create(NpcType::DRUID, "Leaf", {105, 105});
    map.add(orc);
    map.add(druid);
    
    BattleEngine engine(10, bus);
    engine.run(map);
    
    EXPECT_EQ(map.size(), 1u);
    EXPECT_EQ(map.at(0)->getName(), "Grom");
}

TEST(BattleEngineTest, OutOfRange) {
    Map map;
    Publisher bus;
    NpcFactory factory;
    auto orc = factory.create(NpcType::ORC, "Grom", {100, 100});
    auto druid = factory.create(NpcType::DRUID, "Leaf", {200, 200});
    map.add(orc);
    map.add(druid);
    
    BattleEngine engine(10, bus);
    engine.run(map);
    
    EXPECT_EQ(map.size(), 2u);
}

TEST(FileTest, SaveAndLoad) {
    Map map;
    Publisher bus;
    NpcFactory factory;
    auto npc1 = factory.create(NpcType::ORC, "Grom", {100, 200});
    auto npc2 = factory.create(NpcType::SQUIRREL, "Nutty", {150, 250});
    map.add(npc1);
    map.add(npc2);
    
    ASSERT_TRUE(save(map, "test.txt"));
    
    Map loaded_map;
    ASSERT_TRUE(load(loaded_map, "test.txt", bus));
    EXPECT_EQ(loaded_map.size(), 2u);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
