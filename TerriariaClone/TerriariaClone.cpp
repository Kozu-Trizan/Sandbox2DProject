#include <iostream>
#include <raylib.h>
#include <vector>
#include <cstdint>
#include <cstring>
#include <random>

const int UniverseWidth = 4200; // In terms of Blocks
const int UniverseHeight = 1200;

const int ScreenWidth = 1024;
const int ScreenHeight = 512;
const int BLOCK_SIZE = 16;

Color SKY = { 135, 206, 235, 1 };

struct Block {
    std::uint8_t B_ID = 0; // 255 unique block values.
};
Block Air = { 0 };
Block Dirt = { 1 };
Block Grass = { 2 };

Block AllBlocks[3] = { Air, Dirt, Grass };

Block Universe[UniverseHeight][UniverseWidth] = {};

class Player {
private:
    int PosX, PosY;
    int HeightP, WidthP;
    Color colorP = RAYWHITE;
    bool IsInAir = true;
    int MineRange = 2;
    Rectangle player = {0};

public:
    void UpdatePosX(int velocity) {
        this->PosX += velocity;
    }

    void UpdatePosY(int velocity) {
        this->PosY += velocity;
    }

    void Jump(int JumpStep) {
        if (!this->IsInAir) {
            this->PosY -= JumpStep;
        }
    }

    void DrawPlayer() {
        this->player = { (float)this->PosX, (float)this->PosY, (float)this->WidthP, (float)this->HeightP };
        DrawRectangleRec(player, RAYWHITE);
    }

    Rectangle& GetPlayer() {
        return this->player;
    }
    
    bool PlayerCanFall() {
        bool state = (Universe[PosY / BLOCK_SIZE + 1][PosX / BLOCK_SIZE].B_ID == Air.B_ID); // Player Height = 1 * Block size so the + 1 in array index
        (state) ? this->IsInAir = true : this->IsInAir = false;
        return state;
    }

    bool BlockInRange(std::vector<int> Pos) {
        bool InRangeHorizontal = std::abs(this->PosX / BLOCK_SIZE - Pos[0]) <= this->MineRange;
        int Lower = std::min(this->PosY / BLOCK_SIZE, Pos[1]);
        int Upper = std::max(this->PosY / BLOCK_SIZE, Pos[1]);
        bool InRangeAbove = ((Upper - Lower) <= this->MineRange);
        return (InRangeHorizontal && InRangeAbove);
    }

    bool BlockIsVisible(std::vector<int> Pos) {
        int playerBlockY = this->PosY / BLOCK_SIZE;
        // Check from player down to target block, excluding both endpoints
        int startY = std::min(playerBlockY, Pos[1]);
        if (startY == Pos[1]) startY++; // When mining below check from starting(Player) Y level, when mining above check from one level excluding the leven containing the block to mine
        int endY = std::max(playerBlockY, Pos[1]);
        for (int i = startY; i < endY; i++){
            if (Universe[i][Pos[0]].B_ID != 0) {
                return false;
            }
        }
        return true;
    }

    Player() {
        this->PosX = 0;
        this->PosY = 0;
        this->HeightP = 0;
        this->WidthP = 0;
    }

    Player(int PosX, int PosY) {
        this->PosX = PosX;
        this->PosY = PosY;
        this->HeightP = BLOCK_SIZE;
        this->WidthP = BLOCK_SIZE;
    }
};

class Perlin1D {
private:
    std::vector<int> Gradients;
    
    float FadeFunc(float t) {
        return t * t * t * (t * (t * 6 - 15) + 10);
    }

    float Interpolation(float a, float b, float t) {
        return a + t * (b - a);
    }

public:
    Perlin1D() {
        Gradients.resize(UniverseWidth);

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(0, 1);

        for (int i = 0; i < UniverseWidth; i++){
            Gradients[i] = dist(gen) ? 1 : -1;
        }
    }

    float Noise(float CoordX) {
        int FloorX = static_cast<int>(std::floorf(CoordX));
        int CeilX = FloorX + 1;

        float DistanceLeft = CoordX - FloorX;
        float DistanceRight = CoordX - CeilX;

        int GradLeft = Gradients[FloorX % UniverseWidth];
        int GradRight = Gradients[CeilX % UniverseWidth];

        float t = FadeFunc(DistanceLeft);

        return Interpolation(DistanceLeft * GradLeft, DistanceRight * GradRight, t);
    }
};

