#include <iostream>
#include <raylib.h>
#include <vector>

const int ScreenWidth = 1024;
const int ScreenHeight = 512;
const int BLOCK_SIZE = 16;

Color SKY = { 135, 206, 235, 1 };


float WorldMap[ScreenHeight / BLOCK_SIZE][ScreenWidth / BLOCK_SIZE]; // World Map Array
struct Block {
    int B_ID = 0;
};

class Player {
private:
    int PosX, PosY;
    int HeightP, WidthP;
    Color colorP = RAYWHITE;
    bool IsInAir = true;
    int MineRange = 2;

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
        DrawRectangle(this->PosX, this->PosY, this->WidthP, this->HeightP, colorP);
    }
    
    bool PlayerCanFall() {
        bool state = (WorldMap[PosY / BLOCK_SIZE + 1][PosX / BLOCK_SIZE] == 0); // Player Height = 1 * Block size so the + 1 in array index
        (state) ? this->IsInAir = true : this->IsInAir = false;
        return state;
    }

    bool BlockInRange(std::vector<int> Pos) {
        bool InRangeHorizontal = std::abs(this->PosX / BLOCK_SIZE - Pos[0]) <= this->MineRange;
        int Lower = std::min(this->PosY / BLOCK_SIZE, Pos[1]);
        int Upper = std::max(this->PosY / BLOCK_SIZE, Pos[1]);
        bool InRangeAbove = ((Upper - Lower) <= this->MineRange);
        //bool InRangeBelow = ((Pos[1] - this->PosY / BLOCK_SIZE - 1) <= this->MineRange) && ((Pos[1] - this->PosY / BLOCK_SIZE - 1) >= 0);
        return (InRangeHorizontal && InRangeAbove);
    }

    bool BlockIsVisible(std::vector<int> Pos) {
        int playerBlockY = this->PosY / BLOCK_SIZE;
        // Check from player down to target block, excluding both endpoints
        int startY = std::min(playerBlockY, Pos[1]);
        if (startY == Pos[1]) startY++; // When mining below check from starting(Player) Y level, when mining above check from one level excluding the leven containing the block to mine
        int endY = std::max(playerBlockY, Pos[1]);
        for (int i = startY; i < endY; i++){
            if (WorldMap[i][Pos[0]] != 0) {
                return false;
            }
        }
        return true;
    }

    Player(int PosX, int PosY) {
        this->PosX = PosX;
        this->PosY = PosY;
        this->HeightP = BLOCK_SIZE;
        this->WidthP = BLOCK_SIZE;
    }
};

void GenerateMap(float Map[][ScreenWidth / BLOCK_SIZE]) {
    Block Air = { 0 };
    Block Grass = { 2 };
    Block Dirt = { 1 };
    int RandNum(0);
    for (int y = 0; y < ScreenHeight / BLOCK_SIZE; y++){
        for (int x = 0; x < ScreenWidth / BLOCK_SIZE; x++){
            RandNum = static_cast<int>(10 * (x + 1) * std::sin(y) + 10 * (x + 1) * std::cos(y) + 10 * (x + 1) * std::sin(y)) % 3;
            switch (RandNum) {
            case 0:
                Map[y][x] = 0;
                break;
            case 1:
                Map[y][x] = 1;
                break;
            case 2:
                Map[y][x] = 2;
                break;
            }
        }
    }
}


int main() {
    Player player(0, 0);
    int velocity = 2;
    int JumpHeight = 2 * BLOCK_SIZE;
    InitWindow(ScreenWidth, ScreenHeight, "TerriariaProject");
    SetTargetFPS(60);

    GenerateMap(WorldMap);

    while (!WindowShouldClose()) {
        BeginDrawing();

        // Map Generation
        ClearBackground(SKY);

        for (int y = 0; y < ScreenHeight / BLOCK_SIZE; y++){
            for (int  x = 0; x < ScreenWidth / BLOCK_SIZE; x++){
                if (WorldMap[y][x] == 0) {
                    continue;
                }
                else if (WorldMap[y][x] == 1) {
                    Rectangle Block(x * BLOCK_SIZE, y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE);
                    DrawRectangleRec(Block, BROWN);
                }
                else if (WorldMap[y][x] == 2) {
                    Rectangle Block(x * BLOCK_SIZE, y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE);
                    DrawRectangleRec(Block, GREEN);
                }
            }
        }

        // Player
        player.DrawPlayer();
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
            Vector2 PosMouse = GetMousePosition();
            // Double casting as we want just the integer part but Vector2 object expects float
            std::vector<int> PosMouseMap = { static_cast<int>(PosMouse.x / BLOCK_SIZE), static_cast<int>(PosMouse.y / BLOCK_SIZE) };

            if (WorldMap[PosMouseMap[1]][PosMouseMap[0]] != 0 && player.BlockInRange(PosMouseMap) && player.BlockIsVisible(PosMouseMap)) {
                WorldMap[PosMouseMap[1]][PosMouseMap[0]] = 0;
            }

        }

        EndDrawing();
    }
    CloseWindow();
    return 0;
    
}