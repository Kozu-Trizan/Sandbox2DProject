#pragma once
#include <raylib.h>
#include <cstdint>

const int UniverseWidth = 4200; // In terms of Blocks
const int UniverseHeight = 1200;

const int ScreenWidth = 1024;
const int ScreenHeight = 512;
const int BLOCK_SIZE = 16;

const Color SKY = { 135, 206, 235, 1 };

const float MAP_FREQ = 0.002f;
const float MAP_AMP = 100.0f;
const int MAP_BASE_LEVEL = 400;
const int MAP_OCTAVE = 8;

const float CAVE_FREQ_X = 0.032f;
const float CAVE_FREQ_Y = 0.065f;
const float CAVE_AMP = 1.2f;
const int CAVE_OCTAVE = 3;
const float CAVE_THRESHOLD = 0.48f;


 // -----------------BLock IDs-----------------------//
const std::uint8_t AIR_BID = 0;
const std::uint8_t DIRT_BID = 1;
const std::uint8_t GRASS_BID = 2;
const std::uint8_t STONE_BID = 3;
const std::uint8_t WOOD_BID = 4;

// -----------------BLock IDs-----------------------//
const std::uint8_t AIR_WALL_ID = 0;
const std::uint8_t DIRT_WALL_ID = 1;
const std::uint8_t GRASS_WALL_ID = 0;
const std::uint8_t STONE_WALL_ID = 2;
const std::uint8_t WOOD_WALL_ID = 3;

//----------------------Base HP-------------------//
const float AIR_BHP = 0;
const float DIRT_BHP = 5.0f;
const float GRASS_BHP = 2.0f;
const float STONE_BHP = 10.0f;
const float WOOD_BHP = 4.0f;

const std::uint8_t MAX_LIGHT_VAL = 255;

//------------------Light Decay Rates-------------//
const std::uint8_t AIR_LIGHT_DECAY = 12;
const std::uint8_t DIRT_LIGHT_DECAY = 70;
const std::uint8_t GRASS_LIGHT_DECAY = 50;
const std::uint8_t STONE_LIGHT_DECAY = 100;
const std::uint8_t WOOD_LIGHT_DECAY = 30;