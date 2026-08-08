#include <vector>
#include <random>
#include <numeric>
#include <cstring>
#include<algorithm>
#include "Constant.h"
#include "Blocks.h"
#include "MapGen.h"
#include "WavefrontPropagation.h"
#include"GetTexture.h"
Rectangle GetBlockSourceRectangle(std::uint8_t blockID);

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

void RemoveOneBlockSpike(Block **Univ, Block **UnivBuffer) {
    // Copy current state into buffer
    for (int i = 0; i < UniverseHeight; i++) {
        std::memcpy(UnivBuffer[i], Univ[i], sizeof(Block) * UniverseWidth);
    }

    // Check except boundaries to prevent array index out of range
    for (int x = 1; x < UniverseWidth - 1; x++){
        for (int y = 1; y < UniverseHeight - 1; y++) {
            if (UnivBuffer[y][x].B_ID == Air.B_ID) continue;
            
            bool spikeRemoved = false;
            if (
                UnivBuffer[y][x + 1].B_ID == Air.B_ID && // Is Right Neighbour Air
                UnivBuffer[y][x - 1].B_ID == Air.B_ID && // Is Left Neighbour Air
                UnivBuffer[y - 1][x].B_ID == Air.B_ID // Is Top Neighbour Air
                ) {
                std::uint8_t InheritedWallID = 0;
                // Inherit WallID from neighbors to prevent holes in caves
                const int nx[4] = { x-1, x+1, x, x };
                const int ny[4] = { y, y, y-1, y+1 };
                for (int n = 0; n < 4; n++) {
                    if (UnivBuffer[ny[n]][nx[n]].B_ID == Air.B_ID && UnivBuffer[ny[n]][nx[n]].WallID != 0) {
                        InheritedWallID = UnivBuffer[ny[n]][nx[n]].WallID;
                        break;
                    }
                }
                Univ[y][x] = Air;
                Univ[y][x].WallID = InheritedWallID;
                spikeRemoved = true;
            }

            // Make blocks continuous
            if (!spikeRemoved) {
                if (
                    UnivBuffer[y][x + 1].B_ID == UnivBuffer[y][x - 1].B_ID && 
                    UnivBuffer[y][x].B_ID != UnivBuffer[y][x + 1].B_ID
                    ) {
                    // Copy the entire block to preserve its base stats and WallID if applicable
                    Univ[y][x] = UnivBuffer[y][x + 1];
                }
            }
        }
    }
}

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
                // Only calculate cave noise deep enough underground
                if (y > SurfaceY + 30) {
                    float CaveNoise = NoiseForCave(Cave, x, y, CAVE_FREQ_X, CAVE_FREQ_Y, CAVE_AMP, CAVE_OCTAVE);
                    if (CaveNoise > CAVE_THRESHOLD) {
                        Univ[y][x] = Air;
                        Univ[y][x].WallID = Dirt.WallID;
                        Univ[y][x].SetLightValue(0);
                        continue;
                    }
                }
                Univ[y][x] = Dirt;
            }
        }
    }

    // Post processing — allocate buffer once and reuse across passes
    Block** UnivBuffer = new Block*[UniverseHeight];
    for (int i = 0; i < UniverseHeight; i++) {
        UnivBuffer[i] = new Block[UniverseWidth];
    }

    for (int i = 0; i < 3; i++)
    {
        Automata(Univ, UnivBuffer);
        RemoveOneBlockSpike(Univ, UnivBuffer);
    }

    // Free the buffer
    for (int i = 0; i < UniverseHeight; i++) {
        delete[] UnivBuffer[i];
    }
    delete[] UnivBuffer;

    QueueLightSources(0, 0, UniverseWidth, UniverseHeight);
    BreadthFirstSearch(0, 0, 0, 0, UniverseWidth, UniverseHeight);
}

void Automata(Block **Univ, Block **UnivBuffer) {
    // Copy current state into buffer (proper row-by-row deep copy)
    for (int i = 0; i < UniverseHeight; i++) {
        std::memcpy(UnivBuffer[i], Univ[i], sizeof(Block) * UniverseWidth);
    }

    Block ReplacementBlock = Dirt;
    for (int x = 2; x < UniverseWidth - 2; x++){
        for (int y = 2; y < UniverseHeight - 2; y++){
            int Dilation = 0;   // Reset per cell
            int Errosion = 0;   // Reset per cell

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
            if (Dilation >= 6) {
                Univ[y][x] = ReplacementBlock;
            }
            if (Errosion >= 6) {
                std::uint8_t InheritedWallID = 0;
                const int nx[8] = { x-1, x+1, x, x, x-1, x+1, x-1, x+1 };
                const int ny[8] = { y, y, y-1, y+1, y-1, y-1, y+1, y+1 };
                for (int n = 0; n < 8; n++) {
                    if (UnivBuffer[ny[n]][nx[n]].B_ID == Air.B_ID &&
                        UnivBuffer[ny[n]][nx[n]].WallID != 0) {
                        InheritedWallID = UnivBuffer[ny[n]][nx[n]].WallID;
                        break;
                    }
                }
                Univ[y][x] = Air;
                Univ[y][x].WallID = InheritedWallID;
            }
        }
    }
}
Rectangle GetBlockSourceRectangle(std::uint8_t blockID)
{
    switch (blockID)
    {
    case 1: // Dirt
        return { 0, 29*16 , 16, 16 };

    case 2: // Grass
        return { 1*16 , 29*16, 16, 16 };

    case 3: // Stone
        return { 16, 0, 16, 16 };

    default:
        return { 0, 0, 0, 0 };
    }
}

void DrawVisibleWorld(Block **Univ, Camera2D camera) {
    Vector2 TopLeftBound = GetScreenToWorld2D(Vector2{ 0, 0 }, camera);
    Vector2 BottomRightBound = GetScreenToWorld2D({ (float)ScreenWidth, (float)ScreenHeight }, camera);

    for (int y = static_cast<int>((TopLeftBound.y / BLOCK_SIZE)); y < static_cast<int>(BottomRightBound.y / BLOCK_SIZE) + 1; y++) { // + 1 in order to avoid clipping at the right and bottom boundaries.
        if (y < 0 || y >= UniverseHeight) continue;
        for (int x = static_cast<int>(TopLeftBound.x / BLOCK_SIZE); x < static_cast<int>(BottomRightBound.x / BLOCK_SIZE) + 1; x++) {
            if (x < 0 || x >= UniverseWidth) continue;
            if (Univ[y][x].B_ID == 0) {
                if (Univ[y][x].WallID != 0) {
                    DrawRectangle(x * BLOCK_SIZE, y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, { 84, 69, 49, 255 }); // Walls
                }
                continue;
            }
          
            Rectangle source = GetBlockSourceRectangle(Univ[y][x].B_ID);

            Rectangle destination = {
                x * BLOCK_SIZE,
                y * BLOCK_SIZE,
                BLOCK_SIZE,
                BLOCK_SIZE
            };

            DrawTexturePro(
                BlockTexture,
                source,
                destination,
                { 0, 0 },
                0.0f,
                WHITE
            );
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
