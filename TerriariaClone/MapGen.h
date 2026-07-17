#pragma once
#include <vector>
#include <random>
#include "Constant.h"
#include "Blocks.h"

extern Block Universe[UniverseHeight][UniverseWidth];

class Perlin1D {
private:
    std::vector<int> Gradients;

public:
    Perlin1D();
    static float FadeFunc(float t);
    static float Interpolation(float a, float b, float t);
    float Noise(float CoordX);
};

class Perlin2D {
private:
    int PermutationTable[512];
    const Vector2 Gradients[8] = {
        { 1.0f,  0.0f}, {-1.0f,  0.0f}, { 0.0f,  1.0f}, { 0.0f, -1.0f},
        { 0.7071f,  0.7071f}, {-0.7071f,  0.7071f}, { 0.7071f, -0.7071f}, {-0.7071f, -0.7071f}
    };

public:
    Perlin2D(unsigned int seed = 47);
    Vector2 GetGradient(int x, int y);
    float Noise(float CoordX, float CoordY);
    float GradientInfluence(Vector2 Grid, float CoordX, float CoordY);
};

// Standalone Function Prototypes
// Note: Default arguments (like Frequency = 0.01f) belong ONLY in the header file.

float HeightFromPerlin(Perlin1D& Perlin, int PosX, float Frequency, float Amplitude, int BaseLevel, int Octaves);
void RemoveOneBlockSpike(Block Univ[][UniverseWidth]);
int Terracing(float PerlinHeight, int StepSize = 5, float Sharpness = 3.4, float WidthSteep=0.5);

void GenerateVisibleWorld(Block Univ[][UniverseWidth], float Frequency = 0.01f, float Amplitude = 100.0f, int BaseLevel = 400, int Octaves = 1);
void DrawVisibleWorld(Block Univ[][UniverseWidth], Camera2D camera);
bool WorldBoundaryReached(Camera2D camera);