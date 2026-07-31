#pragma once
#include "Constant.h"

struct Block;

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
    float Damage;
    float HP;
    bool WasMining;

public:
    void Spawn();

    void UpdatePosX(int velocity);

    void UpdatePosY(int velocity);

    void Jump(int JumpStep);

    void UpdateGravity();

    void DrawPlayer();

    Rectangle& GetPlayer();

    bool PlayerCanFall();

    bool BlockInRange(BlockPos Pos);

    bool BlockIsVisible(BlockPos Pos);

    Player();

    Player(int PosX, int PosY);

    int getx();

    int gety();

    void ChangeMiningStatus(bool status);
    void Mine(Camera2D camera);

    friend void Move(Player& player, int velocity);
    friend void Mine(BlockPos Pos, Block** Univ, Player player);
};