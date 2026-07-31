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
        AboveBlock = (this->PosY-1)/BLOCK_SIZE;
        if((IsSolid(this->LeftBlock,this->AboveBlock)|| IsSolid(this->RightBlock,this->AboveBlock))==false)
          { this->VelocityY = -JumpStep;      
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
        AboveBlock = (this->PosY-1)/BLOCK_SIZE;
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
        BelowBlock = (this->PosY + this->HeightP)/BLOCK_SIZE;
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
void Player::UpdateWalkAnimation()
{
    animationTimer += GetFrameTime();

    if (animationTimer >= frameDuration)
    {
        animationTimer = 0.0f;

        currentFrame++;

        // There are 8 frames in one row
        if (currentFrame >= 8)
        {
            currentFrame = 0;
        }
    }
}
void Player::SetAnimationRow(float row)
{
    this->animationRow = row;
}
void Player::SetJumpAnimationRow(float row)
{
    this->jumpAnimationRow = row;
}
    
int Player::getx()
{
    return PosX;
}
int Player::gety()
{
    return PosY;
}

/*void Player::DrawPlayer() {
    this->player = { 
           (float)this->PosX, 
           (float)this->PosY, 
           (float)this->WidthP, 
           (float)this->HeightP 
    };
    //DrawRectangleRec(player, RAYWHITE);
    DrawTexture(
        this->walkTexture,
        this->PosX,
        this->PosY,
        WHITE
    );
}*/
void Player::DrawPlayer()
{
    this->player = {
        (float)this->PosX,
        (float)this->PosY,
        (float)this->WidthP,
        (float)this->HeightP
    };

    // One frame is 48 × 64 pixels
    /*Rectangle source = {
        0.0f,
        0.0f,
        48.0f,
        64.0f
    };*/

    // Draw the selected frame at the player's position
    /*Rectangle destination = {
        (float)this->PosX,
        (float)this->PosY,
        48.0f,
        64.0f
    };*/
    const int FRAME_WIDTH = 48;
    const int FRAME_HEIGHT = 64;

    float selectedRow;

    if (this->IsInAir)
    {
        selectedRow = this->jumpAnimationRow;
    }
    else
    {
        selectedRow = this->animationRow;
    }

    Rectangle source = {
        (float)(currentFrame * FRAME_WIDTH),
        animationRow,
        (float)FRAME_WIDTH,
        (float)FRAME_HEIGHT
    };

    Rectangle destination = {
        (float)this->PosX,
        (float)this->PosY,
        (float)FRAME_WIDTH,
        (float)FRAME_HEIGHT
    };
    Texture2D currentTexture;
    if (this->IsInAir)
    {
        currentTexture = this->jumpTexture;
    }
    else
    {
        currentTexture = this->walkTexture;
    }


    DrawTexturePro(
        currentTexture,
        source,
        destination,
        Vector2{ 0.0f, 0.0f },
        0.0f,
        WHITE
    );
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

    this->VelocityY = 0;
    this->OnGround = true;
    this->AboveBlock = 0;
    this->BelowBlock = 0;
    this->RightBlock = 0;
    this->LeftBlock = 0;
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

    this->VelocityY = 0;
    this->OnGround = true;
    this->AboveBlock = 0;
    this->BelowBlock = 0;
    this->RightBlock = 0;
    this->LeftBlock = 0;
    this->currentFrame = 0;
    this->animationTimer = 0.0f;
    this->frameDuration = 0.15f;
    this->animationRow = 64.0f;
    this->jumpAnimationRow = 64.0f;
    // Load texture from the 2D_character folder that you added to the project
    // Make sure the file (e.g., scarfy.png) is added to the project and copied to the output directory
    this->walkTexture = LoadTexture("C:/terrarria/Sandbox2DProject/2D_character/The Female Adventurer - Free/The Female Adventurer - Free/Walk/walk.png");
    this->jumpTexture = LoadTexture("C:/terrarria/Sandbox2DProject/2D_character/The Female Adventurer - Free/The Female Adventurer - Free/Jump - NEW/Normal/Jump.png");
}

Player::~Player() {
    // Unload texture when player is destroyed
    UnloadTexture(this->walkTexture);
    UnloadTexture(this->jumpTexture);
}
