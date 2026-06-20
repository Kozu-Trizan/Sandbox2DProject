#pragma once
#include <cstdint>

struct Block {
    std::uint8_t B_ID = 0;
};

// 'inline' allows these to stay in the header file safely
inline Block Air = { 0 };
inline Block Dirt = { 1 };
inline Block Grass = { 2 };