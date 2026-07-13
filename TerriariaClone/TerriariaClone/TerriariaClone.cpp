#include <iostream>
#include <raylib.h>
#include <vector>
#include<cmath>

const int ScreenWidth = 1024;//defines the x axis
const int ScreenHeight = 512;//defines the y axis
const int BLOCK_SIZE = 16;//defines the block size

Color SKY = { 135, 206, 235, 1 };


float WorldMap[ScreenHeight / BLOCK_SIZE][ScreenWidth / BLOCK_SIZE]; // World Map Array
struct Block {
    int B_ID = 0;
};
bool issolid(int blockx, int blocky);//functiom defination of issolid
class Player {
private:
    int PosX, PosY; //defines the x and y coordinate 
    int HeightP, WidthP;//defines the height and width of the player
    Color colorP = RAYWHITE;//color of the player
    bool IsInAir = true;//checks if the player is falling
    int MineRange = 2;
    int leftblock;
    int rightblock;

public:
    float vely;
    int getisinair(bool state)
    {
         IsInAir = state;//returns the state of the player if they are in air or not
         return IsInAir;
    }
    int getx()
    {
        return PosX; //acesses the private posx 
    }
    int gety()
    {
        return PosY;//acesses the private posy
    }
    void UpdatePosX(int velocity) {
        this->PosX += velocity; //updates the x axis 
    }

    void UpdatePosY(int velocity) {
        this->PosY += velocity;//updates the y axis
    }

   // void Jump(int JumpStep)
    void Jump() {
        if (!this->IsInAir) { //only jumps if the player in in the ground
            int aboveblock = (PosY - 1)/BLOCK_SIZE;
            if((issolid(leftblock, aboveblock) || issolid(rightblock,aboveblock))==false)
            {
                this->PosY -= 50; //moves up by 50 pixels
            this->IsInAir = true;//sets the player in air state to true so that the player can fall down after the jump
            }
        }
    }

    void gravity()
    {
        if(this->IsInAir == true)
        {
        vely += 0.5f;//gravity increases the velocity of the player by 0.5 every frame
        PosY += (int)vely;//updates the player y position by the velocity
        }
    }
    void DrawPlayer() {
        DrawRectangle(this->PosX, this->PosY, this->WidthP, this->HeightP, colorP);
    }
    
    bool PlayerCanFall() {
        //bool state = (WorldMap[PosY / BLOCK_SIZE + 1][PosX / BLOCK_SIZE] == 0); // Player Height = 1 * Block size so the + 1 in array index
        //works same logic code as above but it calls the isslid function to check 
        bool state;
        leftblock = PosX/BLOCK_SIZE; // locates the left part of the block
        rightblock = (PosX + WidthP -1 )/BLOCK_SIZE;// locates the right part of the block
        int blockbelow = (PosY + HeightP)/BLOCK_SIZE;//checks the block below the object
        bool leftsolid = issolid(leftblock, blockbelow);//checks if the block is solid from the left refrence
        bool rightsolid = issolid(rightblock, blockbelow);//checks if the block is solid from right refrence
        if ((leftsolid || rightsolid) == false)//checks if either side of the object is touching the block below
        {
             state = true;
        }
        else
        {
             state = false;
        }
        //bool state = !issolid( PosX/BLOCK_SIZE, PosY/BLOCK_SIZE + 1);
        //(state) ? this->IsInAir = true : this->IsInAir = false;
        IsInAir = state;
        return state;
    }

    bool BlockInRange(std::vector<int> Pos) {
        bool InRangeHorizontal = std::abs(this->PosX / BLOCK_SIZE - Pos[0]) <= this->MineRange;
        int Lower = std::min(this->PosY / BLOCK_SIZE, Pos[1]);
        int Upper = std::max(this->PosY / BLOCK_SIZE, Pos[1]);
        bool InRangeAbove = ((Upper - Lower) <= this->MineRange);
        //bool InRangeBelow = ((Pos[1] - this->PosY / BLOCK_SIZE - 1) <= this->MineRange) && ((Pos[1] - this->PosY / BLOCK_SIZE - 1) >= 0);
        return (InRangeHorizontal && InRangeAbove);
    }

    bool BlockIsVisible(std::vector<int> Pos) {
        int playerBlockY = this->PosY / BLOCK_SIZE;
        // Check from player down to target block, excluding both endpoints
        int startY = std::min(playerBlockY, Pos[1]);
        if (startY == Pos[1]) startY++; // When mining below check from starting(Player) Y level, when mining above check from one level excluding the leven containing the block to mine
        int endY = std::max(playerBlockY, Pos[1]);
        for (int i = startY; i < endY; i++){
            if (WorldMap[i][Pos[0]] != 0) {
                return false;
            }
        }
        return true;
    }

    Player(int PosX, int PosY) {
        this->PosX = PosX;
        this->PosY = PosY;
        this->HeightP = BLOCK_SIZE;
        this->WidthP = BLOCK_SIZE;
    }
};

