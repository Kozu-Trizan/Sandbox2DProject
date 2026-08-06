#include "Move.h"
#include "Player.h"
#include "IsSolid.h"
#include <cmath>

void Move(Player& player, int velocity){
    // Compute the top and bottom block rows the player's bounding box actually occupies
    int topRow    = static_cast<int>(floorf(player.PosY / BLOCK_SIZE));
    // Bottom row: subtract a small epsilon so a player exactly on a block boundary
    int bottomRow = static_cast<int>(floorf((player.PosY + player.HeightP - 0.01f) / BLOCK_SIZE));
    int rowCount  = bottomRow - topRow + 1;

    bool CanMove = true;

    if (velocity > 0){
        // Moving right: check the column just past the right edge after moving
        int blockX = static_cast<int>((player.PosX + player.WidthP - 1 + velocity) / BLOCK_SIZE);
        for (int i = 0; i < rowCount; i++){
            CanMove = CanMove && !IsSolid(blockX, topRow + i);
        }
        if (CanMove) {
            player.UpdatePosX(velocity);
        } else {
            // Snap to block boundary to prevent sub-pixel drift into the wall
            float snappedX = floorf((player.PosX + player.WidthP) / BLOCK_SIZE) * BLOCK_SIZE - player.WidthP;
            player.PosX = snappedX;
        }
    } else {
        // Moving left: check the column just past the left edge after moving
        int blockX = static_cast<int>(floorf((player.PosX + velocity) / BLOCK_SIZE));
        for (int i = 0; i < rowCount; i++) {
            CanMove = CanMove && !IsSolid(blockX, topRow + i);
        }
        if (CanMove) {
            player.UpdatePosX(velocity);
        } else {
            // Snap to block boundary
            float snappedX = ceilf(player.PosX / BLOCK_SIZE) * BLOCK_SIZE;
            player.PosX = snappedX;
        }
    }
}
