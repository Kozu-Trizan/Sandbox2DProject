#include "CouldMove.h"
#include "IsSolid.h"

void CouldMove(Player& player,int nextx, int nexty, int velocity)
{
    int blockY = player.gety() / BLOCK_SIZE;

    if (velocity > 0)
    {
        int blockX = (player.getx() + BLOCK_SIZE - 1 + velocity) / BLOCK_SIZE;

        if (!IsSolid(blockX, blockY))
            player.UpdatePosX(velocity);
    }
    else
    {
        int blockX = (player.getx() + velocity) / BLOCK_SIZE;

        if (!IsSolid(blockX, blockY))
            player.UpdatePosX(velocity);
    }
}