/*void GenerateMap(float Map[][ScreenWidth / BLOCK_SIZE]) {
    Block Air = { 0 };
    Block Grass = { 2 };
    Block Dirt = { 1 };
    int RandNum(0);
    for (int y = 0; y < ScreenHeight / BLOCK_SIZE; y++){
        for (int x = 0; x < ScreenWidth / BLOCK_SIZE; x++){
            RandNum = static_cast<int>(10 * (x + 1) * std::sin(y) + 10 * (x + 1) * std::cos(y) + 10 * (x + 1) * std::sin(y)) % 3;
            switch (RandNum) {
            case 0:
                Map[y][x] = 0;
                break;
            case 1:
                Map[y][x] = 1;
                break;
            case 2:
                Map[y][x] = 2;
                break;
            }
        }
    }
}*/
//chat gpt code form here cause idk how to generate the map
void GenerateMap(float Map[][ScreenWidth / BLOCK_SIZE])
{
    int width = ScreenWidth / BLOCK_SIZE;
    int height = ScreenHeight / BLOCK_SIZE;

    int groundLevel = height / 2; // you can change this

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            if (y < groundLevel)
            {
                Map[y][x] = 0; // air
            }
            else if (y == groundLevel)
            {
                Map[y][x] = 2; // grass
            }
            else
            {
                Map[y][x] = 1; // dirt
            }
        }
    }
}

bool issolid (int blockx, int blocky)
{
    return WorldMap[blocky][blockx]!= 0;//checks each block if they are solid or air
}
void couldmove(Player &player, int a, int b, int c);//function defination of could move
int main() {
    Player player(0, 0);
    int velocity = 2;
    int JumpHeight = 12;
    InitWindow(ScreenWidth, ScreenHeight, "TerriariaProject");
    SetTargetFPS(60);

    GenerateMap(WorldMap);

    while (!WindowShouldClose()) {
        BeginDrawing();

        // Map Generation
        ClearBackground(SKY);

        for (int y = 0; y < ScreenHeight / BLOCK_SIZE; y++){
            for (int  x = 0; x < ScreenWidth / BLOCK_SIZE; x++){
                if (WorldMap[y][x] == 0) {
                    continue;
                }
                else if (WorldMap[y][x] == 1) {
                    Rectangle Block={x * BLOCK_SIZE, y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE};
                    DrawRectangleRec(Block, BROWN);
                }
                else if (WorldMap[y][x] == 2) {
                    Rectangle Block= {x * BLOCK_SIZE, y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE};
                    DrawRectangleRec(Block, GREEN);
                }
            }
        }
        // Player
        player.DrawPlayer();
        if (player.PlayerCanFall()) {
            player.gravity();
        }
        else {
            player.vely = 0;
        }

        //Horizontal Movement
       // if (IsKeyDown(KEY_RIGHT))
        if (IsKeyDown(KEY_D)) {//d key moves right insted of right arrow key
            int nextx = (player.getx() + velocity)/BLOCK_SIZE;//loactes the right block of the player 
            int nexty = player.gety()/BLOCK_SIZE;//locates the block below the player
            couldmove(player, nextx,nexty,velocity);// a funciton which checks the issolid function in a if case to which would update the player postion if the issolid allows the functiom to pass
        }
        //else if (IsKeyDown(KEY_LEFT)) 
        else if (IsKeyDown(KEY_A)) {//a key moves left insted of left arrow key
            int nextx = (player.getx() - velocity)/BLOCK_SIZE;
            int nexty = player.gety()/BLOCK_SIZE;
            couldmove(player, nextx,nexty,-velocity);
        }

        // Vertical Movement
        if (IsKeyPressed(KEY_SPACE)) {
            player.Jump();
        }
        
        // Breaking Blocks
        DrawText(TextFormat("X: %.2f, Y: %.2f", GetMousePosition().x, GetMousePosition().y), 0, 0, 20, RED);
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 PosMouse = GetMousePosition();
            // Double casting as we want just the integer part but Vector2 object expects float
            std::vector<int> PosMouseMap = { static_cast<int>(PosMouse.x / BLOCK_SIZE), static_cast<int>(PosMouse.y / BLOCK_SIZE) };

            if (WorldMap[PosMouseMap[1]][PosMouseMap[0]] != 0 && player.BlockInRange(PosMouseMap) && player.BlockIsVisible(PosMouseMap)) {
                WorldMap[PosMouseMap[1]][PosMouseMap[0]] = 0;
            }

        }

        EndDrawing();
    }
    CloseWindow();
    return 0;
    
}
void couldmove(Player &player,int a ,int b,int c)
{
    if(c>0)//checks if the player is moving left or right
    {
        int newposx = (player.getx() + BLOCK_SIZE - 1 )/BLOCK_SIZE;//initially top left point lai check garxa object ko which causes clipping so refrence point lai right ma shift gareko right gako bela
        if(issolid(newposx,b) == false)
        {
            player.UpdatePosX(c);
        }
    }
    else 
    {
         if(issolid(a,b) == false)//normal checks if the block on the left is solid or not
        {
            player.UpdatePosX(c);//updates the player postion of the block isnt solid
        }
    }

}
//nix-shell -p raylib gcc --run "g++ TerriariaClone.cpp -o game -lraylib -lm -ldl -lpthread -lGL && ./game"