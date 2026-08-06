#pragma once
#include <cstdint>
#include "Player.h"
#include "Inventory.h"

struct BlockPos {
    int x, y;
};

class Block : public Item {
public:
    std::uint8_t B_ID = 0;
    float BaseHP = 0;
    float HP = BaseHP;
    std::uint8_t WallID = 0;
    bool Natural;

    Block(
        std::uint8_t bid = 0,
        float bhp = 0,
        float hp = 0,
        std::uint8_t wid = 0
    ) : B_ID(bid), BaseHP(bhp), HP(hp), WallID(wid) {
        this->HP = this->BaseHP;
        this->Natural = true;
    }

    void RestoreHealth();
    bool SurroundedByAir(BlockPos pos);
    bool operator!=(Block another);
};

// 'inline' allows these to stay in the header file safely
const inline Block Air;
const inline Block Dirt( 1, 5, 5 ,1 );
const inline Block Grass( 2, 2, 2);
