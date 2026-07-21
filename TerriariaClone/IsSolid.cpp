#include "IsSolid.h"
#include "MapGen.h"

bool IsSolid(int blockX, int blockY)
{
    return Universe[blockY][blockX].B_ID != Air.B_ID;
}