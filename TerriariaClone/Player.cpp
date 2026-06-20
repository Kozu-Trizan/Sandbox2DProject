#include "Constant.h"
#include "Blocks.h"
#include "MapGen.h"
#include "Player.h"

void Player::UpdatePosX(int velocity) {
    this->PosX += velocity;
}

void Player::UpdatePosY(int velocity) {
    this->PosY += velocity;
}

void Player::Jump(int JumpStep) {
    if (!this->IsInAir) {
        this->PosY -= JumpStep;
    }
}

void Player::DrawPlayer() {
    this->player = { (float)this->PosX, (float)this->PosY, (float)this->WidthP, (float)this->HeightP };
    DrawRectangleRec(player, RAYWHITE);
}

Rectangle& Player::GetPlayer() {
    return this->player;
}

bool Player::PlayerCanFall() {
    bool state = (Universe[PosY / BLOCK_SIZE + 1][PosX / BLOCK_SIZE].B_ID == Air.B_ID); // Player Height = 1 * Block size so the + 1 in array index
    (state) ? this->IsInAir = true : this->IsInAir = false;
    return state;
}

bool Player::BlockInRange(std::vector<int> Pos) {
    bool InRangeHorizontal = std::abs(this->PosX / BLOCK_SIZE - Pos[0]) <= this->MineRange;
    int Lower = std::min(this->PosY / BLOCK_SIZE, Pos[1]);
    int Upper = std::max(this->PosY / BLOCK_SIZE, Pos[1]);
    bool InRangeAbove = ((Upper - Lower) <= this->MineRange);
    return (InRangeHorizontal && InRangeAbove);
}

bool Player::BlockIsVisible(std::vector<int> Pos) {
    int playerBlockY = this->PosY / BLOCK_SIZE;
    // Check from player down to target block, excluding both endpoints
    int startY = std::min(playerBlockY, Pos[1]);
    if (startY == Pos[1]) startY++; // When mining below check from starting(Player) Y level, when mining above check from one level excluding the leven containing the block to mine
    int endY = std::max(playerBlockY, Pos[1]);
    for (int i = startY; i < endY; i++) {
        if (Universe[i][Pos[0]].B_ID != 0) {
            return false;
        }
    }
    return true;
}

Player::Player() {
    this->PosX = 0;
    this->PosY = 0;
    this->HeightP = 0;
    this->WidthP = 0;
    this->colorP = RAYWHITE;
    this->IsInAir = true;
    this->MineRange = 2;
    this->player = { 0 };
}

Player::Player(int PosX, int PosY) {
    this->PosX = PosX;
    this->PosY = PosY;
    this->HeightP = BLOCK_SIZE;
    this->WidthP = BLOCK_SIZE;
    this->colorP = RAYWHITE;
    this->IsInAir = true;
    this->MineRange = 2;
    this->player = { 0 };
}
