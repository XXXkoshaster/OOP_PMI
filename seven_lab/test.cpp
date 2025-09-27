#include <gtest/gtest.h>
#include "../include/rpg.h"

TEST(NPCTest, CreateNPC) {
    auto npc = NPCFactory::create("Orc", "Grom", 100, 200);
    ASSERT_NE(npc, nullptr);
    EXPECT_EQ(npc->getType(), "Orc");
    EXPECT_EQ(npc->getName(), "Grom");
    EXPECT_EQ(npc->x, 100);
    EXPECT_EQ(npc->y, 200);
}

TEST(NPCTest, DeserializeNPC) {
    std::string data = "Druid Leaf 300 400";
    auto npc = NPCFactory::loadFromLine(data);
    ASSERT_NE(npc, nullptr);
    EXPECT_EQ(npc->getType(), "Druid");
    EXPECT_EQ(npc->getName(), "Leaf");
    EXPECT_EQ(npc->x, 300);
    EXPECT_EQ(npc->y, 400);
}

TEST(NPCTest, DeserializeInvalidNPC) {
    std::string data = "InvalidType Unknown 0 0";
    auto npc = NPCFactory::loadFromLine(data);
    EXPECT_EQ(npc, nullptr);
}

TEST(NPCTest, DistanceToNPC) {
    auto npc1 = NPCFactory::create("Orc", "Grom", 100, 200);
    auto npc2 = NPCFactory::create("Squirrel", "Nutty", 150, 250);

    double distance = npc1->distanceTo(npc2.get());
    EXPECT_NEAR(distance, 70.7107, 0.0001);
}

TEST(NPCTest, SaveAndLoadNPCsFromFile) {
    std::vector<std::shared_ptr<NPC>> npcs;
    npcs.push_back(NPCFactory::create("Orc", "Grom", 100, 200));
    npcs.push_back(NPCFactory::create("Squirrel", "Nutty", 150, 250));

    const std::string filename = "test_npcs.txt";
    ASSERT_TRUE(saveToFile(npcs, filename));

    std::vector<std::shared_ptr<NPC>> loadedNPCs;
    ASSERT_TRUE(loadFromFile(loadedNPCs, filename));

    ASSERT_EQ(loadedNPCs.size(), 2u);
    EXPECT_EQ(loadedNPCs[0]->getType(), "Orc");
    EXPECT_EQ(loadedNPCs[1]->getType(), "Squirrel");
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
