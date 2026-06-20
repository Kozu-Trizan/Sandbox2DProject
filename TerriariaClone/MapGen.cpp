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


void GenerateVisibleWorld(Block Univ[][UniverseWidth], float Frequency, float Amplitude, int BaseLevel, int Octaves) {

    Perlin1D Perlin;

    for (int x = 0; x < UniverseWidth; x++) {
        int SurfaceY = BaseLevel;
        float FrequencyCol = Frequency;
        float AmplitudeCol = Amplitude;
        int OctavesCol = Octaves;
        while (OctavesCol > 0) {
            SurfaceY += static_cast<int>(Perlin.Noise(FrequencyCol * x) * AmplitudeCol);
            FrequencyCol *= 2;
            AmplitudeCol /= 2;
            OctavesCol--;
        }

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