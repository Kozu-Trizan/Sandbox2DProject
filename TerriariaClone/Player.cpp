#include "Constant.h"
#include "Blocks.h"
#include "MapGen.h"
#include "Player.h"
#include "IsSolid.h"
#include "BresenhamAlgorithm.h"

void Player::Spawn() {
    // Player Spawn Logic
    int spawnX = UniverseWidth / 2;
    int spawnY = 0;
    while (spawnY < UniverseHeight && Universe[spawnY][spawnX].B_ID == 0) {
        spawnY++;
    }
    spawnY--; // Prevent Spawnning on a block

    this->PosX = spawnX * BLOCK_SIZE;
    this->PosY = spawnY * BLOCK_SIZE - this->HeightP;
}

void Player::UpdatePosX(int velocity) {
    this->PosX += velocity;
}

void Player::UpdatePosY(int velocity) {
    this->PosY += velocity;
}

void Player::Mine(Camera2D camera) {
    Vector2 PosMouse = GetMousePosition(); // MousePosition function returns coordinates in Screen Space.
    PosMouse = GetScreenToWorld2D(PosMouse, camera); // To convert the Screen space coordinates to world space coordinates that the logic is compatible with
    BlockPos PosMouseMap = { static_cast<int>(PosMouse.x / BLOCK_SIZE), static_cast<int>(PosMouse.y / BLOCK_SIZE) };
    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) { 

        if (Universe[PosMouseMap.y][PosMouseMap.x].B_ID != Air.B_ID && this->BlockInRange(PosMouseMap) && this->BlockIsVisible(PosMouseMap)) {
            //Universe[PosMouseMap[1]][PosMouseMap[0]].B_ID = Air.B_ID;
            this->WasMining = true;
            if (Universe[PosMouseMap.y][PosMouseMap.x].HP > 0) {
                Universe[PosMouseMap.y][PosMouseMap.x].HP -= this->Damage;
            }
            else {
                Universe[PosMouseMap.y][PosMouseMap.x].B_ID = Air.B_ID;
            }
        }

    }
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && this->WasMining == true) {
        Universe[PosMouseMap.y][PosMouseMap.x].HP = Universe[PosMouseMap.y][PosMouseMap.x].BaseHP;
    }
}

void Player::Jump(int JumpStep) {
    if (!this->IsInAir) {
        AboveBlock = (this->PosY-1)/BLOCK_SIZE;
        if((IsSolid(this->LeftBlock,this->AboveBlock)|| IsSolid(this->RightBlock,this->AboveBlock))==false)
          { this->VelocityY = -static_cast<float>(JumpStep);      
            this->IsInAir = true;
          }
    }
}
void Player::UpdateGravity() {
    this->VelocityY = this->VelocityY + 0.4f;
    LeftBlock = this->PosX/BLOCK_SIZE;
    RightBlock = (this->PosX+this->WidthP-1)/BLOCK_SIZE;
    if(this->VelocityY < 0)
    {
        AboveBlock = static_cast<int>((this->PosY-1)/BLOCK_SIZE);
        if((IsSolid(this->LeftBlock,this->AboveBlock)|| IsSolid(this->RightBlock,this->AboveBlock))==false)
        {
            UpdatePosY(this->VelocityY);
        }
        else{
            VelocityY = 0;
        }
    }
    else 
    {
        BelowBlock = static_cast<int>((this->PosY + this->HeightP)/BLOCK_SIZE);
        if((IsSolid(this->LeftBlock,this->BelowBlock)|| IsSolid(this->RightBlock,this->BelowBlock))==false)
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
        LeftBlock = this->PosX/BLOCK_SIZE; // locates the left part of the block
        RightBlock = (this->PosX + this->WidthP -1 )/BLOCK_SIZE;// locates the right part of the block
        BelowBlock = (this->PosY + this->HeightP)/BLOCK_SIZE;//checks the block below the object
        bool leftsolid = IsSolid(LeftBlock, BelowBlock);//checks if the block is solid from the left refrence
        bool rightsolid = IsSolid(RightBlock, BelowBlock);//checks if the block is solid from right refrence
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

bool Player::BlockInRange(BlockPos Pos) {
    bool InRangeHorizontal = std::abs(this->PosX / BLOCK_SIZE - Pos.x) <= this->MineRange;
    int FromHead = (this->PosY / BLOCK_SIZE - Pos.y);
    int FromFoot = (this->PosY / BLOCK_SIZE + (this->HeightP / BLOCK_SIZE - 1) - Pos.y); // Foot at player height - 1 block for head
    bool InRangeVertical = std::abs(FromHead) <= this->MineRange || std::abs(FromFoot) <= this->MineRange;
    return (InRangeHorizontal && InRangeVertical);
}

bool Player::BlockIsVisible(BlockPos Pos) {
    int playerBlockY = this->PosY / BLOCK_SIZE;
    int playerBlockX = this->PosX / BLOCK_SIZE;
    
    return CanReach(playerBlockX, playerBlockY, Pos.x, Pos.y);
}

Player::Player() {
    this->PosX = 0;
    this->PosY = 0;
    this->HeightP = BLOCK_SIZE * 3;
    this->WidthP = BLOCK_SIZE;
    this->colorP = RAYWHITE;
    this->IsInAir = true;
    this->MineRange = 2;
    this->player = { 0 };

    this->VelocityY = 0;
    this->OnGround = true;
    this->AboveBlock = 0;
    this->BelowBlock = 0;
    this->RightBlock = 0;
    this->LeftBlock = 0;

    this->Damage = 0.2f;
    this->HP = 100;
    this->WasMining = false;
}

