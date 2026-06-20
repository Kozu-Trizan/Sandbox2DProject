#pragma once
#include "Constant.h"

class Player {
private:
    int PosX, PosY;
    int HeightP, WidthP;
    Color colorP;
    bool IsInAir;
    int MineRange;
    Rectangle player;

public:
    void UpdatePosX(int velocity);

    void UpdatePosY(int velocity);

    void Jump(int JumpStep);

    void DrawPlayer();

    Rectangle& GetPlayer();

    bool PlayerCanFall();

    bool BlockInRange(std::vector<int> Pos);

    bool BlockIsVisible(std::vector<int> Pos);

    Player();

    Player(int PosX, int PosY);
};