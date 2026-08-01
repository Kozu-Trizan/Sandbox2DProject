#pragma once
#include <cstdint>
#include "Player.h"
#include "Inventory.h"

class Block : public Item {
public:
    std::uint8_t B_ID = 0;
    float BaseHP = 0;
    float HP = BaseHP;
    std::uint8_t WallID = 0;

    Block(
        std::uint8_t bid = 0,
        float bhp = 0,
        float hp = 0,
        std::uint8_t wid = 0
    ) : B_ID(bid), BaseHP(bhp), HP(hp), WallID(wid) {
        this->HP = this->BaseHP;
    }
};

struct BlockPos {
    int x, y;
};

// 'inline' allows these to stay in the header file safely
const inline Block Air;
const inline Block Dirt( 1, 5, 5 ,1 );
const inline Block Grass( 2, 2, 2, 1 );
const inline Block Stone(3, 8, 8, 1);
