#include <iostream>
#include <vector>
#include <cstring>
#include <random>

#include "Constant.h"
#include "Blocks.h"
#include "MapGen.h"
#include "Player.h"
#include "IsSolid.h"
#include "Move.h"
#include "GetTexture.h"

Block** Universe = nullptr;

int main() {  
    Block* UniverseData = new Block[UniverseHeight * UniverseWidth]();
    Universe = new Block * [UniverseHeight];
    for (int i = 0; i < UniverseHeight; i++)
    {
        Universe[i] = UniverseData + i * UniverseWidth;
    }
    InitWindow(ScreenWidth, ScreenHeight, "TerriariaProject");
    SetTargetFPS(60);
    InitializeTexture();

    GenerateVisibleWorld(Universe, MAP_FREQ, MAP_AMP, MAP_BASE_LEVEL, MAP_OCTAVE);

    Player player;
    int velocity = 2;
    int JumpHeight = 10;
    player.UpdateWalkAnimation();

    player.Spawn();
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
        

        if (player.PlayerCanFall()) {
            player.UpdatePosY(velocity);
        }

        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
        player.SetAnimationRow(320.0f);
        player.SetJumpAnimationRow(320.0f);
        player.UpdateWalkAnimation();

        Move(player,velocity);
       }
        else if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
        player.SetAnimationRow(64.0f);
        player.SetJumpAnimationRow(64.0f);
        player.UpdateWalkAnimation();


        Move(player,-velocity);
        }

        if (IsKeyPressed(KEY_SPACE)) {
            player.Jump(JumpHeight);
        }
         player.UpdateGravity();

         Vector2 PosMouse = GetMousePosition(); // MousePosition function returns coordinates in Screen Space.
         PosMouse = GetScreenToWorld2D(PosMouse, camera); // To convert the Screen space coordinates to world space coordinates that the logic is compatible with
         BlockPos PosMouseMap = { static_cast<int>(PosMouse.x / BLOCK_SIZE), static_cast<int>(PosMouse.y / BLOCK_SIZE) };
         Block& TargetBlock = Universe[PosMouseMap.y][PosMouseMap.x];
        
         // Breaking Blocks
         player.Mine(camera, PosMouseMap, TargetBlock);

         // Placing Blocks
         player.PlaceBlock(camera, PosMouseMap, TargetBlock);
        

    //--------------------------------------------------------------------------------------------------------------------

        BeginDrawing();

        // Map Generation
        ClearBackground(WHITE);
        
        DrawBackground();
        DrawParallax(camera);

        BeginMode2D(camera);
        
        DrawVisibleWorld(Universe, camera);

        player.DrawPlayer();

        EndMode2D();

        EndDrawing();
    // ---------------------------------------------------------------------------------------------------------------------------
    }
    DeInitializeTexture();
    CloseWindow();
    delete[] Universe[0]; // frees the contiguous data block
    delete[] Universe;
    return 0;
    
}
//nix-shell -p raylib gcc --run \ 'g++ TerriariaClone.cpp Player.cpp MapGen.cpp -o game \-lraylib -lm -ldl -lpthread -lGL && ./game'