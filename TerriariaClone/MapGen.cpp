#include <vector>
#include <random>
#include <numeric>
#include <cstring>
#include "Constant.h"
#include "Blocks.h"
#include "MapGen.h"

// Perlin1D class

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

// Perlin2D class
Perlin2D::Perlin2D(unsigned int seed) {
    std::vector<int> Permutation(256);
    std::iota(Permutation.begin(), Permutation.end(), 0);

    std::default_random_engine Engine(seed);
    std::shuffle(Permutation.begin(), Permutation.end(), Engine);

    // Duplicate the array to prevent indexing overflow during neighbor lookups
    for (int i = 0; i < 256; i++) {
        Perlin2D::PermutationTable[i] = Permutation[i];
        Perlin2D::PermutationTable[i + 256] = Permutation[i];
    }
}

Vector2 Perlin2D::GetGradient(int CoordX, int CoordY) {
    // Hash coordinates using permutation table
    int Hash = Perlin2D::PermutationTable[(Perlin2D::PermutationTable[CoordX & 255] + CoordY) & 255];
    return Perlin2D::Gradients[Hash % 8];
}

float Perlin2D::GradientInfluence(Vector2 Grid, float CoordX, float CoordY) {
    Vector2 Grad = Perlin2D::GetGradient(static_cast<int>(Grid.x), static_cast<int>(Grid.y));
    float DisX = CoordX - Grid.x;
    float DisY = CoordY - Grid.y;

    return (DisX * Grad.x) + (DisY * Grad.y);
}

float Perlin2D::Noise(float CoordX, float CoordY) {
    int x0 = static_cast<int>(CoordX);
    int y0 = static_cast<int>(CoordY);
    Vector2 BLeft = { static_cast<float>(x0), static_cast<float>(y0 + 1) };
    Vector2 BRight = { static_cast<float>(x0 + 1), static_cast<float>(y0 + 1) };
    Vector2 TLeft = { static_cast<float>(x0), static_cast<float>(y0) };
    Vector2 TRight = { static_cast<float>(x0 + 1), static_cast<float>(y0) };

    float DotBLeft = Perlin2D::GradientInfluence(BLeft, CoordX, CoordY);
    float DotBRight = Perlin2D::GradientInfluence(BRight, CoordX, CoordY);
    float DotTLeft =  Perlin2D::GradientInfluence(TLeft, CoordX, CoordY);
    float DotTRight = Perlin2D::GradientInfluence(TRight, CoordX, CoordY);

    float HorizontalWeight = Perlin1D::FadeFunc(CoordX - std::floorf(CoordX));
    float VerticalWeight = Perlin1D::FadeFunc(CoordY - std::floorf(CoordY));

    return Perlin1D::Interpolation(Perlin1D::Interpolation(DotBLeft, DotBRight, HorizontalWeight), Perlin1D::Interpolation(DotTLeft, DotTRight, HorizontalWeight), VerticalWeight);
}

float HeightFromPerlin(Perlin1D& Perlin, int PosX, float Frequency, float Amplitude, int BaseLevel, int Octaves) {
    float Height = static_cast<float>(BaseLevel);
    while (Octaves > 0) {
        Height += (Perlin.Noise(Frequency * static_cast<float>(PosX)) * Amplitude);
        Frequency *= 2.0f;
        Amplitude /= 2.0f;
        Octaves--;
    }
    Height = std::min(Height, std::roundf(Height));
    return Height;
}

float NoiseForCave(Perlin2D& Cave, int PosX, int PosY, float CaveFreqX, float CaveFreqY, float Amplitude, int Octaves) {
    float Noise = 0;
    while (Octaves > 0) {
        Noise += (Cave.Noise(static_cast<float>(PosX) * CaveFreqX, static_cast<float>(PosY) * CaveFreqY) * Amplitude);
        CaveFreqX *= 2.0f;
        Amplitude /= 2.0f;
        Octaves--;
    }
    return Noise;
}

//void RemoveOneBlockSpike(Block **Univ) {
//    // Check except boundaries to prevent array index out of range
//    for (int x = 1; x < UniverseWidth - 1; x++){
//        for (int y = 1; y < UniverseHeight - 1; y++) {
//            if (Univ[y][x].B_ID == Air.B_ID) continue;
//            if (
//                Univ[y][x + 1].B_ID == Air.B_ID && // Is Right Neighbour Air
//                Univ[y][x - 1].B_ID == Air.B_ID && // Is Left Neighbour Air
//                Univ[y - 1][x].B_ID == Air.B_ID // Is Top Neighbour Air
//                ) {
//                Univ[y][x] = Air;
//            }
//
//            // Make blocks continuous
//            if (
//                Univ[y][x + 1].B_ID == Univ[y][x - 1].B_ID && 
//                Univ[y][x].B_ID != Univ[y][x + 1].B_ID
//                ) Univ[y][x].B_ID = Univ[y][x + 1].B_ID;
//        }
//    }
//}

