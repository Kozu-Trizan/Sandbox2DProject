#include "Constant.h"
#include "Blocks.h"
#include "MapGen.h"
#include "Player.h"
#include "IsSolid.h"
#include "ui.h"
#include "BresenhamAlgorithm.h"
#include "WavefrontPropagation.h"

void Player::ChangeHeldItem(int CellNo) {
    this->HeldItemCellNo = CellNo;
}

// --- Health System Implementation ---

float Player::GetHP() const {
    return HP;
}

float Player::GetMaxHP() const {
    return MaxHP;
}

void Player::TakeDamage(float amount) {
    HP -= amount;
    if (HP < 0.0f) {
        HP = 0.0f; // Prevents HP from going below 0
    }
}

void Player::Heal(float amount) {
    HP += amount;
    if (HP > MaxHP) {
        HP = MaxHP; // Prevents HP from exceeding max capacity
    }
}

void Player::Spawn() {
    // Player Spawn Logic
    float spawnX = UniverseWidth / 2;
    float spawnY = 0;
    while (spawnY < UniverseHeight && Universe[(int)spawnY][(int)spawnX].B_ID == 0) {
        spawnY++;
    }
    spawnY--; // Prevent Spawnning on a block

    this->PosX = spawnX * BLOCK_SIZE;
    this->PosY = spawnY * BLOCK_SIZE - this->HeightP;
    this->VelocityY = 0;
    this->IsInAir = false;
    this->FallHeight = this->PosY;
    this->FallDistance = 0;
    this->BlocksFallen = 0;
}

void Player::UpdatePosX(float velocity) {
    this->PosX += velocity;
}

void Player::UpdatePosY(float velocity) {
    this->PosY += velocity;
}

std::vector<int> Player::GetSize() {
    return { this->WidthP, this->HeightP };
}

bool Player::PlayerOccupiesBlock(BlockPos Pos) {
    // Use floorf to avoid float truncation causing sub-foot block to appear occupied
    float playerTop    = this->PosY;
    float playerBottom = this->PosY + this->HeightP;
    float playerLeft   = this->PosX;
    float playerRight  = this->PosX + this->WidthP;

    float blockLeft   = static_cast<float>(Pos.x) * BLOCK_SIZE;
    float blockRight  = blockLeft + BLOCK_SIZE;
    float blockTop    = static_cast<float>(Pos.y) * BLOCK_SIZE;
    float blockBottom = blockTop + BLOCK_SIZE;

    // Strict overlap: both intervals must overlap by more than a small epsilon
    const float eps = 0.5f;
    return (playerRight  - eps > blockLeft  &&
            playerLeft   + eps < blockRight &&
            playerBottom - eps > blockTop   &&
            playerTop    + eps < blockBottom);
}

void Player::Mine(Camera2D camera, BlockPos PosMouseMap, Block& MineBlock) {
    std::uint8_t WallID;
    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) { 

        if (MineBlock.B_ID != Air.B_ID && this->BlockInRange(PosMouseMap) && this->BlockIsVisible(PosMouseMap) && !this->PlayerOccupiesBlock(PosMouseMap)) {
            this->WasMining = true;
            if (MineBlock.HP > 0) {
                MineBlock.HP -= this->Damage;
            }
            else {
                this->inventory.AddItem(MineBlock);
                WallID = MineBlock.WallID;
                MineBlock = Air;
                MineBlock.WallID = WallID;
                UpdateLocalLight(PosMouseMap);
            }
        }

    }
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && this->WasMining) {
        Universe[PosMouseMap.y][PosMouseMap.x].HP = Universe[PosMouseMap.y][PosMouseMap.x].BaseHP;
        this->WasMining = false;
    }
}

void Player::PlaceBlock(Camera2D camera, BlockPos PosMouseMap, Block& SelectedBlock) {
    std::uint8_t WallID = SelectedBlock.WallID;
    if ((IsMouseButtonPressed(MOUSE_RIGHT_BUTTON) || IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) && this->inventory.cell[this->HeldItemCellNo - 1].ItemCount > 0) {

        if (SelectedBlock.B_ID == Air.B_ID && this->BlockInRange(PosMouseMap) && this->BlockIsVisible(PosMouseMap) && !SelectedBlock.SurroundedByAir(PosMouseMap) && !this->PlayerOccupiesBlock(PosMouseMap)) {
            SelectedBlock = dynamic_cast<Block&>(*this->inventory.cell[this->HeldItemCellNo - 1].item);
            SelectedBlock.RestoreHealth(); // At mining block HP = 0 so to restore health;
            SelectedBlock.WallID = WallID;
            this->inventory.cell[this->HeldItemCellNo - 1].ItemCount -= 1;
            if (this->inventory.cell[this->HeldItemCellNo - 1].ItemCount == 0) {
                delete this->inventory.cell[this->HeldItemCellNo - 1].item;
                this->inventory.cell[this->HeldItemCellNo - 1].item = nullptr;
            }
            UpdateLocalLight(PosMouseMap);
        }

    }
}

