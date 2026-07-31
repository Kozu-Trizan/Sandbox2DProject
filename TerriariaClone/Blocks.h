#pragma once
#include <cstdint>
#include "Player.h"

struct Block {
    std::uint8_t B_ID = 0;
    float BaseHP = 0;
    float HP = BaseHP;
    std::uint8_t WallID = 0;
};

struct BlockPos {
    int x, y;
};

// 'inline' allows these to stay in the header file safely
const inline Block Air;
const inline Block Dirt = { 1, 5, 5 ,1 };
const inline Block Grass = { 2, 2, 2, 1 };
