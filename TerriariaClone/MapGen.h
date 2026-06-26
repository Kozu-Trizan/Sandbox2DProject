#pragma once
#include <vector>
#include <random>
#include "Constant.h"
#include "Blocks.h"

extern Block Universe[UniverseHeight][UniverseWidth];

class Perlin1D {
private:
    std::vector<int> Gradients;

    float FadeFunc(float t);

    float Interpolation(float a, float b, float t);

public:
    Perlin1D();
    float Noise(float CoordX);
};


// Standalone Function Prototypes
// Note: Default arguments (like Frequency = 0.01f) belong ONLY in the header file.

float HeightFromPerlin(Perlin1D& Perlin, int PosX, float Frequency, float Amplitude, int BaseLevel, int Octaves);
void RemoveOneBlockSpike(Block Univ[][UniverseWidth]);

void GenerateVisibleWorld(Block Univ[][UniverseWidth], float Frequency = 0.01f, float Amplitude = 100.0f, int BaseLevel = 400, int Octaves = 1);
void DrawVisibleWorld(Block Univ[][UniverseWidth], Camera2D camera);
bool WorldBoundaryReached(Camera2D camera);