#include "Constant.h"
#include "Blocks.h"
#include "MapGen.h"
#include "Player.h"
#include "IsSolid.h"

void Player::UpdatePosX(int velocity) {
    this->PosX += velocity;
}

void Player::UpdatePosY(int velocity) {
    this->PosY += velocity;
}

void Player::Jump(int JumpStep) {
    if (!this->IsInAir) {
        aboveblock = (this->PosY-1)/BLOCK_SIZE;
        if((IsSolid(this->leftblock,this->aboveblock)|| IsSolid(this->rightblock,this->aboveblock))==false)
          { this->VelocityY = -JumpStep;      
            this->IsInAir = true;
          }
    }
}
void Player::UpdateGravity() {
    this->VelocityY = this->VelocityY + 0.4f;
    leftblock = this->PosX/BLOCK_SIZE;
    rightblock = (this->PosX+this->WidthP-1)/BLOCK_SIZE;
    if(this->VelocityY < 0)
    {
        aboveblock = (this->PosY-1)/BLOCK_SIZE;
        if((IsSolid(this->leftblock,this->aboveblock)|| IsSolid(this->rightblock,this->aboveblock))==false)
        {
            UpdatePosY(this->VelocityY);
        }
        else{
            VelocityY = 0;
        }
    }
    else 
    {
        belowblock = (this->PosY + this->HeightP)/BLOCK_SIZE;
        if((IsSolid(this->leftblock,this->belowblock)|| IsSolid(this->rightblock,this->belowblock))==false)
        {
            UpdatePosY(this->VelocityY);
            IsInAir = true;
        }
        else{
            VelocityY = 0;
            this->IsInAir = false;
        }
        }
    }
    
int Player::getx()
{
    return PosX;
}
int Player::gety()
{
    return PosY;
}

void Player::DrawPlayer() {
    this->player = { (float)this->PosX, (float)this->PosY, (float)this->WidthP, (float)this->HeightP };
    DrawRectangleRec(player, RAYWHITE);
}

Rectangle& Player::GetPlayer() {
    return this->player;
}

bool Player::PlayerCanFall() {
   // bool state = (Universe[PosY / BLOCK_SIZE + 1][PosX / BLOCK_SIZE].B_ID == Air.B_ID); // Player Height = 1 * Block size so the + 1 in array index
   // (state) ? this->IsInAir = true : this->IsInAir = false;
    //return state;
    bool state;
        leftblock = this->PosX/BLOCK_SIZE; // locates the left part of the block
        rightblock = (this->PosX + this->WidthP -1 )/BLOCK_SIZE;// locates the right part of the block
        belowblock = (this->PosY + this->HeightP)/BLOCK_SIZE;//checks the block below the object
        bool leftsolid = IsSolid(leftblock, belowblock);//checks if the block is solid from the left refrence
        bool rightsolid = IsSolid(rightblock, belowblock);//checks if the block is solid from right refrence
        if ((leftsolid || rightsolid) == false)//checks if either side of the object is touching the block below
        {
             state = true;
        }
        else
        {
             state = false;
        }
       IsInAir = state;
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
