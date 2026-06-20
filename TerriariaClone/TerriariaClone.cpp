#include <iostream>
#include <vector>
#include <cstring>
#include <random>

#include "Constant.h"
#include "Blocks.h"
#include "MapGen.h"
#include "Player.h"

Block Universe[UniverseHeight][UniverseWidth] = {};


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