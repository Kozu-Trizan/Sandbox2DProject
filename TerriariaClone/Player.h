#pragma once
#include "Constant.h"
#include <vector>
#include "Inventory.h"

class Block;

struct BlockPos;

class Player {
private:
    float PosX, PosY;
    int HeightP, WidthP;
    Color colorP;
    bool IsInAir;
    int MineRange;
    Rectangle player;
    int LeftBlock;
    int RightBlock;
    int AboveBlock;
    int BelowBlock;
    float VelocityY;
    bool OnGround;
    float animationRow;
    float jumpAnimationRow;
    Texture2D walkTexture;
    Texture2D jumpTexture;
    int currentFrame;
    float animationTimer;
    float frameDuration;
    float Damage;
    float HP;
    bool WasMining;

    Inventory inventory;
    int HeldItemCellNo;

public:
    std::vector<int> GetSize();

    void Spawn();

    void UpdatePosX(int velocity);

    void UpdatePosY(int velocity);

    void Jump(int JumpStep);

    void UpdateGravity();

    void UpdateWalkAnimation();

    void SetAnimationRow(float row);

    void SetJumpAnimationRow(float row);

    void DrawPlayer();

    Rectangle& GetPlayer();

    bool PlayerCanFall();

    bool BlockInRange(BlockPos Pos);

    bool BlockIsVisible(BlockPos Pos);

    bool PlayerOccupiesBlock(BlockPos Pos);

    Player();

    Player(int PosX, int PosY);

    ~Player();

    float getx();

    float gety();

    void ChangeMiningStatus(bool status);
    void Mine(Camera2D camera, BlockPos PosMouseMap, Block& MineBlock);

    void PlaceBlock(Camera2D camera, BlockPos PosMouseMap, Block& MineBlock);

    friend void Move(Player& player, int velocity);
};