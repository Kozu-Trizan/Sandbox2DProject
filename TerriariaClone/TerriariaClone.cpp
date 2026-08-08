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
#include "LightMap.h"
#include "ui.h"

Block** Universe = nullptr;
int main() {  
    Block* UniverseData = new Block[UniverseHeight * UniverseWidth]();
    Universe = new Block * [UniverseHeight];
    for (int i = 0; i < UniverseHeight; i++)
    {
        Universe[i] = UniverseData + i * UniverseWidth;
    }
    InitWindow(ScreenWidth, ScreenHeight, "TerriariaProject");
    InitAudioDevice();
    SetTargetFPS(60);
    InitializeTexture();
    InitLightMap();

    GenerateVisibleWorld(Universe, MAP_FREQ, MAP_AMP, MAP_BASE_LEVEL, MAP_OCTAVE);

    Player player;
    int velocity = 2;
    int JumpHeight = 6;
    player.UpdateWalkAnimation();

    player.Spawn();
    player.DrawPlayer();
    Music bgMusic = LoadMusicStream("assets/music/JNKm6waftWQ-f287bdc79a08e3dbfa6be51ce676564e649.mp3");
    //Camera Configurations
    Camera2D camera = { 0 };

    camera.target = { player.GetPlayer().x + BLOCK_SIZE / 2, player.GetPlayer().y + BLOCK_SIZE / 2 };
    camera.offset = { ScreenWidth / 2.0f, ScreenHeight / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    float MaxZoom = 3.0f;
    float MinZoom = 0.1f;
    PlayMusicStream(bgMusic);
    while (!WindowShouldClose()) {
        UpdateMusicStream(bgMusic);
        // Update
    //--------------------------------------------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------------------------------------------

        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
          if ((IsKeyDown(KEY_RIGHT) && IsKeyDown(KEY_LEFT_SHIFT)) ||( IsKeyDown(KEY_D)&& IsKeyDown(KEY_LEFT_SHIFT)) )
          {
            player.SetAnimationRow(2*320.0f);
            player.SetJumpAnimationRow(2*320.0f);
            player.UpdateWalkAnimation();

             Move(player,2*velocity);
          }
        player.SetAnimationRow(320.0f);
        player.SetJumpAnimationRow(320.0f);
        player.UpdateWalkAnimation();

        Move(player,velocity);
       }
        else if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
           if ((IsKeyDown(KEY_LEFT) && IsKeyDown(KEY_LEFT_SHIFT)) ||( IsKeyDown(KEY_A)&& IsKeyDown(KEY_LEFT_SHIFT)) )
          {
            player.SetAnimationRow(2*320.0f);
            player.SetJumpAnimationRow(2*320.0f);
            player.UpdateWalkAnimation();

             Move(player,-2*velocity);
          }
        player.SetAnimationRow(64.0f);
        player.SetJumpAnimationRow(64.0f);
        player.UpdateWalkAnimation();
        Move(player,-velocity);
        }

        if (IsKeyDown(KEY_K))
        {
          player.IsDead = true;
        }
        if (IsKeyPressed(KEY_UP)) {
            player.SoundVolume=player.SoundVolume+0.5f;
            SetMusicVolume(bgMusic,player.SoundVolume);
        }
        if (IsKeyPressed(KEY_DOWN)) {
            player.SoundVolume=player.SoundVolume-0.5f;
            SetMusicVolume(bgMusic,player.SoundVolume);
        }
        if (IsKeyPressed(KEY_SPACE)) {
            player.Jump(JumpHeight);
        }
        if(player.IsDead == true)
        {
          GenerateVisibleWorld(Universe,
                         MAP_FREQ,
                         MAP_AMP,
                         MAP_BASE_LEVEL,
                         MAP_OCTAVE);
          player.IsDead = false;
          player.SetHP(100);
          player.Spawn();
        }
         player.UpdateGravity();

         // Camera Update
         camera.target = { player.getx() + BLOCK_SIZE / 2, player.gety() + BLOCK_SIZE / 2 };
         
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


         Vector2 PosMouse = GetMousePosition(); // MousePosition function returns coordinates in Screen Space.
         PosMouse = GetScreenToWorld2D(PosMouse, camera); // To convert the Screen space coordinates to world space coordinates that the logic is compatible with
         BlockPos PosMouseMap = { static_cast<int>(PosMouse.x / BLOCK_SIZE), static_cast<int>(PosMouse.y / BLOCK_SIZE) };
         Block& TargetBlock = Universe[PosMouseMap.y][PosMouseMap.x];
         Rectangle Outline = { 0 };
         if (TargetBlock.GetLightDecay() > 0) {
             Outline = { static_cast<float>(static_cast<int>(PosMouse.x / BLOCK_SIZE) * BLOCK_SIZE), static_cast<float>(static_cast<int>(PosMouse.y / BLOCK_SIZE) * BLOCK_SIZE), (float)BLOCK_SIZE, (float)BLOCK_SIZE };
         }
        
         // Breaking Blocks
         player.Mine(camera, PosMouseMap, TargetBlock);

         int HeldItemCellNo = GetKeyPressed();
         if (
             HeldItemCellNo == KEY_ZERO ||
             HeldItemCellNo == KEY_ONE ||
             HeldItemCellNo == KEY_TWO ||
             HeldItemCellNo == KEY_THREE ||
             HeldItemCellNo == KEY_FOUR ||
             HeldItemCellNo == KEY_FIVE ||
             HeldItemCellNo == KEY_SIX ||
             HeldItemCellNo == KEY_SEVEN ||
             HeldItemCellNo == KEY_EIGHT ||
             HeldItemCellNo == KEY_NINE
             ) player.ChangeHeldItem(HeldItemCellNo - 48); // Raylib KEY_ZERO = 48

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
        DrawLightMap(Universe, camera);
        if (TargetBlock.B_ID != Air.B_ID) {
            DrawRectangleLinesEx(Outline, 0.4f, LIGHTGRAY);
        }
        

        EndMode2D();

        DrawPlayerHealthBar(player, 20.0f, 20.0f); // 2. Call healthbar function here!

        EndDrawing();
        // ---------------------------------------------------------------------------------------------------------------------------
    }
    DeInitLightMap();
    UnloadMusicStream(bgMusic);
    CloseAudioDevice(); 
    DeInitializeTexture();
    CloseWindow();
    delete[] Universe[0]; // frees the contiguous data block
    delete[] Universe;
    return 0;

}
//nix-shell -p raylib gcc --run \ 'g++ TerriariaClone.cpp Player.cpp MapGen.cpp -o game \-lraylib -lm -ldl -lpthread -lGL && ./game'
