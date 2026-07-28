#include "Move.h"
#include "Player.h"
#include "IsSolid.h"

void Move(Player& player, int velocity){
    int blockY = player.gety() / BLOCK_SIZE;
    int PlayerBlockHeight = player.HeightP / BLOCK_SIZE;
    bool CanMove = true;

    if (velocity > 0){
        int blockX = (player.getx() + BLOCK_SIZE - 1 + velocity) / BLOCK_SIZE;
        for (int i = 0; i < PlayerBlockHeight; i++){
            CanMove = CanMove && !IsSolid(blockX, blockY + i);
        }

        if (CanMove) player.UpdatePosX(velocity);
    } else {
        int blockX = (player.getx() + velocity) / BLOCK_SIZE;
        for (int i = 0; i < PlayerBlockHeight; i++) {
            CanMove = CanMove && !IsSolid(blockX, blockY + i);
        }
        if (CanMove) player.UpdatePosX(velocity);
    }
}