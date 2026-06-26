#include <vector>
#include <random>
#include "Constant.h"
#include "Blocks.h"
#include "MapGen.h"



float Perlin1D::FadeFunc(float t) {
    return t * t * t * (t * (t * 6 - 15) + 10);
}

float Perlin1D::Interpolation(float a, float b, float t) {
    return a + t * (b - a);
}

Perlin1D::Perlin1D() {
    Perlin1D::Gradients.resize(UniverseWidth);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 1);

    for (int i = 0; i < UniverseWidth; i++) {
        Gradients[i] = dist(gen) ? 1 : -1;
    }
}

float Perlin1D::Noise(float CoordX) {
    int FloorX = static_cast<int>(std::floorf(CoordX));
    int CeilX = FloorX + 1;

    float DistanceLeft = CoordX - FloorX;
    float DistanceRight = CoordX - CeilX;

    int GradLeft = Gradients[FloorX % UniverseWidth];
    int GradRight = Gradients[CeilX % UniverseWidth];

    float t = FadeFunc(DistanceLeft);

    return Interpolation(DistanceLeft * GradLeft, DistanceRight * GradRight, t);
}

float HeightFromPerlin(Perlin1D& Perlin, int PosX, float Frequency, float Amplitude, int BaseLevel, int Octaves) {
    float Height = BaseLevel;
    while (Octaves > 0) {
        Height += (Perlin.Noise(Frequency * PosX) * Amplitude);
        Frequency *= 2;
        Amplitude /= 2;
        Octaves--;
    }
    Height = std::min(Height, std::roundf(Height));
    return Height;
}

void RemoveOneBlockSpike(Block Univ[][UniverseWidth]) {
    // Check except boundaries to prevent array index out of range
    for (int x = 1; x < UniverseWidth - 1; x++){
        for (int y = 1; y < UniverseHeight - 1; y++) {
            if (Univ[y][x].B_ID == Air.B_ID) continue;
            if (
                Univ[y][x + 1].B_ID == Air.B_ID && // Is Right Neighbour Air
                Univ[y][x - 1].B_ID == Air.B_ID && // Is Left Neighbour Air
                Univ[y - 1][x].B_ID == Air.B_ID // Is Top Neighbour Air
                ) Univ[y][x] = Grass;
        }
    }
}

void GenerateVisibleWorld(Block Univ[][UniverseWidth], float Frequency, float Amplitude, int BaseLevel, int Octaves) {

    Perlin1D Perlin;

    for (int x = 0; x < UniverseWidth; x++) {
        int SurfaceY = static_cast<int>(HeightFromPerlin(Perlin, x, Frequency, Amplitude, BaseLevel, Octaves));

        if (SurfaceY < 0) {
            SurfaceY = 0;
        }
        else if (SurfaceY > UniverseHeight) {
            SurfaceY = UniverseHeight - 1;
        }

        for (int y = 0; y < UniverseHeight; y++) {
            if (y < SurfaceY) {
                Univ[y][x] = Air;
            }
            else if (y - SurfaceY == 0) {
                Univ[y][x] = Grass;
            }
            else {
                Univ[y][x] = Dirt;
            }
        }
    }
}

void DrawVisibleWorld(Block Univ[][UniverseWidth], Camera2D camera) {
    Vector2 TopLeftBound = GetScreenToWorld2D(Vector2{ 0, 0 }, camera);
    Vector2 BottomRightBound = GetScreenToWorld2D({ (float)ScreenWidth, (float)ScreenHeight }, camera);
    float VisibleWorldHeight = BottomRightBound.y - TopLeftBound.y;
    float VisibleWorldWidth = BottomRightBound.x - TopLeftBound.x;

    for (int y = static_cast<int>((TopLeftBound.y / BLOCK_SIZE)); y < static_cast<int>(BottomRightBound.y / BLOCK_SIZE) + 1; y++) { // + 1 in order to avoid clipping at the right and bottom boundaries.
        if (y < 0 || y >= UniverseHeight) continue;
        for (int x = static_cast<int>(TopLeftBound.x / BLOCK_SIZE); x < static_cast<int>(BottomRightBound.x / BLOCK_SIZE) + 1; x++) {
            if (x < 0 || x >= UniverseWidth) continue;

            if (Univ[y][x].B_ID == 0) {
                continue;
            }
            else if (Univ[y][x].B_ID == 1) {
                DrawRectangle(x * BLOCK_SIZE, y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, BROWN);
            }
            else if (Univ[y][x].B_ID == 2) {
                DrawRectangle(x * BLOCK_SIZE, y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, GREEN);
            }
        }
    }

    // Post processing
    RemoveOneBlockSpike(Univ);
}

bool WorldBoundaryReached(Camera2D camera) {
    // Convert screen corners to world coordinates to get the camera's visible rectangle
    Vector2 TopLeftWorld = GetScreenToWorld2D({ 0, 0 }, camera);
    Vector2 BottomRightWorld = GetScreenToWorld2D({ (float)ScreenWidth, (float)ScreenHeight }, camera);

    // Check if camera view exceeds world boundaries
    // Returns true if boundary is reached (can't zoom further out)
    float worldWidth = UniverseWidth * BLOCK_SIZE;
    float worldHeight = UniverseHeight * BLOCK_SIZE;

    bool exceedsLeft = TopLeftWorld.x < 0;
    bool exceedsTop = TopLeftWorld.y < 0;
    bool exceedsRight = BottomRightWorld.x > worldWidth;
    bool exceedsBottom = BottomRightWorld.y > worldHeight;

    return exceedsLeft || exceedsTop || exceedsRight || exceedsBottom;
}