static void GenerateVisibleWorld(Block Univ[][UniverseWidth], float Frequency= 0.01f, float Amplitude= 100.0f, int BaseLevel=400, int Octaves=1) {
    
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
            else if(y - SurfaceY == 0) {
                Univ[y][x] = Grass;
            }
            else {
                Univ[y][x] = Dirt;
            }
        }
    }
}

static void DrawVisibleWorld(Block Univ[][UniverseWidth], Camera2D camera) {
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

static bool WorldBoundaryReached(Camera2D camera) {
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

int main() {  
    InitWindow(ScreenWidth, ScreenHeight, "TerriariaProject");
    SetTargetFPS(60);

    GenerateVisibleWorld(Universe, 0.002f, 100.0f, 400, 8);

    // Player Spawn Logic
    int spawnX = UniverseWidth / 2;
    int spawnY = 0;
    while (spawnY < UniverseHeight && Universe[spawnY][spawnX].B_ID == 0) {
        spawnY++;
    }
    spawnY--; // Prevent Spawnning on a block

    Player player(spawnX * BLOCK_SIZE, spawnY * BLOCK_SIZE);
    int velocity = 2;
    int JumpHeight = 2 * BLOCK_SIZE;
    player.DrawPlayer();

    //Camera Configurations
    Camera2D camera = { 0 };
    camera.target = { player.GetPlayer().x + BLOCK_SIZE / 2, player.GetPlayer().y + BLOCK_SIZE / 2 };
    camera.offset = { ScreenWidth / 2.0f, ScreenHeight / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    float MaxZoom = 3.0f;
    float MinZoom = 0.1f;

    while (!WindowShouldClose()) {
        // Update
    //--------------------------------------------------------------------------------------------------------------------
        camera.target = { player.GetPlayer().x + BLOCK_SIZE / 2, player.GetPlayer().y + BLOCK_SIZE / 2 };
        // Camera zoom controls
        float PrevZoom = camera.zoom; 
        camera.zoom = expf(logf(camera.zoom) + ((float)GetMouseWheelMove() * 0.1f));
        
        // Check if zoom boundaries are met
        if (camera.zoom > MaxZoom) {
            camera.zoom = MaxZoom;
        }
        else if (camera.zoom < MinZoom) {
            camera.zoom = MinZoom;
        }
        
        if (WorldBoundaryReached(camera)) {
            camera.zoom = PrevZoom;
        }
        

        // Player fall
        if (player.PlayerCanFall()) {
            player.UpdatePosY(velocity);
        }

        //Horizontal Movement
        if (IsKeyDown(KEY_RIGHT)) {
            player.UpdatePosX(velocity);
        }
        else if (IsKeyDown(KEY_LEFT)) {
            player.UpdatePosX(-velocity);
        }

        // Vertical Movement
        if (IsKeyPressed(KEY_SPACE)) {
            player.Jump(JumpHeight);
        }

        // Breaking Blocks
        DrawText(TextFormat("X: %.2f, Y: %.2f", GetMousePosition().x, GetMousePosition().y), 0, 0, 20, RED);
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 PosMouse = GetMousePosition(); // MousePosition function returns coordinates in Screen Space.
            PosMouse = GetScreenToWorld2D(PosMouse, camera); // To convert the Screen space coordinates to world space coordinates that the logic is compatible with
            std::vector<int> PosMouseMap = { static_cast<int>(PosMouse.x / BLOCK_SIZE), static_cast<int>(PosMouse.y / BLOCK_SIZE) };

            if (Universe[PosMouseMap[1]][PosMouseMap[0]].B_ID != 0 && player.BlockInRange(PosMouseMap) && player.BlockIsVisible(PosMouseMap)) {
                Universe[PosMouseMap[1]][PosMouseMap[0]].B_ID = 0;
            }

        }
    //--------------------------------------------------------------------------------------------------------------------

        BeginDrawing();

        // Map Generation
        ClearBackground(SKY);

        BeginMode2D(camera);
        
        DrawVisibleWorld(Universe, camera);

        player.DrawPlayer();

        EndMode2D();

        EndDrawing();
    // ---------------------------------------------------------------------------------------------------------------------------
    }
    CloseWindow();
    return 0;
    
}