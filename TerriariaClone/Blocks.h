#pragma once
#include <cstdint>
#include "Player.h"
#include "Inventory.h"

struct BlockPos {
    int x, y;

    bool operator<(const BlockPos& other) const {
        if (x != other.x) return x < other.x;
        return y < other.y;
    }
};

class Block : public Item {
public:
    std::uint8_t B_ID = 0;
    float BaseHP = 0;
    float HP = BaseHP;
    std::uint8_t WallID = 0;
    bool Natural;
    std::uint8_t LightValue = 0;
    std::uint8_t LightDecay = 0;

    Block(
        std::uint8_t bid = 0,
        float bhp = 0,
        std::uint8_t wid = 0,
        std::uint8_t lv=0,
        std::uint8_t ld=0
    ) : B_ID(bid), BaseHP(bhp), WallID(wid), LightValue(lv), LightDecay(ld) {
        this->HP = this->BaseHP;
        this->Natural = true;
    }

    void RestoreHealth();
    bool SurroundedByAir(BlockPos& pos);
    bool operator!=(Block& another);
    bool operator==(Block& another);

    std::uint8_t GetLightValue();

    std::uint8_t GetLightDecay();

    void SetLightValue(std::uint8_t NewLightValue);
};

Rectangle GetBlockSourceRectangle(std::uint8_t blockID);
Color GetBlockColor(std::uint8_t blockID);

// 'inline' allows these to stay in the header file safely
const inline Block Air(AIR_BID, AIR_BHP, AIR_WALL_ID, MAX_LIGHT_VAL, AIR_LIGHT_DECAY);
const inline Block Dirt(DIRT_BID, DIRT_BHP, DIRT_WALL_ID, 0, DIRT_LIGHT_DECAY);
const inline Block Grass(GRASS_BID, GRASS_BHP, GRASS_WALL_ID, 0, GRASS_LIGHT_DECAY);
const inline Block Stone(STONE_BID, STONE_BHP, STONE_WALL_ID, 0, STONE_LIGHT_DECAY);
const inline Block Wood(WOOD_BID, WOOD_BHP, WOOD_WALL_ID, 0, WOOD_LIGHT_DECAY);
