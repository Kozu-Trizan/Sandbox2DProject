#pragma once

#include <raylib.h>

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

const float CAVE_FREQ_X = 0.05f;
const float CAVE_FREQ_Y = 0.08f;
const float CAVE_AMP = 0.8f;
const int CAVE_OCTAVE = 6;
const float CAVE_THRESHOLD = 0.55f;