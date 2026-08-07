#pragma once
#include "Constant.h"
#include <vector>

class Block;

struct BlockPos;

class Player {
private:
    int PosX, PosY;
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
    float HP = 10;
    bool WasMining;
    int FallHeight;
    float FallDistance;
    float BlocksFallen;
    
public:
    bool IsDead = false;

    void Spawn();

    void SetHP(int hp);

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


    Player();

    Player(int PosX, int PosY);

    ~Player();

    int getx();

    int gety();

    void ChangeMiningStatus(bool status);
    void Mine(Camera2D camera);

    friend void Move(Player& player, int velocity);
    friend void Mine(BlockPos Pos, Block** Univ, Player player);
};
