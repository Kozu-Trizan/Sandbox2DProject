#include <iostream>
#include <raylib.h>
#include <vector>

const int ScreenWidth = 1024;
const int ScreenHeight = 512;
const int BLOCK_SIZE = 64;

Color SKY = { 135, 206, 235, 1 };


float WorldMap[][16] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
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
        bool state = (WorldMap[PosY / BLOCK_SIZE + 2][PosX / BLOCK_SIZE] == 0); // Player Height = 2* Block size so the + 2 in array index
        (state) ? this->IsInAir = true : this->IsInAir = false;
        return state;
    }

    bool BlockInRange(std::vector<int> Pos) {
        bool InRangeHorizontal = std::abs(this->PosX / BLOCK_SIZE - Pos[0]) <= this->MineRange;
        bool InRangeFromHead = (this->PosY / BLOCK_SIZE - Pos[1]) <= this->MineRange && (this->PosY / BLOCK_SIZE - Pos[1]) >= 0;
        bool InRangeFromFoot = (Pos[1] - this->PosY / BLOCK_SIZE - 2) <= this->MineRange && (Pos[1] - this->PosY / BLOCK_SIZE - 2) >= 0;
        return (InRangeHorizontal && (InRangeFromHead || InRangeFromFoot));
    }

    bool BlockIsVisible(std::vector<int> Pos) {
        for (int i = Pos[1]; i > this->PosY / BLOCK_SIZE + 2; i--){
            if (WorldMap[i][Pos[0]] != 0) {
                return false;
            }
        }
        return true;
    }

    Player(int PosX, int PosY) {
        this->PosX = PosX;
        this->PosY = PosY;
        this->HeightP = 2 * BLOCK_SIZE;
        this->WidthP = BLOCK_SIZE;
    }
};


int main() {
    Player player(0, 0);
    int velocity = 4;
    InitWindow(ScreenWidth, ScreenHeight, "TerriariaProject");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();

        ClearBackground(SKY);
        for (int x = 0; x < 8; x++){
            for (int  y = 0; y < 16; y++){
                if (WorldMap[x][y] == 0) {
                    continue;
                }
                Rectangle Block(y * BLOCK_SIZE, x * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE);
                DrawRectangleRec(Block, BROWN);
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
            player.Jump(BLOCK_SIZE);
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