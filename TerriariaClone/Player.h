#pragma once
#include "Constant.h"
#include <vector>
#include<raylib.h>


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

public:
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

    bool BlockInRange(std::vector<int> Pos);

    bool BlockIsVisible(std::vector<int> Pos);

    Player();

    Player(int PosX, int PosY);

    ~Player();

    int getx();

    int gety();
};