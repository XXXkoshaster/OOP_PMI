#include <gtest/gtest.h>
#include "../include/rpg.h"

TEST(FactoryTest, CreateOrc) {
    NpcFactory factory;
    auto npc = factory.create(NpcType::ORC, "Grom", {100, 200});
    ASSERT_NE(npc, nullptr);
    EXPECT_EQ(npc->getType(), NpcType::ORC);
    EXPECT_EQ(npc->getName(), "Grom");
    EXPECT_EQ(npc->getPos().x, 100);
    EXPECT_EQ(npc->getPos().y, 200);
}

TEST(FactoryTest, CreateDruid) {
    NpcFactory factory;
    auto npc = factory.create(NpcType::DRUID, "Leaf", {300, 400});
    ASSERT_NE(npc, nullptr);
    EXPECT_EQ(npc->getType(), NpcType::DRUID);
}

TEST(FactoryTest, CreateSquirrel) {
    NpcFactory factory;
    auto npc = factory.create(NpcType::SQUIRREL, "Nutty", {150, 250});
    ASSERT_NE(npc, nullptr);
    EXPECT_EQ(npc->getType(), NpcType::SQUIRREL);
}

TEST(FactoryTest, InvalidPosition) {
    NpcFactory factory;
    auto npc = factory.create(NpcType::ORC, "Test", {-10, 200});
    EXPECT_EQ(npc, nullptr);
}

TEST(FactoryTest, EmptyName) {
    NpcFactory factory;
    auto npc = factory.create(NpcType::ORC, "", {100, 200});
    EXPECT_EQ(npc, nullptr);
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

TEST(NpcTest, Distance) {
    NpcFactory factory;
    auto npc1 = factory.create(NpcType::ORC, "Grom", {0, 0});
    auto npc2 = factory.create(NpcType::DRUID, "Leaf", {3, 4});
    EXPECT_DOUBLE_EQ(npc1->distanceTo(npc2.get()), 5.0);
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

TEST(BattleRulesTest, SquirrelLosesToDruid) {
    BattleRules rules;
    EXPECT_EQ(rules.result(NpcType::SQUIRREL, NpcType::DRUID), FightResult::LOSE);
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