// Smooth the World with Plateaus
int Terracing(float PerlinHeight, int StepSize, float Sharpness, float WidthSteep) {
    float Bias = 0;
    PerlinHeight /= static_cast<float>(StepSize);
    int IntegerPart = static_cast<int>(std::floorf(PerlinHeight));
    float FracPart = PerlinHeight - static_cast<float>(IntegerPart);

    if (FracPart < WidthSteep) {
        Bias = WidthSteep * std::powf(2.0f * FracPart, Sharpness);
    }
    else {
        Bias = 1.0f - WidthSteep * std::powf(2.0f * (1.0f - FracPart), Sharpness);
    }

    return static_cast<int>(IntegerPart + Bias) * StepSize;
}

void GenerateVisibleWorld(Block **Univ, float Frequency, float Amplitude, int BaseLevel, int Octaves) {

    Perlin1D Perlin;
    Perlin2D Cave;

    for (int x = 0; x < UniverseWidth; x++) {
        int SurfaceY = Terracing(HeightFromPerlin(Perlin, x, Frequency, Amplitude, BaseLevel, Octaves), 4, 3.8f);

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
            else if (y == SurfaceY) {
                Univ[y][x] = Grass;
            }
            else {
                // Only calculate cave noise for underground blocks
                float CaveNoise = NoiseForCave(Cave, x, y, CAVE_FREQ_X, CAVE_FREQ_Y, CAVE_AMP, CAVE_OCTAVE);
                if (CaveNoise > CAVE_THRESHOLD && y > SurfaceY + 30) {
                    Univ[y][x] = Air;
                }
                else {
                    Univ[y][x] = Dirt;
                }
            }
        }
    }

    // Post processing
    for (int i = 0; i < 20; i++)
    {
        Automata(Univ);
    }

}

void Automata(Block **Univ) {
    Block **UnivBuffer = nullptr;

    UnivBuffer = new Block*[UniverseHeight];
    for (int i = 0; i < UniverseHeight; i++) {
        UnivBuffer[i] = new Block[UniverseWidth];
    }

    std::memcpy(UnivBuffer, Univ, sizeof(Univ));
    int Dilation = 0;
    int Errosion = 0;
    Block ReplacementBlock = Dirt;
    for (int x = 2; x < UniverseWidth - 2; x++){
        for (int y = 2; y < UniverseHeight - 2; y++){
            if (UnivBuffer[y][x].B_ID == Air.B_ID) {
                (UnivBuffer[(y - 1)][(x - 1)].B_ID != Air.B_ID) ? Dilation++ : Dilation += 0;
                (UnivBuffer[(y - 1)][(x)].B_ID != Air.B_ID) ? Dilation++ : Dilation += 0;
                (UnivBuffer[(y - 1)][(x + 1)].B_ID != Air.B_ID) ? Dilation++ : Dilation += 0;
                (UnivBuffer[(y)][(x - 1)].B_ID != Air.B_ID) ? Dilation++ : Dilation += 0;
                (UnivBuffer[(y)][(x + 1)].B_ID != Air.B_ID) ? Dilation++ : Dilation += 0;
                (UnivBuffer[(y + 1)][(x - 1)].B_ID != Air.B_ID) ? Dilation++ : Dilation += 0;
                (UnivBuffer[(y + 1)][(x)].B_ID != Air.B_ID) ? Dilation++ : Dilation += 0;
                (UnivBuffer[(y + 1)][(x + 1)].B_ID != Air.B_ID) ? Dilation++ : Dilation += 0;
            }
            else {
                (UnivBuffer[(y - 1)][(x - 1)].B_ID == Air.B_ID) ? Errosion++ : Errosion += 0;
                (UnivBuffer[(y - 1)][(x)].B_ID == Air.B_ID) ? Errosion++ : Errosion += 0;
                (UnivBuffer[(y - 1)][(x + 1)].B_ID == Air.B_ID) ? Errosion++ : Errosion += 0;
                (UnivBuffer[(y)][(x - 1)].B_ID == Air.B_ID) ? Errosion++ : Errosion += 0;
                (UnivBuffer[(y)][(x + 1)].B_ID == Air.B_ID) ? Errosion++ : Errosion += 0;
                (UnivBuffer[(y + 1)][(x - 1)].B_ID == Air.B_ID) ? Errosion++ : Errosion += 0;
                (UnivBuffer[(y + 1)][(x)].B_ID == Air.B_ID) ? Errosion++ : Errosion += 0;
                (UnivBuffer[(y + 1)][(x + 1)].B_ID == Air.B_ID) ? Errosion++ : Errosion += 0;

            }
            if (Dilation >= 4) {
                Univ[y][x] = ReplacementBlock;
            }
            if (Errosion >= 5) {
                Univ[y][x] = Air;
            }

        }

    }

}

void DrawVisibleWorld(Block **Univ, Camera2D camera) {
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