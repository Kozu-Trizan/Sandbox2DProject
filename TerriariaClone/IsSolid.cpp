#include "IsSolid.h"

extern Block Universe[UniverseHeight][UniverseWidth];

bool IsSolid(int blockX, int blockY)
{
    return Universe[blockY][blockX].B_ID != Air.B_ID;
}