void Player::Jump(int JumpStep) {
    if (!this->IsInAir) {
        AboveBlock = static_cast<int>((this->PosY-1)/BLOCK_SIZE);
        if((IsSolid(this->LeftBlock,this->AboveBlock)|| IsSolid(this->RightBlock,this->AboveBlock))==false)
          { this->VelocityY = -static_cast<float>(JumpStep);      
            this->IsInAir = true;
          }
    }
}
void Player::UpdateGravity() {
    this->VelocityY = this->VelocityY + 0.4f;
    LeftBlock = static_cast<int>(this->PosX / BLOCK_SIZE);
    RightBlock = static_cast<int>((this->PosX + this->WidthP - 1) / BLOCK_SIZE);

    if (this->VelocityY < 0) // Moving UP (jumping)
    {
        float futureTop = this->PosY + this->VelocityY;
        AboveBlock = static_cast<int>(futureTop / BLOCK_SIZE);
        // Check if hitting ceiling - test both left and right edges
        bool leftClear = !IsSolid(this->LeftBlock, this->AboveBlock);
        bool rightClear = !IsSolid(this->RightBlock, this->AboveBlock);

        if (leftClear && rightClear)
        {
            if (IsInAir == 0)
            {
                this->FallHeight = this->PosY;
            }
            UpdatePosY(this->VelocityY);
        }
        else {
            VelocityY = 0;
            // Snap head to bottom of the ceiling block to eliminate float overshoot
            this->PosY = static_cast<float>((this->AboveBlock + 1) * BLOCK_SIZE);
        }
    }
    else // Moving DOWN (falling)
    {
        float futureBottom = this->PosY + this->HeightP + this->VelocityY;
        BelowBlock = static_cast<int>(futureBottom / BLOCK_SIZE);
        // Check if hitting ground - test both left and right edges
        bool leftClear = !IsSolid(this->LeftBlock, this->BelowBlock);
        bool rightClear = !IsSolid(this->RightBlock, this->BelowBlock);

        if (leftClear && rightClear)
        {
            UpdatePosY(this->VelocityY);
            IsInAir = true;
        }
        else {
            if (IsInAir == 1)
            {
                this->FallDistance = this->PosY - this->FallHeight;
                this->BlocksFallen = this->FallDistance / BLOCK_SIZE;
                if (this->BlocksFallen > 10)
                {
                    this->HP = this->HP - (this->BlocksFallen - 10) * 10;
                }
            }
            VelocityY = 0;
            this->IsInAir = false;
            // Snap feet to top of the ground block to eliminate float overshoot
            this->PosY = static_cast<float>(this->BelowBlock * BLOCK_SIZE - this->HeightP);
        }

        if (this->HP <= 0)
        {
            this->IsDead = true;
        }
    }
}

void Player::SetHP(float hp)
{
  this->HP = hp;
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
    
float Player::getx()
{
    return this->PosX;
}
float Player::gety()
{
    return this->PosY;
}


void Player::DrawPlayer()
{
    this->player = {
        (float)this->PosX,
        (float)this->PosY,
        (float)this->WidthP,
        (float)this->HeightP
    };

    const int FRAME_WIDTH = 48;
    const int FRAME_HEIGHT = 48;

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
        (float)this->PosX-16,
        (float)this->PosY -10 ,
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
        LeftBlock = static_cast<int>(this->PosX/BLOCK_SIZE); // locates the left part of the block
        RightBlock = static_cast<int>((this->PosX + this->WidthP - 1)/BLOCK_SIZE);// locates the right part of the block
        BelowBlock = static_cast<int>((this->PosY + this->HeightP)/BLOCK_SIZE);//checks the block below the object
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
    bool InRangeHorizontal = std::abs(static_cast<int>(this->PosX / BLOCK_SIZE) - Pos.x) <= this->MineRange;
    int FromHead = static_cast<int>(this->PosY / BLOCK_SIZE) - Pos.y;
    int FromFoot = static_cast<int>(this->PosY / BLOCK_SIZE + (this->HeightP / BLOCK_SIZE - 1)) - Pos.y; // Foot at player height - 1 block for head
    bool InRangeVertical = std::abs(FromHead) <= this->MineRange || std::abs(FromFoot) <= this->MineRange;
    return (InRangeHorizontal && InRangeVertical);
}

bool Player::BlockIsVisible(BlockPos Pos) {
    int playerBlockY = static_cast<int>(this->PosY / BLOCK_SIZE);
    int playerBlockX = static_cast<int>(this->PosX / BLOCK_SIZE);
    bool IsVisible = false;
    for (int i = 0; i < static_cast<int>(this->HeightP / BLOCK_SIZE); i++) {
        if (IsVisible) break;
        IsVisible = IsVisible || CanReach(playerBlockX, playerBlockY + i, Pos.x, Pos.y);
    }
    
    return IsVisible;
}

Player::Player() {
    this->PosX = 0.0f;
    this->PosY = 0.0f;
    this->HeightP = BLOCK_SIZE * 2;
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

    this->currentFrame = 0;
    this->animationTimer = 0.0f;
    this->frameDuration = 0.15f;
    this->animationRow = 64.0f;
    this->jumpAnimationRow = 64.0f;
    // Load texture from the 2D_character folder that you added to the project
    // Make sure the file (e.g., scarfy.png) is added to the project and copied to the output directory
    this->walkTexture = LoadTexture("assets/2D_character/The Female Adventurer - Free/The Female Adventurer - Free/Walk/walk.png");
    this->jumpTexture = LoadTexture("assets/2D_character/The Female Adventurer - Free/The Female Adventurer - Free/Jump - NEW/Normal/Jump.png");

    this->inventory = Inventory(10);
    this->HeldItemCellNo = 1;

    this->FallHeight = 0;
    this->FallDistance = 0;
    this->BlocksFallen = 0;
}



Player::~Player() {
    // Cleanup textures if they were loaded
    if (this->walkTexture.id != 0) UnloadTexture(this->walkTexture);
    if (this->jumpTexture.id != 0) UnloadTexture(this->jumpTexture);
}

